#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <string_view>
#include "CPPParser.h"

#include "ocall_gen.h"
#include "function_timer.h"

extern xt::function_timer* p_timer;

namespace ocall
{
	enum class render_type : int
	{
		request_write = 1,
		request_read = 2,
		response_write = 4,
		response_read = 8,
		structure = 16
	};

	set<std::string> structures_check;

	void writeTable(std::stringstream& fos, render_type rt, const FunctionObject& func, const ClassObject& ob, std::vector<std::string>& loaded_includes);
	void writeTable(std::stringstream& fos, const ClassObject& func, const Library& library, std::vector<std::string>& loaded_includes);

	bool writeSimpleType(ostream& fos, const std::string& func_name, const std::string& paramType, int comma_count)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string referenceModifiers;
		std::string modifiedParamType = paramType;
		stripReferenceModifiers(modifiedParamType, referenceModifiers);

		for(int i = 0;i<comma_count;i++) fos << '\t';

		if		(isUInt8	(modifiedParamType)) fos << func_name << ":ubyte";
		else if	(isInt8		(modifiedParamType)) fos << func_name << ":byte";
		else if	(isUInt16	(modifiedParamType)) fos << func_name << ":ushort";
		else if	(isInt16	(modifiedParamType)) fos << func_name << ":short";
		else if	(isUInt32	(modifiedParamType)) fos << func_name << ":uint";
		else if	(isInt32	(modifiedParamType)) fos << func_name << ":int";
		else if	(isULong	(modifiedParamType)) fos << func_name << ":ulong";
		else if	(isLong		(modifiedParamType)) fos << func_name << ":long";
		else if	(isUInt64	(modifiedParamType)) fos << func_name << ":ulong";
		else if	(isInt64	(modifiedParamType)) fos << func_name << ":long";
		else if	(isBool		(modifiedParamType)) fos << func_name << ":bool";
		else if	(isDouble	(modifiedParamType)) fos << func_name << ":double";
		else if	(isFloat	(modifiedParamType)) fos << func_name << ":float";
		else if	(isString	(modifiedParamType)) fos << func_name << ":string";
		else
		{
			return false;
		}
		return true;
	}

	std::string buildStructure(std::stringstream& fos, std::string& paramType, const attributes& attrib, const Library& library, std::vector<std::string>& loaded_includes)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string type = unconst(paramType);
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
				const ClassObject* pObj = NULL;
				if(library.FindClassObject(newType, pObj) == true && pObj != NULL)
				{
					if(pObj->type == ObjectTypedef)
					{
						newType = pObj->parentName;
						if(library.FindClassObject(newType, pObj) == true && pObj != NULL)
						{

						}
					}
				}
				writeTable(fos, *pObj, library, loaded_includes);
			}			
		}

		return newType;
	}

