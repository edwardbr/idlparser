#include "stdafx.h"
#include "macrohandler.h"

#include "function_timer.h"
#include <filesystem>
#include <algorithm>

xt::function_timer* p_timer = NULL;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int main(int argv, char* argc[])
{
	char* sourceFile = NULL;
	char* outputFile = NULL;
	char* includePath = NULL;

	//get command line values
	for(int i = 0; i < argv;i++)
	{
		if(!strcmp(argc[i],"-sourceFile"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -sourceFile parameter" << ends;
				return -1;
			}
			sourceFile = argc[i];
			cout << "-sourceFile " << sourceFile << std::endl;
			continue;
		}
		else if(!strcmp(argc[i],"-outputFile"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -outputFile parameter" << ends;
				return -1;
			}
			outputFile = argc[i];
			cout << "-outputFile " << outputFile << std::endl;
			continue;
		}
		else if(!strcmp(argc[i],"-includePath"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -includePath parameter" << ends;
				return -1;
			}
			includePath = argc[i];
			cout << "-includePath " << includePath << std::endl;
			continue;
		}
		else if(strlen(argc[i]) > 2 && argc[i][0] == '-' && argc[i][1] == 'D' )
		{
			if(i >= argv)
			{
				cout << "error missing -D parameter" << ends;
				return -1;
			}
			std::vector<std::string> elems;
			split(argc[i]+2, '=', elems);
			{
				definition def;
				def.m_defName = elems[0];
				if(elems.size() > 1)
				{
					def.m_substitutionString = elems[1];
				}
				cout << "#define: " << def.m_defName << " " << def.m_substitutionString << std::endl;
				defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName, def));
			}
			continue;
		}
	}

	if(sourceFile == NULL)
	{
		cout << "error missing -sourceFile parameter" << ends;
		return -1;
	}

	if(includePath == NULL)
	{
		cout << "error missing -includePath parameter" << ends;
		return -1;
	}

	try
	{
		{
			definition def;
			def.m_defName = "__midl";
			def.m_substitutionString = "10000";
			defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName,def));
		}

		{
			definition def;
			def.m_defName = "GENERATOR";
			def.m_substitutionString = "1";
			defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName,def));
		}

		{
			definition def;
			def.m_defName = "USE_ASIO";
			def.m_substitutionString = "1";
			defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName,def));
		}

		{
			definition def;
			def.m_defName = "size_t";
			def.m_substitutionString = "unsigned int ";
			defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName,def));
		}

		std::string includeDirectories = includePath;

		std::error_code ec;

		std::string modifiedOutputFile = std::experimental::filesystem::canonical(std::string(outputFile), ec).make_preferred().string();
		cout << "modified outputFile: " << modifiedOutputFile << std::endl;

		std::string modifiedSourceFile = std::experimental::filesystem::canonical(std::string(sourceFile), ec).make_preferred().string();
		cout << "modified SourceFile: " << modifiedSourceFile << std::endl;

		cout << "include directories: " << includeDirectories << std::endl;

		std::ifstream source_file(modifiedSourceFile);
		std::ofstream file(modifiedOutputFile);
		Load(file, source_file, includeDirectories);
	}
	catch(...)
	{}
	return 0;
}
