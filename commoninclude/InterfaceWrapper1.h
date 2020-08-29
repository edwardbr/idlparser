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
				explain.WriteExplanation("Seriously out of memory!!");
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

static HRESULT Test(HRESULT hRes)
{
	if(FAILED(hRes))
		ComException(hRes);
	return hRes;
}

template <class T>
class ERRORHANDLER_API InterfaceWrapper// : public T
{
private:
	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return 1;
		//T* pTemp;
		//return m_pInterface.CopyTo(&pTemp);
	}
    
    virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return 1;//m_pInterface.Release();
	}

public:
    STDMETHOD(QueryInterface)(REFIID riid,void **ppvObject)
	{
		HRESULT hRes = m_pInterface->QueryInterface(riid,ppvObject);
		return Test(hRes);
	}
    
	InterfaceWrapper()
	{
		m_pInterface=NULL;
	}
	InterfaceWrapper(T* lp)
	{
		if ((m_pInterface = lp) != NULL)
			m_pInterface->AddRef();
	}
	InterfaceWrapper(const InterfaceWrapper<T>& lp)
	{
		if ((m_pInterface = lp.m_pInterface) != NULL)
			m_pInterface->AddRef();
	}
	~InterfaceWrapper()
	{
		if (m_pInterface)
			m_pInterface->Release();
	}
	operator T*() const
	{
		return (T*)m_pInterface;
	}
	T& operator*() const
	{
		ATLASSERT(m_pInterface!=NULL);
		return *m_pInterface;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the m_pInterface member explicitly.
	T** operator&()
	{
		ATLASSERT(m_pInterface==NULL);
		return &m_pInterface;
	}
	_NoAddRefReleaseOnCComPtr<T>* operator->() const
	{
		ATLASSERT(m_pInterface!=NULL);
		return (_NoAddRefReleaseOnCComPtr<T>*)m_pInterface;
	}
	T* operator=(T* lp)
	{
		return (T*)AtlComPtrAssign((IUnknown**)&m_pInterface, lp);
	}
	T* operator=(const InterfaceWrapper<T>& lp)
	{
		return (T*)AtlComPtrAssign((IUnknown**)&m_pInterface, lp.m_pInterface);
	}
	bool operator!() const
	{
		return (m_pInterface == NULL);
	}
	bool operator<(T* pT) const
	{
		return m_pInterface < pT;
	}
	bool operator==(T* pT) const
	{
		return m_pInterface == pT;
	}
	// Compare two objects for equivalence
	bool IsEqualObject(IUnknown* pOther)
	{
		if (m_pInterface == NULL && pOther == NULL)
			return true; // They are both NULL objects

		if (m_pInterface == NULL || pOther == NULL)
			return false; // One is NULL the other is not

		InterfaceWrapper<IUnknown> punk1;
		InterfaceWrapper<IUnknown> punk2;
		m_pInterface->QueryInterface(IID_IUnknown, (void**)&punk1);
		pOther->QueryInterface(IID_IUnknown, (void**)&punk2);
		return punk1 == punk2;
	}
	void Attach(T* p2, bool addRef = false)
	{
		if (m_pInterface)
			m_pInterface->Release();
		m_pInterface = p2;
		if(addRef)
			m_pInterface->AddRef();
	}
	T* Detach()
	{
		T* pt = m_pInterface;
		m_pInterface = NULL;
		return pt;
	}
	HRESULT CopyTo(T** ppT)
	{
		ATLASSERT(ppT != NULL);
		if (ppT == NULL)
			return E_POINTER;
		*ppT = m_pInterface;
		if (m_pInterface)
			m_pInterface->AddRef();
		return S_OK;
	}
/*	HRESULT SetSite(IUnknown* punkParent)
	{
		return AtlSetChildSite(m_pInterface, punkParent);
	}*/
	HRESULT Advise(IUnknown* pUnk, const IID& iid, LPDWORD pdw)
	{
		return AtlAdvise(m_pInterface, pUnk, iid, pdw);
	}
	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
	{
		ATLASSERT(m_pInterface == NULL);
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&m_pInterface);
	}
	HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
	{
		CLSID clsid;
		HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
		ATLASSERT(m_pInterface == NULL);
		if (SUCCEEDED(hr))
			hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&m_pInterface);
		return hr;
	}
	template <class Q>
	HRESULT QueryInterface(Q** pp) const
	{
		ATLASSERT(pp != NULL && *pp == NULL);
		return m_pInterface->QueryInterface(__uuidof(Q), (void**)pp);
	}
	T* m_pInterface;
};
/*
template <class Impl,class IW>
class CComObject_Wrapper : 
	public IW
{
public:
	CComObject_Wrapper()
	{
		CComObject<Impl>::CreateInstance(&m_objectImpl);
		Attach(m_objectImpl,true);
	}
	CComObject<Impl>& GetObject()
	{
		return *m_objectImpl;
	}
private:
	CComObject<Impl>* m_objectImpl;
};*/

template<class T>
class RefCountedObject : public T
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
			m_actual->AddRef()
			m_count = 1;
		}
	}
	~RefCountedObject()
	{}

	//for attaching raw COM pointers to Wrapped COM pointers
	void Attach(T::COM_TYPE* pUnk)
	{
		InnerRelease();
		T::CreateWrapper(pUnk,m_actual);
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
	void QueryInterface(REFIID riid, CIUnknown** ppvObject)
	{
		if(m_actual != NULL)
			m_actual->QueryInterface(riid, ppvObject);
	}

	int m_count;
	void InnerRelease()
	{
		if(m_actual != NULL)
			m_actual->Release();
		m_actual = NULL;
	}
public:
	IUnknown* GetIUnknown()
	{
		if(m_actual != NULL)
			return m_actual->GetIUnknown();
		return NULL;
	}
	
	T* m_actual;
	friend WrappedFunctions;

	template<typename S> 
	bool Attach(S& pUnk)
	{
		InnerRelease();
		S::COM_TYPE* s = GetInterface(pUnk);
		T::COM_TYPE* t = NULL;
		//_com_ptr_t<_com_IIID<T::COM_TYPE,&__uuidof<T> > tPtr;
		s->QueryInterface(__uuidof(T), (void**)&t);
		if(t == NULL)
			return false;

		T::WRAPPER_LOOKUP::iterator it = s_wrapperLookup.find(t);
		if(it != s_wrapperLookup.end())
			m_actual = (*it).second;
		else
			m_actual = T::CreateWrapper(t);
		m_actual->AddRef();
		t->Release();
		return true;
	}

	void operator = (CIUnknown& source)
	{
		T::COM_TYPE* iface = NULL;
		HRESULT hr = GetInterface(source)->QueryInterface(__uuidof(T), (void**)&iface);
		Test(hr);
		Attach(new T::COM_TYPE_WRAPPER(iface));
	}
	friend WrappedFunctions;
};


