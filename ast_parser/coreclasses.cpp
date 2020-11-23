#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <unordered_map>

#include "commonfuncs.h"
#include "coreclasses.h"

std::ostream& operator<< ( std::ostream& os, attributes& attribs )
{
	if(attribs.size())
	{
		os << '[';
		bool firstpass = true;
		for(std::list<std::string>::iterator it = attribs.begin();it != attribs.end();it++)
		{
			if(!firstpass)
				os << ',' ;
			else
				firstpass = false;
			os << (*it);
		}
		os << "] ";
	}
	return os;
}

ParameterObject::ParameterObject(ClassObject* pContainer, Library* pLibrary) :
	objectBase(pContainer, pLibrary),
	m_bIsMainVal(false),
	m_bIsException(false),
	m_bIsCallBack(false)
{}

ParameterObject::ParameterObject(const ParameterObject& other) : 
	objectBase(other),
	type(other.type), 
	m_bIsMainVal(other.m_bIsMainVal),
	m_bIsException(other.m_bIsException),
	m_bIsCallBack(other.m_bIsCallBack),
	m_arraySuffix(other.m_arraySuffix)
{
}
void ParameterObject::operator = (const ParameterObject& other)
{
	(objectBase&)*this = other;
	type = other.type;
	m_bIsMainVal = other.m_bIsMainVal;
	m_bIsException = other.m_bIsException;
	m_bIsCallBack = other.m_bIsCallBack;
	m_arraySuffix = other.m_arraySuffix;
}

std::ostream& operator<< ( std::ostream& os, ParameterObject& parameter)
{
	os << parameter.m_attributes;
	os << parameter.type << " " << parameter.name;
	return os;
}

FunctionObject::FunctionObject(ClassObject* pContainer, Library* pLibrary) : 
	objectBase(pContainer, pLibrary),
	m_bBadFunction(false),
//	m_bIsGetProperty(false),
//	m_bIsPutProperty(false),
	type(FunctionTypeMethod),
	pure_virtual(false),
	hasImpl(false)
{
}

FunctionObject::FunctionObject(const FunctionObject& other) : 
	objectBase(other),
	type(other.type), 
	returnType(other.returnType), 
	parameters(other.parameters),
//	m_bIsGetProperty(other.m_bIsGetProperty),
//	m_bIsPutProperty(other.m_bIsPutProperty),
	m_bBadFunction(other.m_bBadFunction),
	raises(other.raises),
	pure_virtual(other.pure_virtual),
	hasImpl(other.hasImpl)
{
}
void FunctionObject::operator = (const FunctionObject& other)
{
	(objectBase&)*this = other;
	type = other.type;
//	m_attributes = other.m_attributes;
	returnType = other.returnType;
	parameters = other.parameters;
//	m_bIsGetProperty = other.m_bIsGetProperty;
//	m_bIsPutProperty = other.m_bIsPutProperty;
	m_bBadFunction = other.m_bBadFunction;
	raises = other.raises;
	pure_virtual = other.pure_virtual;
}

std::ostream& operator<< ( std::ostream& os, FunctionObject& function)
{
	os << '\t';
	os << function.m_attributes;
	os << function.returnType << " " << function.name << "(";

	for(std::list<ParameterObject>::iterator it = function.parameters.begin();it != function.parameters.end();it++)
	{
		if(it != function.parameters.begin())
			os << ", ";
		os << *it;
	}
	os << ");\n";
	return os;
}

ClassObject::ClassObject(ClassObject* pContainer, Library* pLibrary, const std::string& ns, interface_spec spec) :
	recurseImport(true),
	recurseImportLib(true),
	objectBase(pContainer, pLibrary),
		type(pContainer == ((ClassObject*)pLibrary) ? ObjectLibrary :  pContainer->type),
	m_namespace(ns),
	m_interface_spec(spec)
{
	if(ns != "")
	{
		std::string n = ns;
	}
}

ClassObject::ClassObject(const ClassObject& other) : 
	recurseImport(true),
	recurseImportLib(true),
	objectBase(other),
	type(other.type), 
	parentName(other.parentName), 
	functions(other.functions),
	m_namespace(other.m_namespace),
	m_interface_spec(other.m_interface_spec)
{
	if(m_namespace != "")
	{
		std::string n = m_namespace;
	}
}

void ClassObject::AddClass(ClassObjectRef classObject)
{
	m_ownedClasses.push_back((*classObject).name);
	GetLibrary().AddClass(classObject);
}

void ClassObject::operator = (const ClassObject& other)
{
	(objectBase&)*this = other;
	type = other.type;
	parentName = other.parentName;
	functions = other.functions;
}

