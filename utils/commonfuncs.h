#pragma once

#include <vector>
#include <string>
#include <iostream>

int Strcmp2(const char *string1, const char *string2);
bool BeginsWith(const char *string1, const char *string2);
bool IsWord(const char *string1, const char *string2);
bool IfIsWordEat(const char*& string1, const char *string2);
bool IsPreproc(const char *string1, const char *string2);
bool IsPreprocEat(const char*& string1, const char *string2);


bool ExtractWord(const char*& pData, std::string& retval);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

inline std::ostream& operator << (std::ostream& _O, const std::string& _X)
{
    _O << _X.c_str();
    return _O; 
}