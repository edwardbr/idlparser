// lientClassWrapper.cpp: implementation of the ClientClassWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "coreclasses.h"
#include "ClientClassWrapper.h"
#include <sstream>
#include "InterpretObject.h"
#include "DumpFunction.h"
#include <filesystem>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IS_DISP_FUNCTION(fn, obj) (fn.HasValue("VAR_DISPATCH") || (obj.parentName == "IDispatch" && !obj.HasValue("dual")))

string getReturnTypeClass(const string& returnType)
{
	if(returnType == "VARIANT")
		return "CComVariant";
	else if(returnType == "BSTR")
		return "CComBSTR";
	return  returnType;
}

struct PropertyDesc
{
	PropertyDesc() :
		isPropPut(false),
		isPropGet(false)
	{}
	PropertyDesc(const PropertyDesc& other) :
		isPropPut(other.isPropPut),
		isPropGet(other.isPropGet),
		returnType(other.returnType)
	{}
	bool isPropPut;
	bool isPropGet;
	string returnType;
};
void PrintDeclspecProperties(const ClassObject& obj, ostream& hos)
{
	std::unordered_map<string, PropertyDesc> PropertiesMap;
	{
		for(list<FunctionObject>::const_iterator it = obj.functions.begin(); it != obj.functions.end();it++)
		{
			const FunctionObject& fn = *it;
			if((fn.parameters.size() != 1) && !IS_DISP_FUNCTION(fn, obj))
				continue;

			if(fn.HasValue("propget") || fn.type == FunctionTypePropertyGet)
			{
				std::unordered_map<string, PropertyDesc>::iterator pit = PropertiesMap.find(fn.name);
				if(pit == PropertiesMap.end())
				{
					PropertyDesc prop;
					prop.isPropGet = true;
					if(IS_DISP_FUNCTION(fn, obj))
						prop.returnType = getReturnTypeClass(fn.returnType);
					else
						prop.returnType = getReturnTypeClass((*fn.parameters.begin()).type);
					//since this is a get function get rid of the trailing '*'
					size_t pos = prop.returnType.rfind("*");
					prop.returnType = prop.returnType.substr(0,pos);
					PropertiesMap.insert(std::unordered_map<string, PropertyDesc>::value_type(fn.name, prop));
				}
				else
					(*pit).second.isPropGet = true;

			}
			else if(fn.HasValue("propput") || fn.type == FunctionTypePropertyPut)
			{
				std::unordered_map<string, PropertyDesc>::iterator pit = PropertiesMap.find(fn.name);
				if(pit == PropertiesMap.end())
				{
					PropertyDesc prop;
					prop.isPropPut = true;
					if(IS_DISP_FUNCTION(fn, obj))
						prop.returnType = getReturnTypeClass(fn.returnType);
					else
						prop.returnType = getReturnTypeClass((*fn.parameters.begin()).type);
					PropertiesMap.insert(std::unordered_map<string, PropertyDesc>::value_type(fn.name, prop));
				}
				else
					(*pit).second.isPropPut = true;

			}
		}
	}
	{
		for(std::unordered_map<string, PropertyDesc>::const_iterator it = PropertiesMap.begin(); it != PropertiesMap.end();it++)
		{
			const string& name = (*it).first;
			const PropertyDesc& desc = (*it).second;

			if(desc.isPropGet && desc.isPropPut)
			{
				hos << "\t__declspec( property( get=get_" << name << ", put=put_" << name << " ) ) " << desc.returnType << " " << name << ";" << endl;
			}
			else if(desc.isPropGet)
			{
				hos << "\t__declspec( property( get=get_" << name << " ) ) " << desc.returnType << " " << name << ";" << endl;
			}
			else if(desc.isPropPut)
				hos << "\t__declspec( property( put=put_" << name << " ) ) " << desc.returnType << " " << name << ";" << endl;
		}
	}
}

void PrintSPDispFunction(const FunctionObject& fn, ostream& hos)
{
	string fnName;
	if(fn.HasValue("propget"))
		fnName = "get_";
	else if(fn.HasValue("propput"))
		fnName = "put_";

	fnName += fn.name;

	list<ParameterObject>::const_reverse_iterator rit = fn.parameters.rbegin();
	const ParameterObject& param = *rit;

	string retType = "void";

	if(rit != fn.parameters.rend() && param.HasValue("retval"))
	{
		string type = param.type;
		type = expandTypeString(type.data(), fn.GetLibrary());
		typeInfo info;
		getTypeStringInfo(type.data(), info, fn.GetLibrary());

		int starcount = 0;
		for(size_t i = 0; i < info.prefix.length();i++)
			if(info.prefix[i] == '*')
				starcount++;


		if(info.type == ObjectTypeInterface || info.type == ObjectTypeDispInterface)
			retType = "C" + info.name;
		else
			retType = info.name;

		if(starcount == 2)
			retType += '*';
	}

	hos << "\t{" << endl;

	if(retType != "void")
		hos << "\t\treturn ";
	else
		hos << "\t\t";
	
	hos << "m_actual->" << fnName << "(";

	//print out the function parameters
	bool firstPass = true;
	for(list<ParameterObject>::const_iterator it = fn.parameters.begin(); it != fn.parameters.end();it++)
	{
		const ParameterObject& param = *it;
		if(firstPass)
			firstPass = false;
		else
			hos << ", ";

		hos << param.name;
	}
	hos << ");" << endl;
	hos << "\t}" << endl;
}

