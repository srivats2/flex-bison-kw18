# flex-bison-kw18
A flex and bison parser that can read in a KW18 file

The configuration files have been provided such that even if you do not have FLEX and BISON on your system the compile will simply use the precompiled headers. 

However, the precompiled headers will be slower than the ones that are compiled natively because they are not using unistd and are in non-interactive mode. Tests have shown about 10% decrease in performance for large files due to this. 

Code has been tested under GCC 5.1.0, clang (602.0.49),  mingw-gcc 5.1.0 and Visual Studio 2012 (with and without win Flex or win Bison
