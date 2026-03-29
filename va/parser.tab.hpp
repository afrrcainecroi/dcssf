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

#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TK_RULE = 258,                 /* TK_RULE  */
    TK_IP = 259,                   /* TK_IP  */
    TK_HOST = 260,                 /* TK_HOST  */
    TK_URL = 261,                  /* TK_URL  */
    TK_ACTION = 262,               /* TK_ACTION  */
    TK_CHANGE_CS = 263,            /* TK_CHANGE_CS  */
    TK_CHANGE_SC = 264,            /* TK_CHANGE_SC  */
    TK_HEADER = 265,               /* TK_HEADER  */
    TK_COOKIE = 266,               /* TK_COOKIE  */
    TK_BODY = 267,                 /* TK_BODY  */
    TK_I_NUMBER = 268,             /* TK_I_NUMBER  */
    TK_F_NUMBER = 269,             /* TK_F_NUMBER  */
    TK_STRING = 270,               /* TK_STRING  */
    TK_C_STRING = 271,             /* TK_C_STRING  */
    TK_MANAGE = 272,               /* TK_MANAGE  */
    TK_REDIRECT = 273,             /* TK_REDIRECT  */
    TK_ACCEPT = 274,               /* TK_ACCEPT  */
    TK_GIVE = 275,                 /* TK_GIVE  */
    TK_BLOCK = 276,                /* TK_BLOCK  */
    TK_QUERY = 277,                /* TK_QUERY  */
    TK_IP_NUMBER = 278,            /* TK_IP_NUMBER  */
    TK_IP_PORT_NUMBER = 279        /* TK_IP_PORT_NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 38 "parser.y"

	std::string * stdstring;
	T_bar_rule  * trule;
	T_cmd_and_pars  * tcmd;
	std::list<string*> *slist;
	tuple<string*, string*, string*> * s3tuple;
	list<tuple<string*, string*, string*>*> * ls3tuple;

#line 97 "parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