void DumpDispFunction(ostream& cos, const FunctionObject& fn)
{
	cos << "\t{" << endl;
	cos << "\t\tEXCEPINFO info;" << endl;
	cos << "\t\tDISPPARAMS dispparams = {0};" << endl;
	if(fn.parameters.size())
	{
		cos << "\t\tVARIANTARG params[" << fn.parameters.size() << "];" << endl;
		cos << "\t\tdispparams.rgvarg = params;" << endl;
	}
	cos << "\t\tdispparams.cArgs = " << fn.parameters.size() << ";" << endl;
	cos << "\t\tdispparams.cNamedArgs = 0;" << endl;
	
	int index = 0;
	for(list<ParameterObject>::const_iterator it = fn.parameters.begin(); it != fn.parameters.end();it++)
	{
		const ParameterObject& param = *it;
		cos << "\t\tdispparams.rgvarg[" << index << "].vt = ";
		dumpVTtype(param.type, cos);
		cos << ";" << endl;
		cos << "\t\tdispparams.rgvarg[" << index << "].";
		dumpVariantMember(param.type, cos);
		cos << " = " << param.name << ";" << endl;
	}

	string value;
	if(!fn.GetValue("id(", value))
	{
		cos << "error missing dispid";
		return;
	}
	int id = atoi(&value.data()[3]);

	cos << "\t\tHRESULT hRes = m_pInterface->Invoke(" << id << ", IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dispparams, NULL, &info, NULL);" << endl;
	cos << "\t\tTest(hRes);" << endl;
	cos << "\t}" << endl;
}

void PrintSPProperty(const FunctionObject& fn, ostream& hos)
{
	std::stringstream baseFileFn;
	std::stringstream headerFileFn;
	std::stringstream cppFileFn;
	std::stringstream spFileFn;
	GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn,string());
	hos << spFileFn.str();

/*	string value;
	if(!fn.GetValue("id(", value))
		return;

	int id = atoi(&value.data()[3]);

	string fnName;
	if(fn.type == FunctionTypePropertyPut)
	{
		
		hos << "\tvirtual void put_" << fn.name << "(" << fn.returnType << " value)" << endl;
		hos << "\t{" << endl;
		hos << "\t\tm_actual->put_" << fn.name << "(value);" << endl;
		hos << "\t}" << endl;
	}
	else if(fn.type == FunctionTypePropertyGet)
	{
		hos << "\tvirtual ";
		if(fn.returnType == "VARIANT")
			hos << "CComVariant";
		else if(fn.returnType == "BSTR")
			hos << "CComBSTR";
		else
			hos << fn.returnType;

		hos << " get_" << fn.name << "()" << endl;
		hos << "\t{" << endl;
		hos << "\t\treturn  m_actual->get_" << fn.name << "();" << endl;
		hos << "\t}" << endl;
	}*/
}

ostream& operator<< ( ostream& hos, const ClientWrapperProperty& func )
{
	string value;
	if(!func.m_fn.GetValue("id(", value))
		return hos;

	int id = atoi(&value.data()[3]);

	string fnName;
	if(func.m_fn.type == FunctionTypePropertyPut)
	{
		
		hos << "\tvirtual void put_" << func.m_fn.name << "(" << func.m_fn.returnType << " value)" << endl;
		hos << "\t{" << endl;
		hos << "\t\tEXCEPINFO info;" << endl;
		hos << "\t\tDISPPARAMS dispparams = {0};" << endl;
		hos << "\t\tdispparams.cArgs = 1;" << endl;
		hos << "\t\tdispparams.cNamedArgs = 0;" << endl;
		hos << "\t\tdispparams.rgvarg[0].vt = ";
		dumpVTtype(func.m_fn.returnType, hos);
		hos << ";" << endl;
		hos << "\t\tdispparams.rgvarg[0].";
		dumpVariantMember(func.m_fn.returnType, hos);
		hos << " = value;" << endl;

		hos << "\t\tHRESULT hRes = m_pInterface->Invoke(" << id << ", IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dispparams, NULL, &info, NULL);" << endl;
		hos << "\t\tif(FAILED(hRes))" << endl;
		hos << "\t\t{" << endl;
		hos << "\t\t}" << endl;
		hos << "\t}" << endl;
	}
	else if(func.m_fn.type == FunctionTypePropertyGet)
	{
		hos << "\tvirtual ";
		if(func.m_fn.returnType == "VARIANT")
			hos << "CComVariant";
		else if(func.m_fn.returnType == "BSTR")
			hos << "CComBSTR";
		else
			hos << func.m_fn.returnType;

		hos << " get_" << func.m_fn.name << "()" << endl;
		hos << "\t{" << endl;
		hos << "\t\tEXCEPINFO info;" << endl;
		hos << "\t\tDISPPARAMS dispparams = {0};" << endl;
		hos << "\t\tdispparams.cArgs = 1;" << endl;
		hos << "\t\tdispparams.cNamedArgs = 0;" << endl;

		hos << "\t\tHRESULT hRes = m_pInterface->Invoke(" << id << ", IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispparams, NULL, &info, NULL);" << endl;
		hos << "\t\tif(FAILED(hRes))" << endl;
		hos << "\t\t{" << endl;
		hos << "\t\t}" << endl;
		hos << "\t\tCComVariant dest;" << endl;
		hos << "\t\tVariantChangeType(&dest, &dispparams.rgvarg[0],NULL,";
		dumpVTtype(func.m_fn.returnType, hos);
		hos << ");" << endl;

		if(func.m_fn.returnType == "VARIANT")
			hos << "\t\treturn CComVariant(dest);" << endl;
		else if(func.m_fn.returnType == "BSTR")
			hos << "\t\treturn CComBSTR(dest.bstrVal);" << endl;
		else
		{
			hos << "\t\treturn dest.";
			dumpVariantMember(func.m_fn.returnType, hos);
			hos << ";" << endl;
		}

		hos << "\t\t" << endl;
		hos << "\t}" << endl;
	}

	return hos;
}

