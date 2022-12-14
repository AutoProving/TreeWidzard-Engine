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

#ifndef YY_TD_TD_PARSER_TAB_H_INCLUDED
# define YY_TD_TD_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef TD_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define TD_DEBUG 1
#  else
#   define TD_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define TD_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined TD_DEBUG */
#if TD_DEBUG
extern int td_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "td_parser.y"

    #include "../TreeDecompositionPACE.h"
    #include <iostream>
    #include <vector>
    #include <set>


#line 65 "td_parser.tab.h"

/* Token kinds.  */
#ifndef TD_TOKENTYPE
# define TD_TOKENTYPE
  enum td_tokentype
  {
    TD_EMPTY = -2,
    TD_EOF = 0,                    /* "end of file"  */
    TD_error = 256,                /* error  */
    TD_UNDEF = 257,                /* "invalid token"  */
    TD_S = 258,                    /* TD_S  */
    TD_TD = 259,                   /* TD_TD  */
    TD_B = 260,                    /* TD_B  */
    TD_NUM = 261,                  /* TD_NUM  */
    TD_COMMENT = 262,              /* TD_COMMENT  */
    TD_NEWLINE = 263               /* TD_NEWLINE  */
  };
  typedef enum td_tokentype td_token_kind_t;
#endif

/* Value type.  */
#if ! defined TD_STYPE && ! defined TD_STYPE_IS_DECLARED
union TD_STYPE
{
#line 26 "td_parser.y"

     int number;
     char* string;
     std::set<unsigned>* set_unsigned;

#line 96 "td_parser.tab.h"

};
typedef union TD_STYPE TD_STYPE;
# define TD_STYPE_IS_TRIVIAL 1
# define TD_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined TD_LTYPE && ! defined TD_LTYPE_IS_DECLARED
typedef struct TD_LTYPE TD_LTYPE;
struct TD_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define TD_LTYPE_IS_DECLARED 1
# define TD_LTYPE_IS_TRIVIAL 1
#endif


extern TD_STYPE td_lval;
extern TD_LTYPE td_lloc;

int td_parse (TreeDecompositionPACE &td, int &result);


#endif /* !YY_TD_TD_PARSER_TAB_H_INCLUDED  */
