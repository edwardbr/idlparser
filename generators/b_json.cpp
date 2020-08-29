#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include "b_json.h"

namespace blocking
{
	namespace json
	{

		set<std::string> json_structures;
		set<std::string> source_structures;
		set<std::string> header_structures;
		set<std::string> structures_check;
		set<std::string> json_structures_check;
		std::string wsdl_namespace;

		size_t findKeyWord(const std::string& type, const char* val, bool check_end)
		{
			int len = strlen(val);
			bool requires_loop = false;

			const char* pos = strstr(type.data(), val);
			
			if(pos == NULL)
			{
				return (size_t)-1;
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
			return pos - type.data();;
		}

		bool isInt(const std::string& type)
		{
			return findKeyWord(type, "int", true) != -1;
		}
		bool isSize_t(const std::string& type)
		{
			return findKeyWord(type, "size_t", true) != -1;
		}
		bool isLong(const std::string& type)
		{
			return findKeyWord(type, "long", true) != -1;
		}
		bool isBool(const std::string& type)
		{
			return findKeyWord(type, "bool", true) != -1;
		}
		bool isDouble(const std::string& type)
		{
			return findKeyWord(type, "double", true) != -1;
		}
		/*bool isDate(const std::string& type)
		{
			return findKeyWord(type, "int", true) != -1;
		}*/
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

		bool readTypeAtomic(ostream& cos, std::string& paramType, const char* paramName)
		{
			if(isInt(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readInt(soap);" << std::endl;
			}
			else if(isSize_t(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readSize_t(soap);" << std::endl;
			}
			else if(isLong(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readLong(soap);" << std::endl;
			}
			else if(isBool(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readBool(soap);" << std::endl;
			}
			else if(isDouble(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readDouble(soap);" << std::endl;
			}
		/*				else if(isDate(paramType) == true)
			{
				cos << "\t\t" << paramType << " " << paramName << " = readDate(soap);" << std::endl;
			}*/
			else if(isCharStar(paramType) == true)
			{
				cos << "\t\t" << "std::string " << paramName << "(get_string(soap));" << std::endl;
			}
			else if(isString(paramType) == true)
			{
				cos << "\t\t" << "std::string " << paramName << "(get_string(soap));" << std::endl;
			}
			else
			{
				return false;
			}
			return true;
		}

		void stripReferenceModifiers(std::string& paramType, std::string& referenceModifiers)
		{
			int i = paramType.length() - 1;
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


		//strip out constness
		std::string unconst(const std::string& type)
		{
			std::string temp = type;
			const char* searchString = "const";
			int pos = findKeyWord(temp, searchString, true);
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

			/*	searchString = "enum";
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
			}*/

			return temp;
		}
		//strip out constness
		std::string unenum(const std::string& type)
		{
			std::string temp = type;
			const char* searchString = "enum";
			int pos = findKeyWord(temp, searchString, true);
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

		void buildStructureMembers(const ClassObject* pObj, std::stringstream& cos, std::stringstream& jos)
		{
			if(pObj->parentName != "")
			{
				const ClassObject* pParent = NULL;
				if(pObj->GetLibrary().FindClassObject(pObj->parentName, pParent) == false || pParent == NULL)
				{
					std::stringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				buildStructureMembers(pParent, cos, jos);
			}
			for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
			{
				if(fit->type != FunctionTypeVariable)
					continue;

				if(fit != pObj->functions.begin() || pObj->parentName != "")
				{
					cos << "\tget(soap, \",\");" << std::endl;
					jos << "," << std::endl;
				}
		//		if(fit->type == FunctionTypeVariable)
				{
					cos << "\tget_label(soap, \"" << fit->name << "\");" << std::endl;
					cos << "\t{" << std::endl;

					std::string unconst_type = unconst(fit->returnType);
					std::string paramType = unconst_type;

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, pObj->GetLibrary());

					jos << "\t\t";
					if(isInt(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readInt(soap);" << std::endl;
						jos << "0";
					}
					else if(isSize_t(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readSize_t(soap);" << std::endl;
						jos << "0";
					}
					else if(isLong(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readLong(soap);" << std::endl;
						jos << "0";
					}
					else if(isBool(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readBool(soap);" << std::endl;
						jos << "true";
					}
					else if(isDouble(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readDouble(soap);" << std::endl;
						jos << "0.0";
					}
		/*				else if(isDate(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = readDate(soap);" << std::endl;
					}*/
					else if(isCharStar(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = get_string(soap);" << std::endl;
						jos << "\"\"";
					}
					else if(isString(paramType) == true)
					{
						cos << "\t\tobject." << fit->name << " = get_string(soap);" << std::endl;
						jos << "\"\"";
					}
					else if(isVector(paramType) == true)
					{
						cos << "\t\t" << buildVector(paramType, pObj->GetLibrary()) << "(soap, object." << fit->name << ");" << std::endl;
						jos << "[]";
					}
					else if(isList(paramType) == true)
					{
						cos << "\t\t" << buildList(paramType, pObj->GetLibrary()) << "(soap, object." << fit->name << ");" << std::endl;
						jos << "[]";
					}
					else if(isMap(paramType) == true)
					{
						cos << "\t\t" << buildMap(paramType, pObj->GetLibrary()) << "(soap, object." << fit->name << ");" << std::endl;
						jos << "[]";
					}
					else if(isSet(paramType) == true)
					{
						cos << "\t\t" << buildSet(paramType, pObj->GetLibrary()) << "(soap, object." << fit->name << ");" << std::endl;
						jos << "[]";
					}
					else if(isEnum(paramType, pObj->GetLibrary()) == true)
					{
						cos << "\t\tobject." << fit->name << " = " << buildEnum(paramType, pObj->GetLibrary()) << "(soap);" << std::endl;

						std::string enumName = unenum(paramType);
						const ClassObject* pTmp = NULL;
						if(pObj->GetLibrary().FindClassObject(enumName, pTmp) == false || pTmp == NULL)
						{
							std::stringstream err;
							err << "unable to find type: " << enumName;
							err << ends;
							string errString(err.str());
							throw errString;
						}
						std::list<FunctionObject>::const_iterator fit = pTmp->functions.begin();
						if(fit != pTmp->functions.end())
						{
							jos << "clarinet." << enumName << "." << fit->name;
						}
						else
						{
							jos << "\"\"";
						}
					}
					else
					{
						cos << "\t\t" << buildStructure(paramType, pObj->GetLibrary()) << "(soap, object." << fit->name  << ");" << std::endl;
						jos << paramType << "()";
					}

					cos << "\t}" << std::endl;
				}
			}
		}

		std::string buildStructure(std::string& paramType, const Library& library)
		{
			std::string type = unconst(paramType);

			std::string functionName;
			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			string suffix;
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				suffix = "_ptr";
			}

			functionName += "read_" + dereftype + suffix;

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << "& object);";

			if(header_structures.find(hos.str()) != header_structures.end())
			{
				return functionName;
			}
			header_structures.insert(hos.str());

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(type, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			std::stringstream cos;
			cos << "void read_" << type << "(struct soap *soap, " << type << "& object)" << std::endl;
			cos << "{" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(isNull(soap) == true)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\tcollection = NULL;" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\tget(soap, \"{\");" << std::endl;

			std::stringstream jos;
			jos << "clarinet.create_" << type << " = function()" << std::endl;
			jos << "{" << std::endl;
			jos << "\tvar tmp = " << std::endl;
			jos << "\t{" << std::endl;

			buildStructureMembers(pObj, cos, jos);

			jos << std::endl << "\t};" << std::endl;
			jos << "\treturn tmp;" << std::endl;
			jos << "};" << std::endl;

			json_structures.insert(jos.str());

			cos << "\tget(soap, \"}\");" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			return functionName;
		}

		std::string getTemplateTypeAlias(const std::string& templateType)
		{
			if(templateType.substr(0,5) == "std::")
			{
				std::string lead;
				lead += templateType[5];
				_strupr((char*)lead.data());
				return lead + templateType.substr(6);
			}
			return templateType;
		}

		std::string buildVector(const std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("readVector_") + templateTypeAlias + (pointer?"_ptr":"_ref");

			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				functionName += "_ptr";
			}
			
			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tbool first_pass = true;" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(isNull(soap) == true)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\tcollection = NULL;" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\tget(soap,\"[\");" << std::endl;
			cos << "\twhile(peek_get(soap, \"]\") == false)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(first_pass == true)" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tfirst_pass = false;" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t\telse" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tget(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			if(pointer == true)
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << "* var = soap_new_" << unconst_type << "(soap, -1);" << std::endl;
					cos << "\t\tcollection" << dereferencer << "push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, *var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection" << dereferencer << "push_back(var);" << std::endl;
				}
				
			}
			else
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << " var;" << std::endl;
					cos << "\t\tcollection" << dereferencer << "push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection" << dereferencer << "push_back(var);" << std::endl;
				}
			}
			cos << "\t}" << std::endl;
			cos << "}" << std::endl;


			source_structures.insert(cos.str());

			return functionName;
		}

		std::string buildList(const std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("readList_") + templateTypeAlias + (pointer?"_ptr":"");

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type  << "& collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream cos;
			cos << "void readList_" << templateTypeAlias << "(struct soap *soap, " << type  << "& collection)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tbool first_pass = true;" << std::endl;
			cos << "\tget(soap,\"[\");" << std::endl;
			cos << "\twhile(peek_get(soap, \"]\") == false)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(first_pass == true)" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tfirst_pass = false;" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t\telse" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tget(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			if(pointer == true)
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << "* var = soap_new_" << unconst_type << "(soap, -1);" << std::endl;
					cos << "\t\tcollection.push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, *var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.push_back(var);" << std::endl;
				}
				
			}
			else
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << " var;" << std::endl;
					cos << "\t\tcollection.push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.push_back(var);" << std::endl;
				}
			}
			cos << "\t}" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			return functionName;
		}

		std::string buildMap(const std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("readMap_") + templateTypeAlias + (pointer?"_ptr":"");

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type  << "& collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream cos;
			cos << "void readMap_" << templateTypeAlias << "(struct soap *soap, " << type  << "& collection)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tbool first_pass = true;" << std::endl;
			cos << "\tget(soap,\"[\");" << std::endl;
			cos << "\twhile(peek_get(soap, \"]\") == false)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(first_pass == true)" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tfirst_pass = false;" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t\telse" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tget(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			if(pointer == true)
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << "* var = soap_new_" << unconst_type << "(soap, -1);" << std::endl;
					cos << "\t\tcollection.push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, *var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.push_back(var);" << std::endl;
				}
			}
			else
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << " var;" << std::endl;
					cos << "\t\tcollection.push_back(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.push_back(var);" << std::endl;
				}
			}
			cos << "\t}" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			return functionName;
		}

		std::string buildSet(const std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("readSet_") + templateTypeAlias + (pointer?"_ptr":"");

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type  << "& collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type  << "& collection)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tbool first_pass = true;" << std::endl;
			cos << "\tget(soap,\"[\");" << std::endl;
			cos << "\twhile(peek_get(soap, \"]\") == false)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(first_pass == true)" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tfirst_pass = false;" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t\telse" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tget(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			if(pointer == true)
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << "* var = soap_new_" << unconst_type << "(soap, -1);" << std::endl;
					cos << "\t\tcollection.insert(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, *var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.insert(var);" << std::endl;
				}
				
			}
			else
			{
				if(readTypeAtomic(cos, templateType, "var") == false)
				{
					std::string unconst_type = unconst(templateType);
					cos << "\t\t" << unconst_type << " var;" << std::endl;
					cos << "\t\tcollection.insert(var);" << std::endl;
					cos << "\t\t" << buildStructure(unconst_type, library) << "(soap, var);" << std::endl;
				}
				else
				{
					cos << "\t\tcollection.insert(var);" << std::endl;
				}
			}
			cos << "\t}" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			return functionName;
		}

		std::string buildEnum(const std::string& type, const Library& library)
		{
			std::string templateType = type.substr(type.find_last_of(' ') + 1);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("readEnum_") + templateTypeAlias + (pointer?"_ptr":"");

			std::stringstream hos;
			hos << templateType << " " << functionName << "(struct soap *soap);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(templateType, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}


			header_structures.insert(hos.str());

			std::stringstream cos;
			cos << templateType << " " << functionName << "(struct soap *soap)" << std::endl;
			cos << "{" << std::endl;
			cos << "\t" << templateType << " __tmp = (" << templateType << ")readLong(soap);" << std::endl;
			cos << "\treturn __tmp;" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			buildJSONEnum(pObj, templateType);

			return functionName;
		}

		void writeJSONFiles(const ClassObject& m_ob, ostream& hos, ostream& cos, ostream& jos, const char* gsoap_namespace, const std::string& header)
		{
			wsdl_namespace = gsoap_namespace;
			std::stringstream tmpjos;//throw away

			hos << "int callJSONFunc(struct soap *soap, const char* function);" << std::endl << std::endl;
			
			cos << "#include \"stdafx.h\"" << std::endl;
			cos << "#include \"json.h\"" << std::endl;
			cos << "#include \"" << header << "\"" << std::endl;
		//	cos << "#include \"interfaces.h\"" << std::endl;

			jos << "var clarinet = [];" << std::endl;
			jos << "clarinet.session = {" << std::endl;
			jos << "\tinitialize: function(logging_call_back) {" << std::endl;
			jos << "\t\tthis.m_logging_call_back = logging_call_back;" << std::endl;
			jos << "\t}," << std::endl;
			jos << std::endl;
			jos << "\tjson_call: function(context, callback, method, soapMess) {" << std::endl;
			jos << "\t\tvar http_request;" << std::endl;
			jos << "\t\tif (window.XMLHttpRequest) { // Mozilla, Safari, ..." << std::endl;
			jos << "\t\t\thttp_request = new XMLHttpRequest();" << std::endl;
			jos << "\t\t}" << std::endl;
			jos << "\t\telse if (window.ActiveXObject) { // IE" << std::endl;
			jos << "\t\t\thttp_request = new ActiveXObject(\"Microsoft.XMLHTTP\");" << std::endl;
			jos << "\t\t}" << std::endl;
			jos << "\t\telse {" << std::endl;
			jos << "\t\t\thttp_request.callback(context, null, null, \"Your browser does not or is configured not to support ajax\");" << std::endl;
			jos << "\t\t\treturn null;" << std::endl;
			jos << "\t\t}" << std::endl;
			jos << std::endl;
			jos << "\t\tvar This = this;" << std::endl;
			jos << std::endl;
			jos << "\t\thttp_request.context = context;" << std::endl;
			jos << "\t\thttp_request.callback = callback;" << std::endl;
			jos << "\t\thttp_request.onreadystatechange = function() { This.json_callback(http_request); };" << std::endl;
			jos << "\t\thttp_request.open(\"POST\", \"/\", true);" << std::endl;
			jos << "\t\thttp_request.setRequestHeader(\"function\", method);" << std::endl;
			jos << "\t\tvar tmp = JSON.stringify(soapMess);" << std::endl;
			jos << "\t\thttp_request.send(tmp);" << std::endl;
			jos << "\t\treturn http_request;" << std::endl;
			jos << "\t}," << std::endl;
			jos << std::endl;
			jos << "\t// this function will be called when result return from web service." << std::endl;
			jos << "\tjson_callback: function(http_request) {" << std::endl;
			jos << "\t\t// return value from web service is an xml document." << std::endl;
			jos << "\t\tvar rawData;" << std::endl;
			jos << "\t\tif (http_request.readyState == 4) {" << std::endl;
			jos << "\t\t\tif (http_request.status == 200) {" << std::endl;
			jos << "\t\t\t\tvar data = null;" << std::endl;
			jos << "\t\t\t\ttry {" << std::endl;
			jos << "\t\t\t\t\tdata = eval(\"(\" + http_request.responseText + \")\");" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\tcatch (e) {" << std::endl;
			jos << "\t\t\t\t\tif (this.m_logging_call_back === null) {" << std::endl;
			jos << "\t\t\t\t\t\tthis.m_logging_call_back.addMessage(\"Error: Decoding callback \" + e);" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\thttp_request.callback(http_request.context, data, http_request, \"Error: Decoding callback\");" << std::endl;
			jos << "\t\t\t\t\treturn;" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\thttp_request.callback(http_request.context, data, http_request, http_request.statusText);" << std::endl;
			jos << "\t\t\t\tthis.print_response(data.resp);" << std::endl;
			jos << "\t\t\t}" << std::endl;
			jos << "\t\t\telse {" << std::endl;
			jos << "\t\t\t\tif (this.m_logging_call_back === null) {" << std::endl;
			jos << "\t\t\t\t\tthis.m_logging_call_back.addMessage(\"Error: No data returned\");" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\thttp_request.callback(http_request.context, null, http_request, \"Error: No data returned\");" << std::endl;
			jos << "\t\t\t}" << std::endl;
			jos << "\t\t}" << std::endl;
			jos << "\t}," << std::endl;
			jos << std::endl;
			jos << "\tprint_response: function(response) {" << std::endl;
			jos << "\t\tif (this.m_logging_call_back === null) {" << std::endl;
			jos << "\t\t\tif (response.messages === null) {" << std::endl;
			jos << "\t\t\t\tfor (i = 0; i < response.messages.length; i++) {" << std::endl;
			jos << "\t\t\t\t\tvar message = response.messages[i];" << std::endl;
			jos << "\t\t\t\t\tif (message.level != clarinet.log_level.log_lock_trace) {" << std::endl;
			jos << "\t\t\t\t\t\tthis.m_logging_call_back.addMessage(clarinet.log_level[message.level] + \": \" + message.message);" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t}" << std::endl;
			jos << "\t\t\telse {" << std::endl;
			jos << "\t\t\t\tthis.m_logging_call_back.addMessage(\"log_info: No messages\");" << std::endl;
			jos << "\t\t\t}" << std::endl;
			jos << "\t\t}" << std::endl;
			jos << "\t}";//no endl here

			cos << "int callJSONFunc(struct soap *soap, const char* function)" << std::endl;
			cos << "{" << std::endl;
			for(std::list<FunctionObject>::const_iterator fit = m_ob.functions.begin();fit != m_ob.functions.end();fit++)
			{
				if(fit->type == FunctionTypeMethod)
				{
					std::string json_func_param_list;
					std::string json_func_structure;


					cos << "\t";
					if(fit != m_ob.functions.begin())
					{
						cos << "else ";
					}

					cos << "if(strcmp(function, \"" << fit->name << "\") == 0)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tget(soap, \"{\");" << std::endl;

					std::string param_list;
					for(list<ParameterObject>::const_iterator pit = fit->parameters.begin();pit != fit->parameters.end();pit++)
					{
						bool is_last = false;

						{
							std::list<ParameterObject>::const_iterator temp = pit;
							temp++;
							if(temp == fit->parameters.end())
							{
								is_last = true;
							}
						}

						if(param_list.length() != 0)
						{
							param_list += ", ";
							if(is_last == false)
							{
								cos << "\t\tget(soap, \",\");" << std::endl;
							}
						}
						if(is_last == false)
						{
							if(param_list.length() != 0)
							{
								json_func_structure += ",";
							}
							json_func_param_list += ", ";
							json_func_param_list += pit->name;
							json_func_structure += pit->name;

							cos << "\t\tget_label(soap, \"" << pit->name << "\");" << std::endl;
						}

						std::string unconst_type = unconst(pit->type);
						std::string paramType = unconst_type;

						//remove reference modifiers from paramType
						std::string referenceModifiers;
						stripReferenceModifiers(paramType, referenceModifiers);
						translateType(paramType, m_ob.GetLibrary());
						paramType = unconst(paramType);
						std::string modified_name = pit->name;
						if(referenceModifiers == "*")
						{
							modified_name = std::string("&") + modified_name;
						}


						if(isInt(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readInt(soap);") << std::endl;
						}
						else if(isSize_t(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readSize_t(soap);") << std::endl;
						}
						else if(isLong(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readLong(soap);") << std::endl;
						}
						else if(isBool(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readBool(soap);") << std::endl;
						}
						else if(isDouble(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readDouble(soap);") << std::endl;
						}
		/*				else if(isDate(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : " = readDate(soap);") << std::endl;
						}*/
						else if(isCharStar(paramType) == true)
						{
							modified_name += ".data()";
							cos << "\t\t" << "std::string " << pit->name << (is_last ? ";" : "(get_string(soap));") << std::endl;
						}
						else if(isString(paramType) == true)
						{
							cos << "\t\t" << "std::string " << pit->name << (is_last ? ";" : "(get_string(soap));") << std::endl;
						}
						else if(isVector(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << ";" << std::endl;
							std::string class_name = buildVector(paramType, m_ob.GetLibrary());
							if(is_last == false)
							{
								cos << "\t\t" << class_name << "(soap, " << pit->name << ");" << std::endl;
							}
						}
						else if(isList(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << ";" << std::endl;
							std::string class_name = buildList(paramType, m_ob.GetLibrary());
							if(is_last == false)
							{
								cos << "\t\t" << class_name << "(soap, " << pit->name << ");" << std::endl;
							}
						}
						else if(isMap(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << ";" << std::endl;
							std::string class_name = buildMap(paramType, m_ob.GetLibrary());
							if(is_last == false)
							{
								cos << "\t\t" << class_name << "(soap, " << pit->name << ");" << std::endl;
							}
						}
						else if(isSet(paramType) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << ";" << std::endl;
							std::string class_name = buildSet(paramType, m_ob.GetLibrary());
							if(is_last == false)
							{
								cos << "\t\t" << class_name << "(soap, " << pit->name << ");" << std::endl;
							}
						}
						else if(isEnum(paramType, m_ob.GetLibrary()) == true)
						{
							cos << "\t\t" << paramType << " " << pit->name << (is_last ? ";" : std::string(" = ") + buildEnum(unconst_type, m_ob.GetLibrary()) + "(soap);") << std::endl;
						}
						else
						{
							cos << "\t\t" << paramType << " " << pit->name << ";" << std::endl;
							if(is_last == false)
							{
								cos << "\t\t" << buildStructure(paramType, m_ob.GetLibrary()) << "(soap, " << pit->name  << ");" << std::endl;
							}
						}
						param_list += modified_name;
					}
					cos << "\t\tget(soap, \"}\");" << std::endl;
					
					cos << "\t\tint ret = " << fit->name << "(soap, " << param_list << ");" << std::endl;
					cos << "\t\tif(ret != SOAP_OK)" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << "\t\t\treturn ret;" << std::endl;
					cos << "\t\t}" << std::endl;
					cos << "\t\tret = soap_response(soap, SOAP_HTML);" << std::endl;
					cos << "\t\tif(ret != SOAP_OK)" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << "\t\t\treturn ret;" << std::endl;
					cos << "\t\t}" << std::endl;

					
					for(list<ParameterObject>::const_iterator pit = fit->parameters.begin();pit != fit->parameters.end();pit++)
					{
						{
							std::list<ParameterObject>::const_iterator temp = pit;
							temp++;
							if(temp != fit->parameters.end())
							{
								continue;
							}
						}

						std::string unconst_type = unconst(pit->type);
						std::string paramType = unconst_type;

						//remove reference modifiers from paramType
						std::string referenceModifiers;
						stripReferenceModifiers(paramType, referenceModifiers);
						translateType(paramType, m_ob.GetLibrary());
						std::string modified_name = pit->name;

						cos << "\t";
						writeType(cos, tmpjos, paramType, pit->name.data(), m_ob.GetLibrary());
					}
					cos << "\t\treturn soap_end_send(soap);" << std::endl;

					jos << "," << std::endl;

					std::string json_func_name = fit->name;
					if(strncmp(fit->name.data(), gsoap_namespace, strlen(gsoap_namespace)) == 0)
					{
						json_func_name = fit->name.substr(strlen(gsoap_namespace));
					}
					jos << "\t" << json_func_name << ": function(context, callback" << json_func_param_list<< ") {" << std::endl;
					jos << "\t\treturn this.json_call(context, callback, \"" << fit->name << "\", {" << json_func_structure << "});" << std::endl;
					jos << "\t}";


		/*			tmpjos << std::endl << "\t};" << std::endl;
					tmpjos << "\treturn tmp;" << std::endl;
					tmpjos << "}" << std::endl;
					json_structures.insert(tmpjos.str());*/
				}
				cos << "\t}" << std::endl;
			}
			jos << std::endl;
			jos << "};" << std::endl;
			jos << std::endl;
				
			cos << "\treturn SOAP_NO_METHOD;" << std::endl;
			cos << "}" << std::endl << std::endl;

			for(set<std::string>::iterator it = header_structures.begin();it != header_structures.end();it++)
			{
				hos << *it << std::endl;
			}
			for(set<std::string>::iterator it = source_structures.begin();it != source_structures.end();it++)
			{
				cos << *it << std::endl;
			}

			jos << std::endl;
			for(set<std::string>::iterator it = json_structures.begin();it != json_structures.end();it++)
			{
				jos << *it << std::endl;
			}
			
		}


		std::string writeVector(std::string& type, const Library& library)
		{
			std::string preTranslatedTemplateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(preTranslatedTemplateType, referenceModifiers);
			std::string templateType(preTranslatedTemplateType);
			translateType(templateType, library);
			std::string unconst_type = unconst(templateType);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("write_vector_") + templateTypeAlias + (pointer?"_ptr":"_ref");
			
			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				functionName += "_ptr";
			}

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream jos;//throw away
			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection)" << std::endl;
			cos << "{" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(collection == NULL)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\twrite(soap,\"null\");" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\twrite(soap,\"[\");" << std::endl;
			cos << "\tfor(" << dereftype << "::iterator it = collection" << dereferencer << "begin();it != collection" << dereferencer << "end();it++)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(it != collection" << dereferencer << "begin())" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\twrite(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t";
			if(pointer == true)
			{
				writeType(cos, jos, templateType, "*(*it)", library);
			}
			else
			{
				writeType(cos, jos, templateType, "*it", library);
			}
			cos << "\t}" << std::endl;
			cos << "\twrite(soap,\"]\");" << std::endl;
			cos << "}" << std::endl;


			source_structures.insert(cos.str());

			return functionName;
		}

		std::string writeList(std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string unconst_type = unconst(templateType);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("write_list_") + templateTypeAlias + (pointer?"_ptr":"_ref");
			
			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				functionName += "_ptr";
			}

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream jos;//throw away

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection)" << std::endl;
			cos << "{" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(collection == NULL)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\twrite(soap,\"null\");" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\twrite(soap,\"[\");" << std::endl;
			cos << "\tfor(" << dereftype << "::iterator it = collection" << dereferencer << "begin();it != collection" << dereferencer << "end();it++)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(it != collection" << dereferencer << "begin())" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\twrite(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			if(pointer == true)
			{
				writeType(cos, jos, templateType, "*(*it)", library);
			}
			else
			{
				writeType(cos, jos, templateType, "*it", library);
			}
			cos << "\t}" << std::endl;
			cos << "\twrite(soap,\"]\");" << std::endl;
			cos << "}" << std::endl;


			source_structures.insert(cos.str());

			return functionName;
		}

		std::string writeMap(std::string& type, const Library& library)
		{
			return std::string();
		}

		std::string writeEnum(std::string& type, const Library& library)
		{
			std::string referenceModifiers;
			stripReferenceModifiers(type, referenceModifiers);
			translateType(type, library);
			std::string unconst_type = unenum(unconst(type));
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("write_enum_") + unconst_type + (pointer?"_ptr":"");
			

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << " val);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(unconst_type, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << " val)" << std::endl;
			cos << "{" << std::endl;
			cos << "\twriteLong(soap, val);" << std::endl;
			cos << "}" << std::endl;


			source_structures.insert(cos.str());

			buildJSONEnum(pObj, unconst_type);

			return functionName;
		}

		void buildJSONEnum(const ClassObject* pObj, std::string& unconst_type)
		{
			if(json_structures_check.find(unconst_type) != json_structures_check.end())
			{
				return;
			}
			json_structures_check.insert(unconst_type);

			if(pObj->functions.size() != 0)
			{
				std::stringstream jos;
				std::stringstream jos2;
				jos << "clarinet." << unconst_type << " = {" << std::endl;
				int i = 0;
				for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
				{
					if(i != 0)
					{
						jos2 << "," << std::endl;
					}
					string val = fit->returnType;
					if(fit->returnType == "")
					{
						jos << "\t" << i << "," << std::endl;
						jos2 << "\t\"" << fit->name << "\"";
					}
					else
					{
						jos << "\t" << fit->returnType << "," << std::endl;
						jos2 << "\t\"" << fit->name << "\"";
					}
					i++;
				}
				jos << std::endl;
				jos << jos2.str().data() << std::endl;
				jos << "};" << std::endl;
				jos << std::endl;

				json_structures.insert(jos.str());
			}
		}

		std::string writeSet(std::string& type, const Library& library)
		{
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string unconst_type = unconst(templateType);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			bool pointer = referenceModifiers.find('*') != -1;
			std::string functionName = std::string("write_set_") + templateTypeAlias + (pointer?"_ptr":"_ref");
				
			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				functionName += "_ptr";
			}

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection);";

			if(structures_check.find(functionName) != structures_check.end())
			{
				return functionName;
			}
			structures_check.insert(functionName);

			header_structures.insert(hos.str());

			std::stringstream jos;//throw away

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << (typeIsPointer ? "" : "&") << " collection)" << std::endl;
			cos << "{" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(collection == NULL)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\twrite(soap,\"null\");" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\twrite(soap,\"[\");" << std::endl;
			cos << "\tfor(" << dereftype << "::iterator it = collection" << dereferencer << "begin();it != collection" << dereferencer << "end();it++)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tif(it != collection" << dereferencer << "begin())" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\twrite(soap,\",\");" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t";
			if(pointer == true)
			{
				writeType(cos, jos, templateType, "*(*it)", library);
			}
			else
			{
				writeType(cos, jos, templateType, "*it", library);
			}
			cos << "\t}" << std::endl;
			cos << "\twrite(soap,\"]\");" << std::endl;
			cos << "}" << std::endl;


			source_structures.insert(cos.str());

			return functionName;
		}

		/*std::string strip_trailing_(std::string data)
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

		void writeStructureItems(const ClassObject* pObj, std::stringstream& cos, std::string& dereferencer, std::stringstream& jos)
		{
			if(pObj->parentName != "")
			{
				const ClassObject* pParent = NULL;
				if(pObj->GetLibrary().FindClassObject(pObj->parentName, pParent) == false || pParent == NULL)
				{
					std::stringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				writeStructureItems(pParent, cos, dereferencer, jos);
			}
			for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
			{
				if(fit != pObj->functions.begin() || pObj->parentName != "")
				{
					cos << "\twrite(soap, \",\");" << std::endl;
					jos << "," << std::endl;
				}
		//		if(fit->type == FunctionTypeVariable)
				{
					cos << "\twrite_label(soap, \"" << fit->name << "\");" << std::endl;

					std::string unconst_type = unconst(fit->returnType);
					std::string paramType = unconst_type;

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, pObj->GetLibrary());

					jos << "\t\t";
					writeType(cos, jos, unconst_type, ("object" + dereferencer + fit->name).data(), pObj->GetLibrary());
				}
			}
		}

		std::string writeStructure(std::string& paramType, const Library& library)
		{
			std::string type = unconst(paramType);

			std::string functionName;
			std::string dereftype = type;
			bool typeIsPointer = false;
			string dereferencer(".");
			string suffix;
			if(type[type.length() - 1] == '*')
			{
				typeIsPointer = true;
				dereftype = type.substr(0, type.length() - 1);
				dereferencer = "->";
				suffix = "_ptr";
			}

			functionName += "write_" + dereftype + suffix;

			std::stringstream hos;
			hos << "void " << functionName << "(struct soap *soap, " << type << "& object);";

			if(header_structures.find(hos.str()) != header_structures.end())
			{
				return functionName;
			}
			header_structures.insert(hos.str());

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(dereftype, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << dereftype;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			std::stringstream cos;
			cos << "void " << functionName << "(struct soap *soap, " << type << "& object)" << std::endl;
			cos << "{" << std::endl;

			if(typeIsPointer == true)
			{
				cos << "\tif(object == NULL)" << std::endl;
				cos << "\t{" << std::endl;
				cos << "\t\twrite(soap,\"null\");" << std::endl;
				cos << "\t\treturn;" << std::endl;
				cos << "\t}" << std::endl;
			}

			cos << "\twrite(soap, \"{\");" << std::endl;

			std::stringstream jos;
			jos << "clarinet.create_" << type << " = function()" << std::endl;
			jos << "{" << std::endl;
			jos << "\tvar tmp = " << std::endl;
			jos << "\t{" << std::endl;

			writeStructureItems(pObj, cos, dereferencer, jos);

			cos << "\twrite(soap, \"}\");" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			jos << std::endl << "\t};" << std::endl;
			jos << "\treturn tmp;" << std::endl;
			jos << "};" << std::endl;

			json_structures.insert(jos.str());

			return functionName;
		}

		std::string writeFunction(std::string& paramType, const Library& library)
		{
			std::string type = unconst(paramType);

			std::stringstream hos;
			hos << "void write_" << type << "(struct soap *soap, " << type << "& object);";

			if(header_structures.find(hos.str()) != header_structures.end())
			{
				return type;
			}
			header_structures.insert(hos.str());

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(type, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			std::stringstream cos;
			cos << "void write_" << type << "(struct soap *soap, " << type << "& object)" << std::endl;
			cos << "{" << std::endl;
			cos << "\twrite(soap, \"{\");" << std::endl;

			std::stringstream jos;
			jos << "clarinet.create_" << type << " = function()" << std::endl;
			jos << "{" << std::endl;
			jos << "\tvar tmp = " << std::endl;
			jos << "\t{" << std::endl;

			for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
			{
				if(fit != pObj->functions.begin())
				{
					cos << "\twrite(soap, \",\");" << std::endl;
					jos << "," << std::endl;
				}
		//		if(fit->type == FunctionTypeVariable)
				{
					std::string unconst_type = unconst(fit->returnType);
					std::string paramType = unconst_type;

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, library);

					jos << "\t\t";
					writeType(cos, jos, unconst_type, ("object." + fit->name).data(), library);
				}
			}
			cos << "\twrite(soap, \"}\");" << std::endl;
			cos << "}" << std::endl;
			source_structures.insert(cos.str());

			jos << std::endl << "\t};" << std::endl;
			jos << "\treturn tmp;" << std::endl;
			jos << "};" << std::endl;

			json_structures.insert(jos.str());

			return type;
		}

		bool writeType(ostream& cos, ostream& jos, std::string& paramType, const char* paramName, const Library& library)
		{
			std::string referenceModifiers;
			std::string translatedType(paramType);
			stripReferenceModifiers(translatedType, referenceModifiers);
			translateType(translatedType, library);
			std::string referenceModifiers2;
			stripReferenceModifiers(translatedType, referenceModifiers2);
			referenceModifiers += referenceModifiers2;
			bool isPointer = false;
			if(referenceModifiers == "*")
			{
				isPointer = true;
			}

			if(isInt(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twriteInt(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twriteInt(soap, " << paramName << ");" << std::endl;
				}
				jos << "0";
			}
			else if(isSize_t(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twriteSize_t(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twriteSize_t(soap, " << paramName << ");" << std::endl;
				}
				jos << "0";
			}
			else if(isLong(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twriteLong(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twriteLong(soap, " << paramName << ");" << std::endl;
				}
				jos << "0";
			}
			else if(isBool(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twriteBool(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twriteBool(soap, " << paramName << ");" << std::endl;
				}
				jos << "false";
			}
			else if(isDouble(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twriteDouble(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twriteDouble(soap, " << paramName << ");" << std::endl;
				}
				jos << "0.0";
			}
		/*				else if(isDate(translatedType) == true)
			{
				cos << "\twriteDate(soap, " << paramName << ");" << std::endl;
			}*/
			else if(isCharStar(translatedType) == true || isCharStar(paramType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twrite_string(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twrite_string(soap, " << paramName << ");" << std::endl;
				}
				jos << "\"\"";
			}
			else if(isString(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\twrite_string(soap, (" << paramName << ")->data());" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\twrite_string(soap, (" << paramName << ").data());" << std::endl;
				}
				jos << "\"\"";
			}
			else if(isVector(paramType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\t" << writeVector(paramType, library) << "(soap, " << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\t" << writeVector(paramType, library) << "(soap, " << paramName << ");" << std::endl;
				}
				jos << "[]";
			}
			else if(isList(paramType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\t" << writeList(paramType, library) << "(soap, " << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\t" << writeList(paramType, library) << "(soap, " << paramName << ");" << std::endl;
				}
				jos << "[]";
			}
			else if(isMap(paramType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\t" << writeMap(paramType, library) << "(soap, " << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\t" << writeMap(paramType, library) << "(soap, " << paramName << ");" << std::endl;
				}
				jos << "[]";
			}
			else if(isSet(paramType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\t" << writeSet(paramType, library) << "(soap, " << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\t" << writeSet(paramType, library) << "(soap, " << paramName << ");" << std::endl;
				}
				jos << "[]";
			}
			else if(isEnum(translatedType, library) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tsoap_send_raw(soap, \"null\", 4);" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\t" << writeEnum(translatedType, library) << "(soap, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\t" << writeEnum(translatedType, library) << "(soap, " << paramName << ");" << std::endl;
				}

				std::string unconst_type = unenum(unconst(translatedType));
				const ClassObject* pObj = NULL;
				if(library.FindClassObject(unconst_type, pObj) == false || pObj == NULL)
				{
					std::stringstream err;
					err << "unable to find type: " << unconst_type;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();
				if(fit != pObj->functions.end())
				{
					jos << "\"" << fit->name << "\"";
				}
				else
				{
					jos << "\"\"";
				}
			}
			else
			{
				cos << "\t" << writeStructure(paramType, library) << "(soap, " << paramName << ");" << std::endl;
				jos << unconst(paramType) << "()";
			}
			return true;
		}

	}
}
