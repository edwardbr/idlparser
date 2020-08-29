// lientClassWrapper.cpp: implementation of the ClientClassWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "coreclasses.h"
#include "ComObjectGen.h"
#include <strstream>
#include "ClientClassWrapper.h"

struct interfaceInfo
{
	interfaceInfo()
	{
		m_toplevel = false;
		m_bDispatch = false;
		m_bAddedToMap = false;
	}
	interfaceInfo(const interfaceInfo& other)
	{
		m_baseName = other.m_baseName;
		m_toplevel = other.m_toplevel;
		m_bDispatch = other.m_bDispatch;
		m_bAddedToMap = other.m_bAddedToMap;
	}
	void operator = (const interfaceInfo& other)
	{
		m_baseName = other.m_baseName;
		m_toplevel = other.m_toplevel;
		m_bDispatch = other.m_bDispatch;
		m_bAddedToMap = other.m_bAddedToMap;
	}
	string m_baseName;
	bool m_toplevel;
	bool m_bDispatch;
	bool m_bAddedToMap;
};

typedef std::unordered_map<string,interfaceInfo> INTERFACEINFO_MAP;

bool InterfaceIsDispatch(const INTERFACEINFO_MAP& topLevelInterfaces, const string& Name)
{
	if(Name == "IDispatch" || Name == "IDispatchEx")
		return true;
	if(Name == "IUnknown")
		return false;
	
	INTERFACEINFO_MAP::const_iterator it = topLevelInterfaces.find(Name);
	if(it != topLevelInterfaces.end())
	{
		const interfaceInfo& info = (*it).second;
		return info.m_bDispatch;
	}
	return false;
}

bool getInterfaces(INTERFACEINFO_MAP& topLevelInterfaces, const Library& lib, const string& interfaceName, bool& dispatch, bool topLevel)
{
	if(interfaceName == "IDispatch" || interfaceName == "IDispatchEx")
	{
		dispatch = true;
		return false;
	}
	if(interfaceName == "IUnknown")
		return false;

	bool cont = true;

	INTERFACEINFO_MAP::iterator it = topLevelInterfaces.find(interfaceName);
	if(it == topLevelInterfaces.end())
	{
		interfaceInfo info;
		const ClassObject* pObj = NULL;

		if(lib.FindClassObject(interfaceName,pObj))
		{
			info.m_baseName = pObj->parentName;
			info.m_toplevel = topLevel;
			if(info.m_baseName != "IUnknown")
				cont = getInterfaces(topLevelInterfaces, lib, info.m_baseName, dispatch, false);
			info.m_bDispatch = dispatch;
		}
		else
		{
			info.m_baseName;
			info.m_toplevel = topLevel;
			info.m_bDispatch = false;
		}
		topLevelInterfaces.insert(INTERFACEINFO_MAP::value_type(interfaceName, info));
	}
	else
	{
		interfaceInfo& info = (*it).second;
		info.m_toplevel &= topLevel;
		dispatch = info.m_bDispatch;
	}

	return cont;
}

void getTopLevelInterfaces(INTERFACEINFO_MAP& topLevelInterfaces, const ClassObject& object)
{
	interfaceInfo dispinfo;
	dispinfo.m_baseName = "IUnknown";
	dispinfo.m_toplevel = false;
	dispinfo.m_bDispatch = true;
	topLevelInterfaces.insert(INTERFACEINFO_MAP::value_type("IDispatch", dispinfo));

	interfaceInfo dispexinfo;
	dispexinfo.m_baseName = "IDispatch";
	dispexinfo.m_toplevel = false;
	dispexinfo.m_bDispatch = true;
	topLevelInterfaces.insert(INTERFACEINFO_MAP::value_type("IDispatchEx", dispexinfo));


	//find interfaces implemented by the co classes by using the functions list (hijacked for this purpose)
	for(list<FunctionObject>::const_iterator it = object.functions.begin();it != object.functions.end();it++)
	{
		const FunctionObject& interfaceData = *it;
		if(!interfaceData.HasValue("source"))
		{
			//spike variable used to return dispatch bool
			bool dispatch = false;
			getInterfaces(topLevelInterfaces, object.GetLibrary(), interfaceData.name, dispatch, true);
		}
	}
}

