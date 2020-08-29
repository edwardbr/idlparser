#include "stdafx.h"
#include "coreclasses.h"
#include "InterpretObject.h"
#include "DumpFunction.h"

void DumpFunction(const functionInfo& func,
				  const string& objName, 
				  const FunctionObject& fn, 
				  ostream& os, 
				  bool istatic, 
				  bool dumpHeader, 
				  bool dumpBody,
				  bool includeTemplates,
				  const char* classSuffixName)
{
/*	functionInfo func;
	func.m_paramSize = fn.parameters.size();

	paramInfo* info = new paramInfo[func.m_paramSize];

	//extract param info
	ExtractParamInfo(lookup, fn, func, info);*/

	if(fn.name == "Write")
		int x = 1;
	//deal with template functions and .cpp files
	DumpFunctionHeader(fn, func, objName, os, istatic, dumpHeader, dumpBody, classSuffixName);

	//dump the implementation code
	if(!(!dumpBody && dumpHeader))
	{
		os << endl;
		DumpFunctionBody(fn, func, os, istatic, dumpHeader, dumpBody);
	}

//	delete [] info;
}

void DumpFunctionHeader(const FunctionObject& fn, 
						const functionInfo& func,  
						const string& obj,
						ostream& os, 
						bool istatic, 
						bool dumpHeader, 
						bool dumpBody,
						const char* classSuffixName)

{
if(fn.name == "FindName" && fn.GetContainer()->name == "ITypeLib")
	int x = 0;
	//print the return parameter
	os << "\t";

	if(istatic && dumpHeader == false)
		os << "static ";
	else if(dumpHeader == true && classSuffixName == NULL)
			os << "virtual ";

	if(fn.returnType != "HRESULT")
		os << fn.returnType << " ";
	else if(func.retval != -1)
	{
		typeInfo& retval = func.m_params[func.retval].typeinfo;
		if(retval.type == ObjectTypeInterface)
			os << "C" << retval.name << "Ptr ";
		else
			os << retval.prefix << " " << retval.name << " ";
	}
	else
		os << "void ";

	if(classSuffixName != NULL)
		os << "C" << obj << classSuffixName << "::";

	if(fn.HasValue("propget"))
		os << "get_";
	else if(fn.HasValue("propput"))
		os << "put_";

	os << fn.name << "(";

	int IID_isPos = 0;
	{
		bool needComma = false;
		for(int pos = 0;pos < func.m_paramSize;pos++)
		{
			paramInfo& param = func.m_params[pos];
			const ParameterObject& po = param.GetParam();

			//ignore iid_is parameters
			if(func.templateCount > 0)
				if(param.iid_is != -1)
					continue;

			if(param.isLength && param.isOutParam)
				continue;

			if(param.isSize && param.isOutParam)
				continue;

			//ignore retval parameters as they are already handled
			if(pos == func.retval)
				continue;

			//now deal with the other parameters
			if(needComma)
				os << ", ";
			else
				needComma = true;

			if(func.templateCount > 0 && param.isTemplate)
			{
				if(param.isInParam)
				{
					if(param.typeinfo.type == ObjectTypeInterface)
						os << "C" << param.typeinfo.name << "&";
					else
						os << "CIUnknown&";
				}
				else if(param.referenceCount == 2)
					os << "RefCountedObjectBase&";
				os << " " <<  po.name;
			}
			else if(param.isArray && param.typeinfo.type == ObjectTypeInterface)
			{
				os << ExtractArrayParamWrapper(param, true) << po.name;
			}
			else if(param.typeinfo.pObj != NULL && param.typeinfo.type == ObjectTypeInterface)
			{
				if(param.referenceCount == 1 || (param.referenceCount == 2 && param.isInParam))
					os << "C" << param.typeinfo.name << "& " <<  po.name;
				else if(param.referenceCount == 2 && !param.isInParam)
					os << "C" << param.typeinfo.name << "Ptr& " <<  po.name;
				else
					os << "****WRONG number of dereferencing****" <<  po.name;
			}
			else
			{
				const string& name = po.type;
				if(po.m_bIsCallBack)
				{
					bool foundBracket = false;
					for(size_t i = 0;i < name.length();i++)
					{
						os << name[i];
						if('(' == name[i] && !foundBracket)
						{	
							foundBracket = true;
							os << "__stdcall ";
						}
					}
				}
				else
				{
					os << name;
				}
				if(!po.m_bIsCallBack)
					os << " " <<  po.name;
				for(list<string>::const_iterator it = po.m_arraySuffix.begin();it != po.m_arraySuffix.end();it++)
					os << *it;
			}
		}
	}
	
	os << ")";
}


