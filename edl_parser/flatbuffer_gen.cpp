#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include "CPPParser.h"
#include "flatbuffer_gen.h"
#include "function_timer.h"

extern xt::function_timer* p_timer;

namespace flatbuffer
{

	set<std::string> source_structures;
	set<std::string> header_structures;
	set<std::string> structures_check;

	bool readTypeAtomic(ostream& fos, std::string& paramType, int next_pos, const char* field, int comma_count)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string referenceModifiers;
		stripReferenceModifiers(paramType, referenceModifiers);
		if(referenceModifiers == "&")
			referenceModifiers = "";
		if(isUInt8(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';

			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT8(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_UNSIGNED_INT8(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isInt8(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT8(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_INT8(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isUInt16(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT16(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_UNSIGNED_INT16(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isInt16(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT16(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_INT16(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isUInt32(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT32(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_UNSIGNED_INT32(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isInt32(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT32(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_INT32(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isULong(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_LONG(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_UNSIGNED_LONG(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isLong(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_LONG(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_LONG(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isUInt64(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT64(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_UNSIGNED_INT64(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isInt64(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT64(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_INT64(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isBool(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_BOOL(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_BOOL(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isDouble(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_DOUBLE(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_DOUBLE(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isFloat(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_FLOAT(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_FLOAT(" << next_pos << ", " << field << ");" << std::endl;
			}
		}
		else if(isString(paramType) == true)
		{
			for(int i = 0;i<comma_count;i++) fos << '\t';
			if(referenceModifiers == "*")
			{
				fos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_STRING(" << next_pos << ", *" << field << "));" << std::endl;
			}
			else
			{
				fos << "READ_STRING(" << next_pos << ", " << field << ");" << std::endl;
			}				
		}
		else
		{
			return false;
		}
		return true;
	}

	bool member_initialisation_atomic(ostream& fos, std::string& paramType, const std::string& name)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(isUInt8(paramType) == true
			|| isInt8(paramType) == true
			|| isUInt16(paramType) == true
			|| isInt16(paramType) == true
			|| isUInt32(paramType) == true
			|| isInt32(paramType) == true
			|| isULong(paramType) == true
			|| isLong(paramType) == true
			|| isUInt64(paramType) == true 
			|| isInt64(paramType) == true)
		{
			fos << "\t\t\t, " << name << "(0)" << std::endl;
		}
		else if(isBool(paramType) == true)
		{
			fos << "\t\t\t, " << name << "(true)" << std::endl;
		}
		else if(isDouble(paramType) == true || isFloat(paramType) == true)
		{
			fos << "\t\t\t, " << name << "(0.0)" << std::endl;
		}
		return true;
	}

	bool member_definitition_atomic(ostream& fos, std::string& paramType, const std::string& name, bool isPointer)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(isUInt8(paramType) == true)
		{
			fos << "\t\tunsigned char" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isInt8(paramType) == true)
		{
			fos << "\t\tchar" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isUInt16(paramType) == true)
		{
			fos << "\t\tunsigned short" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isInt16(paramType) == true)
		{
			fos << "\t\tshort" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isUInt32(paramType) == true)
		{
			fos << "\t\tunsigned uint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isInt32(paramType) == true)
		{
			fos << "\t\tuint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isULong(paramType) == true)
		{
			fos << "\t\tunsigned long" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isLong(paramType) == true)
		{
			fos << "\t\tlong" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isUInt64(paramType) == true)
		{
			fos << "\t\tuint64_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isInt64(paramType) == true)
		{
			fos << "\t\tuint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isBool(paramType) == true)
		{
			fos << "\t\tbool" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isDouble(paramType) == true)
		{
			fos << "\t\tdouble" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isFloat(paramType) == true)
		{
			fos << "\t\tfloat" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else if(isString(paramType) == true)
		{
			fos << "\t\tstd::string" << (isPointer ? "* " : " ") << name << ";" << std::endl;
		}
		else
		{
			return false;
		}
		return true;
	}

	void buildStructureMembers(int& i, const ClassObject* pObj, std::stringstream& fos, bool isPointer)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
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
			buildStructureMembers(i, pParent, fos, isPointer);
		}

		std::string objectDereferencer = ".";
		if(isPointer == true)
		{
			objectDereferencer = "->";
		}

		bool first = true;
		for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
		{
			if(fit->type != FunctionTypeVariable)
				continue;

			if(first == false || pObj->parentName != "")
			{
				fos << "\t\tCHECK_CHARACTER(" << i++ << ", ',');" << std::endl;
			}
			first = false;
			fos << "\t\tREAD_LABEL(" << i++ << ", \"" << fit->name <<"\");" << std::endl;

			std::string unconst_type = unconst(fit->returnType);
			std::string paramType = unconst_type;

			//remove reference modifiers from paramType
			std::string referenceModifiers;
			stripReferenceModifiers(paramType, referenceModifiers);
			translateType(paramType, pObj->GetLibrary());

			std::string paramName = std::string("m_obj") + objectDereferencer + fit->name;
			if(readTypeAtomic(fos, paramType + referenceModifiers, i, paramName.data(), 2))
			{}
			else if(isVector(paramType) == true)
			{
				buildVector(paramType, fit->m_attributes, pObj->GetLibrary());

				std::string ttype = getTemplateParam(paramType);
				std::string innerReferenceModifiers;
				stripReferenceModifiers(ttype, innerReferenceModifiers);
				translateType(ttype, pObj->GetLibrary());

				string suffix = ttype;
				if(suffix == "std::string")
				{
					suffix = "ajax::core::string";
				}
				else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
				{
					suffix = "ajax::core::" + suffix;
				}


				if(referenceModifiers == "*")
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						//cleanup_stream << "\tCLEAN_COLLECTION_POINTER(m_obj" << objectDereferencer << fit->name << ", CLEAN_VECTOR_PTR(" << ttype << ", " << suffix << ",*" << fit->name << "));" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
				}
				else
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
						//cleanup_stream << "\tCLEAN_VECTOR_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << ", m_obj" << objectDereferencer << fit->name << ");" << std::endl;
					}
				}
			}
			else if(isList(paramType) == true)
			{
				buildList(paramType, fit->m_attributes, pObj->GetLibrary());

				std::string ttype = getTemplateParam(paramType);
				std::string innerReferenceModifiers;
				stripReferenceModifiers(ttype, innerReferenceModifiers);
				translateType(ttype, pObj->GetLibrary());

				string suffix = ttype;
				if(suffix == "std::string")
				{
					suffix = "ajax::core::string";
				}
				else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
				{
					suffix = "ajax::core::" + suffix;
				}

				if(referenceModifiers == "*")
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_LIST_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						//cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_LIST_PTR(" << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_LIST_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
				}
				else
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_LIST_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
						//cleanup_stream << "\t\t\tCLEAN_LIST_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_LIST_REF(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
					}							
				}
			}
			else if(isMap(paramType) == true)
			{
				buildMap(paramType, fit->m_attributes, pObj->GetLibrary());

				std::string ttype = getTemplateParam(paramType);
				std::string innerReferenceModifiers;
				stripReferenceModifiers(ttype, innerReferenceModifiers);
				translateType(ttype, pObj->GetLibrary());

				string suffix = ttype;
				if(suffix == "std::string")
				{
					suffix = "ajax::core::string";
				}
				else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
				{
					suffix = "ajax::core::" + suffix;
				}


				if(referenceModifiers == "*")
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_MAP_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
//							cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_MAP_PTR(" << ttype << ", " << suffix << ",*" << fit->name << "));" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_MAP_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
				}
				else
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_MAP_PTR(" << i << ", " << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
//							cleanup_stream << "\t\t\tCLEAN_MAP_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_MAP_REF(" << i << ", " << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
					}
				}
			}
			else if(isSet(paramType) == true)
			{
				buildSet(paramType, fit->m_attributes, pObj->GetLibrary());

				std::string ttype = getTemplateParam(paramType);
				std::string innerReferenceModifiers;
				stripReferenceModifiers(ttype, innerReferenceModifiers);
				translateType(ttype, pObj->GetLibrary());

				string suffix = ttype;
				if(suffix == "std::string")
				{
					suffix = "ajax::core::string";
				}
				else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
				{
					suffix = "ajax::core::" + suffix;
				}


				if(referenceModifiers == "*")
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_SET_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						//cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_SET_PTR(" << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_SET_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
					}
				}
				else
				{
					if(innerReferenceModifiers == "*")
					{
						fos  << "\t\tREAD_SET_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
						//cleanup_stream << "\t\t\tCLEAN_SET_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
					}
					else
					{
						fos  << "\t\tREAD_SET_REF(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
					}							
				}
			}
			else if(isEnum(paramType, pObj->GetLibrary()) == true)
			{
				{
					const ClassObject* enumType = NULL;
					if(pObj->GetLibrary().FindClassObject(paramType, enumType) == false || enumType == NULL)
					{
						std::stringstream err;
						err << "unable to find type: " << paramType;
						err << ends;
						string errString(err.str());
						throw errString;
					}
					std::string parentType = enumType->parentName;
					if(parentType != "")
					{
						translateType(parentType, pObj->GetLibrary());
					}
					else
					{
						parentType = "unsigned __int8";
					}
					if(readTypeAtomic(fos, parentType + referenceModifiers, i, (std::string("m_obj") + objectDereferencer + fit->name).data(), 2) == false)
					{
						throw "invalid base enum type";
					}
				}
			}
			else
			{
				buildStructure(paramType, fit->m_attributes, pObj->GetLibrary());

				std::string streamed_type = paramType;
				if(isJavaScriptObject(paramType))
				{
					streamed_type = "javascript_object";
				}

				if(referenceModifiers == "*")
				{
					fos << "\t\tREAD_NULL(" << i << ", " << streamed_type << ", m_obj" << objectDereferencer << fit->name << ", GET_STRUCT(" << i << ", " << streamed_type << ",*m_obj" << objectDereferencer << fit->name  << "));" << std::endl;
				}
				else
				{
					fos << "\t\tGET_STRUCT(" << i << ", " << streamed_type << ", m_obj" << objectDereferencer << fit->name  << ");" << std::endl;
				}
			}

			i++;
		}
	}

	bool isStruct(std::string type, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		if(type == "std::string")//not interested in these
		{
			return false;
		}

		const ClassObject* pObj = NULL;
		if(library.FindClassObject(type, pObj) == true && pObj != NULL)
		{
			if(pObj->type == ObjectStruct || pObj->type == ObjectClass)
			{
				return true;
			}
		}
		return false;
	}
	
	void buildStructureStreamMembers(int& i, const ClassObject* pObj, std::stringstream& fos, bool isPointer)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
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
			buildStructureStreamMembers(i, pParent, fos, isPointer);
		}

		std::string objectDereferencer = ".";
		if(isPointer == true)
		{
			objectDereferencer = "->";
		}

		bool first = true;
		for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
		{
			if(fit->type != FunctionTypeVariable)
				continue;

			if(first == false || pObj->parentName != "")
			{
				fos << "\tbuf << ',';" << std::endl;
			}
			first = false;
			fos << "\tbuf << \"\\\"" << fit->name << "\\\":\";" << std::endl;

			std::string unconst_type = unconst(fit->returnType);

			std::string paramName = std::string("obj") + objectDereferencer + fit->name;
			if(writeType(fos, unconst_type, paramName.data(), pObj->GetLibrary()) == true)
			{}

			i++;
		}
	}

	void cleanupStream(ostream& cleanup_stream, std::string paramType, std::string paramName, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		//remove reference modifiers from paramType
		std::string referenceModifiers;
		stripReferenceModifiers(paramType, referenceModifiers);
		translateType(paramType, library);
		paramType = unconst(paramType);

		if(isUInt8(paramType) == true
			|| isInt8(paramType) == true
			|| isUInt16(paramType) == true
			|| isInt16(paramType) == true
			|| isUInt32(paramType) == true
			|| isInt32(paramType) == true
			|| isULong(paramType) == true 
			|| isLong(paramType) == true
			|| isUInt64(paramType) == true 
			|| isInt64(paramType) == true
			|| isBool(paramType) == true 
			|| isDouble(paramType) == true
			|| isFloat(paramType) == true
			|| isString(paramType) == true)
		{
			if(referenceModifiers == "*")
			{
				cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
			}
		}
		else if(isJavaScriptObject(paramType) == true)
		{
			if(referenceModifiers == "*")
			{
				cleanup_stream << "\t\t\tCLEAN_JAVASCRIPT_OBJECT(" << paramName << ");" << std::endl;
			}
		}
		else if(isVector(paramType) == true)
		{
			buildVector(paramType, attrib, library);

			std::string ttype = getTemplateParam(paramType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(ttype, innerReferenceModifiers);
			translateType(ttype, library);
			string suffix = ttype;
			if(suffix == "std::string")
			{
				suffix = "ajax::core::string";
			}
			else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
			{
				suffix = "ajax::core::" + suffix;
			}


			if(referenceModifiers == "*")
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << paramName << ", CLEAN_VECTOR_PTR(" << ttype << ", " << suffix << ",*" << paramName << "));" << std::endl;
				}
				else
				{
					cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
				}
			}
			else
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_VECTOR_PTR(" << ttype << ", " << suffix << "," << paramName << ");" << std::endl;
				}
			}
		}
		else if(isList(paramType) == true)
		{
			buildList(paramType, attrib, library);

			
			std::string ttype = getTemplateParam(paramType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(ttype, innerReferenceModifiers);
			translateType(ttype, library);
			string suffix = ttype;
			if(suffix == "std::string")
			{
				suffix = "ajax::core::string";
			}
			else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
			{
				suffix = "ajax::core::" + suffix;
			}


			if(referenceModifiers == "*")
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << paramName << ", CLEAN_LIST_PTR(" << ttype << ", " << suffix << ",*" << paramName << "));" << std::endl;
				}
				else
				{
					cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
				}
			}
			else
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_LIST_PTR(" << ttype << ", " << suffix << "," << paramName << ");" << std::endl;
				}
			}
		}
		else if(isMap(paramType) == true)
		{
			buildMap(paramType, attrib, library);

			std::string ttype = getTemplateParam(paramType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(ttype, innerReferenceModifiers);
			translateType(ttype, library);
			string suffix = ttype;
			if(suffix == "std::string")
			{
				suffix = "ajax::core::string";
			}
			else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
			{
				suffix = "ajax::core::" + suffix;
			}


			if(referenceModifiers == "*")
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << paramName << ", CLEAN_MAP_PTR(" << ttype << ", " << suffix << ",*" << paramName << "));" << std::endl;
				}
				else
				{
					cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
				}
			}
			else
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_MAP_PTR(" << ttype << ", " << suffix << "," << paramName << ");" << std::endl;
				}
			}
		}
		else if(isSet(paramType) == true)
		{
			buildSet(paramType, attrib, library);
			
			std::string ttype = getTemplateParam(paramType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(ttype, innerReferenceModifiers);
			translateType(ttype, library);
			string suffix = ttype;
			if(suffix == "std::string")
			{
				suffix = "ajax::core::string";
			}
			else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
			{
				suffix = "ajax::core::" + suffix;
			}


			if(referenceModifiers == "*")
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << paramName << ", CLEAN_SET_PTR(" << ttype << ", " << suffix << ",*" << paramName << "));" << std::endl;
				}
				else
				{
					cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
				}
			}
			else
			{
				if(innerReferenceModifiers == "*")
				{
					cleanup_stream << "\t\t\tCLEAN_SET_PTR(" << ttype << ", " << suffix << "," << paramName << ");" << std::endl;
				}
			}
		}
		else if(isEnum(paramType, library) == true)
		{
			std::string templateType = paramType.substr(paramType.find_last_of(' ') + 1);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(templateType, innerReferenceModifiers);
			translateType(templateType, library);
			const ClassObject* pObj = NULL;
			if(library.FindClassObject(templateType, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << templateType;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			if(referenceModifiers == "*")
			{
				cleanup_stream << "\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
			}
		}
		else
		{
			std::string struct_type = buildStructure(paramType, attrib, library);
			if(referenceModifiers == "*")
			{
				cleanup_stream << "\t\t\tif(" << paramName<< " != NULL)" << std::endl;
				cleanup_stream << "\t\t\t{" << std::endl;
				cleanup_stream << "\t\t\t\t" << struct_type << "::cleanup(*" << paramName  << ");" << std::endl;
				cleanup_stream << "\t\t\t\tCLEAN_POINTER(" << paramName << ");" << std::endl;
				cleanup_stream << "\t\t\t}" << std::endl;
			}
			else
			{
				cleanup_stream << "\t\t\t" << struct_type << "::cleanup(" << paramName << ");" << std::endl;
			}
		}
	}

	void buildStructureCleanupMembers(int& i, const ClassObject* pObj, std::stringstream& fos, bool isPointer)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
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
			buildStructureCleanupMembers(i, pParent, fos, isPointer);
		}

		std::string objectDereferencer = ".";
		if(isPointer == true)
		{
			objectDereferencer = "->";
		}

		for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
		{
			if(fit->type != FunctionTypeVariable)
				continue;

			std::string unconst_type = unconst(fit->returnType);
			std::string paramType = unconst_type;

			//remove reference modifiers from paramType
			std::string referenceModifiers;
			stripReferenceModifiers(paramType, referenceModifiers);
			translateType(paramType, pObj->GetLibrary());

			cleanupStream(fos, fit->returnType, std::string("obj.") + fit->name, fit->m_attributes, pObj->GetLibrary());

			if(referenceModifiers == "*")
			{
				fos << "\tdelete obj" << objectDereferencer << fit->name  << ";" << std::endl;
				fos << "\tobj" << objectDereferencer << fit->name  << " = NULL;" << std::endl;
			}

			i++;
		}
	}

	void buildStructureInitMembers(const ClassObject* pObj, ostream& fos, std::string objNamePref)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
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
			buildStructureInitMembers(pParent, fos, objNamePref);
		}

		for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
		{
			if(fit->type != FunctionTypeVariable)
				continue;

			std::string unconst_type = unconst(fit->returnType);
			std::string paramType = unconst_type;

			//remove reference modifiers from paramType
			std::string referenceModifiers;
			stripReferenceModifiers(paramType, referenceModifiers);
			translateType(paramType, pObj->GetLibrary());

			if(referenceModifiers == "*")
			{
				fos << "\t" << objNamePref << fit->name << " = NULL;" << std::endl;
			}
			else
			{

				if(isUInt8(paramType) == true || 
					isInt8(paramType) == true || 
					isUInt16(paramType) == true ||
					isInt16(paramType) == true ||
					isUInt32(paramType) == true ||
					isInt32(paramType) == true ||
					isULong(paramType) == true ||
					isLong(paramType) == true ||
					isUInt64(paramType) == true ||
					isInt64(paramType) == true)
				{
					fos << "\t" << objNamePref << fit->name << " = 0;" << std::endl;
				}
				else if(isFloat(paramType) == true || isDouble(paramType) == true)
				{
					fos << "\t" << objNamePref << fit->name << " = 0.0;" << std::endl;
				}
				else if(isBool(paramType) == true)
				{
					fos << "\t" << objNamePref << fit->name << " = false;" << std::endl;
				}
				else if(isEnum(paramType, pObj->GetLibrary()))
				{
					std::string templateType = paramType.substr(paramType.find_last_of(' ') + 1);
					std::string referenceModifiers;
					stripReferenceModifiers(templateType, referenceModifiers);
					translateType(templateType, pObj->GetLibrary());
					const ClassObject* pLocObj = NULL;
					if(pObj->GetLibrary().FindClassObject(templateType, pLocObj) == false || pLocObj == NULL)
					{
						std::stringstream err;
						err << "unable to find type: " << templateType;
						err << ends;
						string errString(err.str());
						throw errString;
					}
					if(pLocObj->functions.empty() == false)
					{
						fos << "\t" << objNamePref << fit->name << " = " << pLocObj->functions.begin()->name << ";" << std::endl;
					}
				}
				else if(isString(paramType) == true ||
					isVector(paramType) == true ||
					isList(paramType) == true || 
					isMap(paramType) == true ||
					isSet(paramType) == true)
				{
				}
				else if(isJavaScriptObject(paramType) == true)
				{
					int x = 0;
				}
				else
				{
					buildStructure(unconst_type, fit->m_attributes, pObj->GetLibrary());
					
					fos << "\t" << paramType << "_parser::initialise(" << (referenceModifiers == "*" ? "*" : "") << objNamePref << fit->name  << ");" << std::endl;
				}
			}
		}
	}

	void buildFunctionInitMembers(const FunctionObject* pObj, ostream& fos, std::string objNamePref)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		for(std::list<ParameterObject>::const_iterator pit = pObj->parameters.begin();pit != pObj->parameters.end();pit++)
		{
			std::string type = pit->type;				
			std::string name = pit->name;
			pit++;
			if(pit == pObj->parameters.end())
			{
				break;
			}
			std::string unconst_type = unconst(type);
			std::string paramType = unconst_type;

			//remove reference modifiers from paramType
			std::string referenceModifiers;
			stripReferenceModifiers(paramType, referenceModifiers);
			translateType(paramType, pObj->GetLibrary());

			if(referenceModifiers == "*")
			{
//					fos << "\t\t\t" << objNamePref << name << " = NULL;" << std::endl;
			}
			else
			{

				if(isUInt8(paramType) == true || 
					isInt8(paramType) == true || 
					isUInt16(paramType) == true ||
					isInt16(paramType) == true ||
					isUInt32(paramType) == true ||
					isInt32(paramType) == true ||
					isULong(paramType) == true ||
					isLong(paramType) == true ||
					isUInt64(paramType) == true ||
					isInt64(paramType) == true)
				{
//						fos << "\t\t\t" << objNamePref << name << " = 0;" << std::endl;
				}
				else if(isFloat(paramType) == true || isDouble(paramType) == true)
				{
//						fos << "\t\t\t" << objNamePref << name << " = 0.0;" << std::endl;
				}
				else if(isBool(paramType) == true)
				{
//						fos << "\t\t\t" << objNamePref << name << " = false;" << std::endl;
				}
				else if(isEnum(paramType, pObj->GetLibrary()))
				{
					std::string templateType = paramType.substr(paramType.find_last_of(' ') + 1);
					std::string referenceModifiers;
					stripReferenceModifiers(templateType, referenceModifiers);
					translateType(templateType, pObj->GetLibrary());
					const ClassObject* pLocObj = NULL;
					if(pObj->GetLibrary().FindClassObject(templateType, pLocObj) == false || pLocObj == NULL)
					{
						std::stringstream err;
						err << "unable to find type: " << templateType;
						err << ends;
						string errString(err.str());
						throw errString;
					}
					if(pLocObj->functions.empty() == false)
					{
//							fos << "\t\t\t" << objNamePref << name << " = " << pLocObj->functions.begin()->name << ";" << std::endl;
					}
				}
				else if(isString(paramType) == true ||
					isVector(paramType) == true ||
					isList(paramType) == true || 
					isMap(paramType) == true ||
					isSet(paramType) == true)
				{
				}
				else if(isJavaScriptObject(paramType) == true)
				{
					int x = 0;
				}
				else
				{
					buildStructure(paramType, pit->m_attributes, pObj->GetLibrary());
					
					fos << "\t\t\t" << paramType << "_parser::initialise(" << (referenceModifiers == "*" ? "*" : "") << objNamePref << name  << ");" << std::endl;
				}
			}
		}
	}

	std::string buildStructure(std::string& paramType, const attributes& attrib, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string type = unconst(paramType);

		if(isJavaScriptObject(type) == true)
		{
			return std::string();
		}

		std::string functionName;
		std::string dereftype = type;
		bool typeIsPointer = false;
//			string dereferencer(".");
//			string suffix;
		if(type[type.length() - 1] == '*')
		{
			typeIsPointer = true;
			dereftype = type.substr(0, type.length() - 1);
//				dereferencer = "->";
//				suffix = "_ptr";
		}

		std::string newType = dereftype + "_parser";
		if(typeIsPointer == true)
		{
			newType = dereftype + "_ptr_parser";
		}


		if(structures_check.find(newType) != structures_check.end())
		{
			return newType;
		}
		structures_check.insert(newType);
		std::stringstream hos;

		header_structures.insert(hos.str());

		const ClassObject* pObj = NULL;
		if(library.FindClassObject(dereftype, pObj) == false || pObj == NULL)
		{
			std::stringstream err;
			err << "unable to find type: " << type;
			err << ends;
			string errString(err.str());
			throw errString;
		}

		std::stringstream fos;
		fos << newType << "* " << newType << "::create(" << type << "& obj)" << std::endl;
		fos << "{" << std::endl;
		fos << "\treturn new " << newType << "(obj);" << std::endl;
		fos << "}" << std::endl;
		fos << std::endl;
		fos << newType << "::" << newType << "(" << type << "& obj) : state_(0), child_(NULL), m_obj(obj)" << std::endl;
		fos << "{" << std::endl;
		fos << "\tinitialise(m_obj);" << std::endl;
		fos << "}" << std::endl;
		fos << std::endl;
		fos << newType << "::~" << newType << "()" << std::endl;
		fos << "{" << std::endl;
		fos << "\tdelete child_;" << std::endl;
		fos << "\tchild_ = NULL;" << std::endl;
//			fos << "\tcleanup(m_obj);" << std::endl;
		fos << "}" << std::endl;
		fos << std::endl;
		fos << "void " << newType << "::initialise(" << type << "& obj)" << std::endl;
		fos << "{" << std::endl;

		std::string objectName = "obj.";
		if(typeIsPointer == true)
		{
			objectName = "obj->";
		}

		buildStructureInitMembers(pObj, fos, objectName);
		fos << "}" << std::endl;
		fos << std::endl;
		fos << "ajax::core::parse_state " << newType << "::parse(const char*& current, const char* const end)" << std::endl;
		fos << "{" << std::endl;
		fos << "\tajax::core::parse_state ret = this->parse_child(child_, current, end);" << std::endl;
		fos << "\tif (ret != ajax::core::parse_complete)" << std::endl;
		fos << "\t{" << std::endl;
		fos << "\t\treturn ret;" << std::endl;
		fos << "\t}" << std::endl;
		fos << "\tswitch(state_)" << std::endl;
		fos << "\t{" << std::endl;
		fos << "\tcase 0:" << std::endl;
		int count = 1;
		fos << "\t\tCHECK_CHARACTER(" << count++ << ", '{');" << std::endl;
		buildStructureMembers(count, pObj, fos, typeIsPointer);
		fos << "\t\tCHECK_CHARACTER(" << count++ << ", '}');" << std::endl;
		fos << "\t\tbreak;" << std::endl;
		fos << "\tdefault:" << std::endl;
		fos << "\t\tret = ajax::core::parse_failed;" << std::endl;
		fos << "\t}" << std::endl;
		fos << "\tif(ret == ajax::core::parse_failed)" << std::endl;
		fos << "\t{" << std::endl;
		fos << "\t\tLOG_ERROR1(\"parsing error at state %s in " << newType << " \", xt::int_to_string(state_));" << std::endl;
		fos << "\t}" << std::endl;
		for(attributes::const_iterator it = attrib.begin(); it != attrib.end(); it++)
		{
			if(*it == "streamed")
			{
				fos << "\tif(ret == ajax::core::parse_complete)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tret = ajax::core::parse_element_complete;" << std::endl;
				fos << "\t}" << std::endl;
				break;
			}
		}
		fos << "\treturn ret;" << std::endl;
		fos << "}" << std::endl;

		fos << "bool " << newType << "::stream(std::ostream& buf, " << paramType << "& obj)" << std::endl;
		fos << "{" << std::endl;
		fos << "\tbuf << '{';" << std::endl;
		buildStructureStreamMembers(count, pObj, fos, typeIsPointer);
		fos << "\tbuf << '}';" << std::endl;
		fos << "\treturn ajax::core::parse_complete;" << std::endl;
		fos << "}" << std::endl;
		fos << std::endl;

		fos << "ajax::core::parse_state " << newType << "::cleanup(" << paramType << "& obj)" << std::endl;
		fos << "{" << std::endl;
		buildStructureCleanupMembers(count, pObj, fos, typeIsPointer);
		fos << "\treturn ajax::core::parse_complete;" << std::endl;
		fos << "}" << std::endl;
		fos << std::endl;

		source_structures.insert(fos.str());

		return newType;
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

	void writeFiles(const ClassObject& m_ob, ostream& fos, std::vector<std::string>& namespaces/*, const std::string& header*/)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		fos << std::endl;
		
		if(namespaces.size() > 0)
		{
			for(size_t i = 0;i < namespaces.size();i+=2)
			{
				fos << "namespace " << namespaces[i] << " {" << std::endl;
			}
			fos << std::endl;
		}
	
		std::set<std::string> used_classes;
		
		for(std::list<FunctionObject>::const_iterator fit = m_ob.functions.begin();fit != m_ob.functions.end();fit++)
		{
			if(used_classes.find(fit->name) != used_classes.end())
			{
				continue;
			}
			used_classes.insert(fit->name);

			if(fit->type == FunctionTypeMethod)
			{
				std::string json_func_param_list;
				std::string json_func_structure;


				std::stringstream parse_stream;
				std::stringstream constructor_stream;
				std::stringstream cleanup_stream1;
				std::stringstream member_stream;
				std::stringstream execute_param_stream;
				std::stringstream execute_stream_stream;
				std::stringstream execute_cleanup_stream;

				std::string param_list;
				int i = 1;
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

					if(is_last == true)
					{
						break;
					}

					if(i != 1)
					{
						execute_param_stream << ", ";
					}

					if(param_list.length() != 0)
					{
						param_list += ", ";
						parse_stream << "\t\t\t\tCHECK_CHARACTER(" << i++ << ", ',');" << std::endl;
					}
					parse_stream << "\t\t\t\tREAD_LABEL(" << i++ << ", \"" << pit->name << "\");" << std::endl;
					
					std::string unconst_type = unconst(pit->type);
					std::string paramType = unconst_type;

					cleanupStream(cleanup_stream1, paramType, pit->name, pit->m_attributes, m_ob.GetLibrary());

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, m_ob.GetLibrary());
					paramType = unconst(paramType);
					std::string modified_name = pit->name;
					bool isPointer = false;
					if(referenceModifiers == "*")
					{
						constructor_stream << "\t\t\t, " << pit->name << "(NULL)" << std::endl;
						isPointer = true;
					}

					std::string memberStreamModifier = referenceModifiers;
					if(memberStreamModifier == "&")
					{
						memberStreamModifier = "";
					}

					execute_param_stream << modified_name;

					if(readTypeAtomic(parse_stream, paramType + referenceModifiers, i, pit->name.data(),4))
					{
						member_definitition_atomic(member_stream, paramType, pit->name, isPointer);
						if(referenceModifiers != "*")
						{
							member_initialisation_atomic(constructor_stream, paramType, pit->name);
						}
					}
					else if(isJavaScriptObject(paramType) == true)
					{
						std::string x;
					}
					else if(isVector(paramType) == true)
					{
						buildVector(paramType, pit->m_attributes, m_ob.GetLibrary());

						std::string ttype = getTemplateParam(paramType);
						std::string innerReferenceModifiers;
						stripReferenceModifiers(ttype, innerReferenceModifiers);
						translateType(ttype, m_ob.GetLibrary());
						string suffix = ttype;
						if(suffix == "std::string")
						{
							suffix = "ajax::core::string";
						}
						else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
						{
							suffix = "ajax::core::" + suffix;
						}


						if(referenceModifiers == "*")
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
						}
						else
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
						}
						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
					}
					else if(isList(paramType) == true)
					{
						buildList(paramType, pit->m_attributes, m_ob.GetLibrary());

						
						std::string ttype = getTemplateParam(paramType);
						std::string innerReferenceModifiers;
						stripReferenceModifiers(ttype, innerReferenceModifiers);
						translateType(ttype, m_ob.GetLibrary());
						string suffix = ttype;
						if(suffix == "std::string")
						{
							suffix = "ajax::core::string";
						}
						else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
						{
							suffix = "ajax::core::" + suffix;
						}


						if(referenceModifiers == "*")
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_LIST_PTR(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_LIST_REF(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
						}
						else
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_LIST_PTR(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_LIST_REF(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}							
						}
						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
					}
					else if(isMap(paramType) == true)
					{
						buildMap(paramType, pit->m_attributes, m_ob.GetLibrary());

						std::string ttype = getTemplateParam(paramType);
						std::string innerReferenceModifiers;
						stripReferenceModifiers(ttype, innerReferenceModifiers);
						translateType(ttype, m_ob.GetLibrary());
						string suffix = ttype;
						if(suffix == "std::string")
						{
							suffix = "ajax::core::string";
						}
						else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
						{
							suffix = "ajax::core::" + suffix;
						}


						if(referenceModifiers == "*")
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_MAP_PTR(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_MAP_REF(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
						}
						else
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_MAP_PTR(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_MAP_REF(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
						}
						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
					}
					else if(isSet(paramType) == true)
					{
						buildSet(paramType, pit->m_attributes, m_ob.GetLibrary());
						
						std::string ttype = getTemplateParam(paramType);
						std::string innerReferenceModifiers;
						stripReferenceModifiers(ttype, innerReferenceModifiers);
						translateType(ttype, m_ob.GetLibrary());
						string suffix = ttype;
						if(suffix == "std::string")
						{
							suffix = "ajax::core::string";
						}
						else if(isUInt8(suffix) || isInt8(suffix) || isUInt16(suffix) || isInt16(suffix) || isUInt32(suffix) || isInt32(suffix) || isUInt64(suffix) || isInt64(suffix) || isULong(suffix) || isLong(suffix) || suffix == "float" || suffix == "double" || isJavaScriptObject(suffix))
						{
							suffix = "ajax::core::" + suffix;
						}


						if(referenceModifiers == "*")
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_SET_PTR(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", READ_SET_REF(" << i << ", " << ttype << ", " << suffix << ",*" << pit->name << "));" << std::endl;
							}
						}
						else
						{
							if(innerReferenceModifiers == "*")
							{
								parse_stream  << "\t\t\t\tREAD_SET_PTR(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}
							else
							{
								parse_stream  << "\t\t\t\tREAD_SET_REF(" << i << ", " << ttype << ", " << suffix << "," << pit->name << ");" << std::endl;
							}							
						}

						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
					}
					else if(isEnum(paramType, m_ob.GetLibrary()) == true)
					{
						const ClassObject* enumType = NULL;
						if(m_ob.GetLibrary().FindClassObject(paramType, enumType) == false || enumType == NULL)
						{
							std::stringstream err;
							err << "unable to find type: " << paramType;
							err << ends;
							string errString(err.str());
							throw errString;
						}

						if(referenceModifiers != "*")
						{
							constructor_stream << "\t\t\t, " << pit->name << "(" << enumType->functions.begin()->name << ")" << std::endl;
						}

						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
				
						std::string parentType = enumType->parentName;
						if(parentType != "")
						{
							translateType(parentType, m_ob.GetLibrary().GetLibrary());
						}
						else
						{
							parentType = "unsigned __int8";
						}
						parse_stream << "\t\t";
						if(readTypeAtomic(parse_stream, parentType + memberStreamModifier, i, pit->name.data(), 2) == false)
						{
							throw "invalid base enum type";
						}
					}
					else
					{
						member_stream << "\t\t" << paramType << memberStreamModifier << " " << pit->name << ";" << std::endl;
						std::string struct_type = buildStructure(paramType, pit->m_attributes, m_ob.GetLibrary());
						if(referenceModifiers == "*")
						{
							parse_stream << "\t\t\t\tREAD_NULL(" << i << ", " << paramType << ", " << pit->name << ", GET_STRUCT(" << i << ", " << paramType << ",*" << pit->name  << "));" << std::endl;
						}
						else
						{
							parse_stream << "\t\t\t\tGET_STRUCT(" << i << ", " << paramType << "," << pit->name  << ");" << std::endl;
						}
					}
					param_list += modified_name;
					i++;
				}


				fos << "\ttemplate<class connection, class web_service_impl>" << std::endl;
				fos << "\tclass " << fit->name << "Executer : public ajax::core::parser_executer<connection, web_service_impl>" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\tpublic:" << std::endl;
				fos << "\t\t" << fit->name << "Executer() : state_(0), child_(NULL)" << std::endl;
				constructor_stream << std::ends;
				fos << constructor_stream.str();
				fos << "\t\t{" << std::endl;
				buildFunctionInitMembers(&(*fit), fos, "");
				fos << "\t\t}" << std::endl;
				fos << std::endl;
				fos << "\t\t~" << fit->name << "Executer()" << std::endl;
				fos << "\t\t{" << std::endl;
				fos << "\t\t\tdelete child_;" << std::endl;
				fos << "\t\t\tchild_ = NULL;" << std::endl;
				fos << "\t\t}" << std::endl;
				fos << std::endl;
				fos << "\t\tvirtual ajax::core::parse_state parse(const char*& current, const char* const end)" << std::endl;
				fos << "\t\t{" << std::endl;
				fos << "\t\t\tajax::core::parse_state ret = this->parse_child(child_, current, end);" << std::endl;
				fos << "\t\t\tif (ret != ajax::core::parse_complete)" << std::endl;
				fos << "\t\t\t{" << std::endl;
				fos << "\t\t\t\treturn ret;" << std::endl;
				fos << "\t\t\t}" << std::endl;

				fos << "\t\t\tswitch(state_)" << std::endl;
				fos << "\t\t\t{" << std::endl;
				fos << "\t\t\tcase 0:" << std::endl;
				
				parse_stream << std::ends;
				fos << parse_stream.str();
				
				fos << "\t\t\t\tbreak;" << std::endl;
				fos << "\t\t\tdefault:" << std::endl;
				fos << "\t\t\t\tret = ajax::core::parse_failed;" << std::endl;
				fos << "\t\t\t}" << std::endl;
				fos << "\t\t\tif(ret == ajax::core::parse_failed)" << std::endl;
				fos << "\t\t\t{" << std::endl;
				fos << "\t\t\t\tLOG_ERROR1(\"parsing error at state %s in " << fit->name << "Executer \", xt::int_to_string(state_));" << std::endl;
				fos << "\t\t\t}" << std::endl;
				fos << "\t\t\treturn ret;" << std::endl;
				fos << "\t\t}" << std::endl;
				fos << std::endl;
				fos << "\t\tvirtual xt::network::response_status execute(connection& con, std::ostream& reply, web_service_impl& ws)" << std::endl;
				fos << "\t\t{" << std::endl;
				if(fit->parameters.size() != 0)
				{
					std::list<ParameterObject>::const_reverse_iterator rit = fit->parameters.rbegin();
					const ParameterObject& last_param = *rit;

					std::string paramType = unconst(last_param.type);

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, m_ob.GetLibrary());
					paramType = unconst(paramType);

					std::stringstream tmp;
					//remove reference modifiers from paramType

					std::string atomic_prefix;

					if(readTypeAtomic(tmp, paramType, i, last_param.name.data(), 3))
					{
						atomic_prefix = "ajax::core::";
					}
					else if(isVector(paramType) == true)
					{
						buildVector(paramType, rit->m_attributes, m_ob.GetLibrary());
					}
					else if(isList(paramType) == true)
					{
						buildList(paramType, rit->m_attributes, m_ob.GetLibrary());
					}
					else if(isMap(paramType) == true)
					{
						buildMap(paramType, rit->m_attributes, m_ob.GetLibrary());
					}
					else if(isSet(paramType) == true)
					{
						buildSet(paramType, rit->m_attributes, m_ob.GetLibrary());
					}
					else if(isEnum(paramType, m_ob.GetLibrary()) == true)
					{
					}
					else
					{
						buildStructure(paramType, rit->m_attributes, m_ob.GetLibrary());
					}
				
					fos << "\t\t\t" << paramType << " " << last_param.name << ";" << std::endl;
					
					execute_stream_stream << "\t\t\t\t" << atomic_prefix << paramType << "_parser::stream(reply, " << last_param.name << ");" << std::endl;
					execute_cleanup_stream << "\t\t\t" << atomic_prefix << paramType << "_parser::cleanup(" << last_param.name << ");" << std::endl;
					execute_cleanup_stream << "\t\t\treturn ret;" << std::endl;

					if(isJavaScriptObject(paramType))
					{
						paramType = paramType;
					}
					else
					{
						fos << "\t\t\t" << atomic_prefix << paramType << "_parser::initialise(" << last_param.name << ");" << std::endl;
					}

					if(fit->parameters.size() > 1)
					{
						fos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << execute_param_stream.str() << ", " << last_param.name << " END_PARAMS;" << std::endl;
					}
					else
					{
						fos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << last_param.name << " END_PARAMS;" << std::endl;
					}
				}
				else
				{
					fos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << execute_param_stream.str() << " END_PARAMS;" << std::endl;
				}
				fos << "\t\t\tif(ret == xt::network::ok)" << std::endl;
				fos << "\t\t\t{" << std::endl;
				//fos << "\t\t\t\tstd::std::stringstream buf;" << std::endl;
				if(fit->parameters.size() != 0)
				{
					std::list<ParameterObject>::const_reverse_iterator rit = fit->parameters.rbegin();

					fos << execute_stream_stream.str();
					//fos << "\t\t\t\treply = buf.str();" << std::endl;
				}
				fos << "\t\t\t}" << std::endl;

				fos << execute_cleanup_stream.str();

				fos << "\t\t}" << std::endl;

				fos << std::endl;
				fos << "\t\tvirtual void cleanup()" << std::endl;
				fos << "\t\t{" << std::endl;
				fos << cleanup_stream1.str();
				fos << "\t\t}" << std::endl;

				fos << "\tprivate:" << std::endl;
				fos << "\t\tunsigned short state_;" << std::endl;
				fos << "\t\tajax::core::async_parser* child_;" << std::endl;
				member_stream << std::ends;
				fos << member_stream.str();
				fos << "\t};" << std::endl;
				fos << std::endl;
			}
		}

		fos << "\ttemplate<class connection, class web_service_impl>" << std::endl;
		fos << "\tbool create_execute_parser(const std::string& function, ajax::core::parser_executer<xt::network::connection, web_service_impl>*& pp_executer)" << std::endl;
		fos << "\t{" << std::endl;
		fos << "\t\tbool ret = true;" << std::endl;

		for(std::list<FunctionObject>::const_iterator fit = m_ob.functions.begin();fit != m_ob.functions.end();fit++)
		{
			if(fit->type == FunctionTypeMethod)
			{
				std::string json_func_param_list;

				fos << "\t\t";
				if(fit != m_ob.functions.begin())
				{
					fos << "else ";
				}

				fos << "if(function == \"" << fit->name << "\")" << std::endl;
				fos << "\t\t{" << std::endl;
				fos << "\t\t\tpp_executer = new " << fit->name << "Executer<connection, web_service_impl>();" << std::endl;
				fos << "\t\t}" << std::endl;
			}
		}
		fos << "\t\telse" << std::endl;
		fos << "\t\t{" << std::endl;
		fos << "\t\t\tret = false;" << std::endl;
		fos << "\t\t}" << std::endl;
		fos << "\t\treturn ret;" << std::endl;
		
		fos << "\t}" << std::endl << std::endl;

		for(set<std::string>::iterator it = source_structures.begin();it != source_structures.end();it++)
		{
			fos << *it << std::endl;
		}
		
		if(namespaces.size() > 0)
		{
			for(size_t i = 0;i < namespaces.size();i+=2)
			{
				fos << "}";
			}
			fos << std::endl;
		}
	}

	bool writeType(ostream& fos, std::string& unconst_type, const char* paramName, const Library& library)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}
		std::string paramType = unconst_type;

		std::string referenceModifiers;
		std::string translatedType(paramType);
		stripReferenceModifiers(translatedType, referenceModifiers);
		translateType(translatedType, library);
		std::string referenceModifiers2;
		stripReferenceModifiers(translatedType, referenceModifiers2);
		referenceModifiers += referenceModifiers2;
		bool isPointer = false;
		std::string paramDereferencer;
		std::string objectDereferencer = ".";
		if(referenceModifiers == "*")
		{
			isPointer = true;
			paramDereferencer = "*";
			objectDereferencer = "->";
		}

		if(	   isUInt8(translatedType))
		{
			if(isPointer == true)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << (unsigned int)*" << paramName << ";" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tbuf << (unsigned int)" << paramName << ";" << std::endl;
			}
		}
		else if(isInt8(translatedType))
		{
			if(isPointer == true)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << (int)*" << paramName << ";" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tbuf << (int)" << paramName << ";" << std::endl;
			}
		}
		else if(isUInt16(translatedType)
			|| isInt16(translatedType)
			|| isUInt32(translatedType)
			|| isInt32(translatedType)
			|| isULong(translatedType)
			|| isLong(translatedType)
			|| isUInt64(translatedType)
			|| isInt64(translatedType)
			|| isFloat(translatedType)
			|| isDouble(translatedType)
			)
		{
			if(isPointer == true)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << *" << paramName << ";" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tbuf << " << paramName << ";" << std::endl;
			}
		}
		else if(isString(translatedType))
		{
			if(isPointer == true)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tjson_format(buf, *" << paramName << ");" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tjson_format(buf, " << paramName << ");" << std::endl;
			}
		}
		else if(isBool(translatedType) == true)
		{
			if(isPointer == true)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse if(*" << paramName << " == true)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"true\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"false\";" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tif(" << paramName << " == true)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"true\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"false\";" << std::endl;
				fos << "\t}" << std::endl;
			}
		}
		else if(isVector(translatedType) == true)
		{
			buildVector(translatedType, attributes(), library);

			std::string templateType = getTemplateParam(translatedType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(templateType, innerReferenceModifiers);
			translateType(templateType, library);
			std::string streamed_type = templateType;
			/*if(isJavaScriptObject(templateType))
			{
				streamed_type = "javascript_object";
			}*/

			fos << '\t';

			if(isPointer)
			{
				fos << "STREAM_DEREF(" << paramName << ", ";
			}

			if(innerReferenceModifiers == "*")
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_VECTOR_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_VECTOR_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}	
			else
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_VECTOR_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_VECTOR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}

			if(isPointer)
			{
				fos << ")";
			}

			fos << ";" << std::endl;
		}
		else if(isList(translatedType) == true)
		{
			buildList(translatedType, attributes(), library);

			std::string templateType = getTemplateParam(translatedType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(templateType, innerReferenceModifiers);
			translateType(templateType, library);
			std::string streamed_type = templateType;
			/*if(isJavaScriptObject(templateType))
			{
				streamed_type = "javascript_object";
			}*/

			fos << '\t';

			if(isPointer)
			{
				fos << "STREAM_DEREF(" << paramName << ", ";
			}

			if(innerReferenceModifiers == "*")
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_LIST_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_LIST_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}	
			else
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_LIST_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_LIST" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}

			if(isPointer)
			{
				fos << ")";
			}

			fos << ";" << std::endl;
		}
		else if(isMap(translatedType) == true)
		{
			buildMap(translatedType, attributes(), library);

			std::string templateType = getTemplateParam(translatedType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(templateType, innerReferenceModifiers);
			translateType(templateType, library);
			std::string streamed_type = templateType;
			/*if(isJavaScriptObject(templateType))
			{
				streamed_type = "javascript_object";
			}*/

			fos << '\t';

			if(isPointer)
			{
				fos << "STREAM_DEREF(" << paramName << ", ";
			}

			if(innerReferenceModifiers == "*")
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_MAP_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_MAP_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}	
			else
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_MAP_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_MAP" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}

			if(isPointer)
			{
				fos << ");";
			}
		}
		else if(isSet(translatedType) == true)
		{
			buildSet(translatedType, attributes(), library);

			std::string templateType = getTemplateParam(translatedType);
			std::string innerReferenceModifiers;
			stripReferenceModifiers(templateType, innerReferenceModifiers);
			translateType(templateType, library);
			std::string streamed_type = templateType;
			/*if(isJavaScriptObject(templateType))
			{
				streamed_type = "javascript_object";
			}*/

			fos << '\t';

			if(isPointer)
			{
				fos << "STREAM_DEREF(" << paramName << ", ";
			}

			if(innerReferenceModifiers == "*")
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_SET_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_SET_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}	
			else
			{
				if(isStruct(templateType, library))
				{
					fos << "STREAM_SET_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
				else
				{
					fos << "STREAM_SET" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
				}
			}

			if(isPointer)
			{
				fos << ")";
			}

			fos << ";" << std::endl;
		}
		else if(isEnum(translatedType, library) == true)
		{
			if(isPointer)
			{
				fos << "\tif(" << paramName << " == NULL)" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << \"null\";" << std::endl;
				fos << "\t}" << std::endl;
				fos << "\telse" << std::endl;
				fos << "\t{" << std::endl;
				fos << "\t\tbuf << *" << paramName <<";" << std::endl;
				fos << "\t}" << std::endl;
			}
			else
			{
				fos << "\tbuf << " << paramName << ";" << std::endl;
			}
		}
		else
		{
			buildStructure(translatedType, attributes(), library);

			fos << '\t';
			if(isPointer)
			{
				fos << "STREAM_DEREF(" << paramName << ", ";
			}


			std::string streamed_type = translatedType;
			if(isJavaScriptObject(translatedType))
			{
				streamed_type = "javascript_object";
			}
			fos << "STREAM_STRUCT(" << streamed_type << ", " << paramDereferencer << paramName  << ")";

			if(isPointer)
			{
				fos << ")";
			}

			fos << ";" << std::endl;
		}
		return true;
	}
}