void dumpVTtype(string returnType, ostream& hos)
{
	size_t pos = 0;
	pos = returnType.find("unsigned char");
	if(pos != string::npos)
	{
		hos << "VT_UI1";
		pos += strlen("unsigned char");
		goto found;
	}
	pos = returnType.find("short");
	if(pos != string::npos)
	{
		hos << "VT_I2";
		pos += strlen("short");
		goto found;
	}
	pos = returnType.find("long");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("long");
		goto found;
	}
	pos = returnType.find("float");
	if(pos != string::npos)
	{
		hos << "VT_R4";
		pos += strlen("float");
		goto found;
	}
	pos = returnType.find("double");
	if(pos != string::npos)
	{
		hos << "VT_R8";
		pos += strlen("double");
		goto found;
	}
	pos = returnType.find("BOOL");
	if(pos != string::npos)
	{
		hos << "VT_BOOL";
		pos += strlen("BOOL");
		goto found;
	}
	pos = returnType.find("SCODE");
	if(pos != string::npos)
	{
		hos << "VT_ERROR";
		pos += strlen("SCODE");
		goto found;
	}
	pos = returnType.find("CURRENCY");
	if(pos != string::npos)
	{
		hos << "VT_CY";
		pos += strlen("CURRENCY");
		goto found;
	}
	pos = returnType.find("DATE");
	if(pos != string::npos)
	{
		hos << "VT_DATE";
		pos += strlen("DATE");
		goto found;
	}
	pos = returnType.find("BSTR");
	if(pos != string::npos)
	{
		hos << "VT_BSTR";
		pos += strlen("BSTR");
		goto found;
	}
	pos = returnType.find("IUnknown*");
	if(pos != string::npos)
	{
		hos << "VT_UNKNOWN";
		pos += strlen("IUnknown*");
		goto found;
	}
	pos = returnType.find("IDispatch*");
	if(pos != string::npos)
	{
		hos << "VT_DISPATCH";
		pos += strlen("IDispatch*");
		goto found;
	}
	pos = returnType.find("SAFEARRAY");
	if(pos != string::npos)
	{
		hos << "VT_ARRAY";
		pos += strlen("SAFEARRAY");
		goto found;
	}
	pos = returnType.find("VARIANT");
	if(pos != string::npos)
	{
		hos << "VT_VARIANT";
		pos += strlen("VARIANT");
		goto found;
	}
	pos = returnType.find("void");
	if(pos != string::npos)
	{
		hos << "VT_UI1";
		pos += strlen("void");
		goto found;
	}
	pos = returnType.find("OLE_HANDLE");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("OLE_HANDLE");
		goto found;
	}
	pos = returnType.find("OLE_XSIZE_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("OLE_XSIZE_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_YSIZE_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("OLE_YSIZE_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_XPOS_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("OLE_XPOS_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_YPOS_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "VT_I4";
		pos += strlen("OLE_YPOS_HIMETRIC");
		goto found;
	}

	hos << "UNKNOWN VARIANT RETURN TYPE:" << returnType;

found:

	while(returnType.length() > pos)
	{
		pos++;
		if(returnType[pos] == '*')
			hos << " | VT_BYREF";

		if(returnType[pos] == '[')
			hos << " | VT_ARRAY";
	}
}

