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




/* First part of user prologue.  */
#line 1 "parser.y"

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <getopt.h>
	#include <iostream>
	#include <string>
	#include <list>
	#include <tuple>
	
	#include <errno.h>
	#include "parserds.h"
	
	using namespace std;
	
	//#define YYERROR_VERBOSE 1       /* For debugging.   */
	//#define YYPARSE_PARAM scanner   /* For pure bison parser. */
	//#define YYLEX_PARAM   scanner   /* For reentrant flex. */
	//#define YYDEBUG 1
	
	extern char * yytext;
	extern int yyleng;
	int  yylex();
	extern int prs_lineno;
	
	int yyerror(const char* msg);
	extern FILE *yyin;
	extern FILE *yyout;
	
	string * yytext2string();
	
	extern list<T_bar_rule*> parsed_rules;
	
	

#line 107 "parser.tab.cpp"

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

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TK_RULE = 3,                    /* TK_RULE  */
  YYSYMBOL_TK_IP = 4,                      /* TK_IP  */
  YYSYMBOL_TK_HOST = 5,                    /* TK_HOST  */
  YYSYMBOL_TK_URL = 6,                     /* TK_URL  */
  YYSYMBOL_TK_ACTION = 7,                  /* TK_ACTION  */
  YYSYMBOL_TK_CHANGE_CS = 8,               /* TK_CHANGE_CS  */
  YYSYMBOL_TK_CHANGE_SC = 9,               /* TK_CHANGE_SC  */
  YYSYMBOL_TK_HEADER = 10,                 /* TK_HEADER  */
  YYSYMBOL_TK_COOKIE = 11,                 /* TK_COOKIE  */
  YYSYMBOL_TK_BODY = 12,                   /* TK_BODY  */
  YYSYMBOL_TK_I_NUMBER = 13,               /* TK_I_NUMBER  */
  YYSYMBOL_TK_F_NUMBER = 14,               /* TK_F_NUMBER  */
  YYSYMBOL_TK_STRING = 15,                 /* TK_STRING  */
  YYSYMBOL_TK_C_STRING = 16,               /* TK_C_STRING  */
  YYSYMBOL_TK_MANAGE = 17,                 /* TK_MANAGE  */
  YYSYMBOL_TK_REDIRECT = 18,               /* TK_REDIRECT  */
  YYSYMBOL_TK_ACCEPT = 19,                 /* TK_ACCEPT  */
  YYSYMBOL_TK_GIVE = 20,                   /* TK_GIVE  */
  YYSYMBOL_TK_BLOCK = 21,                  /* TK_BLOCK  */
  YYSYMBOL_TK_QUERY = 22,                  /* TK_QUERY  */
  YYSYMBOL_TK_IP_NUMBER = 23,              /* TK_IP_NUMBER  */
  YYSYMBOL_TK_IP_PORT_NUMBER = 24,         /* TK_IP_PORT_NUMBER  */
  YYSYMBOL_25_ = 25,                       /* '{'  */
  YYSYMBOL_26_ = 26,                       /* ';'  */
  YYSYMBOL_27_ = 27,                       /* '}'  */
  YYSYMBOL_28_ = 28,                       /* ':'  */
  YYSYMBOL_29_ = 29,                       /* '='  */
  YYSYMBOL_30_ = 30,                       /* ','  */
  YYSYMBOL_31_ = 31,                       /* '('  */
  YYSYMBOL_32_ = 32,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 33,                  /* $accept  */
  YYSYMBOL_start = 34,                     /* start  */
  YYSYMBOL_rules = 35,                     /* rules  */
  YYSYMBOL_rule = 36,                      /* rule  */
  YYSYMBOL_ip = 37,                        /* ip  */
  YYSYMBOL_host = 38,                      /* host  */
  YYSYMBOL_url = 39,                       /* url  */
  YYSYMBOL_cookie_or_header_list = 40,     /* cookie_or_header_list  */
  YYSYMBOL_cookie_or_header_spec = 41,     /* cookie_or_header_spec  */
  YYSYMBOL_action = 42,                    /* action  */
  YYSYMBOL_change_cs = 43,                 /* change_cs  */
  YYSYMBOL_change_sc = 44,                 /* change_sc  */
  YYSYMBOL_replace_list = 45,              /* replace_list  */
  YYSYMBOL_replace_item = 46,              /* replace_item  */
  YYSYMBOL_specific_action = 47,           /* specific_action  */
  YYSYMBOL_manage = 48,                    /* manage  */
  YYSYMBOL_mg_parameters = 49,             /* mg_parameters  */
  YYSYMBOL_mg_parameter = 50,              /* mg_parameter  */
  YYSYMBOL_redirect = 51,                  /* redirect  */
  YYSYMBOL_string_or_empty = 52,           /* string_or_empty  */
  YYSYMBOL_string_or_ip = 53,              /* string_or_ip  */
  YYSYMBOL_gstring = 54                    /* gstring  */
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
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   111

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  44
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  124

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   279


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
      31,    32,     2,     2,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    28,    26,
       2,    29,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,     2,    27,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    92,    92,    95,    97,   101,   137,   215,   217,   221,
     223,   227,   229,   233,   237,   241,   248,   255,   264,   268,
     273,   279,   285,   293,   300,   307,   316,   324,   332,   340,
     349,   358,   363,   365,   373,   375,   377,   381,   390,   392,
     396,   398,   400,   405,   407
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TK_RULE", "TK_IP",
  "TK_HOST", "TK_URL", "TK_ACTION", "TK_CHANGE_CS", "TK_CHANGE_SC",
  "TK_HEADER", "TK_COOKIE", "TK_BODY", "TK_I_NUMBER", "TK_F_NUMBER",
  "TK_STRING", "TK_C_STRING", "TK_MANAGE", "TK_REDIRECT", "TK_ACCEPT",
  "TK_GIVE", "TK_BLOCK", "TK_QUERY", "TK_IP_NUMBER", "TK_IP_PORT_NUMBER",
  "'{'", "';'", "'}'", "':'", "'='", "','", "'('", "')'", "$accept",
  "start", "rules", "rule", "ip", "host", "url", "cookie_or_header_list",
  "cookie_or_header_spec", "action", "change_cs", "change_sc",
  "replace_list", "replace_item", "specific_action", "manage",
  "mg_parameters", "mg_parameter", "redirect", "string_or_empty",
  "string_or_ip", "gstring", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-98)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       6,   -11,    19,   -98,     6,    15,   -98,   -98,    -4,     8,
       9,    31,     1,    20,   -98,    21,   -98,   -98,    22,    25,
      34,    36,    33,    49,    33,    -8,    35,    54,    52,   -98,
     -98,    32,    28,    37,    38,    39,   -98,   -98,   -98,   -98,
      40,    43,    47,   -98,    35,    50,    41,    51,    42,   -98,
     -98,   -98,   -98,    28,   -98,    -8,   -98,   -98,    33,    33,
      33,   -98,    -8,    57,    35,    61,   -98,    55,    53,    56,
      58,   -98,    59,    63,   -98,    64,    71,    33,    33,    33,
      33,    -5,    54,    62,   -98,   -98,   -98,   -98,    65,    66,
      67,    69,    -5,    72,    33,    33,    33,    33,   -98,   -98,
      57,    73,   -98,    68,    74,    75,    76,    60,    33,    33,
      33,    -5,    77,    79,    81,    82,    83,    33,   -98,   -98,
     -98,   -98,    85,   -98
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     3,     0,     1,     4,     0,     0,
       0,     0,     0,     0,    28,     0,    30,    18,     0,     0,
       0,     0,     0,     0,     0,     8,    21,     0,     0,    43,
      44,     0,    32,     0,     0,     0,    41,    42,     7,    40,
       0,     0,     0,    19,    21,     0,     0,     0,     0,    26,
      35,    36,    31,    32,    34,     0,    27,    29,     0,     0,
       0,    22,    10,     0,    21,     0,    33,     0,     0,     0,
       0,     9,     0,     0,    20,     0,     0,     0,     0,     0,
      12,    14,     0,     0,    23,    24,    25,    11,     0,     0,
       0,     0,    14,     0,    39,     0,     0,     0,     5,    13,
       0,     0,    38,     0,     0,     0,     0,     0,    39,    39,
      39,    14,     0,     0,     0,     0,     0,    39,    16,    15,
      17,     6,     0,    37
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -98,   -98,    80,   -98,    27,    -2,   -15,   -88,   -98,   -98,
     -98,   -98,   -43,   -98,   -98,   -98,    48,   -98,   -98,   -97,
     -37,   -22
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,     4,    20,    46,    73,    91,    92,     9,
      21,    48,    43,    44,    17,    31,    52,    53,    34,   101,
      38,   102
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      32,    61,    35,    39,    99,    88,    89,    29,    30,     1,
      54,   113,   114,   115,     5,    36,    37,    90,    67,     6,
     122,    74,     8,   116,    10,    71,    12,    13,    14,    15,
      16,    54,    22,    39,    11,    18,    68,    69,    70,    19,
      39,    50,    51,    29,    30,    40,    41,    42,    29,    30,
      25,    23,    24,    26,    33,    84,    85,    86,    87,    45,
      27,    47,    28,    72,    49,    18,    55,    63,    65,    58,
      56,    57,    59,   103,   104,   105,    60,    83,    62,    64,
      93,    76,   112,    77,     7,   106,    78,    80,    79,    81,
      82,    94,    75,    95,    96,    97,    98,   108,   100,   107,
       0,    66,   111,   109,   110,   118,   117,   119,   120,     0,
     121,   123
};