void AddInterfaceEntries(fstream& headerFile, const string& interfaceName, const string& parentInterfaceName, const Library& lib, bool isFirstDispatch, const string& tabs, INTERFACEINFO_MAP& topLevelInterfaces)
{
	if(interfaceName == "IUnknown")
		return;

	INTERFACEINFO_MAP::iterator it = topLevelInterfaces.find(interfaceName);
	if(it != topLevelInterfaces.end())
	{
		interfaceInfo& info = (*it).second;
		if(!info.m_bAddedToMap || interfaceName == "IDispatch" || interfaceName == "IDispatchEx")
		{
			info.m_bAddedToMap = true;
			if(info.m_bDispatch)
			{
				if(isFirstDispatch)
				{
					isFirstDispatch = false;
					headerFile << tabs << "LWCOM_INTERFACE_ENTRY(" << interfaceName << ")" << endl;
				}
				else
				{
					headerFile << tabs << "LWCOM_INTERFACE_ENTRY2(" << interfaceName << ", " << parentInterfaceName << ")" << endl;
				}
			}
			else
				headerFile << tabs << "LWCOM_INTERFACE_ENTRY(" << interfaceName << ")" << endl;
			AddInterfaceEntries(headerFile, info.m_baseName, interfaceName, lib, isFirstDispatch, tabs, topLevelInterfaces);
		}
	}
	else
		headerFile << tabs << "LWCOM_INTERFACE_ENTRY(" << interfaceName << ")" << endl;

}

void AddInterfaceEntries2COMMAP(fstream& headerFile, const ClassObject& object, const string& tabs, INTERFACEINFO_MAP& topLevelInterfaces)
{
	//find interfaces implemented by the co classes by using the functions list (hijacked for this purpose)
	for(list<FunctionObject>::const_iterator it = object.functions.begin();it != object.functions.end();it++)
	{
		const FunctionObject& interfaceData = *it;

		if(!interfaceData.HasValue("source"))
		{
			INTERFACEINFO_MAP::iterator it = topLevelInterfaces.find(interfaceData.name);
			if(it != topLevelInterfaces.end())
			{
				interfaceInfo& info = (*it).second;
				if(info.m_toplevel)
					AddInterfaceEntries(headerFile, interfaceData.name, "", object.GetLibrary(), true, tabs, topLevelInterfaces);
			}
			else
				AddInterfaceEntries(headerFile, interfaceData.name, "", object.GetLibrary(), true, tabs, topLevelInterfaces);
		}
	}
}