void dumpVariantMember(string returnType, ostream& hos)
{
	string param;
	size_t pos = 0;
	pos = returnType.find("unsigned char");
	if(pos != string::npos)
	{
		param = "bVal";
		pos += strlen("unsigned char");
		goto found;
	}
	pos = returnType.find("short");
	if(pos != string::npos)
	{
		param = "iVal";
		pos += strlen("short");
		goto found;
	}
	pos = returnType.find("long");
	if(pos != string::npos)
	{
		param = "lVal";
		pos += strlen("long");
		goto found;
	}
	pos = returnType.find("float");
	if(pos != string::npos)
	{
		param = "fltVal";
		pos += strlen("float");
		goto found;
	}
	pos = returnType.find("double");
	if(pos != string::npos)
	{
		param = "dblVal";
		pos += strlen("double");
		goto found;
	}
	pos = returnType.find("BOOL");
	if(pos != string::npos)
	{
		param = "boolVal";
		pos += strlen("BOOL");
		goto found;
	}
	pos = returnType.find("SCODE");
	if(pos != string::npos)
	{
		param = "scode";
		pos += strlen("SCODE");
		goto found;
	}
	pos = returnType.find("CURRENCY");
	if(pos != string::npos)
	{
		param = "cyVal";
		pos += strlen("CURRENCY");
		goto found;
	}
	pos = returnType.find("DATE");
	if(pos != string::npos)
	{
		param = "date";
		pos += strlen("DATE");
		goto found;
	}
	pos = returnType.find("BSTR");
	if(pos != string::npos)
	{
		param = "bstrVal";
		pos += strlen("BSTR");
		goto found;
	}
	pos = returnType.find("IUnknown*");
	if(pos != string::npos)
	{
		param = "punkVal";
		pos += strlen("IUnknown*");
		goto found;
	}
	pos = returnType.find("IDispatch*");
	if(pos != string::npos)
	{
		param = "pdispVal";
		pos += strlen("IDispatch*");
		goto found;
	}
	pos = returnType.find("SAFEARRAY");
	if(pos != string::npos)
	{
		param = "parray";
		pos += strlen("SAFEARRAY");
		goto found;
	}
	pos = returnType.find("VARIANT");
	if(pos != string::npos)
	{
		param = "varVal";
		pos += strlen("VARIANT");
		goto found;
	}
	pos = returnType.find("void");
	if(pos != string::npos)
	{
		param = "byref";
		pos += strlen("void");
		goto found;
	}
	pos = returnType.find("OLE_HANDLE");
	if(pos != string::npos)
	{
		param = "lVal";
		pos += strlen("OLE_HANDLE");
		goto found;
	}
	pos = returnType.find("OLE_XSIZE_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "lVal";
		pos += strlen("OLE_XSIZE_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_YSIZE_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "lVal";
		pos += strlen("OLE_YSIZE_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_XPOS_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "lVal";
		pos += strlen("OLE_XPOS_HIMETRIC");
		goto found;
	}
	pos = returnType.find("OLE_YPOS_HIMETRIC");
	if(pos != string::npos)
	{
		hos << "lVal";
		pos += strlen("OLE_YPOS_HIMETRIC");
		goto found;
	}

	hos << "UNKNOWN VARIANT RETURN TYPE:" << returnType;
	return;

found:

	while(returnType.length() > pos)
	{
		pos++;
		if(returnType[pos] == '*')
			param = "p" + param;
	}
	hos << param;
}

/*string getSPReturnType(const FunctionObject& fn)
{
	list<ParameterObject>::const_reverse_iterator rit = fn.parameters.rbegin();
	const ParameterObject& param = *rit;

	string retType = "void";

	if(rit != fn.parameters.rend() && param.HasValue("retval"))
	{
		string type = param.type;
		type = expandTypeString(type.data(), fn.GetLibrary());
		typeInfo info;
		getTypeStringInfo(type.data(), info, fn.GetLibrary());

		int starcount = 0;
		for(size_t i = 0; i < info.prefix.length();i++)
			if(info.prefix[i] == '*')
				starcount++;


		if(info.type == ObjectTypeInterface || info.type == ObjectTypeDispInterface)
			retType = "C" + info.name;
		else
			retType = info.name;

		if(starcount == 2)
			retType += '*';
	}
	return retType;
}*/

/*ostream& operator<< ( ostream& hos, const ClientPrototypeWrapperProperty& func )
{
	std::stringstream baseFileFn;
	std::stringstream headerFileFn;
	std::stringstream cppFileFn;
	GenerateFunctionStreams(func.m_fn, baseFileFn, headerFileFn, cppFileFn,string());
	hos << baseFileFn.str();

	string value;
	if(!func.m_fn.GetValue("id(", value))
		return hos;

	string fnName;
	if(func.m_fn.type == FunctionTypePropertyPut)
	{
		hos << "\tvirtual void put_" << func.m_fn.name << "(";

		bool firstPass = true;
		for(list<ParameterObject>::const_iterator it = func.m_fn.parameters.begin(); it != func.m_fn.parameters.end();it++)
		{
			const ParameterObject& param = *it;
			if(firstPass)
				firstPass = false;
			else
				hos << ", ";

			hos << param.type << " " << param.name;
		}

		hos << ") = 0;" << endl;
	}
	else if(func.m_fn.type == FunctionTypePropertyGet)
	{
		hos << "\tvirtual ";
		if(func.m_fn.returnType == "VARIANT")
			hos << "CComVariant";
		else if(func.m_fn.returnType == "BSTR")
			hos << "CComBSTR";
		else
			hos << getSPReturnType(func.m_fn);//.returnType;

		hos << " get_" << func.m_fn.name << "() = 0;" << endl;
	}
	else
	{
		hos << "\tvirtual void " << func.m_fn.name << "(" << func.m_fn.returnType << " value) = 0;" << endl;
	}

	return hos;
}*/


void DumpInterfaces(const ClassObject& m_ob, ostream& cos, const string& ifName)
{
	const ClassObject* obj;
	if(m_ob.GetLibrary().FindClassObject(ifName, obj))
	{
		if(obj->parentName != "" && obj->parentName != "IDispatch" && obj->parentName != "IUnknown")
			DumpInterfaces(m_ob, cos, obj->parentName);

		cos << "//" << ifName << endl;

		bool isDispatchInterface = false;
		if(!obj->HasValue("dual") && obj->parentName == "IDispatch")
			isDispatchInterface = true;

		for(list<FunctionObject>::const_iterator fit = obj->functions.begin();fit != obj->functions.end();fit++)
		{
			const FunctionObject& fn = *fit;

			if(!fn.HasValue("call_as"))
			{
				functionInfo func(fn, obj->GetLibrary());

				if(fn.HasValue("VAR_DISPATCH") || (obj->parentName == "IDispatch" && !obj->HasValue("dual")))
				{
					if(fn.type == FunctionTypeMethod)
					{
						DumpFunction(func,m_ob.name,fn,cos,false,true,false, false, NULL);
						cos << endl;
						DumpDispFunction(cos, fn);
					}
					else
						cos << ClientWrapperProperty(fn);
				}
				else if(fn.returnType == "HRESULT")
				{
					if(isDispatchInterface)
					{
						DumpFunction(func,m_ob.name,fn,cos,false,true,false, false, NULL);
						cos << endl;
						DumpDispFunction(cos, fn);
					}
					else
					{
						DumpFunction(func,m_ob.name,fn,cos,false,true,true, false, NULL);
					}
				}
				else
				{
					std::stringstream baseFileFn;
					std::stringstream headerFileFn;
					std::stringstream cppFileFn;
					std::stringstream spFileFn;
					GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn,string());
					cos << spFileFn.str();
				}
			}
		}
	}
}

void DumpSPInterfaces(const ClassObject& m_ob, ostream& hos, ostream& cos, const string& ifName, string mostDerivedName)
{
if(m_ob.name == "IXMLDocument")
	int x = 1;

	if(!mostDerivedName.length())
		mostDerivedName = ifName;

	const ClassObject* obj;
	if(m_ob.GetLibrary().FindClassObject(ifName, obj))
	{
		if(obj->parentName != "" && obj->parentName != "IDispatch" && obj->parentName != "IUnknown")
			DumpSPInterfaces(m_ob, hos, cos, obj->parentName, mostDerivedName);

		hos << "//" << ifName << endl;

		bool isDispatchInterface = false;
		if(!obj->HasValue("dual") && obj->parentName == "IDispatch")
			isDispatchInterface = true;

		for(list<FunctionObject>::const_iterator fit = obj->functions.begin();fit != obj->functions.end();fit++)
		{
			const FunctionObject& fn = *fit;
			if(!fn.HasValue("call_as"))
			{
				functionInfo func(fn, obj->GetLibrary());

				if(fn.HasValue("VAR_DISPATCH") || (obj->parentName == "IDispatch" && !obj->HasValue("dual")))
				{

					if(fn.type == FunctionTypeMethod)
					{
						DumpFunction(func, mostDerivedName,fn,hos,false,true,false,true, NULL);
						hos << ";" << endl;
						DumpFunction(func, mostDerivedName,fn,cos,false,true,false, false, "Ptr");
						cos << endl;
						PrintSPDispFunction(fn,cos);
					}
					else
					{
						PrintSPProperty(fn, hos);
					}
				}
				else if(fn.returnType == "HRESULT")
				{
					if(isDispatchInterface)
					{
						DumpFunction(func, mostDerivedName,fn,hos,false,true,false,true, NULL);
						hos << ";" << endl;
						DumpFunction(func, mostDerivedName,fn,cos,false,true,false, false, "Ptr");
						cos << endl;
						PrintSPDispFunction(fn, hos);
					}
					else
					{
						DumpFunction(func, mostDerivedName,fn,hos,false,true,false,true, NULL);
						hos << ";" << endl;
						DumpFunction(func, mostDerivedName,fn,cos,false,true,false, false, "Ptr");
						cos << endl;
						PrintSPFunction(func, *obj, fn, cos);
					}
				}
				else
				{
					std::stringstream baseFileFn;
					std::stringstream headerFileFn;
					std::stringstream cppFileFn;
					std::stringstream spFileFn;
					GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn,string());
					hos << spFileFn.str();
				}
			}
		}
	}
}

