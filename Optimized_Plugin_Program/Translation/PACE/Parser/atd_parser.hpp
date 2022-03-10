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

#ifndef YY_ATD_ATD_PARSER_HPP_INCLUDED
# define YY_ATD_ATD_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef ATD_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define ATD_DEBUG 1
#  else
#   define ATD_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define ATD_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined ATD_DEBUG */
#if ATD_DEBUG
extern int atd_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "atd_parser.y"

    #include "../../../TreeAutomaton/AbstractTreeDecomposition.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <map>
    #include <set>
    #include <tuple>

#line 66 "atd_parser.hpp"

/* Token type.  */
#ifndef ATD_TOKENTYPE
# define ATD_TOKENTYPE
  enum atd_tokentype
  {
    atd_NUM = 258,
    atd_COMMENT = 259,
    atd_NEWLINE = 260,
    atd_LEAF = 261,
    atd_INTROVERTEX = 262,
    atd_INTROEDGE = 263,
    atd_FORGETVERTEX = 264,
    atd_JOIN = 265,
    atd_LEFTP = 266,
    atd_RIGHTP = 267,
    atd_SEP1 = 268,
    atd_SEP2 = 269
  };
#endif

/* Value type.  */
#if ! defined ATD_STYPE && ! defined ATD_STYPE_IS_DECLARED
union ATD_STYPE
{
#line 38 "atd_parser.y"

     unsigned number;
     char* string;

#line 97 "atd_parser.hpp"

};
typedef union ATD_STYPE ATD_STYPE;
# define ATD_STYPE_IS_TRIVIAL 1
# define ATD_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined ATD_LTYPE && ! defined ATD_LTYPE_IS_DECLARED
typedef struct ATD_LTYPE ATD_LTYPE;
struct ATD_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define ATD_LTYPE_IS_DECLARED 1
# define ATD_LTYPE_IS_TRIVIAL 1
#endif


extern ATD_STYPE atd_lval;
extern ATD_LTYPE atd_lloc;
int atd_parse (AbstractTreeDecomposition  &atd, int &result);

#endif /* !YY_ATD_ATD_PARSER_HPP_INCLUDED  */
