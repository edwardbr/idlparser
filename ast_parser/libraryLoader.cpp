#include <stdio.h> 
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include "coreclasses.h"

#include "function_timer.h"

extern xt::function_timer* p_timer;

//special attribute flags:
//pointer to indicate that a typedef is of type pointer
//switch for unions to indicate that the particular member of a union or struct is control member

#define EAT_SPACES(data) while(*data == ' ')data++;
#define EAT_PAST_SEMICOLON(data) while(*data != ';' && *data != '\0') data++;if(*data == ';')data++;

attributes GetAttributes(const char*& pData)
{
	attributes attribs;
	if(*pData == '[')
	{
		pData++;

		EAT_SPACES(pData)

		verboseStream << "[";;
		bool bInAttribute = false;
		std::string property;
		int inBracket = 0;
		for(;*pData!=0;pData++)
		{

			if(!bInAttribute && *pData == ' ')
				continue;
			if(bInAttribute && !inBracket && *pData == ',')
			{
				attribs.push_back(property);
				verboseStream << property.data() << ",";
				property = "";
				bInAttribute = false;
			}
			else if(*pData == ']')
			{
				if(bInAttribute)
					attribs.push_back(property);
				verboseStream << property.data();
				pData++;
				break;
			}
			else
			{
				if(*pData == '(')
					inBracket++;
				else if(*pData == ')')
					inBracket--;
				bInAttribute = true;
				property += *pData;
			}
		}	
		verboseStream << "] ";

		EAT_SPACES(pData)
	}
	return attribs;
}

FunctionObject ClassObject::GetFunction(const char*& pData, attributes& attribs, bool bFunctionIsInterface)
{
	if(name == "IXMLDocument")
		int x = 1;

	FunctionObject func(this, &GetLibrary());
	func.m_attributes = attribs;

	bool bFunctionIsProperty = true;
	bool bConstant = false;

	EAT_SPACES(pData)

	while(*pData)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

		while(*pData != ' ' && *pData != '(' && *pData != ')' && *pData != ';' && *pData != 0)
			func.name += *pData++;

		verboseStream << func.name.data() << " ";

		if(func.name == "const") //CORBA types
			bConstant = true;

		if(func.name == "const" || func.name == "unsigned"  || func.name == "signed" || (m_interface_spec == corba && func.name == "inout") || ((m_interface_spec == corba || m_interface_spec == com) && (func.name == "in" || func.name == "out"))) //CORBA types
		{
			if(func.name == "inout")
			{
				func.m_attributes.push_back("in");
				func.m_attributes.push_back("out");
			}
			else
				func.m_attributes.push_back(func.name);
			func.name = "";
			continue;
		}

		EAT_SPACES(pData)

		if(*pData == '(')
		{
			verboseStream << '(';

			if(func.name[0] == '&' || func.name[0] == '*')
			{
				func.returnType += func.name[0];
				func.name = &func.name.data()[1];
			}
			pData++;
			bFunctionIsProperty = false;
			break;
		}
		else if(*pData == 0 || *pData == ';' || *pData == ')')
		{
			break;
		}
		else
		{
			if(func.returnType.length())
				func.returnType += ' ';

/*			const ClassObject* obj;
			if(FindClassObject(func.name, *pObjects, obj))
			{
				if(obj->type == ObjectTypedef)
				{
					func.name = obj->parentName;
					break;
				}
			}*/
			func.returnType += func.name;
			func.name = "";
		}
	}

	if(bFunctionIsInterface)
		;//no processing
	else if(bFunctionIsProperty)
	{
/*		if(bConstant == false)
			func.type = FunctionTypePropertyPut;
	//		func.m_bIsPutProperty = true;
	//	func.m_bIsGetProperty = true;*/
		func.type = FunctionTypeVariable;
	}
	else
	{
		while(*pData != 0 && *pData != ')' && *pData != ';')
		{
			ParameterObject parameter(this, &GetLibrary());
			bool b_nullParam = false;

			EAT_SPACES(pData)

			parameter.m_attributes = GetAttributes(pData);

			while(*pData != 0)
			{
				if(p_timer != NULL && p_timer->is_timedOut())
				{
					exit(0);
				}

				//deal with call back functions
				if(*pData == '(')
				{
					pData++;
					parameter.type += '(';

					std::string temp;
					while(*pData != ')' && *pData != 0)
					{
						if(ExtractWord(pData,temp))
							parameter.type += temp;
						else
							parameter.type += *pData++;
					}
					assert(*pData == ')');
					if(*pData == ')')
						pData++;
					
					parameter.type += ')';

					parameter.name = temp;

					while(*pData != ')' && *pData != 0)
						parameter.type += *pData++;
					assert(*pData == ')');
					if(*pData == ')')
					{
						pData++;
						parameter.type += ')';
						parameter.m_bIsCallBack = true;
					}
					else 
						std::cerr << "not a call back as expected";
				}
				else
				{
					while(*pData != 0 && *pData != ' ' && *pData != ',' && *pData != '[' && *pData != '*' && *pData != ')')
						parameter.name += *pData++;
				}
				EAT_SPACES(pData)
				while(*pData == '*' || *pData == '&')
				{
					parameter.name += *pData++;
					EAT_SPACES(pData)
				}
				while(*pData == '[')
				{
					std::string suffix;
					if(parameter.name.length())
					{
						while(*pData != ']' && *pData != '\0')
						{
							EAT_SPACES(pData)
							suffix += *pData++;
							EAT_SPACES(pData)
						}
						suffix += *pData++;
						EAT_SPACES(pData)
						parameter.m_arraySuffix.push_back(suffix);
					}
					else
						parameter.m_attributes.merge(GetAttributes(pData));
				}

				verboseStream << parameter.name.data() << " ";

				EAT_SPACES(pData)

				if(parameter.name == "const" || parameter.name == "unsigned"  || parameter.name == "signed" || (m_interface_spec == corba && parameter.name == "inout") || ((m_interface_spec == corba || m_interface_spec == com) && (parameter.name == "in" || parameter.name == "out"))) //CORBA types
				{
					if(parameter.name == "inout")
					{
						parameter.m_attributes.push_back("in");
						parameter.m_attributes.push_back("out");
					}
					else
						parameter.m_attributes.push_back(parameter.name);
					parameter.name = "";
					continue;
				}

				//gsoap yuckyness
				if(*pData == '0' || *pData == '1')
				{
					pData++;
					EAT_SPACES(pData)
				}

				if(*pData == ',' || *pData == ')')
				{
					if(parameter.name == "void")
					{
						b_nullParam = true;
						break;
					}

					if(*pData == ',')
						verboseStream << ", ";

					if(parameter.name[0] == '&' || parameter.name[0] == '*')
					{
						parameter.type += parameter.name[0];
						parameter.name = &parameter.name.data()[1];
					}
					break;
				}
				else
				{
					if(parameter.type.length())
						parameter.type += ' ';

/*					const ClassObject* obj;
					if(GetLibrary().FindClassObject(parameter.name, obj))
						if(obj->type == ObjectTypedef)
							parameter.name = obj->parentName;

//#############this code was commented out doing so will break the com/corba bridge							
					for(std::list<ClassObject*, ClassObjectAllocator>::iterator it = pObjects->m_classes->begin();it != pObjects->m_classes->end();it++)
					{
						if((*it).name == parameter.name)
						{
							if((*it).type == ObjectTypedef)
							{
								parameter.name = (*it).parentName;
								break;
							}
						}
					}*/

					parameter.type += parameter.name;
					parameter.name = "";
				}
			}
			//eat comma
			if(*pData == ',')
				pData++;

			EAT_SPACES(pData)

			if(b_nullParam != true)
				func.parameters.push_back(parameter);
		}
	}

	if(*pData == ')')
	{
		pData++;
		verboseStream << ")";
	}

	while(!bFunctionIsProperty && *pData != 0 && *pData != ';')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		EAT_SPACES(pData)

		if(*pData == '{')
		{
			pData++;
			func.hasImpl = true;
			EAT_SPACES(pData)
			if(*pData != '}')
				std::cerr << "function implementations not supported";
			else
				pData++;
			break;
		}
		else if(func.name == name && *pData == ':')//this is for constructor initialisers
		{
			pData++;
			while(1)
			{
				EAT_SPACES(pData)
				while(*pData++ != '(')
					continue;
				while(*pData++ != ')')
					continue;
				EAT_SPACES(pData)
				if(*pData != ',')
					break;
			}			
		}
		else if(!Strcmp2(&*pData,"raises"))
		{
			pData += 6; //strlen "raises"

			EAT_SPACES(pData)

			if(*pData != '(')
				continue;
			pData++;
			
			EAT_SPACES(pData)

			std::string exception;
			while(*pData != ')' && *pData != 0)
			{
				if(p_timer != NULL && p_timer->is_timedOut())
				{
					exit(0);
				}

				if(*pData == ',')
				{
					func.raises.push_back(exception);
					exception.empty();
				}
				else
					exception += *pData;
				
				pData++;

				EAT_SPACES(pData)
			}
			func.raises.push_back(exception);
			if(*pData == 0)
			{
				break;
			}
		}
		else if(*pData == '=')
		{
			pData++;
			EAT_SPACES(pData)
			if(pData,'0')
			{
				func.pure_virtual = true;
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
	if(func.HasValue("propput") || func.HasValue("propputref"))
	{
//		func.m_bIsPutProperty = true;
		func.type = FunctionTypePropertyPut;
	}
	else if(func.HasValue("propget"))
	{
//		func.m_bIsPutProperty = true;
		func.type = FunctionTypePropertyGet;
	}
	return func;
}

bool isFunction(const char* pData)
{
	while(*pData != '\0' && *pData != ';')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '(')
			return true;
		if(*pData == '=')
			return false;

		pData++;
	}
	return false;
}

