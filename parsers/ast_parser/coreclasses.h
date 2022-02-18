#include "commonfuncs.h"
#include <set>
#include <list>
#include <cassert>

#ifdef WIN32
#ifdef USE_COM
#include <comdef.h>
#include <crtdbg.h>
#include "atlconv.h"
#include <atlbase.h>
#endif
#endif

class parameter_entity;
class function_entity;
class class_entity;
class library_entity;


enum class entity_type
{
	TYPE_NULL			= 0,
	STRUCT				= 1,
	ENUM				= 2,
	EXCEPTION			= 4,
	SEQUENCE			= 8,
	INTERFACE			= 16,
	TYPEDEF				= 32,
	COCLASS				= 64,
	UNION				= 128,
	LIBRARY				= 256,
	DISPATCH_INTERFACE	= 512,//used only temporarily not seen by client apps
	CLASS				= 1024,//used only temporarily not seen by client apps
	TEMPLATE			= 2048,//used only temporarily not seen by client apps
	NAMESPACE			= 4096,//used only temporarily not seen by client apps
};

typedef std::list<std::string> attributes;

struct template_param
{
public:
	std::string type;
	std::string name;
};


//this class contains the name container and attributes used by a derived class
class entity
{
	class_entity* container_;
	library_entity* library_;

protected:
	attributes attributes_;
	std::string name_;

	entity(class_entity* pObjects, library_entity* pLibrary) : 
		container_(pObjects),
		library_(pLibrary)
	{
		assert(container_);
		assert(library_);
	}

	entity(const entity& other) = default;
	entity& operator = (const entity& other) = default;
public:

	class_entity* GetContainer()
	{
		return container_;
	}
	const class_entity* GetContainer() const 
	{
		return container_;
	}

	library_entity& GetLibrary()
	{
		return *library_;
	}
	const library_entity& GetLibrary() const 
	{
		return *library_;
	}
	
	const bool has_value(const char* valueName) const 
	{
		for(std::list<std::string>::const_iterator it = attributes_.begin(); it != attributes_.end();it++)
			if(!strcmp2((*it).data(), valueName))
				return true;
		return false;
	}
	const bool get_value(const char* valueName, std::string& value) const
	{
		for(std::list<std::string>::const_iterator it = attributes_.begin(); it != attributes_.end();it++)
		{
			const char* name = (*it).data();
			if(!strcmp2(name, valueName))
			{
				value = &name[strlen(valueName)];
				return true;
			}
		}
		return false;
	}

	std::string get_name() const {return name_;}
	void set_name(std::string name){name_ = name;}

	const attributes& get_attributes() const {return attributes_;}
	void set_attributes(attributes attribs){attributes_ = attribs;}
	void add_attribute(std::string attrib){attributes_.push_back(attrib);}
	void merge_attributes(attributes attribs){attributes_.merge(attribs);}
};

enum function_type
{
	FunctionTypeOther = 0,
	FunctionTypeMethod,
	FunctionTypePropertyPut,
	FunctionTypePropertyGet,
	FunctionTypeVariable,
};


class parameter_entity : public entity
{
	std::string type_;
	size_t array_size_ = 0;
	bool m_bIsMainVal = false;
	bool m_bIsException = false;
	bool is_call_back_ = false;
	std::list<std::string> array_suffixes_;

public:
	parameter_entity(class_entity* pContainer, library_entity* pLibrary);
	parameter_entity(const parameter_entity& other) = default;
	parameter_entity& operator = (const parameter_entity& other) = default;

	std::string get_type() const {return type_;}
	void set_type(std::string type){type_ = type;}

	size_t get_array_size() const {return array_size_;}
	void set_array_size(size_t size){array_size_ = size;}

	const std::list<std::string>& get_array_suffixes() const {return array_suffixes_;}
	void set_array_suffixes(std::list<std::string> suffixes){array_suffixes_ = suffixes;}
	void add_array_suffix(std::string item){array_suffixes_.push_back(item);}

	bool is_callback() const {return is_call_back_;}
	void set_callback(bool is_cb){is_call_back_ = is_cb;}
};

class function_entity : public entity
{
	std::list<std::string> raises_;
	std::string return_type_;
	size_t array_size_ = 0;
	bool pure_virtual_ = false;
	function_type type_ = FunctionTypeMethod;

	std::list<parameter_entity> parameters_;

public:
	function_entity(class_entity* pContainer, library_entity* pLibrary);
	function_entity(const function_entity& other) = default;
	function_entity& operator = (const function_entity& other) = default;

	function_type get_type() const {return type_;}
	void set_type(function_type type){type_ = type;}

	std::string get_return_type() const {return return_type_;}
	void set_return_type(std::string type){return_type_ = type;}

