#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>

#include "coreclasses.h"
#include "cpp_parser.h"
#include "attributes.h"

#define EAT_SPACES(data)                                                                                               \
    while (*data == ' ')                                                                                               \
        data++;
#define EAT_SPACES_AND_NEW_LINES(data)                                                                                 \
    while (*data == ' ' || *data == '\n' || *data == '\r')                                                             \
        data++;
#define EAT_PAST_SEMICOLON(data)                                                                                       \
    while (*data != ';' && *data != '\0')                                                                              \
        data++;                                                                                                        \
    if (*data == ';')                                                                                                  \
        data++;

std::set<std::string> loaded_files;
std::stack<std::string> current_import;

namespace
{
    void trim_attribute_name(std::string& value)
    {
        value.erase(value.begin(),
                    std::find_if(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch) == 0; }));
        value.erase(
            std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch) == 0; }).base(),
            value.end());
    }

    void push_attribute(attributes& attribs, std::pair<std::string, std::string>& property)
    {
        trim_attribute_name(property.first);
        attribs.push_back(property);
        property = {};
    }
}

attributes get_attributes(const char*& pData)
{
    attributes attribs;
    if (*pData == '[')
    {
        pData++;

        EAT_SPACES(pData)

            ;
        bool bInAttribute = false;
        std::pair<std::string, std::string> property;
        bool inValue = false;
        int inBracket = 0;
        for (; *pData != 0; pData++)
        {

            if (!bInAttribute && *pData == ' ')
                continue;
            if (bInAttribute && !inBracket && *pData == ',')
            {
                push_attribute(attribs, property);
                bInAttribute = false;
                inValue = false;
            }
            else if (*pData == ']')
            {
                if (bInAttribute)
                    push_attribute(attribs, property);
                pData++;
                break;
            }
            else
            {
                if (*pData == '(')
                    inBracket++;
                else if (*pData == ')')
                    inBracket--;
                else if (*pData == '=' && !inValue && inBracket == 0)
                {
                    inValue = true;
                    bInAttribute = true;
                    continue;
                }
                bInAttribute = true;

                if (inValue)
                {
                    if (property.second.empty() && std::isspace(static_cast<unsigned char>(*pData)) != 0)
                        continue;

                    const char* pDataBeforeExtract = pData;
                    const char* pStart = nullptr;
                    const char* pSuffix = nullptr;
                    std::string extracted_string;

                    if (extract_multiline_string_literal(pData, pStart, pSuffix))
                    {
                        property.second.append(pStart, pSuffix - pStart);
                        pData--; // To compensate for loop increment
                        push_attribute(attribs, property);
                        bInAttribute = false;
                        inValue = false;
                    }
                    else if (*pData == '"')
                    {
                        if (extract_string_literal(pData, extracted_string))
                        {
                            auto attribute_name = property.first;
                            trim_attribute_name(attribute_name);
                            // Descriptions are prose, not source-code literals. Store the decoded text so
                            // single-line and multiline descriptions present the same value to generators.
                            if (attribute_name == attribute_types::description)
                                property.second = std::move(extracted_string);
                            else
                                property.second.append(pDataBeforeExtract, pData - pDataBeforeExtract);
                            pData--; // To compensate for loop increment
                            push_attribute(attribs, property);
                            bInAttribute = false;
                            inValue = false;
                        }
                        else
                        {
                            throw std::runtime_error("invalid string literal");
                        }
                    }
                    else
                    {
                        property.second += *pData;
                    }
                }
                else
                {
                    property.first += *pData;
                }
            }
        }
        EAT_SPACES(pData)
    }
    return attribs;
}

namespace
{
    int parse_error_integer_value(const std::string& value, const std::string& error_name,
                                  const std::string& member_name)
    {
        if (value.empty())
            throw std::runtime_error("error value " + error_name + "::" + member_name + " has an empty assignment");

        size_t parsed_chars = 0;
        long long parsed_value = 0;
        try
        {
            parsed_value = std::stoll(value, &parsed_chars, 0);
        }
        catch (const std::exception&)
        {
            throw std::runtime_error("error value " + error_name + "::" + member_name + " must be an integer literal");
        }

        if (parsed_chars != value.size())
            throw std::runtime_error("error value " + error_name + "::" + member_name + " must be an integer literal");

        if (parsed_value < 0)
            throw std::runtime_error("error value " + error_name + "::" + member_name + " must not be negative");

        if (parsed_value > std::numeric_limits<int>::max())
            throw std::runtime_error("error value " + error_name + "::" + member_name + " must fit in int");

        return static_cast<int>(parsed_value);
    }
}

