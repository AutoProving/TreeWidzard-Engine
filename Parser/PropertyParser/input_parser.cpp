/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         INPUT_STYPE
#define YYLTYPE         INPUT_LTYPE
/* Substitute the variable and function names.  */
#define yyparse         input_parse
#define yylex           input_lex
#define yyerror         input_error
#define yydebug         input_debug
#define yynerrs         input_nerrs
#define yylval          input_lval
#define yychar          input_char
#define yylloc          input_lloc

/* First part of user prologue.  */
#line 15 "input_parser.y"

    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <map>
    #include <string.h>
    #include <utility>
    #include <variant>
    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include <algorithm>
    // this function will be generated
    // using flex
    extern int yylex();
    extern int input_lineno;
    extern void yyerror(Conjecture &conj, int &result,std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> varToProperty, char const* msg);
    std::map<char*,ConjectureNode*> sub_formula_variables;
    bool check_varToProperty(std::string v,std::map<std::string, PropertyAssignment*> &varToProperty);
    bool check_sub_formula_variables(char* v);

#line 103 "input_parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "input_parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEPERATOR = 3,                  /* SEPERATOR  */
  YYSYMBOL_FILEPATH = 4,                   /* FILEPATH  */
  YYSYMBOL_LEFTP = 5,                      /* LEFTP  */
  YYSYMBOL_RIGHTP = 6,                     /* RIGHTP  */
  YYSYMBOL_NAME = 7,                       /* NAME  */
  YYSYMBOL_NEWLINE = 8,                    /* NEWLINE  */
  YYSYMBOL_AND = 9,                        /* AND  */
  YYSYMBOL_OR = 10,                        /* OR  */
  YYSYMBOL_IFF = 11,                       /* IFF  */
  YYSYMBOL_IMPLIES = 12,                   /* IMPLIES  */
  YYSYMBOL_NOT = 13,                       /* NOT  */
  YYSYMBOL_TRUE = 14,                      /* TRUE  */
  YYSYMBOL_FALSE = 15,                     /* FALSE  */
  YYSYMBOL_COMMENT = 16,                   /* COMMENT  */
  YYSYMBOL_NUMBER_DOUBLE = 17,             /* NUMBER_DOUBLE  */
  YYSYMBOL_COMMA = 18,                     /* COMMA  */
  YYSYMBOL_FORMULA_NAME = 19,              /* FORMULA_NAME  */
  YYSYMBOL_EXP = 20,                       /* EXP  */
  YYSYMBOL_ATLEAST = 21,                   /* ATLEAST  */
  YYSYMBOL_ATMOST = 22,                    /* ATMOST  */
  YYSYMBOL_LESS = 23,                      /* LESS  */
  YYSYMBOL_BIGGER = 24,                    /* BIGGER  */
  YYSYMBOL_BINARY_ARITHMETIC_OPERATOR = 25, /* BINARY_ARITHMETIC_OPERATOR  */
  YYSYMBOL_BINARY_FUNCTION = 26,           /* BINARY_FUNCTION  */
  YYSYMBOL_UNARY_FUNCTION = 27,            /* UNARY_FUNCTION  */
  YYSYMBOL_INV_ = 28,                      /* INV_  */
  YYSYMBOL_EQUAL = 29,                     /* EQUAL  */
  YYSYMBOL_YYACCEPT = 30,                  /* $accept  */
  YYSYMBOL_START = 31,                     /* START  */
  YYSYMBOL_VARIABLES_CORES_ASSIGNMENT = 32, /* VARIABLES_CORES_ASSIGNMENT  */
  YYSYMBOL_VARIABLE_CORE_ASSIGNMENT = 33,  /* VARIABLE_CORE_ASSIGNMENT  */
  YYSYMBOL_ATOMIC_PREDICATE = 34,          /* ATOMIC_PREDICATE  */
  YYSYMBOL_PARAMETERS = 35,                /* PARAMETERS  */
  YYSYMBOL_PARAMETER = 36,                 /* PARAMETER  */
  YYSYMBOL_VARIABLES_SUBFORMULA_ASSIGNMENTS = 37, /* VARIABLES_SUBFORMULA_ASSIGNMENTS  */
  YYSYMBOL_SUB_FORMULA = 38,               /* SUB_FORMULA  */
  YYSYMBOL_FORMULA = 39,                   /* FORMULA  */
  YYSYMBOL_FORMULA_TERMINAL = 40,          /* FORMULA_TERMINAL  */
  YYSYMBOL_VARIABLE = 41,                  /* VARIABLE  */
  YYSYMBOL_COMMENTS = 42,                  /* COMMENTS  */
  YYSYMBOL_FORMULACOMMENTS = 43            /* FORMULACOMMENTS  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined INPUT_LTYPE_IS_TRIVIAL && INPUT_LTYPE_IS_TRIVIAL \
             && defined INPUT_STYPE_IS_TRIVIAL && INPUT_STYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   237

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  30
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  94

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   284


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29
};

#if INPUT_DEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    82,    82,    85,    86,    88,   112,   113,   116,   120,
     124,   128,   132,   133,   138,   146,   149,   155,   161,   163,
     172,   174,   176,   178,   183,   188,   193,   198,   203,   208,
     213,   218,   223,   227,   232,   237,   242,   248,   249,   251,
     252,   253,   261,   269,   271,   272,   274,   275
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if INPUT_DEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEPERATOR",
  "FILEPATH", "LEFTP", "RIGHTP", "NAME", "NEWLINE", "AND", "OR", "IFF",
  "IMPLIES", "NOT", "TRUE", "FALSE", "COMMENT", "NUMBER_DOUBLE", "COMMA",
  "FORMULA_NAME", "EXP", "ATLEAST", "ATMOST", "LESS", "BIGGER",
  "BINARY_ARITHMETIC_OPERATOR", "BINARY_FUNCTION", "UNARY_FUNCTION",
  "INV_", "EQUAL", "$accept", "START", "VARIABLES_CORES_ASSIGNMENT",
  "VARIABLE_CORE_ASSIGNMENT", "ATOMIC_PREDICATE", "PARAMETERS",
  "PARAMETER", "VARIABLES_SUBFORMULA_ASSIGNMENTS", "SUB_FORMULA",
  "FORMULA", "FORMULA_TERMINAL", "VARIABLE", "COMMENTS", "FORMULACOMMENTS", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-40)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -14,   -14,    21,   -40,   -40,   -40,    11,   -40,    -5,    -1,
      22,    24,   -14,    46,    47,   112,   -40,   -40,    50,    53,
     112,   112,   -40,   -40,   -40,    59,    66,    67,   122,   -40,
     -40,     2,   200,    36,   -40,   112,   112,   112,   -14,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   -40,
     -40,   -40,   -40,     4,   -40,   200,   200,   -40,   -40,   -40,
      43,   -40,   -40,   143,    57,    78,    54,   197,   179,   161,
     170,    48,   -24,   203,   208,   -40,    49,   -40,     7,    14,
     -40,    24,   200,   112,   -40,   -40,   -40,   -40,   -40,   -40,
     -40,   -40,    99,   -40
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      45,    45,     0,     4,    44,     1,     0,    43,     0,     0,
       0,    15,    45,     0,     0,     0,     3,     6,     0,     0,
       0,     0,    39,    40,    42,     0,     0,     0,    47,    38,
      41,     7,     0,     0,    32,     0,     0,     0,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
      13,    12,     9,     0,     8,     0,     0,    20,    21,    22,
       0,    19,    37,     0,     0,     0,    47,    23,    24,    26,
      25,    27,    28,    29,    30,    33,    31,     5,     0,     0,
      17,    15,     0,     0,    35,    36,    46,    11,    10,    18,
      14,    16,     0,    34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -40,   -40,   -40,   -40,   -40,   -40,     5,    10,   -39,    -8,
     -40,    -6,     3,    19
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     6,     9,    18,    53,    54,    15,    60,    28,
      29,    30,     3,    49
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      10,    47,     1,    11,     4,    48,    50,    12,    19,    51,
      77,    50,    33,    34,    51,    16,    79,    80,     7,    52,
      89,     5,    78,    82,    87,    13,    61,    63,    64,    65,
       8,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    66,    62,    91,    14,    39,    40,    41,    42,    61,
      61,    81,    82,    17,     7,    31,    32,    43,    44,    45,
      46,    47,    38,    84,    35,    48,    39,    40,    41,    42,
      44,    36,    37,    47,    47,    92,    61,    48,    43,    44,
      45,    46,    47,    88,    85,    86,    48,    39,    40,    41,
      42,    90,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,     0,    93,     0,    48,    39,    40,
      41,    42,     0,     0,     0,     0,     0,    20,     0,     7,
      43,    44,    45,    46,    47,    21,    22,    23,    48,    24,
      38,    39,    40,    41,    42,     0,     0,     0,    25,    26,
      27,     0,     0,    43,    44,    45,    46,    47,     0,     0,
       0,    48,    39,    40,    41,    42,     0,     0,     0,     0,
       0,    83,     0,     0,    43,    44,    45,    46,    47,     0,
      39,    40,    48,    42,     0,     0,     0,     0,     0,    39,
      40,     0,    43,    44,    45,    46,    47,     0,    39,     0,
      48,    43,    44,    45,    46,    47,     0,     0,     0,    48,
      43,    44,    45,    46,    47,    55,     0,     7,    48,     0,
       0,     0,     0,    56,    57,    58,     0,    59,    43,    44,
      45,    46,    47,     0,    43,    44,    48,    46,    47,    43,
      44,     0,    48,    47,     0,     0,     0,    48
};

static const yytype_int8 yycheck[] =
{
       6,    25,    16,     8,     1,    29,     4,     8,    14,     7,
       6,     4,    20,    21,     7,    12,    55,    56,     7,    17,
       6,     0,    18,     9,    17,     3,    32,    35,    36,    37,
      19,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    38,     6,    82,    20,     9,    10,    11,    12,    55,
      56,     8,     9,     7,     7,     5,     3,    21,    22,    23,
      24,    25,     8,     6,     5,    29,     9,    10,    11,    12,
      22,     5,     5,    25,    25,    83,    82,    29,    21,    22,
      23,    24,    25,    78,     6,    66,    29,     9,    10,    11,
      12,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,
      22,    23,    24,    25,    -1,     6,    -1,    29,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,     5,    -1,     7,
      21,    22,    23,    24,    25,    13,    14,    15,    29,    17,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    26,    27,
      28,    -1,    -1,    21,    22,    23,    24,    25,    -1,    -1,
      -1,    29,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    21,    22,    23,    24,    25,    -1,
       9,    10,    29,    12,    -1,    -1,    -1,    -1,    -1,     9,
      10,    -1,    21,    22,    23,    24,    25,    -1,     9,    -1,
      29,    21,    22,    23,    24,    25,    -1,    -1,    -1,    29,
      21,    22,    23,    24,    25,     5,    -1,     7,    29,    -1,
      -1,    -1,    -1,    13,    14,    15,    -1,    17,    21,    22,
      23,    24,    25,    -1,    21,    22,    29,    24,    25,    21,
      22,    -1,    29,    25,    -1,    -1,    -1,    29
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    16,    31,    42,    42,     0,    32,     7,    19,    33,
      41,     8,     8,     3,    20,    37,    42,     7,    34,    41,
       5,    13,    14,    15,    17,    26,    27,    28,    39,    40,
      41,     5,     3,    39,    39,     5,     5,     5,     8,     9,
      10,    11,    12,    21,    22,    23,    24,    25,    29,    43,
       4,     7,    17,    35,    36,     5,    13,    14,    15,    17,
      38,    41,     6,    39,    39,    39,    42,    39,    39,    39,
      39,    39,    39,    39,    39,    39,    39,     6,    18,    38,
      38,     8,     9,    18,     6,     6,    43,    17,    36,     6,
      37,    38,    39,     6
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    30,    31,    32,    32,    33,    34,    35,    35,    35,
      35,    35,    36,    36,    37,    37,    38,    38,    38,    38,
      38,    38,    38,    39,    39,    39,    39,    39,    39,    39,
      39,    39,    39,    39,    39,    39,    39,    39,    39,    40,
      40,    40,    40,    41,    42,    42,    43,    43
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     7,     4,     0,     6,     1,     0,     1,     1,
       3,     3,     1,     1,     6,     0,     3,     2,     3,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     6,     4,     4,     3,     1,     1,
       1,     1,     1,     1,     2,     0,     3,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = INPUT_EMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == INPUT_EMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (conj, result, coreList, varToCoreName, varToProperty, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use INPUT_error or INPUT_UNDEF. */
