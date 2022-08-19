/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_GR_GR_PARSER_HPP_INCLUDED
# define YY_GR_GR_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef GR_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define GR_DEBUG 1
#  else
#   define GR_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define GR_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined GR_DEBUG */
#if GR_DEBUG
extern int gr_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "gr_parser.y" /* yacc.c:1909  */

    #include "../../../Multigraph/MultiGraph.h"
    #include <iostream>
    #include <vector>

#line 58 "gr_parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef GR_TOKENTYPE
# define GR_TOKENTYPE
  enum gr_tokentype
  {
    GR_P = 258,
    GR_TW = 259,
    GR_NUM = 260,
    GR_COMMENT = 261,
    GR_NEWLINE = 262
  };
#endif

/* Value type.  */
#if ! defined GR_STYPE && ! defined GR_STYPE_IS_DECLARED

union GR_STYPE
{
#line 23 "gr_parser.y" /* yacc.c:1909  */

     unsigned number;
     char* string;

#line 83 "gr_parser.hpp" /* yacc.c:1909  */
};

typedef union GR_STYPE GR_STYPE;
# define GR_STYPE_IS_TRIVIAL 1
# define GR_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined GR_LTYPE && ! defined GR_LTYPE_IS_DECLARED
typedef struct GR_LTYPE GR_LTYPE;
struct GR_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define GR_LTYPE_IS_DECLARED 1
# define GR_LTYPE_IS_TRIVIAL 1
#endif


extern GR_STYPE gr_lval;
extern GR_LTYPE gr_lloc;
int gr_parse (MultiGraph &graph, int &result);

#endif /* !YY_GR_GR_PARSER_HPP_INCLUDED  */
