/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_TERM_TERM_PARSER_HPP_INCLUDED
# define YY_TERM_TERM_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef TERM_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define TERM_DEBUG 1
#  else
#   define TERM_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define TERM_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined TERM_DEBUG */
#if TERM_DEBUG
extern int term_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "term_parser.y"

    #include "../../TreeAutomaton/TreeAutomaton.h"
    #include <iostream>
    #include <vector>

#line 63 "term_parser.hpp"

/* Token kinds.  */
#ifndef TERM_TOKENTYPE
# define TERM_TOKENTYPE
  enum term_tokentype
  {
    TERM_EMPTY = -2,
    TERM_EOF = 0,                  /* "end of file"  */
    TERM_error = 256,              /* error  */
    TERM_UNDEF = 257,              /* "invalid token"  */
    TR_NODE_NUM = 258,             /* TR_NODE_NUM  */
    TR_COMMENT = 259,              /* TR_COMMENT  */
    TR_NEWLINE = 260,              /* TR_NEWLINE  */
    TR_NODE_SYMBOL = 261,          /* TR_NODE_SYMBOL  */
    TR_SEPARATOR = 262             /* TR_SEPARATOR  */
  };
  typedef enum term_tokentype term_token_kind_t;
#endif

/* Value type.  */
#if ! defined TERM_STYPE && ! defined TERM_STYPE_IS_DECLARED
union TERM_STYPE
{
#line 24 "term_parser.y"

     unsigned number;
     char* string;


#line 93 "term_parser.hpp"

};
typedef union TERM_STYPE TERM_STYPE;
# define TERM_STYPE_IS_TRIVIAL 1
# define TERM_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined TERM_LTYPE && ! defined TERM_LTYPE_IS_DECLARED
typedef struct TERM_LTYPE TERM_LTYPE;
struct TERM_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define TERM_LTYPE_IS_DECLARED 1
# define TERM_LTYPE_IS_TRIVIAL 1
#endif


extern TERM_STYPE term_lval;
extern TERM_LTYPE term_lloc;

int term_parse (Term<std::string> &term, int &result);


#endif /* !YY_TERM_TERM_PARSER_HPP_INCLUDED  */