void advancePassStatement(const char*& pData)
{
	while(*pData != '\0' && *pData != ';')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '\"')
		{
			pData++;
			while(*pData && *pData != '\"')
			{
				if(BeginsWith(pData,"\\\""))
					pData++;
				pData++;
			}
		}

		pData++;
	}
	if(*pData == ';')
		pData++;
}

void splitVariable(const std::string& phrase, std::string& name, std::string& type)
{
	size_t j = phrase.length() - 1;
	while(phrase[j] == ' ' && j > 0)
	{
		j--;
	}
	char end_pos = j;
	for(;j > 0;j--)
	{
		char it = phrase[j];
		if(!((it >= '0' && it <= '9') || (it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z') || (it == '_')))
			break;
	}
	char start_pos = j;
	while(phrase[j] == ' ' && j > 0)
	{
		j--;
	}

	name = phrase.substr(start_pos + 1, end_pos);
	type = phrase.substr(0,j + 1);
}

void ClassObject::GetVariable(const char*& pData)
{
	std::string phrase;
	for(;*pData != 0 && *pData != ';' ;pData++)
	{
		phrase += *pData;
	}

	FunctionObject fn(this, &GetLibrary());
	splitVariable(phrase, fn.name, fn.returnType);
	//strip out yucky gsoap limiters
	if(fn.name == "0" || fn.name == "1")
	{
		splitVariable(fn.returnType, fn.name, fn.returnType);
	}
	fn.type = FunctionTypeVariable;
	functions.push_back(fn);

	if(*pData == ';')
		pData++;
}

void ClassObject::GetNamespaceData(const char*& pData, const std::string& ns)
{
	EAT_SPACES(pData)

	std::string nameSpace;
	ExtractWord(pData, nameSpace);

	if(ns.length() != 0)
	{
		nameSpace = ns + "::" + nameSpace;
	}

	EAT_SPACES(pData)

	if(*pData != '{')
	{
		std::stringstream err;
		err << "Error expected character '}'";
		err << std::ends;
		std::string errString(err.str());
		throw errString;
	}
	GetStructure(pData, nameSpace);
}

ClassObject::ClassObjectRef ClassObject::GetInterface(const char*& pData, const ObjectType typ, const attributes& attr, const std::string& ns)
{
	ClassObjectRef cls(new ClassObject(this, &GetLibrary(), ns));
	cls->type = typ;
	cls->m_attributes = attr;

	cls->GetStructure(pData, ns);
	return cls;
}

void ClassObject::GetStructure(const char*& pData, const std::string& ns, bool bInCurlyBrackets)
{
	bool bHasName = false;
	while(*pData != 0)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

		EAT_SPACES(pData)

		if(*pData == 0)
		{
			break;
		}
		
		if(bInCurlyBrackets)
		{
			if(*pData == '}')
			{
				verboseStream << "\n};\n";
				pData++;

				break;
			}
			else
			{
				EAT_SPACES(pData)

				if(GetFileData(pData, NULL))
				{}
				else if(IfIsWordEat(pData,"namespace"))
				{
					GetNamespaceData(pData, ns);
					continue;
				}
				else
				{

					attributes attribs(GetAttributes(pData));
					ClassObjectRef obj(new ClassObject(this, &GetLibrary(), ns));
					if(type == ObjectCoclass)
					{
						functions.push_back(GetFunction(pData, attribs, true));
						EAT_SPACES(pData);
						assert(*pData == ';');
						if(*pData == ';')
							pData++;
					}
					else if(ExtractClass(pData,attribs,obj, ns))
					{
	//					assert(*pData == ';');
						if(*pData == ';')
							pData++;
					}

					else if(type == ObjectUnion)
					{
						if(IfIsWordEat(pData,"case"))
						{
							EAT_SPACES(pData);

							std::string caseName("case(");
							ExtractWord(pData, caseName);
							caseName += ')';

							attribs.push_back(caseName);

							EAT_SPACES(pData);
							assert(*pData == ':');
							pData++;

							EAT_SPACES(pData);
						}

						if(IfIsWordEat(pData,"default"))
						{
							attribs.push_back(std::string("default"));

							EAT_SPACES(pData);
							assert(*pData == ':');
							pData++;

							EAT_SPACES(pData);
						}

						attribs.merge(GetAttributes(pData));

						functions.push_back(GetFunction(pData, attribs, false));
						EAT_SPACES(pData);
						assert(*pData == ';');
						if(*pData == ';')
							pData++;
					}

					else if(type == ObjectEnum)
					{
						std::string elemname;
						while(ExtractWord(pData, elemname))
						{
							if(p_timer != NULL && p_timer->is_timedOut())
							{
								exit(0);
							}
							EAT_SPACES(pData);
							std::string elemValue;
							if(*pData == '=')
							{
								pData++;

								EAT_SPACES(pData);

								if(*pData != ',' && *pData != '}' && *pData != '\0')
								{
									while(*pData != 0 && *pData != ' ' && *pData != '}' && *pData != ',' && *pData != '{' && *pData != ';' && *pData != ':')
										elemValue += *pData++;

									EAT_SPACES(pData);
								}
							}

							FunctionObject fn(this, &GetLibrary());
							fn.name = elemname;
							fn.returnType = elemValue;
							functions.push_back(fn);


							elemname = "";

							if(*pData == ',')
								pData++;

							EAT_SPACES(pData);
						}
					}
					else if(type == ObjectStruct)
					{
						FunctionObject func(GetFunction(pData, attribs, false));
						EAT_SPACES(pData);
						if(func.hasImpl == false)
						{
							assert(*pData == ';');
							if(*pData == ';')
								pData++;
						}
						functions.push_back(func);
					}
					else if(type == ObjectTypeDispInterface)
					{
						if(IfIsWordEat(pData,"properties:"))
							;
						else if(IfIsWordEat(pData,"methods:"))
							;
						else if(isFunction(pData))
						{
							
							functions.push_back(GetFunction(pData, attribs, false));
							EAT_SPACES(pData);
							assert(*pData == ';');
							if(*pData == ';')
								pData++;
						}
						else 
							assert(0);
					}
					else if(IfIsWordEat(pData,"public:"))
					{}
					else if(IfIsWordEat(pData,"private:"))
					{}
					else if(IfIsWordEat(pData,"protected:"))
					{}
					else if(isFunction(pData))
					{
						FunctionObject func(GetFunction(pData, attribs, type == ObjectCoclass));
						EAT_SPACES(pData);
						if(func.hasImpl == false)
						{
							assert(*pData == ';');
							if(*pData == ';')
								pData++;
						}
						functions.push_back(func);
					}
					else 
					{
						GetVariable(pData);
//							advancePassStatement(pData);
					}
				}
			}
		}
		else
		{
			//get name
			if(!bHasName)
			{
				while(*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
					name += *pData++;
				bHasName = true;

				EAT_SPACES(pData)
			}

			if(type == ObjectUnion)
			{
				if(IfIsWordEat(pData,"switch"))
				{
					EAT_SPACES(pData)
					if(*pData != '(')
						assert(0);
					pData++;

					attributes attribs(GetAttributes(pData));
					attribs.push_back(std::string("switch"));
					GetFunction(pData, attribs, false);
	
					EAT_SPACES(pData)
					
					ExtractWord(pData, parentName);
				}
				EAT_SPACES(pData)
			}

			//get the parent name
			if(*pData == ':')
			{
				pData++;

				EAT_SPACES(pData)

				while(*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
				{
					parentName += *pData;
					pData++;
				}

				EAT_SPACES(pData)

				if(parentName == "public" || parentName == "protected" || parentName == "private")
				{
					parentName = "";
					while(*pData != 0 && *pData != ' ' && *pData != '{' && *pData != ';' && *pData != ':')
					{
						parentName += *pData;
						pData++;
					}
				}

				EAT_SPACES(pData)
			}

			if(*pData == '{')
			{
				verboseStream << ' ' << name.data() << "\n";
				verboseStream << "{";
		
				bInCurlyBrackets = true;
				pData++;

				EAT_SPACES(pData)
			}
			else 
				pData++;
		}
		EAT_SPACES(pData)
	}
	if(type == ObjectTypeDispInterface)
	{
		parentName = "IDispatch";
		type = ObjectTypeInterface;
	}

}

ClassObject::ClassObjectRef ClassObject::ParseSequence(const char*& pData, attributes& attribs, const std::string& ns)
{
	ClassObjectRef newInterface(new ClassObject(this, &GetLibrary(), ns));

	EAT_SPACES(pData)

	while(*pData != ' ' && *pData != '>' && *pData != 0)
	{
		newInterface->parentName += *pData;
		pData++;
	}

	if(*pData == 0)
	{
		assert(0);
		return newInterface;
	}
	pData++;

	EAT_SPACES(pData)

	while(*pData != ';' && *pData != 0)
		newInterface->name += *pData++;

	verboseStream << "sequence<" << newInterface->parentName.data() << "> " << newInterface->name.data() << ";\n";
	
	newInterface->type = ObjectSequence;
	newInterface->m_attributes = attribs;

	EAT_PAST_SEMICOLON(pData)
	return newInterface;
}

ClassObject::ClassObjectRef ClassObject::ParseTypedef(const char*& pData, attributes& attribs, const std::string& ns, const char* type)
{
	ClassObjectRef object(new ClassObject(this, &GetLibrary(), ns));

	object->type = ObjectTypedef;

	attribs.merge(GetAttributes(pData));

	ClassObjectRef obj(new ClassObject(object.get(), &object->GetLibrary(), ns));
	if(type == NULL && object->ExtractClass(pData, attribs,obj, ns,false))
	{
		object->parentName = obj->name;
		bool firstPass = true;
		do
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}

			ClassObjectRef source(object);
			if(object->parentName == "")
			{
				source = obj;
			}
			ClassObjectRef temp(new ClassObject(*source));

			//loop around to extract the names
			EAT_SPACES(pData)

			if(!firstPass)
			{
				pData++;//strip out the comma
				EAT_SPACES(pData)
			}

			bool ispointer = *pData == '*';
			if(ispointer)
			{
				pData++;
				EAT_SPACES(pData)
			}

			while(*pData != 0 && *pData != ';' && *pData != '[' && *pData != ',' && *pData != '{')
			{
				temp->name += *pData;
				pData++;
			}
			if(ispointer)
				temp->m_attributes.push_back(std::string("pointer"));
			if(firstPass)
			{
				firstPass = false;
				object->name = temp->name;
			}
			
			AddClass(temp);

			EAT_SPACES(pData)
		} while(*pData == ',');
//		assert(*pData == ';');
	}
	else
	{
		if(type != NULL)
			object->parentName = type;
		while(*pData != 0 && *pData != ';')
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			if(object->parentName.length())
				object->parentName += ' ';

			object->parentName += object->name;
			object->name = "";

			EAT_SPACES(pData)
		
			while(*pData != 0 && *pData != ' ' && *pData != '*' && *pData != ';' && *pData != '{' && *pData != ',')
			{
				object->name += *pData;
				pData++;
			}
			if(*pData != 0 && *pData == '*' )
			{
				object->name += *pData;
				pData++;
			}
			//check for multiple definitions
			if(*pData == ',')
			{
				pData++;
				EAT_SPACES(pData)
				std::string& name = object->parentName;
				std::string type;
				for(size_t i = 0;i < name.length();i++)	
				{
					if(name[i] != '&' && name[i] != '*' && name[i] != '[')
						type += name[i];
					else
						break;
				}
				ParseTypedef(pData, attribs, ns, type.data());
			}

			//removing any nasty member structures, perhaps I'll do some thing intellegent with it one day...
			if(*pData == '{')
			{
				pData++;
				int braketCount = 1;
				while(braketCount)
				{
					if(p_timer != NULL && p_timer->is_timedOut())
					{
						exit(0);
					}
					if(*pData == '\0')
						break;

					if(*pData == '{')
						braketCount++;

					if(*pData == '}')
						braketCount--;
					pData++;
				}
			}
		}
		AddClass(object);
		assert(*pData == ';');
//		pData++;
	}
	
	verboseStream << "typedef " << object->name.data() << " " << object->name.data() << "\n";

//	object.m_attributes = attribs;
	return object;
}

void ClassObject::ParseUnion(const char*& pData, attributes& attribs)
{
	verboseStream << "skipping past a union\n";
	while(*pData && *pData != ';')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '{')
		{
			pData++;
			int braketCount = 1;
			while(braketCount)
			{
				if(p_timer != NULL && p_timer->is_timedOut())
				{
					exit(0);
				}
				if(*pData == '\0')
					break;

				if(*pData == '{')
					braketCount++;

				if(*pData == '}')
				{
					braketCount--;
					if(!braketCount)
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
	while(*pData && *pData != ';')
		pData++;
	
//	if(*pData == ';')
//		pData++;
}

bool ClassObject::ObjectHasTypeDefs(const char* pData)
{
	int braketCount = -1;
	while(*pData && *pData != ';')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '{')
		{
			pData++;
			braketCount = 1;
			while(braketCount)
			{
				if(p_timer != NULL && p_timer->is_timedOut())
				{
					exit(0);
				}
				if(*pData == '\0')
					break;

				if(*pData == '{')
					braketCount++;

				if(*pData == '}')
				{
					braketCount--;
					if(!braketCount)
					{
						pData++;
						break;
					}
				}
				pData++;
			}
		}
		if(!braketCount)
			break;
		pData++;
	}
	EAT_SPACES(pData)
	
	if(	*pData == ';' || 
		*pData == '[' || 
		*pData == '\0' ||
		IsWord(pData,"struct") || 
		IsWord(pData,"interface") || 
		IsWord(pData,"class") || 
		IsWord(pData,"namespace") || 
		IsWord(pData,"dispinterface") || 
		IsWord(pData,"exception") || 
		IsWord(pData,"enum") || 
		IsWord(pData,"union") || 
		IsWord(pData,"typedef") || 
		IsWord(pData,"library") || 
		IsWord(pData,"#include") || 
		IsWord(pData,"import")
	  )
	  return false;

	return true;
}

void ClassObject::ExtractTemplate(const char*& pData, std::list<templateParam>& templateParams)
{
	EAT_SPACES(pData)

	if(*pData != '<')
	{
		std::stringstream err;
		err << "Error expected character '<'";
		err << std::ends;
		std::string errString(err.str());
		throw errString;
	}
	pData++;
	
	std::string phrase;
	for(;*pData != 0 && *pData != '>' ;pData++)
	{
		if(*pData == ',')
		{
			templateParam tpl;
			splitVariable(phrase, tpl.name, tpl.type);
			templateParams.push_back(tpl);
			phrase.clear();
		}
		else
		{
			phrase += *pData;
		}
	}

	templateParam tpl;
	splitVariable(phrase, tpl.name, tpl.type);
	templateParams.push_back(tpl);

	pData++;
}

bool ClassObject::ExtractClass(const char*& pData, attributes& attribs, ClassObjectRef& obj, const std::string& ns, bool handleTypeDefs)
{
	bool bUseTypeDef = false;

	bool is_variable = false;

	if(	IsWord(pData,"struct") || 
		IsWord(pData,"interface") || 
		IsWord(pData,"class") || 
		IsWord(pData,"template") || 
		IsWord(pData,"dispinterface") || 
		IsWord(pData,"exception") || 
		IsWord(pData,"enum") || 
		IsWord(pData,"union")
	  )
	{
		//continue if this is only a forward declarantion
		const char* curlyPos = strchr(&*pData,'{');
		const char* semicolonPos = strchr(&*pData,';');
		if(curlyPos == NULL || curlyPos > semicolonPos)
		{
			if(obj->GetContainer() == NULL || (obj->GetContainer()->type != ObjectClass && obj->GetContainer()->type != ObjectStruct))
			{
				EAT_PAST_SEMICOLON(pData)
				return true;
			}
			is_variable = true;
		}

		if(handleTypeDefs && ObjectHasTypeDefs(pData))
			bUseTypeDef = true;
	}

	if(bUseTypeDef || IfIsWordEat(pData,"typedef"))
		obj = ParseTypedef(pData, attribs, ns);

	else if(IfIsWordEat(pData,"library"))
	{
		if(isHashImport)
		{
			std::cerr << "encountered a library inside a #import\n";
			Library lib;
			lib.GetInterface(pData,ObjectLibrary,attribs, ns);
		}
		else
		{
			verboseStream << "library ";

			obj = GetInterface(pData,ObjectLibrary,attribs, ns);
			AddClass(obj);
		}

		verboseStream << "\n";
	}
	else if(IfIsWordEat(pData,"coclass"))
	{
		if(isHashImport)
			std::cerr << "encountered a coclass inside a #import\n";

		verboseStream << "coclass ";
		
		obj = GetInterface(pData,ObjectCoclass,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}

	else if(is_variable == false && IfIsWordEat(pData,"struct"))
	{
		verboseStream << "struct ";
		
		obj = GetInterface(pData,ObjectStruct,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}

	else if(IfIsWordEat(pData,"union"))
	{
		verboseStream << "union ";
		
		obj = GetInterface(pData,ObjectUnion,attribs, ns);
		AddClass(obj);

		//ParseUnion(pData, attribs);//not implemented
		verboseStream << "\n";
		return false;
	}

	else if(is_variable == false && IfIsWordEat(pData,"enum"))
	{
		verboseStream << "enum ";
		
		obj = GetInterface(pData,ObjectEnum,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}

	else if(IfIsWordEat(pData,"typedef sequence<"))
	{
		verboseStream << "typedef sequence<";
		
		obj = ParseSequence(pData, attribs, ns);
		AddClass(obj);
		
		verboseStream << "\n";
	}

	else if(IfIsWordEat(pData,"exception"))
	{
		verboseStream << "exception ";
		
		obj = GetInterface(pData,ObjectException,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}

	else if(IfIsWordEat(pData,"interface"))
	{
		verboseStream << "interface ";
		
		obj = GetInterface(pData,ObjectTypeInterface,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}

	else if(IfIsWordEat(pData,"class"))
	{
		verboseStream << "class ";
		
		obj = GetInterface(pData,ObjectClass,attribs, ns);
		AddClass(obj);

		verboseStream << "\n";
	}
	else if(IfIsWordEat(pData,"template"))
	{
		verboseStream << "template ";
		EAT_SPACES(pData)

		std::list<templateParam> templateParams;
		ExtractTemplate(pData, templateParams);

		EAT_SPACES(pData)

		if(IfIsWordEat(pData,"class"))
		{
			EAT_SPACES(pData)
			obj = GetInterface(pData,ObjectClass,attribs, ns);
			obj->m_templateParams = templateParams;
			AddClass(obj);
		}
		else
		{
			std::stringstream err;
			err << "Error expected 'class'";
			err << std::ends;
			std::string errString(err.str());
			throw errString;
		}
		verboseStream << "\n";
	}


	else if(IfIsWordEat(pData,"dispinterface"))
	{
		verboseStream << "dispinterface ";
		obj = GetInterface(pData,ObjectTypeDispInterface,attribs, ns);
		AddClass(obj);
		verboseStream << "\n";
	}
	else
		return false;

	EAT_SPACES(pData)
	return true;
}

/*bool ClassObject::LoadUsingEnv(const std::string& file)
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
						ClassObjectRef obj(new ClassObject(this,&GetLibrary(), std::string()));

						obj->name = W2CA(GetInterfaceName(typeInfo));

						TYPEATTR* pTypeAttr;
						HRESULT hr = typeInfo->GetTypeAttr(&pTypeAttr);

					
						if(memcmp(&pTypeAttr->guid, &GUID_NULL, sizeof(GUID)))
						{
							wchar_t buf[] = L"uuid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)   ";
							hr = StringFromGUID2(pTypeAttr->guid,&buf[4],40);
							if(SUCCEEDED(hr))
							{
								buf[4] = '(';
								buf[41] = ')';
								obj->m_attributes.push_back(W2CA(buf));
							}
						}

						{
							char buf[45];
							sprintf(buf,"lcid(%d)",pTypeAttr->lcid);
							obj->m_attributes.push_back(buf);
						}

						{
							char buf[45];
							sprintf(buf,"version(%d.%d)",pTypeAttr->wMajorVerNum,pTypeAttr->wMinorVerNum);
							obj->m_attributes.push_back(buf);
						}

						bool isDispatch = false;

						switch (typekind)
						{
							case TKIND_DISPATCH:
								{
									//we are a dispinterface
									isDispatch = true;

									obj->parentName = "IDispatch";
									obj->type = ObjectTypeInterface;

									//if we get past these two functions then we are a dual interface therefore treat as a custom interface
									HREFTYPE refType;
									hr = typeInfo->GetRefTypeOfImplType(-1, &refType);
									if (SUCCEEDED(hr))
									{
										ITypeInfoPtr dualTypeInfo;
										hr = typeInfo->GetRefTypeInfo(refType, &dualTypeInfo);
										if (SUCCEEDED(hr))
										{
											//we have now got the alter ego don't break but go onto the next block
											typeInfo = dualTypeInfo;
											obj->name = W2CA(GetInterfaceName(typeInfo));
										}
										else
										{
											GetInterfaceFunctions(pTypeAttr, obj, typeInfo);
											GetInterfaceProperties(pTypeAttr, obj, typeInfo);
											break;
										}
									}
									else
									{
										GetInterfaceFunctions(pTypeAttr, obj, typeInfo);
										GetInterfaceProperties(pTypeAttr, obj, typeInfo);
										break;
									}
								}
							case TKIND_INTERFACE:
								{
									obj->type = ObjectTypeInterface;
									
									if(!isDispatch)
										GetInterfaceFunctions(pTypeAttr, obj, typeInfo);

									//get the base class's name
									HREFTYPE hreftype;
									hr = typeInfo->GetRefTypeOfImplType(0, &hreftype);
									if (FAILED(hr))
										break;
							
									ITypeInfoPtr baseTypeInfo;
									hr = typeInfo->GetRefTypeInfo(hreftype, &baseTypeInfo);
									if (FAILED(hr))
										break;

									obj->parentName = W2CA(GetInterfaceName(baseTypeInfo));

								}

								break;


							case TKIND_MODULE:
								//we are a module
								//NOT IMPLEMENTED
								break;

							case TKIND_COCLASS:
								//we are a coclass
								obj->type = ObjectCoclass;
								addClass = false;
								//an interesting excercise but when we are loading a type library we don't need to load 
								//coclass's of other type libraries just their defined types such as unions and interfaces
//								GetCoclassInterfaces(pTypeAttr, obj, typeInfo);
								break; 

							case TKIND_ENUM:
								//we are a enum
								obj->type = ObjectEnum;
								GetVariables(obj, pTypeAttr->cVars, typeInfo);
								break;  

							case TKIND_ALIAS:
								obj->type = ObjectTypedef;
								obj->parentName = GenerateTypeString(pTypeAttr->tdescAlias, typeInfo);
								break; 

							case TKIND_RECORD:
								obj->type = ObjectStruct;
								GetVariables(obj, pTypeAttr->cVars, typeInfo);
								break;

							case TKIND_UNION:
								obj->type = ObjectUnion;
								GetVariables(obj, pTypeAttr->cVars, typeInfo);
								break; 

							default:       
								break;
						}

						typeInfo->ReleaseTypeAttr(pTypeAttr);
						if(addClass)
							AddClass(obj);
					}
				}
			}
		}
		else 
#endif
			if(Load(path.data()))
			return true;
		if(*includes == ';')
			includes++;
	}
	return false;
}*/

#ifdef USE_COM
void ClassObject::GetInterfaceProperties(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo)
{
	USES_CONVERSION;
    // Enumerate methods and return a collection of these.    
    for (unsigned int n=0; n<pTypeAttr->cVars; n++)
    {                
		LPVARDESC pvardesc = NULL;   
        HRESULT hr = typeInfo->GetVarDesc(n, &pvardesc);   
        if (FAILED(hr))
            break;

		FunctionObject fn(&obj,&obj.GetLibrary());
		fn.type = FunctionTypePropertyGet;

		char buf[256];
		sprintf(buf,"id(%d)",pvardesc->memid);
		fn.m_attributes.push_back(buf);

		fn.returnType = GenerateTypeString(pvardesc->elemdescVar.tdesc, typeInfo);

		CComBSTR name;
		unsigned int cNames = 0;
		hr = typeInfo->GetNames(pvardesc->memid, &name, 1, &cNames);    
		if(SUCCEEDED(hr))
			fn.name = W2CA(name.m_str);

if(fn.name == "root" && obj.name == "IXMLDocument")
	int x = 1;

		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FIN)
			fn.m_attributes.push_back("in");
		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FOUT)
			fn.m_attributes.push_back("out");
		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FLCID)
			fn.m_attributes.push_back("lcid");
		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
			fn.m_attributes.push_back("retval");
		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FLCID)
			fn.m_attributes.push_back("lcid");
		if(pvardesc->elemdescVar.paramdesc.wParamFlags & PARAMFLAG_FOPT)
			fn.m_attributes.push_back("optional");

		if(pvardesc->elemdescVar.paramdesc.wParamFlags & (PARAMFLAG_FOPT|PARAMFLAG_FHASDEFAULT))
		{
			CComVariant var(pvardesc->elemdescVar.paramdesc.pparamdescex->varDefaultValue);
			var.ChangeType(VT_BSTR);
			std::string defaultValue = "defaultvalue(\"";
			defaultValue += W2CA(var.bstrVal);
			defaultValue += "\")";
			fn.m_attributes.push_back(defaultValue);
		}

		if(pvardesc->varkind == VAR_PERINSTANCE)
			fn.m_attributes.push_back("VAR_PERINSTANCE");
		else if(pvardesc->varkind == VAR_STATIC)
			fn.m_attributes.push_back("VAR_STATIC");
		else if(pvardesc->varkind == VAR_CONST)
			fn.m_attributes.push_back("VAR_CONST");
		else if(pvardesc->varkind == VAR_DISPATCH)
			fn.m_attributes.push_back("VAR_DISPATCH");


		if(pvardesc->wVarFlags & VARFLAG_FSOURCE)
			fn.m_attributes.push_back("VARFLAG_FSOURCE");
		if(pvardesc->wVarFlags & VARFLAG_FBINDABLE)
			fn.m_attributes.push_back("bindable");
		if(pvardesc->wVarFlags & VARFLAG_FREQUESTEDIT)
			fn.m_attributes.push_back("requestedit");
		if(pvardesc->wVarFlags & VARFLAG_FDISPLAYBIND)
			fn.m_attributes.push_back("displaybind");
		if(pvardesc->wVarFlags & VARFLAG_FDEFAULTBIND)
			fn.m_attributes.push_back("defaultbind");
		if(pvardesc->wVarFlags & VARFLAG_FHIDDEN)
			fn.m_attributes.push_back("hidden");
		if(pvardesc->wVarFlags & VARFLAG_FRESTRICTED)
			fn.m_attributes.push_back("restricted");
		if(pvardesc->wVarFlags & VARFLAG_FDEFAULTCOLLELEM)
			fn.m_attributes.push_back("defaultcollelem");
		if(pvardesc->wVarFlags & VARFLAG_FUIDEFAULT)
			fn.m_attributes.push_back("uidefault");
		if(pvardesc->wVarFlags & VARFLAG_FNONBROWSABLE)
			fn.m_attributes.push_back("nonbrowsable");
		if(pvardesc->wVarFlags & VARFLAG_FREPLACEABLE)
			fn.m_attributes.push_back("replaceable");
		if(pvardesc->wVarFlags & VARFLAG_FIMMEDIATEBIND)
			fn.m_attributes.push_back("immediatebind");

		obj.functions.push_back(fn);

		if(!(pvardesc->wVarFlags & VARFLAG_FREADONLY))
		{
			fn.type = FunctionTypePropertyPut;

			obj.functions.push_back(fn);
		}

		typeInfo->ReleaseVarDesc(pvardesc); 
        pvardesc = NULL;
    }
}

void ClassObject::GetInterfaceFunctions(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo)
{
	USES_CONVERSION;
    for (unsigned int n=0; n<pTypeAttr->cFuncs; n++)
    {                
		FUNCDESC* desc = NULL;
	    HRESULT hr = typeInfo->GetFuncDesc(n, &desc);   
		if(SUCCEEDED(hr))
		{
			FunctionObject fn(&obj,&obj.GetLibrary());
			
			CComBSTR name;
		    HRESULT hr = typeInfo->GetDocumentation(desc->memid, &name, NULL,NULL, NULL); 
			if(SUCCEEDED(hr))
				fn.name = W2CA(name.m_str);

			char buf[256];
			sprintf(buf,"id(%d)",desc->memid);
			fn.m_attributes.push_back(buf);

			switch(desc->invkind)
			{
			case INVOKE_FUNC:
				fn.type = FunctionTypeMethod;
				break;
			case INVOKE_PROPERTYGET:
				fn.type = FunctionTypePropertyGet;
				fn.m_attributes.push_back("propget");
				break;
			case INVOKE_PROPERTYPUT:
				fn.type = FunctionTypePropertyPut;
				fn.m_attributes.push_back("propput");
				break;
			case INVOKE_PROPERTYPUTREF:
				fn.type = FunctionTypePropertyPut;
				fn.m_attributes.push_back("propputref");
				break;
			default:
				return;
			}
	
			fn.returnType = GenerateTypeString(desc->elemdescFunc.tdesc, typeInfo);
	
			if(desc->wFuncFlags & FUNCFLAG_FRESTRICTED)
				fn.m_attributes.push_back("restricted");
			if(desc->wFuncFlags & FUNCFLAG_FSOURCE)
				fn.m_attributes.push_back("FUNCFLAG_FSOURCE");
			if(desc->wFuncFlags & FUNCFLAG_FBINDABLE)
				fn.m_attributes.push_back("bindable");
			if(desc->wFuncFlags & FUNCFLAG_FREQUESTEDIT)
				fn.m_attributes.push_back("requestedit");
			if(desc->wFuncFlags & FUNCFLAG_FDISPLAYBIND)
				fn.m_attributes.push_back("displaybind");
			if(desc->wFuncFlags & FUNCFLAG_FDEFAULTBIND)
				fn.m_attributes.push_back("defaultbind");
			if(desc->wFuncFlags & FUNCFLAG_FHIDDEN)
				fn.m_attributes.push_back("hidden");
			if(desc->wFuncFlags & FUNCFLAG_FUSESGETLASTERROR)
				fn.m_attributes.push_back("FUNCFLAG_FUSESGETLASTERROR");
			if(desc->wFuncFlags & FUNCFLAG_FDEFAULTCOLLELEM)
				fn.m_attributes.push_back("defaultcollelem");
			if(desc->wFuncFlags & FUNCFLAG_FUIDEFAULT)
				fn.m_attributes.push_back("uidefault");
			if(desc->wFuncFlags & FUNCFLAG_FNONBROWSABLE)
				fn.m_attributes.push_back("nonbrowsable");
			if(desc->wFuncFlags & FUNCFLAG_FREPLACEABLE)
				fn.m_attributes.push_back("replaceable");
			if(desc->wFuncFlags & FUNCFLAG_FIMMEDIATEBIND)
				fn.m_attributes.push_back("immediatebind");
			
			BSTR* rgbstrNames = (BSTR*) _alloca(sizeof(BSTR) * (desc->cParams + 1));
			memset(rgbstrNames,0,sizeof(BSTR*)*(desc->cParams+1));

			unsigned int pcNames = 0;
			hr = typeInfo->GetNames(desc->memid,rgbstrNames,desc->cParams+1,&pcNames);
			if(SUCCEEDED(hr))
			{
				for(int p = 1; p < desc->cParams;p++)
				{
					ParameterObject param(&obj,&obj.GetLibrary());
					param.name = W2CA(rgbstrNames[p]);
					param.type = GenerateTypeString(desc->lprgelemdescParam[p].tdesc, typeInfo);

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FIN)
						param.m_attributes.push_back("in");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FOUT)
						param.m_attributes.push_back("out");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FLCID)
						param.m_attributes.push_back("lcid");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
						param.m_attributes.push_back("retval");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FLCID)
						param.m_attributes.push_back("lcid");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & PARAMFLAG_FOPT)
						param.m_attributes.push_back("optional");

					if(desc->lprgelemdescParam[p].paramdesc.wParamFlags & (PARAMFLAG_FOPT|PARAMFLAG_FHASDEFAULT))
					{
						CComVariant var(desc->lprgelemdescParam[p].paramdesc.pparamdescex->varDefaultValue);
						var.ChangeType(VT_BSTR);
						std::string defaultValue = "defaultvalue(\"";
						defaultValue += W2CA(var.bstrVal);
						defaultValue += "\")";
						fn.m_attributes.push_back(defaultValue);
					}
					fn.parameters.push_back(param);
				}
			}

			for(int p = 0; p < desc->cParams;p++)
				SysFreeString(rgbstrNames[p]);

			typeInfo->ReleaseFuncDesc(desc);
		
			obj.functions.push_back(fn);
		}
    }
}