/*
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
*/

 	std::string& replace(std::string& subj, const std::string& old, const std::string& neu)
    {
		auto found = subj.find(old);
        while (found != std::string::npos)
        {
			subj.replace(found, old.length(), neu);
			found = subj.find(old, found + neu.length());
        }
        return subj;
    }

	void writeTable(std::stringstream& fos, const ClassObject& func, const Library& library, std::vector<std::string>& loaded_includes)
	{		
		std::stringstream parse_stream;

		for(auto include : loaded_includes)
		{
			if(include.substr(include.length() - 4) == ".edl")
				continue;
			replace(include, std::string("\\"), std::string("/"));
			parse_stream << "#include <" << include << ">" << std::endl;	
		}

		parse_stream << "table " << func.name << std::endl;
		parse_stream << "{\n";

		for(const FunctionObject& param : func.functions)
		{
			assert(param.type == FunctionType::FunctionTypeVariable);
			std::string paramType = param.returnType;

			//remove reference modifiers from paramType
			std::string referenceModifiers;
			stripReferenceModifiers(paramType, referenceModifiers);
			translateType(paramType, library);
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

			if(writeSimpleType(parse_stream, param.name, paramType + referenceModifiers,1))
			{
			}
			else
			{
				std::string struct_type = buildStructure(fos, paramType, param.m_attributes, library, loaded_includes);
				parse_stream << param.name << ":" << struct_type;
			}

			if(&param != &(*func.functions.rbegin()))
			{
				parse_stream << ",";
			}
			parse_stream << "\n";
		}

		parse_stream << "};\n\n";

		fos << parse_stream.str();
	}
	
	
	void writeFunction(std::stringstream& fos, render_type rt, const FunctionObject& func, const ClassObject& ob, std::vector<std::string>& loaded_includes)
	{		
		std::stringstream parse_stream;

		for(auto include : loaded_includes)
		{
			if(include.substr(include.length() - 4) == ".edl")
				continue;
			replace(include, std::string("\\"), std::string("/"));
			parse_stream << "#include <" << include << ">" << std::endl;	
		}
		
		parse_stream << "table " << func.name << std::endl;
		parse_stream << "{\n";

		for(const ParameterObject& param : func.parameters)
		{
			if(rt != render_type::structure)
			{
				bool is_out = std::find(param.m_attributes.begin(), param.m_attributes.end(), "out") != param.m_attributes.end();
				bool is_in = std::find(param.m_attributes.begin(), param.m_attributes.end(), "in") != param.m_attributes.end();
				if(!is_out && !is_in)
					is_in = true;
				if(is_in && !(rt == render_type::request_write || rt == render_type::request_read))
				{
					continue;
				}
				if(is_out && !(rt == render_type::response_write || rt == render_type::response_read))
				{
					continue;
				}
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

			if(writeSimpleType(parse_stream, func.name, paramType + referenceModifiers,1))
			{
			}
			/*else if(isVector(paramType) == true)
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
			else*/
			else
			{
				std::string struct_type = buildStructure(fos, paramType, param.m_attributes, ob.GetLibrary(), loaded_includes);
				parse_stream << param.name << ":" << struct_type;
			}

			if(&param != &(*func.parameters.rbegin()))
			{
				parse_stream << ",";
			}
			parse_stream << "\n";
		}

		parse_stream << "};\n\n";

		fos << parse_stream.str();
	}

	void write_to_disk(std::filesystem::path& file, std::stringstream& stream)
	{
		std::string orig_data;
		{
			std::ifstream ifs(file);
			std::getline(ifs, orig_data, '\0');
		}
		if (orig_data != stream.str())
		{
			std::ofstream ofs(file);
			ofs << stream.str();
		}
	}

	void writeFiles(const ClassObject& ob, std::filesystem::path& path, std::vector<std::string>& namespaces, std::vector<std::string>& loaded_includes)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
	
		std::set<std::string> used_functions;


		structures_check.clear();
		std::stringstream request_write_stream;
		std::stringstream request_read_stream;
		std::stringstream response_write_stream;
		std::stringstream response_read_stream;
		request_write_stream << "#pragma once\n";
		request_read_stream << "#pragma once\n";
		response_write_stream << "#pragma once\n";
		response_read_stream << "#pragma once\n";


		for(const FunctionObject& func : ob.functions)
		{
			if(func.name.empty() || used_functions.find(func.name) != used_functions.end())
			{
				continue;
			}
			used_functions.insert(func.name);

			if(func.type == FunctionTypeMethod)
			{
				writeFunction(request_write_stream, render_type::request_write, func, ob, loaded_includes);
				writeFunction(request_read_stream, render_type::request_read, func, ob, loaded_includes);
				writeFunction(response_write_stream, render_type::response_write, func, ob, loaded_includes);
				writeFunction(response_read_stream, render_type::response_read, func, ob, loaded_includes);
			}	
		}

		request_write_stream << std::endl;
		request_read_stream << std::endl;
		response_write_stream << std::endl;
		response_read_stream << std::endl;

        std::filesystem::path request_write_file = path / "request_write.h";
        std::filesystem::path request_read_file = path / "request_read.h";
        std::filesystem::path response_write_file = path / "response_write.h";
        std::filesystem::path response_read_file = path / "response_read.h";

		write_to_disk(request_write_file, request_write_stream);
		write_to_disk(request_read_file, request_read_stream);
		write_to_disk(response_write_file, response_write_stream);
		write_to_disk(response_read_file, response_read_stream);

	}
}
