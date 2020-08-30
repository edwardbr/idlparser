#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include <macrohandler.h>
#include <function_timer.h>

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
				std::cout << "error missing -sourceFile parameter\n";
				return -1;
			}
			sourceFile = argc[i];
			std::cout << "-sourceFile " << sourceFile << "\n";
			continue;
		}
		else if(!strcmp(argc[i],"-outputFile"))
		{
			i++;
			if(i >= argv)
			{
				std::cout << "error missing -outputFile parameter\n";
				return -1;
			}
			outputFile = argc[i];
			std::cout << "-outputFile " << outputFile << "\n";
			continue;
		}
		else if(!strcmp(argc[i],"-includePath"))
		{
			i++;
			if(i >= argv)
			{
				std::cout << "error missing -includePath parameter\n";
				return -1;
			}
			includePath = argc[i];
			std::cout << "-includePath " << includePath << "\n";
			continue;
		}
		else if(strlen(argc[i]) > 2 && argc[i][0] == '-' && argc[i][1] == 'D' )
		{
			if(i >= argv)
			{
				std::cout << "error missing -D parameter\n";
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
				std::cout << "#define: " << def.m_defName << " " << def.m_substitutionString << "\n";
				defines.insert(std::unordered_map<std::string, definition>::value_type(def.m_defName, def));
			}
			continue;
		}
	}

	if(sourceFile == NULL)
	{
		std::cout << "error missing -sourceFile parameter\n";
		return -1;
	}

	if(includePath == NULL)
	{
		std::cout << "error missing -includePath parameter\n";
		return -1;
	}

	try
	{
		{
			definition def;
			def.m_defName = "GENERATOR";
			def.m_substitutionString = "1";
			defines.insert(std::unordered_map<std::string, definition>::value_type(def.m_defName,def));
		}

		{
			definition def;
			def.m_defName = "size_t";
			def.m_substitutionString = "unsigned int ";
			defines.insert(std::unordered_map<std::string, definition>::value_type(def.m_defName,def));
		}

		std::string includeDirectories = includePath;

		std::error_code ec;

		std::string modifiedOutputFile = std::filesystem::canonical(std::string(outputFile), ec).make_preferred().string();
		std::cout << "modified outputFile: " << modifiedOutputFile << "\n";

		std::string modifiedSourceFile = std::filesystem::canonical(std::string(sourceFile), ec).make_preferred().string();
		std::cout << "modified SourceFile: " << modifiedSourceFile << "\n";

		std::cout << "include directories: " << includeDirectories << "\n";

		std::ifstream source_file(modifiedSourceFile);
		std::ofstream file(modifiedOutputFile);
		Load(file, source_file, includeDirectories);
	}
	catch(std::exception ex)
	{
		std::cout << ex.what() << '\n';
	}
	return 0;
}
