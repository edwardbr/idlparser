#include "stdafx.h"

#include <string>
using namespace std;
#include "commonfuncs.h"

int Strcmp2(const char *string1, const char *string2)
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

bool BeginsWith(const char *string1, const char *string2)
{
	return !Strcmp2(string1, string2);
}

bool IsWord(const char *string1, const char *string2)
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

bool IsPreproc(const char* string1, const char *string2)
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
	return IsWord(string1, string2);
}

bool IsPreprocEat(const char*& string1, const char *string2)
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
	if(IfIsWordEat(tmp, string2) == true)
	{
		string1 = tmp;
		return true;
	}
	return false;
}



bool IfIsWordEat(const char*& string1, const char *string2)
{
	if(IsWord(string1, string2))
	{
		string1 += strlen(string2);

		//eat space
		while(*string1 == ' ')
			string1++;
		return true;
	}
	return false;
}

bool ExtractWord(const char*& pData, string& retval)
{
	string temp;
	while((*pData >= '0' && *pData <= '9') || (*pData >= 'A' && *pData <= 'Z') || (*pData >= 'a' && *pData <= 'z') || (*pData == '_'))
		temp += *pData++;
	retval += temp;

//if(temp == "DECIMAL")
//	int i = 0;
	return temp.size() > 0;
}