static const yytype_int8 yycheck[] =
{
      22,    44,    24,    25,    92,    10,    11,    15,    16,     3,
      32,   108,   109,   110,    25,    23,    24,    22,    55,     0,
     117,    64,     7,   111,    28,    62,    17,    18,    19,    20,
      21,    53,    31,    55,    26,     4,    58,    59,    60,     8,
      62,    13,    14,    15,    16,    10,    11,    12,    15,    16,
      28,    31,    31,    28,     5,    77,    78,    79,    80,     5,
      26,     9,    26,     6,    32,     4,    29,    26,    26,    29,
      32,    32,    29,    95,    96,    97,    29,     6,    28,    28,
      82,    26,    22,    30,     4,   100,    30,    28,    30,    26,
      26,    29,    65,    28,    28,    28,    27,    29,    26,    26,
      -1,    53,    26,    29,    29,    26,    29,    26,    26,    -1,
      27,    26
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    34,    35,    36,    25,     0,    35,     7,    42,
      28,    26,    17,    18,    19,    20,    21,    47,     4,     8,
      37,    43,    31,    31,    31,    28,    28,    26,    26,    15,
      16,    48,    54,     5,    51,    54,    23,    24,    53,    54,
      10,    11,    12,    45,    46,     5,    38,     9,    44,    32,
      13,    14,    49,    50,    54,    29,    32,    32,    29,    29,
      29,    45,    28,    26,    28,    26,    49,    53,    54,    54,
      54,    53,     6,    39,    45,    37,    26,    30,    30,    30,
      28,    26,    26,     6,    54,    54,    54,    54,    10,    11,
      22,    40,    41,    38,    29,    28,    28,    28,    27,    40,
      26,    52,    54,    54,    54,    54,    39,    26,    29,    29,
      29,    26,    22,    52,    52,    52,    40,    29,    26,    26,
      26,    27,    52,    26
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    33,    34,    35,    35,    36,    36,    37,    37,    38,
      38,    39,    39,    40,    40,    41,    41,    41,    42,    43,
      44,    45,    45,    46,    46,    46,    47,    47,    47,    47,
      47,    48,    49,    49,    50,    50,    50,    51,    52,    52,
      53,    53,    53,    54,    54
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,    12,    16,     3,     2,     3,
       2,     3,     2,     2,     0,     6,     6,     6,     3,     3,
       3,     0,     2,     5,     5,     5,     4,     4,     1,     4,
       1,     2,     0,     2,     1,     1,     1,    12,     1,     0,
       1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
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
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* rules: rule  */
#line 95 "parser.y"
     {parsed_rules.push_back((yyvsp[0].trule));}
#line 1219 "parser.tab.cpp"
    break;

  case 4: /* rules: rule rules  */
#line 97 "parser.y"
           {parsed_rules.push_front((yyvsp[-1].trule));}
#line 1225 "parser.tab.cpp"
    break;

  case 5: /* rule: TK_RULE '{' action ';' ip ';' host ';' url ';' cookie_or_header_list '}'  */
#line 102 "parser.y"
{
	auto p = new T_bar_rule();
	
	//action
	//printf("$3=%X, What: %d, Parm: %X\n", $3, $3->what, $3->parameters);
	p->action = (yyvsp[-9].tcmd)->what;
	for (auto it: *((yyvsp[-9].tcmd)->parameters))
	{
		p->action_parameters.push_back(*it);
		delete it;
	}
	delete (yyvsp[-9].tcmd)->parameters;
	delete (yyvsp[-9].tcmd);
	
	p->ip = *((yyvsp[-7].stdstring)); delete (yyvsp[-7].stdstring);
	p->host = *((yyvsp[-5].stdstring)); delete (yyvsp[-5].stdstring);
	p->url = *((yyvsp[-3].stdstring)); delete (yyvsp[-3].stdstring);
	
	//list<tuple<string*, string*, string*>*> * ls3tuple;
	for (auto it: *((yyvsp[-1].ls3tuple)))
	{
		string key;
		key = get<0>(*it)->substr(0,1);  //C/H
		key += *get<1>(*it);
		p->mapof_chq[key] = *get<2>(*it);
		delete get<0>(*it);
		delete get<1>(*it);
		delete get<2>(*it);
		delete it;
	}
	delete (yyvsp[-1].ls3tuple);
	
	(yyval.trule)=p;
}
#line 1264 "parser.tab.cpp"
    break;

  case 6: /* rule: TK_RULE '{' action ';' change_cs ';' change_sc ';' ip ';' host ';' url ';' cookie_or_header_list '}'  */
#line 138 "parser.y"
{
	auto p = new T_bar_rule();
	
	//action
	p->action = (yyvsp[-13].tcmd)->what;
	for (auto it: *((yyvsp[-13].tcmd)->parameters))
	{
		p->action_parameters.push_back(*it);
		delete it;
	}
	delete (yyvsp[-13].tcmd)->parameters;
	delete (yyvsp[-13].tcmd);
	
	p->ip = *((yyvsp[-7].stdstring)); delete (yyvsp[-7].stdstring);
	p->host = *((yyvsp[-5].stdstring)); delete (yyvsp[-5].stdstring);
	p->url = *((yyvsp[-3].stdstring)); delete (yyvsp[-3].stdstring);
	
	//list<tuple<string*, string*, string*>*> * ls3tuple;
	for (auto it: *((yyvsp[-1].ls3tuple)))
	{
		string key;
		key = get<0>(*it)->substr(0,1);  //C/H
		key += *get<1>(*it);
		p->mapof_chq[key] = *get<2>(*it);
		delete get<0>(*it);
		delete get<1>(*it);
		delete get<2>(*it);
		delete it;
	}
	delete (yyvsp[-1].ls3tuple);
	
	p->is_change = false;
	if ((yyvsp[-11].ls3tuple) == nullptr)
	{
		p->change_parameters_cs.clear();
	}
	else
	{
		p->is_change = true;
		for (auto it: *((yyvsp[-11].ls3tuple)))
		{
			string key;
			key = get<0>(*it)->substr(0,1);  //C/H/B
			key += *get<1>(*it);
			p->change_parameters_cs.push_back(make_pair(key, *get<2>(*it)));
			delete get<0>(*it);
			delete get<1>(*it);
			delete get<2>(*it);
			delete it;
		}
		delete (yyvsp[-11].ls3tuple);
	}
	if ((yyvsp[-9].ls3tuple) == nullptr)
	{
		p->change_parameters_sc.clear();
	}
	else
	{
		p->is_change = true;
		for (auto it: *((yyvsp[-9].ls3tuple)))
		{
			string key;
			key = get<0>(*it)->substr(0,1);  //C/H/B
			key += *get<1>(*it);
			p->change_parameters_sc.push_back(make_pair(key, *get<2>(*it)));
			delete get<0>(*it);
			delete get<1>(*it);
			delete get<2>(*it);
			delete it;
		}
		delete (yyvsp[-9].ls3tuple);
	}
	(yyval.trule)=p;
}
#line 1343 "parser.tab.cpp"
    break;

  case 7: /* ip: TK_IP ':' string_or_ip  */
#line 215 "parser.y"
                       {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1349 "parser.tab.cpp"
    break;

  case 8: /* ip: TK_IP ':'  */
#line 217 "parser.y"
          {(yyval.stdstring)=new string();}
#line 1355 "parser.tab.cpp"
    break;

  case 9: /* host: TK_HOST ':' string_or_ip  */
#line 221 "parser.y"
                         {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1361 "parser.tab.cpp"
    break;

  case 10: /* host: TK_HOST ':'  */
#line 223 "parser.y"
            {(yyval.stdstring)=new string();}
#line 1367 "parser.tab.cpp"
    break;

  case 11: /* url: TK_URL ':' gstring  */
#line 227 "parser.y"
                   {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1373 "parser.tab.cpp"
    break;

  case 12: /* url: TK_URL ':'  */
#line 229 "parser.y"
           {(yyval.stdstring)=new string();}
#line 1379 "parser.tab.cpp"
    break;

  case 13: /* cookie_or_header_list: cookie_or_header_spec cookie_or_header_list  */
#line 234 "parser.y"
{ (yyvsp[0].ls3tuple)->push_front((yyvsp[-1].s3tuple)); (yyval.ls3tuple)=(yyvsp[0].ls3tuple); }
#line 1385 "parser.tab.cpp"
    break;

  case 14: /* cookie_or_header_list: %empty  */
#line 237 "parser.y"
{ auto ls3 = new list<tuple<string*, string*, string*>*>(); (yyval.ls3tuple)=ls3; }
#line 1391 "parser.tab.cpp"
    break;

  case 15: /* cookie_or_header_spec: TK_COOKIE ':' gstring '=' string_or_empty ';'  */
#line 242 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("COOKIE"), (yyvsp[-3].stdstring), (yyvsp[-1].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1401 "parser.tab.cpp"
    break;

  case 16: /* cookie_or_header_spec: TK_HEADER ':' gstring '=' string_or_empty ';'  */
#line 249 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("HEADER"), (yyvsp[-3].stdstring), (yyvsp[-1].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1411 "parser.tab.cpp"
    break;

  case 17: /* cookie_or_header_spec: TK_QUERY ':' gstring '=' string_or_empty ';'  */
#line 256 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("QUERY"), (yyvsp[-3].stdstring), (yyvsp[-1].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1421 "parser.tab.cpp"
    break;

  case 18: /* action: TK_ACTION ':' specific_action  */
#line 264 "parser.y"
                              {(yyval.tcmd)=(yyvsp[0].tcmd);}
#line 1427 "parser.tab.cpp"
    break;

  case 19: /* change_cs: TK_CHANGE_CS ':' replace_list  */
#line 269 "parser.y"
{ (yyval.ls3tuple) = (yyvsp[0].ls3tuple); }
#line 1433 "parser.tab.cpp"
    break;

  case 20: /* change_sc: TK_CHANGE_SC ':' replace_list  */
#line 274 "parser.y"
{ (yyval.ls3tuple) = (yyvsp[0].ls3tuple); }
#line 1439 "parser.tab.cpp"
    break;

  case 21: /* replace_list: %empty  */
#line 279 "parser.y"
{
	auto ls3 = new list<tuple<string*, string*, string*>*>();
	//ls3->push_front($1);
	(yyval.ls3tuple)=ls3;
}
#line 1449 "parser.tab.cpp"
    break;

  case 22: /* replace_list: replace_item replace_list  */
#line 286 "parser.y"
{
	(yyvsp[0].ls3tuple)->push_front((yyvsp[-1].s3tuple));
	(yyval.ls3tuple) = (yyvsp[0].ls3tuple);
}
#line 1458 "parser.tab.cpp"
    break;

  case 23: /* replace_item: TK_HEADER '=' gstring ',' gstring  */
#line 294 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("HEADER"), (yyvsp[-2].stdstring), (yyvsp[0].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1468 "parser.tab.cpp"
    break;

  case 24: /* replace_item: TK_COOKIE '=' gstring ',' gstring  */
#line 301 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("COOKIE"), (yyvsp[-2].stdstring), (yyvsp[0].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1478 "parser.tab.cpp"
    break;

  case 25: /* replace_item: TK_BODY '=' gstring ',' gstring  */
#line 308 "parser.y"
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("BODY"), (yyvsp[-2].stdstring), (yyvsp[0].stdstring));
	(yyval.s3tuple)=s3;
}
#line 1488 "parser.tab.cpp"
    break;

  case 26: /* specific_action: TK_MANAGE '(' manage ')'  */
#line 317 "parser.y"
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_manage;
	cmd->parameters = (yyvsp[-1].slist);
	(yyval.tcmd)=cmd;
}
#line 1499 "parser.tab.cpp"
    break;

  case 27: /* specific_action: TK_REDIRECT '(' redirect ')'  */
#line 325 "parser.y"
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_redirect;
	cmd->parameters = (yyvsp[-1].slist);
	(yyval.tcmd)=cmd;
}
#line 1510 "parser.tab.cpp"
    break;

  case 28: /* specific_action: TK_ACCEPT  */
