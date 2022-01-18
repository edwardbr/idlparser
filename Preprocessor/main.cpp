#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <algorithm>

#include <macro_parser.h>
#include "edl_macro_handler.h"

#include <function_timer.h>
#include "commonfuncs.h"

xt::function_timer* p_timer = NULL;

int main(int argv, char* argc[])
{
	std::unique_ptr<macro_parser> parser;

	char* sourceFile = NULL;
	char* outputFile = NULL;
	paths includePaths;
	
	for(int i = 0; i < argv;i++)
	{
		if(!strcmp(argc[i],"-dialect"))
		{
			i++;
			if(i >= argv)
			{
				std::cout << "error missing -dialect parameter\n";
				return -1;
			}
			if(!strcmp(argc[i], "standard_macro"))
			{
				parser = std::unique_ptr<macro_parser>(new macro_parser());
			}
			else if(!strcmp(argc[i], "edl_macro"))
			{
				parser = std::unique_ptr<macro_parser>(new edl_macro_parser());
			}
			else
			{
				std::cerr << "invalid dialect\n";
			}
			
		}
	}

	if(!parser)
	{
		parser = std::unique_ptr<macro_parser>(new macro_parser());
	}

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

			char* includePath = argc[i];
			std::cout << "-includePath " << includePath << "\n";
			std::vector<std::string> results;

			split(includePath, '^', results);
			for(auto result : results)
			{
				std::filesystem::path p(result);
				if(p.has_root_directory())
				{
					includePaths.push_back(p);
					std::cout << "include directory: " << p.string() << "\n";
				}
				else
				{
					includePaths.push_back(std::filesystem::current_path()/p);
					std::cout << "include directory: " << (std::filesystem::current_path()/p).string() << "\n";
				}
			}
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
				macro_parser::definition def;
				std::string defName = elems[0];
				if(elems.size() > 1)
				{
					def.m_substitutionString = elems[1];
				}
				std::cout << "#define: " << defName << " " << def.m_substitutionString << "\n";
				parser->AddDefine(defName, def);
			}
			continue;
		}
	}

	if(sourceFile == NULL)
	{
		std::cout << "error missing -sourceFile parameter\n";
		return -1;
	}

	try
	{
		{
			macro_parser::definition def;
			def.m_substitutionString = "1";
			parser->AddDefine("GENERATOR", def);

		}

		{
			macro_parser::definition def;
			def.m_substitutionString = "unsigned int ";
			parser->AddDefine("size_t", def);
		}

		std::error_code ec;

		std::string modifiedOutputFile = std::filesystem::canonical(std::string(outputFile), ec).make_preferred().string();
		std::cout << "modified outputFile: " << modifiedOutputFile << "\n";

		std::string modifiedSourceFile = std::filesystem::canonical(std::string(sourceFile), ec).make_preferred().string();
		std::cout << "modified SourceFile: " << modifiedSourceFile << "\n";

        std::vector<std::string> loaded_includes;

		std::ofstream output_file(modifiedOutputFile);
		parser->Load(output_file, modifiedSourceFile, includePaths, loaded_includes);
		output_file.close();
	}
	catch(std::exception ex)
	{
		std::cout << ex.what() << '\n';
	}
	return 0;
}
