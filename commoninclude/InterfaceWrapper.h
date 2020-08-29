/*#ifdef ERRORHANDLER_EXPORTS
	#define ERRORHANDLER_API __declspec(dllexport)
	#define EXP_ERRORHANDLER 
#else
	#define ERRORHANDLER_API __declspec(dllimport)
	#define EXP_ERRORHANDLER extern
	//disable warnings on extern before template instantiation
    #pragma warning (disable : 4231)
    #pragma warning (disable : 4275)
	#ifdef _DEBUG
		#pragma comment(lib,"errorhandlerd.lib")
	#else
		#pragma comment(lib,"errorhandler.lib")
	#endif
#endif*/
#define ERRORHANDLER_API
#define EXP_ERRORHANDLER 

//Some COM MAP stuff for base interface classes
#define BEGIN_LWCOM_MAP(x) public: \
	typedef x _ComMapClass; \
	IUnknown* _GetRawUnknown() \
	{ ATLASSERT(_GetEntries()[0].pFunc == _ATL_SIMPLEMAPENTRY); return (IUnknown*)((int)this+_GetEntries()->dw); } \
	IUnknown* GetUnknown() {return _GetRawUnknown();}\
	const static _ATL_INTMAP_ENTRY* WINAPI _GetEntries() { \
	static const _ATL_INTMAP_ENTRY _entries[] = { DEBUG_QI_ENTRY(x)


#ifndef _ATL_NO_UUIDOF
#define _ATL_IIDOF(x) __uuidof(x)
#else
#define _ATL_IIDOF(x) IID_##x
#endif

#define LWCOM_INTERFACE_ENTRY_BREAK(x)\
	{&_ATL_IIDOF(x), \
	NULL, \
	_Break},

#define LWCOM_INTERFACE_ENTRY_NOINTERFACE(x)\
	{&_ATL_IIDOF(x), \
	NULL, \
	_NoInterface},

#define LWCOM_INTERFACE_ENTRY(x)\
	{&_ATL_IIDOF(x), \
	offsetofclass(C##x, _ComMapClass), \
	_ATL_SIMPLEMAPENTRY},

#define LWCOM_INTERFACE_ENTRY_IID(iid, x)\
	{&iid,\
	offsetofclass(C##x, _ComMapClass),\
	_ATL_SIMPLEMAPENTRY},

// The impl macros are now obsolete
#define LWCOM_INTERFACE_ENTRY_IMPL(x)\
	COM_INTERFACE_ENTRY_IID(_ATL_IIDOF(x), C##x##Impl<_ComMapClass>)

#define LWCOM_INTERFACE_ENTRY_IMPL_IID(iid, x)\
	COM_INTERFACE_ENTRY_IID(iid, C##x##Impl<_ComMapClass>)
//

#define LWCOM_INTERFACE_ENTRY2(x, x2)\
	{&_ATL_IIDOF(x),\
	(DWORD)((C##x*)(C##x2*)((_ComMapClass*)8))-8,\
	_ATL_SIMPLEMAPENTRY},

#define LWCOM_INTERFACE_ENTRY2_IID(iid, x, x2)\
	{&iid,\
	(DWORD)((C##x*)(C##x2*)((_ComMapClass*)8))-8,\
	_ATL_SIMPLEMAPENTRY},

#define LWCOM_INTERFACE_ENTRY_FUNC(iid, dw, func)\
	{&iid, \
	dw, \
	func},

#define LWCOM_INTERFACE_ENTRY_FUNC_BLIND(dw, func)\
	{NULL, \
	dw, \
	func},

#define LWCOM_INTERFACE_ENTRY_TEAR_OFF(iid, x)\
	{&iid,\
	(DWORD)&_CComCreatorData<\
		CComInternalCreator< CComTearOffObject< C##x > >\
		>::data,\
	_Creator},

#define LWCOM_INTERFACE_ENTRY_CACHED_TEAR_OFF(iid, x, punk)\
	{&iid,\
	(DWORD)&_CComCacheData<\
		CComCreator< CComCachedTearOffObject< C##x > >,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define LWCOM_INTERFACE_ENTRY_AGGREGATE(iid, punk)\
	{&iid,\
	(DWORD)offsetof(_ComMapClass, punk),\
	_Delegate},

#define LWCOM_INTERFACE_ENTRY_AGGREGATE_BLIND(punk)\
	{NULL,\
	(DWORD)offsetof(_ComMapClass, punk),\
	_Delegate},

#define LWCOM_INTERFACE_ENTRY_AUTOAGGREGATE(iid, punk, clsid)\
	{&iid,\
	(DWORD)&_CComCacheData<\
		CComAggregateCreator<_ComMapClass, &clsid>,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define LWCOM_INTERFACE_ENTRY_AUTOAGGREGATE_BLIND(punk, clsid)\
	{NULL,\
	(DWORD)&_CComCacheData<\
		CComAggregateCreator<_ComMapClass, &clsid>,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define LWCOM_INTERFACE_ENTRY_CHAIN(classname)\
	{NULL,\
	(DWORD)&_CComChainData<classname, _ComMapClass>::data,\
	_Chain},


#define END_LWCOM_MAP() {NULL, 0, 0}}; return _entries;} \
	virtual void AddRef() = 0; \
	virtual void Release() = 0; \
	virtual void QueryInterface(REFIID iid, void** ppvObject) \
	{ \
		/* First entry in the com map should be a simple map entry*/ \
		HRESULT hr = AtlInternalQueryInterface(this, _GetEntries(), iid, ppvObject); \
		if(FAILED(hr)) \
			*ppvObject = NULL; \
	} 

//some exception macros
#ifdef NO_EXCEPTIONS
	#define TRY {
	#define CATCH } if(0) {
	#define CATCH_PARAM(a) } if(0) {a;
#else
	#define TRY try {
	#define CATCH } catch(...) {
	#define CATCH_PARAM(a) } catch(a) {
#endif
	#define END_CATCH }

class ERRORHANDLER_API ExplainDest
{
public:
	ExplainDest()
	{}
	virtual ~ExplainDest()
	{}
	virtual void WriteExplanation(LPCTSTR data)
	{
		ATLTRACE(data);
	}
};

class ERRORHANDLER_API ReasonDatabase
{
public:
	ReasonDatabase()
	{}
	virtual ~ReasonDatabase()
	{}
	virtual GetMessage(LCID, long, const wchar_t*, BSTR*)
	{}
};

class ERRORHANDLER_API Exception
{
public:
	Exception()
	{}
	virtual ~Exception()
	{}
	virtual void Explain(ExplainDest& explain,ReasonDatabase& db, LCID locale) = 0;
};

class ERRORHANDLER_API SE_Exception : public Exception
{
private:
    DWORD m_nSE;
public:    
	SE_Exception(DWORD se) : m_nSE(se)
	{
		throw new SE_Exception(*this);
	}
	SE_Exception(const SE_Exception& other) : m_nSE(other.m_nSE)
	{}
	virtual ~SE_Exception() {}
    unsigned int getSeNumber() 
	{ 
		return m_nSE; 
	}
	void Explain(ExplainDest& explain,ReasonDatabase&, LCID /* should be using this but nto sure which error codes to use*/)
	{
		switch(m_nSE)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			explain.WriteExplanation(_T("ACCESS_VIOLATION\n"));    
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			explain.WriteExplanation(_T("ARRAY_BOUNDS_EXCEEDED\n"));    
			break;
		case EXCEPTION_BREAKPOINT:
			explain.WriteExplanation(_T("BREAKPOINT\n"));    
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			explain.WriteExplanation(_T("DATATYPE_MISALIGNMENT\n"));    
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			explain.WriteExplanation(_T("FLT_DENORMAL_OPERAND\n"));    
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			explain.WriteExplanation(_T("FLT_DIVIDE_BY_ZERO\n"));    
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			explain.WriteExplanation(_T("FLT_INEXACT_RESULT\n"));    
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			explain.WriteExplanation(_T("FLT_INVALID_OPERATION\n"));    
			break;
		case EXCEPTION_FLT_OVERFLOW:
			explain.WriteExplanation(_T("FLT_OVERFLOW\n"));    
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			explain.WriteExplanation(_T("FLT_STACK_CHECK\n"));    
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			explain.WriteExplanation(_T("FLT_UNDERFLOW\n"));    
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			explain.WriteExplanation(_T("ILLEGAL_INSTRUCTION\n"));    
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			explain.WriteExplanation(_T("IN_PAGE_ERROR\n"));    
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			explain.WriteExplanation(_T("INT_DIVIDE_BY_ZERO\n"));    
			break;
		case EXCEPTION_INT_OVERFLOW:
			explain.WriteExplanation(_T("INT_OVERFLOW\n"));    
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			explain.WriteExplanation(_T("INVALID_DISPOSITION\n"));    
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			explain.WriteExplanation(_T("NONCONTINUABLE_EXCEPTION\n"));    
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			explain.WriteExplanation(_T("PRIV_INSTRUCTION\n"));    
			break;
		case EXCEPTION_SINGLE_STEP:
			explain.WriteExplanation(_T("SINGLE_STEP\n"));    
			break;
		case EXCEPTION_STACK_OVERFLOW:
			explain.WriteExplanation(_T("STACK_OVERFLOW\n"));    
			break;
		default:
			explain.WriteExplanation(_T("UNDEFINED EXCEPTION\n"));    
			break;
		}
	}
};

class ERRORHANDLER_API ComException : public Exception
{
public:
	ComException(HRESULT hresult = S_OK, IErrorInfo* pErrorInfo = NULL)
	{
		m_hresult = hresult;
		m_pErrorInfo = pErrorInfo;
		if(m_pErrorInfo)
			m_pErrorInfo->AddRef();
		throw new ComException(*this);
	}
	virtual ~ComException()
	{
		if(m_pErrorInfo)
			m_pErrorInfo->Release();
	}

	enum 
	{
		WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
		WCODE_HRESULT_LAST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF+1, 0) - 1
	};
	
	virtual void Explain(ExplainDest& explain,ReasonDatabase&, LCID locale)
	{
		if(m_pErrorInfo == NULL)
		{
			TCHAR* errormessage = 0;
			FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							m_hresult,
							LANGIDFROMLCID(locale), 
							errormessage,
							0,
							NULL );
			if (errormessage != NULL) 
			{
				int nLen = lstrlen(errormessage);
				if (nLen > 1 && errormessage[nLen - 1] == '\n') 
				{
					errormessage[nLen - 1] = 0;
					if (errormessage[nLen - 2] == '\r') 
							errormessage[nLen - 2] = 0;
				}
			} 
			else 
			{
				errormessage = (LPTSTR)LocalAlloc(0, 32 * sizeof(TCHAR));
				if (errormessage != NULL) 
				{
					long wCode = (m_hresult >= WCODE_HRESULT_FIRST && m_hresult <= WCODE_HRESULT_LAST) ? WORD(m_hresult - WCODE_HRESULT_FIRST) : 0;
					if (wCode != 0) 
						wsprintf(errormessage, TEXT("IDispatch error #%d"), wCode);
					else 
						wsprintf(errormessage, TEXT("Unknown error 0x%0lX"), m_hresult);
				}
			}
			if (errormessage != NULL) 
			{
				explain.WriteExplanation(errormessage);
				LocalFree((HLOCAL)errormessage);
			}
			else
				explain.WriteExplanation(_T("Seriously out of memory!!"));
		}
		else
		{
			CComBSTR desc;
			m_pErrorInfo->GetDescription(&desc);
			USES_CONVERSION;
			explain.WriteExplanation(OLE2T(desc));
		}
	}
	ComException(ComException& other)
	{
		*this = other;
	}
	void operator = (ComException& exception)
	{
		m_hresult = exception.m_hresult;
		m_pErrorInfo = exception.m_pErrorInfo;
		if(m_pErrorInfo)
			m_pErrorInfo->AddRef();
	}
	HRESULT GetHResult()
	{
		return m_hresult;
	}
protected:
	HRESULT m_hresult;
	IErrorInfo* m_pErrorInfo;
};

class ERRORHANDLER_API SystemException : public ComException
{
public:
	SystemException(DWORD err = NULL)
	{
		if(!err)
			err = GetLastError();
		m_hresult = HRESULT_FROM_WIN32(err);
		m_pErrorInfo = NULL;
		ComException(*this);
	}
};

class ERRORHANDLER_API ResourceException : public Exception
{
public:
	ResourceException(UINT id, HINSTANCE inst) : m_id(id), m_inst(inst)
	{
		throw new ResourceException(*this);
	}
	~ResourceException()
	{
	}
	
	virtual void Explain(ExplainDest& explain,ReasonDatabase&, LCID locale)
	{
		TCHAR buf[MAX_PATH];
		buf[0] = '\0';
		if(LoadString(m_inst, m_id, buf, MAX_PATH))
			explain.WriteExplanation(buf);
		else
		{
			DWORD err = GetLastError();
			FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							err,
							LANGIDFROMLCID(locale), 
							buf,
							MAX_PATH,
							NULL );
			explain.WriteExplanation(buf);
		}
	}
	ResourceException(ResourceException& other)
	{
		*this = other;
	}
	void operator = (ResourceException& exception)
	{
		m_id = exception.m_id;
		m_inst = exception.m_inst;
	}
private:
	UINT m_id;
	HINSTANCE m_inst;
};

class ERRORHANDLER_API ComMemAllocProtect
{
public:
	ComMemAllocProtect(void* pData)
	{
		m_pData = pData;
	}
	~ComMemAllocProtect()
	{
		if(m_pData)
			CoTaskMemFree(m_pData);
	}
private:
	void* m_pData;
};

template <class T> 
class ERRORHANDLER_API HeapItemProtect
{
public:
	operator const T*() const
	{
		return m_pItem;
	}
	T* data() 
	{
		return m_pItem;
	}
/*	T* operator->() 
	{
		return m_pItem;
	}*/
	HeapItemProtect(T* pItem)
	{
		m_pItem = pItem;
	}
	T* Detach()
	{
		T* pItem = m_pItem;
		m_pItem = NULL;
		return m_pItem;
	}
	void Attach(T* pItem)
	{
		m_pItem = pItem;
	}
	T* GetData()
	{
		return m_pItem;
	}
	virtual ~HeapItemProtect()
	{
		if(m_pItem)
		{
			delete m_pItem;
			m_pItem = NULL;
		}
	}
protected:
	T* m_pItem;
};

template <class T>
class ERRORHANDLER_API HeapVectorProtect : public HeapItemProtect<T>
{
public:
	HeapVectorProtect(T* pItem) : HeapItemProtect<T>(pItem)
	{
	}
	~HeapVectorProtect()
	{
		if(m_pItem)
		{
			delete [] m_pItem;
			m_pItem = NULL;
		}
	}
};

class ERRORHANDLER_API ProtectHandle
{
public:
	operator HANDLE()
	{
		return m_handle;
	}
	static ProtectHandle ProtectHandle::CreateFile(LPCTSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		HANDLE handle = ::CreateFile(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes, hTemplateFile);
		if(INVALID_HANDLE_VALUE == handle)
			SystemException();
		return ProtectHandle(handle);
	}
	ProtectHandle(HANDLE handle = NULL) : m_handle(handle)
	{
		if(INVALID_HANDLE_VALUE == handle)
			SystemException();
	}
	ProtectHandle(ProtectHandle& other) : m_handle(NULL)
	{
		*this = other;
	}
	void operator = (HANDLE other)
	{
		if(INVALID_HANDLE_VALUE == other)
			SystemException();
		m_handle = other;
	}
	void Attach(HANDLE other)
	{
		if(INVALID_HANDLE_VALUE == other)
			SystemException();
		m_handle = other;
	}
	void operator = (ProtectHandle& other)
	{
		if(m_handle)
			CloseHandle(m_handle);

		if(!DuplicateHandle(GetCurrentProcess(),other.m_handle,GetCurrentProcess(),&m_handle,NULL,TRUE,DUPLICATE_SAME_ACCESS))
			SystemException();
	}

	virtual ~ProtectHandle()
	{
		if(m_handle)
		{
			TRY
				CloseHandle(m_handle);
			CATCH
			//we don't throw exceptions in a destructor
			END_CATCH
		}
	}

protected:
	HANDLE m_handle;
};

class ERRORHANDLER_API ProtectFindFileHandle : public ProtectHandle
{
public:
	static ProtectFindFileHandle ProtectFindFileHandle::FindFirstFile(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileData)
	{
		HANDLE handle = ::FindFirstFile(lpFileName,lpFindFileData);
		if(INVALID_HANDLE_VALUE == handle)
			SystemException();
		return ProtectFindFileHandle(handle);
	}
	ProtectFindFileHandle(HANDLE handle) : ProtectHandle(handle){}
	ProtectFindFileHandle(ProtectFindFileHandle& other) : ProtectHandle(other.m_handle){}

	virtual ~ProtectFindFileHandle() 
	{
		if(m_handle)
		{
			TRY
				FindClose(m_handle);
			CATCH
			//we don't throw exceptions in a destructor
			END_CATCH
			m_handle = NULL;//we don't want another exception being thrown by 
		}
	}
};

class  CIUnknown;

static HRESULT Test(HRESULT hRes)
{
	if(FAILED(hRes))
		ComException(hRes);
	return hRes;
}

class RefCountedObjectBase
{
public:
	virtual REFIID GetUUID() = 0;
	virtual void Attach(void* pUnk) = 0;
};

template<class T>
class RefCountedObject : public T, public RefCountedObjectBase
{
protected:

	RefCountedObject()
	{
		m_actual = NULL;
		m_count = 0;
	}
	RefCountedObject(T* actual) : m_actual(actual), m_count(0)
	{
		if(m_actual)
		{
			m_actual->AddRef();
			m_count = 1;
		}
	}
	RefCountedObject(const RefCountedObject& other) : m_actual(other.m_actual), m_count(0)
	{
		if(m_actual)
		{
			m_actual->AddRef();
			m_count = 1;
		}
	}
	~RefCountedObject()
	{}

	//for attaching raw COM pointers to Wrapped COM pointers
	virtual void Attach(void* pUnk)
	{
		Attach((T::COM_TYPE*) pUnk);
	}
	void Attach(T* pUnk)
	{
		InnerRelease();
		m_actual = pUnk;
		if(m_actual)
		{
			m_actual->AddRef();
			m_count = 1;
		}
	}
	//repeated here to resolve virtual inheritance paths
	virtual REFIID GetUUID()
	{
		return T::GetUUID();
	}

	T* m_actual;
protected:
	void AddRef()
	{
		m_count++;
	}
	void Release()
	{
		if(!(--m_count))
		{
			InnerRelease();
			delete this;
		}
	}

	int m_count;
	void InnerRelease()
	{
		if(m_actual != NULL)
			m_actual->Release();
		m_actual = NULL;
	}
public:
	void Attach(T::COM_TYPE* pUnk)
	{
		Attach(T::CreateWrapper(pUnk));
	}
	IUnknown* GetIUnknown()
	{
		if(m_actual != NULL)
			return m_actual->GetIUnknown();
		return NULL;
	}
	
	bool Attach(CIUnknown& Unk)
	{
		InnerRelease();
		IUnknown* pUnk = Unk.GetIUnknown();
		if(pUnk)
		{
			T::COM_TYPE* t = NULL;
			//_com_ptr_t<_com_IIID<T::COM_TYPE,&__uuidof<T> > tPtr;
			pUnk->QueryInterface(T::GetUUID(), (void**)&t);
			if(t == NULL)
				return false;

/*			WRAPPER_LOOKUP::iterator it = s_wrapperLookup.find(t);
			if(it != s_wrapperLookup.end())
				m_actual = (T*)(*it).second;
			else*/
				m_actual = (T*)T::CreateWrapper(t);
			m_actual->AddRef();
			t->Release();
		}
		return true;
	}

	void operator = (CIUnknown& source)
	{
		Attach(source);
	}
	bool operator == (CIUnknown& source)
	{
		return (GetIUnknown() == source.GetIUnknown());
	}
//	friend CWrappedFunctions;

	//lookup map for this interface
//	typedef map<IUnknown*, CIUnknown*> WRAPPER_LOOKUP;
//	static WRAPPER_LOOKUP s_wrapperLookup;
};



template<class IT, class T>
class InterfaceWrapper : public T
{
protected:
	IT* m_pInterface;
	void AddRef()
	{
		m_count++;
	}
	void Release()
	{
		if(!(--m_count))
		{
			InnerRelease();
			delete this;
		}
	}
	int m_count;
	void InnerRelease()
	{
		if(m_pInterface != NULL)
			m_pInterface->Release();
		m_pInterface = NULL;
	}
	InterfaceWrapper(IT*pInterface) : m_count(0), m_pInterface(pInterface)
	{
		pInterface->AddRef();
	}

public:
	IUnknown* GetIUnknown()
	{
		return m_pInterface;
	}

	void Attach(IT* pUnk)
	{
		InnerRelease();
		m_pInterface = pUnk;
		if(m_pInterface != NULL)
			m_pInterface->AddRef();
	}
	IT* Detach()
	{
		IT* ret = m_pInterface;
		m_pInterface = NULL;
		return ret;
	}
};

template <typename T> 
class DataWrapper
{
	public:
	typedef T COM_TYPE;
};

template <class T/*, class Allocator<T>*/ >
class SmartPointerCollection
{
	void CreateBuffer(T*& dataPointer, unsigned int size, unsigned int& dest_size)
	{
		ReleaseBuffer(dataPointer, dest_size);
		if(size)
		{
			dataPointer = new T[size];
			dest_size = size;
		}
	}
	void ResizeBuffer(T*& dataPointer, unsigned int desired_size, unsigned int& dest_size)
	{
		T* tempPointer = new T[desired_size];
		for(int i = 0;i < desired_size && i < dest_size; i++)
		{
			tempPointer[i] = dataPointer[i]
		}
		ReleaseBuffer(dataPointer, dest_size);
		dataPointer = tempPointer;
		dest_size = desired_size;
	}
	void ReleaseBuffer(T*& dataPointer, unsigned int& dest_size)
	{
		delete [] dataPointer;
		dataPointer = NULL;
		dest_size = 0;
	}
};

template <class T/*, class Allocator<T>*/ >
class StructCollection
{
	void CreateBuffer(T*& dataPointer, unsigned int size, unsigned int& dest_size)
	{
		ReleaseBuffer(dataPointer, dest_size);
		if(size)
		{
			dataPointer = T::COM_TYPE* CoTaskMemAlloc(sizeof(T::COM_TYPE*) * size);
			memset(dataPointer,0,sizeof(T::COM_TYPE*) * size);
			dest_size = size;
		}
	}
	void ResizeBuffer(T*& dataPointer, unsigned int desired_size, unsigned int& dest_size)
	{
		if(desired_size != dest_size)
		{
			dataPointer = CoTaskMemRealloc(dataPointer, sizeof(T::COM_TYPE*) * desired_size);
			if(desired_size > dest_size)
				memset(dataPointer + sizeof(T::COM_TYPE*) * dest_size,0,sizeof(T::COM_TYPE*) * (desired_size - dest_size));
			dest_size = desired_size;
		}
	}
	void ReleaseBuffer(T*& dataPointer, unsigned int& dest_size)
	{
		CoTaskMemFree(dataPointer);
		dataPointer = NULL;
		dest_size = 0;
	}
};

template<class T, class CollectionType = SmartPointerCollection<T> >
class Collection
{
	unsigned int m_length;
	unsigned int m_size;
	T::COM_TYPE* m_data;

public:
	Collection(unsigned int size = 0) : m_length(0), m_size(size), m_data(NULL)
	{
		CreateBuffer(size);
	}
	~Collection()
	{
		ReleaseBuffer();
	}

	void CreateBuffer(unsigned int size)
	{
		CollectionType::CreateBuffer(m_data, size, m_size);
	}

	void ReleaseBuffer()
	{
		CollectionType::ReleaseBuffer(m_data);
	}

	void ResizeBuffer(unsigned int size)
	{
		CollectionType::ResizeBuffer(m_data, size, m_size);
	}

	//WARNING this function does a shallow copy only!!!!
	//I would like to make it private but cant easily.
	void AllocateData(T::COM_TYPE* data)
	{
//		m_data [] m_data;
		m_data = data;
	}
	T::COM_TYPE* GetRawBuffer()
	{
		return m_data;
	}
	unsigned int GetLength() const
	{
		return m_length;
	}
	void SetLength(unsigned int len)
	{
		m_length = len;
	}
	unsigned int GetSize() const
	{
		return m_size;
	}
	void SetSize(unsigned int size)
	{
		CollectionType::ResizeBuffer(m_data, size, m_size);
	}
	T& operator [] (unsigned int pos) const
	{
		if(pos >= m_length)
			throw L"Error index out of range";
		return &m_data[pos];
	}
};

class CWrappedComFunctions;

template<typename T> 
T::COM_TYPE* GetInterface(T& obj)
{
	return static_cast<T::COM_TYPE*> (obj.GetIUnknown());
}