void DumpFunctionBody(	const FunctionObject& fn, 
						const functionInfo& func, 
						ostream& os, 
						bool istatic, 
						bool dumpHeader, 
						bool dumpBody)

{
if(fn.name == "FindName" && fn.GetContainer()->name == "ITypeLib")
	int x = 0;
	os << "\t{" << endl;
	
	//initiallise the temporary variables
	{
		for(int i = 0;i < func.m_paramSize; i++)
		{
			paramInfo& param = func.m_params[i];
			const ParameterObject& po = param.GetParam();

			assert(!(param.typeinfo.pObj == NULL && param.typeinfo.type == ObjectTypeInterface));

			//interface smart pointers
			if(param.isArray)
			{
				if(param.typeinfo.type == ObjectTypeInterface)
				{
					/*if(param.size_is_breakdown.size() > 1)
					{
						os << "\t\t" << "function too complicated" << endl;
						break;
					}*/

					bool allocateOnClient = allocateArrayOnClient(param);
					
					if(allocateOnClient)
					{
						if(param.isOutParam)
						{
							size_is_item& item = *param.size_is_breakdown.begin();
							os << "\t\t" << po.name << ".CreateBuffer( " << item.name << ");" << endl;
						}
						else if(param.isInOutParam)
						{
							size_is_item& item = *param.size_is_breakdown.begin();
							os << "\t\t" << po.name << ".ResizeBuffer( " << item.name << ");" << endl;
						}
					}
					else if(param.referenceCount > 1)
						os << "\t\t" << param.typeinfo.name << "* " << po.name << "Tmp;" << endl;
				}
			}
			else if(param.typeinfo.type == ObjectTypeInterface && param.referenceCount == 2)
			{
				os << "\t\t" << param.typeinfo.name << "* " << po.name << "Tmp;" << endl;
			}
			//interfaces of type REFIID and the like
			else if(func.templateCount > 0 && param.refiid_is != -1 && param.referenceCount == 2)
			{
				os << "\t\t";
				if(param.typeinfo.prefix.length())
					os << param.typeinfo.prefix  << " ";
				os << param.typeinfo.name  << "* " << po.name << "Tmp;" << endl;
			}
			else if(i == func.retval)
			{
				if(param.referenceCount == 2)
					os << "*****UNEXPECTED*****";
				else
					os << "\t\t" << param.typeinfo.prefix << " " << param.typeinfo.name << " " << po.name << ";" << endl;
			}
			else if(param.isSize && param.isOutParam)
				os << "\t\t" << param.typeinfo.prefix << " " << param.typeinfo.name << " " << po.name << " = 0;" << endl;
			else if(param.isLength && param.isOutParam)
				os << "\t\t" << param.typeinfo.prefix << " " << param.typeinfo.name << " " << po.name << " = 0;" << endl;
		}
	}

	//now call the function
	os << "\t\t" << fn.returnType << " retval = ";
	if(istatic)
		os << "::";
	else
		os << "m_pInterface->";

	//the name
	if(fn.HasValue("propget"))
		os << "get_";
	else if(fn.HasValue("propput"))
		os << "put_";
	os << fn.name << "(";

	//and the parameters
	{
		for(int i = 0;i < func.m_paramSize; i++)
		{
			paramInfo& param = func.m_params[i];
			const ParameterObject& po = param.GetParam();
			assert(!(param.typeinfo.pObj == NULL && param.typeinfo.type == ObjectTypeInterface));

			if(i > 0)
				os << ", ";

			if(func.templateCount > 0 && param.iid_is != -1)
				os << func.m_params[param.iid_is].GetParam().name << ".GetUUID()";
			else if(param.isArray)
			{
				if(param.typeinfo.type == ObjectTypeInterface)
				{
					bool allocateOnClient = allocateArrayOnClient(param);
					
					if(param.isInParam)//cast out the const
					{
						string type = param.typeinfo.name;
						if(strstr(type.data(), "void"))
							type = "BYTE";

						os << "const_cast<" << ExtractArrayParamWrapper(param, false) << ">(" << po.name << ").GetRawBuffer()";
					}
					else if(!allocateOnClient)
					{
						if(param.referenceCount > 1)
							os << "&" << po.name << "Tmp";
						else
							os << po.name << ".GetRawBuffer()";
					}
					else
						os << po.name << ".GetRawBuffer()";
				}
				else
					os << po.name;
			}
			else if(param.typeinfo.type == ObjectTypeInterface && param.referenceCount == 2)
				os << "&" << po.name << "Tmp";
			else if(func.templateCount > 0 && param.refiid_is != -1 && param.referenceCount == 2)
				os << "&" << po.name << "Tmp";
			else if(param.referenceCount == 1 && param.typeinfo.type == ObjectTypeInterface)
				os << "GetInterface(" << po.name << ")";
			else if(param.isSize && param.isOutParam)
				os << "&" << po.name;
			else if(param.isLength && param.isOutParam)
				os << "&" << po.name;
			else
			{
				if(i == func.retval)
					os << "&";
				os << po.name;
			}
		}
	}
	os << ");" << endl;

	if(fn.returnType == "HRESULT")
		os << "\t\tTest(retval);" << endl;

	if(fn.returnType == "HRESULT" && func.retval != -1 && func.m_params[func.retval].typeinfo.type == ObjectTypeInterface)
		os << "\t\tC" << func.m_params[func.retval].typeinfo.name  << "Ptr " << func.m_params[func.retval].GetParam().name << ";" << endl;

	//initiallise the temporary interface smart pointers
	{
		for(int i = 0;i < func.m_paramSize; i++)
		{
			paramInfo& param = func.m_params[i];
			const ParameterObject& po = param.GetParam();

			assert(!(param.typeinfo.pObj == NULL && param.typeinfo.type == ObjectTypeInterface));

			//the size_is attribute is a pain in the arse as it represents an expression that I suspect is 
			//blindly pasted into midl generated code without any reinterpretation.  Since some of the parameters
			//in my generated code are not directly passed by the client code I have to interpret it.  Hence this
			//fun
			if(param.isArray)
			{
				if(param.typeinfo.type == ObjectTypeInterface)
				{
					
					bool allocateOnClient = allocateArrayOnClient(param);
					if(param.isOutParam)
					{
						if(!allocateOnClient && param.referenceCount > 1)
							os << "\t\t" << po.name << ".AllocateData(" << po.name << "Tmp);" << endl;

						/*if(param.array_size_ref != -1)
							os << func.m_params[param.array_size_ref].GetParam().name << ", ";
						else 
							os << "*** missing size ***, ";
						if(param.array_length_ref != -1)
							os << func.m_params[param.array_length_ref].GetParam().name;
						else 
						{
							if(param.array_size_ref != -1)
								os << func.m_params[param.array_size_ref].GetParam().name;
							else 
								os << "*** missing size or length ***";
						}
						os << ");" << endl;*/
					}
					if(param.isInOutParam || param.isOutParam)
					{
						if(!allocateOnClient && param.size_is_breakdown.size())
						{
							os << "\t\t" << po.name << ".SetSize(";
							string firstpiece;
							bool firstPass = true;
							for(list<size_is_item>::iterator it = param.size_is_breakdown.begin();it != param.size_is_breakdown.end();it++)
							{
								if(firstPass)
								{
									if((*it).name[0] == ',')
										os << "NULL";
									firstPass = false;
								}

								if((*it).pos != -1)
								{//we have a parameter
									paramInfo sizeParam = func.m_params[(*it).pos];
									if(sizeParam.isOutParam)
									{
										assert(firstpiece[firstpiece.length() - 1] == '*');
										os << firstpiece.substr(0,firstpiece.length()-1);
									}
									else
										os << firstpiece;
									firstpiece = "";
									os << (*it).name;
								}
								else
								{//we have a piece of string
									firstpiece = (*it).name;
								}
							}
							os << ");" << endl;
						}
						if(param.array_length_ref != -1)
						{
							paramInfo& otherPi = func.m_params[param.array_length_ref];
							const ParameterObject& otherPO = otherPi.GetParam();
							if((otherPi.isOutParam && !otherPi.isSize && !otherPi.isLength) || otherPi.isInOutParam)
								os << "\t\t" << po.name << ".SetLength(*" << otherPO.name << ");" << endl;
							else
								os << "\t\t" << po.name << ".SetLength(" << otherPO.name << ");" << endl;
						}

	/*					if(param.array_size_ref != -1)
						{
						}
						else 
							os << "*** missing size ***, ";

						if(param.array_length_ref != -1)
							os << "\t\t" << po.name << ".SetLength(" << func.m_params[param.array_length_ref].GetParam().name << ");" << endl;
						else 
						{
							if(param.array_size_ref != -1)
								os << "\t\t" << po.name << ".SetLength(" << func.m_params[param.array_size_ref].GetParam().name << ");" << endl;
							else 
								os << "*** missing size or length ***, ";
						}*/
					}
				}
			}
			else if(param.typeinfo.type == ObjectTypeInterface && param.referenceCount == 2)
				os << "\t\t" << po.name << ".Attach(" << po.name << "Tmp);" << endl;
			else if(func.templateCount > 0 && param.refiid_is != -1 && param.referenceCount == 2)
				os << "\t\t" << po.name << ".Attach(" << po.name << "Tmp);" << endl;
		}
	}

	if(fn.returnType != "HRESULT")
	{
		if(func.retval == -1)
			os << "\t\treturn retval;" << endl;
		else
			os << "\t\treturn " << func.m_params[func.retval].GetParam().name << "Ptr;" << endl;
	}
	else if(func.retval != -1)
	{
		os << "\t\treturn " << func.m_params[func.retval].GetParam().name << ";" << endl;
	}

	os << "\t}" << endl;
}