function_entity class_entity::parse_function(const char*& pData, attributes& attribs, bool bFunctionIsInterface)
{
    function_entity func;
    func.swap(attribs);

    bool bFunctionIsProperty = true;

    EAT_SPACES(pData)

    std::string func_name;
    std::string return_type;

    while (*pData)
    {
        {
            int in_template = 0;
            while (in_template
                   || *pData != ' ' && *pData != '(' && *pData != ')' && *pData != ';' && *pData != '[' && *pData != 0)
            {
                assert(in_template >= 0);
                if (*pData == '<')
                    in_template++;
                else if (*pData == '>')
                    in_template--;
                func_name += *pData++;
            }
        }

        if (func_name == "static")
        {
            func.set_static(true);
            func_name = "";
            EAT_SPACES(pData);
            continue;
        }
        else if (func_name == "constexpr")
        {
            func.set_entity_type(entity_type::CONSTEXPR);
            func_name = "";
        }
        else if (func_name == "const" || (interface_spec_ == edl && func_name == "public"))
        {
            func.push_back(func_name);
            func_name = "";
            continue;
        }
        else if ((func_name == "struct" || func_name == "enum")
                 && std::find_if(attribs.begin(), attribs.end(), [](const std::pair<std::string, std::string>& attr)
                                 { return attr.first == attribute_types::tolerate_struct_or_enum; })
                        == attribs.end())
        {
            throw std::runtime_error(
                "struct and enum are not valid parameter names for function and member declarations");
        }

        EAT_SPACES(pData)

        if (*pData == '(')
        {
            if (func_name[0] == '&' || func_name[0] == '*')
            {
                return_type += func_name[0];
                func_name = &func_name.data()[1];
            }
            pData++;
            bFunctionIsProperty = false;
            break;
        }
        else if (*pData == '[')
        {
            auto count = 0;
            std::string buf;
            do
            {
                if (*pData == '[')
                    count++;
                else if (*pData == ']')
                    count--;
                else
                    buf += *pData;
                pData++;
            } while (count && *pData);

            func.set_array_string(buf);
        }
        else if (*pData == 0 || *pData == ';' || *pData == ')')
        {
            break;
        }
        else
        {
            if (*pData == '=')
                break;

            if (return_type.length())
                return_type += ' ';

            return_type += func_name;
            func_name = "";
        }
    }

    if (bFunctionIsInterface)
        ; // no processing
    else if (bFunctionIsProperty)
    {
        // Struct field default values are captured here: parse_function (not
        // parse_variable) handles the `=` suffix for FUNCTION_VARIABLE entries.
        // parse_variable is the simpler path that runs only when isFunction()
        // returns false AND there is no `=` to consume.
        if (func.get_entity_type() != entity_type::CONSTEXPR)
            func.set_entity_type(entity_type::FUNCTION_VARIABLE);
        if (*pData == '=') // this may be a default value
        {
            pData++;

            EAT_SPACES(pData)

            std::string default_value;
            while (*pData != ';' && *pData != 0)
                default_value += *pData++;
            func.set_default_value(default_value);
            if (*pData != ';')
                pData++;
        }
    }
    else
    {
        while (*pData != 0 && *pData != ')' && *pData != ';')
        {
            parameter_entity parameter;
            bool b_nullParam = false;

            EAT_SPACES(pData)

            auto attribs = get_attributes(pData);
            parameter.swap(attribs);

            std::string parameter_name;
            std::string parameter_type;

            while (*pData != 0)
            {
                // deal with call back functions
                if (*pData == '(')
                {
                    pData++;
                    parameter_type += '(';

                    std::string temp;
                    while (*pData != ')' && *pData != 0)
                    {
                        if (extract_word(pData, temp))
                            parameter_type += temp;
                        else
                            parameter_type += *pData++;
                    }
                    assert(*pData == ')');
                    if (*pData == ')')
                        pData++;

                    parameter_type += ')';

                    parameter_name = temp;

                    while (*pData != ')' && *pData != 0)
                        parameter_type += *pData++;
                    assert(*pData == ')');
                    if (*pData == ')')
                    {
                        pData++;
                        parameter_type += ')';
                        parameter.set_callback(true);
                    }
                    else
                        throw std::runtime_error("not a call back as expected");
                }
                else if (*pData == '[')
                {
                    auto count = 0;
                    std::string buf;
                    do
                    {
                        if (*pData == '[')
                            count++;
                        else if (*pData == ']')
                            count--;
                        else
                            buf += *pData;
                        pData++;
                    } while (count && *pData);
                    parameter.set_array_size(std::stoul(buf));
                }
                else
                {
                    int template_angle = 0;
                    while (*pData != 0 && ((*pData != '[' && *pData != '*' && *pData != ')') || template_angle))
                    {
                        if (*pData == '<')
                            template_angle++;
                        if (*pData == '>')
                            template_angle--;
                        if (template_angle == 0 && (*pData == ' ' || *pData == ','))
                            break;
                        parameter_name += *pData++;
                    }
                }
                EAT_SPACES(pData)
                while (*pData == '*' || *pData == '&')
                {
                    parameter_name += *pData++;
                    EAT_SPACES(pData)
                }
                while (*pData == '[')
                {
                    std::string suffix;
                    if (parameter_name.length())
                    {
                        while (*pData != ']' && *pData != '\0')
                        {
                            EAT_SPACES(pData)
                            suffix += *pData++;
                            EAT_SPACES(pData)
                        }
                        suffix += *pData++;
                        EAT_SPACES(pData)
                        parameter.add_array_suffix(suffix);
                    }
                    else
                    {
                        auto attribs = get_attributes(pData);
                        parameter.merge(attribs);
                    }
                }

                EAT_SPACES(pData)

                if (parameter_name == "const")
                {
                    parameter.push_back(parameter_name);
                    parameter_name = "";
                    continue;
                }
                if ((parameter_name == "struct" || parameter_name == "enum")
                    && std::find_if(attribs.begin(), attribs.end(), [](const std::pair<std::string, std::string>& attr)
                                    { return attr.first == attribute_types::tolerate_struct_or_enum; })
                           == attribs.end())
                {
                    throw std::runtime_error(
                        "struct and enum are not valid parameter names for function and member declarations");
                }

                // gsoap yuckyness
                // if (*pData == '0' || *pData == '1')
                // {
                //     pData++;
                //     EAT_SPACES(pData)
                // }

                if (*pData == ',' || *pData == ')')
                {
                    if (parameter_name == "void")
                    {
                        b_nullParam = true;
                        break;
                    }

                    if (parameter_name[0] == '&' || parameter_name[0] == '*')
                    {
                        parameter_type += parameter_name[0];
                        parameter_name = &parameter_name.data()[1];
                    }
                    break;
                }
                else
                {
                    if (parameter_type.length())
                        parameter_type += ' ';
                    parameter_type += parameter_name;
                    parameter_name = "";
                }
            }
            parameter.set_name(parameter_name);
            parameter.set_type(parameter_type);
            // eat comma
            if (*pData == ',')
                pData++;

            EAT_SPACES(pData)

            if (b_nullParam != true)
                func.add_parameter(parameter);
        }

        if (*pData == ')')
        {
            pData++;
        }
    }

    while (!bFunctionIsProperty && *pData != 0 && *pData != ';')
    {
        EAT_SPACES(pData)

        if (*pData == '{')
        {
            throw std::runtime_error("function implementations are not supported");
        }
        else if (func_name == get_name() && *pData == ':') // this is for constructor initialisers
        {
            pData++;
            while (1)
            {
                EAT_SPACES(pData)
                while (*pData++ != '(')
                    continue;
                while (*pData++ != ')')
                    continue;
                EAT_SPACES(pData)
                if (*pData != ',')
                    break;
            }
        }
        else if (!strcmp2(&*pData, "raises"))
        {
            pData += 6; // strlen "raises"

            EAT_SPACES(pData)

            if (*pData != '(')
                continue;
            pData++;

            EAT_SPACES(pData)

            std::string exception;
            while (*pData != ')' && *pData != 0)
            {
                if (*pData == ',')
                {
                    func.add_raises(exception);
                    exception.clear();
                }
                else
                    exception += *pData;

                pData++;

                EAT_SPACES(pData)
            }
            func.add_raises(exception);
            if (*pData == 0)
            {
                break;
            }
        }
        else if (*pData == '=')
        {
            pData++;
            EAT_SPACES(pData)
            if (*pData == '0')
            {
                func.set_pure_virtual(true);
                pData++;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    func.set_name(func_name);
    func.set_return_type(return_type);
    return func;
}

bool isFunction(const char* pData)
{
    while (*pData != '\0' && *pData != ';')
    {
        if (*pData == '(')
            return true;
        if (*pData == '=')
            return false;

        pData++;
    }
    return false;
}

void advancePassStatement(const char*& pData)
{
    while (*pData != '\0' && *pData != ';')
    {
        if (*pData == '\"')
        {
            pData++;
            while (*pData && *pData != '\"')
            {
                if (begins_with(pData, "\\\""))
                    pData++;
                pData++;
            }
        }

        pData++;
    }
    if (*pData == ';')
        pData++;
}

void splitVariable(const std::string& phrase, std::string& name, std::string& type)
{
    size_t j = phrase.length() - 1;
    while (phrase[j] == ' ' && j > 0)
    {
        j--;
    }
    size_t end_pos = j;
    for (; j > 0; j--)
    {
        char it = phrase[j];
        if (!((it >= '0' && it <= '9') || (it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z') || (it == '_')))
            break;
    }
    size_t start_pos = j;
    while (phrase[j] == ' ' && j > 0)
    {
        j--;
    }

    name = phrase.substr(start_pos + 1, end_pos + 1);
    type = phrase.substr(0, j + 1);
}

void splitTemplate(std::string phrase, std::string& name, std::string& type, std::string& default_value)
{
    phrase = trim_string(phrase);

    auto pos = phrase.find_first_of(" ");

    type = phrase.substr(0, pos);
    name = trim_string(phrase.substr(pos + 1));

    pos = name.find_first_of("=");
    if (pos != std::string::npos)
    {
        default_value = trim_string(name.substr(pos + 1));
        name = trim_string(name.substr(0, pos));
    }
}

void class_entity::parse_variable(const char*& pData, bool in_import)
{
    std::string phrase;
    for (; *pData != 0 && *pData != ';'; pData++)
    {
        phrase += *pData;
    }

    function_entity fn;
    std::string fn_name;
    std::string fn_return_type;
    splitVariable(phrase, fn_name, fn_return_type);
    // strip out yucky gsoap limiters
    if (fn_name == "0" || fn_name == "1")
    {
        splitVariable(fn_return_type, fn_name, fn_return_type);
    }
    fn.set_name(fn_name);
    fn.set_return_type(fn_return_type);
    fn.set_entity_type(entity_type::FUNCTION_VARIABLE);
    fn.set_is_in_import(in_import);
    add_function(fn);

    if (*pData == ';')
        pData++;
}

void class_entity::parse_namespace(const char*& pData, bool in_import)
{
    EAT_SPACES(pData)

    std::string nameSpace;
    extract_word(pData, nameSpace);

    EAT_SPACES(pData)

    if (*pData != '{')
    {
        throw std::runtime_error("Error expected character '}'");
    }
    parse_structure(pData, false, in_import);
}

std::shared_ptr<class_entity> class_entity::parse_interface(const char*& pData, const entity_type typ, attributes& attr,
                                                            bool in_import)
{
    auto cls = std::make_shared<class_entity>(this);

    cls->set_is_in_import(in_import);
    cls->set_entity_type(typ);
    cls->swap(attr);

    cls->parse_structure(pData, false, in_import);
    return cls;
}

std::string class_entity::parse_quote(const char*& pData, const char* macro_name)
{
    EAT_SPACES_AND_NEW_LINES(pData)
    if (*pData != '(')
        throw std::runtime_error(std::string("missing bracket after ") + macro_name);
    pData++;

    EAT_SPACES_AND_NEW_LINES(pData)

    std::string contents;

    const char* pStart = nullptr;
    const char* pSuffix = nullptr;
    if (extract_multiline_string_literal(pData, pStart, pSuffix))
    {
        contents = std::string(pStart, pSuffix);
    }
    else
    {
        if (!extract_string_literal(pData, contents))
            throw std::runtime_error(std::string("missing initial \" in ") + macro_name);
    }
    if (!*pData || *pData != ')')
        throw std::runtime_error(std::string("invalid ending in ") + macro_name + " (no bracket)");
    pData++;
    return contents;
}

void class_entity::parse_structure(const char*& pData, bool bInCurlyBrackets, bool in_import)
{
    bool bHasName = false;
    while (*pData != 0)
    {
        EAT_SPACES(pData)

        if (*pData == 0)
        {
            break;
        }

        if (bInCurlyBrackets)
        {
            if (*pData == '}')
            {
                pData++;

                break;
            }
            else
            {
                EAT_SPACES(pData)

                auto attribs = get_attributes(pData);
                if (parse_include(pData, NULL, in_import))
                {
                    continue;
                }
                else if (interface_spec_ == edl
                         && (is_word(pData, "enclave") || is_word(pData, "trusted") || is_word(pData, "untrusted")))
                {
                    parse_namespace(pData, in_import);
                    EAT_SPACES(pData)
                    if (*pData == ';')
                        pData++;
                    continue;
                }
                else
                {
                    auto obj = std::make_shared<class_entity>(this);
                    if (parse_class(pData, attribs, obj, true, in_import))
                    {
                        if (*pData == ';')
                            pData++;
                    }
                    else if (get_entity_type() == entity_type::ENUM || get_entity_type() == entity_type::ERROR)
                    {
                        const bool is_error_declaration = get_entity_type() == entity_type::ERROR;
                        long long next_error_value = 1;
                        size_t error_value_index = 0;
                        std::map<int, std::string> error_values_seen;
                        attributes member_attribs;
                        member_attribs.swap(attribs);

                        std::string elemname;
                        while (true)
                        {
                            member_attribs.merge(get_attributes(pData));
                            if (!extract_word(pData, elemname))
                                break;

                            EAT_SPACES(pData);
                            std::string elemValue;
                            bool has_explicit_value = false;
                            if (*pData == '=')
                            {
                                pData++;
                                has_explicit_value = true;

                                EAT_SPACES(pData);

                                if (*pData != ',' && *pData != '}' && *pData != '\0')
                                {
                                    while (*pData != 0 && *pData != ' ' && *pData != '}' && *pData != ','
                                           && *pData != '{' && *pData != ';' && *pData != ':')
                                        elemValue += *pData++;

                                    EAT_SPACES(pData);
                                }
                            }

                            if (is_error_declaration)
                            {
                                int numeric_value = 0;
                                if (elemname == "OK")
                                {
                                    if (error_value_index != 0)
                                        throw std::runtime_error("OK is reserved and must be the first value in error "
                                                                 + get_name());

                                    numeric_value = has_explicit_value
                                                        ? parse_error_integer_value(elemValue, get_name(), elemname)
                                                        : 0;
                                    if (numeric_value != 0)
                                        throw std::runtime_error("OK must be 0 in error " + get_name());
                                    elemValue = "0";
                                    next_error_value = 1;
                                }
                                else
                                {
                                    if (!has_explicit_value && next_error_value > std::numeric_limits<int>::max())
                                        throw std::runtime_error("implicit error value " + get_name() + "::" + elemname
                                                                 + " must fit in int");
                                    numeric_value = has_explicit_value
                                                        ? parse_error_integer_value(elemValue, get_name(), elemname)
                                                        : static_cast<int>(next_error_value);
                                    if (numeric_value == 0)
                                        throw std::runtime_error("only OK may use value 0 in error " + get_name());
                                    elemValue = std::to_string(numeric_value);
                                    next_error_value = numeric_value + 1;
                                }

                                const auto existing = error_values_seen.find(numeric_value);
                                if (existing != error_values_seen.end())
                                {
                                    std::cerr << "warning: duplicate error value " << numeric_value << " in error "
                                              << get_name() << " for " << existing->second << " and " << elemname
                                              << '\n';
                                }
                                else
                                {
                                    error_values_seen.emplace(numeric_value, elemname);
                                }
                                ++error_value_index;
                            }

                            function_entity fn;
                            fn.swap(member_attribs);
                            fn.set_name(elemname);
                            fn.set_return_type(elemValue);
                            fn.set_has_explicit_value(has_explicit_value);
                            fn.set_is_in_import(in_import);
                            add_function(fn);

                            elemname = "";
                            member_attribs = {};

                            if (*pData == ',')
                                pData++;
                            else if (*pData == ';')
                                throw std::runtime_error(
                                    "enum values must be separated by ',' not ';' — use ',' between "
                                    "enum values and omit the trailing separator before '}'");

                            EAT_SPACES(pData);
                        }
                    }
                    else if (get_entity_type() == entity_type::STRUCT)
                    {
                        if (if_is_word_eat(pData, "public:"))
                        {
                            function_entity func;
                            func.set_name("public:");
                            func.set_entity_type(entity_type::FUNCTION_PUBLIC);
                            func.set_is_in_import(in_import);
                            add_function(func);
                        }
                        else if (if_is_word_eat(pData, "private:"))
                        {
                            function_entity func;
                            func.set_name("private:");
                            func.set_entity_type(entity_type::FUNCTION_PRIVATE);
                            func.set_is_in_import(in_import);
                            add_function(func);
                        }
                        else if (if_is_word_eat(pData, "#cpp_quote"))
                        {
                            function_entity func;
                            func.set_name(parse_quote(pData, "#cpp_quote"));
                            func.set_is_in_import(in_import);
                            func.set_entity_type(entity_type::CPPQUOTE);
                            add_function(func);
                        }
                        else if (if_is_word_eat(pData, "#rust_quote"))
                        {
                            function_entity func;
                            func.set_name(parse_quote(pData, "#rust_quote"));
                            func.set_is_in_import(in_import);
                            func.set_entity_type(entity_type::RUSTQUOTE);
                            add_function(func);
                        }
                        else
                        {
                            function_entity func(parse_function(pData, attribs, false));
                            EAT_SPACES(pData);
                            assert(*pData == ';');
                            if (*pData == ';')
                                pData++;
                            func.set_is_in_import(in_import);
                            add_function(func);
                        }
                    }
                    else if (if_is_word_eat(pData, "public:"))
                    {
                        function_entity func;
                        func.set_name("public:");
                        func.set_entity_type(entity_type::FUNCTION_PUBLIC);
                        func.set_is_in_import(in_import);
                        add_function(func);
                    }
                    else if (if_is_word_eat(pData, "private:"))
                    {
                        function_entity func;
                        func.set_name("private:");
                        func.set_entity_type(entity_type::FUNCTION_PRIVATE);
                        func.set_is_in_import(in_import);
                        add_function(func);
                    }
                    else if (if_is_word_eat(pData, "#cpp_quote"))
                    {
                        function_entity func;
                        func.set_name(parse_quote(pData, "#cpp_quote"));
                        func.set_is_in_import(in_import);
                        func.set_entity_type(entity_type::CPPQUOTE);
                        add_function(func);
                    }
                    else if (if_is_word_eat(pData, "#rust_quote"))
                    {
                        function_entity func;
                        func.set_name(parse_quote(pData, "#rust_quote"));
                        func.set_is_in_import(in_import);
                        func.set_entity_type(entity_type::RUSTQUOTE);
                        add_function(func);
                    }
                    else if (if_is_word_eat(pData, "constexpr"))
                    {
                        function_entity fn(parse_function(pData, attribs, false));
                        fn.set_entity_type(entity_type::CONSTEXPR);
                        fn.set_is_in_import(in_import);
                        add_function(fn);

                        if (*pData == ';')
                            pData++;
                    }
                    else if (isFunction(pData))
                    {
                        function_entity func(parse_function(pData, attribs, false));
                        EAT_SPACES(pData);
                        assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                        func.set_is_in_import(in_import);
                        add_function(func);
                    }
                    else
                    {
                        parse_variable(pData, in_import);
                    }
                }
            }
        }
        else
        {
            // get name
            if (!bHasName)
            {
                std::string name;
                while (*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
                    name += *pData++;
                set_name(name);
                bHasName = true;

                EAT_SPACES(pData)
            }

            // get the parent name
            if (*pData == ':')
            {
                if (get_entity_type() == entity_type::ERROR)
                    throw std::runtime_error("error declarations do not support explicit base types");

                pData++;

                EAT_SPACES(pData)

                std::string parent_name;
                while (*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
                {
                    parent_name += *pData;
                    pData++;
                }

                EAT_SPACES(pData)

                if (parent_name == "public" || parent_name == "protected" || parent_name == "private")
                {
                    parent_name = "";
                    while (*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
                    {
                        parent_name += *pData;
                        pData++;
                    }
                }

                EAT_SPACES(pData)

                std::shared_ptr<class_entity> pObj;
                if (!find_class(parent_name, pObj))
                {
                    if (get_entity_type() == entity_type::ENUM)
                    {
                        auto last_owner = get_owner();
                        auto owner = get_owner();
                        do
                        {
                            owner = owner->get_owner();
                            if (owner)
                                last_owner = owner;
                        } while (owner);

                        pObj = std::make_shared<class_entity>(last_owner);
                        pObj->set_entity_type(entity_type::TYPE_NULL);
                        pObj->set_name(parent_name);
                        last_owner->add_class(pObj);
                    }
                    else
                    {
                        std::stringstream err;
                        err << "type " << parent_name << " not known";
                        err << std::ends;
                        std::string errString(err.str());
                        throw std::runtime_error(errString);
                    }
                }
                add_base_class(pObj.get());
            }

            if (*pData == '{')
            {
                bInCurlyBrackets = true;
                pData++;

                EAT_SPACES(pData)
            }
            else
                pData++;
        }
        EAT_SPACES(pData)
    }
}

std::shared_ptr<class_entity> class_entity::parse_typedef(const char*& pData, attributes& attribs, const char* type,
                                                          bool in_import)
{
    auto object = std::make_shared<class_entity>(this);

    object->set_entity_type(entity_type::TYPEDEF);

    auto tmp = get_attributes(pData);
    attribs.merge(tmp);

    auto obj = std::make_shared<class_entity>(object.get());

    if (type == NULL && object->parse_class(pData, attribs, obj, false, in_import))
    {
        object->set_alias_name(obj->get_name());

        bool firstPass = true;
        do
        {
            std::shared_ptr<class_entity> source(object);
            if (object->get_owner() == nullptr)
            {
                source = obj;
            }
            auto temp = std::make_shared<class_entity>(source.get());

            // loop around to extract the names
            EAT_SPACES(pData)

            if (!firstPass)
            {
                pData++; // strip out the comma
                EAT_SPACES(pData)
            }

            bool ispointer = *pData == '*';
            if (ispointer)
            {
                pData++;
                EAT_SPACES(pData)
            }

            std::string name;
            while (*pData != 0 && *pData != ';' && *pData != '[' && *pData != ',' && *pData != '{')
            {
                name += *pData;
                pData++;
            }
            temp->set_name(name);
            if (ispointer)
                temp->push_back(std::string("pointer"));

            if (firstPass)
            {
                firstPass = false;
                object->set_name(temp->get_name());
            }

            add_class(temp);

            EAT_SPACES(pData)
        } while (*pData == ',');
    }
    else
    {
        std::string parent_name;

        std::string object_name = object->get_name();
        if (type != NULL)
            parent_name = type;
        while (*pData != 0 && *pData != ';')
        {
            if (parent_name.length())
                parent_name += ' ';

            parent_name += object_name;
            object_name = "";

            EAT_SPACES(pData)

            int template_count = 0;
            while (*pData != 0 && (*pData != ' ' || template_count > 0) && *pData != '*' && *pData != ';'
                   && *pData != '{' && *pData != '['
                   && (*pData != ',' || template_count > 0))
            {
                if (*pData == '<')
                    template_count++;
                else if (*pData == '>')
                    template_count--;
                object_name += *pData;
                pData++;
            }
            if (*pData != 0 && *pData == '*')
            {
                object_name += *pData;
                pData++;
            }
            // check for multiple definitions
            if (*pData == ',')
            {
                pData++;
                EAT_SPACES(pData)
                std::string& name = parent_name;
                std::string type;
                for (size_t i = 0; i < name.length(); i++)
                {
                    if (name[i] != '&' && name[i] != '*' && name[i] != '[')
                        type += name[i];
                    else
                        break;
                }
                parse_typedef(pData, attribs, type.data(), in_import);
            }

            // removing any nasty member structures, perhaps I'll do some thing intellegent with it one day...
            if (*pData == '{')
            {
                pData++;
                int braketCount = 1;
                while (braketCount)
                {
                    if (*pData == '\0')
                        break;

                    if (*pData == '{')
                        braketCount++;

                    if (*pData == '}')
                        braketCount--;
                    pData++;
                }
            }

            // deal with arrays
            if (*pData == '[')
            {
                parent_name += *pData;
                pData++;
                while (*pData != ']')
                {
                    if (*pData == '\0')
                        break;

                    parent_name += *pData;
                    pData++;
                }
                if (*pData)
                {
                    parent_name += *pData;
                    pData++;
                }
            }
        }
        object->set_name(object_name);
        object->set_alias_name(parent_name);
        add_class(object);
        assert(*pData == ';');
    }

    return object;
}

void class_entity::parse_union(const char*& pData, attributes& attribs)
{
    while (*pData && *pData != ';')
    {
        if (*pData == '{')
        {
            pData++;
            int braketCount = 1;
            while (braketCount)
            {
                if (*pData == '\0')
                    break;

                if (*pData == '{')
                    braketCount++;

                if (*pData == '}')
                {
                    braketCount--;
                    if (!braketCount)
                    {
                        pData++;
                        return;
                    }
                }
                pData++;
            }
        }
        pData++;
    }
    while (*pData && *pData != ';')
        pData++;
}

bool class_entity::has_typedefs(const char* pData)
{
    int braketCount = -1;
    while (*pData && *pData != ';')
    {
        if (*pData == '{')
        {
            pData++;
            braketCount = 1;
            while (braketCount)
            {
                if (*pData == '\0')
                    break;

                if (*pData == '{')
                    braketCount++;

                if (*pData == '}')
                {
                    braketCount--;
                    if (!braketCount)
                    {
                        pData++;
                        break;
                    }
                }
                pData++;
            }
        }
        if (!braketCount)
            break;
        pData++;
    }
    EAT_SPACES(pData)

    if (*pData == ';' || *pData == '[' || *pData == '\0' || is_word(pData, "struct") || is_word(pData, "interface")
        || is_word(pData, "class") || is_word(pData, "namespace") || is_word(pData, "exception")
        || is_word(pData, "enum") || is_word(pData, "error") || is_word(pData, "union") || is_word(pData, "typedef")
        || is_word(pData, "#include") || is_word(pData, "import"))
        return false;

    return true;
}

void class_entity::parse_template(const char*& pData, std::list<template_declaration>& templateParams)
{
    EAT_SPACES(pData)

    if (*pData != '<')
    {
        std::stringstream err;
        err << "Error expected character '<'";
        err << std::ends;
        std::string errString(err.str());
        throw std::runtime_error(errString);
    }
    pData++;

    std::string phrase;
    for (; *pData != 0 && *pData != '>'; pData++)
    {
        if (*pData == ',')
        {
            template_declaration tpl;
            std::string name;
            splitTemplate(phrase, name, tpl.type, tpl.default_value);
            tpl.set_name(name);
            templateParams.push_back(tpl);
            phrase.clear();
        }
        else
        {
            if (phrase.empty())
                EAT_SPACES(pData);
            phrase += *pData;
        }
    }

    template_declaration tpl;
    if (!phrase.empty())
    {
        std::string name;
        splitTemplate(phrase, name, tpl.type, tpl.default_value);
        tpl.set_name(name);
        templateParams.push_back(tpl);
    }

    pData++;
}

bool class_entity::parse_class(const char*& pData, attributes& attribs, std::shared_ptr<class_entity>& obj,
                               bool handleTypeDefs, bool in_import)
{
    bool bUseTypeDef = false;

    bool is_variable = false;

    if (is_word(pData, "struct") || is_word(pData, "interface") || is_word(pData, "class") || is_word(pData, "template")
        || is_word(pData, "exception") || is_word(pData, "enum") || is_word(pData, "error") || is_word(pData, "union"))
    {
        // continue if this is only a forward declarantion
        const char* curlyPos = strchr(&*pData, '{');
        const char* semicolonPos = strchr(&*pData, ';');
        if (curlyPos == NULL || curlyPos > semicolonPos)
        {
            is_variable = true;
        }

        if (handleTypeDefs && has_typedefs(pData))
            bUseTypeDef = true;
    }

    if (bUseTypeDef || if_is_word_eat(pData, "typedef"))
        obj = parse_typedef(pData, attribs, NULL, in_import);

    else if (is_variable == false && if_is_word_eat(pData, "struct"))
    {
        obj = parse_interface(pData, entity_type::STRUCT, attribs, in_import);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "namespace"))
    {
        obj = parse_interface(pData, entity_type::NAMESPACE, attribs, in_import);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "import"))
    {
        obj = parse_interface(pData, entity_type::NAMESPACE, attribs, in_import);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "enum"))
    {
        obj = parse_interface(pData, entity_type::ENUM, attribs, in_import);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "error"))
    {
        obj = parse_interface(pData, entity_type::ERROR, attribs, in_import);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "interface"))
    {
        obj = parse_interface(pData, entity_type::INTERFACE, attribs, in_import);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "class"))
    {
        obj = parse_interface(pData, entity_type::CLASS, attribs, in_import);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "template"))
    {
        EAT_SPACES(pData)

        std::list<template_declaration> templateParams;
        parse_template(pData, templateParams);

        EAT_SPACES(pData)

        entity_type type = entity_type::TYPE_NULL;
        if (if_is_word_eat(pData, "class"))
            type = entity_type::CLASS;
        if (if_is_word_eat(pData, "struct"))
            type = entity_type::STRUCT;

        if (type != entity_type::TYPE_NULL)
        {
            EAT_SPACES(pData)
            obj = parse_interface(pData, type, attribs, in_import);
            obj->template_params_ = templateParams;
            obj->set_is_template(true);
            add_class(obj);
        }
        else
        {
            std::stringstream err;
            err << "Error expected 'class'";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
    }
    else
        return false;

    EAT_SPACES(pData)
    return true;
}

void class_entity::extract_path_and_load(const char*& pData, const char* file, bool in_import)
{
    EAT_SPACES(pData)

    assert(*pData == '\"' || *pData == '<');

    pData++;

    std::array<char, 1024> path;
    std::string temp;
    while (*pData)
    {
        if (*pData == '\"' || *pData == '>')
        {
            pData++;

            const char* fname_ext = std::max(strrchr(temp.data(), '\\'), strrchr(temp.data(), '/'));
            if (fname_ext != NULL)
            {
                if (!load(temp.data(), in_import))
                    throw std::runtime_error(std::string("failed to load ") + temp);
            }
            else
            {
                const char* fname_ext = std::max(strrchr(file, '\\'), strrchr(file, '/'));
                if (fname_ext != NULL)
                {
                    auto last_pos = std::copy(file, fname_ext + 1, path.begin());
                    std::copy(temp.begin(), temp.end(), last_pos);
                }
                else
                {
                    std::copy(temp.begin(), temp.end(), path.begin());
                }
                if (!load(path.data(), in_import))
                {
                    throw std::runtime_error(std::string("failed to load ") + std::string(path.data()));
                }
            }
            return;
        }

        temp += *pData++;
    }
}
void move_past_comments(const char*& pData)
{
    int count = 0;
    while (*pData)
    {
        if (*pData == '\"')
        {
            count++;
            pData++;
            if (count == 2)
                break;
        }
        else
            pData++;
    }
}

bool class_entity::parse_include(const char*& pData, const char* file, bool in_import)
{
    if (if_is_word_eat(pData, "#include"))
    {
        if (recurseImportLib)
            extract_path_and_load(pData, file, in_import);
        else
            move_past_comments(pData);
        return true;
    }
    if (if_is_word_eat(pData, "import"))
    {
        EAT_SPACES(pData);
        if (*pData != '\"')
        {
            std::stringstream err;
            err << "import path not supplied";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        pData++;

        std::string path;

        while (*pData != '\"' && *pData != 0)
        {
            path += *pData;
            pData++;
        }

        if (*pData != '\"')
        {
            std::stringstream err;
            err << "import path not supplied";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        pData++;
        current_import.push(path);

        EAT_SPACES(pData);
        if (*pData != '{')
        {
            std::stringstream err;
            err << "import { missing";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        pData++;
        parse_structure(pData, true, true);

        current_import.pop();
        return true;
    }
    return false;
}

bool class_entity::load(const char* file, bool in_import)
{
    if (loaded_files.find(file) != loaded_files.end())
        return true;

    loaded_files.insert(file);
    std::error_code ec;
    std::string path(std::filesystem::canonical(file, ec).string());

    std::ifstream preproc_stream(path);
    if (preproc_stream.is_open() == false)
    {
        return 0;
    }
    std::string preproc_data;
    std::getline(preproc_stream, preproc_data, '\0');

    const char* tmp = preproc_data.data();
    parse_structure(tmp, true, in_import);

    return true;
}
