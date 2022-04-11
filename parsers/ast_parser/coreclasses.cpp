#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <unordered_map>

#include "cpp_parser.h"
#include "commonfuncs.h"
#include "coreclasses.h"

class_entity::class_entity(class_entity* owner, interface_spec spec) :
	owner_(owner),
	recurseImport(true),
	recurseImportLib(true),
	interface_spec_(spec)
{}

void class_entity::add_class(std::shared_ptr<class_entity> classObject)
{
	classes_.push_back(classObject);
}

bool class_entity::find_class(std::string type, std::shared_ptr<class_entity>& obj) const
{
	if(type.length() > 1 && type[0] == ':' && type[1] == ':')
	{
		if(!owner_) //we are the ultimate global namespace so strip out the leading "::"
			type = type.substr(2);
		else
			return owner_->find_class(type, obj);
	}

	auto path = split_namespaces(type);
	if(find_class(path, obj))
		return true;
	
	if(owner_) //we are the ultimate global namespace so strip out the leading "::"
		return owner_->find_class(type, obj);
	else
		return false;
}

bool class_entity::find_class(const std::vector<std::string>& type, std::shared_ptr<class_entity>& obj) const
{
	for(auto& cls : classes_)
	{
		if(cls->get_name() == type[0])
		{
			if(type.size() == 1)
			{
				obj = cls;
				return true;
			}
			else
			{
				std::vector<std::string> t(++type.begin(), type.end());
				return cls->find_class(t, obj);
			}
		}
	}
	return false;
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