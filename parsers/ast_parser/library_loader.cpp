#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <stack>

#include "coreclasses.h"

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

attributes GetAttributes(const char*& pData)
{
    attributes attribs;
    if (*pData == '[')
    {
        pData++;

        EAT_SPACES(pData)

            ;
        bool bInAttribute = false;
        std::string property;
        int inBracket = 0;
        for (; *pData != 0; pData++)
        {

            if (!bInAttribute && *pData == ' ')
                continue;
            if (bInAttribute && !inBracket && *pData == ',')
            {
                attribs.push_back(property);
                property = "";
                bInAttribute = false;
            }
            else if (*pData == ']')
            {
                if (bInAttribute)
                    attribs.push_back(property);
                pData++;
                break;
            }
            else
            {
                if (*pData == '(')
                    inBracket++;
                else if (*pData == ')')
                    inBracket--;
                bInAttribute = true;
                property += *pData;
            }
        }
        EAT_SPACES(pData)
    }
    return attribs;
}

function_entity class_entity::parse_function(const char*& pData, attributes& attribs, bool bFunctionIsInterface)
{
    function_entity func;
    func.set_attributes(attribs);

    bool bFunctionIsProperty = true;
    bool bConstant = false;

    EAT_SPACES(pData)

    std::string func_name;
    std::string return_type;
    int array_size = 0;

    while (*pData)
    {
	    {
	        int in_template = 0;
    	    while (in_template || *pData != ' ' && *pData != '(' && *pData != ')' && *pData != ';' && *pData != '[' && *pData != 0)
            {
                assert(in_template >= 0);
                if (*pData == '<')
                    in_template++;
                else if (*pData == '>')
                    in_template--;
                func_name += *pData++;
            }
        }

        if (func_name == "const") // CORBA types
            bConstant = true;

        if (func_name == "const" || func_name == "unsigned" || func_name == "signed"
            || (interface_spec_ == corba && func_name == "inout") || // CORBA types
            ((interface_spec_ == corba || interface_spec_ == com) && (func_name == "in" || func_name == "out"))
            || (interface_spec_ == edl && func_name == "public"))
        {
            if (func_name == "inout")
            {
                func.add_attribute("in");
                func.add_attribute("out");
            }
            else
                func.add_attribute(func_name);
            func_name = "";
            continue;
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
            if(*pData == '=')
                break;

            if (return_type.length())
                return_type += ' ';

            /*			const class_entity* obj;
                                    if(find_class(func_name, *pObjects, obj))
                                    {
                                            if(obj->type == TYPEDEF)
                                            {
                                                    func_name = obj->parentName;
                                                    break;
                                            }
                                    }*/
            return_type += func_name;
            func_name = "";
        }
    }

    if (bFunctionIsInterface)
        ; // no processing
    else if (bFunctionIsProperty)
    {
        /*		if(bConstant == false)
                                func.set_type(FunctionTypePropertyPut);
                //		func.m_bIsPutProperty = true;
                //	func.m_bIsGetProperty = true;*/
        func.set_type(FunctionTypeVariable);
        if(*pData == '=') //this may be a default value
        {
            pData++;

            EAT_SPACES(pData)
            
            std::string default_value;
            while (*pData != ';' && *pData != 0)
                default_value += *pData++;            
            func.set_default_value(default_value);
            if(*pData != ';')
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

            parameter.set_attributes(GetAttributes(pData));

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
                    while (*pData != 0 && ((*pData != '[' && *pData != '*'
                           && *pData != ')') || template_angle))
                    {
                        if(*pData == '<')
                            template_angle++;
                        if(*pData == '>')
                            template_angle--;
                        if(template_angle == 0 && (*pData == ' ' || *pData == ','))
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
                        parameter.merge_attributes(GetAttributes(pData));
                }

                EAT_SPACES(pData)

                if (parameter_name == "const" || parameter_name == "unsigned" || parameter_name == "signed"
                    || (interface_spec_ == corba && parameter_name == "inout")
                    || ((interface_spec_ == corba || interface_spec_ == com)
                        && (parameter_name == "in" || parameter_name == "out"))) // CORBA types
                {
                    if (parameter_name == "inout")
                    {
                        parameter.add_attribute("in");
                        parameter.add_attribute("out");
                    }
                    else
                        parameter.add_attribute(parameter_name);
                    parameter_name = "";
                    continue;
                }

                // gsoap yuckyness
                if (*pData == '0' || *pData == '1')
                {
                    pData++;
                    EAT_SPACES(pData)
                }

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

                    /*					const class_entity* obj;
                                                            if(GetLibrary().find_class(parameter_name, obj))
                                                                    if(obj->type == TYPEDEF)
                                                                            parameter_name = obj->parentName;

                    //#############this code was commented out doing so will break the com/corba bridge
                                                            for(std::list<class_entity*, ClassObjectAllocator>::iterator
                    it = pObjects->m_classes->begin();it != pObjects->m_classes->end();it++)
                                                            {
                                                                    if((*it).name == parameter_name)
                                                                    {
                                                                            if((*it).type == TYPEDEF)
                                                                            {
                                                                                    parameter_name = (*it).parentName;
                                                                                    break;
                                                                            }
                                                                    }
                                                            }*/

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
        //		pData++;
    }

    //	pData++;
    if (func.has_value("propput") || func.has_value("propputref"))
    {
        //		func.m_bIsPutProperty = true;
        func.set_type(FunctionTypePropertyPut);
    }
    else if (func.has_value("propget"))
    {
        //		func.m_bIsPutProperty = true;
        func.set_type(FunctionTypePropertyGet);
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

void class_entity::parse_variable(const char*& pData)
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
    fn.set_type(FunctionTypeVariable);
    functions_.push_back(fn);

    if (*pData == ';')
        pData++;
}

void class_entity::parse_namespace(const char*& pData)
{
    EAT_SPACES(pData)

    std::string nameSpace;
    extract_word(pData, nameSpace);

    /*if (ns.length() != 0)
    {
        nameSpace = ns + "::" + nameSpace;
    }*/

    EAT_SPACES(pData)

    if (*pData != '{')
    {
        throw std::runtime_error("Error expected character '}'");
    }
    parse_structure(pData, false);
}

std::shared_ptr<class_entity> class_entity::parse_interface(const char*& pData, const entity_type typ,
                                                            const attributes& attr)
{
    auto cls = std::make_shared<class_entity>(this);

    cls->set_type(typ);
    cls->set_attributes(attr);

    cls->parse_structure(pData, false);
    return cls;
}

std::string class_entity::parse_cpp_quote(const char*& pData)
{
    EAT_SPACES_AND_NEW_LINES(pData)
    if(*pData != '(')
        throw std::runtime_error("missing bracket after cpp_quote");
    pData++;

    EAT_SPACES_AND_NEW_LINES(pData)

    std::string contents;
    
    const char* pStart = nullptr;
    const char* pSuffix = nullptr;
    if(extract_multiline_string_literal(pData, pStart, pSuffix))
    {
        contents = std::string(pStart, pSuffix);
    }
    else
    {
        if(!extract_string_literal(pData, contents))
            throw std::runtime_error("missing initial \" in cpp_quote");
    }
    if(!*pData || *pData != ')')
        throw std::runtime_error("invalid ending in cpp_quote (no bracket)");
    pData++;  
    return contents;
}

void class_entity::parse_structure(const char*& pData, bool bInCurlyBrackets)
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

                attributes attribs(GetAttributes(pData));
                if (parse_include(pData, NULL))
                {
                    continue;
                }
                else if (interface_spec_ == edl
                         && (is_word(pData, "enclave") || is_word(pData, "trusted") || is_word(pData, "untrusted")))
                {
                    parse_namespace(pData);
                    EAT_SPACES(pData)
                    if (*pData == ';')
                        pData++;
                    continue;
                }
                else
                {
                    auto obj = std::make_shared<class_entity>(this);
                    if (get_type() == entity_type::COCLASS)
                    {
                        functions_.push_back(parse_function(pData, attribs, true));
                        EAT_SPACES(pData);
                        assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                    }
                    else if (parse_class(pData, attribs, obj, true))
                    {
                        //					assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                    }

                    else if (get_type() == entity_type::UNION)
                    {
                        if (if_is_word_eat(pData, "case"))
                        {
                            EAT_SPACES(pData);

                            std::string caseName("case(");
                            extract_word(pData, caseName);
                            caseName += ')';

                            attribs.push_back(caseName);

                            EAT_SPACES(pData);
                            assert(*pData == ':');
                            pData++;

                            EAT_SPACES(pData);
                        }

                        if (if_is_word_eat(pData, "default"))
                        {
                            attribs.push_back(std::string("default"));

                            EAT_SPACES(pData);
                            assert(*pData == ':');
                            pData++;

                            EAT_SPACES(pData);
                        }

                        attribs.merge(GetAttributes(pData));

                        functions_.push_back(parse_function(pData, attribs, false));
                        EAT_SPACES(pData);
                        assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                    }

                    else if (get_type() == entity_type::ENUM)
                    {
                        std::string elemname;
                        while (extract_word(pData, elemname))
                        {
                            EAT_SPACES(pData);
                            std::string elemValue;
                            if (*pData == '=')
                            {
                                pData++;

                                EAT_SPACES(pData);

                                if (*pData != ',' && *pData != '}' && *pData != '\0')
                                {
                                    while (*pData != 0 && *pData != ' ' && *pData != '}' && *pData != ','
                                           && *pData != '{' && *pData != ';' && *pData != ':')
                                        elemValue += *pData++;

                                    EAT_SPACES(pData);
                                }
                            }

                            function_entity fn;
                            fn.set_name(elemname);
                            fn.set_return_type(elemValue);
                            functions_.push_back(fn);

                            elemname = "";

                            if (*pData == ',')
                                pData++;

                            EAT_SPACES(pData);
                        }
                    }
                    else if (get_type() == entity_type::STRUCT)
                    {
                        function_entity func(parse_function(pData, attribs, false));
                        EAT_SPACES(pData);
                        assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                        functions_.push_back(func);
                    }
                    else if (get_type() == entity_type::DISPATCH_INTERFACE)
                    {
                        if (if_is_word_eat(pData, "properties:"))
                            ;
                        else if (if_is_word_eat(pData, "methods:"))
                            ;
                        else if (isFunction(pData))
                        {

                            functions_.push_back(parse_function(pData, attribs, false));
                            EAT_SPACES(pData);
                            assert(*pData == ';');
                            if (*pData == ';')
                                pData++;
                        }
                        else
                            assert(0);
                    }
                    else if (if_is_word_eat(pData, "public:"))
                    {
                    }
                    else if (if_is_word_eat(pData, "private:"))
                    {
                    }
                    else if (if_is_word_eat(pData, "protected:"))
                    {
                    }
                    else if (if_is_word_eat(pData, "cpp_quote"))
                    {
                        function_entity func;
                        func.set_name(parse_cpp_quote(pData));
                        func.set_type(FunctionTypeCppQuote);
                        functions_.push_back(func);
                    }
                    else if (isFunction(pData))
                    {
                        function_entity func(parse_function(pData, attribs, get_type() == entity_type::COCLASS));
                        EAT_SPACES(pData);
                        assert(*pData == ';');
                        if (*pData == ';')
                            pData++;
                        functions_.push_back(func);
                    }
                    else
                    {
                        parse_variable(pData);
                        //							advancePassStatement(pData);
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

            if (get_type() == entity_type::UNION)
            {
                if (if_is_word_eat(pData, "switch"))
                {
                    EAT_SPACES(pData)
                    if (*pData != '(')
                        assert(0);
                    pData++;

                    attributes attribs(GetAttributes(pData));
                    attribs.push_back(std::string("switch"));
                    parse_function(pData, attribs, false);

                    EAT_SPACES(pData)

                    std::string switch_name;
                    extract_word(pData, switch_name);
                    set_alias_name(switch_name);
                }
                EAT_SPACES(pData)
            }

            // get the parent name
            if (*pData == ':')
            {
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
                    if(get_type() == entity_type::ENUM)
                    {
                        auto last_owner = get_owner();
                        auto owner = get_owner();
                        do
                        {
                            owner = owner->get_owner();
                            if(owner)
                                last_owner = owner;
                        }while(owner);

                        
                        pObj = std::make_shared<class_entity>(last_owner);
                        pObj->set_type(entity_type::TYPE_NULL);
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
    if (get_type() == entity_type::DISPATCH_INTERFACE)
    {
        std::shared_ptr<class_entity> pObj;
        if (!find_class("IDispatch", pObj))
        {
            std::stringstream err;
            err << "type "
                << "IDispatch"
                << " not known";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        add_base_class(pObj.get());
        set_type(entity_type::INTERFACE);
    }
}

/*std::shared_ptr<class_entity> class_entity::parse_sequence(const char*& pData, attributes& attribs)
{
        auto newInterface = std::make_shared<class_entity>(shared_from_this());

    EAT_SPACES(pData)

        std::string parent_name;
    while (*pData != ' ' && *pData != '>' && *pData != 0)
    {
        parent_name += *pData;
        pData++;
    }
        newInterface->set_parent_name(parent_name);

    if (*pData == 0)
    {
        assert(0);
        return newInterface;
    }
    pData++;

    EAT_SPACES(pData)

        std::string name;
    while (*pData != ';' && *pData != 0)
        name += *pData++;

        newInterface->set_name(name);

    newInterface->set_type(entity_type::SEQUENCE);
    newInterface->set_attributes(attribs);

    EAT_PAST_SEMICOLON(pData)
    return newInterface;
}*/

std::shared_ptr<class_entity> class_entity::parse_typedef(const char*& pData, attributes& attribs, const char* type)
{
    auto object = std::make_shared<class_entity>(this);

    object->set_type(entity_type::TYPEDEF);

    attribs.merge(GetAttributes(pData));

    auto obj = std::make_shared<class_entity>(object.get());

    if (type == NULL && object->parse_class(pData, attribs, obj, false))
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
                temp->add_attribute(std::string("pointer"));

            if (firstPass)
            {
                firstPass = false;
                object->set_name(temp->get_name());
            }

            add_class(temp);

            EAT_SPACES(pData)
        } while (*pData == ',');
        //		assert(*pData == ';');
    }
    else
    {
        std::string parent_name;
        /*{
            auto owner = object->get_owner();
            if (owner)
            {
                parent_name = owner->get_name();
            }
        }*/

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
            while (*pData != 0 && *pData != ' ' && *pData != '*' && *pData != ';' && *pData != '{' && *pData != '[' && (*pData != ',' || template_count > 0))
            {
                if(*pData == '<')
                    template_count++;
                else if(*pData == '>')
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
                parse_typedef(pData, attribs, type.data());
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
                        
            //deal with arrays
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
                if(*pData)
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

    //	if(*pData == ';')
    //		pData++;
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
        || is_word(pData, "class") || is_word(pData, "namespace") || is_word(pData, "dispinterface")
        || is_word(pData, "exception") || is_word(pData, "enum") || is_word(pData, "union") || is_word(pData, "typedef")
        || is_word(pData, "library") || is_word(pData, "#include") || is_word(pData, "import"))
        return false;

    return true;
}

void class_entity::parse_template(const char*& pData, std::list<template_param>& templateParams)
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
            template_param tpl;
            splitVariable(phrase, tpl.name, tpl.type);
            templateParams.push_back(tpl);
            phrase.clear();
        }
        else
        {
            if(phrase.empty())
                EAT_SPACES(pData);
            phrase += *pData;
        }
    }

    template_param tpl;
    if (!phrase.empty())
    {
        splitVariable(phrase, tpl.name, tpl.type);
        templateParams.push_back(tpl);
    }

    pData++;
}

bool class_entity::parse_class(const char*& pData, attributes& attribs, std::shared_ptr<class_entity>& obj,
                               bool handleTypeDefs)
{
    bool bUseTypeDef = false;

    bool is_variable = false;

    if (is_word(pData, "struct") || is_word(pData, "interface") || is_word(pData, "class") || is_word(pData, "template")
        || is_word(pData, "dispinterface") || is_word(pData, "exception") || is_word(pData, "enum")
        || is_word(pData, "union"))
    {
        // continue if this is only a forward declarantion
        const char* curlyPos = strchr(&*pData, '{');
        const char* semicolonPos = strchr(&*pData, ';');
        if (curlyPos == NULL || curlyPos > semicolonPos)
        {
            /*if (obj->GetContainer() == NULL
                || (obj->GetContainer()->get_type() != entity_type::CLASS
                                        && obj->GetContainer()->get_type() != entity_type::STRUCT
                                        )
                                )
            {
                EAT_PAST_SEMICOLON(pData)
                return true;
            }*/
            is_variable = true;
        }

        if (handleTypeDefs && has_typedefs(pData))
            bUseTypeDef = true;
    }

    if (bUseTypeDef || if_is_word_eat(pData, "typedef"))
        obj = parse_typedef(pData, attribs, NULL);

    else if (if_is_word_eat(pData, "library"))
    {
        obj = parse_interface(pData, entity_type::LIBRARY, attribs);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "coclass"))
    {
        obj = parse_interface(pData, entity_type::COCLASS, attribs);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "struct"))
    {
        obj = parse_interface(pData, entity_type::STRUCT, attribs);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "namespace"))
    {
        obj = parse_interface(pData, entity_type::NAMESPACE, attribs);
        add_class(obj);
    }
    else if (is_variable == false && if_is_word_eat(pData, "import"))
    {
        obj = parse_interface(pData, entity_type::NAMESPACE, attribs);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "union"))
    {
        obj = parse_interface(pData, entity_type::UNION, attribs);
        add_class(obj);
        return false;
    }
    else if (is_variable == false && if_is_word_eat(pData, "enum"))
    {
        obj = parse_interface(pData, entity_type::ENUM, attribs);
        add_class(obj);
    }
    /*else if (if_is_word_eat(pData, "typedef sequence<"))
    {
        obj = parse_sequence(pData, attribs);
        add_class(obj);
    }*/
    else if (if_is_word_eat(pData, "exception"))
    {
        obj = parse_interface(pData, entity_type::EXCEPTION, attribs);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "interface"))
    {
        obj = parse_interface(pData, entity_type::INTERFACE, attribs);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "class"))
    {
        obj = parse_interface(pData, entity_type::CLASS, attribs);
        add_class(obj);
    }
    else if (if_is_word_eat(pData, "template"))
    {
        EAT_SPACES(pData)

        std::list<template_param> templateParams;
        parse_template(pData, templateParams);

        EAT_SPACES(pData)

        entity_type type = entity_type::TYPE_NULL;
        if(if_is_word_eat(pData, "class"))
            type = entity_type::CLASS;
        if(if_is_word_eat(pData, "struct"))
            type = entity_type::STRUCT;

        if (type != entity_type::TYPE_NULL)
        {
            EAT_SPACES(pData)
            obj = parse_interface(pData, type, attribs);
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
    else if (if_is_word_eat(pData, "dispinterface"))
    {
        obj = parse_interface(pData, entity_type::DISPATCH_INTERFACE, attribs);
        add_class(obj);
    }
    else
        return false;

    EAT_SPACES(pData)
    return true;
}

/*bool class_entity::LoadUsingEnv(const std::string& file)
{
        const char* includes = includeDirectories.data();
        if(!includes)
                return false;

#ifdef USE_COM
        USES_CONVERSION;
#endif

        while(*includes)
        {
                std::string path;
                while(*includes && *includes != ';')
                        path += *includes++;
                path += '\\' + file;

#ifdef USE_COM
                ITypeLibPtr typeLib;
                HRESULT hr = LoadTypeLib(A2CW(path.data()),&typeLib);

                if(SUCCEEDED(hr))
                {
                        UINT count = typeLib->GetTypeInfoCount();
                        for(int i = 0;i < count;i++)
                        {
                            ITypeInfoPtr typeInfo;
                                hr = typeLib->GetTypeInfo(i, &typeInfo);
                                if (SUCCEEDED(hr))
                                {
                                        TYPEKIND typekind;
                                        hr = typeLib->GetTypeInfoType(i, &typekind);
                                        if (SUCCEEDED(hr))
                                        {
                                                bool addClass = true;
                                                                                        auto obj =
std::make_shared<class_entity>(shared_from_this());

std::string()));

                                                obj->name = W2CA(GetInterfaceName(typeInfo));

                                                TYPEATTR* pTypeAttr;
                                                HRESULT hr = typeInfo->GetTypeAttr(&pTypeAttr);


                                                if(memcmp(&pTypeAttr->guid, &GUID_NULL, sizeof(GUID)))
                                                {
                                                        wchar_t buf[] = L"uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx) ";
                                                        hr = StringFromGUID2(pTypeAttr->guid,&buf[4],40);
                                                        if(SUCCEEDED(hr))
                                                        {
                                                                buf[4] = '(';
                                                                buf[41] = ')';
                                                                obj->add_attribute(W2CA(buf));
                                                        }
                                                }

                                                {
                                                        char buf[45];
                                                        sprintf(buf,"lcid(%d)",pTypeAttr->lcid);
                                                        obj->add_attribute(buf);
                                                }

                                                {
                                                        char buf[45];
                                                        sprintf(buf,"version(%d.%d)",pTypeAttr->wMajorVerNum,pTypeAttr->wMinorVerNum);
                                                        obj->add_attribute(buf);
                                                }

                                                bool isDispatch = false;

                                                switch (typekind)
                                                {
                                                        case TKIND_DISPATCH:
                                                                {
                                                                        //we are a dispinterface
                                                                        isDispatch = true;

                                                                        obj->parentName = "IDispatch";
                                                                        obj->type = entity_type::INTERFACE;

                                                                        //if we get past these two functions then we are
a dual interface therefore treat as a custom interface HREFTYPE refType; hr = typeInfo->GetRefTypeOfImplType(-1,
&refType); if (SUCCEEDED(hr))
                                                                        {
                                                                                ITypeInfoPtr dualTypeInfo;
                                                                                hr = typeInfo->GetRefTypeInfo(refType,
&dualTypeInfo); if (SUCCEEDED(hr))
                                                                                {
                                                                                        //we have now got the alter ego
don't break but go onto the next block typeInfo = dualTypeInfo; obj->name = W2CA(GetInterfaceName(typeInfo));
                                                                                }
                                                                                else
                                                                                {
                                                                                        GetInterfaceFunctions(pTypeAttr,
obj, typeInfo); GetInterfaceProperties(pTypeAttr, obj, typeInfo); break;
                                                                                }
                                                                        }
                                                                        else
                                                                        {
                                                                                GetInterfaceFunctions(pTypeAttr, obj,
typeInfo); GetInterfaceProperties(pTypeAttr, obj, typeInfo); break;
                                                                        }
                                                                }
                                                        case TKIND_INTERFACE:
                                                                {
                                                                        obj->type = entity_type::INTERFACE;

                                                                        if(!isDispatch)
                                                                                GetInterfaceFunctions(pTypeAttr, obj,
typeInfo);

                                                                        //get the base class's name
                                                                        HREFTYPE hreftype;
                                                                        hr = typeInfo->GetRefTypeOfImplType(0,
&hreftype); if (FAILED(hr)) break;

                                                                        ITypeInfoPtr baseTypeInfo;
                                                                        hr = typeInfo->GetRefTypeInfo(hreftype,
&baseTypeInfo); if (FAILED(hr)) break;

                                                                        obj->parentName =
W2CA(GetInterfaceName(baseTypeInfo));

                                                                }

                                                                break;


                                                        case TKIND_MODULE:
                                                                //we are a module
                                                                //NOT IMPLEMENTED
                                                                break;

                                                        case TKIND_COCLASS:
                                                                //we are a coclass
                                                                obj->type = entity_type::COCLASS;
                                                                addClass = false;
                                                                //an interesting excercise but when we are loading a
type library we don't need to load
                                                                //coclass's of other type libraries just their defined
types such as unions and interfaces
//								GetCoclassInterfaces(pTypeAttr, obj, typeInfo);
                                                                break;

                                                        case TKIND_ENUM:
                                                                //we are a enum
                                                                obj->type = entity_type::ENUM;
                                                                GetVariables(obj, pTypeAttr->cVars, typeInfo);
                                                                break;

                                                        case TKIND_ALIAS:
                                                                obj->type = TYPEDEF;
                                                                obj->parentName =
GenerateTypeString(pTypeAttr->tdescAlias, typeInfo); break;

                                                        case TKIND_RECORD:
                                                                obj->type = entity_type::STRUCT;
                                                                GetVariables(obj, pTypeAttr->cVars, typeInfo);
                                                                break;

                                                        case TKIND_UNION:
                                                                obj->type = entity_type::UNION;
                                                                GetVariables(obj, pTypeAttr->cVars, typeInfo);
                                                                break;

                                                        default:
                                                                break;
                                                }

                                                typeInfo->ReleaseTypeAttr(pTypeAttr);
                                                if(addClass)
                                                        add_class(obj);
                                        }
                                }
                        }
                }
                else
#endif
                        if(load(path.data()))
                        return true;
                if(*includes == ';')
                        includes++;
        }
        return false;
}*/

#ifdef USE_COM
void class_entity::GetInterfaceProperties(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo)
{
    USES_CONVERSION;
    // Enumerate methods and return a collection of these.
    for (unsigned int n = 0; n < pTypeAttr->cVars; n++)
    {
        LPVARDESC pvardesc = NULL;
        HRESULT hr = typeInfo->GetVarDesc(n, &pvardesc);
        if (FAILED(hr))
            break;

        function_entity fn(&obj);
        fn.type = FunctionTypePropertyGet;

        char buf[256];
        sprintf(buf, "id(%d)", pvardesc->memid);
        fn.add_attribute(buf);

        fn.return_type = GenerateTypeString(pvardesc->elemdescVar.tdesc, typeInfo);

        CComBSTR name;
        unsigned int cNames = 0;
        hr = typeInfo->GetNames(pvardesc->memid, &name, 1, &cNames);
        if (SUCCEEDED(hr))
            fn.name = W2CA(name.m_str);

        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FIN)
            fn.add_attribute("in");
        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FOUT)
            fn.add_attribute("out");
        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FLCID)
            fn.add_attribute("lcid");
        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
            fn.add_attribute("retval");
        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FLCID)
            fn.add_attribute("lcid");
        if (pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FOPT)
            fn.add_attribute("optional");

        if (pvardesc->elemdescVar.paramdesc.wParamFlags & (PARAMFLAG_FOPT | PARAMFLAG_FHASDEFAULT))
        {
            CComVariant var(pvardesc->elemdescVar.paramdesc.pparamdescex->varDefaultValue);
            var.ChangeType(VT_BSTR);
            std::string defaultValue = "defaultvalue(\"";
            defaultValue += W2CA(var.bstrVal);
            defaultValue += "\")";
            fn.add_attribute(defaultValue);
        }

        if (pvardesc->varkind == VAR_PERINSTANCE)
            fn.add_attribute("VAR_PERINSTANCE");
        else if (pvardesc->varkind == VAR_STATIC)
            fn.add_attribute("VAR_STATIC");
        else if (pvardesc->varkind == VAR_CONST)
            fn.add_attribute("VAR_CONST");
        else if (pvardesc->varkind == VAR_DISPATCH)
            fn.add_attribute("VAR_DISPATCH");

        if (pvardesc->wVarFlags & VARFLAG_FSOURCE)
            fn.add_attribute("VARFLAG_FSOURCE");
        if (pvardesc->wVarFlags & VARFLAG_FBINDABLE)
            fn.add_attribute("bindable");
        if (pvardesc->wVarFlags & VARFLAG_FREQUESTEDIT)
            fn.add_attribute("requestedit");
        if (pvardesc->wVarFlags & VARFLAG_FDISPLAYBIND)
            fn.add_attribute("displaybind");
        if (pvardesc->wVarFlags & VARFLAG_FDEFAULTBIND)
            fn.add_attribute("defaultbind");
        if (pvardesc->wVarFlags & VARFLAG_FHIDDEN)
            fn.add_attribute("hidden");
        if (pvardesc->wVarFlags & VARFLAG_FRESTRICTED)
            fn.add_attribute("restricted");
        if (pvardesc->wVarFlags & VARFLAG_FDEFAULTCOLLELEM)
            fn.add_attribute("defaultcollelem");
        if (pvardesc->wVarFlags & VARFLAG_FUIDEFAULT)
            fn.add_attribute("uidefault");
        if (pvardesc->wVarFlags & VARFLAG_FNONBROWSABLE)
            fn.add_attribute("nonbrowsable");
        if (pvardesc->wVarFlags & VARFLAG_FREPLACEABLE)
            fn.add_attribute("replaceable");
        if (pvardesc->wVarFlags & VARFLAG_FIMMEDIATEBIND)
            fn.add_attribute("immediatebind");

        obj.functions_.push_back(fn);

        if (!(pvardesc->wVarFlags & VARFLAG_FREADONLY))
        {
            fn.type = FunctionTypePropertyPut;

            obj.functions_.push_back(fn);
        }

        typeInfo->ReleaseVarDesc(pvardesc);
        pvardesc = NULL;
    }
}

