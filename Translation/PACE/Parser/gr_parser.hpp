/* A Bison Parser, made by GNU Bison 3.6.4.  */

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
   part or all of the Bison Parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   Parser generator using the skeleton or a modified version thereof
   as a Parser skeleton.  Alternatively, if you modify or redistribute
   the Parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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
#line 3 "gr_parser.y"

    #include "../../../Multigraph/MultiGraph.h"
    #include <iostream>
    #include <vector>

#line 63 "gr_parser.hpp"

/* Token kinds.  */
#ifndef GR_TOKENTYPE
# define GR_TOKENTYPE
  enum gr_tokentype
  {
    GR_EMPTY = -2,
    GR_EOF = 0,                    /* "end of file"  */
    GR_error = 256,                /* error  */
    GR_UNDEF = 257,                /* "invalid token"  */
    GR_P = 258,                    /* GR_P  */
    GR_TW = 259,                   /* GR_TW  */
    GR_NUM = 260,                  /* GR_NUM  */
    GR_COMMENT = 261,              /* GR_COMMENT  */
    GR_NEWLINE = 262               /* GR_NEWLINE  */
  };
  typedef enum gr_tokentype gr_token_kind_t;
#endif

/* Value type.  */
#if ! defined GR_STYPE && ! defined GR_STYPE_IS_DECLARED
union GR_STYPE
{
#line 23 "gr_parser.y"

     unsigned number;
     char* string;

#line 92 "gr_parser.hpp"

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
