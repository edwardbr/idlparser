/******************************************************************************
Copywrite (c) Edward Boggis-Rolfe Jan 1999

Moniker Ltd.
www.btinternet.com/~moniker
To report any bugs please contact
m o n i k e r @ b t i n t e r n e t . c o m
please do not spam

This code may be freely distributed on the proviso that 
this commented text stays at the head of the file unchanged
I make no guarantees that this code is safe, use at your own risk.
*******************************************************************************/


/*******************************************************************************

This code allows the programmer to need implement only one exception handler in 
an executable.  If many handlers are used and are large then the executable size 
will reduce enormously by using this technique

If this set of exception handlers do not work (different calling convention or 
different machine architecture) then use the header in the backup directory
*******************************************************************************/




#include "stdarg.h"

#pragma once

//must not be a template class unless you want code bloat
class __declspec(novtable) function_carrier_base
{
public:
	virtual void post_filter() = 0;
	function_carrier_base() : m_err(0)
	{}
	virtual void set_error(long err){m_err = err;}
	long m_err;
};
/*class __declspec(novtable) function_carrier_base : public function_carrier_base
{
public:
	function_carrier_base() : m_err(0)
	{}
	virtual void set_error(long err){m_err = err;}
	long m_err;
};
class __declspec(novtable) function_carrier_base : public function_carrier_base
{
public:
	virtual void set_error(long){;}
};*/

/*******************************************************************************
The first of only two exception handlers that you wil ever need to implement
With the use of the function_carrier_base v-table; TryFunc can indirectly call the 
inner function without knowing its type 

It is up to the user of this function to implement it

Here is a bare bones example

void TryFunc(function_carrier_base& functionCarrier)
{
	try
	{
		functionCarrier.post_filter();
	}
	catch(...)
	{
	
	}
}
*******************************************************************************/
//void TryFunc(function_carrier_base& functionCarrier);
//void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp );

/*******************************************************************************
The second of only two exception handlers that you wil ever need to implement
Indended for use with ATL replace BEGIN_MSG_MAP with BEGIN_TRY_MSG_MAP to 
provide transparent exception handling
*******************************************************************************/
class CProcessMessageProtect
{
public:
	virtual BOOL InnerProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,LRESULT& lResult, DWORD dwMsgMapID) = 0;
};

//BOOL MessageProcessTry(CProcessMessageProtect* pImplementor ,HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);


#define BEGIN_TRY_MSG_MAP(theClass) \
public: \
	BOOL  ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
			return MessageProcessTry(this, hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID); \
	} \
	BOOL InnerProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bHandled = TRUE; \
		hWnd; \
		uMsg; \
		wParam; \
		lParam; \
		lResult; \
		bHandled; \
		switch(dwMsgMapID) \
		{ \
		case 0:

#define END_TRY_MSG_MAP() \
			break; \
		default: \
			ATLTRACE2(atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID); \
			ATLASSERT(FALSE); \
			break; \
		} \
		return FALSE; \
	}

#ifdef NO_EXCEPTIONS

#define EXCEPT_PROTECT(func) \
	func

#define PROTECT_FUNCTION0(retvaltype, func) \
	retvaltype func ()

#define PROTECT_FUNCTION1(retvaltype, func, ARG1, arg1) \
	retvaltype func (ARG1 arg1)

#define PROTECT_FUNCTION2(retvaltype, func, ARG1, arg1, ARG2, arg2) \
	retvaltype func (ARG1 arg1, ARG2 arg2)

#define PROTECT_FUNCTION3(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_FUNCTION4(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_FUNCTION5(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_FUNCTION6(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_FUNCTION7(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_FUNCTION8(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)

//*********************************************************************************

#define PROTECT_VOID_FUNCTION0(func) \
	void func ()

#define PROTECT_VOID_FUNCTION1(func, ARG1, arg1) \
	void func (ARG1 arg1)

#define PROTECT_VOID_FUNCTION2(func, ARG1, arg1, ARG2, arg2) \
	void func (ARG1 arg1, ARG2 arg2)

#define PROTECT_VOID_FUNCTION3(func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_VOID_FUNCTION4(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_VOID_FUNCTION5(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_VOID_FUNCTION6(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_VOID_FUNCTION7(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_VOID_FUNCTION8(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)

