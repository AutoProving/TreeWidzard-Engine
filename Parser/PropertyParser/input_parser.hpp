/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_INPUT_INPUT_PARSER_HPP_INCLUDED
# define YY_INPUT_INPUT_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef INPUT_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define INPUT_DEBUG 1
#  else
#   define INPUT_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define INPUT_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined INPUT_DEBUG */
#if INPUT_DEBUG
extern int input_debug;
#endif
/* "%code requires" blocks.  */
#line 5 "input_parser.y"

    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include <iostream>
    #include <vector>
    #include <map>
    #include <utility>


#line 66 "input_parser.hpp"

/* Token type.  */
#ifndef INPUT_TOKENTYPE
# define INPUT_TOKENTYPE
  enum input_tokentype
  {
    SEPERATOR = 258,
    FILEPATH = 259,
    LEFTP = 260,
    RIGHTP = 261,
    NAME = 262,
    NEWLINE = 263,
    AND = 264,
    OR = 265,
    IFF = 266,
    IMPLIES = 267,
    NOT = 268,
    TRUE = 269,
    FALSE = 270,
    COMMENT = 271,
    NUMBER_DOUBLE = 272,
    COMMA = 273,
    FORMULA_NAME = 274,
    EXP = 275,
    ATLEAST = 276,
    ATMOST = 277,
    LESS = 278,
    BIGGER = 279
  };
#endif

/* Value type.  */
#if ! defined INPUT_STYPE && ! defined INPUT_STYPE_IS_DECLARED
union INPUT_STYPE
{
#line 37 "input_parser.y"

     ConjectureNode *conjectureNode;
     double number;
     char* string;
     std::vector<int> * vec;
     PropertyAssignment *property;

#line 110 "input_parser.hpp"

};
typedef union INPUT_STYPE INPUT_STYPE;
# define INPUT_STYPE_IS_TRIVIAL 1
# define INPUT_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined INPUT_LTYPE && ! defined INPUT_LTYPE_IS_DECLARED
typedef struct INPUT_LTYPE INPUT_LTYPE;
struct INPUT_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define INPUT_LTYPE_IS_DECLARED 1
# define INPUT_LTYPE_IS_TRIVIAL 1
#endif


extern INPUT_STYPE input_lval;
extern INPUT_LTYPE input_lloc;
int input_parse (Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty);

#endif /* !YY_INPUT_INPUT_PARSER_HPP_INCLUDED  */
