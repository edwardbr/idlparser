// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5A3A5F0F_E677_4B71_BA36_9EAAA07C8B4D__INCLUDED_)
#define AFX_STDAFX_H__5A3A5F0F_E677_4B71_BA36_9EAAA07C8B4D__INCLUDED_

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4355 )
#pragma warning(disable : 4786)

#include "windows.h"
#endif

#include <strstream>
#include <stdio.h> 
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <unordered_map>
using namespace std;
using namespace stdext;

#include <fstream>

inline ostream& operator << (ostream& _O, const std::string& _X)
{
    _O << _X.c_str();
    return _O; 
}

#include <direct.h>

#endif // !defined(AFX_STDAFX_H__5A3A5F0F_E677_4B71_BA36_9EAAA07C8B4D__INCLUDED_)
