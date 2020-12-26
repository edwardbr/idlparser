#include <iostream>
#include <sstream>
#include "edl_macro_handler.h"

bool edl_macro_parser::Init()
{
    return macro_parser::Init();
}

bool edl_macro_parser::ParseInclude(const char*& pData, int ignoreText, std::ostream& dest, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	if(!IfIsWordEat(pData,"include") == true)
	{
		return false;
	}

	while(*pData == ' ' || *pData == '\t')
		pData++;

	if(!(*pData == '\"' || *pData == '<'))
	{
		std::stringstream err;
		err << "Error unexpected character: " << *pData;
		err << std::ends;
		std::string errString(err.str());
		throw errString;
	}
	pData++;

	std::string var;
	while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
	{
		var += *pData++;
	}

	if(!(*pData == '\"' || *pData == '>'))
	{
		std::stringstream err;
		err << "Error unexpected character: " << *pData;
		err << std::ends;
		std::string errString(err.str());
		throw errString;
	}
	pData++;

	ParseAndLoad(ignoreText, dest, includeDirectories, var.data(), loaded_includes);
	return true;
}