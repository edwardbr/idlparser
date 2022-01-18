#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "function_timer.h"

#include "commonfuncs.h"

#include <coreclasses.h>

#include <ComObjectGen.h>
#include <ClientClassWrapper.h>
#include "enclave_marshalling_generator.h"

using namespace std;


xt::function_timer* p_timer = NULL;
std::stringstream verboseStream;

namespace javascript_json{	namespace json	{		extern string namespace_name;	}}

int main(int argv, char* argc[])
{
	string rootIdl;
	string headerPath;
	string cppPath;
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

		//read the original data and close the files afterwards
		{
			std::error_code ec;
			headerPath = std::filesystem::canonical(headerPath, ec).make_preferred().string();
			ifstream hfs(headerPath.data());
			std::getline(hfs, interfaces_h_data, '\0');

			cppPath = std::filesystem::canonical(cppPath, ec).make_preferred().string();
			ifstream cfs(cppPath.data());
			std::getline(cfs, interfaces_cpp_data, '\0');
		}

		std::stringstream header_stream;
		std::stringstream cpp_stream;

		//do the generation to the ostrstreams
		{
			enclave_marshalling_generator::host_ecall::write_files(objects, header_stream, cpp_stream, namespaces, headerPath);

			header_stream << ends;
			cpp_stream << ends;
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

	}
	catch (std::string msg)
	{
		std::cerr << msg << std::endl;
		return 1;
	}

	return 0;
}