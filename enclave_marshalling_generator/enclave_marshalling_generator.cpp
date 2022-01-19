
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
        void write_interface_predeclaration(const Library& lib, const ClassObject& m_ob, writer& header, writer& source)
        {
            source("class {}_proxy;", 
                m_ob.name, 
                m_ob.name);
        };

        void write_interface(const Library& lib, const ClassObject& m_ob, writer& header, writer& source)
        {
            header("class {}{}{} : public i_unknown", 
                m_ob.name, 
                m_ob.parentName.empty() ? "" : ":",
                m_ob.parentName);
			header("{{");
			header("public:");

            source("class {}_proxy : public {}", 
                m_ob.name, 
                m_ob.name);
			source("{{");
            source("xxxxx& xxxx_;");
            source("uint64_t object_id_;");
			source("public:");
			source("");
            source("{}_proxy(xxxxx& xxxx, uint64_t object_id) : ", m_ob.name);
            source("  xxxx_(xxxx),", m_ob.name);
            source("  object_id_(object_id)", m_ob.name);
            source("  {{}}", m_ob.name);
			source("");

            for(auto& function : m_ob.functions)
            {
                if(function.type != FunctionTypeMethod)
                    continue;
                
                header.print_tabs();
                source.print_tabs();
                header.raw("virtual {} {}(", function.returnType, function.name);
                source.raw("virtual {} {}_proxy::{} override (", function.returnType, m_ob.name, function.name);
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

                source("const auto in_ =  = yas::save<yas::mem|yas::json>(YAS_OBJECT_NVP(");
                source("  \"in\"");

                int count = 0;
                for(auto& parameter : function.parameters)
                {
                    auto in_it = std::find(parameter.m_attributes.begin(), parameter.m_attributes.end(), "in");
                    auto out_it = std::find(parameter.m_attributes.begin(), parameter.m_attributes.end(), "out");
                    auto byval_it = std::find(parameter.m_attributes.begin(), parameter.m_attributes.end(), "by_value");
                    if(out_it != parameter.m_attributes.end() && in_it == parameter.m_attributes.end())
                        continue;

                    source("  ,(\"_{}\", {})", count++, parameter.name);
                }
                
                source("  ));");
                
                source("std::vector<uint8_t> out_;");
                source("int ret = call_stub(in_, out_);");
                source("if(ret)");
                source("{{");
                source("return ret;");
                source("}}");

                source("yas::load<yas::mem|yas::json>(out_, YAS_OBJECT_NVP(");
                source("  \"out\"");

                count = 0;
                for(auto& parameter : function.parameters)
                {
                    auto in_it = std::find(parameter.m_attributes.begin(), parameter.m_attributes.end(), "in");
                    auto out_it = std::find(parameter.m_attributes.begin(), parameter.m_attributes.end(), "out");
                    if(out_it == parameter.m_attributes.end() && in_it != parameter.m_attributes.end())
                        continue;

                    source("  ,(\"_{}\", {})", count++, parameter.name);
                }
                source("  ));");

                source("return ret;");
                source("}}");
                source("");
            }

            header("}};");            
            header("");

            source("}};");            
            source("");
        };

        void write_struct(const ClassObject& m_ob, writer& header, writer& source)
        {
            header("struct {}{}{}", 
                m_ob.name, 
                m_ob.parentName.empty() ? "" : ":",
                m_ob.parentName);
			header("{{");

            for(auto& field : m_ob.functions)
            {
                if(field.type != FunctionTypeVariable)
                    continue;
                
                header.print_tabs();
                header.raw("{} {};\n", field.returnType, field.name);
            }

            header("");
            header("// one member-function for save/load");
            header("template<typename Ar>");
            header("void serialize(Ar &ar)");
            header("{{");
            header("ar & YAS_OBJECT(\"{}\"",m_ob.name);

            int count = 0;
            for(auto& field : m_ob.functions)
            {
                header("  ,(\"_{}\", {})", count++, field.name);
            }
            header(")");

            header("}}");
			

            header("}};");
        };

        void write_library(const Library& lib, const ClassObject& m_ob, writer& header, writer& source)
        {
            for(auto& name : m_ob.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectTypeInterface)
                    write_interface_predeclaration(lib, *obj, header, source);
            }
            
            source("");

            for(auto& name : m_ob.m_ownedClasses)
            {
                const ClassObject* obj = nullptr;
                if(!lib.FindClassObject(name, obj))
                {
                    continue;
                }
                if(obj->type == ObjectTypeInterface)
                    write_interface(lib, *obj, header, source);
            }


            header("//a marshalable interface for other zones");
            header("class i_{} : public i_zone", m_ob.name);
			header("{{");
            header("public:");
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
                    header("virtual error_code query_interface(i_unknown& from, remote_shared_ptr<{0}>& to) = 0;", obj->name);
                }
            }	

            header("");
            header("//static functions passed to global functions in the target zone");
            for(auto& function : m_ob.functions)
            {
                if(function.type != FunctionTypeMethod)
                    continue;
                
                header.print_tabs();
                header.raw("virtual {} {}(", function.returnType, function.name);
                bool has_parameter = false;
                for(auto& parameter : function.parameters)
                {
                    if(has_parameter)
                    {
                        header.raw(", ");
                    }
                    has_parameter = true;
                    header.raw("{} {}",parameter.type, parameter.name);
                }
                header.raw(") = 0;\n");
            }
            
            header("}};");
            header("");
            
            
            header("#ifndef _IN_ENCLAVE");
            header("//the class that encapsulates an environment or zone");
            header("//only host code can use this class directly other enclaves *may* have access to the i_zone derived interface");
            header("class {} : public i_{}", m_ob.name, m_ob.name);
			header("{{");
            header("std::unique_ptr<enclave_info> enclave_;");
            header("public:");

            header("{}();", m_ob.name);
            header("~{}();", m_ob.name);
            header("");
            header("error_code load(std::string& dll_file_name);");
            header("");

            source("error_code {}::load(std::string& dll_file_name);", m_ob.name);
            source("{{");
            source("}}");
            source("");

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
                    header("error_code query_interface(i_unknown& from, remote_shared_ptr<{}>& to) override;", obj->name);
                    source("error_code {}::query_interface(i_unknown& from, remote_shared_ptr<{}>& to) override;", obj->name, m_ob.name);
                    source("{{");
                    source("}}");
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
                source.raw("{} {}::{}(", function.returnType, m_ob.name, function.name);
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
                header.raw(") override;\n");
                source.raw(")\n");
                source("{{");
                source("}}");
            }            
            header("}};");
            header("#endif //_IN_ENCLAVE");
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

            header("//a shared pointer that works accross enclaves");
            header("template<class T>class remote_shared_ptr{{}};");
            header("");

            header("//a weak pointer that works accross enclaves");
            header("template<class T>class remote_weak_ptr{{}};");
            header("");

            header("class enclave_info;");
            header("");            

            header("//the base interface to all interfaces");
            header("class i_unknown{{}};");
            header("");

            header("//a handler for new threads, this function needs to be thread safe!");
            header("class i_thread_target : public i_unknown");
            header("{{");
            header("virtual error_code thread_started(std::string& thread_name) = 0;");
            header("}};");
            header("");

            header("//a message channel between zones (a pair of spsc queues behind an executor) not thread safe!");
            header("class i_message_channel : public i_unknown{{}};");            
            header("");

            header("//a handler for new threads, this function needs to be thread safe!");
            header("class i_message_target : public i_unknown");
            header("{{");
            header("//Set up a link with another zone");
            header("virtual error_code add_peer_channel(std::string link_name, i_message_channel& channel) = 0;");
            header("//This will be called if the other zone goes down");
            header("virtual error_code remove_peer_channel(std::string link_name) = 0;");
            header("}};");
            header("");

            header("//logical security environment");
            header("class i_zone : public i_unknown");
            header("{{");
            header("//this runs until the thread dies, this will also setup a connection with the message pump");
            header("void start_thread(i_thread_target& target, std::string thread_name);");
            header("");

            header("//this is to allow messaging between enclaves this will create an i_message_channel");
            header("error_code create_message_link(i_message_target& target, i_zone& other_zone, std::string link_name);");
            header("}};");
            header("");

  			source("#include \"{}\"", header_filename);
            source("");


            for(auto& ns : namespaces)
            {
                header("namespace {}", ns);
                header("{{");
                source("namespace {}", ns);
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

            for(auto& ns : namespaces)
            {
                header("}}");
                source("}}");
            }
        }
    }
}