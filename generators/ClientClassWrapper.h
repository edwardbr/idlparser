// lientClassWrapper.h: interface for the ClientClassWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTCLASSWRAPPER_H__F4DADA82_9AE0_4E31_817E_B7F2BAA28D57__INCLUDED_)
#define AFX_CLIENTCLASSWRAPPER_H__F4DADA82_9AE0_4E31_817E_B7F2BAA28D57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void dumpVTtype(std::string returnType, std::ostream& os);
void dumpVariantMember(std::string returnType, std::ostream& os);


/*struct ClientPrototypeWrapperFunction
{
	ClientPrototypeWrapperFunction(const FunctionObject& info) : m_fn(info)
	{}

	friend std::ostream& operator<< ( std::ostream& os, const ClientPrototypeWrapperFunction& funcs );
	const FunctionObject& m_fn;
};

std::ostream& operator<< ( std::ostream& os, const ClientPrototypeWrapperFunction& func );*/

/*struct ClientPrototypeWrapperProperty
{
	ClientPrototypeWrapperProperty(const FunctionObject& info) : m_fn(info)
	{}

	friend std::ostream& operator<< ( std::ostream& os, const ClientPrototypeWrapperProperty& funcs );
	const FunctionObject& m_fn;
};

std::ostream& operator<< ( std::ostream& os, const ClientPrototypeWrapperProperty& func );
*/
/*struct ClientWrapperFunction
{
	ClientWrapperFunction(const FunctionObject& info) : m_fn(info)
	{}

	friend std::ostream& operator<< ( std::ostream& os, const ClientWrapperFunction& funcs );
	const FunctionObject& m_fn;
};

std::ostream& operator<< ( std::ostream& os, const ClientWrapperFunction& func );*/

struct ClientWrapperProperty
{
	ClientWrapperProperty(const FunctionObject& info) : m_fn(info)
	{}

	friend std::ostream& operator<< ( std::ostream& os, const ClientWrapperProperty& funcs );
	const FunctionObject& m_fn;
};

std::ostream& operator<< ( std::ostream& os, const ClientWrapperProperty& func );


void DumpDispFunction(std::ostream& hos, std::ostream& cos, const FunctionObject& fn);
void DumpInterfaces(const ClassObject& m_ob, std::ostream& hos, std::ostream& cos, const std::string& ifName);
void DumpSPInterfaces(const ClassObject& m_ob, std::ostream& hos, std::ostream& cos, const std::string& ifName, std::string mostDerivedName);
void DumpFunctionPrototypes(const ClassObject& m_ob, std::ostream& hos, const ClassObject& obj);
void DumpClientWrapper(const ClassObject& m_ob, std::ostream& hos, std::ostream& cos);
void DumpClientWrappers(const Library& info, std::ostream& hos, std::ostream& cos);
void GenerateFunctionStreams(const FunctionObject& fn, std::iostream& baseFileFn, std::iostream& headerFileFn, std::iostream& cppFileFn, std::iostream& spFileFn, const std::string& objectName);


#endif // !defined(AFX_CLIENTCLASSWRAPPER_H__F4DADA82_9AE0_4E31_817E_B7F2BAA28D57__INCLUDED_)