void ClassObject::GetCoclassInterfaces(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo)
{
	USES_CONVERSION;
	// Enumerate interfaces/dispinterfaces in coclass and return a collection of these.
	for (unsigned int n=0; n<pTypeAttr->cImplTypes; n++)
	{       
		FunctionObject fn(&obj,&obj.GetLibrary());

		int flags = 0;
		HRESULT hr = typeInfo->GetImplTypeFlags(n,&flags);
		if(flags & IMPLTYPEFLAG_FDEFAULT)
			fn.m_attributes.push_back("default");
		if(flags & IMPLTYPEFLAG_FSOURCE)
			fn.m_attributes.push_back("source");
		if(flags & IMPLTYPEFLAG_FRESTRICTED)
			fn.m_attributes.push_back("restricted");
		if(flags & IMPLTYPEFLAG_FDEFAULTVTABLE)
			fn.m_attributes.push_back("defaultvtable");

		HREFTYPE hreftype;
		hr = typeInfo->GetRefTypeOfImplType(n, &hreftype);  
		if(SUCCEEDED(hr))
		{
			ITypeInfoPtr ifTypeInfo;
			hr = typeInfo->GetRefTypeInfo(hreftype, &ifTypeInfo);   
			if(SUCCEEDED(hr))
				fn.name = W2CA(GetInterfaceName(ifTypeInfo));
		}
		obj.functions.push_back(fn);
	}
}