#define YYERRCODE INPUT_UNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if INPUT_DEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined INPUT_LTYPE_IS_TRIVIAL && INPUT_LTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, conj, result, coreList, varToCoreName, varToProperty); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Conjecture &conj, int &result, std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> &varToProperty)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (conj);
  YY_USE (result);
  YY_USE (coreList);
  YY_USE (varToCoreName);
  YY_USE (varToProperty);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Conjecture &conj, int &result, std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> &varToProperty)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, conj, result, coreList, varToCoreName, varToProperty);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, Conjecture &conj, int &result, std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> &varToProperty)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), conj, result, coreList, varToCoreName, varToProperty);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, conj, result, coreList, varToCoreName, varToProperty); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !INPUT_DEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !INPUT_DEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Conjecture &conj, int &result, std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> &varToProperty)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (conj);
  YY_USE (result);
  YY_USE (coreList);
  YY_USE (varToCoreName);
  YY_USE (varToProperty);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined INPUT_LTYPE_IS_TRIVIAL && INPUT_LTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (Conjecture &conj, int &result, std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> &varToProperty)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = INPUT_EMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == INPUT_EMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= INPUT_EOF)
    {
      yychar = INPUT_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == INPUT_error)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = INPUT_UNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = INPUT_EMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* START: COMMENTS VARIABLES_CORES_ASSIGNMENT FORMULA_NAME NEWLINE VARIABLES_SUBFORMULA_ASSIGNMENTS FORMULA FORMULACOMMENTS  */
