# idlparser
Yes well I do not make any apologies for this.  I wrote this around 1998 to parse header and idl files, and then used it often to generate code to generate interfaces between COM, CORBA, XMLRPC and REST interfaces.  It was checked in as a complete mess, being a useful proof of concept I have never got around to tidying up until now.  As the code is prehistoric and was written by me a green programmer at the time there is plenty of bad practice in the code that I need to clean up, however as a fire and forget code generator it is generally OK as a development tool, in no way should it be used in non developer environments.

It consists of a simple preprocessor that can import data from other files using #include or import style declarations, strip out comments, then expand out macros into naked idl/c/c++.  It blends the features of several interface specification files rather than being configured for one specific one.  If you have a breaking difference in your interface definition file then you may need to fork the code.

Once the preprocessor has done its stuff, AST parser can be used to interpret the cleaned data into a tree of types and function definitions.  The parser inderstands square bracket style attributes used in for instance COM style IDL.  It is not picky on the dialect of file you are using, it understands COM and CORBA idl, or simple pre-c++11 c/c++ headers.  You may get name clashes however, if you have for instance a function called coclass, it may misinterpret it as a COM class rather than a function that has nothing to do with COM, therefore you may need to tweek the code somewhere. 

You then need to write your own a code generator to output the interface implementation according to your need.  There are several example generators included, I will try and dig out some example code that utilise them.  

The AST understands classes, scructs, member functions and variables and basic templating, it does not understand c++ concepts such as rvalue references, lambdas and constant expressions.  It also does not understand the bodies of functions, nor was it ever designed to do so.  If you need to third party class definitions in your idl, but do not need to specify their implementations you can declare stubbed out definitions for the parser to consume, please have a look in the defines folder for example stubbed definitions for some stl collection classes.  

Although there are better commercial preprocessors, this implementation is designed for the AST parser, unless there is a strong need to use an alternative preprocessor, use this one.

TODO: 
reactivate some tests and examples
improve documentation