void DumpFunctionPrototypes(ostream& hos, const ClassObject& obj)
{
	hos << "//" << obj.name << endl;

	for(list<FunctionObject>::const_iterator fit = obj.functions.begin();fit != obj.functions.end();fit++)
	{
		const FunctionObject& fn = *fit;

		if(!fn.HasValue("call_as"))
		{
//			functionInfo func(fn,obj.GetLibrary());

			std::stringstream baseFileFn;
			std::stringstream headerFileFn;
			std::stringstream cppFileFn;
			std::stringstream spFileFn;
			GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn,string());
			hos << baseFileFn.str();

			/*if(IS_DISP_FUNCTION(fn, obj))
			{
				if(fn.type == FunctionTypeMethod)
				{
					DumpFunction(func, obj.name,fn,hos,false,true,false,true, NULL);
					hos << " = 0;" << endl;
				}
				else
				{
					std::stringstream baseFileFn;
					std::stringstream headerFileFn;
					std::stringstream cppFileFn;
					std::stringstream spFileFn;
					GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn,string());
					hos << baseFileFn.str();
//					hos << ClientPrototypeWrapperProperty(fn);
//				}
			}
			else if(fn.returnType == "HRESULT")
			{
				DumpFunction(func, obj.name,fn,hos,false,true,false,true, NULL);
				hos << " = 0;" << endl;
			}*/
		}
	}
	PrintDeclspecProperties(obj, hos);
}

