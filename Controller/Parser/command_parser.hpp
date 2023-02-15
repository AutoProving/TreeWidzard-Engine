/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Skeleton interface for Bison GLR parsers in C

   Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

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

#ifndef YY_COMMAND_COMMAND_PARSER_HPP_INCLUDED
# define YY_COMMAND_COMMAND_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef COMMAND_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define COMMAND_DEBUG 1
#  else
#   define COMMAND_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define COMMAND_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined COMMAND_DEBUG */
#if COMMAND_DEBUG
extern int command_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "command_parser.y"

    #include <iostream>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>

#line 60 "command_parser.hpp"

/* Token kinds.  */
#ifndef COMMAND_TOKENTYPE
# define COMMAND_TOKENTYPE
  enum command_tokentype
  {
    COMMAND_EMPTY = -2,
    COMMAND_EOF = 0,               /* "end of file"  */
    COMMAND_error = 256,           /* error  */
    COMMAND_UNDEF = 257,           /* "invalid token"  */
    command_newline = 258,         /* command_newline  */
    command_search_signature = 259, /* command_search_signature  */
    command_print_state_flag = 260, /* command_print_state_flag  */
    command_print_loop_flag = 261, /* command_print_loop_flag  */
    command_string = 262,          /* command_string  */
    command_help = 263,            /* command_help  */
    command_end = 264,             /* command_end  */
    command_parse_signature = 265, /* command_parse_signature  */
    command_parse_pace = 266,      /* command_parse_pace  */
    command_parse_abstract = 267,  /* command_parse_abstract  */
    command_term_signature = 268,  /* command_term_signature  */
    command_print_state_tree = 269, /* command_print_state_tree  */
    command_random_signature = 270, /* command_random_signature  */
    command_number = 271,          /* command_number  */
    command_premise = 272,         /* command_premise  */
    command_no_bfs_dag = 273,      /* command_no_bfs_dag  */
    command_nthreads = 274,        /* command_nthreads  */
    command_pw = 275,              /* command_pw  */
    command_tw = 276,              /* command_tw  */
    command_equal = 277,           /* command_equal  */
    command_print_directed_bipartite_graph = 278, /* command_print_directed_bipartite_graph  */
    command_write_files = 279      /* command_write_files  */
  };
  typedef enum command_tokentype command_token_kind_t;
#endif

/* Value type.  */
#if ! defined COMMAND_STYPE && ! defined COMMAND_STYPE_IS_DECLARED
union COMMAND_STYPE
{
#line 32 "command_parser.y"

     float number;
     char* string;

#line 106 "command_parser.hpp"

};
typedef union COMMAND_STYPE COMMAND_STYPE;
# define COMMAND_STYPE_IS_TRIVIAL 1
# define COMMAND_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined COMMAND_LTYPE && ! defined COMMAND_LTYPE_IS_DECLARED
typedef struct COMMAND_LTYPE COMMAND_LTYPE;
struct COMMAND_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define COMMAND_LTYPE_IS_DECLARED 1
# define COMMAND_LTYPE_IS_TRIVIAL 1
#endif


extern COMMAND_STYPE command_lval;
extern COMMAND_LTYPE command_lloc;
int command_parse (int &result, std::string &width_type, int &width_value);

#endif /* !YY_COMMAND_COMMAND_PARSER_HPP_INCLUDED  */