void class_entity::GetInterfaceFunctions(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo)
{
    USES_CONVERSION;
    for (unsigned int n = 0; n < pTypeAttr->cFuncs; n++)
    {
        FUNCDESC* desc = NULL;
        HRESULT hr = typeInfo->GetFuncDesc(n, &desc);
        if (SUCCEEDED(hr))
        {
            function_entity fn(&obj);

            CComBSTR name;
            HRESULT hr = typeInfo->GetDocumentation(desc->memid, &name, NULL, NULL, NULL);
            if (SUCCEEDED(hr))
                fn.name = W2CA(name.m_str);

            char buf[256];
            sprintf(buf, "id(%d)", desc->memid);
            fn.add_attribute(buf);

            switch (desc->invkind)
            {
            case INVOKE_FUNC:
                fn.type = FunctionTypeMethod;
                break;
            case INVOKE_PROPERTYGET:
                fn.type = FunctionTypePropertyGet;
                fn.add_attribute("propget");
                break;
            case INVOKE_PROPERTYPUT:
                fn.type = FunctionTypePropertyPut;
                fn.add_attribute("propput");
                break;
            case INVOKE_PROPERTYPUTREF:
                fn.type = FunctionTypePropertyPut;
                fn.add_attribute("propputref");
                break;
            default:
                return;
            }

            fn.return_type = GenerateTypeString(desc->elemdescFunc.tdesc, typeInfo);

            if (desc->wFuncFlags & FUNCFLAG_FRESTRICTED)
                fn.add_attribute("restricted");
            if (desc->wFuncFlags & FUNCFLAG_FSOURCE)
                fn.add_attribute("FUNCFLAG_FSOURCE");
            if (desc->wFuncFlags & FUNCFLAG_FBINDABLE)
                fn.add_attribute("bindable");
            if (desc->wFuncFlags & FUNCFLAG_FREQUESTEDIT)
                fn.add_attribute("requestedit");
            if (desc->wFuncFlags & FUNCFLAG_FDISPLAYBIND)
                fn.add_attribute("displaybind");
            if (desc->wFuncFlags & FUNCFLAG_FDEFAULTBIND)
                fn.add_attribute("defaultbind");
            if (desc->wFuncFlags & FUNCFLAG_FHIDDEN)
                fn.add_attribute("hidden");
            if (desc->wFuncFlags & FUNCFLAG_FUSESGETLASTERROR)
                fn.add_attribute("FUNCFLAG_FUSESGETLASTERROR");
            if (desc->wFuncFlags & FUNCFLAG_FDEFAULTCOLLELEM)
                fn.add_attribute("defaultcollelem");
            if (desc->wFuncFlags & FUNCFLAG_FUIDEFAULT)
                fn.add_attribute("uidefault");
            if (desc->wFuncFlags & FUNCFLAG_FNONBROWSABLE)
                fn.add_attribute("nonbrowsable");
            if (desc->wFuncFlags & FUNCFLAG_FREPLACEABLE)
                fn.add_attribute("replaceable");
            if (desc->wFuncFlags & FUNCFLAG_FIMMEDIATEBIND)
                fn.add_attribute("immediatebind");

            BSTR* rgbstrNames = (BSTR*)_alloca(sizeof(BSTR) * (desc->cParams + 1));
            memset(rgbstrNames, 0, sizeof(BSTR*) * (desc->cParams + 1));

            unsigned int pcNames = 0;
            hr = typeInfo->GetNames(desc->memid, rgbstrNames, desc->cParams + 1, &pcNames);
            if (SUCCEEDED(hr))
            {
                for (int p = 1; p < desc->cParams; p++)
                {
                    parameter_entity param(&obj);
                    param.name = W2CA(rgbstrNames[p]);
                    param.type = GenerateTypeString(desc->lprgelemdescParam[p].tdesc, typeInfo);

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FIN)
                        param.add_attribute("in");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FOUT)
                        param.add_attribute("out");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FLCID)
                        param.add_attribute("lcid");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
                        param.add_attribute("retval");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FLCID)
                        param.add_attribute("lcid");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FOPT)
                        param.add_attribute("optional");

                    if (desc->lprgelemdescParam[p].paramdesc.wParamFlags & (PARAMFLAG_FOPT | PARAMFLAG_FHASDEFAULT))
                    {
                        CComVariant var(desc->lprgelemdescParam[p].paramdesc.pparamdescex->varDefaultValue);
                        var.ChangeType(VT_BSTR);
                        std::string defaultValue = "defaultvalue(\"";
                        defaultValue += W2CA(var.bstrVal);
                        defaultValue += "\")";
                        fn.add_attribute(defaultValue);
                    }
                    fn.parameters.push_back(param);
                }
            }

            for (int p = 0; p < desc->cParams; p++)
                SysFreeString(rgbstrNames[p]);

            typeInfo->ReleaseFuncDesc(desc);

            obj.functions_.push_back(fn);
        }
    }
}

