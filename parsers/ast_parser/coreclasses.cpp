#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <unordered_map>
#include <memory>

#include "cpp_parser.h"
#include "commonfuncs.h"
#include "coreclasses.h"

class_entity::class_entity(class_entity* owner, interface_spec spec)
    : entity(entity_type::NAMESPACE)
    , owner_(owner)
    , recurseImport(true)
    , recurseImportLib(true)
    , interface_spec_(spec)
{
}

void class_entity::deduct_template_type(const template_declaration& decl, template_deduction& deduction) const
{
    deduction.declaration = decl;

    if (decl.type == "class")
        deduction.type = template_deduction_type::CLASS;
    else if (decl.type == "typename")
        deduction.type = template_deduction_type::TYPENAME;
    else
        deduction.type = template_deduction_type::OTHER;

    if (deduction.type == template_deduction_type::OTHER)
    {
        // we need something smarter than "size_t"
        if (decl.type != "size_t" && !find_class(decl.type, deduction.identified_type))
        {
            std::cout << "template type: " << decl.type << " not found\n";
        }
    }
}

const std::list<std::shared_ptr<entity>> class_entity::get_elements(entity_type types) const
{
    std::list<std::shared_ptr<entity>> functions;
    for (auto& element : elements_)
    {
        if ((element->get_entity_type() & types) != entity_type::TYPE_NULL)
            functions.push_back(std::static_pointer_cast<entity>(element));
    }
    return functions;
}

const std::list<std::shared_ptr<function_entity>> class_entity::get_functions() const
{
    std::list<std::shared_ptr<function_entity>> functions;
    for (auto& element : elements_)
    {
        if (element->get_entity_type() >= entity_type::FUNCTION_METHOD)
            functions.push_back(std::static_pointer_cast<function_entity>(element));
    }
    return functions;
}

void class_entity::add_function(std::shared_ptr<function_entity> fn)
{
    elements_.push_back(std::static_pointer_cast<entity>(fn));
}
void class_entity::add_function(function_entity fn)
{
    elements_.push_back(std::static_pointer_cast<entity>(std::make_shared<function_entity>(fn)));
}

void class_entity::add_class(std::shared_ptr<class_entity> classObject)
{
    if (!current_import.empty())
        classObject->set_import_lib(current_import.top());

    auto classes = get_classes();
    for (auto it = classes.begin(); it != classes.end(); ++it)
    {
        auto& cls = *it;
        if (cls->get_name() == classObject->get_name())
        {
            if (cls->get_import_lib() == "")
                return;
            if (cls->get_import_lib() != "" && classObject->get_import_lib() == "")
            {
                continue;
            }
            return;
        }
    }
    elements_.push_back(std::static_pointer_cast<entity>(classObject));
}

const std::list<std::shared_ptr<class_entity>> class_entity::get_classes() const
{
    std::list<std::shared_ptr<class_entity>> classes;
    for (auto& element : elements_)
    {
        if (element->get_entity_type() < entity_type::FUNCTION_METHOD)
            classes.push_back(std::static_pointer_cast<class_entity>(element));
    }
    return classes;
}

bool class_entity::find_class(std::string type, std::shared_ptr<class_entity>& obj) const
{
    if (type.length() > 1 && type[0] == ':' && type[1] == ':')
    {
        if (!owner_) // we are the ultimate global namespace so strip out the leading "::"
            type = type.substr(2);
        else
            return owner_->find_class(type, obj);
    }

    auto path = split_namespaces(type);
    if (find_class(path, obj))
        return true;

    if (owner_) // we are the ultimate global namespace so strip out the leading "::"
        return owner_->find_class(type, obj);
    else
        return false;
}

bool class_entity::find_class(const std::vector<std::string>& type, std::shared_ptr<class_entity>& obj) const
{
    for (auto& cls : get_classes())
    {
        if (cls->get_name() == type[0])
        {
            if (type.size() == 1)
            {
                obj = cls;
                return true;
            }
            else
            {
                std::vector<std::string> t(++type.begin(), type.end());
                // keep looking if not found : may happen when an imported namespace "hides" the loaded one
                bool found = cls->find_class(t, obj);
                if (found)
                    return true;
            }
        }
    }
    return false;
}

const class_entity& get_root(const class_entity& cls)
{
    auto* tmp = cls.get_owner();
    while (tmp)
    {
        auto tmp1 = tmp->get_owner();
        if (!tmp1)
            break;
        tmp = tmp1;
    }
    if (tmp)
        return *tmp;
    return cls;
}

std::string get_full_name(const class_entity& cls, bool stop_when_base_has_no_name, bool add_prefix_delimiter,
                          std::string delimiter)
{
    auto name = cls.get_name();
    auto* tmp = cls.get_owner();
    while (tmp)
    {
        if (stop_when_base_has_no_name && tmp->get_name().empty())
            break;
        name = tmp->get_name() + delimiter + name;
        auto tmp1 = tmp->get_owner();
        if (!tmp1)
            break;
        tmp = tmp1;
    }
    if (add_prefix_delimiter)
        name = "::" + name;
    return name;
}

/*std::string expandTypeString(const char* type, const class_entity& lib)
{
        std::string temp;
        std::string ret;

        //eat spaces
        int i = 0;
        for(;type[i] != 0;i++)
                if(i != ' ')
                        break;

        for(;;i++)
        {
                if(type[i] == ' ' || type[i] == '&' || type[i] == '*' || type[i] == '[' || type[i] == 0)
                {
                        if(temp.length())
                        {
                                const class_entity* obj;
                                if(lib.find_class(temp, scope, obj))
                                {
                                        if(obj->get_type() == entity_type::TYPEDEF)
                                                temp = expandTypeString(obj->get_parent_name().data(), scope, lib);
                                }
                                ret += temp;
                        }
                        if(type[i] == 0)
                                break;
                        ret += type[i];
                        temp = "";
                }
                else
                        temp += type[i];
        }
        return ret;
}

void getTypeStringInfo(const char* type, typeInfo& info, const class_entity& lib)
{
        std::string temp;
        bool inSuffix = false;
        //eat spaces
        int i = 0;
        for(;type[i] != 0;i++)
                if(i != ' ')
                        break;

        for(;;i++)
        {
                if(type[i] == ' ' || type[i] == '&' || type[i] == '*' || type[i] == '[' || type[i] == 0)
                {
                        if(type[i] == '&' || type[i] == '*' || type[i] == '[')
                                inSuffix = true;

                        if(temp.length())
                        {
                                if(info.prefix.length())
                                        info.prefix += ' ';
                                if(temp == "const" || temp == "unsigned" || temp == "signed")
                                        info.prefix += temp;
                                else
                                {
                                        info.prefix += info.name;
                                        const class_entity* obj;
                                        if(lib.find_class(temp, scope, obj))
                                        {
                                                inSuffix = true;
                                                info.type = obj->type;
                                                info.pObj = obj;
                                        }
                                        info.name = temp;
                                }
                        }

                        if(inSuffix)
                                for(;type[i] != 0;i++)
                                        info.suffix += type[i];

                        if(type[i] == 0)
                                break;

                        temp = "";
                }
                else
                        temp += type[i];
        }
}*/
