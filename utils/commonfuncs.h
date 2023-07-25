#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

int strcmp2(const char *string1, const char *string2);
bool begins_with(const char *string1, const char *string2);
bool is_word(const char *string1, const char *string2);
bool if_is_word_eat(const char*& string1, const char *string2);
bool is_preproc(const char *string1, const char *string2);
bool is_preproc_eat(const char*& string1, const char *string2);


bool extract_word(const char*& pData, std::string& retval);
bool extract_multiline_string_literal(const char*& pData, const char*& pStart, const char*& pSuffix);
bool extract_string_literal(const char*& pData, std::string& contents);

std::vector<std::string> split(const std::string &s, char delim);

inline std::ostream& operator << (std::ostream& _O, const std::string& _X)
{
    _O << _X.c_str();
    return _O; 
}

std::string to_lower(std::string s);
std::string to_upper(std::string s);