std::ostream& operator<< ( std::ostream& os, ClassObject& other)
{
	if(other.m_attributes.size())
	{
		os << other.m_attributes;
		os << "\n";
	}
	switch(other.type)
	{
	case ObjectStruct:
		{
			os << "struct " << other.name;
			if(other.parentName.length())
				os << " : " << other.parentName;
			os << "\n" << '{' << "\n";
			for(std::list<FunctionObject>::iterator it = other.functions.begin();it != other.functions.end();it++)
				os << *it;
			os << "};\n\n";
			break;
		}
	case ObjectEnum:
		{
			os << "enum " << other.name;
			os << '{' << "\n";
			for(std::list<FunctionObject>::iterator it = other.functions.begin();it != other.functions.end();it++)
				os << *it;
			os << "};\n\n";
			break;
		}
	case ObjectException:
		{
			os << "exception " << other.name;
			if(other.parentName.length())
				os << " : " << other.parentName;
			os << "\n" << '{' << "\n";
			for(std::list<FunctionObject>::iterator it = other.functions.begin();it != other.functions.end();it++)
				os << *it;
			os << "};\n\n";
			break;
		}
	case ObjectSequence:
		{
			os << "sequence<" << other.parentName << "> " << other.name;
			break;
		}
	case ObjectTypeInterface:
		{
			os << "interface " << other.name;
			if(other.parentName.length())
				os << " : " << other.parentName;
			os << "\n" << '{' << "\n";
			for(std::list<FunctionObject>::iterator it = other.functions.begin();it != other.functions.end();it++)
				os << *it;
			os << "};\n\n";
			break;
		}
	case ObjectTypedef:
		{
			os << "typedef " << other.parentName << " " << other.name;
			break;
		}
	case ObjectCoclass:
		{
			os << "coclass " << other.name;
			break;
		}
	case ObjectUnion:
		{
			os << "union " << other.name;
			break;
		}
	default:
		verboseStream << "arrg unknown type\n";
	}
#ifdef _DEBUG
	verboseStream.flush();
#endif
	return os;
}

std::ostream& operator<< ( std::ostream& os, Library& objs )
{
	for(CLASS_OBJECT_LIST::iterator it = objs.m_classes.begin();it != objs.m_classes.end();it++)
		os << *(*it);
	return os;
}

extern std::string getTemplateParam(const std::string& type);
extern void stripReferenceModifiers(std::string& paramType, std::string& referenceModifiers);
extern bool isSet(const std::string& type);
extern bool isVector(const std::string& type);
extern bool isList(const std::string& type);

bool Library::FindClassObject(const std::string& type, const ClassObject*& obj) const
{
	std::string typ = type;
	std::string namesp;

	std::string subType;
	if(isSet(typ))
	{
		subType = getTemplateParam(typ);
		std::string referenceModifiers;
		stripReferenceModifiers(subType, referenceModifiers);

		const ClassObject* subobj = NULL;
		if(FindClassObject(subType, subobj) == false)
		{
			return false;
		}
		if(subobj->type == ObjectTypedef)
		{
			subType = subobj->parentName;
		}
		typ = "std::set<" + subType + referenceModifiers + ">";
	}
	else if(isVector(typ))
	{
		subType = getTemplateParam(typ);
		std::string referenceModifiers;
		stripReferenceModifiers(subType, referenceModifiers);

		const ClassObject* subobj = NULL;
		if(FindClassObject(subType, subobj) == false)
		{
			return false;
		}
		if(subobj->type == ObjectTypedef)
		{
			subType = subobj->parentName;
		}
		typ = "std::vector<" + subType + referenceModifiers + ">";
	}
	else if(isList(typ))
	{
		subType = getTemplateParam(typ);
		std::string referenceModifiers;
		stripReferenceModifiers(subType, referenceModifiers);

		const ClassObject* subobj = NULL;
		if(FindClassObject(subType, subobj) == false)
		{
			return false;
		}
		if(subobj->type == ObjectTypedef)
		{
			subType = subobj->parentName;
		}

		typ = "std::list<" + subType + referenceModifiers + ">";
	}
	else
	{
		std::vector<std::string> elems;
		split(typ, ':', elems);
		assert(elems.size() % 2 == 1 && elems.size() <= 3);

		if(elems.size() == 3)
		{
			namesp = elems[0];
			typ = elems[2];
		}
	}

	for(const auto cls : m_classes)
	{
		const ClassObject& clsRef = *cls;
		/*if(pobj.type == ObjectTypedef)
		{
			name = pobj.parentName;
		}*/
		if(clsRef.name == typ && 
		(namesp == clsRef.m_namespace || 
		(m_interface_spec == edl && clsRef.m_namespace == "enclave")))
		{
			obj = &clsRef;
			return true;
		}
	}
	return false;
}

std::string expandTypeString(const char* type, const Library& lib)
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
				const ClassObject* obj;
				if(lib.FindClassObject(temp, obj))
				{
					if(obj->type == ObjectTypedef)
						temp = expandTypeString(obj->parentName.data(), lib);
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

void getTypeStringInfo(const char* type, typeInfo& info, const Library& lib)
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
					const ClassObject* obj;
					if(lib.FindClassObject(temp, obj))
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
}