#include "stdafx.h"
#include <sstream>
#include "coreclasses.h"
#include "CPPParser.h"

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

size_t findKeyWord(const std::string& type, const char* val, bool check_end)
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


bool isUInt8(std::string& type)
{
	if(findKeyWord(type, "char", true) != -1  ||
			findKeyWord(type, "unsigned char", true) != -1 ||
			findKeyWord(type, "unsigned __int8", true) != -1 ||
			findKeyWord(type, "uint8_t", true) != -1)
	{
		type = "uint8_t";
		return true;
	}
	return false;
}
bool isInt8(std::string& type)
{
	if(findKeyWord(type, "signed char", true) != -1 ||
		findKeyWord(type, "signed __int8", true) != -1 ||
		findKeyWord(type, "__int8", true) != -1 ||
		findKeyWord(type, "int8_t", true) != -1)
	{
		type = "int8_t";
		return true;
	}
	return false;
}
bool isUInt16(std::string& type)
{
	if(findKeyWord(type, "unsigned short", true) != -1 ||
			findKeyWord(type, "unsigned __int16", true) != -1 ||
			findKeyWord(type, "uint16_t", true) != -1)
	{
		type = "uint16_t";
		return true;
	}
	return false;
}
bool isInt16(std::string& type)
{
	if(findKeyWord(type, "short", true) != -1 ||
			findKeyWord(type, "__int16", true) != -1 ||
			findKeyWord(type, "signed short", true) != -1 ||
			findKeyWord(type, "signed __int16", true) != -1 ||
			findKeyWord(type, "int16_t", true) != -1)
	{
		type = "int16_t";
		return true;
	}
	return false;
}
bool isUInt32(std::string& type)
{
	if(findKeyWord(type, "unsigned", true) != -1 ||
			findKeyWord(type, "unsigned int", true) != -1 ||
			findKeyWord(type, "unsigned __int32", true) != -1 ||
			findKeyWord(type, "uint32_t", true) != -1)
	{
		type = "uint32_t";
		return true;
	}
	return false;
}
bool isInt32(std::string& type)
{
	if(findKeyWord(type, "int", true) != -1 ||
			findKeyWord(type, "__int32", true) != -1 ||
			findKeyWord(type, "signed int", true) != -1 ||
			findKeyWord(type, "signed __int32", true) != -1 ||
			findKeyWord(type, "int32_t", true) != -1)
	{
		type = "int32_t";
		return true;
	}
	return false;
}
bool isULong(const std::string& type)
{
	return 	findKeyWord(type, "unsigned long", true) != -1;
}
bool isLong(const std::string& type)
{
	return findKeyWord(type, "long", true) != -1;
}
bool isUInt64(std::string& type)
{
	if(findKeyWord(type, "unsigned long long", true) != -1 ||
			findKeyWord(type, "unsigned __int64", true) != -1 ||
			findKeyWord(type, "uint64_t", true) != -1)
	{
		type = "uint64_t";
		return true;
	}
	return false;
}
bool isInt64(std::string& type)
{
	if( findKeyWord(type, "long long", true) != -1 ||
			findKeyWord(type, "__int64", true) != -1 ||
			findKeyWord(type, "signed long long", true) != -1 ||
			findKeyWord(type, "signed __int64", true) != -1 ||
			findKeyWord(type, "int64_t", true) != -1)
	{
		type = "int64_t";
		return true;
	}
	return false;
}


bool isBool(const std::string& type)
{
	return findKeyWord(type, "bool", true) != -1;
}
bool isFloat(const std::string& type)
{
	return findKeyWord(type, "float", true) != -1;
}
bool isDouble(const std::string& type)
{
	return findKeyWord(type, "double", true) != -1;
}
bool isCharStar(const std::string& type)
{
	return findKeyWord(type, "char*", true) != -1 || findKeyWord(type, "char *", true) != -1;
}
bool isString(const std::string& type)
{
	return findKeyWord(type, "std::string", true) != -1;
}
bool isVector(const std::string& type)
{
	return findKeyWord(type, "std::vector", false) != -1;
}
bool isList(const std::string& type)
{
	return findKeyWord(type, "std::list", false) != -1;
}
bool isMap(const std::string& type)
{
	return findKeyWord(type, "std::map", false) != -1;
}
bool isSet(const std::string& type)
{
	return findKeyWord(type, "std::set", false) != -1;
}
bool isJavaScriptObject(const std::string& type)
{
	return findKeyWord(type, "javascript_object", false) != -1;
}
bool isEnum(const std::string& type, const Library& library)
{
	if(findKeyWord(type, "enum", true) == -1)
	{
		const ClassObject* pObj = NULL;
		if(library.FindClassObject(type, pObj) == true && pObj != NULL)
		{
			if(pObj->type == ObjectEnum)
			{
				return true;
			}
		}
		return false;
	}
	return true;
}


//strip out constness
std::string unconst(const std::string& type)
{
	std::string temp = type;
	const char* searchString = "const";
	size_t pos = findKeyWord(temp, searchString, true);
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
	pos = findKeyWord(temp, searchString, true);
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
std::string unenum(const std::string& type)
{
	std::string temp = type;
	const char* searchString = "enum";
	size_t pos = findKeyWord(temp, searchString, true);
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


void stripReferenceModifiers(std::string& paramType, std::string& referenceModifiers)
{
	size_t i = paramType.length() - 1;
	for(;i >= 0 && (paramType[i] == '*' || paramType[i] == '&' || paramType[i] == ' ');i--)
	{
		if(paramType[i] == '*' || paramType[i] == '&')
		{
			referenceModifiers += paramType[i];
		}
	}
	paramType = paramType.substr(0, i + 1);
}

void translateType(std::string& paramType, const Library& library)
{
	const ClassObject* pObj = NULL;
	if(library.FindClassObject(paramType, pObj) == true && pObj != NULL)
	{
		if(pObj->type == ObjectTypedef)
		{
			paramType = pObj->parentName;
		}
	}
}

std::string getTemplateParam(const std::string& type)
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
		err << ends;
		string errString(err.str());
		throw errString;
	}
	return param;
}
