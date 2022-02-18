#include <sstream>
#include "coreclasses.h"
#include "cpp_parser.h"

/*std::string wsdl_namespace("xt__");

std::string strip_trailing_(std::string data)
{
	if(strncmp(data.data(), wsdl_namespace.data(), wsdl_namespace.length()) == 0)
	{
		data = data.substr(wsdl_namespace.length());
	}

	int i = data.length() - 1;
	for(;i >= 0;i--)
	{
		if(data[i] != '_')
		{
			return data.substr(0,i+1);
		}
	}
	return data;
}*/

size_t find_key_word(std::string type, const char* val, bool check_end)
{
	size_t len = strlen(val);
	bool requires_loop = false;

	const char* pos = strstr(type.data(), val);
	
	if(pos == NULL)
	{
		return -1;
	}

	if(pos != type.data() && pos[-1] != ' ' && 
		pos[-1] != ' ' && 
		pos[-1] != '*' && 
		pos[-1] != '&')
	{
		return -1;
	}

	if(pos != type.data())
	{
		requires_loop = *(pos - 1) != ' ';
	}
	
	while(check_end?(pos[len] != 0 && pos[len] != ' '):false || requires_loop == true)
	{
		pos++;
		pos = strstr(pos, val);
		if(pos == NULL)
		{
			return -1;
		}
		requires_loop = *(pos - 1) != ' ';
	}	
	return pos - type.data();
}


bool is_uint8(std::string type)
{
	if(find_key_word(type, "char", true) != -1  ||
			find_key_word(type, "unsigned char", true) != -1 ||
			find_key_word(type, "unsigned __int8", true) != -1 ||
			find_key_word(type, "uint8_t", true) != -1)
	{
		type = "uint8_t";
		return true;
	}
	return false;
}
bool is_int8(std::string type)
{
	if(find_key_word(type, "signed char", true) != -1 ||
		find_key_word(type, "signed __int8", true) != -1 ||
		find_key_word(type, "__int8", true) != -1 ||
		find_key_word(type, "int8_t", true) != -1)
	{
		type = "int8_t";
		return true;
	}
	return false;
}
bool is_uint16(std::string type)
{
	if(find_key_word(type, "unsigned short", true) != -1 ||
			find_key_word(type, "unsigned __int16", true) != -1 ||
			find_key_word(type, "uint16_t", true) != -1)
	{
		type = "uint16_t";
		return true;
	}
	return false;
}
bool is_int16(std::string type)
{
	if(find_key_word(type, "short", true) != -1 ||
			find_key_word(type, "__int16", true) != -1 ||
			find_key_word(type, "signed short", true) != -1 ||
			find_key_word(type, "signed __int16", true) != -1 ||
			find_key_word(type, "int16_t", true) != -1)
	{
		type = "int16_t";
		return true;
	}
	return false;
}
bool is_uint32(std::string type)
{
	if(find_key_word(type, "unsigned", true) != -1 ||
			find_key_word(type, "unsigned int", true) != -1 ||
			find_key_word(type, "unsigned __int32", true) != -1 ||
			find_key_word(type, "uint32_t", true) != -1)
	{
		type = "uint32_t";
		return true;
	}
	return false;
}
bool is_int32(std::string type)
{
	if(find_key_word(type, "int", true) != -1 ||
			find_key_word(type, "__int32", true) != -1 ||
			find_key_word(type, "signed int", true) != -1 ||
			find_key_word(type, "signed __int32", true) != -1 ||
			find_key_word(type, "int32_t", true) != -1)
	{
		type = "int32_t";
		return true;
	}
	return false;
}
bool is_ulong(std::string type)
{
	return 	find_key_word(type, "unsigned long", true) != -1;
}
bool is_long(std::string type)
{
	return find_key_word(type, "long", true) != -1;
}
bool is_uint64(std::string type)
{
	if(find_key_word(type, "unsigned long long", true) != -1 ||
			find_key_word(type, "unsigned __int64", true) != -1 ||
			find_key_word(type, "uint64_t", true) != -1)
	{
		type = "uint64_t";
		return true;
	}
	return false;
}
bool is_int64(std::string type)
{
	if( find_key_word(type, "long long", true) != -1 ||
			find_key_word(type, "__int64", true) != -1 ||
			find_key_word(type, "signed long long", true) != -1 ||
			find_key_word(type, "signed __int64", true) != -1 ||
			find_key_word(type, "int64_t", true) != -1)
	{
		type = "int64_t";
		return true;
	}
	return false;
}