void DumpClientWrapper(const ClassObject& m_ob, ostream& hos, ostream& cos)
{
	if(m_ob.name == "IXMLDocument")
		int i = 0;
	//create a COM equivelant interface
	hos << endl;
	hos << "//create a COM equivelant interface" << endl;

	{
		string uuidString;
		for(attributes::const_iterator it = m_ob.m_attributes.begin();it != m_ob.m_attributes.end();it++)
		{
			int i = 0;
			while((*it)[i] == ' ')
				i++;
			if((*it).substr(i,5) == "uuid(")
			{
				uuidString = (*it).substr(i + 5,36);
				break;
			}
		}
	
//		hos << "class __declspec(novtable) __declspec(uuid(\"" << uuidString << "\")) ERRORHANDLER_API C" << m_ob.name;
		hos << "class __declspec(novtable) ERRORHANDLER_API C" << m_ob.name;
	}
	if(m_ob.parentName.length())
		hos << " : public C" << m_ob.parentName;
	hos << endl;
	hos << "{" << endl;
	if(m_ob.name == "IUnknown")
	{
		//set up the necessary base functionallity of the class hierachy
		hos << "protected:" << endl;
		hos << "\tvirtual void AddRef() = 0;" << endl;
		hos << "\tvirtual void Release() = 0;" << endl;
		hos << "public:" << endl;
		hos << "virtual ~C" << m_ob.name << "(){};" << endl;
		hos << "\tvirtual IUnknown* GetIUnknown() = 0;" << endl;
		hos << endl;
		hos << "private:" << endl;
	}

	cos << "\tREFIID C" << m_ob.name << "::GetUUID()" << endl;
	cos << "\t{" << endl;
		if(!m_ob.HasValue("dual") && m_ob.parentName == "IDispatch")
			cos << "\t\treturn __uuidof(" << m_ob.name << ");" << endl;
		else
			cos << "\t\treturn __uuidof(" << m_ob.name << ");" << endl;
	cos << "\t}" << endl;
	
	hos << "\tC" << m_ob.name << "Wrapper* CreateWrapper(void* pInterface);" << endl;
	hos << "\tfriend RefCountedObject<C" << m_ob.name << ">;" << endl;
	
	hos << "public:" << endl;
	hos << "\tstatic REFIID GetUUID();" << endl;
	hos << "\ttypedef " << m_ob.name << " COM_TYPE;" << endl;

	if(m_ob.name != "IDispatch" && m_ob.name != "IUnknown")
	{
		if(m_ob.name == "IEnumUnknown")
			int i = 0;
		DumpFunctionPrototypes(hos, m_ob);
	}

	hos << "};" << endl;
	hos << endl;


	//create a COM equivelant interface wrapping smart pointer
	cos << "//create a COM equivelant interface wrapper" << endl;
	cos << "class ERRORHANDLER_API C" << m_ob.name << "Wrapper :" << endl;
	cos << "\tpublic InterfaceWrapper<" << m_ob.name << ", C" << m_ob.name << ">" << endl;

	cos << "{" << endl;
	cos << "public:" << endl;
	cos << "\tC" << m_ob.name << "Wrapper(" << m_ob.name << "* pInterface) : InterfaceWrapper<" << m_ob.name << ", C" << m_ob.name << ">(pInterface)" << endl;
	cos << "\t{}" << endl;

	if(m_ob.name == "IMalloc")
		int x = 1;
	if(m_ob.name != "IDispatch" && m_ob.name != "IUnknown")
		DumpInterfaces(m_ob, cos, m_ob.name);
	
	cos << "};" << endl;
	cos << endl;

	cos << "C" << m_ob.name << "Wrapper* C" << m_ob.name << "::CreateWrapper(void* pInterface)" << endl;
	cos << "{" << endl;
	cos << "\treturn new C" << m_ob.name << "Wrapper(static_cast<" << m_ob.name << "*>(pInterface));" << endl;
	cos << "}" << endl;
	cos << endl;


	//create a COM equivelant smart pointer
	hos << "//create a COM equivelant smart pointer" << endl;
	hos << "class ERRORHANDLER_API C" << m_ob.name << "Ptr :" << endl;
	hos << "\tpublic RefCountedObject<C" << m_ob.name << ">" << endl;

	hos << "{" << endl;

	hos << "public:" << endl;
	hos << "\tC" << m_ob.name << "Ptr()" << endl;
	hos << "\t{}" << endl;

	hos << "\tC" << m_ob.name << "Ptr(C" << m_ob.name << "* intrfc) : RefCountedObject<C" << m_ob.name << ">(intrfc)" << endl;
	hos << "\t{}" << endl;

	if(m_ob.name != "IDispatch" && m_ob.name != "IUnknown")
	{
		if(m_ob.name == "CIXMLDocument")
			int x = 1;
		DumpSPInterfaces(m_ob, hos, cos, m_ob.name, m_ob.name);
	}

	hos << "};" << endl;
	hos << endl;
}