//hresult protection functions


#define PROTECT_HRESULT_FUNCTION0(func) \
	STDMETHOD(func) ()

#define PROTECT_HRESULT_FUNCTION1(func, ARG1, arg1) \
	STDMETHOD(func) (ARG1 arg1)

#define PROTECT_HRESULT_FUNCTION2(func, ARG1, arg1, ARG2, arg2) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2)

#define PROTECT_HRESULT_FUNCTION3(func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_HRESULT_FUNCTION4(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_HRESULT_FUNCTION5(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_HRESULT_FUNCTION6(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_HRESULT_FUNCTION7(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_HRESULT_FUNCTION8(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)


#else

/********************************************************************************
The function_carrier classes carry the pointer to the object, a pointer to the function, 
function parameters and the return value.  

They are derived from function_carrier_base 

The function post_filter will use the va_list in a type safe manner to un-pack the 
parameter list for calling the inner function, using the template parameters
for parameter type identification
********************************************************************************/

template <class the_class, typename RETVAL>
class function_carrier0 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)();
	function_carrier0(the_class* theClass,FN fn, RETVAL* retval) : 
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)();}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};

template <class the_class, typename RETVAL, typename ARG1>
class function_carrier1 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1);
	function_carrier1(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1) : 
						m_arg1(arg1),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(m_arg1);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2>
class function_carrier2 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2);
	function_carrier2(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	virtual void post_filter(){	*m_retVal = (m_theClass->*m_fn)(m_arg1, m_arg2);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3>
class function_carrier3 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3);
	function_carrier3(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	virtual void post_filter(){	*m_retVal = (m_theClass->*m_fn)(m_arg1, arg2, arg3);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class function_carrier4 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3, ARG4);
	function_carrier4(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(m_arg1, arg2, arg3, arg4);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
class function_carrier5 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5);
	function_carrier5(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(arg1, arg2, arg3, arg4, arg5);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
class function_carrier6 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
	function_carrier6(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(arg1, arg2, arg3, arg4, arg5, arg6);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7>
class function_carrier7 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
	function_carrier7(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_arg7(arg7),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	ARG7 m_arg7;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};
template <class the_class, typename RETVAL, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
class function_carrier8 : public function_carrier_base
{
public:
	typedef RETVAL (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
	function_carrier8(the_class* theClass,FN fn, RETVAL* retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_arg7(arg7),
						m_arg8(arg8),
						m_theClass(theClass), 
						m_fn(fn), 
						m_retVal(retval){}
	//squeezed onto one line to ease debugging
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	ARG7 m_arg7;
	ARG8 m_arg8;
	virtual void post_filter(){*m_retVal = (m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8);}
	the_class* m_theClass;
	FN m_fn;
	RETVAL* m_retVal;
};


/********************************************************************************
the whole cabudle again for functions returning void

void_function_carriers again are derived from function_carrier_base
********************************************************************************/


template <class the_class>
class void_function_carrier0 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)();
	void_function_carrier0(the_class* theClass,FN fn) : 
						m_theClass(theClass), 
						m_fn(fn)
	{
	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)();
	}
	the_class* m_theClass;
	FN m_fn;
};

template <class the_class, typename ARG1>
class void_function_carrier1 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1);
	void_function_carrier1(the_class* theClass,FN fn, ARG1 arg1) : 
						m_arg1(arg1),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1);
	}
	ARG1 m_arg1;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2>