void CreateComObject(const ClassObject& object, string& outputPath)
{
	string headerFileName(outputPath);
	string cppFileName(outputPath);
	string baseFileName(outputPath);

	headerFileName += "\\" + object.name + "BP.h";
	cppFileName += "\\" + object.name + "BP.cpp";
	baseFileName += "\\" + object.name + "Base.h";

	fstream headerFile(headerFileName.data(),ios::out);
	fstream cppFile(cppFileName.data(),ios::out);
	fstream baseFile(baseFileName.data(),ios::out);

	string tabs;
	tabs = "\t";

	string objNameUpr(object.name.data());
	//quick way to make an upper case string
	_strupr((char*)objNameUpr.data());

	//generate some metadata info
	INTERFACEINFO_MAP topLevelInterfaces;
	getTopLevelInterfaces(topLevelInterfaces, object);

	///////////////////////////////////////////////////////////////////////////////
	//basefile
	baseFile << "#ifndef __" << objNameUpr << "BASE_H_" << endl;
	baseFile << "#define __" << objNameUpr << "BASE_H_" << endl << endl;
	baseFile << "class C" << object.name << "BP;" << endl << endl;

	baseFile << "class " << object.name << "Base : " << endl;

	{
		bool needComma = false;
		for(INTERFACEINFO_MAP::iterator it = topLevelInterfaces.begin();it != topLevelInterfaces.end();it++)
		{
			const interfaceInfo& info = (*it).second;
			if(info.m_toplevel)
			{
				if(needComma == false)
					needComma = true;
				else
					baseFile << "," << endl;

				baseFile << tabs << "public C" << (*it).first;
			}
		}
		baseFile << endl;
	}

	baseFile << "{" << endl;
	baseFile << "public:" << endl;
	baseFile << tabs << "C" << object.name << "BP* m_pBoilerPlate;" << endl;
	baseFile << tabs << object.name << "Base(C" << object.name << "BP* pBoilerPlate) : m_pBoilerPlate(pBoilerPlate)" << endl;
	baseFile << tabs << "{};" << endl << endl;

	baseFile << tabs << "//developer needs to implement this static function" << endl;
	baseFile << tabs << "static " << object.name << "Base* CreateInstance(C" << object.name << "BP* pComObjectBP);" << endl << endl;

	baseFile << tabs << "BEGIN_LWCOM_MAP(" << object.name << "Base)" << endl;
	AddInterfaceEntries2COMMAP(baseFile, object, tabs + tabs, topLevelInterfaces);
	baseFile << tabs << "END_LWCOM_MAP()" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.h
	headerFile << "#ifndef __" << objNameUpr << "BP_H_" << endl;
	headerFile << "#define __" << objNameUpr << "BP_H_" << endl << endl;
	headerFile << "#include \"resource.h\"       // main symbols" << endl;

	headerFile << "#include \"" << object.name << "Base.h\"" << endl << endl;

	headerFile << "/////////////////////////////////////////////////////////////////////////////" << endl;
	headerFile << "// C" << object.name << "BP" << endl << endl;

	headerFile << "class ATL_NO_VTABLE C" << object.name << "BP" << " : " << endl;

	headerFile << tabs << "public CComObjectRootEx<CComMultiThreadModel>," << endl;
	headerFile << tabs << "public CComCoClass<C" << object.name << "BP" << ", &CLSID_" << object.name << ">," << endl;
	headerFile << tabs << "public ISupportErrorInfo," << endl;
	headerFile << tabs << "public IConnectionPointContainerImpl<C" << object.name << "BP" << ">," << endl;

	//iterate through all the interfaces to declare them in the base class list
	{
		bool needCommas = false;
		//find interfaces implemented by the co classes by using the functions list (hijacked for this purpose)
		for(list<FunctionObject>::const_iterator it = object.functions.begin();it != object.functions.end();it++)
		{
			const FunctionObject& interfaceData = *it;

			//print interfaces that are not event sinks
			if(!interfaceData.HasValue("source"))
			{
				//print interfaces that are toplevel only
				bool printInterface = true;
				bool distInterface = false;

				INTERFACEINFO_MAP::iterator it = topLevelInterfaces.find(interfaceData.name);
				if(it != topLevelInterfaces.end())
				{
					interfaceInfo& info = (*it).second;
					printInterface = info.m_toplevel;
					distInterface = info.m_bDispatch;
				}
				
				
				if(printInterface)
				{
					if(needCommas == false)
						needCommas = true;
					else
						headerFile << "," << endl;

					if(distInterface)
						headerFile << tabs << "public IDispatchImpl<" << interfaceData.name << ", &IID_" << interfaceData.name << ", &LIBID_TESTAPPLib>";
					else
						headerFile << tabs << "public " << interfaceData.name;
				}
			}
		}
		headerFile << endl;
	}
	headerFile << "{" << endl;

	headerFile << "protected:" << endl;
	headerFile << tabs << object.name << "Base* m_pAppImpl;" << endl;

	headerFile << "public:" << endl;

	headerFile << tabs << "static C" << object.name << "BP* CreateComWrapper(" << object.name << "Base* pAppImpl);" << endl;

	headerFile << tabs << "C" << object.name << "BP();" << endl;

	headerFile << tabs << "HRESULT FinalConstruct();" << endl;
	headerFile << tabs << "HRESULT FinalRelease();" << endl << endl;

	headerFile << "DECLARE_REGISTRY_RESOURCEID(IDR_" << objNameUpr << ")" << endl << endl;

	headerFile << "DECLARE_PROTECT_FINAL_CONSTRUCT()" << endl << endl;

	headerFile << "BEGIN_COM_MAP(C" << object.name << "BP" << ")" << endl;

	AddInterfaceEntries2COMMAP(headerFile, object, tabs, topLevelInterfaces);

	headerFile << tabs << "COM_INTERFACE_ENTRY(ISupportErrorInfo)" << endl;
	headerFile << tabs << "COM_INTERFACE_ENTRY(IConnectionPointContainer)" << endl;
	headerFile << "END_COM_MAP()" << endl << endl;
	headerFile << "BEGIN_CONNECTION_POINT_MAP(C" << object.name << "BP" << ")" << endl;
	headerFile << "END_CONNECTION_POINT_MAP()" << endl << endl;
	headerFile << "// ISupportsErrorInfo" << endl;
	headerFile << tabs << "STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.cpp
	cppFile << "#include \"stdafx.h\"" << endl;
	cppFile << "#include \"" << object.name << "BP.h\"" << endl << endl;

	cppFile << "C" << object.name << "BP::C" << object.name << "BP() : m_pAppImpl(NULL)" << endl;
	cppFile << "{" << endl;
	cppFile << "}" << endl << endl;

	cppFile << "C" << object.name << "BP* C" << object.name << "BP::CreateComWrapper(" << object.name << "Base* pAppImpl)" << endl;
	cppFile << "{" << endl;
	cppFile << tabs << "CComObject<C" << object.name << "BP>* pObj = new CComObject<C" << object.name << "BP>();" << endl;
	cppFile << tabs << "pObj->m_pAppImpl = pAppImpl;" << endl;
	cppFile << tabs << "pAppImpl->m_pBoilerPlate = pObj;" << endl;
	cppFile << tabs << "return pObj;" << endl;
	cppFile << "}" << endl << endl;

	cppFile << "HRESULT C" << object.name << "BP::FinalConstruct()" << endl;
	cppFile << "{" << endl;
	cppFile << tabs << "//creates an instance of a C" << object.name << "BP type class" << endl;
	cppFile << tabs << "m_pAppImpl = " << object.name << "Base::CreateInstance(this);" << endl;
	cppFile << tabs << "m_pAppImpl->AddRef();" << endl;
	cppFile << tabs << "return S_OK;" << endl;
	cppFile << "}" << endl << endl;

	cppFile << "HRESULT C" << object.name << "BP::FinalRelease()" << endl;
	cppFile << "{" << endl;
	cppFile << tabs << "m_pAppImpl->Release();" << endl;
	cppFile << tabs << "return S_OK;" << endl;
	cppFile << "}" << endl << endl;

	cppFile << "STDMETHODIMP C" << object.name << "BP::InterfaceSupportsErrorInfo(REFIID riid)" << endl;
	cppFile << "{" << endl;
	cppFile << tabs << "static const IID* arr[] = " << endl;
	cppFile << tabs << "{" << endl;

	//iterate through all the interfaces and add them to the InterfaceSupportsErrorInfo
	{
		bool needCommas = false;
		//find interfaces implemented by the co classes by using the functions list (hijacked for this purpose)
		for(list<FunctionObject>::const_iterator it = object.functions.begin();it != object.functions.end();it++)
		{
			const FunctionObject& interfaceData = *it;
			if(!interfaceData.HasValue("source"))
			{
				const ClassObject* interfaceRef = NULL;
				//having got the name of an interface get it's metadata
				if(object.GetLibrary().FindClassObject(interfaceData.name,interfaceRef))
				{
					if(!interfaceRef->HasValue("source"))
						cppFile << tabs << tabs << "&IID_" << interfaceRef->name << "," << endl;
				}
			}
		}
	}
	
	cppFile << tabs << "};" << endl;
	cppFile << tabs << "for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)" << endl;
	cppFile << tabs << "{" << endl;
	cppFile << tabs << tabs << "if (InlineIsEqualGUID(*arr[i],riid))" << endl;
	cppFile << tabs << tabs <<tabs << "return S_OK;" << endl;
	cppFile << tabs << "}" << endl;
	cppFile << tabs << "return S_FALSE;" << endl;
	cppFile << "}" << endl << endl;

	//find interfaces implemented by the co classes by using the functions list (hijacked for this purpose)
	{
		set<string> functions;
		for(list<FunctionObject>::const_iterator it = object.functions.begin();it != object.functions.end();it++)
		{
			const FunctionObject& interfaceData = *it;
			if(!interfaceData.HasValue("source"))
			{
				const ClassObject* interfaceRef = NULL;
				//having got the name of an interface get it's metadata
				if(object.GetLibrary().FindClassObject(interfaceData.name,interfaceRef))
				{
					///////////////////////////////////////////////////////////////////////////////
					//basefile

					///////////////////////////////////////////////////////////////////////////////
					//BPfile.h
					headerFile << "//" << interfaceRef->name << endl;
					headerFile << "public:" << endl;

					///////////////////////////////////////////////////////////////////////////////
					//BPfile.cpp

					for(list<FunctionObject>::const_iterator fit = interfaceRef->functions.begin();fit != interfaceRef->functions.end();fit++)
					{
						const FunctionObject& fn = *fit;

						strstream baseFileFn;
						strstream headerFileFn;
						strstream cppFileFn;
						strstream spFileFn;
	
						GenerateFunctionStreams(fn, baseFileFn, headerFileFn, cppFileFn, spFileFn, object.name);

/*						///////////////////////////////////////////////////////////////////////////////
						//basefile
						string returnParamName;
						string returnType  = "void";
						for(list<ParameterObject>::const_iterator pit = fn.parameters.begin(); pit != fn.parameters.end();pit++)
						{
							const ParameterObject& param = *pit;
							if(param.HasValue("retval"))
							{
								returnType = param.type;
								int pos = returnType.rfind("*");
								returnType = returnType.substr(0,pos);
								returnParamName = param.name;
								break;
							}
						}

						if(fn.type == FunctionTypeMethod)
							baseFileFn << "virtual " << returnType << " " << fn.name << "(";
						else if(fn.type == FunctionTypePropertyPut)
							baseFileFn << "virtual " << returnType << " put_" << fn.name << "(";
						else //fn.type == FunctionTypePropertyPut
							baseFileFn << "virtual " << returnType << " get_" << fn.name << "(";

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
							cppFileFn << "STDMETHODIMP C" << object.name << "BP::" << fn.name << "(";
						else if(fn.type == FunctionTypePropertyPut)
							cppFileFn << "STDMETHODIMP C" << object.name << "BP::put_" << fn.name << "(";
						else //fn.type == FunctionTypePropertyPut
							cppFileFn << "STDMETHODIMP C" << object.name << "BP::get_" << fn.name << "(";

						{
							bool needCommas = false;
							for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
							{
								const ParameterObject& param = *pit;

								if(needCommas == false)
									needCommas = true;
								else
								{
									if(!param.HasValue("retval"))
										baseFileFn << ",";
									headerFileFn << ",";
									cppFileFn << ",";
								}

								///////////////////////////////////////////////////////////////////////////////
								//basefile
								if(!param.HasValue("retval"))
									baseFileFn << param.type << " " << param.name;

								///////////////////////////////////////////////////////////////////////////////
								//BPfile.h
								headerFileFn << param.type << " " << param.name;

								///////////////////////////////////////////////////////////////////////////////
								//BPfile.cpp
								cppFileFn << param.type << " " << param.name;
							}
						}
						///////////////////////////////////////////////////////////////////////////////
						//basefile
						baseFileFn << ") = 0;" << endl << ends;

						///////////////////////////////////////////////////////////////////////////////
						//BPfile.h
						headerFileFn << ");" << endl << ends;

						///////////////////////////////////////////////////////////////////////////////
						//BPfile.cpp
						cppFileFn << ")" << endl;
						cppFileFn << "{" << endl;
						
						cppFileFn << tabs;
						if(returnParamName.length())
							cppFileFn << "function_carrier" << fn.parameters.size() - 1 << "<" << object.name << "Base, " << returnType;
						else
							cppFileFn << "void_function_carrier" << fn.parameters.size() << "<" << object.name << "Base";

						{
							for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
							{
								const ParameterObject& param = *pit;

								if(!param.HasValue("retval"))
									cppFileFn << ", " << param.type;
							}
						}

						cppFileFn << "> passer(m_pAppImpl, " << object.name << "Base::";

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

*/					
						if(functions.find(baseFileFn.str()) == functions.end())
						{
							baseFile << tabs << baseFileFn.str();
							headerFile << tabs << headerFileFn.str();
							cppFile << cppFileFn.str() << endl;
							functions.insert(baseFileFn.str());
						}
						else
						{
							baseFile << tabs << "//" << baseFileFn.str();
							headerFile << tabs << "//" << headerFileFn.str();
							cppFile << "/*" << cppFileFn.str() << "*/" << endl;
						}
					}
					///////////////////////////////////////////////////////////////////////////////
					//basefile

					///////////////////////////////////////////////////////////////////////////////
					//BPfile.h
					headerFile << endl;

					///////////////////////////////////////////////////////////////////////////////
					//BPfile.cpp
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	//basefile
	baseFile << "};" << endl;
	baseFile << "#endif __" << objNameUpr << "BASE_H_" << endl << endl;

	///////////////////////////////////////////////////////////////////////////////
	//BPfile.h
	headerFile << "};" << endl;
	headerFile << "#endif // __" << objNameUpr << "BP_H_" << endl;


	///////////////////////////////////////////////////////////////////////////////
	//BPfile.cpp

}

void CreateComObjects(const Library& objects, string& outputPath)
{
	///////////////////////////////////////////////////////////////////////////////
	//ObjectMap.h
	string objectMapFileName(outputPath);
	objectMapFileName += "\\ObjectMap.h";
	fstream objectMapFile(objectMapFileName.data(),ios::out);
	objectMapFile << "//This file provides a list of COM class macros used in the instanciation of COM objects" << endl;

	///////////////////////////////////////////////////////////////////////////////
	//BPList.h
	string BPListFileName(outputPath);
	BPListFileName += "\\BPList.h";
	fstream BPListFile(BPListFileName.data(),ios::out);
	BPListFile << "//This file provides a list of COM class macros used in the instanciation of COM objects" << endl;

		
	set<string> processedClasses;

	const CLASS_OBJECT_LIST& listOfClasses = objects.m_classes;
	for(CLASS_OBJECT_LIST::const_iterator it = listOfClasses.begin();it != listOfClasses.end();it++)
	{
		const ClassObject& obj = *(*it);

		if(obj.type == ObjectCoclass && processedClasses.end() == processedClasses.find(obj.name))
		{
			///////////////////////////////////////////////////////////////////////////////
			//ObjectMap.h
			objectMapFile << "OBJECT_ENTRY(CLSID_" << obj.name << ", C" << obj.name << "BP)" << endl;

			///////////////////////////////////////////////////////////////////////////////
			//BPList.h
			BPListFile << "#include \"" << obj.name << "BP.h\"" << endl;


			///////////////////////////////////////////////////////////////////////////////
			//dump other files
			processedClasses.insert(obj.name);
			CreateComObject(obj,outputPath);
		}
	}
}