void class_entity::GetCoclassInterfaces(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo)
{
    USES_CONVERSION;
    // Enumerate interfaces/dispinterfaces in coclass and return a collection of these.
    for (unsigned int n = 0; n < pTypeAttr->cImplTypes; n++)
    {
        function_entity fn(&obj);

        int flags = 0;
        HRESULT hr = typeInfo->GetImplTypeFlags(n, &flags);
        if (flags & IMPLTYPEFLAG_FDEFAULT)
            fn.add_attribute("default");
        if (flags & IMPLTYPEFLAG_FSOURCE)
            fn.add_attribute("source");
        if (flags & IMPLTYPEFLAG_FRESTRICTED)
            fn.add_attribute("restricted");
        if (flags & IMPLTYPEFLAG_FDEFAULTVTABLE)
            fn.add_attribute("defaultvtable");

        HREFTYPE hreftype;
        hr = typeInfo->GetRefTypeOfImplType(n, &hreftype);
        if (SUCCEEDED(hr))
        {
            ITypeInfoPtr ifTypeInfo;
            hr = typeInfo->GetRefTypeInfo(hreftype, &ifTypeInfo);
            if (SUCCEEDED(hr))
                fn.name = W2CA(GetInterfaceName(ifTypeInfo));
        }
        obj.functions_.push_back(fn);
    }
}