#line 333 "parser.y"
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_accept;
	cmd->parameters = new list<string*>();
	(yyval.tcmd)=cmd;
}
#line 1521 "parser.tab.cpp"
    break;

  case 29: /* specific_action: TK_GIVE '(' gstring ')'  */
#line 341 "parser.y"
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_give;
	cmd->parameters = new list<string*>();
        cmd->parameters->push_back((yyvsp[-1].stdstring));
	(yyval.tcmd)=cmd;
}
#line 1533 "parser.tab.cpp"
    break;

  case 30: /* specific_action: TK_BLOCK  */
#line 350 "parser.y"
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_block;
	cmd->parameters = new list<string*>();
	(yyval.tcmd)=cmd;
}
#line 1544 "parser.tab.cpp"
    break;

  case 31: /* manage: gstring mg_parameters  */
#line 359 "parser.y"
{ (yyvsp[0].slist)->push_front((yyvsp[-1].stdstring)); (yyval.slist)=(yyvsp[0].slist);}
#line 1550 "parser.tab.cpp"
    break;

  case 32: /* mg_parameters: %empty  */
#line 363 "parser.y"
            {auto ls = new list<string*>(); (yyval.slist)=ls;}
#line 1556 "parser.tab.cpp"
    break;

  case 33: /* mg_parameters: mg_parameter mg_parameters  */
