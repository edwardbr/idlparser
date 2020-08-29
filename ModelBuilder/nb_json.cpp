#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include "CPPParser.h"
#include "nb_json.h"
#include "function_timer.h"

extern xt::function_timer* p_timer;

namespace non_blocking
{
	namespace json
	{

		set<std::string> source_structures;
		set<std::string> header_structures;
		set<std::string> structures_check;

		bool readTypeAtomic(ostream& cos, std::string& paramType, int next_pos, const char* field, int comma_count)
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
				for(int i = 0;i<comma_count;i++) cos << '\t';

				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT8(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_UNSIGNED_INT8(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isInt8(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT8(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_INT8(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isUInt16(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT16(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_UNSIGNED_INT16(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isInt16(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT16(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_INT16(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isUInt32(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT32(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_UNSIGNED_INT32(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isInt32(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT32(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_INT32(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isULong(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_LONG(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_UNSIGNED_LONG(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isLong(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_LONG(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_LONG(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isUInt64(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_UNSIGNED_INT64(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_UNSIGNED_INT64(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isInt64(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_INT64(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_INT64(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isBool(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_BOOL(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_BOOL(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isDouble(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_DOUBLE(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_DOUBLE(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isFloat(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_FLOAT(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_FLOAT(" << next_pos << ", " << field << ");" << std::endl;
				}
			}
			else if(isString(paramType) == true)
			{
				for(int i = 0;i<comma_count;i++) cos << '\t';
				if(referenceModifiers == "*")
				{
					cos << "READ_NULL(" << next_pos << ", " << paramType << ", " << field << ", READ_STRING(" << next_pos << ", *" << field << "));" << std::endl;
				}
				else
				{
					cos << "READ_STRING(" << next_pos << ", " << field << ");" << std::endl;
				}				
			}
			else
			{
				return false;
			}
			return true;
		}


		bool member_initialisation_atomic(ostream& cos, std::string& paramType, const std::string& name)
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
				cos << "\t\t\t, " << name << "(0)" << std::endl;
			}
			else if(isBool(paramType) == true)
			{
				cos << "\t\t\t, " << name << "(true)" << std::endl;
			}
			else if(isDouble(paramType) == true || isFloat(paramType) == true)
			{
				cos << "\t\t\t, " << name << "(0.0)" << std::endl;
			}
			return true;
		}

		bool member_definitition_atomic(ostream& cos, std::string& paramType, const std::string& name, bool isPointer)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			if(isUInt8(paramType) == true)
			{
				cos << "\t\tunsigned char" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isInt8(paramType) == true)
			{
				cos << "\t\tchar" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isUInt16(paramType) == true)
			{
				cos << "\t\tunsigned short" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isInt16(paramType) == true)
			{
				cos << "\t\tshort" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isUInt32(paramType) == true)
			{
				cos << "\t\tunsigned uint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isInt32(paramType) == true)
			{
				cos << "\t\tuint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isULong(paramType) == true)
			{
				cos << "\t\tunsigned long" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isLong(paramType) == true)
			{
				cos << "\t\tlong" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isUInt64(paramType) == true)
			{
				cos << "\t\tuint64_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isInt64(paramType) == true)
			{
				cos << "\t\tuint32_t" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isBool(paramType) == true)
			{
				cos << "\t\tbool" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isDouble(paramType) == true)
			{
				cos << "\t\tdouble" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isFloat(paramType) == true)
			{
				cos << "\t\tfloat" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else if(isString(paramType) == true)
			{
				cos << "\t\tstd::string" << (isPointer ? "* " : " ") << name << ";" << std::endl;
			}
			else
			{
				return false;
			}
			return true;
		}



		void buildStructureMembers(int& i, const ClassObject* pObj, ostringstream& cos, bool isPointer)
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
					ostringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				buildStructureMembers(i, pParent, cos, isPointer);
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
					cos << "\t\tCHECK_CHARACTER(" << i++ << ", ',');" << std::endl;
				}
				first = false;
				cos << "\t\tREAD_LABEL(" << i++ << ", \"" << fit->name <<"\");" << std::endl;

				std::string unconst_type = unconst(fit->returnType);
				std::string paramType = unconst_type;

				//remove reference modifiers from paramType
				std::string referenceModifiers;
				stripReferenceModifiers(paramType, referenceModifiers);
				translateType(paramType, pObj->GetLibrary());

				std::string paramName = std::string("m_obj") + objectDereferencer + fit->name;
				if(readTypeAtomic(cos, paramType + referenceModifiers, i, paramName.data(), 2))
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
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
							//cleanup_stream << "\tCLEAN_COLLECTION_POINTER(m_obj" << objectDereferencer << fit->name << ", CLEAN_VECTOR_PTR(" << ttype << ", " << suffix << ",*" << fit->name << "));" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							cos  << "\t\tREAD_VECTOR_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
							//cleanup_stream << "\tCLEAN_VECTOR_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_VECTOR_REF(" << i << ", " << ttype << ", " << suffix << ", m_obj" << objectDereferencer << fit->name << ");" << std::endl;
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
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_LIST_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
							//cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_LIST_PTR(" << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_LIST_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							cos  << "\t\tREAD_LIST_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
							//cleanup_stream << "\t\t\tCLEAN_LIST_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_LIST_REF(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
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
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_MAP_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
//							cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_MAP_PTR(" << ttype << ", " << suffix << ",*" << fit->name << "));" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_MAP_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							cos  << "\t\tREAD_MAP_PTR(" << i << ", " << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
//							cleanup_stream << "\t\t\tCLEAN_MAP_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_MAP_REF(" << i << ", " << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
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
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_SET_PTR(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
							//cleanup_stream << "\t\t\tCLEAN_COLLECTION_POINTER(" << fit->name << ", CLEAN_SET_PTR(" << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_NULL(" << i << ", " << paramType << ", m_obj" << objectDereferencer << fit->name << ", READ_SET_REF(" << i << ", " << ttype << ", " << suffix << ",*m_obj" << objectDereferencer << fit->name << "));" << std::endl;
						}
					}
					else
					{
						if(innerReferenceModifiers == "*")
						{
							cos  << "\t\tREAD_SET_PTR(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
							//cleanup_stream << "\t\t\tCLEAN_SET_PTR(" << ttype << ", " << suffix << "," << fit->name << ");" << std::endl;
						}
						else
						{
							cos  << "\t\tREAD_SET_REF(" << i << ", " << ttype << ", " << suffix << ",m_obj" << objectDereferencer << fit->name << ");" << std::endl;
						}							
					}
				}
				else if(isEnum(paramType, pObj->GetLibrary()) == true)
				{
					{
						const ClassObject* enumType = NULL;
						if(pObj->GetLibrary().FindClassObject(paramType, enumType) == false || enumType == NULL)
						{
							ostringstream err;
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
						if(readTypeAtomic(cos, parentType + referenceModifiers, i, (std::string("m_obj") + objectDereferencer + fit->name).data(), 2) == false)
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
						cos << "\t\tREAD_NULL(" << i << ", " << streamed_type << ", m_obj" << objectDereferencer << fit->name << ", GET_STRUCT(" << i << ", " << streamed_type << ",*m_obj" << objectDereferencer << fit->name  << "));" << std::endl;
					}
					else
					{
						cos << "\t\tGET_STRUCT(" << i << ", " << streamed_type << ", m_obj" << objectDereferencer << fit->name  << ");" << std::endl;
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
		
		void buildStructureStreamMembers(int& i, const ClassObject* pObj, ostringstream& cos, bool isPointer)
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
					ostringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				buildStructureStreamMembers(i, pParent, cos, isPointer);
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
					cos << "\tbuf << ',';" << std::endl;
				}
				first = false;
				cos << "\tbuf << \"\\\"" << fit->name << "\\\":\";" << std::endl;

				std::string unconst_type = unconst(fit->returnType);

				std::string paramName = std::string("obj") + objectDereferencer + fit->name;
				if(writeType(cos, unconst_type, paramName.data(), pObj->GetLibrary()) == true)
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
					ostringstream err;
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


		void buildStructureCleanupMembers(int& i, const ClassObject* pObj, ostringstream& cos, bool isPointer)
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
					ostringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				buildStructureCleanupMembers(i, pParent, cos, isPointer);
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

				cleanupStream(cos, fit->returnType, std::string("obj.") + fit->name, fit->m_attributes, pObj->GetLibrary());

				if(referenceModifiers == "*")
				{
					cos << "\tdelete obj" << objectDereferencer << fit->name  << ";" << std::endl;
					cos << "\tobj" << objectDereferencer << fit->name  << " = NULL;" << std::endl;
				}

				i++;
			}
		}


		void buildStructureInitMembers(const ClassObject* pObj, ostream& cos, std::string objNamePref)
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
					ostringstream err;
					err << "unable to find type: " << pObj->parentName;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				buildStructureInitMembers(pParent, cos, objNamePref);
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
					cos << "\t" << objNamePref << fit->name << " = NULL;" << std::endl;
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
						cos << "\t" << objNamePref << fit->name << " = 0;" << std::endl;
					}
					else if(isFloat(paramType) == true || isDouble(paramType) == true)
					{
						cos << "\t" << objNamePref << fit->name << " = 0.0;" << std::endl;
					}
					else if(isBool(paramType) == true)
					{
						cos << "\t" << objNamePref << fit->name << " = false;" << std::endl;
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
							ostringstream err;
							err << "unable to find type: " << templateType;
							err << ends;
							string errString(err.str());
							throw errString;
						}
						if(pLocObj->functions.empty() == false)
						{
							cos << "\t" << objNamePref << fit->name << " = " << pLocObj->functions.begin()->name << ";" << std::endl;
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
						
						cos << "\t" << paramType << "_parser::initialise(" << (referenceModifiers == "*" ? "*" : "") << objNamePref << fit->name  << ");" << std::endl;
					}
				}
			}
		}

		void buildFunctionInitMembers(const FunctionObject* pObj, ostream& cos, std::string objNamePref)
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
//					cos << "\t\t\t" << objNamePref << name << " = NULL;" << std::endl;
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
//						cos << "\t\t\t" << objNamePref << name << " = 0;" << std::endl;
					}
					else if(isFloat(paramType) == true || isDouble(paramType) == true)
					{
//						cos << "\t\t\t" << objNamePref << name << " = 0.0;" << std::endl;
					}
					else if(isBool(paramType) == true)
					{
//						cos << "\t\t\t" << objNamePref << name << " = false;" << std::endl;
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
							ostringstream err;
							err << "unable to find type: " << templateType;
							err << ends;
							string errString(err.str());
							throw errString;
						}
						if(pLocObj->functions.empty() == false)
						{
//							cos << "\t\t\t" << objNamePref << name << " = " << pLocObj->functions.begin()->name << ";" << std::endl;
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
						
						cos << "\t\t\t" << paramType << "_parser::initialise(" << (referenceModifiers == "*" ? "*" : "") << objNamePref << name  << ");" << std::endl;
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
			ostringstream hos;

			hos << "class " << newType << " : public ajax::core::async_parser" << std::endl;
			hos << "{" << std::endl;
			hos << "public:" << std::endl;
			hos << "\tstatic " << newType << "* " << "create(" << type << "& obj);" << std::endl;
			hos << "\texplicit " << newType << "(" << type << "& obj);" << std::endl;
			hos << "\tvirtual ~" << newType << "();" << std::endl;
			hos << "\t" << type << "& get_value(){return m_obj;}" << std::endl;
			hos << "\tstatic void initialise(" << type << "& obj);" << std::endl;
			hos << "\tvirtual ajax::core::parse_state parse(const char*& current, const char* const end);" << std::endl;
			hos << "\tstatic bool stream(std::ostream& buf, " << type << "& object);" << std::endl;
			hos << "\tstatic ajax::core::parse_state cleanup(" << type << "& object);" << std::endl;
			hos << "private:" << std::endl;
			hos << "\tint state_;" << std::endl;
			hos << "\tasync_parser* child_;" << std::endl;
			hos << "\t" << type << "& m_obj;" << std::endl;
			hos << "};" << std::endl;
			hos << std::endl;
			header_structures.insert(hos.str());

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(dereftype, pObj) == false || pObj == NULL)
			{
				ostringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			ostringstream cos;
			cos << newType << "* " << newType << "::create(" << type << "& obj)" << std::endl;
			cos << "{" << std::endl;
			cos << "\treturn new " << newType << "(obj);" << std::endl;
			cos << "}" << std::endl;
			cos << std::endl;
			cos << newType << "::" << newType << "(" << type << "& obj) : state_(0), child_(NULL), m_obj(obj)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tinitialise(m_obj);" << std::endl;
			cos << "}" << std::endl;
			cos << std::endl;
			cos << newType << "::~" << newType << "()" << std::endl;
			cos << "{" << std::endl;
			cos << "\tdelete child_;" << std::endl;
			cos << "\tchild_ = NULL;" << std::endl;
//			cos << "\tcleanup(m_obj);" << std::endl;
			cos << "}" << std::endl;
			cos << std::endl;
			cos << "void " << newType << "::initialise(" << type << "& obj)" << std::endl;
			cos << "{" << std::endl;

			std::string objectName = "obj.";
			if(typeIsPointer == true)
			{
				objectName = "obj->";
			}

			buildStructureInitMembers(pObj, cos, objectName);
			cos << "}" << std::endl;
			cos << std::endl;
			cos << "ajax::core::parse_state " << newType << "::parse(const char*& current, const char* const end)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tajax::core::parse_state ret = this->parse_child(child_, current, end);" << std::endl;
			cos << "\tif (ret != ajax::core::parse_complete)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\treturn ret;" << std::endl;
			cos << "\t}" << std::endl;
			cos << "\tswitch(state_)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\tcase 0:" << std::endl;
			int count = 1;
			cos << "\t\tCHECK_CHARACTER(" << count++ << ", '{');" << std::endl;
			buildStructureMembers(count, pObj, cos, typeIsPointer);
			cos << "\t\tCHECK_CHARACTER(" << count++ << ", '}');" << std::endl;
			cos << "\t\tbreak;" << std::endl;
			cos << "\tdefault:" << std::endl;
			cos << "\t\tret = ajax::core::parse_failed;" << std::endl;
			cos << "\t}" << std::endl;
			cos << "\tif(ret == ajax::core::parse_failed)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tLOG_ERROR1(\"parsing error at state %s in " << newType << " \", xt::int_to_string(state_));" << std::endl;
			cos << "\t}" << std::endl;
			for(attributes::const_iterator it = attrib.begin(); it != attrib.end(); it++)
			{
				if(*it == "streamed")
				{
					cos << "\tif(ret == ajax::core::parse_complete)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tret = ajax::core::parse_element_complete;" << std::endl;
					cos << "\t}" << std::endl;
					break;
				}
			}
			cos << "\treturn ret;" << std::endl;
			cos << "}" << std::endl;

			cos << "bool " << newType << "::stream(std::ostream& buf, " << paramType << "& obj)" << std::endl;
			cos << "{" << std::endl;
			cos << "\tbuf << '{';" << std::endl;
			buildStructureStreamMembers(count, pObj, cos, typeIsPointer);
			cos << "\tbuf << '}';" << std::endl;
			cos << "\treturn ajax::core::parse_complete;" << std::endl;
			cos << "}" << std::endl;
			cos << std::endl;

			cos << "ajax::core::parse_state " << newType << "::cleanup(" << paramType << "& obj)" << std::endl;
			cos << "{" << std::endl;
			buildStructureCleanupMembers(count, pObj, cos, typeIsPointer);
			cos << "\treturn ajax::core::parse_complete;" << std::endl;
			cos << "}" << std::endl;
			cos << std::endl;

			source_structures.insert(cos.str());

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

		void writeFiles(const ClassObject& m_ob, ostream& hos, ostream& cos, std::vector<std::string>& namespaces/*, const std::string& header*/)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			cos << std::endl;
			
			if(namespaces.size() > 0)
			{
				for(size_t i = 0;i < namespaces.size();i+=2)
				{
					hos << "namespace " << namespaces[i] << " {" << std::endl;
					cos << "namespace " << namespaces[i] << " {" << std::endl;
				}
				hos << std::endl;
				cos << std::endl;
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
								ostringstream err;
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


					cos << "\ttemplate<class connection, class web_service_impl>" << std::endl;
					cos << "\tclass " << fit->name << "Executer : public ajax::core::parser_executer<connection, web_service_impl>" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\tpublic:" << std::endl;
					cos << "\t\t" << fit->name << "Executer() : state_(0), child_(NULL)" << std::endl;
					constructor_stream << std::ends;
					cos << constructor_stream.str();
					cos << "\t\t{" << std::endl;
					buildFunctionInitMembers(&(*fit), cos, "");
					cos << "\t\t}" << std::endl;
					cos << std::endl;
					cos << "\t\t~" << fit->name << "Executer()" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << "\t\t\tdelete child_;" << std::endl;
					cos << "\t\t\tchild_ = NULL;" << std::endl;
					cos << "\t\t}" << std::endl;
					cos << std::endl;
					cos << "\t\tvirtual ajax::core::parse_state parse(const char*& current, const char* const end)" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << "\t\t\tajax::core::parse_state ret = this->parse_child(child_, current, end);" << std::endl;
					cos << "\t\t\tif (ret != ajax::core::parse_complete)" << std::endl;
					cos << "\t\t\t{" << std::endl;
					cos << "\t\t\t\treturn ret;" << std::endl;
					cos << "\t\t\t}" << std::endl;

					cos << "\t\t\tswitch(state_)" << std::endl;
					cos << "\t\t\t{" << std::endl;
					cos << "\t\t\tcase 0:" << std::endl;
					
					parse_stream << std::ends;
					cos << parse_stream.str();
					
					cos << "\t\t\t\tbreak;" << std::endl;
					cos << "\t\t\tdefault:" << std::endl;
					cos << "\t\t\t\tret = ajax::core::parse_failed;" << std::endl;
					cos << "\t\t\t}" << std::endl;
					cos << "\t\t\tif(ret == ajax::core::parse_failed)" << std::endl;
					cos << "\t\t\t{" << std::endl;
					cos << "\t\t\t\tLOG_ERROR1(\"parsing error at state %s in " << fit->name << "Executer \", xt::int_to_string(state_));" << std::endl;
					cos << "\t\t\t}" << std::endl;
					cos << "\t\t\treturn ret;" << std::endl;
					cos << "\t\t}" << std::endl;
					cos << std::endl;
					cos << "\t\tvirtual xt::network::response_status execute(connection& con, std::ostream& reply, web_service_impl& ws)" << std::endl;
					cos << "\t\t{" << std::endl;
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
					
						cos << "\t\t\t" << paramType << " " << last_param.name << ";" << std::endl;
						
						execute_stream_stream << "\t\t\t\t" << atomic_prefix << paramType << "_parser::stream(reply, " << last_param.name << ");" << std::endl;
						execute_cleanup_stream << "\t\t\t" << atomic_prefix << paramType << "_parser::cleanup(" << last_param.name << ");" << std::endl;
						execute_cleanup_stream << "\t\t\treturn ret;" << std::endl;
	
						if(isJavaScriptObject(paramType))
						{
							paramType = paramType;
						}
						else
						{
							cos << "\t\t\t" << atomic_prefix << paramType << "_parser::initialise(" << last_param.name << ");" << std::endl;
						}

						if(fit->parameters.size() > 1)
						{
							cos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << execute_param_stream.str() << ", " << last_param.name << " END_PARAMS;" << std::endl;
						}
						else
						{
							cos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << last_param.name << " END_PARAMS;" << std::endl;
						}
					}
					else
					{
						cos << "\t\t\txt::network::response_status ret = CONTEXT(" << fit->name << ") " << execute_param_stream.str() << " END_PARAMS;" << std::endl;
					}
					cos << "\t\t\tif(ret == xt::network::ok)" << std::endl;
					cos << "\t\t\t{" << std::endl;
					//cos << "\t\t\t\tstd::ostringstream buf;" << std::endl;
					if(fit->parameters.size() != 0)
					{
						std::list<ParameterObject>::const_reverse_iterator rit = fit->parameters.rbegin();

						cos << execute_stream_stream.str();
						//cos << "\t\t\t\treply = buf.str();" << std::endl;
					}
					cos << "\t\t\t}" << std::endl;

					cos << execute_cleanup_stream.str();

					cos << "\t\t}" << std::endl;

					cos << std::endl;
					cos << "\t\tvirtual void cleanup()" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << cleanup_stream1.str();
					cos << "\t\t}" << std::endl;

					cos << "\tprivate:" << std::endl;
					cos << "\t\tunsigned short state_;" << std::endl;
					cos << "\t\tajax::core::async_parser* child_;" << std::endl;
					member_stream << std::ends;
					cos << member_stream.str();
					cos << "\t};" << std::endl;
					cos << std::endl;
				}
			}

			cos << "\ttemplate<class connection, class web_service_impl>" << std::endl;
			cos << "\tbool create_execute_parser(const std::string& function, ajax::core::parser_executer<xt::network::connection, web_service_impl>*& pp_executer)" << std::endl;
			cos << "\t{" << std::endl;
			cos << "\t\tbool ret = true;" << std::endl;

			for(std::list<FunctionObject>::const_iterator fit = m_ob.functions.begin();fit != m_ob.functions.end();fit++)
			{
				if(fit->type == FunctionTypeMethod)
				{
					std::string json_func_param_list;

					cos << "\t\t";
					if(fit != m_ob.functions.begin())
					{
						cos << "else ";
					}

					cos << "if(function == \"" << fit->name << "\")" << std::endl;
					cos << "\t\t{" << std::endl;
					cos << "\t\t\tpp_executer = new " << fit->name << "Executer<connection, web_service_impl>();" << std::endl;
					cos << "\t\t}" << std::endl;
				}
			}
			cos << "\t\telse" << std::endl;
			cos << "\t\t{" << std::endl;
			cos << "\t\t\tret = false;" << std::endl;
			cos << "\t\t}" << std::endl;
			cos << "\t\treturn ret;" << std::endl;
			
			cos << "\t}" << std::endl << std::endl;

			for(set<std::string>::iterator it = header_structures.begin();it != header_structures.end();it++)
			{
				hos << *it << std::endl;
			}
			for(set<std::string>::iterator it = source_structures.begin();it != source_structures.end();it++)
			{
				cos << *it << std::endl;
			}
			
			if(namespaces.size() > 0)
			{
				for(size_t i = 0;i < namespaces.size();i+=2)
				{
					hos << "}";
					cos << "}";
				}
				hos << std::endl;
				cos << std::endl;
			}
		}

		bool writeType(ostream& cos, std::string& unconst_type, const char* paramName, const Library& library)
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
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << (unsigned int)*" << paramName << ";" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tbuf << (unsigned int)" << paramName << ";" << std::endl;
				}
			}
			else if(isInt8(translatedType))
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << (int)*" << paramName << ";" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tbuf << (int)" << paramName << ";" << std::endl;
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
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << *" << paramName << ";" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tbuf << " << paramName << ";" << std::endl;
				}
			}
			else if(isString(translatedType))
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tjson_format(buf, *" << paramName << ");" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tjson_format(buf, " << paramName << ");" << std::endl;
				}
			}
			else if(isBool(translatedType) == true)
			{
				if(isPointer == true)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse if(*" << paramName << " == true)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"true\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"false\";" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tif(" << paramName << " == true)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"true\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"false\";" << std::endl;
					cos << "\t}" << std::endl;
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

				cos << '\t';

				if(isPointer)
				{
					cos << "STREAM_DEREF(" << paramName << ", ";
				}

				if(innerReferenceModifiers == "*")
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_VECTOR_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_VECTOR_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}	
				else
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_VECTOR_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_VECTOR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}

				if(isPointer)
				{
					cos << ")";
				}

				cos << ";" << std::endl;
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

				cos << '\t';

				if(isPointer)
				{
					cos << "STREAM_DEREF(" << paramName << ", ";
				}

				if(innerReferenceModifiers == "*")
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_LIST_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_LIST_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}	
				else
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_LIST_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_LIST" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}

				if(isPointer)
				{
					cos << ")";
				}

				cos << ";" << std::endl;
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

				cos << '\t';

				if(isPointer)
				{
					cos << "STREAM_DEREF(" << paramName << ", ";
				}

				if(innerReferenceModifiers == "*")
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_MAP_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_MAP_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}	
				else
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_MAP_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_MAP" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}

				if(isPointer)
				{
					cos << ");";
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

				cos << '\t';

				if(isPointer)
				{
					cos << "STREAM_DEREF(" << paramName << ", ";
				}

				if(innerReferenceModifiers == "*")
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_SET_STRUCT_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_SET_PTR" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}	
				else
				{
					if(isStruct(templateType, library))
					{
						cos << "STREAM_SET_STRUCT" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
					else
					{
						cos << "STREAM_SET" << "(" << streamed_type << ", " << paramDereferencer << paramName << ")";
					}
				}

				if(isPointer)
				{
					cos << ")";
				}

				cos << ";" << std::endl;
			}
			else if(isEnum(translatedType, library) == true)
			{
				if(isPointer)
				{
					cos << "\tif(" << paramName << " == NULL)" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << \"null\";" << std::endl;
					cos << "\t}" << std::endl;
					cos << "\telse" << std::endl;
					cos << "\t{" << std::endl;
					cos << "\t\tbuf << *" << paramName <<";" << std::endl;
					cos << "\t}" << std::endl;
				}
				else
				{
					cos << "\tbuf << " << paramName << ";" << std::endl;
				}
			}
			else
			{
				buildStructure(translatedType, attributes(), library);

				cos << '\t';
				if(isPointer)
				{
					cos << "STREAM_DEREF(" << paramName << ", ";
				}


				std::string streamed_type = translatedType;
				if(isJavaScriptObject(translatedType))
				{
					streamed_type = "javascript_object";
				}
				cos << "STREAM_STRUCT(" << streamed_type << ", " << paramDereferencer << paramName  << ")";

				if(isPointer)
				{
					cos << ")";
				}

				cos << ";" << std::endl;
			}
			return true;
		}
	}
}