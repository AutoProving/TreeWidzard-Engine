DESCRIPTION
The files in the current folder are used to parse the input of the program.
The file "parser.y" defines the grammar and the file
"lexer.l" defines the tokens. The files "parser.hpp", "parser.cpp"
and "lexer.cpp" are automatically created from the previous
two files using bison and flex.

CREATING THE PARSER EXECUTABLES

The commands to create these files are the following.

flex -o lexer.cpp lexer.l  // this creates the file lexer.cpp from lexer.l
bison -o parser.cpp parser.y // this creates the file parser.cpp and parser.hpp from parser.y

INSTALLING flex AND bison

The two packages flex and bison can be installed in ubuntu using the following commands.

apt-get install flex
apt-get install bison


USING THE PARSER IN THE MAIN PROGRAM

To use the parser in the program, include the file parser.hpp into main.cpp