#line 83 "input_parser.y"
                    {conj.setRoot((yyvsp[-1].conjectureNode)); result = 0;}
#line 1342 "input_parser.cpp"
    break;

  case 3: /* VARIABLES_CORES_ASSIGNMENT: VARIABLES_CORES_ASSIGNMENT VARIABLE_CORE_ASSIGNMENT NEWLINE COMMENTS  */
#line 85 "input_parser.y"
                                                                                                       {}
#line 1348 "input_parser.cpp"
    break;

  case 5: /* VARIABLE_CORE_ASSIGNMENT: VARIABLE SEPERATOR ATOMIC_PREDICATE LEFTP PARAMETERS RIGHTP  */
#line 89 "input_parser.y"
                                {
				if(check_varToProperty((yyvsp[-5].string),varToProperty)){ std::cout<<" variable " << (yyvsp[-5].string) << " is written at least two times" <<std::endl; YYERROR; exit(20);}
				if(!coreList.count((yyvsp[-3].string))){std::cout<<"core name \" "<<(yyvsp[-3].string)<<"\"is not exist"<<std::endl; YYERROR;}
				// if(!coreList[$3].count("CoreType")){std::cout<<"CoreType of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
				// if(coreList[$3]["CoreType"]!="Bool"){std::cout<<"CoreType of "<<$3<< " isn't Bool, check the core properties"<<std::endl; YYERROR;}
				// if(!coreList[$3].count("ParameterType")){std::cout<<"Parameter of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
				// if(coreList[$3]["ParameterType"]!="UnsignedInt"){std::cout<<"Parameter of "<<$3<< " isn't UnsignedInt, check the core properties"<<std::endl; YYERROR;}
				// if(!coreList[$3].count("PrimaryOperator")){std::cout<<"PrimaryOperator for "<<$3<<" isn't set, check the core properties"<<std::endl;YYERROR;}
				// if(coreList[$3]["PrimaryOperator"]=="AtLeast" and (strcmp($4,"<")==0 or strcmp($4,"<=")==0)   ){
				// std::cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<std::endl;YYERROR;}
				// if( coreList[$3]["PrimaryOperator"]=="AtMost" and (strcmp($4,">")==0 or strcmp($4,">=")==0)  )
				// {std::cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<std::endl;YYERROR;}
				(yyval.property) = new PropertyAssignment(); //$$->setParameterType("UnsignedInt");
				(yyval.property)->setName((yyvsp[-3].string));
				// $$->setOp($4);
				// $$->setParameter($5);
				varToProperty[(yyvsp[-5].string)] = (yyval.property); varToCoreName[(yyvsp[-5].string)] = (yyvsp[-3].string);
				(yyval.property)->setParametersVec(*(yyvsp[-1].vec));
				}