void PrintSPFunction(const functionInfo& func, const ClassObject& obj, const FunctionObject& fn, ostream& hos)
{
	//extract param info
	string fnName;
	if(fn.HasValue("propget"))
		fnName = "get_";
	else if(fn.HasValue("propput"))
		fnName = "put_";

	fnName += fn.name;
	bool firstPass = true;
	list<ParameterObject>::const_iterator it;

	hos << "\t{" << endl;
	if(func.retval != -1)
		hos << "\t\treturn ";
	else
		hos << "\t\t";

	hos << "m_actual->" << fnName << "(";
	
	firstPass = true;
	for(int i = 0;i < func.m_paramSize;i++)
	{
		paramInfo& param = func.m_params[i];

		//ignore iid_is parameters
		if(param.iid_is != -1)
			continue;

		if(param.isLength && param.isOutParam)
			continue;

		if(param.isSize && param.isOutParam)
			continue;

		//ignore retval parameters as they are already handled
		if(i == func.retval)
			continue;

		if(firstPass)
			firstPass = false;
		else
			hos << ", ";

		hos << param.GetParam().name;
	}

	hos << ");" << endl;
	hos << "\t}" << endl;
}


string ExtractArrayParamWrapper(paramInfo& param, bool showConst) 
{
	string output;
	if(param.typeinfo.type == ObjectTypeInterface)
	{
		if(param.referenceCount == 2 && param.isInParam)
		{
			if(showConst)
				output += "const ";
			output += "Collection<C" + param.typeinfo.name + "Ptr>& ";
		}
		else if(param.referenceCount == 2 && !param.isInParam)
			output += "Collection<C" + param.typeinfo.name + "Ptr>& ";
		else
			output += "****WRONG number of dereferencing****";
	}
	else
	{
		string type = param.typeinfo.name;
		if(strstr(type.data(), "void"))
			type = "BYTE";

		if(param.isInParam && showConst)
			output += "const ";
		output += "Collection<DataWrapper<" + type + "> >& ";
	}
	return output;
}

bool allocateArrayOnClient(paramInfo& param)
{
	//if parameter is an in parameter
	if(param.isInOutParam || param.isInParam)
		return true;
	//if parameter is not a ** then we have to create the buffer locally
	else if(param.isOutParam && param.referenceCount == 1)
		return true;
	return false;
}
