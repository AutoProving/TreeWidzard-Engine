/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

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
#line 14 "input_parser.y"

    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <map>
    #include <string.h>
    #include <utility>
    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include <algorithm>
   
    // this function will be generated
    // using flex
    extern int yylex();
    extern int input_lineno;
    extern void yyerror(Conjecture &conj, int &result,map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> varToProperty, char const* msg);
    map<char*,ConjectureNode*> sub_formula_variables;
    
    bool check_varToProperty(string v,map<string, PropertyAssignment*> &varToProperty);
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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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


#line 164 "input_parser.cpp"

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
    BIGGER = 279,
    BINARY_ARITHMETIC_OPERATOR = 280,
    BINARY_FUNCTION = 281,
    UNARY_FUNCTION = 282
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

#line 211 "input_parser.cpp"

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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


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
#define YYLAST   163

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  47
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  92

#define YYUNDEFTOK  2
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

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
      25,    26,    27
};

#if INPUT_DEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    78,    78,    81,    82,    84,   100,   101,   112,   114,
     115,   116,   118,   119,   120,   121,   122,   130,   133,   139,
     145,   148,   157,   159,   161,   163,   168,   173,   178,   183,
     188,   193,   198,   204,   208,   213,   218,   223,   224,   226,
     227,   228,   236,   244,   246,   247,   249,   250
};
#endif

#if INPUT_DEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEPERATOR", "FILEPATH", "LEFTP",
  "RIGHTP", "NAME", "NEWLINE", "AND", "OR", "IFF", "IMPLIES", "NOT",
  "TRUE", "FALSE", "COMMENT", "NUMBER_DOUBLE", "COMMA", "FORMULA_NAME",
  "EXP", "ATLEAST", "ATMOST", "LESS", "BIGGER",
  "BINARY_ARITHMETIC_OPERATOR", "BINARY_FUNCTION", "UNARY_FUNCTION",
  "$accept", "START", "VARIABLES_CORES_ASSIGNMENT",
  "VARIABLE_CORE_ASSIGNMENT", "NUMBER_DOUBLES", "ATOMIC_PREDICATE",
  "COMPARATOR", "VARIABLES_SUBFORMULA_ASSIGNMENTS", "SUB_FORMULA",
  "FORMULA", "FORMULA_TERMINAL", "VARIABLE", "COMMENTS", "FORMULACOMMENTS", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282
};
# endif

#define YYPACT_NINF (-43)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -10,   -10,    13,   -43,   -43,   -43,    -2,   -43,    14,    41,
      68,    65,   -10,    66,    79,    39,   -43,   -43,    -3,    89,
      39,    39,   -43,   -43,   -43,    88,    93,   103,   -43,   -43,
      -1,   -43,   -43,   -43,   -43,    87,   141,    58,   -43,    39,
      39,   -10,    39,    39,    39,    39,    39,    39,    39,    39,
      39,   -43,    99,   -43,    37,   -43,   141,   141,   -43,   -43,
     -43,    49,   -43,   -43,   111,    78,   108,   120,     2,   128,
      38,   -18,    92,    53,   138,   -43,   -43,   -43,   101,    36,
     -43,    65,   141,    39,   -43,   -43,   -43,   -43,   -43,   -43,
      85,   -43
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      45,    45,     0,     4,    44,     1,     0,    43,     0,     0,
       0,    17,    45,     0,     0,     0,     3,    11,     7,     0,
       0,     0,    39,    40,    42,     0,     0,    47,    38,    41,
       0,    13,    14,    15,    12,     0,     0,     0,    33,     0,
       0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     2,     0,    10,     0,     5,     0,     0,    22,    23,
      24,     0,    21,    37,     0,     0,    47,    25,    26,    28,
      27,    29,    30,    31,    32,    34,     6,     8,     0,     0,
      19,    17,     0,     0,    36,    46,     9,    20,    16,    18,
       0,    35
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -43,   -43,   -43,   -43,   -43,   -43,   -43,    50,   -42,   -11,
     -43,    -6,     0,    64
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     6,     9,    54,    18,    35,    15,    61,    27,
      28,    29,     3,    51
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      10,     4,    30,    52,    47,     7,     1,    50,    19,    37,
      38,    42,    16,     5,    79,    80,    53,     8,    31,    32,
      33,    34,    11,    46,    47,    48,    49,    50,    64,    65,
      62,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      89,    66,    87,    77,    20,    82,     7,    42,    43,    12,
      62,    62,    21,    22,    23,    78,    24,    81,    82,    46,
      47,    48,    49,    50,    63,    25,    26,    42,    43,    44,
      45,    13,    90,    17,    46,    47,    62,    49,    50,    46,
      47,    48,    49,    50,    84,    14,     7,    42,    43,    44,
      45,    91,    36,    39,    42,    43,    44,    45,    40,    46,
      47,    48,    49,    50,    55,    76,    46,    47,    48,    49,
      50,    41,    42,    43,    44,    45,    41,    50,    86,     0,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    83,
      85,    88,    46,    47,    48,    49,    50,    42,    43,     0,
      45,    46,    47,    48,    49,    50,    56,     0,     7,    46,
      47,    48,    49,    50,    57,    58,    59,     0,    60,    46,
      47,     0,     0,    50
};