#line 1372 "input_parser.cpp"
    break;

  case 7: /* PARAMETERS: %empty  */
#line 113 "input_parser.y"
                         {
		(yyval.vec) = new std::vector<std::variant<char*,int, bool>>();
        }
#line 1380 "input_parser.cpp"
    break;

  case 8: /* PARAMETERS: PARAMETER  */
#line 116 "input_parser.y"
                    {
		(yyval.vec) = new std::vector<std::variant<char*,int, bool>>();
		(yyval.vec)->emplace_back((yyvsp[0].string));
		}
#line 1389 "input_parser.cpp"
    break;

  case 9: /* PARAMETERS: NUMBER_DOUBLE  */
#line 120 "input_parser.y"
                                {
                 (yyval.vec) = new std::vector<std::variant<char*,int, bool>>();
      		  (yyval.vec)->emplace_back((yyvsp[0].number));
                 }
#line 1398 "input_parser.cpp"
    break;

  case 10: /* PARAMETERS: PARAMETERS COMMA PARAMETER  */
#line 124 "input_parser.y"
                                             {
		(yyval.vec) = (yyvsp[-2].vec);
                (yyval.vec)->emplace_back((yyvsp[0].string));
		}
#line 1407 "input_parser.cpp"
    break;

  case 11: /* PARAMETERS: PARAMETERS COMMA NUMBER_DOUBLE  */
