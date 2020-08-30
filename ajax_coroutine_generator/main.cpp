#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "function_timer.h"

#include <coreclasses.h>

#include <ComObjectGen.h>
#include <ClientClassWrapper.h>
#include <nb_json.h>
#include <javascript_json.h>

using namespace std;
using namespace stdext;


inline ostream& operator << (ostream& _O, const std::string& _X)
{
    _O << _X.c_str();
    return _O; 
}


xt::function_timer* p_timer = NULL;
std::stringstream verboseStream;

namespace javascript_json{	namespace json	{		extern string namespace_name;	}}

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
	string rootIdl;
	string javascriptPath;
	string headerPath;
	string cppPath;
	string ajax_class_name;
	std::vector<std::string> namespaces;

	//get command line values
	for(int i = 0; i < argv;i++)
	{
		if(!strcmp(argc[i],"-rootIdl"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -rootIdl parameter" << ends;
				return -1;
			}
			rootIdl = argc[i];
			continue;
		}
		if (!strcmp(argc[i], "-javascriptPath"))
		{
			i++;
			if (i >= argv)
			{
				cout << "error missing -javascriptPath parameter" << ends;
				return -1;
			}
			javascriptPath = argc[i];
			continue;
		}
		if (!strcmp(argc[i], "-headerPath"))
		{
			i++;
			if (i >= argv)
			{
				cout << "error missing -headerPath parameter" << ends;
				return -1;
			}
			headerPath = argc[i];
			continue;
		}
		if (!strcmp(argc[i], "-cppPath"))
		{
			i++;
			if (i >= argv)
			{
				cout << "error missing -cppPath parameter" << ends;
				return -1;
			}
			cppPath = argc[i];
			continue;
		}
		if(!strcmp(argc[i],"-ajax_class_name"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -ajax_class_name parameter" << ends;
				return -1;
			}
			ajax_class_name = argc[i];
			continue;
		}
		if(!strcmp(argc[i],"-generated_namespace"))
		{
			i++;
			if(i >= argv)
			{
				cout << "error missing -generated_namespace parameter" << ends;
				return -1;
			}

			split(argc[i], ':', namespaces);
			continue;
		}
	}

	if(rootIdl.empty())
	{
		cout << "error missing -rootIdl parameter" << ends;
		return -1;
	}

	if (javascriptPath.empty() == true)
	{
		cout << "error missing -javascriptPath parameter" << ends;
		return -1;
	}
	if (headerPath.empty() == true)
	{
		cout << "error missing -headerPath parameter" << ends;
		return -1;
	}
	if (cppPath.empty() == true)
	{
		cout << "error missing -cppPath parameter" << ends;
		return -1;
	}

	//load the idl file
	Library objects;
	objects.Load(rootIdl.data());

	try
	{
		string interfaces_h_data;
		string interfaces_cpp_data;
		string ajax_data;

		//read the original data and close the files afterwards
		{
			std::error_code ec;
			headerPath = std::filesystem::canonical(headerPath, ec).make_preferred().string();
			ifstream hfs(headerPath.data());
			std::getline(hfs, interfaces_h_data, '\0');

			cppPath = std::filesystem::canonical(cppPath, ec).make_preferred().string();
			ifstream cfs(cppPath.data());
			std::getline(cfs, interfaces_cpp_data, '\0');

			javascriptPath = std::filesystem::canonical(javascriptPath, ec).make_preferred().string();
			ifstream afs(javascriptPath.data());
			std::getline(afs, ajax_data, '\0');
		}

		std::stringstream header_stream;
		std::stringstream cpp_stream;
		std::stringstream ajax_stream;

		//do the generation to the ostrstreams
		{
			non_blocking::json::writeFiles(objects, header_stream, cpp_stream, namespaces/*, headerPath*/);

			header_stream << ends;
			cpp_stream << ends;

			javascript_json::json::namespace_name = ajax_class_name;
			javascript_json::json::writeJSONFiles(objects, ajax_stream);
			ajax_stream << ends;
		}

		//compare and write if different
		if(interfaces_h_data != header_stream.str())
		{
			ofstream file(headerPath.data());
			file << header_stream.str();
		}
		if(interfaces_cpp_data != cpp_stream.str())
		{
			ofstream file(cppPath.data());
			file << cpp_stream.str();
		}
		if(ajax_data != ajax_stream.str())
		{
			ofstream file(javascriptPath.data());
			file << ajax_stream.str();
		}

	}
	catch (std::string msg)
	{
		std::cerr << msg << std::endl;
		return 1;
	}

	return 0;
}