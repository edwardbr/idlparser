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
#include <nb_json.h>
#include <javascript_json.h>

using namespace std;


xt::function_timer* p_timer = NULL;
std::stringstream verboseStream;

namespace javascript_json{	namespace json	{		extern string namespace_name;	}}

int main(int argv, char* argc[])
{
	string rootIdl;
	string headerPath;
	string cppPath;

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
	DumpClientWrappers(objects, ofstream(headerPath.data()), ofstream(cppPath.data()));

	//dump the files in the outputPath
	CreateComObjects(objects, std::string(""));

	return 0;
}