#line 128 "input_parser.y"
                                                 {
                (yyval.vec) = (yyvsp[-2].vec);
                 (yyval.vec)->emplace_back((yyvsp[0].number));
                }
#line 1416 "input_parser.cpp"
    break;

  case 12: /* PARAMETER: NAME  */
#line 132 "input_parser.y"
                       {(yyval.string) = (yyvsp[0].string);}
#line 1422 "input_parser.cpp"
    break;

  case 13: /* PARAMETER: FILEPATH  */
#line 133 "input_parser.y"
                           {(yyval.string) = (yyvsp[0].string);}
#line 1428 "input_parser.cpp"
    break;

  case 14: /* VARIABLES_SUBFORMULA_ASSIGNMENTS: EXP VARIABLE SEPERATOR SUB_FORMULA NEWLINE VARIABLES_SUBFORMULA_ASSIGNMENTS  */
#line 139 "input_parser.y"
                                                                {if(check_varToProperty((yyvsp[-4].string),varToProperty)){
                                                                    std::cout<< "variable " << (yyvsp[-4].string) << " declared at least two times" <<std::endl; YYERROR;}
                                                                  if(check_sub_formula_variables((yyvsp[-4].string))){
                                                                    std::cout<< "variable " << (yyvsp[-4].string) << " declared at least two times" <<std::endl; YYERROR;
                                                                  }
                                                                  sub_formula_variables.insert(std::make_pair((yyvsp[-4].string), (yyvsp[-2].conjectureNode)));
                                                                  }
#line 1440 "input_parser.cpp"
    break;

  case 16: /* SUB_FORMULA: SUB_FORMULA AND SUB_FORMULA  */
#line 149 "input_parser.y"
                                          {(yyval.conjectureNode)= new ConjectureNode(OPERATOR,"and");
                std::vector<ConjectureNode*> children; children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));
                (yyval.conjectureNode)->setChildren(children);
                (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
                }
#line 1450 "input_parser.cpp"
    break;

  case 17: /* SUB_FORMULA: NOT SUB_FORMULA  */
