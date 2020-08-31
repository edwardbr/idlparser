# idlparser
Yes well I do not make any apologies for this.  I wrote this around 1998 to parse header and idl files, and then used it often to generate code to generate interfaces between COM, CORBA, XMLRPC and REST interfaces.  It is a complete mess and I have not used it since 2017, being a useful proof of concept I have never got around to tidying up.  I think it is time now that I did.

It consists of a simple preprocessor that can import data from other files using #include or import style declarations, strip out comments and then expand out macros into naked c/c++.  

Once the preprocessor has done its stuff, AST parser can be used to interpret the cleaned data into a tree of types and function definitions.  The parser inderstands square bracket style attributes used in COM style IDL.  It is not picky on the dialect of file you are using it understands COM and CORBA idl, or simple c++ headers.  However if you have for instance a function called coclass, it may misinterpret it as a COM class rather than a function that has nothing to do with COM, therefore you may need to tweek the code. 

You then need to write your own a code generator to output the interface implementation according to your need.

The AST understands classes, scructs, member functions and variables and basic templating, it does not understand c++ concepts such as rvalue references, lambdas and constant expressions.

Although there are better commercial preprocessors, this implementation strips out all the spaces and unnecessary carriage returns, thereby unencumbering the the AST parser from needing to deal with these.

I will start work on tidying it up and making it compile on cmake.

The initial commit, is a version designed to read idl files and generate json rpc coroutines to work with libuv and asio.  Other versions were commissioned by various clients and I no longer have the code for them.
