
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
        print_tabs();
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
        count_ += tmp/2;
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
        //entry point
        void write_files(const ClassObject& m_ob, std::ostream& hos, std::ostream& cos, const std::vector<std::string>& namespaces, const std::string& header_filename)
        {
            writer header(hos);
            writer source(cos);

            for(auto& ns : namespaces)
            {
                header("namespace {}{{", ns);
                source("namespace {}{{", ns);
            }
			header("class {}{}{}", 
                m_ob.name, 
                m_ob.parentName.empty() ? "" : ":",
                m_ob.parentName);
			header("{{");
			source("{{");

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

			
			header("#include \"json.h\"");
			source("#include \"{}\"", header_filename);

            header("}}");
            source("}}");
            for(auto& ns : namespaces)
            {
                header("}}");
                source("}}");
            }
        }
    }
}