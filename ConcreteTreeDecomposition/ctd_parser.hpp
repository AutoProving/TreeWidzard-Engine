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

#ifndef YY_CTD_CTD_PARSER_HPP_INCLUDED
#define YY_CTD_CTD_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef CTD_DEBUG
#if defined YYDEBUG
#if YYDEBUG
#define CTD_DEBUG 1
#else
#define CTD_DEBUG 0
#endif
#else /* ! defined YYDEBUG */
#define CTD_DEBUG 0
#endif /* ! defined YYDEBUG */
#endif /* ! defined CTD_DEBUG */
#if CTD_DEBUG
extern int ctd_debug;
#endif
/* "%code requires" blocks.  */
#line 3 "ctd_parser.y"

#include <iostream>
#include <memory>
#include <set>
#include <tuple>
#include <vector>
#include "../TreeAutomaton/ConcreteTreeDecomposition.h"

#line 66 "ctd_parser.hpp"

/* Token kinds.  */
#ifndef CTD_TOKENTYPE
#define CTD_TOKENTYPE
enum ctd_tokentype {
	// CTD_EMPTY = -2,
	CTD_EOF = 0,			/* "end of file"  */
	CTD_error = 256,		/* error  */
	CTD_UNDEF = 257,		/* "invalid token"  */
	CTD_NUM = 258,			/* CTD_NUM  */
	CTD_COMMENT = 259,		/* CTD_COMMENT  */
	CTD_NEWLINE = 260,		/* CTD_NEWLINE  */
	CTD_EMPTY = 261,		/* CTD_EMPTY  */
	CTD_INTROVERTEX = 262,	/* CTD_INTROVERTEX  */
	CTD_INTROEDGE = 263,	/* CTD_INTROEDGE  */
	CTD_FORGETVERTEX = 264, /* CTD_FORGETVERTEX  */
	CTD_JOIN = 265,			/* CTD_JOIN  */
	CTD_LEFTP = 266,		/* CTD_LEFTP  */
	CTD_RIGHTP = 267,		/* CTD_RIGHTP  */
	CTD_SEP1 = 268,			/* CTD_SEP1  */
	CTD_SEP2 = 269			/* CTD_SEP2  */
};
typedef enum ctd_tokentype ctd_token_kind_t;
#endif

/* Value type.  */
#if !defined CTD_STYPE && !defined CTD_STYPE_IS_DECLARED
union CTD_STYPE {
#line 36 "ctd_parser.y"

	unsigned number;
	char *string;

#line 102 "ctd_parser.hpp"
};
typedef union CTD_STYPE CTD_STYPE;
#define CTD_STYPE_IS_TRIVIAL 1
#define CTD_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if !defined CTD_LTYPE && !defined CTD_LTYPE_IS_DECLARED
typedef struct CTD_LTYPE CTD_LTYPE;
struct CTD_LTYPE {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
};
#define CTD_LTYPE_IS_DECLARED 1
#define CTD_LTYPE_IS_TRIVIAL 1
#endif

extern CTD_STYPE ctd_lval;
extern CTD_LTYPE ctd_lloc;

int ctd_parse(ConcreteTreeDecomposition &ctd, int &result);

#endif /* !YY_CTD_CTD_PARSER_HPP_INCLUDED  */