bool is_bool(std::string type)
{
	return find_key_word(type, "bool", true) != -1;
}
bool is_float(std::string type)
{
	return find_key_word(type, "float", true) != -1;
}
bool is_double(std::string type)
{
	return find_key_word(type, "double", true) != -1;
}
bool is_char_star(std::string type)
{
	return find_key_word(type, "char*", true) != -1 || find_key_word(type, "char *", true) != -1;
}
/*bool isString(std::string type)
{
	return find_key_word(type, "std::string", true) != -1;
}
bool isVector(std::string type)
{
	return find_key_word(type, "std::vector", false) != -1;
}
bool isList(std::string type)
{
	return find_key_word(type, "std::list", false) != -1;
}
bool isMap(std::string type)
{
	return find_key_word(type, "std::map", false) != -1;
}
bool isSet(std::string type)
{
	return find_key_word(type, "std::set", false) != -1;
}
bool isJavaScriptObject(std::string type)
{
	return find_key_word(type, "javascript_object", false) != -1;
}*/
bool is_enum(std::string type, const library_entity& library)
{
	if(find_key_word(type, "enum", true) == -1)
	{
		std::shared_ptr<class_entity> pObj;
		if(library.find_class(type, pObj) == true && pObj != NULL)
		{
			if(pObj->get_type() == entity_type::ENUM)
			{
				return true;
			}
		}
		return false;
	}
	return true;
}


//strip out constness
std::string unconst(std::string type)
{
	std::string temp = type;
	const char* searchString = "const";
	size_t pos = find_key_word(temp, searchString, true);
	if(pos != -1)
	{
		temp = temp.substr(0, pos) + temp.substr(pos + strlen(searchString));
		while(temp[0] == ' ')
		{
			temp = temp.substr(1);
		}
		while(temp[temp.length() - 1] == ' ')
		{
			temp = temp.substr(0, temp.length() - 1);
		}
	}	

	searchString = "struct";
	pos = find_key_word(temp, searchString, true);
	if(pos != -1)
	{
		temp = temp.substr(0, pos) + temp.substr(pos + strlen(searchString));
		while(temp[0] == ' ')
		{
			temp = temp.substr(1);
		}
		while(temp[temp.length() - 1] == ' ')
		{
			temp = temp.substr(0, temp.length() - 1);
		}
	}	

	return temp;
}
//strip out constness
std::string unenum(std::string type)
{
	std::string temp = type;
	const char* searchString = "enum";
	size_t pos = find_key_word(temp, searchString, true);
	if(pos != -1)
	{
		temp = temp.substr(0, pos) + temp.substr(pos + strlen(searchString));
		while(temp[0] == ' ')
		{
			temp = temp.substr(1);
		}
		while(temp[temp.length() - 1] == ' ')
		{
			temp = temp.substr(0, temp.length() - 1);
		}
	}

	return temp;
}


void strip_reference_modifiers(std::string& param_type, std::string& referenceModifiers)
{
	size_t i = param_type.length() - 1;
	for(;i >= 0 && (param_type[i] == '*' || param_type[i] == '&' || param_type[i] == '^' || param_type[i] == ' ');i--)
	{
		if(param_type[i] == '*' || param_type[i] == '&'|| param_type[i] == '^')
		{
			referenceModifiers = param_type[i] + referenceModifiers;
		}
	}
	param_type = param_type.substr(0, i + 1);
}

void translate_type(std::string param_type, const library_entity& library)
{
	std::shared_ptr<class_entity> pObj;
	if(library.find_class(param_type, pObj) == true && pObj != NULL)
	{
		if(pObj->get_type() == entity_type::TYPEDEF)
		{
			auto owner = pObj->get_owner().lock();
			param_type = owner->get_name();
		}
	}
}

std::string get_template_param(std::string type)
{
	std::string param;
	bool inBrackets = false;
	for(size_t i = 0;i < type.length();i++)
	{
		if(inBrackets == false)
		{
			if(type[i] == '<')
			{
				inBrackets = true;
			}
		}
		else
		{
			if(type[i] == '>')
			{
				break;
			}
			param += type[i];
		}
	}
	if(inBrackets == false)
	{
		std::stringstream err;
		err << "template type missing '>'";
		err << std::ends;
		std::string errString(err.str());
		throw errString;
	}
	return param;
}

std::vector<std::string> split_namespaces(std::string type)
{
	std::vector<std::string> ret;
	auto elems = split(type, ':');
	assert(elems.size() % 2);

	for(auto i = 0 ; i < elems.size();i++)
	{
		if(!(i%2))
		{
			ret.push_back(elems[i]);
		}
	}
	return ret;
}