void class_entity::GetVariables(class_entity& theClass, unsigned variableCount, ITypeInfo* typeInfo)
{
    USES_CONVERSION;
    for (unsigned int n = 0; n < variableCount; n++)
    {
        LPVARDESC pvardesc = NULL;
        HRESULT hr = typeInfo->GetVarDesc(n, &pvardesc);
        if (SUCCEEDED(hr))
        {
            function_entity fn(&theClass);
            char buf[256];
            sprintf(buf, "id(%d)", pvardesc->memid);
            fn.add_attribute(buf);

            CComBSTR name;
            unsigned int cNames = 0; // not used
            typeInfo->GetNames(pvardesc->memid, &name, 1, &cNames);
            fn.name = W2CA(name);

            fn.return_type = GenerateTypeString(pvardesc->elemdescVar.tdesc, typeInfo);

            typeInfo->ReleaseVarDesc(pvardesc);

            theClass.functions_.push_back(fn);
        }
    }
}

std::string class_entity::GenerateTypeString(TYPEDESC& typedesc, ITypeInfo* typeInfo)
{
    USES_CONVERSION;
    if (typedesc.vt == VT_USERDEFINED)
    {
        ITypeInfoPtr userTypeInfo;
        HRESULT hr = typeInfo->GetRefTypeInfo(typedesc.hreftype, &userTypeInfo);
        if (SUCCEEDED(hr))
            return std::string(W2CA(GetInterfaceName(userTypeInfo)));
    }

    if (typedesc.vt == VT_CARRAY)
    {
        std::string ret(GenerateTypeString(typedesc.lpadesc->tdescElem, typeInfo));
        for (int i = 0; i < typedesc.lpadesc->cDims; i++)
        {
            SAFEARRAYBOUND& arrayBound = typedesc.lpadesc->rgbounds[i];
            char buf[20];
            sprintf(buf, "[%d..%d]", arrayBound.lLbound, arrayBound.lLbound + arrayBound.cElements - 1);
            ret += buf;
        }
        return ret;
    }
    if (typedesc.vt == VT_PTR || typedesc.vt == VT_SAFEARRAY)
        return std::string(GenerateTypeString(*typedesc.lptdesc, typeInfo) + "*");

    std::string ret;
    switch ((typedesc.vt ^ VT_ARRAY) & (typedesc.vt ^ VT_BYREF))
    {
    case VT_NULL:
        ret = "NULL";
        break;
    case VT_I2:
        ret = "short";
        break;
    case VT_I4:
        ret = "long";
        break;
    case VT_R4:
        ret = "float";
        break;
    case VT_R8:
        ret = "double";
        break;
    case VT_CY:
        ret = "CURRENCY";
        break;
    case VT_DATE:
        ret = "DATE";
        break;
    case VT_BSTR:
        ret = "BSTR";
        break;
    case VT_DISPATCH:
        ret = "IDispatch*";
        break;
    case VT_ERROR:
        ret = "SCODE";
        break;
    case VT_BOOL:
        ret = "BOOL";
        break;
    case VT_VARIANT:
        ret = "VARIANT";
        break;
    case VT_UNKNOWN:
        ret = "IUnknown*";
        break;
    case VT_I1:
        ret = "char";
        break;
    case VT_UI1:
        ret = "unsigned char";
        break;
    case VT_UI2:
        ret = "unsigned int";
        break;
    case VT_UI4:
        ret = "unsigned long";
        break;
    case VT_I8:
        ret = "__int64";
        break;
    case VT_UI8:
        ret = "unsigned __int64";
        break;
    case VT_INT:
        ret = "int";
        break;
    case VT_UINT:
        ret = "unsigned int";
        break;
    case VT_VOID:
        ret = "void";
        break;
    case VT_HRESULT:
        ret = "HRESULT";
        break;
    case VT_LPSTR:
        ret = "char*";
        break;
    case VT_LPWSTR:
        ret = "WCHAR*";
        break;

        /*	case VT_DECIMAL:
                        ret = "DECIMAL";
                        break;
                case VT_FILETIME:
                        ret = "FILETIME";
                        break;
                case VT_BLOB:
                        ret = "BLOB";
                        break;
                case VT_STREAM:
                        ret = "STREAM";
                        break;
                case VT_STORAGE:
                        ret = "STORAGE";
                        break;
                case VT_STREAMED_OBJECT:
                        ret = "STREAMED_OBJECT";
                        break;
                case VT_STORED_OBJECT:
                        ret = "STORED_OBJECT";
                        break;
                case VT_BLOB_OBJECT:
                        ret = "BLOB_OBJECT";
                        break;
                case VT_CF:
                        ret = "CF";
                        break;
                case VT_CLSID:
                        ret = "CLSID";
                        break;*/
    default:
        ret = "UNKNOWN_TYPE";
    }
    if (typedesc.vt & VT_ARRAY)
        ret += "[]";
    if (typedesc.vt & VT_BYREF)
        ret += "*";
    return ret;
}