class void_function_carrier2 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2);
	void_function_carrier2(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3>
class void_function_carrier3 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3);
	void_function_carrier3(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class void_function_carrier4 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3, ARG4);
	void_function_carrier4(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
class void_function_carrier5 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5);
	void_function_carrier5(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
class void_function_carrier6 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
	void_function_carrier6(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7>
class void_function_carrier7 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
	void_function_carrier7(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_arg7(arg7),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	ARG7 m_arg7;
	the_class* m_theClass;
	FN m_fn;
};
template <class the_class, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
class void_function_carrier8 : public function_carrier_base
{
public:
	typedef void (the_class::*FN)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
	void_function_carrier8(the_class* theClass,FN fn, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) : 
						m_arg1(arg1),
						m_arg2(arg2),
						m_arg3(arg3),
						m_arg4(arg4),
						m_arg5(arg5),
						m_arg6(arg6),
						m_arg7(arg7),
						m_arg8(arg8),
						m_theClass(theClass), 
						m_fn(fn)
	{	
	}
	virtual void post_filter()
	{
		(m_theClass->*m_fn)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8);
	}
	ARG1 m_arg1;
	ARG2 m_arg2;
	ARG3 m_arg3;
	ARG4 m_arg4;
	ARG5 m_arg5;
	ARG6 m_arg6;
	ARG7 m_arg7;
	ARG8 m_arg8;
	the_class* m_theClass;
	FN m_fn;
};

/********************************************************************************/

/********************************************************************************
the PROTECT_FUNCTION macro save the programmer the hassle of defining two functions
by declaring the functions, with the outer function being managed by the macro.

the first parameter is cast to a variable of va_list and it allows the function_carriers
to call the inner functions with the correct data see above
********************************************************************************/
#define EXCEPT_PROTECT(func) \
	Inner_##func


#define PROTECT_FUNCTION0(retvaltype, func) \
	retvaltype func () \
	{ \
		retvaltype retval = NULL; \
		function_carrier0<THE_CLASS,retvaltype> passer(this, Inner_##func, &retval); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func ()

#define PROTECT_FUNCTION1(retvaltype, func, ARG1, arg1) \
	retvaltype func (ARG1 arg1) \
	{ \
		retvaltype retval = NULL; \
		function_carrier1<THE_CLASS,retvaltype, ARG1> passer(this, Inner_##func, &retval, ARG1 arg1); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1)

#define PROTECT_FUNCTION2(retvaltype, func, ARG1, arg1, ARG2, arg2) \
	retvaltype func (ARG1 arg1, ARG2 arg2) \
	{ \
		retvaltype retval = NULL; \
		function_carrier2<THE_CLASS,retvaltype, ARG1, ARG2>passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2)

#define PROTECT_FUNCTION3(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3) \
	{ \
		retvaltype retval = NULL; \
		function_carrier3<THE_CLASS,retvaltype, ARG1, ARG2, ARG3>passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_FUNCTION4(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) \
	{ \
		retvaltype retval = NULL; \
		function_carrier4<THE_CLASS,retvaltype, ARG1, ARG2, ARG3, ARG4>passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_FUNCTION5(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) \
	{ \
		retvaltype retval = NULL; \
		function_carrier5<THE_CLASS,retvaltype, ARG1, ARG2, ARG3, ARG4, ARG5>passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_FUNCTION6(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) \
	{ \
		retvaltype retval = NULL; \
		function_carrier6<THE_CLASS,retvaltype, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_FUNCTION7(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) \
	{ \
		retvaltype retval = NULL; \
		function_carrier7<THE_CLASS,retvaltype, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_FUNCTION8(retvaltype, func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	retvaltype func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) \
	{ \
		retvaltype retval = NULL; \
		function_carrier8<THE_CLASS,retvaltype, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8); \
		TryFunc(passer); \
		return retval; \
	} \
	retvaltype Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)

//*********************************************************************************

#define PROTECT_VOID_FUNCTION0(func) \
	void func () \
	{ \
		void_function_carrier0<THE_CLASS> passer(this, Inner_##func); \
		TryFunc(passer); \
	} \
	void Inner_##func ()

#define PROTECT_VOID_FUNCTION1(func, ARG1, arg1) \
	void func (ARG1 arg1) \
	{ \
		void_function_carrier1<THE_CLASS,ARG1> passer(this, Inner_##func, ARG1 arg1); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1)

#define PROTECT_VOID_FUNCTION2(func, ARG1, arg1, ARG2, arg2) \
	void func (ARG1 arg1, ARG2 arg2) \
	{ \
		void_function_carrier2<THE_CLASS,ARG1, ARG2>passer(this, Inner_##func, ARG1 arg1, ARG2 arg2); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2)

#define PROTECT_VOID_FUNCTION3(func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3) \
	{ \
		void_function_carrier3<THE_CLASS,ARG1, ARG2, ARG3>passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_VOID_FUNCTION4(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) \
	{ \
		void_function_carrier4<THE_CLASS,ARG1, ARG2, ARG3, ARG4>passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_VOID_FUNCTION5(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) \
	{ \
		void_function_carrier5<THE_CLASS,ARG1, ARG2, ARG3, ARG4, ARG5>passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_VOID_FUNCTION6(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) \
	{ \
		void_function_carrier6<THE_CLASS,ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_VOID_FUNCTION7(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) \
	{ \
		void_function_carrier7<THE_CLASS,ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_VOID_FUNCTION8(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	void func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) \
	{ \
		void_function_carrier8<THE_CLASS,ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8> passer(this, Inner_##func, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8); \
		TryFunc(passer); \
	} \
	void Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)


//hresult protection functions


#define PROTECT_HRESULT_FUNCTION0(func) \
	STDMETHOD(func) () \
	{ \
		HRESULT retval; \
		function_carrier0<THE_CLASS,HRESULT> passer(this, Inner_##func, &retval); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func ()

#define PROTECT_HRESULT_FUNCTION1(func, ARG1, arg1) \
	STDMETHOD(func) (ARG1 arg1) \
	{ \
		HRESULT retval; \
		function_carrier1<THE_CLASS,HRESULT, ARG1> passer(this, Inner_##func, &retval, ARG1 arg1); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1)

#define PROTECT_HRESULT_FUNCTION2(func, ARG1, arg1, ARG2, arg2) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2) \
	{ \
		HRESULT retval; \
		function_carrier2<THE_CLASS,HRESULT, ARG1, ARG2> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2)

#define PROTECT_HRESULT_FUNCTION3(func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3) \
	{ \
		HRESULT retval; \
		function_carrier3<THE_CLASS,HRESULT, ARG1, ARG2, ARG3> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3)

#define PROTECT_HRESULT_FUNCTION4(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) \
	{ \
		HRESULT retval; \
		function_carrier4<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4)

#define PROTECT_HRESULT_FUNCTION5(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) \
	{ \
		HRESULT retval; \
		function_carrier5<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)

#define PROTECT_HRESULT_FUNCTION6(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) \
	{ \
		HRESULT retval; \
		function_carrier6<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6)

#define PROTECT_HRESULT_FUNCTION7(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) \
	{ \
		HRESULT retval; \
		function_carrier7<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7)

#define PROTECT_HRESULT_FUNCTION8(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) \
	{ \
		HRESULT retval; \
		function_carrier8<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	} \
	HRESULT Inner_##func (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8)



#define PROTECT_HRESULT_VFUNCTION0(func) \
	STDMETHOD(func) () \
	{ \
		HRESULT retval; \
		function_carrier0<THE_CLASS,HRESULT> passer(this, Inner_##func, &retval); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION1(func, ARG1, arg1) \
	STDMETHOD(func) (ARG1 arg1) \
	{ \
		HRESULT retval; \
		function_carrier1<THE_CLASS,HRESULT, ARG1> passer(this, Inner_##func, &retval, ARG1 arg1); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION2(func, ARG1, arg1, ARG2, arg2) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2) \
	{ \
		HRESULT retval; \
		function_carrier2<THE_CLASS,HRESULT, ARG1, ARG2> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION3(func, ARG1, arg1, ARG2, arg2, ARG3, arg3) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3) \
	{ \
		HRESULT retval; \
		function_carrier3<THE_CLASS,HRESULT, ARG1, ARG2, ARG3> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION4(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) \
	{ \
		HRESULT retval; \
		function_carrier4<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION5(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5) \
	{ \
		HRESULT retval; \
		function_carrier5<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION6(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6) \
	{ \
		HRESULT retval; \
		function_carrier6<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION7(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7) \
	{ \
		HRESULT retval; \
		function_carrier7<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#define PROTECT_HRESULT_VFUNCTION8(func, ARG1, arg1, ARG2, arg2, ARG3, arg3, ARG4, arg4, ARG5, arg5, ARG6, arg6, ARG7, arg7, ARG8, arg8) \
	STDMETHOD(func) (ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8) \
	{ \
		HRESULT retval; \
		function_carrier8<THE_CLASS,HRESULT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8> passer(this, Inner_##func, &retval, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5, ARG6 arg6, ARG7 arg7, ARG8 arg8); \
		TryFunc(passer); \
		return passer.m_err == S_OK? retval : passer.m_err; \
	}

#endif