	bool is_pure_virtual() const {return pure_virtual_;}
	void set_pure_virtual(bool pv){pure_virtual_ = pv;}

	size_t get_array_size() const {return array_size_;}
	void set_array_size(size_t size){array_size_ = size;}

	const std::list<parameter_entity>& get_parameters() const {return parameters_;}
	void set_parameters(std::list<parameter_entity> parameters){parameters_ = parameters;}
	void add_parameter(parameter_entity parameter){parameters_.push_back(parameter);}

	const std::list<std::string>& get_raises() const {return raises_;}
	void set_raises(std::list<std::string> raisess){raises_ = raisess;}
	void add_raises(std::string raises){raises_.push_back(raises);}
};

enum interface_spec
{
	header,
	com,
	corba,
	edl
};

class class_entity : public entity
{
	std::weak_ptr<class_entity> owner_;
	std::string unidentified_owner_name_;
	entity_type type_;
	std::list<function_entity> functions_;
	std::list<std::shared_ptr<class_entity> > classes_;
	std::list<template_param> template_params_;
	bool recurseImport = true;
	bool recurseImportLib = true;
	interface_spec interface_spec_;

	bool find_class(const std::vector<std::string>& type, std::shared_ptr<class_entity>& obj) const;

public:

	class_entity(class_entity* pContainer, library_entity* pLibrary, bool is_include, interface_spec spec = header);
	class_entity(const class_entity& other) = default;
	class_entity& operator = (const class_entity& other) = default;

	entity_type get_type() const {return type_;}
	void set_type(entity_type type){type_ = type;}

	std::weak_ptr<class_entity> get_owner() const {return owner_;}
	void set_owner(std::weak_ptr<class_entity> owner){owner_ = owner;}

	std::string get_unidentified_owner_name() const {return unidentified_owner_name_;}
	void set_unidentified_owner_name(std::string name) {unidentified_owner_name_ = name;}

	const std::list<function_entity>& get_functions() const {return functions_;}
	void set_functions(std::list<function_entity> raisess){functions_ = raisess;}
	void add_function(function_entity raises){functions_.push_back(raises);}

	void add_class(std::shared_ptr<class_entity> classObject);
	const std::list<std::shared_ptr<class_entity> >& get_classes() const {return classes_;}
	

	//std::shared_ptr<class_entity> parse_sequence(const char*& pData, attributes& attribs, bool is_include);
	void parse_union(const char*& pData, attributes& attribs);
	bool has_typedefs(const char* pData);
	std::shared_ptr<class_entity> parse_typedef(const char*& pData, attributes& attribs, const char* type, bool is_include);
	bool parse_class(const char*& pData, attributes& attribs, std::shared_ptr<class_entity>& obj, bool handleTypeDefs, bool is_include);
	function_entity parse_function(const char*& pData, attributes& attribs, bool bFunctionIsInterface);
	void parse_variable(const char*& pData);
	std::shared_ptr<class_entity> parse_interface(const char*& pData, const entity_type type, const attributes& attr, bool is_include);
	void parse_template(const char*& pData, std::list<template_param>& templateParams);
	void parse_namespace(const char*& pData, bool is_include);
	void parse_structure(const char*& pData, bool bInCurlyBrackets, bool is_include);

	bool load(const char* file, bool is_include = false);
	void extract_path_and_load(const char*& pData, const char* file, bool is_include);
	bool parse_include(const char*& pData, const char* file, bool is_include);

	bool find_class(std::string type, std::shared_ptr<class_entity>& obj) const;

#ifdef USE_COM
	CComBSTR GetInterfaceName(ITypeInfo* typeInfo);
	void GetInterfaceAttributes(class_entity& theClass, ITypeInfoPtr& typeInfo, unsigned short& functionCount, unsigned short& variableCount, unsigned short& implTypes);
	std::string GenerateTypeString(TYPEDESC& typedesc, ITypeInfo* typeInfo);
	void GetVariables(class_entity& theClass, unsigned variableCount, ITypeInfo* typeInfo);
	void GetCoclassInterfaces(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo);
	void GetInterfaceFunctions(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo);
	void GetInterfaceProperties(TYPEATTR* pTypeAttr, class_entity& obj, ITypeInfo* typeInfo);
#endif
};

class library_entity : public class_entity
{
public:
	library_entity() : class_entity(this, this, false)
	{}
	~library_entity()
	{
	}


};

extern std::stringstream verboseStream;
extern int is_hash_import;

struct typeInfo
{
	typeInfo() : type(entity_type::TYPE_NULL), pObj(NULL)
	{}
	entity_type type;
	std::string prefix;
	std::string name;
	std::string suffix;
	const class_entity* pObj;
};

//std::string expandTypeString(const char* type, const library_entity& lib);
//void getTypeStringInfo(const char* type, typeInfo& info, const library_entity& lib);