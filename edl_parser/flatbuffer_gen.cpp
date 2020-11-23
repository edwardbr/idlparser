#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include <filesystem>
#include <algorithm>
#include "CPPParser.h"
#include "flatbuffer_gen.h"
#include "function_timer.h"

extern xt::function_timer* p_timer;

namespace flatbuffer
{
	set<std::string> structures_check;

	bool writeSimpleType(ostream& fos, bool is_request, const FunctionObject& func, const ClassObject& ob, std::string& paramType, int comma_count)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string referenceModifiers;
		stripReferenceModifiers(paramType, referenceModifiers);

		for(int i = 0;i<comma_count;i++) fos << '\t';

		if		(isUInt8	(paramType)) fos << func.name << ":ubyte";
		else if	(isInt8		(paramType)) fos << func.name << ":byte";
		else if	(isUInt16	(paramType)) fos << func.name << ":ushort";
		else if	(isInt16	(paramType)) fos << func.name << ":short";
		else if	(isUInt32	(paramType)) fos << func.name << ":uint";
		else if	(isInt32	(paramType)) fos << func.name << ":int";
		else if	(isULong	(paramType)) fos << func.name << ":ulong";
		else if	(isLong		(paramType)) fos << func.name << ":long";
		else if	(isUInt64	(paramType)) fos << func.name << ":ulong";
		else if	(isInt64	(paramType)) fos << func.name << ":long";
		else if	(isBool		(paramType)) fos << func.name << ":bool";
		else if	(isDouble	(paramType)) fos << func.name << ":double";
		else if	(isFloat	(paramType)) fos << func.name << ":float";
		else if	(isString	(paramType)) fos << func.name << ":string";
		else
		{
			return false;
		}
		return true;
	}

	std::string buildStructure(std::string& paramType, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string type = unconst(paramType);

		std::string functionName;
		std::string dereftype = type;
		bool typeIsPointer = false;
		if(type[type.length() - 1] == '*')
		{
			typeIsPointer = true;
			dereftype = type.substr(0, type.length() - 1);
		}
		std::string newType;

		if(dereftype == "void")
		{
			newType = "ulong";
		}
		else
		{
			newType = dereftype;

			if(structures_check.find(newType) == structures_check.end())
			{
				structures_check.insert(newType);
			}			
		}

		return newType;
	}

	std::string buildVector(const std::string& type, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string templateType = getTemplateParam(type);
		std::string referenceModifiers;
		stripReferenceModifiers(templateType, referenceModifiers);
		translateType(templateType, library);

		if(isUInt8(templateType) == false
			&& isInt8(templateType) == false
			&& isUInt16(templateType) == false
			&& isInt16(templateType) == false
			&& isUInt32(templateType) == false
			&& isInt32(templateType) == false
			&& isULong(templateType) == false 
			&& isLong(templateType) == false
			&& isUInt64(templateType) == false 
			&& isInt64(templateType) == false
			&& isBool(templateType) == false 
			&& isDouble(templateType) == false
			&& isFloat(templateType) == false
			&& isString(templateType) == false)
		{
			std::string unconst_type = unconst(templateType);
			buildStructure(unconst_type, attrib, library);
		}

		return std::string();
	}

	std::string buildList(const std::string& type, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string templateType = getTemplateParam(type);
		std::string referenceModifiers;
		stripReferenceModifiers(templateType, referenceModifiers);
		translateType(templateType, library);

		if(isUInt8(templateType) == false
			&& isInt8(templateType) == false
			&& isUInt16(templateType) == false
			&& isInt16(templateType) == false
			&& isUInt32(templateType) == false
			&& isInt32(templateType) == false
			&& isULong(templateType) == false 
			&& isLong(templateType) == false
			&& isUInt64(templateType) == false 
			&& isInt64(templateType) == false
			&& isBool(templateType) == false 
			&& isDouble(templateType) == false
			&& isFloat(templateType) == false
			&& isString(templateType) == false)
		{
			std::string unconst_type = unconst(templateType);
			buildStructure(unconst_type, attrib, library);
		}

		return std::string();
	}

	std::string buildMap(const std::string& type, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string templateType = getTemplateParam(type);
		std::string referenceModifiers;
		stripReferenceModifiers(templateType, referenceModifiers);
		translateType(templateType, library);

		if(isUInt8(templateType) == false
			&& isInt8(templateType) == false
			&& isUInt16(templateType) == false
			&& isInt16(templateType) == false
			&& isUInt32(templateType) == false
			&& isInt32(templateType) == false
			&& isULong(templateType) == false 
			&& isLong(templateType) == false
			&& isUInt64(templateType) == false 
			&& isInt64(templateType) == false
			&& isBool(templateType) == false 
			&& isDouble(templateType) == false
			&& isFloat(templateType) == false
			&& isString(templateType) == false)
		{
			std::string unconst_type = unconst(templateType);
			buildStructure(unconst_type, attrib, library);
		}

		return std::string();
	}

	std::string buildSet(const std::string& type, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string templateType = getTemplateParam(type);
		std::string referenceModifiers;
		stripReferenceModifiers(templateType, referenceModifiers);
		translateType(templateType, library);

		if(isUInt8(templateType) == false
			&& isInt8(templateType) == false
			&& isUInt16(templateType) == false
			&& isInt16(templateType) == false
			&& isUInt32(templateType) == false
			&& isInt32(templateType) == false
			&& isULong(templateType) == false 
			&& isLong(templateType) == false
			&& isUInt64(templateType) == false 
			&& isInt64(templateType) == false
			&& isBool(templateType) == false 
			&& isDouble(templateType) == false
			&& isFloat(templateType) == false
			&& isString(templateType) == false)
		{
			std::string unconst_type = unconst(templateType);
			buildStructure(unconst_type, attrib, library);
		}

		return std::string();
	}

	void writeFunction(bool is_request, const FunctionObject& func, const ClassObject& ob, std::filesystem::path& path, std::vector<std::string>& namespaces)
	{	
		structures_check.clear();
		auto file = path / (func.name + (is_request ? "_request.fbs" : "_response.fbs"));

		std::stringstream fos;

		{
			std::stringstream parse_stream;

			if(!namespaces.empty())
			{
				fos << "namespace ";
				for(const auto& ns : namespaces )
				{
					if(&ns != &namespaces.front())
						fos << ".";
					fos << ns;
				}
				fos << ";\n";
			}

			for(const ParameterObject& param : func.parameters)
			{
				bool is_out = std::find(param.m_attributes.begin(), param.m_attributes.end(), "out") != param.m_attributes.end();
				bool is_in = std::find(param.m_attributes.begin(), param.m_attributes.end(), "out") != param.m_attributes.end();
				if(!is_out && !is_in)
					is_in = true;
				if(is_request != is_out)
				{
					continue;
				}

				std::string unconst_type = unconst(param.type);
				std::string paramType = unconst_type;

				//remove reference modifiers from paramType
				std::string referenceModifiers;
				stripReferenceModifiers(paramType, referenceModifiers);
				translateType(paramType, ob.GetLibrary());
				paramType = unconst(paramType);
				std::string modified_name = param.name;
				bool isPointer = false;
				if(referenceModifiers == "*")
				{
					isPointer = true;
				}

				std::string memberStreamModifier = referenceModifiers;
				if(memberStreamModifier == "&")
				{
					memberStreamModifier = "";
				}

				if(writeSimpleType(parse_stream, is_request, func, ob, paramType + referenceModifiers,1))
				{
				}
				else if(isVector(paramType) == true)
				{
					buildVector(paramType, param.m_attributes, ob.GetLibrary());

					std::string ttype = getTemplateParam(paramType);
					std::string innerReferenceModifiers;
					stripReferenceModifiers(ttype, innerReferenceModifiers);
					translateType(ttype, ob.GetLibrary());
					string suffix = ttype;
					if(suffix == "std::string")
					{
						suffix = "ajax::core::string";
					}
					else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double")
					{
						suffix = "ajax::core::" + suffix;
					}


					if(referenceModifiers == "*")
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_VECTOR_PTR(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_VECTOR_REF(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_VECTOR_PTR(" << ttype << ", " << suffix << "," << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_VECTOR_REF(" << ttype << ", " << suffix << "," << param.name << ");";
						}
					}
				}
				else if(isList(paramType) == true)
				{
					buildList(paramType, param.m_attributes, ob.GetLibrary());

					
					std::string ttype = getTemplateParam(paramType);
					std::string innerReferenceModifiers;
					stripReferenceModifiers(ttype, innerReferenceModifiers);
					translateType(ttype, ob.GetLibrary());
					string suffix = ttype;
					if(suffix == "std::string")
					{
						suffix = "ajax::core::string";
					}
					else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double")
					{
						suffix = "ajax::core::" + suffix;
					}


					if(referenceModifiers == "*")
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_LIST_PTR(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_LIST_REF(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_LIST_PTR(" << ttype << ", " << suffix << "," << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_LIST_REF(" << ttype << ", " << suffix << "," << param.name << ");";
						}							
					}
				}
				else if(isMap(paramType) == true)
				{
					buildMap(paramType, param.m_attributes, ob.GetLibrary());

					std::string ttype = getTemplateParam(paramType);
					std::string innerReferenceModifiers;
					stripReferenceModifiers(ttype, innerReferenceModifiers);
					translateType(ttype, ob.GetLibrary());
					string suffix = ttype;
					if(suffix == "std::string")
					{
						suffix = "ajax::core::string";
					}
					else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double")
					{
						suffix = "ajax::core::" + suffix;
					}


					if(referenceModifiers == "*")
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_MAP_PTR(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_MAP_REF(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_MAP_PTR(" << ttype << ", " << suffix << "," << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_MAP_REF(" << ttype << ", " << suffix << "," << param.name << ");";
						}
					}
				}
				else if(isSet(paramType) == true)
				{
					buildSet(paramType, param.m_attributes, ob.GetLibrary());
					
					std::string ttype = getTemplateParam(paramType);
					std::string innerReferenceModifiers;
					stripReferenceModifiers(ttype, innerReferenceModifiers);
					translateType(ttype, ob.GetLibrary());
					string suffix = ttype;
					if(suffix == "std::string")
					{
						suffix = "ajax::core::string";
					}
					else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double")
					{
						suffix = "ajax::core::" + suffix;
					}


					if(referenceModifiers == "*")
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_SET_PTR(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_SET_REF(" << ttype << ", " << suffix << ",*" << param.name << ");";
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							parse_stream  << "READ_SET_PTR(" << ttype << ", " << suffix << "," << param.name << ");";
						}
						else
						{
							parse_stream  << "READ_SET_REF(" << ttype << ", " << suffix << "," << param.name << ");";
						}							
					}
				}
				else if(isEnum(paramType, ob.GetLibrary()) == true)
				{
					const ClassObject* enumType = NULL;
					if(ob.GetLibrary().FindClassObject(paramType, enumType) == false || enumType == NULL)
					{
						std::stringstream err;
						err << "unable to find type: " << paramType;
						string errString(err.str());
						throw errString;
					}

					std::string parentType = enumType->parentName;
					if(parentType != "")
					{
						translateType(parentType, ob.GetLibrary().GetLibrary());
					}
					else
					{
						parentType = "unsigned __int8";
					}
					parse_stream << "";
					if(writeSimpleType(parse_stream, is_request, func, ob, parentType + memberStreamModifier, 1) == false)
					{
						throw "invalid base enum type";
					}
				}
				else
				{
					std::string struct_type = buildStructure(paramType, param.m_attributes, ob.GetLibrary());
					parse_stream << param.name << ":" << struct_type;
				}

				if(&param != &(*func.parameters.rbegin()))
				{
					parse_stream << ",";
				}
				parse_stream << "\n";
			}


			fos << "table " << func.name << std::endl;
			fos << "{\n";
			
			fos << parse_stream.str();
			
			fos << "};\n\n";

			fos << "root_type " << func.name << ";\n";

			fos << std::endl;
		}
		
		std::string orig_data;
		{
			std::ifstream ifs(file);
			std::getline(ifs, orig_data, '\0');
		}
		if (orig_data != fos.str())
		{
			std::ofstream ofs(file);
			ofs << fos.str();
		}
	}

	void writeFiles(const ClassObject& ob, std::filesystem::path& path, std::vector<std::string>& namespaces)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
	
		std::set<std::string> used_functions;
		
		for(const FunctionObject& func : ob.functions)
		{
			if(func.name.empty() || used_functions.find(func.name) != used_functions.end())
			{
				continue;
			}
			used_functions.insert(func.name);

			if(func.type == FunctionTypeMethod)
			{
				writeFunction(true, func, ob, path, namespaces);
				writeFunction(false, func, ob, path, namespaces);
			}	
		}

	}
}