void ClassObject::GetVariables(ClassObject& theClass, unsigned variableCount, ITypeInfo* typeInfo)
{
	USES_CONVERSION;
	for(unsigned int n=0; n<variableCount; n++)
	{
		LPVARDESC pvardesc = NULL;   
		HRESULT hr = typeInfo->GetVarDesc(n, &pvardesc);   
		if (SUCCEEDED(hr))
		{
			FunctionObject fn(&theClass,&theClass.GetLibrary());
			char buf[256];
			sprintf(buf,"id(%d)",pvardesc->memid);
			fn.m_attributes.push_back(buf);

			CComBSTR name;
			unsigned int cNames = 0; //not used
			typeInfo->GetNames(pvardesc->memid, &name, 1, &cNames);   
			fn.name = W2CA(name);

			fn.returnType = GenerateTypeString(pvardesc->elemdescVar.tdesc, typeInfo);
		
			typeInfo->ReleaseVarDesc(pvardesc); 
			
			theClass.functions.push_back(fn);
		}
	}
}								


std::string ClassObject::GenerateTypeString(TYPEDESC& typedesc, ITypeInfo* typeInfo)
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
		for(int i = 0; i < typedesc.lpadesc->cDims;i++)
		{
			SAFEARRAYBOUND& arrayBound = typedesc.lpadesc->rgbounds[i];
			char buf[20];
			sprintf(buf,"[%d..%d]",arrayBound.lLbound, arrayBound.lLbound + arrayBound.cElements - 1);
			ret += buf;
		}
		return ret;
	}	
	if (typedesc.vt == VT_PTR || typedesc.vt == VT_SAFEARRAY)
		return std::string(GenerateTypeString(*typedesc.lptdesc, typeInfo) + "*");

	std::string ret;
	switch((typedesc.vt ^ VT_ARRAY) & (typedesc.vt ^ VT_BYREF))
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
	if(typedesc.vt & VT_ARRAY)
		ret += "[]";
	if(typedesc.vt & VT_BYREF)
		ret += "*";
	return ret;
}

