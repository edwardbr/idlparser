# idlparser
Yes well I do not make any apologies for this.  I wrote this around 1998 to parse header and idl files, and then used it often to generate code to generate interfaces between COM, CORBA, XMLRPC and REST interfaces.  It is a complete mess and I have not used it since 2017, being a useful proof of concept I have never got around to tidying up.  I think it is time now that I did.

It consists of a simple preprocessor that expands out macros into naked c/c++.  
And then a parser to read the C++ into an AST for processing by a code generator.

The AST understands classes, scructs, member functions and variables and basic templating, it does not understand c++ concepts such as rvalue references, lambdas and constant expressions.

Although there are better commercial preprocessors, this implementation strips out all the spaces and unnecessary carriage returns, thereby unencumbering the the AST parser from needing to deal with these.

I will start work on tidying it up and making it compile on cmake.

The initial commit, is a version designed to read idl files and generate json rpc coroutines to work with libuv and asio.  Other versions were commissioned by various clients and I no longer have the code for them.