CComBSTR class_entity::GetInterfaceName(ITypeInfo* typeInfo)
{
    CComBSTR ret;
    ITypeLibPtr tempTypeLib;
    unsigned int nIndex;
    HRESULT hr = typeInfo->GetContainingTypeLib(&tempTypeLib, &nIndex);
    if (FAILED(hr))
        return ret;

    tempTypeLib->GetDocumentation(nIndex, &ret, NULL, NULL, NULL);

    return ret;
}
#endif

void class_entity::extract_path_and_load(const char*& pData, const char* file)
{
    EAT_SPACES(pData)

    assert(*pData == '\"' || *pData == '<');

    pData++;

    char path[1024];
    std::string temp;
    while (*pData)
    {
        if (*pData == '\"' || *pData == '>')
        {
            pData++;

            const char* fname_ext = std::max(strrchr(temp.data(), '\\'), strrchr(temp.data(), '/'));
            if (fname_ext != NULL)
            {
                if (!load(temp.data()))
                    //					if(!LoadUsingEnv(temp))
                    throw std::runtime_error(std::string("failed to load ") + temp);
            }
            else
            {
                const char* fname_ext = std::max(strrchr(file, '\\'), strrchr(file, '/'));
                if (fname_ext != NULL)
                {
                    strncpy(path, file, fname_ext - file + 1);
                    strcat(path, temp.data());
                }
                else
                {
                    strcpy(path, temp.data());
                }
                if (!load(path))
                {
                    //					if(!LoadUsingEnv(temp))
                    throw std::runtime_error(std::string("failed to load ") +  path);
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

bool class_entity::parse_include(const char*& pData, const char* file)
{
    if (if_is_word_eat(pData, "#include"))
    {
        if (recurseImportLib)
            extract_path_and_load(pData, file);
        else
            move_past_comments(pData);
        return true;
    }
    /*if (if_is_word_eat(pData, "importlib"))
    {
        is_hash_import++;
        pData++; // get past (
        EAT_SPACES(pData)
        if (recurseImportLib)
            extract_path_and_load(pData, file);
        else
            move_past_comments(pData);
        EAT_SPACES(pData)
        pData++; // get past )
        EAT_SPACES(pData)
        assert(*pData == ';');
        if (*pData == ';')
            pData++;
        is_hash_import--;
        return true;
    }*/
    if (if_is_word_eat(pData, "import"))
    {        
        EAT_SPACES(pData);
        if(*pData != '\"')
        {
            std::stringstream err;
            err << "import path not supplied";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        pData++;
        
        std::string path;
        
        while(*pData != '\"' && *pData != 0)
        {
            path += *pData;
            pData++;
        }
        
        if(*pData != '\"')
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
        if(*pData != '{')
        {
            std::stringstream err;
            err << "import { missing";
            err << std::ends;
            std::string errString(err.str());
            throw std::runtime_error(errString);
        }
        pData++;
        parse_structure(pData, true);

        current_import.pop();
        return true;
    }
    return false;
}

bool class_entity::load(const char* file)
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
    parse_structure(tmp, true);

    return true;
}
