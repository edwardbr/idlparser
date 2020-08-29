#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4355 )
#pragma warning(disable : 4786)
#endif

#include <strstream>
#include <stdio.h> 
#include <iostream>
#include <string>
#include <list>
#include <set>
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

