#include "commonfuncs.h"
#include <assert.h>

#ifdef WIN32
#ifdef USE_COM
#include <comdef.h>
#include <crtdbg.h>
#include "atlconv.h"
#include <atlbase.h>
#endif
#endif

struct ParameterObject;
struct FunctionObject;
struct ClassObject;
struct Library;


enum ObjectType
{
	ObjectTypeNull		=  0,
	ObjectStruct		=  1,
	ObjectEnum			=  2,
	ObjectException		=  4,
	ObjectSequence		=  8,
	ObjectTypeInterface	=  16,
	ObjectTypedef		=  32,
	ObjectCoclass		=  64,
	ObjectUnion			= 128,
	ObjectLibrary		= 256,
	ObjectTypeDispInterface	=  512,//used only temporarily not seen by client apps
	ObjectClass	=  1024,//used only temporarily not seen by client apps
	ObjectTemplate	=  2048,//used only temporarily not seen by client apps
	ObjectNamespace	=  4096,//used only temporarily not seen by client apps

	ObjectTypeFixedSize = ObjectStruct | ObjectEnum | ObjectUnion
};

typedef std::list<std::string> attributes;

struct templateParam
{
public:
	std::string type;
	std::string name;
};


//this class contains the name container and attributes used by a derived class
struct objectBase
{
protected:
	objectBase(ClassObject* pObjects, Library* pLibrary) : 
		m_pContainer(pObjects),
		m_pLibrary(pLibrary)
	{
		assert(m_pContainer);
		assert(m_pLibrary);
	}

	objectBase(const objectBase& other) : 
		m_pContainer(other.m_pContainer), 
		m_attributes(other.m_attributes), 
		name(other.name),
		m_pLibrary(other.m_pLibrary)
	{
		assert(m_pContainer);
		assert(m_pLibrary);
	}
public:
	void operator = (const objectBase& other)
	{
		assert(other.m_pContainer);
		m_attributes = other.m_attributes;
		m_pContainer = other.m_pContainer;
		name = other.name;
		m_pLibrary = other.m_pLibrary;

		assert(m_pContainer);
		assert(m_pLibrary);
	}

	ClassObject* GetContainer()
	{
		return m_pContainer;
	}
	const ClassObject* GetContainer() const 
	{
		return m_pContainer;
	}

	Library& GetLibrary()
	{
		return *m_pLibrary;
	}
	const Library& GetLibrary() const 
	{
		return *m_pLibrary;
	}

	
	const bool HasValue(const char* valueName) const 
	{
		for(std::list<std::string>::const_iterator it = m_attributes.begin(); it != m_attributes.end();it++)
			if(!Strcmp2((*it).data(), valueName))
				return true;
		return false;
	}
	const bool GetValue(const char* valueName, std::string& value) const
	{
		for(std::list<std::string>::const_iterator it = m_attributes.begin(); it != m_attributes.end();it++)
		{
			const char* name = (*it).data();
			if(!Strcmp2(name, valueName))
			{
				value = &name[strlen(valueName)];
				return true;
			}
		}
		return false;
	}
	attributes m_attributes;
	std::string name;
private:
	ClassObject* m_pContainer;
	Library* m_pLibrary;
};

std::ostream& operator<< ( std::ostream& os, attributes& attribs );

enum FunctionType
{
	FunctionTypeOther = 0,
	FunctionTypeMethod,
	FunctionTypePropertyPut,
	FunctionTypePropertyGet,
	FunctionTypeVariable,
};


struct ParameterObject : objectBase
{
	ParameterObject(ClassObject* pContainer, Library* pLibrary);
	ParameterObject(const ParameterObject& other);
	void operator = (const ParameterObject& other);
	std::string type;
	bool m_bIsMainVal;
	bool m_bIsException;
	bool m_bIsCallBack;
	std::list<std::string> m_arraySuffix;

	friend std::ostream& operator<< ( std::ostream& os, ParameterObject& params );
};
std::ostream& operator<< ( std::ostream& os, ParameterObject& params );

struct FunctionObject : objectBase
{
	FunctionObject(ClassObject* pContainer, Library* pLibrary);
	FunctionObject(const FunctionObject& other);
	void operator = (const FunctionObject& other);
	std::list<std::string> raises;
	std::string returnType;
	bool pure_virtual;
	FunctionType type;
	bool hasImpl;

