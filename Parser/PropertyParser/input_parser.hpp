/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SEPERATOR = 258,
     STRING = 259,
     LEFTP = 260,
     RIGHTP = 261,
     NAME = 262,
     AND = 263,
     OR = 264,
     IFF = 265,
     IMPLIES = 266,
     NOT = 267,
     TRUE = 268,
     FALSE = 269,
     COMMENT = 270,
     NUMBER_DOUBLE = 271,
     COMMA = 272,
     FORMULA_NAME = 273,
     EXP = 274,
     ATLEAST = 275,
     ATMOST = 276,
     LESS = 277,
     BIGGER = 278,
     BINARY_ARITHMETIC_OPERATOR = 279,
     BINARY_FUNCTION = 280,
     UNARY_FUNCTION = 281,
     INV_ = 282,
     EQUAL = 283
   };
#endif
/* Tokens.  */
#define SEPERATOR 258
#define STRING 259
#define LEFTP 260
#define RIGHTP 261
#define NAME 262
#define AND 263
#define OR 264
#define IFF 265
#define IMPLIES 266
#define NOT 267
#define TRUE 268
#define FALSE 269
#define COMMENT 270
#define NUMBER_DOUBLE 271
#define COMMA 272
#define FORMULA_NAME 273
#define EXP 274
#define ATLEAST 275
#define ATMOST 276
#define LESS 277
#define BIGGER 278
#define BINARY_ARITHMETIC_OPERATOR 279
#define BINARY_FUNCTION 280
#define UNARY_FUNCTION 281
#define INV_ 282
#define EQUAL 283




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "Parser/PropertyParser/input_parser.y"
{
     ConjectureNode *conjectureNode;
     int number;
     char* string;
     std::vector<std::variant<char*,int, bool>> *vec;
     PropertyAssignment *property;
}
/* Line 1529 of yacc.c.  */
#line 113 "Parser/PropertyParser/input_parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE input_lval;