void DumpClientWrappers(const Library& classes, ostream& hos, ostream& cos)
{
	hos << "#pragma once" << endl;
	hos << "#include \"InterfaceWrapper.h\"" << endl << endl;

	cos << "#include \"stdafx.h\"" << endl;

	set<string> processedClasses;

	hos << "//pre-define the classes" << endl;
	const CLASS_OBJECT_LIST& listOfClasses = classes.m_classes;
	CLASS_OBJECT_LIST::const_iterator it = listOfClasses.begin();
	for(;it != listOfClasses.end();it++)
	{
		const ClassObject& obj = *(*it);
		if(	(	obj.type == ObjectTypeInterface || obj.type == ObjectTypeDispInterface) && 
				processedClasses.end() == processedClasses.find(obj.name) && 
				(obj.parentName.length() || obj.name == "IUnknown") && obj.HasValue("uuid"))
		{
			hos << "class ERRORHANDLER_API C" << obj.name << ";" << endl;
			hos << "class ERRORHANDLER_API C" << obj.name << "Wrapper;" << endl;
			hos << "class ERRORHANDLER_API C" << obj.name << "Ptr;" << endl;
		}
	}

	hos << endl;

	for(it = listOfClasses.begin();it != listOfClasses.end();it++)
	{
		const ClassObject& obj = *(*it);

		if(	(	obj.type == ObjectTypeInterface || obj.type == ObjectTypeDispInterface) && 
				processedClasses.end() == processedClasses.find(obj.name) && 
				(obj.parentName.length() || obj.name == "IUnknown") && obj.HasValue("uuid"))
		{
			processedClasses.insert(obj.name);

			DumpClientWrapper(obj, hos, cos);
		}
	}

	hos << endl;
	hos.flush();
}