CComBSTR ClassObject::GetInterfaceName(ITypeInfo* typeInfo)
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

void ClassObject::ParseAndLoad(const char*& pData, const char* file)
{
	EAT_SPACES(pData)

	assert(*pData == '\"' || *pData == '<');

	pData++;

	char path[1024];
	std::string temp;
	while(*pData)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '\"' || *pData == '>')
		{
			pData++;


			const char* fname_ext = std::max(strrchr( temp.data(), '\\'), strrchr( temp.data(), '/'));
			if(fname_ext != NULL)
			{
				if(!Load(temp.data()))
//					if(!LoadUsingEnv(temp))
						std::cerr << "failed to load " << temp << "\n";
			}
			else
			{
				const char* fname_ext = std::max(strrchr( file, '\\'), strrchr( file, '/'));
				if(fname_ext != NULL)
				{
					strncpy(path, file, fname_ext - file + 1);
					strcat(path, temp.data());
				}
				else
				{
					strcpy(path, temp.data());
				}
				if(!Load(path))
//					if(!LoadUsingEnv(temp))
						std::cerr << "failed to load " << path << "\n";
			}
			return;
		}

		temp += *pData++;
	}
}
void MovePastComments(const char*& pData)
{
	int count = 0;
	while(*pData != NULL)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(*pData == '\"')
		{
			count++;
			pData++;
			if(count == 2)
				break;
		}
		else
			pData++;
	}
}

