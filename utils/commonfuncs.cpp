#include <string>
#include <fstream>
#include <sstream>

#include "commonfuncs.h"

int strcmp2(const char *string1, const char *string2)
{
	while(*string2)
	{
		if(*string1 != *string2)
			return *string1 - *string2;
		string1++;
		string2++;
	}
	return 0;
}

bool begins_with(const char *string1, const char *string2)
{
	return !strcmp2(string1, string2);
}

bool is_word(const char *string1, const char *string2)
{
	while(*string2)
	{
		if(*string1 != *string2)
			return false;
		string1++;
		string2++;
	}
	if((*string1 >= '0' && *string1 <= '9') || (*string1 >= 'A' && *string1 <= 'Z') || (*string1 >= 'a' && *string1 <= 'z') || (*string1 == '_'))
		return false;
	return true;
}

bool is_preproc(const char* string1, const char *string2)
{
	if(*string1 != '#')
	{
		return false;
	}
	string1++;

	while(*string1 == ' ' || *string1 == '\t')
	{
		string1++;
	}
	return is_word(string1, string2);
}

bool is_preproc_eat(const char*& string1, const char *string2)
{
	const char* tmp = string1;
	if(*tmp++ != '#')
	{
		return false;
	}

	while(*tmp == ' ' || *tmp == '\t')
	{
		tmp++;
	}
	if(if_is_word_eat(tmp, string2) == true)
	{
		string1 = tmp;
		return true;
	}
	return false;
}



bool if_is_word_eat(const char*& string1, const char *string2)
{
	if(is_word(string1, string2))
	{
		string1 += strlen(string2);

		//eat space
		while(*string1 == ' ')
			string1++;
		return true;
	}
	return false;
}

bool extract_word(const char*& pData, std::string& retval)
{
	std::string temp;
	while((*pData >= '0' && *pData <= '9') || (*pData >= 'A' && *pData <= 'Z') || (*pData >= 'a' && *pData <= 'z') || (*pData == '_'))
		temp += *pData++;
	retval += temp;

//if(temp == "DECIMAL")
//	int i = 0;
	return temp.size() > 0;
}


std::vector<std::string> split(const std::string &s, char delim) 
{
	std::vector<std::string> ret;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        ret.push_back(item);
    }
    return ret;
}


std::string to_lower(std::string s) 
{
    std::transform(s.begin(), s.end(), s.begin(),
       [](unsigned char c){ return tolower(c); }
    );
    return s;
}

std::string to_upper(std::string s) 
{
    std::transform(s.begin(), s.end(), s.begin(),
       [](unsigned char c){ return toupper(c); }
    );
    return s;
}