void CreateParamFragment(const functionInfo& func,paramInfo& param, iostream& os)
{
	const ParameterObject& po = param.GetParam();

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

void GenerateFunctionStreams(const FunctionObject& fn, iostream& baseFileFn, iostream& headerFileFn, iostream& cppFileFn, iostream& spFileFn, const string& objectName)
{

	functionInfo func(fn,fn.GetLibrary());

	if(fn.name == "createElement")
		int i = 0;
	string tabs;
	tabs = "\t";

	///////////////////////////////////////////////////////////////////////////////
	//basefile
	string returnParamName;
	string returnType  = "void";
	string smartReturnType = returnType;
	
	if(func.retval != -1)
	{
		const ParameterObject& param = func.m_params[func.retval].GetParam();
		returnType = param.type;
		size_t pos = returnType.rfind("*");
		returnType = returnType.substr(0,pos);
		returnParamName = param.name;
		smartReturnType = returnType;

		if(returnType[pos-1] == '*')
		{
			string type = returnType.substr(0,pos - 1);
			const ClassObject* pCOB = NULL;
			if(fn.GetLibrary().FindClassObject(type,pCOB) == true)
			{
				if(pCOB->type == ObjectTypeInterface || pCOB->type == ObjectTypeDispInterface)
					smartReturnType = "C" + type + "Ptr";
			}
		}
	}


	if(fn.type == FunctionTypeMethod)
		baseFileFn << "virtual " << smartReturnType << " " << fn.name << "(";
	else if(fn.type == FunctionTypePropertyPut)
		baseFileFn << "virtual " << smartReturnType << " put_" << fn.name << "(";
	else //fn.type == FunctionTypePropertyPut
		baseFileFn << "virtual " << smartReturnType << " get_" << fn.name << "(";

	if(fn.type == FunctionTypeMethod)
		spFileFn << "virtual " << smartReturnType << " " << fn.name << "(";
	else if(fn.type == FunctionTypePropertyPut)
		spFileFn << "virtual " << smartReturnType << " put_" << fn.name << "(";
	else //fn.type == FunctionTypePropertyPut
		spFileFn << "virtual " << smartReturnType << " get_" << fn.name << "(";

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.h
	if(fn.type == FunctionTypeMethod)
		headerFileFn << "STDMETHOD(" << fn.name << ")(";
	else if(fn.type == FunctionTypePropertyPut)
		headerFileFn << "STDMETHOD(put_" << fn.name << ")(";
	else //fn.type == FunctionTypePropertyPut
		headerFileFn << "STDMETHOD(get_" << fn.name << ")(";

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.cpp
	if(fn.type == FunctionTypeMethod)
		cppFileFn << "STDMETHODIMP C" << objectName << "BP::" << fn.name << "(";
	else if(fn.type == FunctionTypePropertyPut)
		cppFileFn << "STDMETHODIMP C" << objectName << "BP::put_" << fn.name << "(";
	else //fn.type == FunctionTypePropertyPut
		cppFileFn << "STDMETHODIMP C" << objectName << "BP::get_" << fn.name << "(";

	{
		bool needCommas = false;
		bool needWCommas = false;
		int pos = 0;
		for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
		{
			const ParameterObject& param = *pit;
			paramInfo& pi = func.m_params[pos];

			///////////////////////////////////////////////////////////////////////////////
			//basefile
			bool doparam = true;

			//ignore iid_is parameters
			if(func.templateCount > 0)
			{
				if(pi.iid_is != -1)
				{
					doparam = false;
				}
			}

			if(pi.isLength && pi.isOutParam)
			{
				doparam = false;
			}

			if(pi.isSize && pi.isOutParam)
			{
				doparam = false;
			}

			//ignore retval parameters as they are already handled
			if(pos == func.retval)
			{
				doparam = false;
			}

			if(doparam == true)
			{
				if(needWCommas == false)
					needWCommas = true;
				else
				{
					baseFileFn << ",";
					spFileFn << ",";
				}

				CreateParamFragment(func, pi, baseFileFn);
				CreateParamFragment(func, pi, spFileFn);
				
//				baseFileFn << param.type << " " << param.name;
//				spFileFn << param.type << " " << param.name;
			}
			
			if(needCommas == false)
				needCommas = true;
			else
			{
				headerFileFn << ",";
				cppFileFn << ",";
			}

			///////////////////////////////////////////////////////////////////////////////
			//BPfile.h
			headerFileFn << param.type << " " << param.name;

			///////////////////////////////////////////////////////////////////////////////
			//BPfile.cpp
			cppFileFn << param.type << " " << param.name;

			pos++;
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	//basefile
	baseFileFn << ") = 0;" << endl << ends;

	///////////////////////////////////////////////////////////////////////////////
	//spFile
	spFileFn << ");" << endl << ends;
/*	spFileFn << "{" << endl;
	
	spFileFn << tabs;
	if(returnParamName.length())
		spFileFn << "return ";

	spFileFn << "m_actual->";

	if(fn.type == FunctionTypeMethod)
		spFileFn << fn.name;
	else if(fn.type == FunctionTypePropertyPut)
		spFileFn << "put_" << fn.name;
	else //fn.type == FunctionTypePropertyPut
		spFileFn << "get_" << fn.name;

	spFileFn << "(";

	{ 
		bool firstloop = true;
		for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
		{
			const ParameterObject& param = *pit;

			if(!param.HasValue("retval"))
			{
				if(firstloop == true)
				{
					firstloop = false;
				}
				else
				{
					spFileFn << ", ";
				}

				spFileFn << param.name;
			}
		}
	}

	spFileFn << ");" << endl;
	spFileFn << "}" << endl << ends;*/

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.h
	headerFileFn << ");" << endl << ends;

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.cpp
	cppFileFn << ")" << endl;
	cppFileFn << "{" << endl;
	
	cppFileFn << tabs;
	if(returnParamName.length())
		cppFileFn << "function_carrier" << fn.parameters.size() - 1 << "<" << objectName << "Base, " << returnType;
	else
		cppFileFn << "void_function_carrier" << fn.parameters.size() << "<" << objectName << "Base";

	{
		for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
		{
			const ParameterObject& param = *pit;

			if(!param.HasValue("retval"))
				cppFileFn << ", " << param.type;
		}
	}

	cppFileFn << "> passer(m_pAppImpl, " << objectName << "Base::";

	if(fn.type == FunctionTypeMethod)
		cppFileFn << fn.name;
	else if(fn.type == FunctionTypePropertyPut)
		cppFileFn << "put_" << fn.name;
	else //fn.type == FunctionTypePropertyPut
		cppFileFn << "get_" << fn.name;

	if(returnParamName.length())
		cppFileFn << ", " << returnParamName;

	{
		for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
		{
			const ParameterObject& param = *pit;

			if(!param.HasValue("retval"))
				cppFileFn << ", " << param.name;
		}
	}

	cppFileFn << ");" << endl;
	cppFileFn << tabs << "TryFunc(passer);" << endl;
	cppFileFn << tabs << "return passer.m_err == 0 ? S_OK : passer.m_err;" << endl;
	cppFileFn << "}" << endl << ends;
}