bool ClassObject::GetFileData(const char*& pData, const char* file)
{
	if(IfIsWordEat(pData,"#include"))
	{
		if(recurseImportLib)
			ParseAndLoad(pData, file);
		else
			MovePastComments(pData);
		return true;
	}
	if(IfIsWordEat(pData,"importlib"))
	{
		isHashImport++;
		pData++; //get past (
		EAT_SPACES(pData)
		if(recurseImportLib)
			ParseAndLoad(pData, file);
		else
			MovePastComments(pData);
		EAT_SPACES(pData)
		pData++; //get past )
		EAT_SPACES(pData)
		assert(*pData == ';');
		if(*pData == ';')
			pData++;
		isHashImport--;
		return true;
	}
	if(IfIsWordEat(pData,"import" ))
	{
		isHashImport++;
		if(recurseImport)
			ParseAndLoad(pData, file);
		else
			MovePastComments(pData);
		EAT_SPACES(pData)
		assert(*pData == ';');
		if(*pData == ';')
			pData++;
		isHashImport--;
		return true;
	}
	return false;
}

bool ClassObject::Load(const char* file)
{
	if(loadedFiles.find(file) != loadedFiles.end())
		return true;

	loadedFiles.insert(file);
	std::error_code ec;
	std::string path(std::filesystem::canonical(file, ec).string());


	std::ifstream preproc_stream(path);
	if(preproc_stream.is_open() == false)
	{
		return 0;
	}
	std::string preproc_data = std::string(std::istreambuf_iterator<char>(preproc_stream), std::istreambuf_iterator<char>());

	const char* tmp = preproc_data.data();
	GetStructure(tmp, std::string(), true);
		
	return true;
}

std::set<std::string> loadedFiles;
static int isHashImport = 0;