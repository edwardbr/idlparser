#pragma once

// standard attributes that are used by the idl parser, other attributes may be used by the generator

namespace attribute_types
{
    ///////////////////////////////
    // function modifiers
    ///////////////////////////////

    // this makes a function const the idl parser does not support modifiers at the end of a function declaration after the last ')'
    constexpr const char* const_function = "const";   


    ///////////////////////////////
    // parameter modifiers
    ///////////////////////////////

    // in parameter (the default)
    constexpr const char* in_param = "in";

    // out parameter for returning values to the client
    constexpr const char* out_param = "out";

    // pass by value parameter (not really needed now?)
    constexpr const char* by_value_param = "by_value";


    // attributes that change the default behaviour of the idl parser
    
    // please dont use this, struct and class are not keywords that the idl parser should be tolerating
    constexpr const char* tolerate_struct_or_class = "buggy_tolerate_struct_or_class";
}