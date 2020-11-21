#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include "function_timer.h"

#include <coreclasses.h>

#include <ComObjectGen.h>
#include <ClientClassWrapper.h>
#include <javascript_json.h>
#include "commonfuncs.h"

#include <macrohandler.h>
#include "edl_macro_handler.h"
#include "flatbuffer_gen.h"

xt::function_timer* p_timer = NULL;
std::stringstream verboseStream;

int main(int argc, char* argv[])
{
    std::unique_ptr<macro_parser> parser = std::unique_ptr<macro_parser>(new edl_macro_parser());

    std::string sourceFile;
    std::string preparseFile;
    std::string outputFile;
    paths includePaths;
    std::string flatbufferPath;
    std::string headerPath;
    std::string cppPath;
    std::string flatbuffer_class_name;
    std::vector<std::string> namespaces;

    {
        // get command line values
        for (int i = 0; i < argc; i++)
        {
            if (!strcmp(argv[i], "-sourceFile"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -sourceFile parameter\n";
                    return -1;
                }
                sourceFile = argv[i];
                std::cout << "-sourceFile " << sourceFile << "\n";
                continue;
            }
            else if (!strcmp(argv[i], "-outputFile"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -outputFile parameter\n";
                    return -1;
                }
                outputFile = argv[i];
                std::cout << "-outputFile " << outputFile << "\n";
                continue;
            }
            else if (!strcmp(argv[i], "-preparseFile"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -preparseFile parameter\n";
                    return -1;
                }
                preparseFile = argv[i];
                std::cout << "-preparseFile " << preparseFile << "\n";
                continue;
            }
            else if (!strcmp(argv[i], "-includePath"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -includePath parameter\n";
                    return -1;
                }

                char* includePath = argv[i];
                std::cout << "-includePath " << includePath << "\n";
                std::vector<std::string> results;

#ifdef WIN32
                split(includePath, ';', results);
#else
                split(includePath, ':', results);
#endif
                for (auto result : results)
                {
                    std::filesystem::path p(result);
                    if (p.has_root_directory())
                    {
                        includePaths.push_back(p);
                        std::cout << "include directory: " << p.string() << "\n";
                    }
                    else
                    {
                        includePaths.push_back(std::filesystem::current_path() / p);
                        std::cout << "include directory: " << (std::filesystem::current_path() / p).string() << "\n";
                    }
                }
                continue;
            }
            else if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == 'D')
            {
                if (i >= argc)
                {
                    std::cout << "error missing -D parameter\n";
                    return -1;
                }
                std::vector<std::string> elems;
                split(argv[i] + 2, '=', elems);
                {
                    macro_parser::definition def;
                    std::string defName = elems[0];
                    if (elems.size() > 1)
                    {
                        def.m_substitutionString = elems[1];
                    }
                    std::cout << "#define: " << defName << " " << def.m_substitutionString << "\n";
                    parser->AddDefine(defName, def);
                }
                continue;
            }
            else if (!strcmp(argv[i], "-flatbufferPath"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -flatbufferPath parameter\n";
                    return -1;
                }
                flatbufferPath = argv[i];
                continue;
            }
            else if (!strcmp(argv[i], "-headerPath"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -headerPath parameter\n";
                    return -1;
                }
                headerPath = argv[i];
                continue;
            }
            else if (!strcmp(argv[i], "-cppPath"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -cppPath parameter\n";
                    return -1;
                }
                cppPath = argv[i];
                continue;
            }
            else if (!strcmp(argv[i], "-flatbuffer_class_name"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -flatbuffer_class_name parameter\n";
                    return -1;
                }
                flatbuffer_class_name = argv[i];
                continue;
            }
            else if (!strcmp(argv[i], "-generated_namespace"))
            {
                i++;
                if (i >= argc)
                {
                    std::cout << "error missing -generated_namespace parameter\n";
                    return -1;
                }

                split(argv[i], ':', namespaces);
                continue;
            }
        }

        if (sourceFile.empty())
        {
            std::cout << "error missing -sourceFile parameter\n";
            return -1;
        }
        if (flatbufferPath.empty() == true)
        {
            std::cout << "error missing -flatbufferPath parameter\n";
            return -1;
        }
        if (headerPath.empty() == true)
        {
            std::cout << "error missing -headerPath parameter\n";
            return -1;
        }
        if (cppPath.empty() == true)
        {
            std::cout << "error missing -cppPath parameter\n";
            return -1;
        }
    }

    try
    {
        std::cout << "modified preparseFile: " << preparseFile << "\n";
        std::cout << "modified SourceFile: " << sourceFile << "\n";

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

            std::ifstream source_file(sourceFile);
            std::ofstream preparse_file(preparseFile);
            parser->Load(preparse_file, source_file, includePaths);
            preparse_file.close();
        }
        {
            // load the idl file
            Library objects;
            objects.Load(preparseFile.data());

            std::string interfaces_h_data;
            std::string interfaces_cpp_data;
            std::string ajax_data;

            // read the original data and close the files afterwards
            {
                std::error_code ec;
                //headerPath = std::filesystem::canonical(headerPath, ec).make_preferred().string();
                std::ifstream hfs(headerPath.data());
                std::getline(hfs, interfaces_h_data, '\0');

                //cppPath = std::filesystem::canonical(cppPath, ec).make_preferred().string();
                std::ifstream cfs(cppPath.data());
                std::getline(cfs, interfaces_cpp_data, '\0');

                //flatbufferPath = std::filesystem::canonical(flatbufferPath, ec).make_preferred().string();
                std::ifstream afs(flatbufferPath.data());
                std::getline(afs, ajax_data, '\0');
            }

            std::stringstream header_stream;
            std::stringstream cpp_stream;
            std::ofstream file(flatbufferPath.data());//to be reverted
            std::stringstream fb_stream;

            // do the generation to the ostrstreams
            {
                flatbuffer::writeFiles(objects, file, namespaces );

                /*javascript_json::json::namespace_name = flatbuffer_class_name;
                javascript_json::json::writeJSONFiles(objects, ajax_stream);*/
            }

            // compare and write if different
            if (interfaces_h_data != header_stream.str())
            {
                std::ofstream file(headerPath.data());
                file << header_stream.str();
            }
            if (interfaces_cpp_data != cpp_stream.str())
            {
                std::ofstream file(cppPath.data());
                file << cpp_stream.str();
            }
            if (ajax_data != fb_stream.str())
            {
                std::ofstream file(flatbufferPath.data());
                file << fb_stream.str();
            }
        }
    }
    catch (std::string msg)
    {
        std::cerr << msg << std::endl;
        return 1;
    }
    catch (std::exception ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}