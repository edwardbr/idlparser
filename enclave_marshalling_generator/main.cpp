#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <clipp.h>

#include "function_timer.h"

#include "commonfuncs.h"
#include "macro_parser.h"

#include <coreclasses.h>

#include <ComObjectGen.h>
#include <ClientClassWrapper.h>
#include "enclave_marshalling_generator.h"

using namespace std;


xt::function_timer* p_timer = NULL;
std::stringstream verboseStream;

namespace javascript_json{	namespace json	{		extern string namespace_name;	}}

int main(const int argc, char* argv[])
{
	string rootIdl;
	string headerPath;
	string cppPath;
	string output_path;
	std::vector<std::string> namespaces;
	std::vector<std::string> include_paths;
	std::vector<std::string> defines;
	std::vector<std::string> wrong_elements;
	bool dump_preprocessor_output_and_die = false;

	
    auto cli = (
        clipp::required("-i", "--idl").doc("the idl to be parsed") & clipp::value("idl",rootIdl),
        clipp::required("-p", "--output_path").doc("base output path") & clipp::value("output_path",output_path),
        clipp::required("-h", "--header").doc("the generated header relative filename") & clipp::value("header",headerPath),
        clipp::required("-c", "--cpp").doc("the generated source relative filename") & clipp::value("cpp",cppPath),
		clipp::repeatable(clipp::option("-p", "--path") & clipp::value("path",include_paths)).doc("locations of include files used by the idl"),
		clipp::option("-n","--namespace").doc("namespace of the generated interface") & clipp::value("namespace",namespaces),
		clipp::option("--dump_preprocessor_output_and_die").set(dump_preprocessor_output_and_die).doc("dump preprocessor output and die"),
		clipp::repeatable(clipp::option("-D") & clipp::value("define",defines)).doc("macro define"),
		clipp::any_other(wrong_elements)
    );

	clipp::parsing_result res = clipp::parse(argc, argv, cli);
	if(!wrong_elements.empty())
	{
		std::cout << "Error unrecognised parameters\n";
		for(auto& element : wrong_elements)
		{
			std::cout << element << "\n";
		}		

		std::cout << "Please read documentation\n" << clipp::make_man_page(cli, argv[0]);
		return -1;
	}
    if(!res) 
	{
		cout << clipp::make_man_page(cli, argv[0]);
		return -1;
	}

	std::replace( headerPath.begin(), headerPath.end(), '\\', '/'); 
	std::replace( cppPath.begin(), cppPath.end(), '\\', '/'); 
	std::replace( output_path.begin(), output_path.end(), '\\', '/'); 

    std::unique_ptr<macro_parser> parser = std::unique_ptr<macro_parser>(new macro_parser());

	for(auto& define : defines)
	{
		std::vector<std::string> elems;
		split(define, '=', elems);
		{
			macro_parser::definition def;
			std::string defName = elems[0];
			if(elems.size() > 1)
			{
				def.m_substitutionString = elems[1];
			}
			//std::cout << "#define: " << defName << " " << def.m_substitutionString << "\n";
			parser->AddDefine(defName, def);
		}
	}

	std::string pre_parsed_data;
	
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

		auto idl = std::filesystem::absolute(rootIdl, ec);
		if(!std::filesystem::exists(idl))
		{
			cout << "Error file " << idl << " does not exist";
			return -1;
		}

		std::vector<std::filesystem::path> parsed_paths;
		for(auto& path : include_paths)
		{
			parsed_paths.emplace_back(std::filesystem::canonical(path, ec).make_preferred());
		}

        std::vector<std::string> loaded_includes;

		std::stringstream output_stream;
		parser->Load(output_stream, rootIdl, parsed_paths, loaded_includes);
		pre_parsed_data = output_stream.str();
		if(dump_preprocessor_output_and_die)
		{
			std::cout << pre_parsed_data << '\n';
			return 0;
		}
	}
	catch(std::exception ex)
	{
		std::cout << ex.what() << '\n';
		return -1;
	}

	//load the idl file
	Library objects;
	const auto* ppdata = pre_parsed_data.data();
	objects.GetStructure(ppdata,std::string(), true, false);

	try
	{
		string interfaces_h_data;
		string interfaces_cpp_data;

		auto header_path = std::filesystem::path(output_path) / "include" / headerPath;
		auto cpp_path = std::filesystem::path(output_path) / "src" / cppPath;

		std::filesystem::create_directories(header_path.parent_path());
		std::filesystem::create_directories(header_path.parent_path());

		//read the original data and close the files afterwards
		{
			std::error_code ec;
			ifstream hfs(header_path);
			std::getline(hfs, interfaces_h_data, '\0');

			ifstream cfs(cpp_path);
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
			ofstream file(header_path);
			file << header_stream.str();
		}
		if(interfaces_cpp_data != cpp_stream.str())
		{
			ofstream file(cpp_path);
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