#line 155 "input_parser.y"
                               {(yyval.conjectureNode)= new ConjectureNode(OPERATOR,"not");
                std::vector<ConjectureNode*> children; children.push_back((yyvsp[0].conjectureNode));
                (yyval.conjectureNode)->setChildren(children);
                (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
                }
#line 1460 "input_parser.cpp"
    break;

  case 18: /* SUB_FORMULA: LEFTP SUB_FORMULA RIGHTP  */
#line 161 "input_parser.y"
                                       {(yyval.conjectureNode) = (yyvsp[-1].conjectureNode);}
#line 1466 "input_parser.cpp"
    break;

  case 19: /* SUB_FORMULA: VARIABLE  */
#line 163 "input_parser.y"
                       {if(check_varToProperty((yyvsp[0].string),varToProperty) and !check_sub_formula_variables((yyvsp[0].string)) ){
                    (yyval.conjectureNode) = new ConjectureNode(CORE_VARIABLE, (yyvsp[0].string));
                }else if(check_sub_formula_variables((yyvsp[0].string)) and !check_varToProperty((yyvsp[0].string),varToProperty) ) {
                    (yyval.conjectureNode) = sub_formula_variables[(yyvsp[0].string)];
                }else{
                    std::cout<<" variable "<< (yyvsp[0].string) << " is not valid"<< std::endl; YYERROR;
                }
            }
#line 1479 "input_parser.cpp"
    break;

  case 20: /* SUB_FORMULA: TRUE  */
#line 172 "input_parser.y"
                   {(yyval.conjectureNode) = new ConjectureNode(NUMBER,1);}
#line 1485 "input_parser.cpp"
    break;

  case 21: /* SUB_FORMULA: FALSE  */
#line 174 "input_parser.y"
                    {(yyval.conjectureNode) = new ConjectureNode(NUMBER,0);}
#line 1491 "input_parser.cpp"
    break;

  case 22: /* SUB_FORMULA: NUMBER_DOUBLE  */
#line 176 "input_parser.y"
                            {(yyval.conjectureNode)= new ConjectureNode(NUMBER,(yyvsp[0].number));}
#line 1497 "input_parser.cpp"
    break;

  case 23: /* FORMULA: FORMULA AND FORMULA  */
#line 178 "input_parser.y"
                                  {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"and");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1507 "input_parser.cpp"
    break;

  case 24: /* FORMULA: FORMULA OR FORMULA  */
#line 183 "input_parser.y"
                                 {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"or");
                    std::vector<ConjectureNode*> children;
                    children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
                    (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1517 "input_parser.cpp"
    break;

  case 25: /* FORMULA: FORMULA IMPLIES FORMULA  */
#line 188 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"implies");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1527 "input_parser.cpp"
    break;

  case 26: /* FORMULA: FORMULA IFF FORMULA  */
#line 193 "input_parser.y"
                                  {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"iff");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1537 "input_parser.cpp"
    break;

  case 27: /* FORMULA: FORMULA ATLEAST FORMULA  */
#line 198 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,">=");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1547 "input_parser.cpp"
    break;

  case 28: /* FORMULA: FORMULA ATMOST FORMULA  */
#line 203 "input_parser.y"
                                     {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"<=");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1557 "input_parser.cpp"
    break;

  case 29: /* FORMULA: FORMULA LESS FORMULA  */
#line 208 "input_parser.y"
                                   {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"<");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1567 "input_parser.cpp"
    break;

  case 30: /* FORMULA: FORMULA BIGGER FORMULA  */
#line 213 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,">");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1577 "input_parser.cpp"
    break;

  case 31: /* FORMULA: FORMULA EQUAL FORMULA  */
#line 218 "input_parser.y"
                                 {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"==");
                    std::vector<ConjectureNode*> children;
                    children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
                    (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
                    }
#line 1587 "input_parser.cpp"
    break;

  case 32: /* FORMULA: NOT FORMULA  */
#line 223 "input_parser.y"
                           {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"not");
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1596 "input_parser.cpp"
    break;

  case 33: /* FORMULA: FORMULA BINARY_ARITHMETIC_OPERATOR FORMULA  */
#line 227 "input_parser.y"
                                                          {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,(yyvsp[-1].string));
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1606 "input_parser.cpp"
    break;

  case 34: /* FORMULA: BINARY_FUNCTION LEFTP FORMULA COMMA FORMULA RIGHTP  */
#line 232 "input_parser.y"
                                                                 {(yyval.conjectureNode) = new ConjectureNode(FUNCTION_BINARY,(yyvsp[-5].string));
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-3].conjectureNode)); children.push_back((yyvsp[-1].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-3].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[-1].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1616 "input_parser.cpp"
    break;

  case 35: /* FORMULA: UNARY_FUNCTION LEFTP FORMULA RIGHTP  */
#line 237 "input_parser.y"
                                                  {(yyval.conjectureNode) = new ConjectureNode(FUNCTION_UNARY,(yyvsp[-3].string));
            std::vector<ConjectureNode*> children;
            children.push_back((yyvsp[-1].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-1].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1626 "input_parser.cpp"
    break;

  case 36: /* FORMULA: INV_ LEFTP FORMULA RIGHTP  */
#line 242 "input_parser.y"
                                        {(yyval.conjectureNode) = new ConjectureNode(INV,(yyvsp[-3].string));
            std::vector<ConjectureNode*> children;
            if((yyvsp[-1].conjectureNode)->getType() != CORE_VARIABLE){yyerror(conj, result, coreList, varToCoreName, varToProperty, "INV should be in a form INV(variable)" ); YYERROR;}
            children.push_back((yyvsp[-1].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-1].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1637 "input_parser.cpp"
    break;

  case 37: /* FORMULA: LEFTP FORMULA RIGHTP  */
#line 248 "input_parser.y"
                                        {(yyval.conjectureNode) = (yyvsp[-1].conjectureNode);}
#line 1643 "input_parser.cpp"
    break;

  case 39: /* FORMULA_TERMINAL: TRUE  */
#line 251 "input_parser.y"
                                {(yyval.conjectureNode) = new ConjectureNode(NUMBER,1); }
#line 1649 "input_parser.cpp"
    break;

  case 40: /* FORMULA_TERMINAL: FALSE  */
#line 252 "input_parser.y"
                                {(yyval.conjectureNode) = new ConjectureNode(NUMBER,0); }
#line 1655 "input_parser.cpp"
    break;

  case 41: /* FORMULA_TERMINAL: VARIABLE  */
#line 253 "input_parser.y"
                                { if(check_varToProperty((yyvsp[0].string),varToProperty) and !check_sub_formula_variables((yyvsp[0].string)) ){
                                   (yyval.conjectureNode) = new ConjectureNode(CORE_VARIABLE, (yyvsp[0].string));
                                  }else if(check_sub_formula_variables((yyvsp[0].string)) and !check_varToProperty((yyvsp[0].string),varToProperty) ) {
                                    (yyval.conjectureNode) = sub_formula_variables[(yyvsp[0].string)];
                                  }else{
                                    std::cout<<" variable "<< (yyvsp[0].string) << " is not valid"<< std::endl; YYERROR;
                                  }
                                }
#line 1668 "input_parser.cpp"
    break;

  case 42: /* FORMULA_TERMINAL: NUMBER_DOUBLE  */
#line 261 "input_parser.y"
                                 {(yyval.conjectureNode)= new ConjectureNode(NUMBER,(yyvsp[0].number));}
#line 1674 "input_parser.cpp"
    break;

  case 44: /* COMMENTS: COMMENT COMMENTS  */
#line 271 "input_parser.y"
                                                                   {}
#line 1680 "input_parser.cpp"
    break;


#line 1684 "input_parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == INPUT_EMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (conj, result, coreList, varToCoreName, varToProperty, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= INPUT_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == INPUT_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, conj, result, coreList, varToCoreName, varToProperty);
          yychar = INPUT_EMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, conj, result, coreList, varToCoreName, varToProperty);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (conj, result, coreList, varToCoreName, varToProperty, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != INPUT_EMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, conj, result, coreList, varToCoreName, varToProperty);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, conj, result, coreList, varToCoreName, varToProperty);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 277 "input_parser.y"


void yyerror(Conjecture &/*conj*/, int &/*result*/,std::map<std::string,std::map<std::string,std::string>> &/*coreList*/, std::map<std::string,std::string> &/*varToCoreName*/, std::map<std::string, PropertyAssignment*> /*varToProperty*/, char const* msg){

  std::cerr<< "\033[1;31mERORR:\033[0m" << std::endl;
  std::cerr<<"\033[1;31mError in the input file line " <<input_lineno << "\033[0m" << std::endl;
  std::cerr << "\033[1;31m"<<msg <<"\033[0m"<< std::endl;

}

bool check_varToProperty(std::string v,std::map<std::string, PropertyAssignment*> &varToProperty ){
    if(varToProperty.count(v)){
        return true;
    }else{
        return false;
    }
}

bool check_sub_formula_variables(char* v){
    if(sub_formula_variables.count(v)){
        return true;
    }else{
        return false;
    }
}