static const yytype_int8 yycheck[] =
{
       6,     1,     5,     4,    22,     7,    16,    25,    14,    20,
      21,     9,    12,     0,    56,    57,    17,    19,    21,    22,
      23,    24,     8,    21,    22,    23,    24,    25,    39,    40,
      36,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      82,    41,     6,     6,     5,     9,     7,     9,    10,     8,
      56,    57,    13,    14,    15,    18,    17,     8,     9,    21,
      22,    23,    24,    25,     6,    26,    27,     9,    10,    11,
      12,     3,    83,     7,    21,    22,    82,    24,    25,    21,
      22,    23,    24,    25,     6,    20,     7,     9,    10,    11,
      12,     6,     3,     5,     9,    10,    11,    12,     5,    21,
      22,    23,    24,    25,    17,     6,    21,    22,    23,    24,
      25,     8,     9,    10,    11,    12,     8,    25,    17,    -1,
       9,    10,    11,    12,    21,    22,    23,    24,    25,    18,
      66,    81,    21,    22,    23,    24,    25,     9,    10,    -1,
      12,    21,    22,    23,    24,    25,     5,    -1,     7,    21,
      22,    23,    24,    25,    13,    14,    15,    -1,    17,    21,
      22,    -1,    -1,    25
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    16,    29,    40,    40,     0,    30,     7,    19,    31,
      39,     8,     8,     3,    20,    35,    40,     7,    33,    39,
       5,    13,    14,    15,    17,    26,    27,    37,    38,    39,
       5,    21,    22,    23,    24,    34,     3,    37,    37,     5,
       5,     8,     9,    10,    11,    12,    21,    22,    23,    24,
      25,    41,     4,    17,    32,    17,     5,    13,    14,    15,
      17,    36,    39,     6,    37,    37,    40,    37,    37,    37,
      37,    37,    37,    37,    37,    37,     6,     6,    18,    36,
      36,     8,     9,    18,     6,    41,    17,     6,    35,    36,
      37,     6
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    28,    29,    30,    30,    31,    31,    31,    31,    32,
      32,    33,    34,    34,    34,    34,    35,    35,    36,    36,
      36,    36,    36,    36,    36,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    38,
      38,    38,    38,    39,    40,    40,    41,    41
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     7,     4,     0,     5,     6,     3,     6,     3,
       1,     1,     1,     1,     1,     1,     6,     0,     3,     2,
       3,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     6,     4,     3,     1,     1,
       1,     1,     1,     1,     2,     0,     3,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined INPUT_LTYPE_IS_TRIVIAL && INPUT_LTYPE_IS_TRIVIAL

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

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, conj, result, coreList, varToCoreName, varToProperty); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  YYUSE (conj);
  YYUSE (result);
  YYUSE (coreList);
  YYUSE (varToCoreName);
  YYUSE (varToProperty);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp, conj, result, coreList, varToCoreName, varToProperty);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , conj, result, coreList, varToCoreName, varToProperty);
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (conj);
  YYUSE (result);
  YYUSE (coreList);
  YYUSE (varToCoreName);
  YYUSE (varToProperty);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
yyparse (Conjecture &conj, int &result, map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> &varToProperty)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
  yychar = YYEMPTY;
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
  case 2:
#line 79 "input_parser.y"
                    {conj.setRoot((yyvsp[-1].conjectureNode)); result = 0;}
#line 1580 "input_parser.cpp"
    break;

  case 3:
#line 81 "input_parser.y"
                                                                                                   {}
#line 1586 "input_parser.cpp"
    break;

  case 5:
#line 85 "input_parser.y"
                                {if(check_varToProperty((yyvsp[-4].string),varToProperty)){ cout<<" variable " << (yyvsp[-4].string) << " is written at least two times" <<endl; YYERROR; exit(20);}
                                if(!coreList.count((yyvsp[-2].string))){cout<<"core name \" "<<(yyvsp[-2].string)<<"\"is not exist"<<endl; YYERROR;}
                                if(!coreList[(yyvsp[-2].string)].count("CoreType")){cout<<"CoreType of "<<(yyvsp[-2].string)<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[(yyvsp[-2].string)]["CoreType"]!="Bool"){cout<<"CoreType of "<<(yyvsp[-2].string)<< " isn't Bool, check the core properties"<<endl; YYERROR;}
                                if(!coreList[(yyvsp[-2].string)].count("ParameterType")){cout<<"Parameter of "<<(yyvsp[-2].string)<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[(yyvsp[-2].string)]["ParameterType"]!="UnsignedInt"){cout<<"Parameter of "<<(yyvsp[-2].string)<< " isn't UnsignedInt, check the core properties"<<endl; YYERROR;}
                                if(!coreList[(yyvsp[-2].string)].count("PrimaryOperator")){cout<<"PrimaryOperator for "<<(yyvsp[-2].string)<<" isn't set, check the core properties"<<endl;YYERROR;}
                                if(coreList[(yyvsp[-2].string)]["PrimaryOperator"]=="AtLeast" and (strcmp((yyvsp[-1].string),"<")==0 or strcmp((yyvsp[-1].string),"<=")==0)   ){
                                   cout<<"PrimaryOperator for "<<(yyvsp[-2].string)<<" is " <<coreList[(yyvsp[-2].string)]["PrimaryOperator"]<<" and the written operator is " <<(yyvsp[-1].string)<<" , check the core properties"<<endl;YYERROR;}
                                if( coreList[(yyvsp[-2].string)]["PrimaryOperator"]=="AtMost" and (strcmp((yyvsp[-1].string),">")==0 or strcmp((yyvsp[-1].string),">=")==0)  )
                                {cout<<"PrimaryOperator for "<<(yyvsp[-2].string)<<" is " <<coreList[(yyvsp[-2].string)]["PrimaryOperator"]<<" and the written operator is " <<(yyvsp[-1].string)<<" , check the core properties"<<endl;YYERROR;}
                                (yyval.property) = new PropertyAssignment(); (yyval.property)->setParameterType("UnsignedInt");
                                (yyval.property)->setName((yyvsp[-2].string)); (yyval.property)->setOp((yyvsp[-1].string));
                                (yyval.property)->setParameter((yyvsp[0].number));  varToProperty[(yyvsp[-4].string)] = (yyval.property); varToCoreName[(yyvsp[-4].string)] = (yyvsp[-2].string);}
#line 1605 "input_parser.cpp"
    break;

  case 6:
#line 100 "input_parser.y"
                                                                                     {}
#line 1611 "input_parser.cpp"
    break;

  case 7:
#line 102 "input_parser.y"
                        {if(check_varToProperty((yyvsp[-2].string),varToProperty)){ cout<<" variable " << (yyvsp[-2].string) << " is written at least two times" <<endl; YYERROR; exit(20);}
                                if(!coreList.count((yyvsp[0].string))){cout<<"core name \" "<<(yyvsp[0].string)<<"\"is not exist"<<endl; YYERROR;}
                                if(!coreList[(yyvsp[0].string)].count("CoreType")){cout<<"CoreType of "<<(yyvsp[0].string)<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(!(coreList[(yyvsp[0].string)]["CoreType"]=="Bool" or coreList[(yyvsp[0].string)]["CoreType"]=="Min" or coreList[(yyvsp[0].string)]["CoreType"]=="Max"))
                                {cout<<"CoreType of "<<(yyvsp[0].string)<< " isn't correct, check the core properties, the given CoreType is "<<coreList[(yyvsp[0].string)]["CoreType"] <<endl; YYERROR;}
                                if(!coreList[(yyvsp[0].string)].count("ParameterType")){cout<<"Parameter of "<<(yyvsp[0].string)<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[(yyvsp[0].string)]["ParameterType"]!="ParameterLess"){cout<<"Parameter of "<<(yyvsp[0].string) <<" is " << coreList[(yyvsp[0].string)]["ParameterType"]<< " and it should be ParameterLess, check the core properties"<<endl; YYERROR;}
                                (yyval.property) = new PropertyAssignment(); (yyval.property)->setParameterType("ParameterLess");
                                (yyval.property)->setName((yyvsp[0].string));
                                varToProperty[(yyvsp[-2].string)] = (yyval.property); varToCoreName[(yyvsp[-2].string)] = (yyvsp[0].string);}
#line 1626 "input_parser.cpp"
    break;

  case 8:
#line 112 "input_parser.y"
                                                                                           {}
#line 1632 "input_parser.cpp"
    break;

  case 9:
#line 114 "input_parser.y"
                                                             {(yyval.vec) = new std::vector<int>; (yyval.vec) = (yyvsp[-2].vec); (yyval.vec)->push_back((yyvsp[0].number));}
#line 1638 "input_parser.cpp"
    break;

  case 10:
#line 115 "input_parser.y"
                                    { (yyval.vec) = new std::vector<int>;  (yyval.vec)->push_back((yyvsp[0].number));}
#line 1644 "input_parser.cpp"
    break;

  case 12:
#line 118 "input_parser.y"
                    {(yyval.string)=(yyvsp[0].string);}
#line 1650 "input_parser.cpp"
    break;

  case 13:
#line 119 "input_parser.y"
                     {(yyval.string)=(yyvsp[0].string);}
#line 1656 "input_parser.cpp"
    break;

  case 14:
#line 120 "input_parser.y"
                    {(yyval.string)=(yyvsp[0].string);}
#line 1662 "input_parser.cpp"
    break;

  case 15:
#line 121 "input_parser.y"
                   {(yyval.string)=(yyvsp[0].string);}
#line 1668 "input_parser.cpp"
    break;

  case 16:
#line 123 "input_parser.y"
                                                                {if(check_varToProperty((yyvsp[-4].string),varToProperty)){
                                                                    cout<< "variable " << (yyvsp[-4].string) << " declared at least two times" <<endl; YYERROR;}
                                                                  if(check_sub_formula_variables((yyvsp[-4].string))){
                                                                    cout<< "variable " << (yyvsp[-4].string) << " declared at least two times" <<endl; YYERROR;
                                                                  }
                                                                  sub_formula_variables.insert(make_pair((yyvsp[-4].string), (yyvsp[-2].conjectureNode)));
                                                                  }
#line 1680 "input_parser.cpp"
    break;

  case 18:
#line 133 "input_parser.y"
                                          {(yyval.conjectureNode)= new ConjectureNode(OPERATOR,"and");
                vector<ConjectureNode*> children; children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));
                (yyval.conjectureNode)->setChildren(children);
                (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
                }
#line 1690 "input_parser.cpp"
    break;

  case 19:
#line 139 "input_parser.y"
                               {(yyval.conjectureNode)= new ConjectureNode(OPERATOR,"not");
                vector<ConjectureNode*> children; children.push_back((yyvsp[0].conjectureNode));
                (yyval.conjectureNode)->setChildren(children);
                (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
                }
#line 1700 "input_parser.cpp"
    break;

  case 20:
#line 145 "input_parser.y"
                                       {(yyval.conjectureNode) = (yyvsp[-1].conjectureNode);}
#line 1706 "input_parser.cpp"
    break;

  case 21:
#line 148 "input_parser.y"
                       {if(check_varToProperty((yyvsp[0].string),varToProperty) and !check_sub_formula_variables((yyvsp[0].string)) ){
                    (yyval.conjectureNode) = new ConjectureNode(CORE_VARIABLE, (yyvsp[0].string));
                }else if(check_sub_formula_variables((yyvsp[0].string)) and !check_varToProperty((yyvsp[0].string),varToProperty) ) {
                    (yyval.conjectureNode) = sub_formula_variables[(yyvsp[0].string)];
                }else{
                    cout<<" variable "<< (yyvsp[0].string) << " is not valid"<< endl; YYERROR;
                }
            }
#line 1719 "input_parser.cpp"
    break;

  case 22:
#line 157 "input_parser.y"
                   {(yyval.conjectureNode) = new ConjectureNode(NUMBER,1);}
#line 1725 "input_parser.cpp"
    break;

  case 23:
#line 159 "input_parser.y"
                    {(yyval.conjectureNode) = new ConjectureNode(NUMBER,0);}
#line 1731 "input_parser.cpp"
    break;

  case 24:
#line 161 "input_parser.y"
                            {(yyval.conjectureNode)= new ConjectureNode(NUMBER,(yyvsp[0].number));}
#line 1737 "input_parser.cpp"
    break;

  case 25:
#line 163 "input_parser.y"
                                  {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"and");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1747 "input_parser.cpp"
    break;

  case 26:
#line 168 "input_parser.y"
                                 {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"or");
                    vector<ConjectureNode*> children;
                    children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
                    (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1757 "input_parser.cpp"
    break;

  case 27:
#line 173 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"implies");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1767 "input_parser.cpp"
    break;

  case 28:
#line 178 "input_parser.y"
                                  {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"iff");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1777 "input_parser.cpp"
    break;

  case 29:
#line 183 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,">=");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1787 "input_parser.cpp"
    break;

  case 30:
#line 188 "input_parser.y"
                                     {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"<=");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1797 "input_parser.cpp"
    break;

  case 31:
#line 193 "input_parser.y"
                                   {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"<");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1807 "input_parser.cpp"
    break;

  case 32:
#line 198 "input_parser.y"
                                      {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,">");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1817 "input_parser.cpp"
    break;

  case 33:
#line 204 "input_parser.y"
                           {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,"not");
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1826 "input_parser.cpp"
    break;

  case 34:
#line 208 "input_parser.y"
                                                          {(yyval.conjectureNode) = new ConjectureNode(OPERATOR,(yyvsp[-1].string));
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-2].conjectureNode)); children.push_back((yyvsp[0].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-2].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[0].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1836 "input_parser.cpp"
    break;

  case 35:
#line 213 "input_parser.y"
                                                                 {(yyval.conjectureNode) = new ConjectureNode(FUNCTION_BINARY,(yyvsp[-5].string));
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-3].conjectureNode)); children.push_back((yyvsp[-1].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-3].conjectureNode)->setParent((yyval.conjectureNode)); (yyvsp[-1].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1846 "input_parser.cpp"
    break;

  case 36:
#line 218 "input_parser.y"
                                                  {(yyval.conjectureNode) = new ConjectureNode(FUNCTION_UNARY,(yyvsp[-3].string));
            vector<ConjectureNode*> children;
            children.push_back((yyvsp[-1].conjectureNode));	(yyval.conjectureNode)->setChildren(children);
            (yyvsp[-1].conjectureNode)->setParent((yyval.conjectureNode));
            }
#line 1856 "input_parser.cpp"
    break;

  case 37:
#line 223 "input_parser.y"
                                        {(yyval.conjectureNode) = (yyvsp[-1].conjectureNode);}
#line 1862 "input_parser.cpp"
    break;

  case 39:
#line 226 "input_parser.y"
                                {(yyval.conjectureNode) = new ConjectureNode(NUMBER,1); }
#line 1868 "input_parser.cpp"
    break;

  case 40:
#line 227 "input_parser.y"
                                {(yyval.conjectureNode) = new ConjectureNode(NUMBER,0); }
#line 1874 "input_parser.cpp"
    break;

  case 41:
#line 228 "input_parser.y"
                                { if(check_varToProperty((yyvsp[0].string),varToProperty) and !check_sub_formula_variables((yyvsp[0].string)) ){
                                   (yyval.conjectureNode) = new ConjectureNode(CORE_VARIABLE, (yyvsp[0].string));
                                  }else if(check_sub_formula_variables((yyvsp[0].string)) and !check_varToProperty((yyvsp[0].string),varToProperty) ) {
                                    (yyval.conjectureNode) = sub_formula_variables[(yyvsp[0].string)];
                                  }else{
                                    cout<<" variable "<< (yyvsp[0].string) << " is not valid"<< endl; YYERROR;
                                  }
                                }
#line 1887 "input_parser.cpp"
    break;

  case 42:
#line 236 "input_parser.y"
                                 {(yyval.conjectureNode)= new ConjectureNode(NUMBER,(yyvsp[0].number));}
#line 1893 "input_parser.cpp"
    break;

  case 44:
#line 246 "input_parser.y"
                                                                   {}
#line 1899 "input_parser.cpp"
    break;


#line 1903 "input_parser.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (conj, result, coreList, varToCoreName, varToProperty, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (conj, result, coreList, varToCoreName, varToProperty, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, conj, result, coreList, varToCoreName, varToProperty);
          yychar = YYEMPTY;
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp, yylsp, conj, result, coreList, varToCoreName, varToProperty);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (conj, result, coreList, varToCoreName, varToProperty, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
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
                  yystos[+*yyssp], yyvsp, yylsp, conj, result, coreList, varToCoreName, varToProperty);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 252 "input_parser.y"


void yyerror(Conjecture &conj, int &result,map<string,map<string,string>> &coreList, map<string,string> &varToCoreName,map<string, PropertyAssignment*> varToProperty, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " line " <<input_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp 
}

bool check_varToProperty(string v,map<string, PropertyAssignment*> &varToProperty ){
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