#line 366 "parser.y"
{
	//cerr << "Parametro: " << *($1) << endl;
	(yyvsp[0].slist)->push_front((yyvsp[-1].stdstring)); (yyval.slist)=(yyvsp[0].slist);
}
#line 1565 "parser.tab.cpp"
    break;

  case 34: /* mg_parameter: gstring  */
#line 373 "parser.y"
        {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1571 "parser.tab.cpp"
    break;

  case 35: /* mg_parameter: TK_I_NUMBER  */
#line 375 "parser.y"
            {(yyval.stdstring)=yytext2string();}
#line 1577 "parser.tab.cpp"
    break;

  case 36: /* mg_parameter: TK_F_NUMBER  */
#line 377 "parser.y"
            {(yyval.stdstring)=yytext2string();}
#line 1583 "parser.tab.cpp"
    break;

  case 37: /* redirect: TK_HOST '=' string_or_ip ';' TK_URL '=' string_or_empty ';' TK_QUERY '=' string_or_empty ';'  */
#line 382 "parser.y"
{
	auto ls = new list<string*>();
	ls->push_back((yyvsp[-9].stdstring)); ls->push_back((yyvsp[-5].stdstring)); ls->push_back((yyvsp[-1].stdstring));
	(yyval.slist)=ls;
}
#line 1593 "parser.tab.cpp"
    break;

  case 38: /* string_or_empty: gstring  */
#line 390 "parser.y"
        {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1599 "parser.tab.cpp"
    break;

  case 39: /* string_or_empty: %empty  */
#line 392 "parser.y"
           {string * s = new string(); (yyval.stdstring) = s ; }
#line 1605 "parser.tab.cpp"
    break;

  case 40: /* string_or_ip: gstring  */
#line 396 "parser.y"
        {(yyval.stdstring)=(yyvsp[0].stdstring);}
#line 1611 "parser.tab.cpp"
    break;

  case 41: /* string_or_ip: TK_IP_NUMBER  */
#line 398 "parser.y"
             {(yyval.stdstring)=yytext2string();}
#line 1617 "parser.tab.cpp"
    break;

  case 42: /* string_or_ip: TK_IP_PORT_NUMBER  */
#line 400 "parser.y"
                  {(yyval.stdstring)=yytext2string();}
#line 1623 "parser.tab.cpp"
    break;

  case 43: /* gstring: TK_STRING  */
#line 405 "parser.y"
          {(yyval.stdstring)=yytext2string();}
#line 1629 "parser.tab.cpp"
    break;

  case 44: /* gstring: TK_C_STRING  */
#line 408 "parser.y"
{
	extern string stringa_tipo_c;
	//cerr << "LEtta: <" << stringa_tipo_c << ">" << endl;
	(yyval.stdstring)=new string(stringa_tipo_c);
}
#line 1639 "parser.tab.cpp"
    break;


#line 1643 "parser.tab.cpp"

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 416 "parser.y"


/****************************************************
 start of code section
	*****************************************************/

string * yytext2string()
{
	string * s = new string();
	s->assign(yytext, yyleng);
	return s;
}

int yyerror(const char* msg) {
	fprintf(stderr,"\n\nError %s at line:%d.\n<%s>\n",msg, prs_lineno, yytext);
	return 0;
}

int main_none(int argc, char **argv);

int main_none(int argc, char **argv)
{
	int c;
	
	//extern int yydebug;
	//yydebug=0;
	
	FILE *file;
	
	yyin = stdin;
	
	int iret = yyparse();
	
	printf("%d\n", iret);
	return 0;
}