	std::list<ParameterObject> parameters;
	bool m_bBadFunction;

	friend std::ostream& operator<< ( std::ostream& os, FunctionObject& funcs );
};
std::ostream& operator<< ( std::ostream& os, FunctionObject& funcs );

enum interface_spec
{
	header,
	com,
	corba
};

struct ClassObject : objectBase
{
	using ClassObjectRef = std::shared_ptr<ClassObject>;

	ClassObject(ClassObject* pContainer, Library* pLibrary, const std::string& ns, interface_spec spec = header);
	ClassObject(const ClassObject& other);

	std::string parentName;
	ObjectType type;
	std::list<FunctionObject> functions;
	std::list<std::string> m_ownedClasses;
	std::list<templateParam> m_templateParams;

	void AddClass(ClassObjectRef classObject);
	void operator = (const ClassObject& other);
	friend std::ostream& operator<< ( std::ostream& os, ClassObject& objs );
	ClassObjectRef ParseSequence(const char*& pData, attributes& attribs, const std::string& ns);
	void ParseUnion(const char*& pData, attributes& attribs);
	bool ObjectHasTypeDefs(const char* pData);
	ClassObjectRef ParseTypedef(const char*& pData, attributes& attribs, const std::string& ns, const char* type = NULL);
	bool ExtractClass(const char*& pData, attributes& attribs, ClassObjectRef& obj, const std::string& ns, bool handleTypeDefs = true);
	FunctionObject GetFunction(const char*& pData, attributes& attribs, bool bFunctionIsInterface);
	void GetVariable(const char*& pData);
	ClassObjectRef GetInterface(const char*& pData, const ObjectType type, const attributes& attr, const std::string& ns);
	void ExtractTemplate(const char*& pData, std::list<templateParam>& templateParams);
	void GetNamespaceData(const char*& pData, const std::string& ns);
	void GetStructure(const char*& pData, const std::string& ns, bool bInCurlyBrackets = false);
	bool Load(const char* file);
	void ParseAndLoad(const char*& pData, const char* file);
	bool GetFileData(const char*& pData, const char* file);

#ifdef USE_COM
	CComBSTR GetInterfaceName(ITypeInfo* typeInfo);
	void GetInterfaceAttributes(ClassObject& theClass, ITypeInfoPtr& typeInfo, unsigned short& functionCount, unsigned short& variableCount, unsigned short& implTypes);
	std::string GenerateTypeString(TYPEDESC& typedesc, ITypeInfo* typeInfo);
	void GetVariables(ClassObject& theClass, unsigned variableCount, ITypeInfo* typeInfo);
	void GetCoclassInterfaces(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo);
	void GetInterfaceFunctions(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo);
	void GetInterfaceProperties(TYPEATTR* pTypeAttr, ClassObject& obj, ITypeInfo* typeInfo);
#endif
	std::string m_namespace;
	bool recurseImport;
	bool recurseImportLib;
	interface_spec m_interface_spec;
};

std::ostream& operator<< ( std::ostream& os, ClassObject& objs );

typedef std::list<ClassObject::ClassObjectRef > CLASS_OBJECT_LIST;

struct Library : ClassObject//objectBase
{
	Library() : ClassObject(this, this, std::string())
	{}
	~Library()
	{
	}
	friend std::ostream& operator<< ( std::ostream& os, Library& objs );

	bool FindClassObject(const std::string& type, const ClassObject*& obj) const;
	CLASS_OBJECT_LIST m_classes;
	void AddClass(ClassObjectRef classObject)
	{
//		m_classes.insert(std::unordered_map<std::string, ClassObject>::value_type(classObject.name,classObject));
		m_classes.push_back(classObject);
	}
};
extern std::set<std::string> loadedFiles;

//bool CheckFor(const char*& pData, char* keyword);
std::ostream& operator<< ( std::ostream& os, Library& objs );

extern std::stringstream verboseStream;
extern int isHashImport;

struct typeInfo
{
	typeInfo() : type(ObjectTypeNull), pObj(NULL)
	{}
	ObjectType type;
	std::string prefix;
	std::string name;
	std::string suffix;
	const ClassObject* pObj;
};

std::string expandTypeString(const char* type, const Library& lib);
void getTypeStringInfo(const char* type, typeInfo& info, const Library& lib);