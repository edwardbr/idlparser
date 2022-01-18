
#include "coreclasses.h"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <type_traits>
#include <algorithm>


class writer
{
    std::ostream& strm_;
    int count_ = 0;
public:
    writer(std::ostream& strm) : strm_(strm){};
    template <
        typename S, 
        typename... Args>
    void operator()(const S& format_str, Args&&... args)
    {
        int tmp = 0;
        std::for_each(std::begin(format_str), std::end(format_str), [&](char val){
            if(val == '{')
            {
                tmp++;
            }
            else if(val == '}')
            {
                tmp--;
            }
        });
        assert(!(tmp%2));//should always be in pairs
        if(tmp >= 0)
            print_tabs();
        count_ += tmp/2;
        if(tmp < 0)
            print_tabs();
        fmt::print(strm_, format_str, args...);
        fmt::print(strm_, "\n");
    }
    template <
        typename S, 
        typename... Args>
    void raw(const S& format_str, Args&&... args)
    {
        fmt::print(strm_, format_str, args...);
    }
    void print_tabs()
    {
        for(int i = 0;i < count_;i++)
        {
            strm_ << '\t';
        }
    }
};

namespace enclave_marshalling_generator
{
    namespace host_ecall
    {
        void write_interface(const ClassObject& m_ob, writer& header, writer& source)
        {
            header("class {}{}{} : public i_unknown", 
                m_ob.name, 
                m_ob.parentName.empty() ? "" : ":",
                m_ob.parentName);
			header("{{");
			source("{{");
			header("public:");

            for(auto& function : m_ob.functions)
            {
                if(function.type != FunctionTypeMethod)
                    continue;
                
                header.print_tabs();
                source.print_tabs();
                header.raw("virtual {} {}(", function.returnType, function.name);
                source.raw("virtual {} {}(", function.returnType, function.name);
                bool has_parameter = false;
                for(auto& parameter : function.parameters)
                {
                    if(has_parameter)
                    {
                        header.raw(", ");
                        source.raw(", ");
                    }
                    has_parameter = true;
                    header.raw("{} {}",parameter.type, parameter.name);
                    source.raw("{} {}",parameter.type, parameter.name);
                }
                header.raw(") = 0;\n");
                source.raw(")\n");
                source("{{");
                source("}}");
            }

            header("}};");
            source("}}");
            header("");
        };

        void write_struct(const ClassObject& m_ob, writer& header, writer& source)
        {
            header("struct {}{}{}", 
                m_ob.name, 
                m_ob.parentName.empty() ? "" : ":",
                m_ob.parentName);
			header("{{");
			source("{{");

            for(auto& function : m_ob.functions)
            {
                if(function.type != FunctionTypeVariable)
                    continue;
                
                header.print_tabs();
                header.raw("{} {};\n", function.returnType, function.name);
                source("{{");
                source("}}");
            }

			

            header("}};");
            source("}}");
        };

        void write_library(const Library& lib, const ClassObject& m_ob, writer& header, writer& source)
        {
            header("//a placeholder class");
            header("class i_unknown");
            header("{{");
            header("}};");
            header("");

            header("//a handler for new threads, this function needs to be thread safe!");
            header("class i_thread_target : public i_unknown");
            header("{{");
            header("virtual error_code thread_started(std::string& thread_name) = 0;");
            header("}};");
            header("");

            header("//a handler for new threads, this function needs to be thread safe!");
            header("class i_message_target : public i_unknown");
            header("{{");
            header("virtual error_code add_peer(std::string peer_name, i_message_channel& channel) = 0;");
            header("virtual error_code remove_queue_pair(std::string peer_name) = 0;");
            header("}};");
            header("");

            for(auto& name : m_ob.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectTypeInterface)
                    write_interface(*obj, header, source);
            }

            header("//the class that encapsulates an environment or zone");
            header("class {} : public i_zone", m_ob.name);
			header("{{");
            header("public:");

            header("{}(std::string& dll_file_name);", m_ob.name);
            header("~{}();", m_ob.name);
            header("");

            header("//this runs until the thread dies, this will also setup a connection with the message pump");
            header("void start_thread(std::string thread_name, i_thread_target& target);");
            header("");

            header("//this is to allow messaging between enclaves");
            header("error_code create_message_link(std::string link_name, i_zone& other_zone, i_message_channel& channel);");
            header("");

            header("//polymorphic helper functions");
            for(auto& name : m_ob.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectTypeInterface)
                {
                    header("error_code query_interface(i_unknown& from, std::shared_ptr<{0}>& to);", obj->name);
                }
            }	

            header("");
            header("//static functions passed to global functions in the target zone");
            for(auto& function : m_ob.functions)
            {
                if(function.type != FunctionTypeMethod)
                    continue;
                
                header.print_tabs();
                source.print_tabs();
                header.raw("{} {}(", function.returnType, function.name);
                source.raw("{} {}(", function.returnType, function.name);
                bool has_parameter = false;
                for(auto& parameter : function.parameters)
                {
                    if(has_parameter)
                    {
                        header.raw(", ");
                        source.raw(", ");
                    }
                    has_parameter = true;
                    header.raw("{} {}",parameter.type, parameter.name);
                    source.raw("{} {}",parameter.type, parameter.name);
                }
                header.raw(");\n");
                source.raw(")\n");
                source("{{");
                source("}}");
            }
            header("}};");
            source("}};");
        };

        //entry point
        void write_files(const Library& lib, std::ostream& hos, std::ostream& cos, const std::vector<std::string>& namespaces, const std::string& header_filename)
        {
            writer header(hos);
            writer source(cos);

            header("#include <memory>");
            header("#include <vector>");
            header("#include <map>");
            header("#include <string>");
            header("");
            header("using error_code = int;");
            header("");
            header("//logical security environments");
            header("class i_zone{{}};");
            header("//a message channel between zones (a pair of spsc queues behind an executor)");
            header("class i_message_channel{{}};");            
            header("");

            for(auto& ns : namespaces)
            {
                header("namespace {}", ns);
                header("{{");
                source("namespace {}{{", ns);
                source("{{");
            }

            for(auto& name : lib.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectStruct)
                    write_struct(*obj, header, source);
            }
            header("");

            for(auto& name : lib.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectLibrary)
                    write_library(lib, *obj, header, source);
            }

			source("#include \"{}\"", header_filename);


            for(auto& ns : namespaces)
            {
                header("}}");
                source("}}");
            }
        }
    }
}