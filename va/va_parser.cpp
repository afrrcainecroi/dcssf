// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 33 "va_parser.y"

    #include <iostream>
    #include "va_scanner.h"
    #include "va_parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static VaParser::Parser::symbol_type yylex(VaParser::Scanner &scanner, VaParser::Interpreter &driver) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace VaParser;

#line 58 "va_parser.cpp"




#include "va_parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 9 "va_parser.y"
namespace  VaParser  {
#line 158 "va_parser.cpp"

  /// Build a parser object.
   Parser :: Parser  (VaParser::Scanner &scanner_yyarg, VaParser::Interpreter &driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

   Parser ::~ Parser  ()
  {}

   Parser ::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
   Parser ::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

   Parser ::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
   Parser ::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
   Parser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

   Parser ::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

   Parser ::symbol_kind_type
   Parser ::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

   Parser ::stack_symbol_type::stack_symbol_type ()
  {}

   Parser ::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ar_definition: // ar_definition
      case symbol_kind::S_access_operations: // access_operations
      case symbol_kind::S_access_operation: // access_operation
      case symbol_kind::S_access_rules: // access_rules
      case symbol_kind::S_vr_definition: // vr_definition
      case symbol_kind::S_clauses: // clauses
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_observable: // observable
      case symbol_kind::S_cat_observables: // cat_observables
      case symbol_kind::S_ar_observable: // ar_observable
      case symbol_kind::S_ar_cat_observables: // ar_cat_observables
      case symbol_kind::S_item_when: // item_when
      case symbol_kind::S_basic_item_when: // basic_item_when
      case symbol_kind::S_basic_access_rules: // basic_access_rules
      case symbol_kind::S_item_basic_access_rule: // item_basic_access_rule
      case symbol_kind::S_item_basic_access_rule_with_not: // item_basic_access_rule_with_not
      case symbol_kind::S_item_action: // item_action
      case symbol_kind::S_action_variable_item: // action_variable_item
      case symbol_kind::S_set_definition: // set_definition
        value.YY_MOVE_OR_COPY<  Command  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.YY_MOVE_OR_COPY<  bool  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_sezione_lista_with: // sezione_lista_with
      case symbol_kind::S_lista_with: // lista_with
      case symbol_kind::S_observables_list: // observables_list
      case symbol_kind::S_ar_observables_list: // ar_observables_list
      case symbol_kind::S_lista_when: // lista_when
      case symbol_kind::S_lista_action: // lista_action
      case symbol_kind::S_action_variable_list: // action_variable_list
      case symbol_kind::S_observable_list: // observable_list
      case symbol_kind::S_observable_assign_list: // observable_assign_list
        value.YY_MOVE_OR_COPY<  list< Command >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.YY_MOVE_OR_COPY<  list< pair<string, string> >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.YY_MOVE_OR_COPY<  list< string >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENT: // "ident"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_VARIABLE: // "variable ($string)"
      case symbol_kind::S_I_NUMBER: // "integer number"
      case symbol_kind::S_IP_WITH_STAR: // "ip_with_star"
      case symbol_kind::S_IP_WITHOUT_STAR: // "ip_without_star"
      case symbol_kind::S_URL_WITH_STAR: // "url_with_star"
      case symbol_kind::S_TIMING: // "timing"
      case symbol_kind::S_HTTP_METHOD_GET: // HTTP_METHOD_GET
      case symbol_kind::S_HTTP_METHOD_POST: // HTTP_METHOD_POST
      case symbol_kind::S_HTTP_METHOD_HEAD: // HTTP_METHOD_HEAD
      case symbol_kind::S_HTTP_METHOD_PUT: // HTTP_METHOD_PUT
      case symbol_kind::S_HTTP_METHOD_DELETE: // HTTP_METHOD_DELETE
      case symbol_kind::S_HTTP_METHOD_TRACE: // HTTP_METHOD_TRACE
      case symbol_kind::S_HTTP_METHOD_OPTIONS: // HTTP_METHOD_OPTIONS
      case symbol_kind::S_HTTP_METHOD_CONNECT: // HTTP_METHOD_CONNECT
      case symbol_kind::S_HTTP_METHOD_PATCH: // HTTP_METHOD_PATCH
      case symbol_kind::S_OBS_EVENT_ACM_LIGHT: // OBS_EVENT_ACM_LIGHT
      case symbol_kind::S_pattern_extractor: // pattern_extractor
      case symbol_kind::S_true_false: // true_false
      case symbol_kind::S_http_method: // http_method
      case symbol_kind::S_network_event: // network_event
      case symbol_kind::S_ident_or_string: // ident_or_string
      case symbol_kind::S_ip_address: // ip_address
      case symbol_kind::S_ip_or_domain: // ip_or_domain
        value.YY_MOVE_OR_COPY<  string  > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

   Parser ::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ar_definition: // ar_definition
      case symbol_kind::S_access_operations: // access_operations
      case symbol_kind::S_access_operation: // access_operation
      case symbol_kind::S_access_rules: // access_rules
      case symbol_kind::S_vr_definition: // vr_definition
      case symbol_kind::S_clauses: // clauses
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_observable: // observable
      case symbol_kind::S_cat_observables: // cat_observables
      case symbol_kind::S_ar_observable: // ar_observable
      case symbol_kind::S_ar_cat_observables: // ar_cat_observables
      case symbol_kind::S_item_when: // item_when
      case symbol_kind::S_basic_item_when: // basic_item_when
      case symbol_kind::S_basic_access_rules: // basic_access_rules
      case symbol_kind::S_item_basic_access_rule: // item_basic_access_rule
      case symbol_kind::S_item_basic_access_rule_with_not: // item_basic_access_rule_with_not
      case symbol_kind::S_item_action: // item_action
      case symbol_kind::S_action_variable_item: // action_variable_item
      case symbol_kind::S_set_definition: // set_definition
        value.move<  Command  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.move<  bool  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_sezione_lista_with: // sezione_lista_with
      case symbol_kind::S_lista_with: // lista_with
      case symbol_kind::S_observables_list: // observables_list
      case symbol_kind::S_ar_observables_list: // ar_observables_list
      case symbol_kind::S_lista_when: // lista_when
      case symbol_kind::S_lista_action: // lista_action
      case symbol_kind::S_action_variable_list: // action_variable_list
      case symbol_kind::S_observable_list: // observable_list
      case symbol_kind::S_observable_assign_list: // observable_assign_list
        value.move<  list< Command >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.move<  list< pair<string, string> >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.move<  list< string >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENT: // "ident"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_VARIABLE: // "variable ($string)"
      case symbol_kind::S_I_NUMBER: // "integer number"
      case symbol_kind::S_IP_WITH_STAR: // "ip_with_star"
      case symbol_kind::S_IP_WITHOUT_STAR: // "ip_without_star"
      case symbol_kind::S_URL_WITH_STAR: // "url_with_star"
      case symbol_kind::S_TIMING: // "timing"
      case symbol_kind::S_HTTP_METHOD_GET: // HTTP_METHOD_GET
      case symbol_kind::S_HTTP_METHOD_POST: // HTTP_METHOD_POST
      case symbol_kind::S_HTTP_METHOD_HEAD: // HTTP_METHOD_HEAD
      case symbol_kind::S_HTTP_METHOD_PUT: // HTTP_METHOD_PUT
      case symbol_kind::S_HTTP_METHOD_DELETE: // HTTP_METHOD_DELETE
      case symbol_kind::S_HTTP_METHOD_TRACE: // HTTP_METHOD_TRACE
      case symbol_kind::S_HTTP_METHOD_OPTIONS: // HTTP_METHOD_OPTIONS
      case symbol_kind::S_HTTP_METHOD_CONNECT: // HTTP_METHOD_CONNECT
      case symbol_kind::S_HTTP_METHOD_PATCH: // HTTP_METHOD_PATCH
      case symbol_kind::S_OBS_EVENT_ACM_LIGHT: // OBS_EVENT_ACM_LIGHT
      case symbol_kind::S_pattern_extractor: // pattern_extractor
      case symbol_kind::S_true_false: // true_false
      case symbol_kind::S_http_method: // http_method
      case symbol_kind::S_network_event: // network_event
      case symbol_kind::S_ident_or_string: // ident_or_string
      case symbol_kind::S_ip_address: // ip_address
      case symbol_kind::S_ip_or_domain: // ip_or_domain
        value.move<  string  > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ar_definition: // ar_definition
      case symbol_kind::S_access_operations: // access_operations
      case symbol_kind::S_access_operation: // access_operation
      case symbol_kind::S_access_rules: // access_rules
      case symbol_kind::S_vr_definition: // vr_definition
      case symbol_kind::S_clauses: // clauses
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_observable: // observable
      case symbol_kind::S_cat_observables: // cat_observables
      case symbol_kind::S_ar_observable: // ar_observable
      case symbol_kind::S_ar_cat_observables: // ar_cat_observables
      case symbol_kind::S_item_when: // item_when
      case symbol_kind::S_basic_item_when: // basic_item_when
      case symbol_kind::S_basic_access_rules: // basic_access_rules
      case symbol_kind::S_item_basic_access_rule: // item_basic_access_rule
      case symbol_kind::S_item_basic_access_rule_with_not: // item_basic_access_rule_with_not
      case symbol_kind::S_item_action: // item_action
      case symbol_kind::S_action_variable_item: // action_variable_item
      case symbol_kind::S_set_definition: // set_definition
        value.copy<  Command  > (that.value);
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.copy<  bool  > (that.value);
        break;

      case symbol_kind::S_sezione_lista_with: // sezione_lista_with
      case symbol_kind::S_lista_with: // lista_with
      case symbol_kind::S_observables_list: // observables_list
      case symbol_kind::S_ar_observables_list: // ar_observables_list
      case symbol_kind::S_lista_when: // lista_when
      case symbol_kind::S_lista_action: // lista_action
      case symbol_kind::S_action_variable_list: // action_variable_list
      case symbol_kind::S_observable_list: // observable_list
      case symbol_kind::S_observable_assign_list: // observable_assign_list
        value.copy<  list< Command >  > (that.value);
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.copy<  list< pair<string, string> >  > (that.value);
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.copy<  list< string >  > (that.value);
        break;

      case symbol_kind::S_IDENT: // "ident"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_VARIABLE: // "variable ($string)"
      case symbol_kind::S_I_NUMBER: // "integer number"
      case symbol_kind::S_IP_WITH_STAR: // "ip_with_star"
      case symbol_kind::S_IP_WITHOUT_STAR: // "ip_without_star"
      case symbol_kind::S_URL_WITH_STAR: // "url_with_star"
      case symbol_kind::S_TIMING: // "timing"
      case symbol_kind::S_HTTP_METHOD_GET: // HTTP_METHOD_GET
      case symbol_kind::S_HTTP_METHOD_POST: // HTTP_METHOD_POST
      case symbol_kind::S_HTTP_METHOD_HEAD: // HTTP_METHOD_HEAD
      case symbol_kind::S_HTTP_METHOD_PUT: // HTTP_METHOD_PUT
      case symbol_kind::S_HTTP_METHOD_DELETE: // HTTP_METHOD_DELETE
      case symbol_kind::S_HTTP_METHOD_TRACE: // HTTP_METHOD_TRACE
      case symbol_kind::S_HTTP_METHOD_OPTIONS: // HTTP_METHOD_OPTIONS
      case symbol_kind::S_HTTP_METHOD_CONNECT: // HTTP_METHOD_CONNECT
      case symbol_kind::S_HTTP_METHOD_PATCH: // HTTP_METHOD_PATCH
      case symbol_kind::S_OBS_EVENT_ACM_LIGHT: // OBS_EVENT_ACM_LIGHT
      case symbol_kind::S_pattern_extractor: // pattern_extractor
      case symbol_kind::S_true_false: // true_false
      case symbol_kind::S_http_method: // http_method
      case symbol_kind::S_network_event: // network_event
      case symbol_kind::S_ident_or_string: // ident_or_string
      case symbol_kind::S_ip_address: // ip_address
      case symbol_kind::S_ip_or_domain: // ip_or_domain
        value.copy<  string  > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ar_definition: // ar_definition
      case symbol_kind::S_access_operations: // access_operations
      case symbol_kind::S_access_operation: // access_operation
      case symbol_kind::S_access_rules: // access_rules
      case symbol_kind::S_vr_definition: // vr_definition
      case symbol_kind::S_clauses: // clauses
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_observable: // observable
      case symbol_kind::S_cat_observables: // cat_observables
      case symbol_kind::S_ar_observable: // ar_observable
      case symbol_kind::S_ar_cat_observables: // ar_cat_observables
      case symbol_kind::S_item_when: // item_when
      case symbol_kind::S_basic_item_when: // basic_item_when
      case symbol_kind::S_basic_access_rules: // basic_access_rules
      case symbol_kind::S_item_basic_access_rule: // item_basic_access_rule
      case symbol_kind::S_item_basic_access_rule_with_not: // item_basic_access_rule_with_not
      case symbol_kind::S_item_action: // item_action
      case symbol_kind::S_action_variable_item: // action_variable_item
      case symbol_kind::S_set_definition: // set_definition
        value.move<  Command  > (that.value);
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.move<  bool  > (that.value);
        break;

      case symbol_kind::S_sezione_lista_with: // sezione_lista_with
      case symbol_kind::S_lista_with: // lista_with
      case symbol_kind::S_observables_list: // observables_list
      case symbol_kind::S_ar_observables_list: // ar_observables_list
      case symbol_kind::S_lista_when: // lista_when
      case symbol_kind::S_lista_action: // lista_action
      case symbol_kind::S_action_variable_list: // action_variable_list
      case symbol_kind::S_observable_list: // observable_list
      case symbol_kind::S_observable_assign_list: // observable_assign_list
        value.move<  list< Command >  > (that.value);
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.move<  list< pair<string, string> >  > (that.value);
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.move<  list< string >  > (that.value);
        break;

      case symbol_kind::S_IDENT: // "ident"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_VARIABLE: // "variable ($string)"
      case symbol_kind::S_I_NUMBER: // "integer number"
      case symbol_kind::S_IP_WITH_STAR: // "ip_with_star"
      case symbol_kind::S_IP_WITHOUT_STAR: // "ip_without_star"
      case symbol_kind::S_URL_WITH_STAR: // "url_with_star"
      case symbol_kind::S_TIMING: // "timing"
      case symbol_kind::S_HTTP_METHOD_GET: // HTTP_METHOD_GET
      case symbol_kind::S_HTTP_METHOD_POST: // HTTP_METHOD_POST
      case symbol_kind::S_HTTP_METHOD_HEAD: // HTTP_METHOD_HEAD
      case symbol_kind::S_HTTP_METHOD_PUT: // HTTP_METHOD_PUT
      case symbol_kind::S_HTTP_METHOD_DELETE: // HTTP_METHOD_DELETE
      case symbol_kind::S_HTTP_METHOD_TRACE: // HTTP_METHOD_TRACE
      case symbol_kind::S_HTTP_METHOD_OPTIONS: // HTTP_METHOD_OPTIONS
      case symbol_kind::S_HTTP_METHOD_CONNECT: // HTTP_METHOD_CONNECT
      case symbol_kind::S_HTTP_METHOD_PATCH: // HTTP_METHOD_PATCH
      case symbol_kind::S_OBS_EVENT_ACM_LIGHT: // OBS_EVENT_ACM_LIGHT
      case symbol_kind::S_pattern_extractor: // pattern_extractor
      case symbol_kind::S_true_false: // true_false
      case symbol_kind::S_http_method: // http_method
      case symbol_kind::S_network_event: // network_event
      case symbol_kind::S_ident_or_string: // ident_or_string
      case symbol_kind::S_ip_address: // ip_address
      case symbol_kind::S_ip_or_domain: // ip_or_domain
        value.move<  string  > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
   Parser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
   Parser ::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
   Parser ::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
   Parser ::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
   Parser ::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   Parser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   Parser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   Parser ::debug_level_type
   Parser ::debug_level () const
  {
    return yydebug_;
  }

  void
   Parser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

   Parser ::state_type
   Parser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
   Parser ::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
   Parser ::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
   Parser ::operator() ()
  {
    return parse ();
  }

  int
   Parser ::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, driver));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_ar_definition: // ar_definition
      case symbol_kind::S_access_operations: // access_operations
      case symbol_kind::S_access_operation: // access_operation
      case symbol_kind::S_access_rules: // access_rules
      case symbol_kind::S_vr_definition: // vr_definition
      case symbol_kind::S_clauses: // clauses
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_term: // term
      case symbol_kind::S_factor: // factor
      case symbol_kind::S_observable: // observable
      case symbol_kind::S_cat_observables: // cat_observables
      case symbol_kind::S_ar_observable: // ar_observable
      case symbol_kind::S_ar_cat_observables: // ar_cat_observables
      case symbol_kind::S_item_when: // item_when
      case symbol_kind::S_basic_item_when: // basic_item_when
      case symbol_kind::S_basic_access_rules: // basic_access_rules
      case symbol_kind::S_item_basic_access_rule: // item_basic_access_rule
      case symbol_kind::S_item_basic_access_rule_with_not: // item_basic_access_rule_with_not
      case symbol_kind::S_item_action: // item_action
      case symbol_kind::S_action_variable_item: // action_variable_item
      case symbol_kind::S_set_definition: // set_definition
        yylhs.value.emplace<  Command  > ();
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        yylhs.value.emplace<  bool  > ();
        break;

      case symbol_kind::S_sezione_lista_with: // sezione_lista_with
      case symbol_kind::S_lista_with: // lista_with
      case symbol_kind::S_observables_list: // observables_list
      case symbol_kind::S_ar_observables_list: // ar_observables_list
      case symbol_kind::S_lista_when: // lista_when
      case symbol_kind::S_lista_action: // lista_action
      case symbol_kind::S_action_variable_list: // action_variable_list
      case symbol_kind::S_observable_list: // observable_list
      case symbol_kind::S_observable_assign_list: // observable_assign_list
        yylhs.value.emplace<  list< Command >  > ();
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        yylhs.value.emplace<  list< pair<string, string> >  > ();
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        yylhs.value.emplace<  list< string >  > ();
        break;

      case symbol_kind::S_IDENT: // "ident"
      case symbol_kind::S_STRING: // "string"
      case symbol_kind::S_VARIABLE: // "variable ($string)"
      case symbol_kind::S_I_NUMBER: // "integer number"
      case symbol_kind::S_IP_WITH_STAR: // "ip_with_star"
      case symbol_kind::S_IP_WITHOUT_STAR: // "ip_without_star"
      case symbol_kind::S_URL_WITH_STAR: // "url_with_star"
      case symbol_kind::S_TIMING: // "timing"
      case symbol_kind::S_HTTP_METHOD_GET: // HTTP_METHOD_GET
      case symbol_kind::S_HTTP_METHOD_POST: // HTTP_METHOD_POST
      case symbol_kind::S_HTTP_METHOD_HEAD: // HTTP_METHOD_HEAD
      case symbol_kind::S_HTTP_METHOD_PUT: // HTTP_METHOD_PUT
      case symbol_kind::S_HTTP_METHOD_DELETE: // HTTP_METHOD_DELETE
      case symbol_kind::S_HTTP_METHOD_TRACE: // HTTP_METHOD_TRACE
      case symbol_kind::S_HTTP_METHOD_OPTIONS: // HTTP_METHOD_OPTIONS
      case symbol_kind::S_HTTP_METHOD_CONNECT: // HTTP_METHOD_CONNECT
      case symbol_kind::S_HTTP_METHOD_PATCH: // HTTP_METHOD_PATCH
      case symbol_kind::S_OBS_EVENT_ACM_LIGHT: // OBS_EVENT_ACM_LIGHT
      case symbol_kind::S_pattern_extractor: // pattern_extractor
      case symbol_kind::S_true_false: // true_false
      case symbol_kind::S_http_method: // http_method
      case symbol_kind::S_network_event: // network_event
      case symbol_kind::S_ident_or_string: // ident_or_string
      case symbol_kind::S_ip_address: // ip_address
      case symbol_kind::S_ip_or_domain: // ip_or_domain
        yylhs.value.emplace<  string  > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: %empty
#line 449 "va_parser.y"
                   {}
#line 953 "va_parser.cpp"
    break;

  case 3: // program: program set_definition
#line 452 "va_parser.y"
                                   {
                driver.addCommand(yystack_[0].value.as <  Command  > ());
            }
#line 961 "va_parser.cpp"
    break;

  case 4: // program: program vr_definition
#line 457 "va_parser.y"
                                  {
                driver.addCommand(yystack_[0].value.as <  Command  > ());
            }
#line 969 "va_parser.cpp"
    break;

  case 5: // program: program ar_definition
#line 462 "va_parser.y"
                                  {
                driver.addCommand(yystack_[0].value.as <  Command  > ());
            }
#line 977 "va_parser.cpp"
    break;

  case 6: // program: "end of file"
#line 466 "va_parser.y"
                {}
#line 983 "va_parser.cpp"
    break;

  case 7: // ar_definition: "define" "Access rule" ident_or_string access_rules "action" access_operations "semicolon"
#line 473 "va_parser.y"
                {
                    yylhs.value.as <  Command  > () = Command(token::TOKEN_AR_DEF, yystack_[4].value.as <  string  > ());
                    yylhs.value.as <  Command  > ().addArg(yystack_[3].value.as <  Command  > ());
                    yylhs.value.as <  Command  > ().addArg(yystack_[1].value.as <  Command  > ());
                }
#line 993 "va_parser.cpp"
    break;

  case 8: // access_operations: access_operation access_operations
#line 480 "va_parser.y"
                                                       { yylhs.value.as <  Command  > () = yystack_[0].value.as <  Command  > (); yylhs.value.as <  Command  > ().addArg(yystack_[1].value.as <  Command  > ());}
#line 999 "va_parser.cpp"
    break;

  case 9: // access_operations: %empty
#line 482 "va_parser.y"
                            { yylhs.value.as <  Command  > () = Command(token::TOKEN_ACCESS_OPERATIONS, ""); }
#line 1005 "va_parser.cpp"
    break;

  case 10: // access_operation: "trace" ident_or_string "comma" ident_or_string "comma" ident_or_string
#line 487 "va_parser.y"
                    {
                        list<string> lst;
                        lst.push_back(yystack_[4].value.as <  string  > ());
                        lst.push_back(yystack_[2].value.as <  string  > ());
                        lst.push_back(yystack_[0].value.as <  string  > ());
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_TRACE, "", lst);
                    }
#line 1017 "va_parser.cpp"
    break;

  case 11: // access_operation: "In tutti gli header esegue, compresa url, esegue il replace" ident_or_string ident_or_string
#line 499 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_REPLACE_HEADER_IN_REQUEST, "", lst); }
#line 1023 "va_parser.cpp"
    break;

  case 12: // access_operation: HTTP_ANSWER_HEADER_REPLACE ident_or_string ident_or_string
#line 502 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_REPLACE_HEADER_IN_ANSWER, "", lst); }
#line 1029 "va_parser.cpp"
    break;

  case 13: // access_operation: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" "equal" ident_or_string
#line 505 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[3].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_HEADER_TO_REQUEST, "", lst); }
#line 1035 "va_parser.cpp"
    break;

  case 14: // access_operation: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar" "equal" ident_or_string
#line 508 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[3].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_HEADER_TO_ANSWER, "", lst); }
#line 1041 "va_parser.cpp"
    break;

  case 15: // access_operation: HTTP_HEADER "leftsqpar" ident_or_string "equal" ident_or_string "rightsqpar" "equal" ident_or_string "equal" ident_or_string
#line 511 "va_parser.y"
                    {   list<string> lst; lst.push_back(yystack_[7].value.as <  string  > ()); lst.push_back(yystack_[5].value.as <  string  > ()); lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ());
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_V_HEADER_TO_V_REQUEST, "", lst); }
#line 1048 "va_parser.cpp"
    break;

  case 16: // access_operation: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "equal" ident_or_string "rightsqpar" "equal" ident_or_string "equal" ident_or_string
#line 515 "va_parser.y"
                    {   list<string> lst; lst.push_back(yystack_[7].value.as <  string  > ()); lst.push_back(yystack_[5].value.as <  string  > ()); lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ());
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_V_HEADER_TO_V_ANSWER, "", lst); }
#line 1055 "va_parser.cpp"
    break;

  case 17: // access_operation: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" "equal" ident_or_string
#line 519 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[3].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_COOKIE_TO_REQUEST, "", lst); }
#line 1061 "va_parser.cpp"
    break;

  case 18: // access_operation: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar" "equal" ident_or_string
#line 522 "va_parser.y"
                    { list<string> lst; lst.push_back(yystack_[3].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_COOKIE_TO_ANSWER, "", lst); }
#line 1067 "va_parser.cpp"
    break;

  case 19: // access_operation: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "equal" ident_or_string "rightsqpar" "equal" ident_or_string "equal" ident_or_string
#line 525 "va_parser.y"
                    {   list<string> lst; lst.push_back(yystack_[7].value.as <  string  > ()); lst.push_back(yystack_[5].value.as <  string  > ()); lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ());
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER, "", lst); }
#line 1074 "va_parser.cpp"
    break;

  case 20: // access_operation: " add " observable " to " "set" ident_or_string pattern_extractor
#line 532 "va_parser.y"
                                                                             {list<Command> lst; 
                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[1].value.as <  string  > (), lst);
                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                    }
#line 1084 "va_parser.cpp"
    break;

  case 21: // access_operation: " add " cat_observables " to " "set" ident_or_string
#line 538 "va_parser.y"
                                                               {list<Command> lst; 
                        lst.push_back(yystack_[3].value.as <  Command  > ()); 
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[0].value.as <  string  > (), lst);
                    }
#line 1093 "va_parser.cpp"
    break;

  case 22: // access_operation: " add " observable " to " "set" ident_or_string "integer number" pattern_extractor
#line 543 "va_parser.y"
                                                                                     {list<Command> lst; 
                        lst.push_back(yystack_[5].value.as <  Command  > ()); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[1].value.as <  string  > ()));
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[2].value.as <  string  > (), lst);
                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                    }
#line 1104 "va_parser.cpp"
    break;

  case 23: // access_operation: " add " cat_observables " to " "set" ident_or_string "integer number"
#line 550 "va_parser.y"
                                                                        {list<Command> lst; 
                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[0].value.as <  string  > ()));
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[1].value.as <  string  > (), lst);
                    }
#line 1114 "va_parser.cpp"
    break;

  case 24: // access_operation: " del " observable " from " "set" ident_or_string pattern_extractor
#line 556 "va_parser.y"
                                                                                  {  list<Command> lst; 
                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[1].value.as <  string  > (), lst);
                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                    }
#line 1124 "va_parser.cpp"
    break;

  case 25: // access_operation: " del " observable " from " "set" ident_or_string "integer number" pattern_extractor
#line 562 "va_parser.y"
                                                                                        {   list<Command> lst; 
                        lst.push_back(yystack_[5].value.as <  Command  > ()); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[1].value.as <  string  > ()));
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[2].value.as <  string  > (), lst);
                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                    }
#line 1135 "va_parser.cpp"
    break;

  case 26: // access_operation: " del " cat_observables " from " "set" ident_or_string
#line 569 "va_parser.y"
                                                                 {  list<Command> lst; 
                        lst.push_back(yystack_[3].value.as <  Command  > ()); 
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[0].value.as <  string  > (), lst);
                    }
#line 1144 "va_parser.cpp"
    break;

  case 27: // access_operation: " del " cat_observables " from " "set" ident_or_string "integer number"
#line 574 "va_parser.y"
                                                                           {   list<Command> lst; 
                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[0].value.as <  string  > ()));
                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[1].value.as <  string  > (), lst);
                    }
#line 1154 "va_parser.cpp"
    break;

  case 28: // access_operation: "Calls a lisp function" ident_or_string "Parameters for a lisp function" ar_observables_list
#line 580 "va_parser.y"
                                                                  {
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, yystack_[2].value.as <  string  > (), yystack_[0].value.as <  list< Command >  > ());
                    }
#line 1162 "va_parser.cpp"
    break;

  case 29: // access_operation: "TCP_REDIRECT" ident_or_string "equal" ident_or_string ssl_server_options
#line 590 "va_parser.y"
                    { 
                        list<string> lst; 
                        lst.push_back(yystack_[3].value.as <  string  > ()); 
                        lst.push_back(yystack_[1].value.as <  string  > ());
                        for (auto & it: yystack_[0].value.as <  list< string >  > ())
                            lst.push_back(it);
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_TCP_REDIRECT_VSTRING, "", lst); 
                    }
#line 1175 "va_parser.cpp"
    break;

  case 30: // access_operation: "TCP_REDIRECT" ident_or_string ssl_server_options
#line 600 "va_parser.y"
                    { 
                        list<string> lst; 
                        lst.push_back(yystack_[1].value.as <  string  > ()); 
                        for (auto & it: yystack_[0].value.as <  list< string >  > ())
                            lst.push_back(it);
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_TCP_REDIRECT_STRING, "", lst); 
                    }
#line 1187 "va_parser.cpp"
    break;

  case 31: // access_operation: "TCP_REDIRECT" ident_or_string "equal" "HTTP HOST" ssl_server_options
#line 609 "va_parser.y"
                    {
                        list<string> lst; 
                        lst.push_back(yystack_[3].value.as <  string  > ()); 
                        for (auto & it: yystack_[0].value.as <  list< string >  > ())
                            lst.push_back(it);
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_TCP_REDIRECT_VHOST, "", lst); 
                    }
#line 1199 "va_parser.cpp"
    break;

  case 32: // access_operation: "TCP_REDIRECT" "HTTP HOST" ssl_server_options
#line 618 "va_parser.y"
                    { 
                        list<string> lst; 
                        for (auto & it: yystack_[0].value.as <  list< string >  > ())
                            lst.push_back(it);
                        yylhs.value.as <  Command  > () = Command(token::TOKEN_TCP_REDIRECT_HOST, "", lst); 
                    }
#line 1210 "va_parser.cpp"
    break;

  case 33: // access_operation: "HTTP_REDIRECT" ident_or_string "equal" ident_or_string
#line 625 "va_parser.y"
                                                                        { list<string> lst; lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_VREDIRECT, "", lst); }
#line 1216 "va_parser.cpp"
    break;

  case 34: // access_operation: "HTTP_REDIRECT" ident_or_string
#line 627 "va_parser.y"
                                                  { list<string> lst; lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_REDIRECT, "", lst); }
#line 1222 "va_parser.cpp"
    break;

  case 35: // access_operation: "BANDWIDTH" ident_or_string "equal" "integer number"
#line 633 "va_parser.y"
                                                              { list<string> lst; lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VBANDWIDTH, "", lst); }
#line 1228 "va_parser.cpp"
    break;

  case 36: // access_operation: "BANDWIDTH" "integer number"
#line 635 "va_parser.y"
                                        { list<string> lst; lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_BANDWIDTH, "", lst); }
#line 1234 "va_parser.cpp"
    break;

  case 37: // access_operation: "ANSWER" ident_or_string "equal" ident_or_string
#line 637 "va_parser.y"
                                                                 { list<string> lst; lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VANSWER, "", lst); }
#line 1240 "va_parser.cpp"
    break;

  case 38: // access_operation: "ANSWER" ident_or_string
#line 639 "va_parser.y"
                                           { list<string> lst; lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_ANSWER, "", lst); }
#line 1246 "va_parser.cpp"
    break;

  case 39: // access_operation: "scm.manage" observable_list
#line 641 "va_parser.y"
                                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_SCM_MANAGE, "", yystack_[0].value.as <  list< Command >  > ()); }
#line 1252 "va_parser.cpp"
    break;

  case 40: // access_operation: "scm.give" ident_or_string
#line 643 "va_parser.y"
                                             { list<string> lst; lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_SCM_GIVE, "", lst); }
#line 1258 "va_parser.cpp"
    break;

  case 41: // access_operation: "scm.manage" observable_assign_list
#line 645 "va_parser.y"
                                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_VSCM_MANAGE, "", yystack_[0].value.as <  list< Command >  > ()); }
#line 1264 "va_parser.cpp"
    break;

  case 42: // access_operation: "scm.give" ident_or_string "equal" ident_or_string
#line 647 "va_parser.y"
                                                                    { list<string> lst; lst.push_back(yystack_[2].value.as <  string  > ()); lst.push_back(yystack_[0].value.as <  string  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VSCM_GIVE, "", lst); }
#line 1270 "va_parser.cpp"
    break;

  case 43: // access_rules: "condition" basic_access_rules
#line 654 "va_parser.y"
                                             { list<Command> lc; lc.push_back(yystack_[0].value.as <  Command  > ()); yylhs.value.as <  Command  > ()=Command(token::TOKEN_IF, "", lc); }
#line 1276 "va_parser.cpp"
    break;

  case 44: // vr_definition: "define" "validation rule" ident_or_string "condition" clauses "semicolon"
#line 661 "va_parser.y"
                {
                    list<Command> lst1; lst1.push_back(yystack_[1].value.as <  Command  > ());
                    list<Command> lst;
                    lst.push_back(Command(token::TOKEN_VR_DEF_WHEN, "", lst1));
                    //lst.push_back(Command(token::TOKEN_VR_DEF_ACTION, "", $7));
                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF, yystack_[3].value.as <  string  > (), lst);
                }
#line 1288 "va_parser.cpp"
    break;

  case 45: // clauses: clauses " next " expr
#line 670 "va_parser.y"
                              {auto lst = list<Command>(); lst.push_back(yystack_[2].value.as <  Command  > ()); lst.push_back(yystack_[0].value.as <  Command  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_IS_LOP_NEXT, "", lst); }
#line 1294 "va_parser.cpp"
    break;

  case 46: // clauses: expr
#line 672 "va_parser.y"
                                {yylhs.value.as <  Command  > () = yystack_[0].value.as <  Command  > ();}
#line 1300 "va_parser.cpp"
    break;

  case 47: // expr: expr " or " term
#line 675 "va_parser.y"
                      {auto lst = list<Command>(); lst.push_back(yystack_[2].value.as <  Command  > ()); lst.push_back(yystack_[0].value.as <  Command  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_IS_LOP_OR, "", lst); }
#line 1306 "va_parser.cpp"
    break;

  case 48: // expr: term
#line 677 "va_parser.y"
              {yylhs.value.as <  Command  > () = yystack_[0].value.as <  Command  > (); }
#line 1312 "va_parser.cpp"
    break;

  case 49: // term: term " and " factor
#line 680 "va_parser.y"
                         { auto lst = list<Command>(); lst.push_back(yystack_[2].value.as <  Command  > ()); lst.push_back(yystack_[0].value.as <  Command  > ()); yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_IS_LOP_AND, "", lst); }
#line 1318 "va_parser.cpp"
    break;

  case 50: // term: factor
#line 682 "va_parser.y"
                         {yylhs.value.as <  Command  > () = yystack_[0].value.as <  Command  > ();}
#line 1324 "va_parser.cpp"
    break;

  case 51: // factor: "leftpar" clauses "rightpar"
#line 685 "va_parser.y"
                                 {yylhs.value.as <  Command  > ()=yystack_[1].value.as <  Command  > ();}
#line 1330 "va_parser.cpp"
    break;

  case 52: // factor: lista_when sezione_lista_with "action" lista_action
#line 687 "va_parser.y"
                                                         {
                                        auto c_when =   Command(token::TOKEN_VR_DEF_IS_LISTA_WHEN, "", yystack_[3].value.as <  list< Command >  > ());
                                        auto c_with =   Command(token::TOKEN_VR_DEF_IS_LISTA_WITH, "", yystack_[2].value.as <  list< Command >  > ());
                                        auto c_action = Command(token::TOKEN_VR_DEF_ACTION, "", yystack_[0].value.as <  list< Command >  > ());
                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_IS_A_WHEN_WITH, "");
                                        yylhs.value.as <  Command  > ().addArg(c_when);
                                        yylhs.value.as <  Command  > ().addArg(c_with);
                                        yylhs.value.as <  Command  > ().addArg(c_action);
                                    }
#line 1344 "va_parser.cpp"
    break;

  case 53: // factor: lista_when sezione_lista_with
#line 697 "va_parser.y"
                                      {
                                        auto c_when =   Command(token::TOKEN_VR_DEF_IS_LISTA_WHEN, "", yystack_[1].value.as <  list< Command >  > ());
                                        auto c_with =   Command(token::TOKEN_VR_DEF_IS_LISTA_WITH, "", yystack_[0].value.as <  list< Command >  > ());
                                        auto c_action = Command(token::TOKEN_VR_DEF_ACTION, "");
                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_IS_A_WHEN_WITH, "");
                                        yylhs.value.as <  Command  > ().addArg(c_when);
                                        yylhs.value.as <  Command  > ().addArg(c_with);
                                        yylhs.value.as <  Command  > ().addArg(c_action);
                                    }
#line 1358 "va_parser.cpp"
    break;

  case 54: // sezione_lista_with: "var" lista_with
#line 708 "va_parser.y"
                                   { yylhs.value.as <  list< Command >  > () = yystack_[0].value.as <  list< Command >  > (); }
#line 1364 "va_parser.cpp"
    break;

  case 55: // sezione_lista_with: %empty
#line 710 "va_parser.y"
                           {yylhs.value.as <  list< Command >  > ()=list<Command>();}
#line 1370 "va_parser.cpp"
    break;

  case 56: // lista_with: ident_or_string "equal" observable pattern_extractor lista_with
#line 713 "va_parser.y"
                                                                          {   
cout << "lista_with: legge " << yystack_[4].value.as <  string  > () << " = " << yystack_[2].value.as <  Command  > ().m_name << endl;
                                                            auto lst = list<Command>(); 
                                                            lst.push_back(yystack_[2].value.as <  Command  > ()); 
                                                            auto c = Command(token::TOKEN_EQUAL, yystack_[4].value.as <  string  > (), lst);
                                                            c.addArg(yystack_[1].value.as <  string  > ());
                                                            yylhs.value.as <  list< Command >  > ()=yystack_[0].value.as <  list< Command >  > ();
                                                            yylhs.value.as <  list< Command >  > ().push_back(c); 
                                                        }
#line 1384 "va_parser.cpp"
    break;

  case 57: // lista_with: ident_or_string "equal" cat_observables lista_with
#line 723 "va_parser.y"
                                                             {   
                                                            auto lst = list<Command>(); 
                                                            lst.push_back(yystack_[1].value.as <  Command  > ()); 
                                                            auto c = Command(token::TOKEN_EQUAL, yystack_[3].value.as <  string  > (), lst);
                                                            yylhs.value.as <  list< Command >  > ()=yystack_[0].value.as <  list< Command >  > ();
                                                            yylhs.value.as <  list< Command >  > ().push_back(c); 
                                                        }
#line 1396 "va_parser.cpp"
    break;

  case 58: // lista_with: %empty
#line 731 "va_parser.y"
                   { yylhs.value.as <  list< Command >  > () = list<Command>(); cout << "lista_with: empty\n";}
#line 1402 "va_parser.cpp"
    break;

  case 59: // observable: "agent id"
#line 735 "va_parser.y"
                                 { yylhs.value.as <  Command  > ()=Command(token::TOKEN_AGENT_ID, ""); }
#line 1408 "va_parser.cpp"
    break;

  case 60: // observable: SA_OS
#line 736 "va_parser.y"
                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_OS, ""); }
#line 1414 "va_parser.cpp"
    break;

  case 61: // observable: SA_LOGGED_USER
#line 737 "va_parser.y"
                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_LOGGED_USER, ""); }
#line 1420 "va_parser.cpp"
    break;

  case 62: // observable: SA_PROCESS_NAME
#line 738 "va_parser.y"
                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_PROCESS_NAME, ""); }
#line 1426 "va_parser.cpp"
    break;

  case 63: // observable: "OBS_NAME"
#line 739 "va_parser.y"
                                  { yylhs.value.as <  Command  > ()=Command(token::TOKEN_OBS_NAME, ""); }
#line 1432 "va_parser.cpp"
    break;

  case 64: // observable: "OBS_TIME"
#line 740 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_TIME, ""); }
#line 1438 "va_parser.cpp"
    break;

  case 65: // observable: "OBS_OBSID"
#line 741 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_OBSID, ""); }
#line 1444 "va_parser.cpp"
    break;

  case 66: // observable: "OBS_STIME"
#line 742 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_STIME, ""); }
#line 1450 "va_parser.cpp"
    break;

  case 67: // observable: "OBS_ETIME"
#line 743 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_ETIME, ""); }
#line 1456 "va_parser.cpp"
    break;

  case 68: // observable: "OBS_TAG"
#line 744 "va_parser.y"
                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_TAG, ""); }
#line 1462 "va_parser.cpp"
    break;

  case 69: // observable: "OBS_EVENT"
#line 745 "va_parser.y"
                                  { yylhs.value.as <  Command  > ()=Command(token::TOKEN_OBS_EVENT, ""); }
#line 1468 "va_parser.cpp"
    break;

  case 70: // observable: "NET_TCP_IPDST"
#line 746 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPDST, ""); }
#line 1474 "va_parser.cpp"
    break;

  case 71: // observable: "NET_TCP_IPSRC"
#line 747 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
#line 1480 "va_parser.cpp"
    break;

  case 72: // observable: "NET_RESULT"
#line 748 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_RESULT, ""); }
#line 1486 "va_parser.cpp"
    break;

  case 73: // observable: "NET_TCP_SESID"
#line 749 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_SESID, ""); }
#line 1492 "va_parser.cpp"
    break;

  case 74: // observable: HTTP_METHOD
#line 750 "va_parser.y"
                                    { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_METHOD, ""); }
#line 1498 "va_parser.cpp"
    break;

  case 75: // observable: "HTTP HOST"
#line 751 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HOST, ""); }
#line 1504 "va_parser.cpp"
    break;

  case 76: // observable: "HTTP URL"
#line 752 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL, ""); }
#line 1510 "va_parser.cpp"
    break;

  case 77: // observable: "HTTP URI"
#line 753 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URI, ""); }
#line 1516 "va_parser.cpp"
    break;

  case 78: // observable: "HTTP host in the headers"
#line 754 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
#line 1522 "va_parser.cpp"
    break;

  case 79: // observable: "HTTP host in the url"
#line 755 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL_HOST, ""); }
#line 1528 "va_parser.cpp"
    break;

  case 80: // observable: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 756 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 1534 "va_parser.cpp"
    break;

  case 81: // observable: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 757 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER, yystack_[1].value.as <  string  > ()); }
#line 1540 "va_parser.cpp"
    break;

  case 82: // observable: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar"
#line 758 "va_parser.y"
                                                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, yystack_[1].value.as <  string  > ()); }
#line 1546 "va_parser.cpp"
    break;

  case 83: // observable: HTTP_QUERY
#line 759 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
#line 1552 "va_parser.cpp"
    break;

  case 84: // observable: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 760 "va_parser.y"
                                                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, yystack_[1].value.as <  string  > ()); }
#line 1558 "va_parser.cpp"
    break;

  case 85: // observable: HTTP_DATA
#line 761 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, "0_FULL_QUERY"); }
#line 1564 "va_parser.cpp"
    break;

  case 86: // observable: HTTP_ANSWER_CODE
#line 762 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
#line 1570 "va_parser.cpp"
    break;

  case 87: // observable: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 763 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 1576 "va_parser.cpp"
    break;

  case 88: // observable: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 764 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_HEADER, yystack_[1].value.as <  string  > ()); }
#line 1582 "va_parser.cpp"
    break;

  case 89: // observable: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 765 "va_parser.y"
                                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, yystack_[1].value.as <  string  > ()); }
#line 1588 "va_parser.cpp"
    break;

  case 90: // observable: HTTP_ANSWER_DATA
#line 766 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }
#line 1594 "va_parser.cpp"
    break;

  case 91: // observable: "ident"
#line 768 "va_parser.y"
                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_IDENT, yystack_[0].value.as <  string  > ()); }
#line 1600 "va_parser.cpp"
    break;

  case 92: // observable: "string"
#line 769 "va_parser.y"
                               { 
                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_STRING, yystack_[0].value.as <  string  > ()); 
                                }
#line 1608 "va_parser.cpp"
    break;

  case 93: // observables_list: observable pattern_extractor
#line 774 "va_parser.y"
                                                     {yylhs.value.as <  list< Command >  > () = list<Command>(); auto a = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(a);}
#line 1614 "va_parser.cpp"
    break;

  case 94: // observables_list: observables_list "comma" observable pattern_extractor
#line 776 "va_parser.y"
                                                                            {yylhs.value.as <  list< Command >  > ()=yystack_[3].value.as <  list< Command >  > (); auto a = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ());  yylhs.value.as <  list< Command >  > ().push_back(a);}
#line 1620 "va_parser.cpp"
    break;

  case 95: // cat_observables: CAT "leftgrpar" observables_list "rightsgrpar"
#line 779 "va_parser.y"
                                                                  {yylhs.value.as <  Command  > ()=Command(token::TOKEN_CAT, "", yystack_[1].value.as <  list< Command >  > ());}
#line 1626 "va_parser.cpp"
    break;

  case 96: // ar_observable: "OBS_TAG"
#line 784 "va_parser.y"
                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_TAG, ""); }
#line 1632 "va_parser.cpp"
    break;

  case 97: // ar_observable: "NET_TCP_IPDST"
#line 785 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPDST, ""); }
#line 1638 "va_parser.cpp"
    break;

  case 98: // ar_observable: "NET_TCP_IPSRC"
#line 786 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
#line 1644 "va_parser.cpp"
    break;

  case 99: // ar_observable: "NET_RESULT"
#line 787 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_RESULT, ""); }
#line 1650 "va_parser.cpp"
    break;

  case 100: // ar_observable: "NET_TCP_SESID"
#line 788 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_SESID, ""); }
#line 1656 "va_parser.cpp"
    break;

  case 101: // ar_observable: HTTP_METHOD
#line 789 "va_parser.y"
                                    { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_METHOD, ""); }
#line 1662 "va_parser.cpp"
    break;

  case 102: // ar_observable: "HTTP HOST"
#line 790 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HOST, ""); }
#line 1668 "va_parser.cpp"
    break;

  case 103: // ar_observable: "HTTP URL"
#line 791 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL, ""); }
#line 1674 "va_parser.cpp"
    break;

  case 104: // ar_observable: "HTTP URI"
#line 792 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URI, ""); }
#line 1680 "va_parser.cpp"
    break;

  case 105: // ar_observable: "HTTP host in the headers"
#line 793 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
#line 1686 "va_parser.cpp"
    break;

  case 106: // ar_observable: "HTTP host in the url"
#line 794 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL_HOST, ""); }
#line 1692 "va_parser.cpp"
    break;

  case 107: // ar_observable: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 795 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 1698 "va_parser.cpp"
    break;

  case 108: // ar_observable: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 796 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER, yystack_[1].value.as <  string  > ()); }
#line 1704 "va_parser.cpp"
    break;

  case 109: // ar_observable: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar"
#line 797 "va_parser.y"
                                                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, yystack_[1].value.as <  string  > ()); }
#line 1710 "va_parser.cpp"
    break;

  case 110: // ar_observable: HTTP_QUERY
#line 798 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
#line 1716 "va_parser.cpp"
    break;

  case 111: // ar_observable: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 799 "va_parser.y"
                                                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, yystack_[1].value.as <  string  > ()); }
#line 1722 "va_parser.cpp"
    break;

  case 112: // ar_observable: HTTP_DATA
#line 800 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, "0_FULL_QUERY"); }
#line 1728 "va_parser.cpp"
    break;

  case 113: // ar_observable: HTTP_ANSWER_CODE
#line 801 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
#line 1734 "va_parser.cpp"
    break;

  case 114: // ar_observable: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 802 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 1740 "va_parser.cpp"
    break;

  case 115: // ar_observable: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 803 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_HEADER, yystack_[1].value.as <  string  > ()); }
#line 1746 "va_parser.cpp"
    break;

  case 116: // ar_observable: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 804 "va_parser.y"
                                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, yystack_[1].value.as <  string  > ()); }
#line 1752 "va_parser.cpp"
    break;

  case 117: // ar_observable: HTTP_ANSWER_DATA
#line 805 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }
#line 1758 "va_parser.cpp"
    break;

  case 118: // ar_observable: "ident"
#line 807 "va_parser.y"
                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_IDENT, yystack_[0].value.as <  string  > ());  }
#line 1764 "va_parser.cpp"
    break;

  case 119: // ar_observable: "string"
#line 808 "va_parser.y"
                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_STRING, yystack_[0].value.as <  string  > ()); }
#line 1770 "va_parser.cpp"
    break;

  case 120: // ar_observable: "time"
#line 809 "va_parser.y"
                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_TIME, "");   }
#line 1776 "va_parser.cpp"
    break;

  case 121: // ar_observables_list: ar_observable pattern_extractor
#line 812 "va_parser.y"
                                                        {yylhs.value.as <  list< Command >  > () = list<Command>(); auto a = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(a);}
#line 1782 "va_parser.cpp"
    break;

  case 122: // ar_observables_list: ar_observables_list "comma" ar_observable pattern_extractor
#line 814 "va_parser.y"
                                                                                  {yylhs.value.as <  list< Command >  > ()=yystack_[3].value.as <  list< Command >  > (); auto a = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ());  yylhs.value.as <  list< Command >  > ().push_back(a);}
#line 1788 "va_parser.cpp"
    break;

  case 123: // ar_cat_observables: CAT "leftgrpar" ar_observables_list "rightsgrpar"
#line 817 "va_parser.y"
                                                                     {yylhs.value.as <  Command  > ()=Command(token::TOKEN_CAT, "", yystack_[1].value.as <  list< Command >  > ());}
#line 1794 "va_parser.cpp"
    break;

  case 124: // lista_when: item_when
#line 820 "va_parser.y"
                         { yylhs.value.as <  list< Command >  > ()=list<Command>(); yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ()); }
#line 1800 "va_parser.cpp"
    break;

  case 125: // lista_when: lista_when item_when
#line 822 "va_parser.y"
                                     { yylhs.value.as <  list< Command >  > ()=yystack_[1].value.as <  list< Command >  > (); yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ()); }
#line 1806 "va_parser.cpp"
    break;

  case 126: // lista_when: %empty
#line 824 "va_parser.y"
                       { yylhs.value.as <  list< Command >  > () = list<Command>(); }
#line 1812 "va_parser.cpp"
    break;

  case 127: // pattern_extractor: "string" "string"
#line 827 "va_parser.y"
                                  { yylhs.value.as <  string  > ()=Base64::encode(yystack_[1].value.as <  string  > ()) + ":" + Base64::encode(yystack_[0].value.as <  string  > ()); }
#line 1818 "va_parser.cpp"
    break;

  case 128: // pattern_extractor: %empty
#line 829 "va_parser.y"
                                      { yylhs.value.as <  string  > ()=string(); }
#line 1824 "va_parser.cpp"
    break;

  case 129: // item_when: "!" basic_item_when
#line 833 "va_parser.y"
                            {auto negate = Command(token::TOKEN_NOT, "NOT"); yylhs.value.as <  Command  > ()=yystack_[0].value.as <  Command  > (); yylhs.value.as <  Command  > ().addArg(negate);}
#line 1830 "va_parser.cpp"
    break;

  case 130: // item_when: basic_item_when
#line 835 "va_parser.y"
                        {yylhs.value.as <  Command  > ()=yystack_[0].value.as <  Command  > ();}
#line 1836 "va_parser.cpp"
    break;

  case 131: // basic_item_when: "OBS_NAME" "is" "in" ident_or_string pattern_extractor
#line 839 "va_parser.y"
                                                                 { yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1842 "va_parser.cpp"
    break;

  case 132: // basic_item_when: "OBS_NAME" "is" ident_or_string pattern_extractor
#line 841 "va_parser.y"
                                                              { yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1848 "va_parser.cpp"
    break;

  case 133: // basic_item_when: "OBS_EVENT" "is" network_event pattern_extractor
#line 843 "va_parser.y"
                                                             { yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ()); }
#line 1854 "va_parser.cpp"
    break;

  case 134: // basic_item_when: "NET_TCP_IPDST" "is" "in" ident_or_string pattern_extractor
#line 845 "va_parser.y"
                                                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1860 "va_parser.cpp"
    break;

  case 135: // basic_item_when: "NET_TCP_IPDST" "is" ip_address pattern_extractor
#line 847 "va_parser.y"
                                                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1866 "va_parser.cpp"
    break;

  case 136: // basic_item_when: HTTP_METHOD "is" "in" ident_or_string pattern_extractor
#line 849 "va_parser.y"
                                                                    { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1872 "va_parser.cpp"
    break;

  case 137: // basic_item_when: HTTP_METHOD "is" http_method pattern_extractor
#line 851 "va_parser.y"
                                                             { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1878 "va_parser.cpp"
    break;

  case 138: // basic_item_when: "HTTP HOST" "is" "in" ident_or_string pattern_extractor
#line 853 "va_parser.y"
                                                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1884 "va_parser.cpp"
    break;

  case 139: // basic_item_when: "HTTP HOST" "is" ident_or_string pattern_extractor
#line 855 "va_parser.y"
                                                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1890 "va_parser.cpp"
    break;

  case 140: // basic_item_when: "HTTP URL" "is" "in" ident_or_string pattern_extractor
#line 857 "va_parser.y"
                                                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1896 "va_parser.cpp"
    break;

  case 141: // basic_item_when: "HTTP URL" "is" ident_or_string pattern_extractor
#line 859 "va_parser.y"
                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1902 "va_parser.cpp"
    break;

  case 142: // basic_item_when: "HTTP URI" "is" "in" ident_or_string pattern_extractor
#line 861 "va_parser.y"
                                                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1908 "va_parser.cpp"
    break;

  case 143: // basic_item_when: "HTTP URI" "is" ident_or_string pattern_extractor
#line 863 "va_parser.y"
                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1914 "va_parser.cpp"
    break;

  case 144: // basic_item_when: "agent id" "is" "in" ident_or_string pattern_extractor
#line 865 "va_parser.y"
                                                                 { yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1920 "va_parser.cpp"
    break;

  case 145: // basic_item_when: "agent id" "is" ident_or_string pattern_extractor
#line 867 "va_parser.y"
                                                              { yylhs.value.as <  Command  > ()=Command(token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1926 "va_parser.cpp"
    break;

  case 146: // basic_item_when: "HTTP host in the headers" "is" ident_or_string pattern_extractor
#line 869 "va_parser.y"
                                                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1932 "va_parser.cpp"
    break;

  case 147: // basic_item_when: "HTTP host in the headers" "is" "in" ident_or_string pattern_extractor
#line 871 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1938 "va_parser.cpp"
    break;

  case 148: // basic_item_when: "HTTP host in the url" "is" ident_or_string pattern_extractor
#line 873 "va_parser.y"
                                                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1944 "va_parser.cpp"
    break;

  case 149: // basic_item_when: "HTTP host in the url" "is" "in" ident_or_string pattern_extractor
#line 875 "va_parser.y"
                                                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1950 "va_parser.cpp"
    break;

  case 150: // basic_item_when: EXISTS HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 877 "va_parser.y"
                                                                                           { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, yystack_[2].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1956 "va_parser.cpp"
    break;

  case 151: // basic_item_when: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 879 "va_parser.y"
                                                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, yystack_[4].value.as <  string  > (), lst);if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ()); 
                                                                                }
#line 1964 "va_parser.cpp"
    break;

  case 152: // basic_item_when: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 883 "va_parser.y"
                                                                                                         { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, yystack_[5].value.as <  string  > (), lst); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 1972 "va_parser.cpp"
    break;

  case 153: // basic_item_when: EXISTS HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 887 "va_parser.y"
                                                                                           { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, yystack_[2].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 1978 "va_parser.cpp"
    break;

  case 154: // basic_item_when: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 889 "va_parser.y"
                                                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 1987 "va_parser.cpp"
    break;

  case 155: // basic_item_when: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 894 "va_parser.y"
                                                                                                         { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 1996 "va_parser.cpp"
    break;

  case 156: // basic_item_when: EXISTS HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 899 "va_parser.y"
                                                                                          { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, yystack_[2].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2002 "va_parser.cpp"
    break;

  case 157: // basic_item_when: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 901 "va_parser.y"
                                                                                                     { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2011 "va_parser.cpp"
    break;

  case 158: // basic_item_when: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 906 "va_parser.y"
                                                                                                        { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2020 "va_parser.cpp"
    break;

  case 159: // basic_item_when: HTTP_QUERY "is" ident_or_string pattern_extractor
#line 911 "va_parser.y"
                                                                { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, "0_FULL_QUERY", lst);  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2028 "va_parser.cpp"
    break;

  case 160: // basic_item_when: HTTP_QUERY "is" "in" ident_or_string pattern_extractor
#line 915 "va_parser.y"
                                                                    { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2037 "va_parser.cpp"
    break;

  case 161: // basic_item_when: EXISTS HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 920 "va_parser.y"
                                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, yystack_[2].value.as <  string  > ()); 
                                                                                            if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2044 "va_parser.cpp"
    break;

  case 162: // basic_item_when: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 923 "va_parser.y"
                                                                                                    { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2053 "va_parser.cpp"
    break;

  case 163: // basic_item_when: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 928 "va_parser.y"
                                                                                                       { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2062 "va_parser.cpp"
    break;

  case 164: // basic_item_when: HTTP_DATA "is" ident_or_string pattern_extractor
#line 933 "va_parser.y"
                                                               { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, "0_FULL_QUERY", lst);  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2070 "va_parser.cpp"
    break;

  case 165: // basic_item_when: HTTP_DATA "is" "in" ident_or_string pattern_extractor
#line 937 "va_parser.y"
                                                                  { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2079 "va_parser.cpp"
    break;

  case 166: // basic_item_when: EXISTS HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 944 "va_parser.y"
                                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, yystack_[2].value.as <  string  > ()); 
                                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                                }
#line 2087 "va_parser.cpp"
    break;

  case 167: // basic_item_when: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 948 "va_parser.y"
                                                                                                           { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2096 "va_parser.cpp"
    break;

  case 168: // basic_item_when: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 953 "va_parser.y"
                                                                                                              { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2105 "va_parser.cpp"
    break;

  case 169: // basic_item_when: HTTP_ANSWER_DATA "is" ident_or_string pattern_extractor
#line 958 "va_parser.y"
                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, "0_FULL_DATA", lst);  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2113 "va_parser.cpp"
    break;

  case 170: // basic_item_when: HTTP_ANSWER_DATA "is" "in" ident_or_string pattern_extractor
#line 962 "va_parser.y"
                                                                         { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, "0_FULL_DATA", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2122 "va_parser.cpp"
    break;

  case 171: // basic_item_when: EXISTS HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 969 "va_parser.y"
                                                                                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS, yystack_[2].value.as <  string  > ());
                                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                                    }
#line 2130 "va_parser.cpp"
    break;

  case 172: // basic_item_when: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 973 "va_parser.y"
                                                                                                             { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2139 "va_parser.cpp"
    break;

  case 173: // basic_item_when: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 978 "va_parser.y"
                                                                                                                { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2148 "va_parser.cpp"
    break;

  case 174: // basic_item_when: EXISTS HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 983 "va_parser.y"
                                                                                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS, yystack_[2].value.as <  string  > ());
                                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                                }
#line 2156 "va_parser.cpp"
    break;

  case 175: // basic_item_when: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 987 "va_parser.y"
                                                                                                             { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2165 "va_parser.cpp"
    break;

  case 176: // basic_item_when: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 992 "va_parser.y"
                                                                                                                { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2174 "va_parser.cpp"
    break;

  case 177: // basic_item_when: HTTP_ANSWER_CODE "is" ident_or_string pattern_extractor
#line 997 "va_parser.y"
                                                                                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2180 "va_parser.cpp"
    break;

  case 178: // basic_item_when: HTTP_ANSWER_CODE "is" "in" ident_or_string pattern_extractor
#line 999 "va_parser.y"
                                                                                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2186 "va_parser.cpp"
    break;

  case 179: // basic_item_when: EXISTS "OBS_TAG" "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1001 "va_parser.y"
                                                                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS, yystack_[2].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2192 "va_parser.cpp"
    break;

  case 180: // basic_item_when: "OBS_TAG" "is" ident_or_string pattern_extractor
#line 1003 "va_parser.y"
                                                             { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2198 "va_parser.cpp"
    break;

  case 181: // basic_item_when: "OBS_TAG" "is" "in" ident_or_string pattern_extractor
#line 1005 "va_parser.y"
                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2204 "va_parser.cpp"
    break;

  case 182: // basic_item_when: SA_OS "is" ident_or_string pattern_extractor
#line 1007 "va_parser.y"
                                                           { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_OS_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2210 "va_parser.cpp"
    break;

  case 183: // basic_item_when: SA_OS "is" "in" ident_or_string pattern_extractor
#line 1009 "va_parser.y"
                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2216 "va_parser.cpp"
    break;

  case 184: // basic_item_when: SA_PROCESS_NAME "is" ident_or_string pattern_extractor
#line 1011 "va_parser.y"
                                                                     { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2222 "va_parser.cpp"
    break;

  case 185: // basic_item_when: SA_PROCESS_NAME "is" "in" ident_or_string pattern_extractor
#line 1013 "va_parser.y"
                                                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2228 "va_parser.cpp"
    break;

  case 186: // basic_item_when: SA_LOGGED_USER "is" ident_or_string pattern_extractor
#line 1015 "va_parser.y"
                                                                    { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2234 "va_parser.cpp"
    break;

  case 187: // basic_item_when: SA_LOGGED_USER "is" "in" ident_or_string pattern_extractor
#line 1017 "va_parser.y"
                                                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2240 "va_parser.cpp"
    break;

  case 188: // basic_item_when: "NET_TCP_IPSRC" "is" "in" ident_or_string pattern_extractor
#line 1019 "va_parser.y"
                                                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2246 "va_parser.cpp"
    break;

  case 189: // basic_item_when: "NET_TCP_IPSRC" "is" "ip_with_star" pattern_extractor
#line 1021 "va_parser.y"
                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2252 "va_parser.cpp"
    break;

  case 190: // basic_item_when: "NET_RESULT" "is" true_false pattern_extractor
#line 1023 "va_parser.y"
                                                           { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2258 "va_parser.cpp"
    break;

  case 191: // basic_item_when: "NET_TCP_SESID" "is" ident_or_string pattern_extractor
#line 1025 "va_parser.y"
                                                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, yystack_[1].value.as <  string  > ()); if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2264 "va_parser.cpp"
    break;

  case 192: // basic_item_when: cat_observables "is" "in" ident_or_string
#line 1027 "va_parser.y"
                                                      {yylhs.value.as <  Command  > () = yystack_[3].value.as <  Command  > (); yylhs.value.as <  Command  > ().m_name = yystack_[0].value.as <  string  > ();}
#line 2270 "va_parser.cpp"
    break;

  case 193: // basic_item_when: "Calls a lisp function" ident_or_string "Parameters for a lisp function" observables_list
#line 1029 "va_parser.y"
                                                           {
                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, yystack_[2].value.as <  string  > (), yystack_[0].value.as <  list< Command >  > ());
                }
#line 2278 "va_parser.cpp"
    break;

  case 194: // basic_access_rules: item_basic_access_rule basic_access_rules
#line 1034 "va_parser.y"
                                                               {yylhs.value.as <  Command  > ()=yystack_[0].value.as <  Command  > (); yylhs.value.as <  Command  > ().addArg(yystack_[1].value.as <  Command  > ());}
#line 2284 "va_parser.cpp"
    break;

  case 195: // basic_access_rules: item_basic_access_rule
#line 1036 "va_parser.y"
                                           {yylhs.value.as <  Command  > ()=Command(token::TOKEN_BASIC_ACCESS_RULES, ""); yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  Command  > ());}
#line 2290 "va_parser.cpp"
    break;

  case 196: // item_basic_access_rule: "!" item_basic_access_rule_with_not
#line 1045 "va_parser.y"
                                            {auto negate = Command(token::TOKEN_NOT, "NOT"); yylhs.value.as <  Command  > ()=yystack_[0].value.as <  Command  > (); yylhs.value.as <  Command  > ().addArg(negate);}
#line 2296 "va_parser.cpp"
    break;

  case 197: // item_basic_access_rule: item_basic_access_rule_with_not
#line 1047 "va_parser.y"
                                        {yylhs.value.as <  Command  > ()=yystack_[0].value.as <  Command  > ();}
#line 2302 "va_parser.cpp"
    break;

  case 198: // item_basic_access_rule_with_not: "NET_TCP_IPDST" "is" ip_address pattern_extractor
#line 1052 "va_parser.y"
                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2308 "va_parser.cpp"
    break;

  case 199: // item_basic_access_rule_with_not: HTTP_METHOD "is" http_method pattern_extractor
#line 1054 "va_parser.y"
                                                            { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2314 "va_parser.cpp"
    break;

  case 200: // item_basic_access_rule_with_not: "HTTP HOST" "is" ident_or_string pattern_extractor
#line 1056 "va_parser.y"
                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2320 "va_parser.cpp"
    break;

  case 201: // item_basic_access_rule_with_not: "HTTP URL" "is" ident_or_string pattern_extractor
#line 1058 "va_parser.y"
                                                             { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2326 "va_parser.cpp"
    break;

  case 202: // item_basic_access_rule_with_not: "HTTP URI" "is" ident_or_string pattern_extractor
#line 1060 "va_parser.y"
                                                             { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2332 "va_parser.cpp"
    break;

  case 203: // item_basic_access_rule_with_not: EXISTS HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1062 "va_parser.y"
                                                                                          { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, yystack_[2].value.as <  string  > ());  
                                                                                            if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                            }
#line 2340 "va_parser.cpp"
    break;

  case 204: // item_basic_access_rule_with_not: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 1066 "va_parser.y"
                                                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2349 "va_parser.cpp"
    break;

  case 205: // item_basic_access_rule_with_not: EXISTS HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1071 "va_parser.y"
                                                                                          { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, yystack_[2].value.as <  string  > ());  
                                                                                            if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                            }
#line 2357 "va_parser.cpp"
    break;

  case 206: // item_basic_access_rule_with_not: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 1075 "va_parser.y"
                                                                                                     { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2366 "va_parser.cpp"
    break;

  case 207: // item_basic_access_rule_with_not: EXISTS HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1080 "va_parser.y"
                                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, yystack_[2].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2372 "va_parser.cpp"
    break;

  case 208: // item_basic_access_rule_with_not: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 1082 "va_parser.y"
                                                                                                     { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2381 "va_parser.cpp"
    break;

  case 209: // item_basic_access_rule_with_not: HTTP_QUERY "is" ident_or_string pattern_extractor
#line 1087 "va_parser.y"
                                                                 { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, "0_FULL_QUERY", lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2390 "va_parser.cpp"
    break;

  case 210: // item_basic_access_rule_with_not: EXISTS HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1092 "va_parser.y"
                                                                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, yystack_[2].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2396 "va_parser.cpp"
    break;

  case 211: // item_basic_access_rule_with_not: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 1094 "va_parser.y"
                                                                                                    { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2405 "va_parser.cpp"
    break;

  case 212: // item_basic_access_rule_with_not: HTTP_DATA "is" ident_or_string pattern_extractor
#line 1099 "va_parser.y"
                                                               { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, "0_FULL_QUERY", lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2414 "va_parser.cpp"
    break;

  case 213: // item_basic_access_rule_with_not: EXISTS HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" pattern_extractor
#line 1105 "va_parser.y"
                                                                                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, yystack_[2].value.as <  string  > ());  
                                                                                                if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                                }
#line 2422 "va_parser.cpp"
    break;

  case 214: // item_basic_access_rule_with_not: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" "is" ident_or_string pattern_extractor
#line 1109 "va_parser.y"
                                                                                                           { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, yystack_[4].value.as <  string  > (), lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2431 "va_parser.cpp"
    break;

  case 215: // item_basic_access_rule_with_not: HTTP_ANSWER_DATA "is" ident_or_string pattern_extractor
#line 1114 "va_parser.y"
                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, "0_FULL_DATA", lst); 
                                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                }
#line 2440 "va_parser.cpp"
    break;

  case 216: // item_basic_access_rule_with_not: "NET_TCP_SESID" "is" ident_or_string pattern_extractor
#line 1126 "va_parser.y"
                                                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2446 "va_parser.cpp"
    break;

  case 217: // item_basic_access_rule_with_not: "NET_TCP_IPSRC" "is" ip_address pattern_extractor
#line 1128 "va_parser.y"
                                                             { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2452 "va_parser.cpp"
    break;

  case 218: // item_basic_access_rule_with_not: "NET_TCP_IPDST" "is" "in" ident_or_string pattern_extractor
#line 1130 "va_parser.y"
                                                                     { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2458 "va_parser.cpp"
    break;

  case 219: // item_basic_access_rule_with_not: "HTTP HOST" "is" "in" ident_or_string pattern_extractor
#line 1132 "va_parser.y"
                                                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2464 "va_parser.cpp"
    break;

  case 220: // item_basic_access_rule_with_not: "HTTP URL" "is" "in" ident_or_string pattern_extractor
#line 1134 "va_parser.y"
                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2470 "va_parser.cpp"
    break;

  case 221: // item_basic_access_rule_with_not: "HTTP URI" "is" "in" ident_or_string pattern_extractor
#line 1136 "va_parser.y"
                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2476 "va_parser.cpp"
    break;

  case 222: // item_basic_access_rule_with_not: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 1138 "va_parser.y"
                                                                                                        { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2485 "va_parser.cpp"
    break;

  case 223: // item_basic_access_rule_with_not: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 1143 "va_parser.y"
                                                                                                        { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2494 "va_parser.cpp"
    break;

  case 224: // item_basic_access_rule_with_not: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 1148 "va_parser.y"
                                                                                                       { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2503 "va_parser.cpp"
    break;

  case 225: // item_basic_access_rule_with_not: HTTP_QUERY "is" "in" ident_or_string pattern_extractor
#line 1153 "va_parser.y"
                                                                  { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2512 "va_parser.cpp"
    break;

  case 226: // item_basic_access_rule_with_not: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 1158 "va_parser.y"
                                                                                                      { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2521 "va_parser.cpp"
    break;

  case 227: // item_basic_access_rule_with_not: HTTP_DATA "is" "in" ident_or_string pattern_extractor
#line 1163 "va_parser.y"
                                                                 { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2530 "va_parser.cpp"
    break;

  case 228: // item_basic_access_rule_with_not: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar" "is" "in" ident_or_string pattern_extractor
#line 1169 "va_parser.y"
                                                                                                             { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, yystack_[5].value.as <  string  > (), lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2539 "va_parser.cpp"
    break;

  case 229: // item_basic_access_rule_with_not: HTTP_ANSWER_DATA "is" "in" ident_or_string pattern_extractor
#line 1174 "va_parser.y"
                                                                        { list<string> lst; lst.push_back(yystack_[1].value.as <  string  > ()); 
                                                                                        yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, "0_FULL_DATA", lst); 
                                                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                                                    }
#line 2548 "va_parser.cpp"
    break;

  case 230: // item_basic_access_rule_with_not: "OBS_TAG" "is" "in" ident_or_string pattern_extractor
#line 1181 "va_parser.y"
                                                               { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2554 "va_parser.cpp"
    break;

  case 231: // item_basic_access_rule_with_not: "NET_TCP_IPSRC" "is" "in" ident_or_string pattern_extractor
#line 1183 "va_parser.y"
                                                                     { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2560 "va_parser.cpp"
    break;

  case 232: // item_basic_access_rule_with_not: "time" "is" "in" ident_or_string pattern_extractor
#line 1185 "va_parser.y"
                                                            { yylhs.value.as <  Command  > () = Command(token::TOKEN_TIME, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2566 "va_parser.cpp"
    break;

  case 233: // item_basic_access_rule_with_not: "NET_TCP_SESID" "is" "in" ident_or_string pattern_extractor
#line 1187 "va_parser.y"
                                                                     { yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN, yystack_[1].value.as <  string  > ());  if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2572 "va_parser.cpp"
    break;

  case 234: // item_basic_access_rule_with_not: ar_cat_observables "is" "in" ident_or_string
#line 1189 "va_parser.y"
                                                         {yylhs.value.as <  Command  > ()=yystack_[3].value.as <  Command  > (); yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());}
#line 2578 "va_parser.cpp"
    break;

  case 235: // item_basic_access_rule_with_not: "Calls a lisp function" ident_or_string "Parameters for a lisp function" ar_observables_list
#line 1191 "va_parser.y"
                                                              {
                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, yystack_[2].value.as <  string  > (), yystack_[0].value.as <  list< Command >  > ());
                }
#line 2586 "va_parser.cpp"
    break;

  case 236: // true_false: "true"
#line 1196 "va_parser.y"
                 { yylhs.value.as <  string  > () = "TRUE"; }
#line 2592 "va_parser.cpp"
    break;

  case 237: // true_false: "false"
#line 1198 "va_parser.y"
                  { yylhs.value.as <  string  > () = "FALSE"; }
#line 2598 "va_parser.cpp"
    break;

  case 238: // lista_action: item_action pattern_extractor
#line 1201 "va_parser.y"
                                              { yylhs.value.as <  list< Command >  > ()=list<Command>(); auto a = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(a); }
#line 2604 "va_parser.cpp"
    break;

  case 239: // lista_action: lista_action item_action pattern_extractor
#line 1203 "va_parser.y"
                                                           { yylhs.value.as <  list< Command >  > ()=yystack_[2].value.as <  list< Command >  > (); auto itac = yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) itac.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(itac); }
#line 2610 "va_parser.cpp"
    break;

  case 240: // item_action: "report" ident_or_string "leftgrpar" action_variable_list "rightsgrpar"
#line 1213 "va_parser.y"
                                                                                 { 
                                                                    yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_ACTION_VREP_CREATE, yystack_[3].value.as <  string  > (), yystack_[1].value.as <  list< Command >  > ());
                                                                }
#line 2618 "va_parser.cpp"
    break;

  case 241: // item_action: "Mysql.append" "leftpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" "leftgrpar" action_variable_list "rightsgrpar" "rightpar"
#line 1226 "va_parser.y"
            {
                yylhs.value.as <  Command  > ()=Command(token::TOKEN_MYSQL_APPEND, "");
                yylhs.value.as <  Command  > ().addArg(Command(token::TOKEN_IDENT, yystack_[13].value.as <  string  > ())); 
                yylhs.value.as <  Command  > ().addArg(Command(token::TOKEN_IDENT, yystack_[11].value.as <  string  > ())); 
                yylhs.value.as <  Command  > ().addArg(Command(token::TOKEN_IDENT, yystack_[9].value.as <  string  > ())); 
                yylhs.value.as <  Command  > ().addArg(Command(token::TOKEN_IDENT, yystack_[7].value.as <  string  > ())); 
                yylhs.value.as <  Command  > ().addArg(Command(token::TOKEN_IDENT, yystack_[5].value.as <  string  > ()));
                for (auto cmd: yystack_[2].value.as <  list< Command >  > ())
                    yylhs.value.as <  Command  > ().addArg(cmd);
            }
#line 2633 "va_parser.cpp"
    break;

  case 242: // item_action: "Enable" "validation rule" ident_or_string
#line 1237 "va_parser.y"
                                      {yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_ENABLE_VR, yystack_[0].value.as <  string  > ());}
#line 2639 "va_parser.cpp"
    break;

  case 243: // item_action: "Disable" "validation rule" ident_or_string
#line 1239 "va_parser.y"
                                       {yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_DISABLE_VR, yystack_[0].value.as <  string  > ());}
#line 2645 "va_parser.cpp"
    break;

  case 244: // item_action: "Enable" "Access rule" ident_or_string
#line 1241 "va_parser.y"
                                      {yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_ENABLE_AR, yystack_[0].value.as <  string  > ());}
#line 2651 "va_parser.cpp"
    break;

  case 245: // item_action: "Disable" "Access rule" ident_or_string
#line 1243 "va_parser.y"
                                       {yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_DISABLE_AR, yystack_[0].value.as <  string  > ());}
#line 2657 "va_parser.cpp"
    break;

  case 246: // item_action: " add " observable " to " "set" ident_or_string pattern_extractor
#line 1245 "va_parser.y"
                                                                     {list<Command> lst; 
                                                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[1].value.as <  string  > (), lst);
                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                        }
#line 2667 "va_parser.cpp"
    break;

  case 247: // item_action: " add " cat_observables " to " "set" ident_or_string
#line 1251 "va_parser.y"
                                                       {list<Command> lst; 
                                                        lst.push_back(yystack_[3].value.as <  Command  > ()); 
                                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[0].value.as <  string  > (), lst);
                                                    }
#line 2676 "va_parser.cpp"
    break;

  case 248: // item_action: " add " observable " to " "set" ident_or_string "integer number" pattern_extractor
#line 1256 "va_parser.y"
                                                                             {list<Command> lst; 
                                                        lst.push_back(yystack_[5].value.as <  Command  > ()); 
                                                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[1].value.as <  string  > ()));
                                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[2].value.as <  string  > (), lst);
                                                        if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                        }
#line 2687 "va_parser.cpp"
    break;

  case 249: // item_action: " add " cat_observables " to " "set" ident_or_string "integer number"
#line 1263 "va_parser.y"
                                                                {list<Command> lst; 
                                                        lst.push_back(yystack_[4].value.as <  Command  > ()); 
                                                        lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[0].value.as <  string  > ()));
                                                        yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETADD, yystack_[1].value.as <  string  > (), lst);
                                                        }
#line 2697 "va_parser.cpp"
    break;

  case 250: // item_action: " del " observable " from " "set" ident_or_string pattern_extractor
#line 1269 "va_parser.y"
                                                                          {  list<Command> lst; 
                                                            lst.push_back(yystack_[4].value.as <  Command  > ()); 
                                                            yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[1].value.as <  string  > (), lst);
                                                            if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                        }
#line 2707 "va_parser.cpp"
    break;

  case 251: // item_action: " del " observable " from " "set" ident_or_string "integer number" pattern_extractor
#line 1275 "va_parser.y"
                                                                                {   list<Command> lst; 
                                                                    lst.push_back(yystack_[5].value.as <  Command  > ()); 
                                                                    lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[1].value.as <  string  > ()));
                                                                    yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[2].value.as <  string  > (), lst);
                                                                    if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                        }
#line 2718 "va_parser.cpp"
    break;

  case 252: // item_action: " del " cat_observables " from " "set" ident_or_string
#line 1283 "va_parser.y"
                                                         {  list<Command> lst; 
                                                            lst.push_back(yystack_[3].value.as <  Command  > ()); 
                                                            yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[0].value.as <  string  > (), lst);
                                                        }
#line 2727 "va_parser.cpp"
    break;

  case 253: // item_action: " del " cat_observables " from " "set" ident_or_string "integer number"
#line 1288 "va_parser.y"
                                                                   {   list<Command> lst; 
                                                                    lst.push_back(yystack_[4].value.as <  Command  > ()); 
                                                                    lst.push_back(Command(token::TOKEN_I_NUMBER, yystack_[0].value.as <  string  > ()));
                                                                    yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETDEL, yystack_[1].value.as <  string  > (), lst);
                                                                    }
#line 2737 "va_parser.cpp"
    break;

  case 254: // item_action: "set" ident_or_string "@" ident_or_string "equal" observable pattern_extractor
#line 1294 "va_parser.y"
                                                                                      {
                                                            list<Command> lst;
                                                            lst.push_back(Command(token::TOKEN_STRING, yystack_[5].value.as <  string  > ()));
                                                            lst.push_back(Command(token::TOKEN_STRING, yystack_[3].value.as <  string  > ()));
                                                            lst.push_back(yystack_[1].value.as <  Command  > ());
                                                            yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETVAR, "", lst);
                                                            if (yystack_[0].value.as <  string  > ().size() > 0) yylhs.value.as <  Command  > ().addArg(yystack_[0].value.as <  string  > ());
                                                        }
#line 2750 "va_parser.cpp"
    break;

  case 255: // item_action: "set" ident_or_string "@" ident_or_string "equal" cat_observables
#line 1303 "va_parser.y"
                                                                         {
                                                            list<Command> lst;
                                                            lst.push_back(Command(token::TOKEN_STRING, yystack_[4].value.as <  string  > ()));
                                                            lst.push_back(Command(token::TOKEN_STRING, yystack_[2].value.as <  string  > ()));
                                                            lst.push_back(yystack_[0].value.as <  Command  > ());
                                                            yylhs.value.as <  Command  > ()=Command(token::TOKEN_ACTION_SETVAR, "", lst);
                                                            }
#line 2762 "va_parser.cpp"
    break;

  case 256: // item_action: "Calls a lisp function" ident_or_string "Parameters for a lisp function" observables_list
#line 1311 "va_parser.y"
                                                       {
                yylhs.value.as <  Command  > () = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, yystack_[2].value.as <  string  > (), yystack_[0].value.as <  list< Command >  > ());
            }
#line 2770 "va_parser.cpp"
    break;

  case 257: // action_variable_list: action_variable_item pattern_extractor
#line 1317 "va_parser.y"
                                                               { yylhs.value.as <  list< Command >  > () = list<Command>(); auto a=yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(a);}
#line 2776 "va_parser.cpp"
    break;

  case 258: // action_variable_list: action_variable_list "comma" action_variable_item pattern_extractor
#line 1319 "va_parser.y"
                                                                                          { yylhs.value.as <  list< Command >  > ()=yystack_[3].value.as <  list< Command >  > (); auto a=yystack_[1].value.as <  Command  > (); if (yystack_[0].value.as <  string  > ().size() > 0) a.addArg(yystack_[0].value.as <  string  > ()); yylhs.value.as <  list< Command >  > ().push_back(a); }
#line 2782 "va_parser.cpp"
    break;

  case 259: // action_variable_item: "agent id"
#line 1322 "va_parser.y"
                                 { yylhs.value.as <  Command  > ()=Command(token::TOKEN_AGENT_ID, ""); }
#line 2788 "va_parser.cpp"
    break;

  case 260: // action_variable_item: "OBS_NAME"
#line 1323 "va_parser.y"
                                  { yylhs.value.as <  Command  > ()=Command(token::TOKEN_OBS_NAME, ""); }
#line 2794 "va_parser.cpp"
    break;

  case 261: // action_variable_item: "OBS_EVENT"
#line 1324 "va_parser.y"
                                  { yylhs.value.as <  Command  > ()=Command(token::TOKEN_OBS_EVENT, ""); }
#line 2800 "va_parser.cpp"
    break;

  case 262: // action_variable_item: "NET_TCP_IPDST"
#line 1325 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPDST, ""); }
#line 2806 "va_parser.cpp"
    break;

  case 263: // action_variable_item: HTTP_METHOD
#line 1326 "va_parser.y"
                                    { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_METHOD, ""); }
#line 2812 "va_parser.cpp"
    break;

  case 264: // action_variable_item: "HTTP HOST"
#line 1327 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HOST, ""); }
#line 2818 "va_parser.cpp"
    break;

  case 265: // action_variable_item: "HTTP URL"
#line 1328 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL, ""); }
#line 2824 "va_parser.cpp"
    break;

  case 266: // action_variable_item: "HTTP URI"
#line 1329 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URI, ""); }
#line 2830 "va_parser.cpp"
    break;

  case 267: // action_variable_item: "HTTP host in the headers"
#line 1330 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
#line 2836 "va_parser.cpp"
    break;

  case 268: // action_variable_item: "HTTP host in the url"
#line 1331 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_URL_HOST, ""); }
#line 2842 "va_parser.cpp"
    break;

  case 269: // action_variable_item: HTTP_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 1332 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 2848 "va_parser.cpp"
    break;

  case 270: // action_variable_item: HTTP_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 1333 "va_parser.y"
                                                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_HEADER, yystack_[1].value.as <  string  > ()); }
#line 2854 "va_parser.cpp"
    break;

  case 271: // action_variable_item: HTTP_QUERY
#line 1334 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
#line 2860 "va_parser.cpp"
    break;

  case 272: // action_variable_item: HTTP_QUERY "leftsqpar" ident_or_string "rightsqpar"
#line 1335 "va_parser.y"
                                                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_QUERY, yystack_[1].value.as <  string  > ()); }
#line 2866 "va_parser.cpp"
    break;

  case 273: // action_variable_item: HTTP_DATA
#line 1336 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, "0_FULL_DATA"); }
#line 2872 "va_parser.cpp"
    break;

  case 274: // action_variable_item: HTTP_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 1337 "va_parser.y"
                                                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_DATA, yystack_[1].value.as <  string  > ()); }
#line 2878 "va_parser.cpp"
    break;

  case 275: // action_variable_item: HTTP_ANSWER_DATA
#line 1338 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }
#line 2884 "va_parser.cpp"
    break;

  case 276: // action_variable_item: HTTP_ANSWER_DATA "leftsqpar" ident_or_string "rightsqpar"
#line 1339 "va_parser.y"
                                                                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_DATA, yystack_[1].value.as <  string  > ()); }
#line 2890 "va_parser.cpp"
    break;

  case 277: // action_variable_item: HTTP_ANSWER_HEADER "leftsqpar" ident_or_string "rightsqpar"
#line 1340 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_HEADER, yystack_[1].value.as <  string  > ()); }
#line 2896 "va_parser.cpp"
    break;

  case 278: // action_variable_item: HTTP_ANSWER_COOKIE "leftsqpar" ident_or_string "rightsqpar"
#line 1341 "va_parser.y"
                                                                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_COOKIE, yystack_[1].value.as <  string  > ()); }
#line 2902 "va_parser.cpp"
    break;

  case 279: // action_variable_item: HTTP_ANSWER_CODE
#line 1342 "va_parser.y"
                                         { yylhs.value.as <  Command  > () = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
#line 2908 "va_parser.cpp"
    break;

  case 280: // action_variable_item: "OBS_TAG"
#line 1343 "va_parser.y"
                                { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_TAG, ""); }
#line 2914 "va_parser.cpp"
    break;

  case 281: // action_variable_item: SA_OS
#line 1344 "va_parser.y"
                              { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_OS, ""); }
#line 2920 "va_parser.cpp"
    break;

  case 282: // action_variable_item: SA_PROCESS_NAME
#line 1345 "va_parser.y"
                                        { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_PROCESS_NAME, ""); }
#line 2926 "va_parser.cpp"
    break;

  case 283: // action_variable_item: SA_LOGGED_USER
#line 1346 "va_parser.y"
                                       { yylhs.value.as <  Command  > () = Command(token::TOKEN_SA_LOGGED_USER, ""); }
#line 2932 "va_parser.cpp"
    break;

  case 284: // action_variable_item: "NET_TCP_IPSRC"
#line 1347 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
#line 2938 "va_parser.cpp"
    break;

  case 285: // action_variable_item: "NET_RESULT"
#line 1348 "va_parser.y"
                                   { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_RESULT, ""); }
#line 2944 "va_parser.cpp"
    break;

  case 286: // action_variable_item: "NET_TCP_SESID"
#line 1349 "va_parser.y"
                                      { yylhs.value.as <  Command  > () = Command(token::TOKEN_NET_TCP_SESID, ""); }
#line 2950 "va_parser.cpp"
    break;

  case 287: // action_variable_item: "OBS_TIME"
#line 1350 "va_parser.y"
                                 { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_TIME, ""); }
#line 2956 "va_parser.cpp"
    break;

  case 288: // action_variable_item: "OBS_OBSID"
#line 1351 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_OBSID, ""); }
#line 2962 "va_parser.cpp"
    break;

  case 289: // action_variable_item: "OBS_STIME"
#line 1352 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_STIME, ""); }
#line 2968 "va_parser.cpp"
    break;

  case 290: // action_variable_item: "OBS_ETIME"
#line 1353 "va_parser.y"
                                  { yylhs.value.as <  Command  > () = Command(token::TOKEN_OBS_ETIME, ""); }
#line 2974 "va_parser.cpp"
    break;

  case 291: // action_variable_item: ident_or_string
#line 1356 "va_parser.y"
                                        {yylhs.value.as <  Command  > () = Command(token::TOKEN_IDENT, yystack_[0].value.as <  string  > ());}
#line 2980 "va_parser.cpp"
    break;

  case 292: // action_variable_item: cat_observables
#line 1358 "va_parser.y"
                                        {yylhs.value.as <  Command  > () = yystack_[0].value.as <  Command  > ();}
#line 2986 "va_parser.cpp"
    break;

  case 293: // http_method: HTTP_METHOD_GET
#line 1362 "va_parser.y"
                                {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 2992 "va_parser.cpp"
    break;

  case 294: // http_method: HTTP_METHOD_POST
#line 1364 "va_parser.y"
                                 {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 2998 "va_parser.cpp"
    break;

  case 295: // http_method: HTTP_METHOD_HEAD
#line 1366 "va_parser.y"
                                 {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3004 "va_parser.cpp"
    break;

  case 296: // http_method: HTTP_METHOD_PUT
#line 1368 "va_parser.y"
                                {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3010 "va_parser.cpp"
    break;

  case 297: // http_method: HTTP_METHOD_DELETE
#line 1370 "va_parser.y"
                                   {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3016 "va_parser.cpp"
    break;

  case 298: // http_method: HTTP_METHOD_TRACE
#line 1372 "va_parser.y"
                                  {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3022 "va_parser.cpp"
    break;

  case 299: // http_method: HTTP_METHOD_OPTIONS
#line 1374 "va_parser.y"
                                    {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3028 "va_parser.cpp"
    break;

  case 300: // http_method: HTTP_METHOD_CONNECT
#line 1376 "va_parser.y"
                                    {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3034 "va_parser.cpp"
    break;

  case 301: // http_method: HTTP_METHOD_PATCH
#line 1378 "va_parser.y"
                                  {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ().substr(5);}
#line 3040 "va_parser.cpp"
    break;

  case 302: // set_definition: "define" "dictionary" ident_or_string "equal" "leftgrpar" ident_or_string_pairs_list "rightsgrpar" "semicolon"
#line 1383 "va_parser.y"
            {
                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, yystack_[5].value.as <  string  > (), yystack_[2].value.as <  list< pair<string, string> >  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3049 "va_parser.cpp"
    break;

  case 303: // set_definition: "define" "dictionary" ident_or_string "equal" "leftgrpar" "rightsgrpar" "semicolon"
#line 1389 "va_parser.y"
            {
                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, yystack_[4].value.as <  string  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3058 "va_parser.cpp"
    break;

  case 304: // set_definition: "define" "dictionary" ident_or_string "equal" "mysql" "leftgrpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "rightsgrpar" "semicolon"
#line 1401 "va_parser.y"
            {
                list<string> lst;
                lst.push_back(yystack_[10].value.as <  string  > ());
                lst.push_back(yystack_[8].value.as <  string  > ());
                lst.push_back(yystack_[6].value.as <  string  > ());
                lst.push_back(yystack_[4].value.as <  string  > ());
                lst.push_back(yystack_[2].value.as <  string  > ());
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, yystack_[14].value.as <  string  > (), lc);
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3076 "va_parser.cpp"
    break;

  case 305: // set_definition: "define" "set" ident_or_string "equal" "leftgrpar" ident_or_string_list "rightsgrpar" "semicolon"
#line 1416 "va_parser.y"
            {
                Command cmd(token::TOKEN_SET_LIST_ID, yystack_[5].value.as <  string  > (), yystack_[2].value.as <  list< string >  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3085 "va_parser.cpp"
    break;

  case 306: // set_definition: "define" "set" ident_or_string "equal" "leftgrpar" "rightsgrpar" "semicolon"
#line 1422 "va_parser.y"
            {
                Command cmd(token::TOKEN_SET_LIST_ID, yystack_[4].value.as <  string  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3094 "va_parser.cpp"
    break;

  case 307: // set_definition: "define" "set" ident_or_string "equal" "mysql" "leftgrpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "rightsgrpar" "semicolon"
#line 1434 "va_parser.y"
            {
                list<string> lst;
                lst.push_back(yystack_[10].value.as <  string  > ());
                lst.push_back(yystack_[8].value.as <  string  > ());
                lst.push_back(yystack_[6].value.as <  string  > ());
                lst.push_back(yystack_[4].value.as <  string  > ());
                lst.push_back(yystack_[2].value.as <  string  > ());
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_ID, yystack_[14].value.as <  string  > (), lc);
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3112 "va_parser.cpp"
    break;

  case 308: // set_definition: "define" "ipset" ident_or_string "equal" "leftgrpar" ip_with_star_list "rightsgrpar" "semicolon"
#line 1466 "va_parser.y"
            {
                Command cmd(token::TOKEN_SET_LIST_IPSTAR, yystack_[5].value.as <  string  > (), yystack_[2].value.as <  list< string >  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3121 "va_parser.cpp"
    break;

  case 309: // set_definition: "define" "ipset" ident_or_string "equal" "mysql" "leftgrpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "rightsgrpar" "semicolon"
#line 1478 "va_parser.y"
            {
                list<string> lst;
                lst.push_back(yystack_[10].value.as <  string  > ());
                lst.push_back(yystack_[8].value.as <  string  > ());
                lst.push_back(yystack_[6].value.as <  string  > ());
                lst.push_back(yystack_[4].value.as <  string  > ());
                lst.push_back(yystack_[2].value.as <  string  > ());
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_IPSTAR, yystack_[14].value.as <  string  > (), lc);
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3139 "va_parser.cpp"
    break;

  case 310: // set_definition: "define" "timeset" ident_or_string "equal" "leftgrpar" timing_list "rightsgrpar" "semicolon"
#line 1493 "va_parser.y"
            {
                Command cmd(token::TOKEN_SET_LIST_TIMING, yystack_[5].value.as <  string  > (), yystack_[2].value.as <  list< string >  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3148 "va_parser.cpp"
    break;

  case 311: // set_definition: "define" "timeset" ident_or_string "equal" "mysql" "leftgrpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "rightsgrpar" "semicolon"
#line 1505 "va_parser.y"
            {
                list<string> lst;
                lst.push_back(yystack_[10].value.as <  string  > ());
                lst.push_back(yystack_[8].value.as <  string  > ());
                lst.push_back(yystack_[6].value.as <  string  > ());
                lst.push_back(yystack_[4].value.as <  string  > ());
                lst.push_back(yystack_[2].value.as <  string  > ());
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_TIMING, yystack_[14].value.as <  string  > (), lc);
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3166 "va_parser.cpp"
    break;

  case 312: // set_definition: "define" "urlset" ident_or_string "equal" "leftgrpar" url_with_star_list "rightsgrpar" "semicolon"
#line 1520 "va_parser.y"
            {
                Command cmd(token::TOKEN_SET_LIST_URLSTAR, yystack_[5].value.as <  string  > (), yystack_[2].value.as <  list< string >  > ());
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3175 "va_parser.cpp"
    break;

  case 313: // set_definition: "define" "urlset" ident_or_string "equal" "mysql" "leftgrpar" ip_or_domain "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "comma" ident_or_string "rightsgrpar" "semicolon"
#line 1532 "va_parser.y"
            {
                list<string> lst;
                lst.push_back(yystack_[10].value.as <  string  > ());
                lst.push_back(yystack_[8].value.as <  string  > ());
                lst.push_back(yystack_[6].value.as <  string  > ());
                lst.push_back(yystack_[4].value.as <  string  > ());
                lst.push_back(yystack_[2].value.as <  string  > ());
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_URLSTAR, yystack_[14].value.as <  string  > (), lc);
                yylhs.value.as <  Command  > ()=cmd;
            }
#line 3193 "va_parser.cpp"
    break;

  case 314: // check_or_not_the_server_cert: %empty
#line 1553 "va_parser.y"
                                     { yylhs.value.as <  bool  > () = false; }
#line 3199 "va_parser.cpp"
    break;

  case 315: // check_or_not_the_server_cert: "leftpar" 'T' "rightpar"
#line 1555 "va_parser.y"
                                                     { yylhs.value.as <  bool  > () = true; }
#line 3205 "va_parser.cpp"
    break;

  case 316: // check_or_not_the_server_cert: "leftpar" 't' "rightpar"
#line 1557 "va_parser.y"
                                                     { yylhs.value.as <  bool  > () = true; }
#line 3211 "va_parser.cpp"
    break;

  case 317: // check_or_not_the_server_cert: "leftpar" 'F' "rightpar"
#line 1559 "va_parser.y"
                                                     { yylhs.value.as <  bool  > () = false; }
#line 3217 "va_parser.cpp"
    break;

  case 318: // check_or_not_the_server_cert: "leftpar" 'f' "rightpar"
#line 1561 "va_parser.y"
                                                     { yylhs.value.as <  bool  > () = false; }
#line 3223 "va_parser.cpp"
    break;

  case 319: // ssl_server_options: %empty
#line 1564 "va_parser.y"
                           { yylhs.value.as <  list< string >  > () = list<string>(); }
#line 3229 "va_parser.cpp"
    break;

  case 320: // ssl_server_options: "leftsqpar" "string" "comma" "string" "comma" "string" "comma" "string" check_or_not_the_server_cert "rightsqpar"
#line 1566 "va_parser.y"
                                                                                                             { //server_ca server_hostname client_cert client_key
                auto ls = list<string>();
                ls.push_back(yystack_[8].value.as <  string  > ());
                ls.push_back(yystack_[6].value.as <  string  > ());
                ls.push_back(yystack_[4].value.as <  string  > ());
                ls.push_back(yystack_[2].value.as <  string  > ());
                ls.push_back(yystack_[1].value.as <  bool  > ()?"CHECK":"DONT CHECK");
                yylhs.value.as <  list< string >  > ()=ls;
            }
#line 3243 "va_parser.cpp"
    break;

  case 321: // ssl_server_options: "leftsqpar" "string" "comma" "string" check_or_not_the_server_cert "rightsqpar"
#line 1576 "va_parser.y"
                                                                                  { //server_ca server_hostname
                auto ls = list<string>();
                ls.push_back(yystack_[4].value.as <  string  > ());
                ls.push_back(yystack_[2].value.as <  string  > ());
                ls.push_back(yystack_[1].value.as <  bool  > ()?"CHECK":"DONT CHECK");
                yylhs.value.as <  list< string >  > ()=ls;
            }
#line 3255 "va_parser.cpp"
    break;

  case 322: // ssl_server_options: "leftsqpar" "string" check_or_not_the_server_cert "rightsqpar"
#line 1584 "va_parser.y"
                                                                     { //server_ca
                auto ls = list<string>();
                ls.push_back(yystack_[2].value.as <  string  > ());
                ls.push_back(yystack_[1].value.as <  bool  > ()?"CHECK":"DONT CHECK");
                yylhs.value.as <  list< string >  > ()=ls;
            }
#line 3266 "va_parser.cpp"
    break;

  case 323: // network_event: "NET_TCP_CONNECT"
#line 1594 "va_parser.y"
                                {yylhs.value.as <  string  > ()=string("NET_TCP_CONNECT");}
#line 3272 "va_parser.cpp"
    break;

  case 324: // network_event: "NET_TCP_ACCEPT"
#line 1596 "va_parser.y"
                               {yylhs.value.as <  string  > ()=string("NET_TCP_ACCEPT");}
#line 3278 "va_parser.cpp"
    break;

  case 325: // network_event: "NET_TCP_SEND"
#line 1598 "va_parser.y"
                             {yylhs.value.as <  string  > ()=string("NET_TCP_SEND");}
#line 3284 "va_parser.cpp"
    break;

  case 326: // network_event: "NET_TCP_RECEIVE"
#line 1600 "va_parser.y"
                             {yylhs.value.as <  string  > ()=string("NET_TCP_RECV");}
#line 3290 "va_parser.cpp"
    break;

  case 327: // network_event: "NET_TCP_CLOSE"
#line 1602 "va_parser.y"
                              {yylhs.value.as <  string  > ()=string("NET_TCP_CLOSE");}
#line 3296 "va_parser.cpp"
    break;

  case 328: // timing_list: "timing"
#line 1605 "va_parser.y"
                    {yylhs.value.as <  list< string >  > () = list<string>(); yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ()); }
#line 3302 "va_parser.cpp"
    break;

  case 329: // timing_list: timing_list "comma" "timing"
#line 1607 "va_parser.y"
                                     {yylhs.value.as <  list< string >  > ()=yystack_[2].value.as <  list< string >  > (); yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());}
#line 3308 "va_parser.cpp"
    break;

  case 330: // ip_with_star_list: ip_address
#line 1610 "va_parser.y"
                                  {
                                        yylhs.value.as <  list< string >  > () = list<string>();
                                        yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                                    }
#line 3317 "va_parser.cpp"
    break;

  case 331: // ip_with_star_list: ip_with_star_list "comma" ip_address
#line 1616 "va_parser.y"
                                    {
                                        yylhs.value.as <  list< string >  > ()=yystack_[2].value.as <  list< string >  > ();
                                        yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                    }
#line 3326 "va_parser.cpp"
    break;

  case 332: // url_with_star_list: "url_with_star"
#line 1622 "va_parser.y"
                                     {
                                        yylhs.value.as <  list< string >  > () = list<string>();
                                        yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                                    }
#line 3335 "va_parser.cpp"
    break;

  case 333: // url_with_star_list: url_with_star_list "comma" "url_with_star"
#line 1628 "va_parser.y"
                                    {
                                        yylhs.value.as <  list< string >  > ()=yystack_[2].value.as <  list< string >  > ();
                                        yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                                    }
#line 3344 "va_parser.cpp"
    break;

  case 334: // ident_or_string_list: ident_or_string
#line 1635 "va_parser.y"
                                        {
                                            yylhs.value.as <  list< string >  > () = list<string>();
                                            yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                                        }
#line 3353 "va_parser.cpp"
    break;

  case 335: // ident_or_string_list: ident_or_string_list "comma" ident_or_string
#line 1641 "va_parser.y"
                                        {
                                            yylhs.value.as <  list< string >  > ()=yystack_[2].value.as <  list< string >  > ();
                                            yylhs.value.as <  list< string >  > ().push_back(yystack_[0].value.as <  string  > ());
                                        }
#line 3362 "va_parser.cpp"
    break;

  case 336: // ident_or_string_pairs_list: "leftpar" ident_or_string "comma" ident_or_string "rightpar"
#line 1648 "va_parser.y"
                                                                                   {
                                            yylhs.value.as <  list< pair<string, string> >  > () = list<pair<string, string>>();
                                            yylhs.value.as <  list< pair<string, string> >  > ().push_back(make_pair(yystack_[3].value.as <  string  > (), yystack_[1].value.as <  string  > ()));
                                        }
#line 3371 "va_parser.cpp"
    break;

  case 337: // ident_or_string_pairs_list: ident_or_string_pairs_list "comma" "leftpar" ident_or_string "comma" ident_or_string "rightpar"
#line 1654 "va_parser.y"
                                        {
                                            yylhs.value.as <  list< pair<string, string> >  > ()=yystack_[6].value.as <  list< pair<string, string> >  > ();
                                            yylhs.value.as <  list< pair<string, string> >  > ().push_back(make_pair(yystack_[3].value.as <  string  > (), yystack_[1].value.as <  string  > ()));
                                        }
#line 3380 "va_parser.cpp"
    break;

  case 338: // observable_list: observable
#line 1662 "va_parser.y"
                            {
                                yylhs.value.as <  list< Command >  > () = list<Command>();
                                yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ());
                            }
#line 3389 "va_parser.cpp"
    break;

  case 339: // observable_list: observable_list "comma" observable
#line 1668 "va_parser.y"
                        {
                            yylhs.value.as <  list< Command >  > ()=yystack_[2].value.as <  list< Command >  > ();
                            yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ());
                        }
#line 3398 "va_parser.cpp"
    break;

  case 340: // observable_assign_list: ident_or_string "equal" observable
#line 1674 "va_parser.y"
                                                         {
                                            yylhs.value.as <  list< Command >  > () = list<Command>();
                                            yylhs.value.as <  list< Command >  > ().push_back(Command(token::TOKEN_STRING, yystack_[2].value.as <  string  > ()));
                                            yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ());
                                        }
#line 3408 "va_parser.cpp"
    break;

  case 341: // observable_assign_list: observable_assign_list "comma" ident_or_string "equal" observable
#line 1681 "va_parser.y"
                                        {
                                            yylhs.value.as <  list< Command >  > ()=yystack_[4].value.as <  list< Command >  > ();
                                            yylhs.value.as <  list< Command >  > ().push_back(Command(token::TOKEN_STRING, yystack_[2].value.as <  string  > ()));
                                            yylhs.value.as <  list< Command >  > ().push_back(yystack_[0].value.as <  Command  > ());
                                        }
#line 3418 "va_parser.cpp"
    break;

  case 342: // ident_or_string: "ident"
#line 1689 "va_parser.y"
                            { yylhs.value.as <  string  > () = yystack_[0].value.as <  string  > (); }
#line 3424 "va_parser.cpp"
    break;

  case 343: // ident_or_string: "string"
#line 1691 "va_parser.y"
                             { yylhs.value.as <  string  > () = yystack_[0].value.as <  string  > (); }
#line 3430 "va_parser.cpp"
    break;

  case 344: // ip_address: "ip_with_star"
#line 1694 "va_parser.y"
                         {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ();}
#line 3436 "va_parser.cpp"
    break;

  case 345: // ip_address: "ip_without_star"
#line 1696 "va_parser.y"
                             {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ();}
#line 3442 "va_parser.cpp"
    break;

  case 346: // ip_or_domain: ip_address
#line 1699 "va_parser.y"
                           {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ();}
#line 3448 "va_parser.cpp"
    break;

  case 347: // ip_or_domain: ident_or_string
#line 1701 "va_parser.y"
                                {yylhs.value.as <  string  > ()=yystack_[0].value.as <  string  > ();}
#line 3454 "va_parser.cpp"
    break;


#line 3458 "va_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
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


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
   Parser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   Parser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
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
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
   Parser ::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  //  Parser ::context.
   Parser ::context::context (const  Parser & yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
   Parser ::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
   Parser ::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
   Parser ::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short  Parser ::yypact_ninf_ = -870;

  const short  Parser ::yytable_ninf_ = -344;

  const short
   Parser ::yypact_[] =
  {
      48,  -870,    32,  -870,   -20,  -870,  -870,  -870,    62,    62,
      62,    62,    62,    62,    62,  -870,  -870,     7,    43,    97,
     147,   240,   -15,    17,    41,   193,   213,   215,   217,  1443,
    1318,   182,    59,   271,    11,   303,   250,   305,   307,   318,
     328,   342,  1443,  1593,   268,   281,   290,   296,   321,   322,
      62,   371,   324,   338,   339,   340,   344,   346,   347,   349,
     386,   387,    40,    42,    44,   350,   400,   405,   353,   381,
     382,   377,    50,   365,   327,  -870,   383,  1535,  -870,  -870,
     384,  1228,   396,   397,   398,    62,   418,   399,   406,   407,
     408,   409,   468,   473,   168,   190,   227,   202,   411,  -870,
    1318,  -870,    54,   482,    36,  -870,   393,    62,   483,    38,
     393,  -870,  -870,   231,  -870,   393,  -870,   234,   393,  -870,
     283,   393,   224,  -870,    37,    46,    64,    66,    72,    82,
     403,   961,   332,    12,    28,    62,   337,    84,   100,   319,
      62,    62,    62,   103,    62,   157,    62,   159,   169,    62,
      62,   179,   183,   186,   484,   485,   486,   488,   489,   490,
     491,   492,  -870,  1443,  1443,  1443,   414,    62,   432,  -870,
     438,  -870,   199,   201,   206,   429,   352,    30,    65,   210,
     447,   292,    62,    62,    62,   214,    62,   216,    62,   218,
     517,   518,   519,   520,   524,   459,  -870,   466,   466,    18,
    1138,    62,    62,   366,    62,    62,    62,    62,    62,   529,
     532,   533,   534,   537,    54,  -870,    62,   544,  -870,  -870,
     545,   546,  -870,   547,   548,   549,   250,   551,   552,   554,
     559,   553,   566,   563,   564,  -870,    62,   574,    62,   574,
      62,   574,    62,   574,    62,   574,    62,   574,   961,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,   565,
     567,   568,   569,   570,  -870,   571,   572,  -870,  -870,  -870,
     574,   298,  -870,  -870,  -870,  -870,  -870,   574,   574,    62,
      62,   574,   574,  -870,  -870,   574,    62,   574,    62,   574,
      62,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,
     574,   573,   577,   578,    62,   574,   579,    62,   574,   580,
      62,   574,    62,   574,   581,   592,    62,   574,    62,   574,
      62,   574,    62,    62,    62,    62,    62,    62,    62,    62,
     365,   327,  -870,    62,  -870,   602,     8,    62,    62,   574,
      62,   574,    62,   574,   352,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,   575,
     599,   605,   606,   608,  -870,   609,   610,   574,   300,    62,
     574,    62,   574,    62,   574,    62,   574,   601,   612,   623,
      62,   574,   628,    62,   574,   629,    62,   574,    62,    62,
      62,    62,    62,    62,   555,   562,   619,   620,   633,   196,
     639,   640,  -870,   637,   638,   643,   644,   645,  -870,   646,
     647,   648,    62,    62,   576,    62,    62,    62,    62,  -870,
    -870,  -870,  -870,    62,    62,    62,  -870,    62,  -870,  -870,
      62,  -870,  -870,    62,  -870,  -870,    62,   574,   655,  -870,
     574,  -870,   574,  -870,   574,  -870,   574,  -870,   574,  -870,
     652,    62,    62,    62,    62,    62,    62,    62,  -870,   961,
    -870,  -870,  -870,   574,   574,  -870,  -870,  -870,   574,  -870,
     574,  -870,   574,  -870,   496,   585,   588,   574,  -870,   589,
     574,  -870,   590,   574,  -870,   574,  -870,   591,   597,   574,
    -870,   574,  -870,   574,  -870,   659,   665,   666,   667,   668,
     669,   670,   671,  -870,   466,   466,   466,    62,    62,    62,
     -47,   -33,   649,     8,   574,   574,   574,  -870,   574,  -870,
     574,  -870,   676,    62,    62,    62,    62,    62,    62,    62,
    -870,   352,  -870,   574,  -870,   574,  -870,   574,  -870,   574,
    -870,   611,   613,   615,   574,  -870,   617,   574,  -870,   624,
     574,  -870,   675,   690,   692,   694,   695,  -870,   630,   650,
     651,   653,   656,  -870,    23,  -870,   961,    62,   961,    62,
      62,   704,    62,    62,  -870,  -870,   352,   701,    78,   228,
     230,   705,   709,   711,   714,   715,   716,   717,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,   707,   718,   721,   722,   723,
     725,   728,   574,  -870,  -870,  -870,  -870,  -870,   222,   252,
     256,  -870,   258,  -870,   263,  -870,  -870,   266,   269,  -870,
    -870,  -870,   574,   574,   574,   574,   574,   574,   574,   574,
     574,    62,   635,   706,   719,   720,   674,   731,   657,    62,
      62,    62,    62,   393,   574,  -870,  -870,  -870,  -870,  -870,
     729,   733,   734,   736,   737,   738,   739,   574,  -870,  -870,
    -870,  -870,   272,   274,   276,  -870,   293,  -870,   301,  -870,
     574,   574,   574,   574,   574,    62,    62,    62,    62,   364,
     633,   633,  -870,   747,  -870,  -870,  -870,  -870,  -870,   744,
     676,   750,    62,   752,    62,   754,    62,   756,    62,  -870,
      62,    62,    62,    62,    62,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,    62,   574,    62,   574,    62,   574,    62,
     574,    62,   574,    62,   574,    62,   574,  -870,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,    62,  -870,   708,   724,   726,
     730,    62,   686,   961,  -870,  -870,  -870,  -870,   755,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,    62,   574,
      62,   574,    62,   574,    62,   574,    62,   574,  -870,  -870,
    -870,  -870,  -870,    77,   763,   164,   766,   144,   774,   762,
    -870,  -870,   961,    62,    62,   764,    62,   765,    62,   767,
      62,   768,   778,   779,   780,   781,   782,   574,  -870,   574,
    -870,   574,  -870,   574,  -870,   574,  -870,   574,  -870,   574,
    -870,  -870,    62,    62,    62,    62,   769,  -870,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,  -870,  -870,   783,   784,   785,   786,   787,
    -870,   788,   789,  -870,  -870,  -870,  -870,   334,   574,  -870,
     652,    62,   574,  -870,   574,  -870,   574,  -870,   574,  -870,
     574,  -870,   574,  -870,  -870,   574,  -870,  -870,   793,   794,
     795,   796,   369,  -870,  -870,  -870,  -870,   799,  -870,   800,
    -870,   807,  -870,    62,  -870,    62,    62,    62,    62,  -870,
    -870,  -870,  -870,  -870,  -870,  -870,   223,   792,   304,   823,
     466,    62,    62,    62,    62,    62,    62,    62,   686,  -870,
    -870,   816,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,   828,   817,    62,    62,    62,   818,   820,
     821,   824,   825,   574,  -870,  -870,   574,  -870,  -870,   574,
    -870,   826,   827,   829,   832,   843,   844,   845,   574,    62,
     830,  -870,   831,   852,   853,    62,    62,    62,    62,    62,
    -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,  -870,
    -870,   850,   834,    62,    62,    62,   822,   847,   848,   849,
     851,    62,   857,  -870,  -870,  -870,   858,   859,   860,   861,
     862,   856,   863,  -870,  -870,  -870,  -870,  -870,    62,  -870,
     864,   865,   686,   343,   868,  -870
  };

  const short
   Parser ::yydefact_[] =
  {
       2,     6,     0,     1,     0,     5,     4,     3,     0,     0,
       0,     0,     0,     0,     0,   342,   343,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   126,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    48,    50,     0,    55,   124,   130,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
     195,   197,     9,     0,     0,   334,     0,     0,     0,     0,
       0,   344,   345,     0,   330,     0,   332,     0,     0,   328,
       0,     0,     0,   129,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,   126,   126,   126,     0,    58,    53,   125,
       0,   196,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,   306,     0,     0,   347,   346,
       0,     0,   303,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,     0,   128,     0,   128,
       0,   128,     0,   128,     0,   128,     0,   128,     0,    91,
      92,    59,    75,    76,    77,    79,    78,    69,    71,    70,
      73,    72,    63,    64,    65,    66,    67,    68,    74,     0,
       0,    83,    85,    90,    86,     0,     0,    60,    61,    62,
     128,     0,   323,   324,   325,   326,   327,   128,   128,     0,
       0,   128,   128,   236,   237,   128,     0,   128,     0,   128,
       0,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     128,     0,     0,     0,     0,   128,     0,     0,   128,     0,
       0,   128,     0,   128,     0,     0,     0,   128,     0,   128,
       0,   128,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    47,    49,     0,    54,     0,     0,     0,     0,   128,
       0,   128,     0,   128,     0,   118,   119,   120,   102,   103,
     104,   106,   105,    98,    97,   100,    99,    96,   101,     0,
       0,   110,   112,   117,   113,     0,     0,   128,     0,     0,
     128,     0,   128,     0,   128,     0,   128,     0,     0,     0,
       0,   128,     0,     0,   128,     0,     0,   128,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   319,   319,
      91,    92,   338,    39,    41,     0,    40,    34,    36,     0,
      38,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,   335,   305,     0,     0,     0,   302,     0,   331,   308,
       0,   333,   312,     0,   329,   310,     0,   128,     0,   145,
     128,   139,   128,   141,   128,   143,   128,   148,   128,   146,
     193,     0,     0,     0,     0,     0,     0,     0,    93,     0,
      95,   133,   189,   128,   128,   135,   191,   190,   128,   132,
     128,   180,   128,   137,     0,     0,     0,   128,   159,     0,
     128,   164,     0,   128,   169,   128,   177,     0,     0,   128,
     182,   128,   186,   128,   184,     0,     0,     0,     0,     0,
       0,     0,     0,   192,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,   128,   128,   128,   200,   128,   201,
     128,   202,   235,     0,     0,     0,     0,     0,     0,     0,
     121,     0,   123,   128,   217,   128,   198,   128,   216,   128,
     199,     0,     0,     0,   128,   209,     0,   128,   212,     0,
     128,   215,     0,     0,     0,     0,     0,   234,     0,     0,
       0,     0,     0,    32,     0,    30,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   144,   127,
     138,   140,   142,   149,   147,     0,     0,     0,     0,     0,
       0,     0,   128,   188,   134,   131,   181,   136,     0,     0,
       0,   160,     0,   165,     0,   170,   178,     0,     0,   183,
     187,   185,   128,   128,   128,   128,   128,   128,   128,   128,
     128,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,   238,   232,   219,   220,   221,
       0,     0,     0,     0,     0,     0,     0,   128,   231,   218,
     233,   230,     0,     0,     0,   225,     0,   227,     0,   229,
     128,   128,   128,   128,   128,     0,     0,     0,     0,   314,
     319,   319,   339,     0,   340,    42,    33,    35,    37,     0,
      28,     0,     0,     0,     0,     0,     0,     0,     0,   336,
       0,     0,     0,     0,     0,    80,    81,    82,    84,    89,
      88,    87,    94,     0,   128,     0,   128,     0,   128,     0,
     128,     0,   128,     0,   128,     0,   128,   179,   150,   153,
     156,   161,   166,   171,   174,    58,    57,     0,     0,     0,
       0,     0,     0,     0,   242,   244,   243,   245,     0,   239,
     107,   108,   109,   111,   116,   115,   114,   122,     0,   128,
       0,   128,     0,   128,     0,   128,     0,   128,   203,   205,
     207,   210,   213,   128,    21,   128,    26,     0,     0,     0,
      31,    29,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   151,   128,
     154,   128,   157,   128,   162,   128,   167,   128,   172,   128,
     175,    56,     0,     0,     0,     0,     0,   259,   264,   265,
     266,   268,   267,   261,   284,   262,   286,   285,   260,   287,
     288,   289,   290,   280,   263,     0,     0,   271,   273,   275,
     279,     0,     0,   281,   283,   282,   292,     0,   128,   291,
     256,     0,   128,   204,   128,   206,   128,   208,   128,   211,
     128,   214,   128,    20,    23,   128,    24,    27,     0,     0,
       0,     0,   314,   322,   341,    10,    17,     0,    13,     0,
      14,     0,    18,     0,   337,     0,     0,     0,     0,   152,
     155,   158,   163,   168,   173,   176,   128,   247,   128,   252,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   240,
     257,     0,   222,   223,   224,   226,   228,    22,    25,   315,
     316,   317,   318,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,   246,   249,   128,   250,   253,   128,
     255,     0,     0,     0,     0,     0,     0,     0,   128,     0,
       0,   321,     0,     0,     0,     0,     0,     0,     0,     0,
     248,   251,   254,   269,   270,   272,   274,   276,   277,   278,
     258,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   314,    15,    16,    19,     0,     0,     0,     0,
       0,     0,     0,   307,   304,   309,   313,   311,     0,   320,
       0,     0,     0,     0,     0,   241
  };

  const short
   Parser ::yypgoto_[] =
  {
    -870,  -870,  -870,   663,  -870,  -870,  -870,   836,   732,   727,
     735,  -870,  -631,  -169,  -241,  -182,   341,  -346,  -870,  -870,
     693,   809,   841,   797,  -870,   811,  -870,  -870,   370,  -118,
     -22,   740,  -870,  -869,  -397,  -870,  -870,  -870,  -870,  -870,
    -870,  -870,  -870,    -8,   -25,  -101
  };

  const short
   Parser ::yydefgoto_[] =
  {
       0,     2,     5,   213,   214,    31,     6,    72,    73,    74,
      75,   168,   344,   280,   281,    76,   377,   378,    98,    77,
     449,    78,    79,    99,   100,   101,   295,   523,   524,   857,
     858,   310,     7,   789,   573,   287,   120,   113,   117,   104,
     109,   413,   414,   218,   219,   220
  };

  const short
   Parser ::yytable_[] =
  {
      17,    18,    19,    20,    21,    22,    23,   460,   532,   225,
     746,   114,   575,   934,   228,   405,   407,   231,    24,   288,
     234,    15,    16,   107,   105,   649,    15,    16,   404,   406,
     108,   412,     3,   515,   516,   111,   112,   111,   112,   651,
      15,    16,   130,     8,     9,    10,    11,    12,     1,    15,
      16,   216,    13,   223,    25,   217,   142,   224,   144,    32,
     146,    29,    15,    16,   162,    15,    16,    15,    16,    15,
      16,   517,   111,   112,   163,    15,    16,   175,   103,   197,
     198,   448,   518,   872,   199,    15,    16,    15,    16,   702,
     200,   201,   202,    30,   203,   703,   204,   289,   650,   221,
     205,     4,   519,    15,    16,   408,    15,    16,    26,   291,
     690,    33,   652,   290,   821,   379,   237,   239,   241,   243,
     245,   247,   236,  1002,   143,    14,   145,   292,   147,   297,
     299,   238,   311,   312,   313,   315,   316,   318,   319,   321,
     323,   324,   325,   327,   329,   331,   206,   207,   208,   240,
     381,   242,   380,   382,   520,   521,   522,   244,    27,   345,
      15,    16,    15,    16,   349,   351,   353,   246,   448,   296,
     875,   384,    15,    16,   387,   388,   389,   391,   392,   394,
     395,   397,    15,    16,   184,   298,    15,    16,   314,    15,
      16,   409,   415,   416,   417,   419,   420,   421,   422,   423,
     424,   438,    15,    16,    15,    16,   186,   574,   431,    15,
      16,    34,   572,    15,    16,   209,   210,    15,    16,    15,
      16,    15,    16,   211,   212,    15,    16,   448,   447,   943,
     450,    36,   452,    38,   454,    40,   456,   235,   458,   704,
     700,   706,   317,   188,   320,   705,   226,   707,   163,   229,
     227,    28,   185,   230,   322,    15,    16,   111,   112,    15,
      16,    15,    16,    35,   326,   102,    15,    16,   328,    15,
      16,   330,    15,    16,   187,    15,    16,    15,    16,    15,
      16,   473,   474,    37,   348,    39,   350,    41,   478,   106,
     480,   352,   482,   790,   791,   383,    15,    16,   232,   390,
     612,   393,   233,   396,    15,    16,   487,   723,   448,   490,
     946,   189,   493,   469,   495,   541,   116,   470,   499,   542,
     501,   110,   503,   115,   505,   506,   507,   508,   509,   510,
     511,   512,   641,   643,   645,   513,   118,   725,   119,   525,
     526,   727,   528,   729,   530,   640,   642,   644,   731,   918,
     165,   733,   124,   919,   735,   355,   356,   768,   918,   770,
     121,   772,  1014,   190,   191,   125,   192,   193,   194,    15,
      16,   543,   418,   545,   126,   547,   787,   549,   774,   788,
     127,   787,   554,   357,   933,   557,   776,   164,   560,   131,
     562,   563,   564,   565,   566,   567,    15,    16,   293,   294,
     111,   112,   140,   141,   300,   128,   129,   692,   132,   694,
     878,   879,   880,   881,   584,   585,   149,   587,   588,   589,
     590,   150,   133,   134,   135,   591,   592,   593,   136,   594,
     137,   138,   595,   139,   148,   596,   176,   151,   597,   358,
     359,   360,   361,   362,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   605,   606,   607,   608,   609,   610,   611,
     282,   283,   284,   285,   286,   152,   153,   166,   170,   249,
     250,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     172,   173,   174,   177,   182,   363,   364,   365,   366,   183,
     178,   179,   180,   181,   367,   195,   215,   222,   248,   343,
     332,   333,   334,   368,   335,   336,   337,   338,   339,   646,
     647,   648,   860,   369,   370,   346,   371,   372,   373,   154,
     374,   375,   376,   347,   354,   660,   661,   662,   663,   664,
     665,   666,   385,   398,   399,   400,   401,   251,   155,   156,
     402,   157,   158,   159,   403,   425,   160,   161,   426,   427,
     428,   429,   758,   252,   253,   254,   255,   256,   432,   435,
     433,   434,   436,   441,   437,   439,   691,   440,   443,   693,
     856,   695,   696,   442,   698,   699,   444,   445,   448,   446,
     618,   461,   568,   462,   463,   464,   465,   466,   467,   569,
     484,   533,    51,   257,   485,   486,   489,   492,   497,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   498,
     724,   726,   728,   514,   730,   534,   732,   268,   551,   734,
     736,   535,   536,   884,   537,   538,   539,   269,   270,   552,
     271,   272,   273,   345,   274,   275,   276,   277,   278,   279,
     553,   754,   755,   756,   757,   556,   559,   570,   571,   572,
    -342,  -343,   576,   577,   578,   579,   580,   581,   582,   599,
     689,   653,   747,   583,   769,   771,   773,   469,   775,   619,
     777,   586,   620,   622,   624,   627,   632,   783,   784,   785,
     786,   628,   633,   634,   635,   636,   637,   638,   639,    15,
      16,   541,   680,   685,   795,   672,   797,   673,   799,   674,
     801,   676,   802,   803,   804,   805,   806,   681,   678,   682,
     697,   683,   684,   686,   687,   807,   688,   809,   701,   811,
     708,   813,   709,   815,   715,   817,   710,   819,   950,   711,
     712,   713,   714,   748,   751,   716,   856,   345,   717,   718,
     719,   949,   720,   826,   859,   721,   760,   749,   750,   752,
     761,   762,   753,   763,   764,   765,   766,   827,   792,   793,
     862,   794,   864,   796,   866,   798,   868,   800,   870,   874,
     861,   822,   877,   828,   829,   830,   831,   832,   882,   883,
     910,   887,   889,   893,   891,   885,   886,   823,   888,   824,
     890,   894,   892,   825,   895,   896,   897,   898,   945,   911,
     912,   913,   914,   915,   916,   917,   929,   930,   931,   932,
     935,   936,    51,   833,   906,   907,   908,   909,   937,   834,
     835,   836,   837,   838,   839,   840,   841,   842,   843,   948,
     856,   959,   960,   965,   961,   966,   967,   844,   992,   968,
     969,   996,   983,   973,   974,   982,   975,   845,   846,   976,
     847,   848,   849,   921,   850,   851,   852,   853,   854,   855,
     977,   978,   979,   984,   985,   991,   997,   998,   999,   787,
    1000,  1008,  1003,  1004,  1005,  1006,  1007,   430,   122,  1011,
    1009,  1015,   667,  1012,   123,   938,   169,   939,   940,   941,
     942,   341,   171,   654,  1013,   340,   958,   196,     0,     0,
     342,     0,     0,   951,   952,   953,   954,   955,   956,   957,
     859,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   386,     0,     0,     0,     0,     0,   962,   963,   964,
       0,     0,   451,     0,   453,     0,   455,     0,   457,     0,
     459,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   981,     0,     0,     0,     0,     0,   986,   987,   988,
     989,   990,     0,     0,   249,   250,     0,     0,     0,     0,
       0,     0,     0,   468,     0,   993,   994,   995,     0,     0,
     471,   472,     0,  1001,   475,   476,     0,     0,   477,     0,
     479,     0,   481,     0,     0,     0,     0,     0,     0,     0,
    1010,     0,     0,   483,   859,     0,     0,     0,   488,     0,
       0,   491,     0,     0,   494,     0,   496,     0,     0,     0,
     500,     0,   502,     0,   504,     0,     0,     0,     0,     0,
       0,     0,   251,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   527,     0,   529,     0,   531,     0,   252,   253,
     254,   255,   256,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     540,     0,     0,   544,     0,   546,     0,   548,     0,   550,
       0,     0,     0,     0,   555,     0,     0,   558,   257,     0,
     561,     0,     0,     0,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,     0,     0,     0,     0,     0,     0,
       0,     0,   268,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   269,   270,     0,   271,   272,   273,     0,   274,
     275,   276,   277,   278,   279,     0,     0,     0,     0,     0,
     598,   410,   411,   600,     0,   601,     0,   602,     0,   603,
       0,   604,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   613,   614,     0,     0,
       0,   615,     0,   616,     0,   617,     0,     0,     0,     0,
     621,     0,     0,   623,     0,     0,   625,     0,   626,     0,
       0,     0,   629,     0,   630,     0,   631,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   251,
       0,     0,     0,     0,     0,     0,     0,   655,   656,   657,
       0,   658,     0,   659,     0,   252,   253,   254,   255,   256,
       0,     0,     0,     0,     0,     0,   668,     0,   669,     0,
     670,     0,   671,     0,     0,     0,     0,   675,     0,     0,
     677,     0,     0,   679,     0,     0,     0,     0,     0,    80,
       0,     0,     0,     0,     0,   257,     0,     0,     0,     0,
       0,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,     0,     0,     0,     0,     0,     0,     0,     0,   268,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   269,
     270,     0,   271,   272,   273,   722,   274,   275,   276,   277,
     278,   279,     0,     0,     0,    82,    83,    84,     0,     0,
       0,     0,    85,     0,     0,   737,   738,   739,   740,   741,
     742,   743,   744,   745,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   759,     0,    80,
       0,     0,     0,     0,    86,     0,     0,     0,     0,     0,
     767,    87,    88,    89,     0,    81,     0,     0,     0,     0,
      90,     0,     0,   778,   779,   780,   781,   782,     0,    91,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    92,
      93,     0,    94,    95,    96,     0,     0,     0,     0,     0,
       0,     0,     0,    97,     0,    82,    83,    84,     0,     0,
       0,     0,    85,     0,     0,     0,     0,   808,     0,   810,
       0,   812,     0,   814,     0,   816,     0,   818,     0,   820,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    86,     0,     0,     0,     0,     0,
       0,    87,    88,    89,     0,    42,     0,     0,     0,     0,
      90,     0,   863,     0,   865,     0,   867,     0,   869,    91,
     871,     0,     0,     0,     0,     0,   873,     0,   876,    92,
      93,     0,    94,    95,    96,     0,     0,     0,     0,     0,
      43,     0,     0,    97,     0,     0,     0,     0,     0,     0,
     899,     0,   900,     0,   901,     0,   902,     0,   903,     0,
     904,     0,   905,     0,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,     0,     0,    50,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   920,     0,     0,     0,   922,     0,   923,     0,   924,
       0,   925,     0,   926,     0,   927,     0,     0,   928,    51,
      52,     0,     0,     0,     0,     0,    53,    54,    55,    56,
      57,     0,    43,     0,     0,    58,     0,     0,     0,     0,
       0,     0,     0,     0,    59,     0,     0,     0,     0,   944,
       0,   947,     0,     0,    60,    61,    44,    62,    63,    64,
       0,    65,    66,    67,    68,    69,    70,   167,    71,     0,
       0,     0,    45,    46,    47,    48,    49,     0,     0,    50,
       0,     0,     0,     0,     0,     0,   970,     0,     0,   971,
       0,     0,   972,     0,     0,     0,     0,     0,     0,     0,
       0,   980,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    51,    52,     0,    44,     0,     0,     0,    53,    54,
      55,    56,    57,     0,     0,     0,     0,    58,     0,     0,
      45,    46,    47,    48,    49,     0,    59,    50,     0,     0,
       0,     0,     0,     0,     0,     0,    60,    61,     0,    62,
      63,    64,     0,    65,    66,    67,    68,    69,    70,     0,
      71,     0,     0,     0,     0,     0,     0,     0,     0,    51,
      52,     0,     0,     0,     0,     0,    53,    54,    55,    56,
      57,     0,     0,     0,     0,    58,     0,     0,     0,     0,
       0,     0,     0,     0,    59,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    60,    61,     0,    62,    63,    64,
       0,    65,    66,    67,    68,    69,    70,     0,    71
  };

  const short
   Parser ::yycheck_[] =
  {
       8,     9,    10,    11,    12,    13,    14,   248,   354,   110,
     641,    36,   409,   882,   115,   197,   198,   118,    11,     7,
     121,     3,     4,    12,    32,    72,     3,     4,   197,   198,
      19,   200,     0,    25,    26,     7,     8,     7,     8,    72,
       3,     4,    50,    63,    64,    65,    66,    67,     0,     3,
       4,    15,    72,    15,    11,    19,    16,    19,    16,    18,
      16,    76,     3,     4,    14,     3,     4,     3,     4,     3,
       4,    63,     7,     8,    24,     3,     4,    85,    19,    25,
      26,     4,    74,     6,    30,     3,     4,     3,     4,    11,
      36,    37,    38,    76,    40,    17,    42,    85,   145,   107,
      46,    69,    94,     3,     4,    87,     3,     4,    11,   134,
      87,    70,   145,    85,   745,    85,   124,   125,   126,   127,
     128,   129,    85,   992,    84,   145,    84,   135,    84,   137,
     138,    85,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,    92,    93,    94,    85,
      85,    85,   177,   178,   146,   147,   148,    85,    11,   167,
       3,     4,     3,     4,   172,   173,   174,    85,     4,    85,
       6,   179,     3,     4,   182,   183,   184,   185,   186,   187,
     188,   189,     3,     4,    16,    85,     3,     4,    85,     3,
       4,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   226,     3,     4,     3,     4,    16,    11,   216,     3,
       4,    18,    16,     3,     4,   161,   162,     3,     4,     3,
       4,     3,     4,   169,   170,     3,     4,     4,   236,     6,
     238,    18,   240,    18,   242,    18,   244,    13,   246,    11,
     586,    11,    85,    16,    85,    17,    15,    17,    24,    15,
      19,    11,    84,    19,    85,     3,     4,     7,     8,     3,
       4,     3,     4,    70,    85,    83,     3,     4,    85,     3,
       4,    85,     3,     4,    84,     3,     4,     3,     4,     3,
       4,   289,   290,    70,    85,    70,    85,    70,   296,    18,
     298,    85,   300,   690,   691,    85,     3,     4,    15,    85,
     469,    85,    19,    85,     3,     4,   314,    85,     4,   317,
       6,    84,   320,    15,   322,    15,     9,    19,   326,    19,
     328,    18,   330,    18,   332,   333,   334,   335,   336,   337,
     338,   339,   514,   515,   516,   343,    18,    85,    10,   347,
     348,    85,   350,    85,   352,   514,   515,   516,    85,    15,
      23,    85,    84,    19,    85,     3,     4,    85,    15,    85,
      18,    85,    19,   161,   162,    84,   164,   165,   166,     3,
       4,   379,     6,   381,    84,   383,    12,   385,    85,    15,
      84,    12,   390,    31,    15,   393,    85,    22,   396,    18,
     398,   399,   400,   401,   402,   403,     3,     4,    61,    62,
       7,     8,    16,    16,    85,    84,    84,   576,    84,   578,
     266,   267,   268,   269,   422,   423,    16,   425,   426,   427,
     428,    16,    84,    84,    84,   433,   434,   435,    84,   437,
      84,    84,   440,    84,    84,   443,    18,    84,   446,    87,
      88,    89,    90,    91,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   461,   462,   463,   464,   465,   466,   467,
     128,   129,   130,   131,   132,    84,    84,    84,    84,     3,
       4,   152,   153,   154,   155,   156,   157,   158,   159,   160,
      84,    84,    84,    84,    16,   133,   134,   135,   136,    16,
      84,    84,    84,    84,   142,    84,    14,    14,    95,    85,
      16,    16,    16,   151,    16,    16,    16,    16,    16,   517,
     518,   519,   753,   161,   162,    83,   164,   165,   166,   142,
     168,   169,   170,    85,    95,   533,   534,   535,   536,   537,
     538,   539,    85,    16,    16,    16,    16,    71,   161,   162,
      16,   164,   165,   166,    85,    16,   169,   170,    16,    16,
      16,    14,   653,    87,    88,    89,    90,    91,    14,    12,
      15,    15,    14,     9,    15,    14,   574,    15,    15,   577,
     752,   579,   580,    14,   582,   583,    10,    14,     4,    15,
      84,    16,    27,    16,    16,    16,    16,    16,    16,    27,
      17,    16,   126,   127,    17,    17,    17,    17,    17,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,    17,
     618,   619,   620,    11,   622,    16,   624,   151,    17,   627,
     628,    16,    16,   792,    16,    16,    16,   161,   162,    17,
     164,   165,   166,   641,   168,   169,   170,   171,   172,   173,
      17,   649,   650,   651,   652,    17,    17,    28,    28,    16,
      11,    11,    15,    15,    11,    11,    11,    11,    11,     4,
       4,    12,    27,    15,   672,   673,   674,    15,   676,    84,
     678,    95,    84,    84,    84,    84,    17,   685,   686,   687,
     688,    84,    17,    17,    17,    17,    17,    17,    17,     3,
       4,    15,    17,    63,   702,    84,   704,    84,   706,    84,
     708,    84,   710,   711,   712,   713,   714,    17,    84,    17,
       6,    17,    17,    63,    63,   723,    63,   725,    17,   727,
      15,   729,    13,   731,    17,   733,    15,   735,   910,    15,
      15,    15,    15,    27,    60,    17,   918,   745,    17,    17,
      17,   910,    17,   751,   752,    17,    17,    28,    28,    18,
      17,    17,    95,    17,    17,    17,    17,    71,    11,    15,
     768,    11,   770,    11,   772,    11,   774,    11,   776,     6,
      15,    63,     6,    87,    88,    89,    90,    91,     4,    17,
      11,    17,    17,    15,    17,   793,   794,    63,   796,    63,
     798,    13,   800,    63,    15,    15,    15,    15,     6,    16,
      16,    16,    16,    16,    16,    16,    13,    13,    13,    13,
      11,    11,   126,   127,   822,   823,   824,   825,    11,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,     6,
    1012,    15,     4,    15,    17,    15,    15,   151,     4,    15,
      15,    19,    11,    17,    17,    15,    17,   161,   162,    17,
     164,   165,   166,   861,   168,   169,   170,   171,   172,   173,
      17,    17,    17,    11,    11,    15,    19,    19,    19,    12,
      19,    15,    14,    14,    14,    14,    14,   214,    42,    15,
      17,    13,   541,    18,    43,   893,    77,   895,   896,   897,
     898,   164,    81,   523,  1012,   163,   918,   100,    -1,    -1,
     165,    -1,    -1,   911,   912,   913,   914,   915,   916,   917,
     918,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   181,    -1,    -1,    -1,    -1,    -1,   935,   936,   937,
      -1,    -1,   239,    -1,   241,    -1,   243,    -1,   245,    -1,
     247,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   959,    -1,    -1,    -1,    -1,    -1,   965,   966,   967,
     968,   969,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   280,    -1,   983,   984,   985,    -1,    -1,
     287,   288,    -1,   991,   291,   292,    -1,    -1,   295,    -1,
     297,    -1,   299,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    1008,    -1,    -1,   310,  1012,    -1,    -1,    -1,   315,    -1,
      -1,   318,    -1,    -1,   321,    -1,   323,    -1,    -1,    -1,
     327,    -1,   329,    -1,   331,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   349,    -1,   351,    -1,   353,    -1,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     377,    -1,    -1,   380,    -1,   382,    -1,   384,    -1,   386,
      -1,    -1,    -1,    -1,   391,    -1,    -1,   394,   127,    -1,
     397,    -1,    -1,    -1,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   151,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   161,   162,    -1,   164,   165,   166,    -1,   168,
     169,   170,   171,   172,   173,    -1,    -1,    -1,    -1,    -1,
     447,     3,     4,   450,    -1,   452,    -1,   454,    -1,   456,
      -1,   458,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   473,   474,    -1,    -1,
      -1,   478,    -1,   480,    -1,   482,    -1,    -1,    -1,    -1,
     487,    -1,    -1,   490,    -1,    -1,   493,    -1,   495,    -1,
      -1,    -1,   499,    -1,   501,    -1,   503,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   524,   525,   526,
      -1,   528,    -1,   530,    -1,    87,    88,    89,    90,    91,
      -1,    -1,    -1,    -1,    -1,    -1,   543,    -1,   545,    -1,
     547,    -1,   549,    -1,    -1,    -1,    -1,   554,    -1,    -1,
     557,    -1,    -1,   560,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,
      -1,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   161,
     162,    -1,   164,   165,   166,   612,   168,   169,   170,   171,
     172,   173,    -1,    -1,    -1,    87,    88,    89,    -1,    -1,
      -1,    -1,    94,    -1,    -1,   632,   633,   634,   635,   636,
     637,   638,   639,   640,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   654,    -1,    31,
      -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,
     667,   133,   134,   135,    -1,    47,    -1,    -1,    -1,    -1,
     142,    -1,    -1,   680,   681,   682,   683,   684,    -1,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   161,
     162,    -1,   164,   165,   166,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   175,    -1,    87,    88,    89,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    -1,   724,    -1,   726,
      -1,   728,    -1,   730,    -1,   732,    -1,   734,    -1,   736,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,
      -1,   133,   134,   135,    -1,    12,    -1,    -1,    -1,    -1,
     142,    -1,   769,    -1,   771,    -1,   773,    -1,   775,   151,
     777,    -1,    -1,    -1,    -1,    -1,   783,    -1,   785,   161,
     162,    -1,   164,   165,   166,    -1,    -1,    -1,    -1,    -1,
      47,    -1,    -1,   175,    -1,    -1,    -1,    -1,    -1,    -1,
     807,    -1,   809,    -1,   811,    -1,   813,    -1,   815,    -1,
     817,    -1,   819,    -1,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    -1,    -1,    94,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   858,    -1,    -1,    -1,   862,    -1,   864,    -1,   866,
      -1,   868,    -1,   870,    -1,   872,    -1,    -1,   875,   126,
     127,    -1,    -1,    -1,    -1,    -1,   133,   134,   135,   136,
     137,    -1,    47,    -1,    -1,   142,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,    -1,   906,
      -1,   908,    -1,    -1,   161,   162,    71,   164,   165,   166,
      -1,   168,   169,   170,   171,   172,   173,    82,   175,    -1,
      -1,    -1,    87,    88,    89,    90,    91,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,   943,    -1,    -1,   946,
      -1,    -1,   949,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   958,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   126,   127,    -1,    71,    -1,    -1,    -1,   133,   134,
     135,   136,   137,    -1,    -1,    -1,    -1,   142,    -1,    -1,
      87,    88,    89,    90,    91,    -1,   151,    94,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   161,   162,    -1,   164,
     165,   166,    -1,   168,   169,   170,   171,   172,   173,    -1,
     175,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,
     127,    -1,    -1,    -1,    -1,    -1,   133,   134,   135,   136,
     137,    -1,    -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   161,   162,    -1,   164,   165,   166,
      -1,   168,   169,   170,   171,   172,   173,    -1,   175
  };

  const short
   Parser ::yystos_[] =
  {
       0,     0,   271,     0,    69,   272,   276,   302,    63,    64,
      65,    66,    67,    72,   145,     3,     4,   313,   313,   313,
     313,   313,   313,   313,    11,    11,    11,    11,    11,    76,
      76,   275,    18,    70,    18,    70,    18,    70,    18,    70,
      18,    70,    12,    47,    71,    87,    88,    89,    90,    91,
      94,   126,   127,   133,   134,   135,   136,   137,   142,   151,
     161,   162,   164,   165,   166,   168,   169,   170,   171,   172,
     173,   175,   277,   278,   279,   280,   285,   289,   291,   292,
      31,    47,    87,    88,    89,    94,   126,   133,   134,   135,
     142,   151,   161,   162,   164,   165,   166,   175,   288,   293,
     294,   295,    83,    19,   309,   313,    18,    12,    19,   310,
      18,     7,     8,   307,   314,    18,     9,   308,    18,    10,
     306,    18,   277,   292,    84,    84,    84,    84,    84,    84,
     313,    18,    84,    84,    84,    84,    84,    84,    84,    84,
      16,    16,    16,    84,    16,    84,    16,    84,    84,    16,
      16,    84,    84,    84,   142,   161,   162,   164,   165,   166,
     169,   170,    14,    24,    22,    23,    84,    82,   281,   291,
      84,   295,    84,    84,    84,   313,    18,    84,    84,    84,
      84,    84,    16,    16,    16,    84,    16,    84,    16,    84,
     161,   162,   164,   165,   166,    84,   293,    25,    26,    30,
      36,    37,    38,    40,    42,    46,    92,    93,    94,   161,
     162,   169,   170,   273,   274,    14,    15,    19,   313,   314,
     315,   313,    14,    15,    19,   315,    15,    19,   315,    15,
      19,   315,    15,    19,   315,    13,    85,   313,    85,   313,
      85,   313,    85,   313,    85,   313,    85,   313,    95,     3,
       4,    71,    87,    88,    89,    90,    91,   127,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   151,   161,
     162,   164,   165,   166,   168,   169,   170,   171,   172,   173,
     283,   284,   128,   129,   130,   131,   132,   305,     7,    85,
      85,   314,   313,    61,    62,   296,    85,   313,    85,   313,
      85,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     301,   313,   313,   313,    85,   313,   313,    85,   313,   313,
      85,   313,    85,   313,   313,   313,    85,   313,    85,   313,
      85,   313,    16,    16,    16,    16,    16,    16,    16,    16,
     278,   279,   280,    85,   282,   313,    83,    85,    85,   313,
      85,   313,    85,   313,    95,     3,     4,    31,    87,    88,
      89,    90,    91,   133,   134,   135,   136,   142,   151,   161,
     162,   164,   165,   166,   168,   169,   170,   286,   287,    85,
     314,    85,   314,    85,   313,    85,   301,   313,   313,   313,
      85,   313,   313,    85,   313,   313,    85,   313,    16,    16,
      16,    16,    16,    85,   283,   285,   283,   285,    87,   313,
       3,     4,   283,   311,   312,   313,   313,   313,     6,   313,
     313,   313,   313,   313,   313,    16,    16,    16,    16,    14,
     273,   313,    14,    15,    15,    12,    14,    15,   314,    14,
      15,     9,    14,    15,    10,    14,    15,   313,     4,   290,
     313,   290,   313,   290,   313,   290,   313,   290,   313,   290,
     284,    16,    16,    16,    16,    16,    16,    16,   290,    15,
      19,   290,   290,   313,   313,   290,   290,   290,   313,   290,
     313,   290,   313,   290,    17,    17,    17,   313,   290,    17,
     313,   290,    17,   313,   290,   313,   290,    17,    17,   313,
     290,   313,   290,   313,   290,   313,   313,   313,   313,   313,
     313,   313,   313,   313,    11,    25,    26,    63,    74,    94,
     146,   147,   148,   297,   298,   313,   313,   290,   313,   290,
     313,   290,   287,    16,    16,    16,    16,    16,    16,    16,
     290,    15,    19,   313,   290,   313,   290,   313,   290,   313,
     290,    17,    17,    17,   313,   290,    17,   313,   290,    17,
     313,   290,   313,   313,   313,   313,   313,   313,    27,    27,
      28,    28,    16,   304,    11,   304,    15,    15,    11,    11,
      11,    11,    11,    15,   313,   313,    95,   313,   313,   313,
     313,   313,   313,   313,   313,   313,   313,   313,   290,     4,
     290,   290,   290,   290,   290,   313,   313,   313,   313,   313,
     313,   313,   283,   290,   290,   290,   290,   290,    84,    84,
      84,   290,    84,   290,    84,   290,   290,    84,    84,   290,
     290,   290,    17,    17,    17,    17,    17,    17,    17,    17,
     283,   285,   283,   285,   283,   285,   313,   313,   313,    72,
     145,    72,   145,    12,   298,   290,   290,   290,   290,   290,
     313,   313,   313,   313,   313,   313,   313,   286,   290,   290,
     290,   290,    84,    84,    84,   290,    84,   290,    84,   290,
      17,    17,    17,    17,    17,    63,    63,    63,    63,     4,
      87,   313,   283,   313,   283,   313,   313,     6,   313,   313,
     287,    17,    11,    17,    11,    17,    11,    17,    15,    13,
      15,    15,    15,    15,    15,    17,    17,    17,    17,    17,
      17,    17,   290,    85,   313,    85,   313,    85,   313,    85,
     313,    85,   313,    85,   313,    85,   313,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   282,    27,    27,    28,
      28,    60,    18,    95,   313,   313,   313,   313,   315,   290,
      17,    17,    17,    17,    17,    17,    17,   290,    85,   313,
      85,   313,    85,   313,    85,   313,    85,   313,   290,   290,
     290,   290,   290,   313,   313,   313,   313,    12,    15,   303,
     304,   304,    11,    15,    11,   313,    11,   313,    11,   313,
      11,   313,   313,   313,   313,   313,   313,   313,   290,   313,
     290,   313,   290,   313,   290,   313,   290,   313,   290,   313,
     290,   282,    63,    63,    63,    63,   313,    71,    87,    88,
      89,    90,    91,   127,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   151,   161,   162,   164,   165,   166,
     168,   169,   170,   171,   172,   173,   285,   299,   300,   313,
     284,    15,   313,   290,   313,   290,   313,   290,   313,   290,
     313,   290,     6,   290,     6,     6,   290,     6,   266,   267,
     268,   269,     4,    17,   283,   313,   313,    17,   313,    17,
     313,    17,   313,    15,    13,    15,    15,    15,    15,   290,
     290,   290,   290,   290,   290,   290,   313,   313,   313,   313,
      11,    16,    16,    16,    16,    16,    16,    16,    15,    19,
     290,   313,   290,   290,   290,   290,   290,   290,   290,    13,
      13,    13,    13,    15,   303,    11,    11,    11,   313,   313,
     313,   313,   313,     6,   290,     6,     6,   290,     6,   283,
     285,   313,   313,   313,   313,   313,   313,   313,   300,    15,
       4,    17,   313,   313,   313,    15,    15,    15,    15,    15,
     290,   290,   290,    17,    17,    17,    17,    17,    17,    17,
     290,   313,    15,    11,    11,    11,   313,   313,   313,   313,
     313,    15,     4,   313,   313,   313,    19,    19,    19,    19,
      19,   313,   303,    14,    14,    14,    14,    14,    15,    17,
     313,    15,    18,   299,    19,    13
  };

  const short
   Parser ::yyr1_[] =
  {
       0,   270,   271,   271,   271,   271,   271,   272,   273,   273,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   275,   276,   277,   277,   278,   278,   279,
     279,   280,   280,   280,   281,   281,   282,   282,   282,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   284,   284,   285,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   287,   287,   288,   289,   289,   289,   290,   290,   291,
     291,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   293,   293,   294,   294,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   296,   296,   297,   297,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   299,   299,   300,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   300,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   300,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   300,
     300,   300,   300,   301,   301,   301,   301,   301,   301,   301,
     301,   301,   302,   302,   302,   302,   302,   302,   302,   302,
     302,   302,   302,   302,   303,   303,   303,   303,   303,   304,
     304,   304,   304,   305,   305,   305,   305,   305,   306,   306,
     307,   307,   308,   308,   309,   309,   310,   310,   311,   311,
     312,   312,   313,   313,   314,   314,   315,   315
  };

  const signed char
   Parser ::yyr2_[] =
  {
       0,     2,     0,     2,     2,     2,     1,     7,     2,     0,
       6,     3,     3,     6,     6,    10,    10,     6,     6,    10,
       6,     5,     7,     6,     6,     7,     5,     6,     4,     5,
       3,     5,     3,     4,     2,     4,     2,     4,     2,     2,
       2,     2,     4,     2,     6,     3,     1,     3,     1,     3,
       1,     3,     4,     2,     2,     0,     5,     4,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     4,     1,     4,     1,     1,     4,     4,     4,
       1,     1,     1,     2,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       1,     4,     1,     1,     4,     4,     4,     1,     1,     1,
       1,     2,     4,     4,     1,     2,     0,     2,     0,     2,
       1,     5,     4,     4,     5,     4,     5,     4,     5,     4,
       5,     4,     5,     4,     5,     4,     4,     5,     4,     5,
       6,     7,     8,     6,     7,     8,     6,     7,     8,     4,
       5,     6,     7,     8,     4,     5,     6,     7,     8,     4,
       5,     6,     7,     8,     6,     7,     8,     4,     5,     6,
       4,     5,     4,     5,     4,     5,     4,     5,     5,     4,
       4,     4,     4,     4,     2,     1,     2,     1,     4,     4,
       4,     4,     4,     6,     7,     6,     7,     6,     7,     4,
       6,     7,     4,     6,     7,     4,     4,     4,     5,     5,
       5,     5,     8,     8,     8,     5,     8,     5,     8,     5,
       5,     5,     5,     5,     4,     4,     1,     1,     2,     3,
       5,    16,     3,     3,     3,     3,     6,     5,     7,     6,
       6,     7,     5,     6,     7,     6,     4,     2,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       4,     1,     4,     1,     4,     1,     4,     4,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     8,     7,    17,     8,     7,    17,     8,    17,
       8,    17,     8,    17,     0,     3,     3,     3,     3,     0,
      10,     6,     4,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     5,     7,     1,     3,
       3,     5,     1,     1,     1,     1,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const  Parser ::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"ident\"",
  "\"string\"", "\"variable ($string)\"", "\"integer number\"",
  "\"ip_with_star\"", "\"ip_without_star\"", "\"url_with_star\"",
  "\"timing\"", "\"equal\"", "\"leftpar\"", "\"rightpar\"",
  "\"semicolon\"", "\"comma\"", "\"leftsqpar\"", "\"rightsqpar\"",
  "\"leftgrpar\"", "\"rightsgrpar\"", "\".\"", "\"$\"", "\" or \"",
  "\" and \"", "\" next \"", "\" add \"", "\" del \"", "\" to \"",
  "\" from \"", "\"vexpiry\"", "\"TCP_REDIRECT\"", "\"time\"",
  "TCP_REDIRECT_STRING", "TCP_REDIRECT_VSTRING", "TCP_REDIRECT_HOST",
  "TCP_REDIRECT_VHOST", "\"scm.manage\"", "\"scm.give\"",
  "\"HTTP_REDIRECT\"", "\"HTTP_VREDIRECT\"", "\"BANDWIDTH\"",
  "\"VBANDWIDTH\"", "\"ANSWER\"", "\"VANSWER\"", "\"vscm.manage\"",
  "\"vscm.give\"", "\"trace\"", "\"!\"", "\"when\"", "\"if\"", "\"then\"",
  "\"enabled\"", "\"disabled\"", "\"default\"", "\">\"", "\"<\"", "\">=\"",
  "\"<=\"", "\"+\"", "\"-\"", "\"@\"", "\"true\"", "\"false\"", "\"set\"",
  "\"dictionary\"", "\"ipset\"", "\"urlset\"", "\"timeset\"", "\"parset\"",
  "\"define\"", "\"mysql\"", "\"agent id\"", "\"validation rule\"",
  "\"validation report\"", "\"report\"", "\"mreport\"", "\"condition\"",
  "\"sequence\"", "\"of\"", "\"key\"", "\"frequence\"", "\"counter\"",
  "\"var\"", "\"action\"", "\"is\"", "\"in\"", "\"always\"",
  "\"HTTP HOST\"", "\"HTTP URL\"", "\"HTTP URI\"",
  "\"HTTP host in the url\"", "\"HTTP host in the headers\"",
  "\"In tutti gli header esegue, compresa url, esegue il replace\"",
  "HTTP_ANSWER_HEADER_REPLACE", "\"Calls a lisp function\"",
  "\"Parameters for a lisp function\"", "\"program\"", "SET_LIST_ID",
  "DICTIONARY_LIST_ID", "SET_MYSQL", "SET_LIST_IPSTAR", "SET_LIST_URLSTAR",
  "SET_LIST_TIMING", "VR_DEF", "VR_DEF_WHEN", "VR_DEF_ACTION",
  "VR_DEF_WHEN_AGENT_ID_IS", "VR_DEF_WHEN_AGENT_ID_IS_IN",
  "VR_DEF_WHEN_OBS_NAME_IS", "VR_DEF_WHEN_OBS_NAME_IS_IN",
  "VR_DEF_WHEN_OBS_EVENT_IS", "VR_DEF_WHEN_OBS_EVENT_IS_IN",
  "VR_DEF_WHEN_IPPORT_IS", "VR_DEF_WHEN_IPPORT_IS_IN",
  "VR_DEF_WHEN_HTTP_METHOD_IS", "VR_DEF_WHEN_HTTP_METHOD_IS_IN",
  "VR_DEF_WHEN_HTTP_HOST_IS", "VR_DEF_WHEN_HTTP_HOST_IS_IN",
  "VR_DEF_WHEN_HTTP_URL_IS", "VR_DEF_WHEN_HTTP_URL_IS_IN",
  "VR_DEF_WHEN_HTTP_URI_IS", "VR_DEF_WHEN_HTTP_URI_IS_IN", "VR_EMPTY",
  "AR_DEF", "AR_DEF_WHEN", "AR_DEF_ACTION", "CAT", "\"OBS_EVENT\"",
  "\"NET_TCP_CONNECT\"", "\"NET_TCP_ACCEPT\"", "\"NET_TCP_SEND\"",
  "\"NET_TCP_RECEIVE\"", "\"NET_TCP_CLOSE\"", "\"NET_TCP_IPSRC\"",
  "\"NET_TCP_IPDST\"", "\"NET_TCP_SESID\"", "\"NET_RESULT\"",
  "\"OBS_NAME\"", "\"OBS_TIME\"", "\"OBS_OBSID\"", "\"OBS_STIME\"",
  "\"OBS_ETIME\"", "\"OBS_TAG\"", "\"Data\"", "\"Make\"",
  "\"Access rule\"", "\"Enable\"", "\"Disable\"", "\"Mysql.append\"",
  "MYSQL_APPEND_VARIABLES", "\"Mysql.update\"", "HTTP_METHOD",
  "HTTP_METHOD_GET", "HTTP_METHOD_POST", "HTTP_METHOD_HEAD",
  "HTTP_METHOD_PUT", "HTTP_METHOD_DELETE", "HTTP_METHOD_TRACE",
  "HTTP_METHOD_OPTIONS", "HTTP_METHOD_CONNECT", "HTTP_METHOD_PATCH",
  "HTTP_COOKIE", "HTTP_HEADER", "HTTP_PORT", "HTTP_QUERY", "HTTP_DATA",
  "HTTP_ANSWER_DATA", "HTTP_ANSWER", "HTTP_ANSWER_CODE",
  "HTTP_ANSWER_HEADER", "HTTP_ANSWER_COOKIE", "SA_OS", "SA_LOGGED_USER",
  "SA_PROCESS_NAME", "SA_PROCESS_ATTACH", "EXISTS",
  "VR_DEF_WHEN_HTTP_COOKIE_IS", "VR_DEF_WHEN_HTTP_COOKIE_IS_IN",
  "VR_DEF_WHEN_HTTP_COOKIE_EXISTS", "VR_DEF_WHEN_HTTP_HEADER_IS",
  "VR_DEF_WHEN_HTTP_HEADER_IS_IN", "VR_DEF_WHEN_HTTP_HEADER_EXISTS",
  "VR_DEF_WHEN_HTTP_QUERY_IS", "VR_DEF_WHEN_HTTP_QUERY_IS_IN",
  "VR_DEF_WHEN_HTTP_QUERY_EXISTS", "VR_DEF_WHEN_HTTP_ANSWER_CODE_IS",
  "VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN",
  "VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS",
  "VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN",
  "VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS",
  "VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS",
  "VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN",
  "VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS", "VR_DEF_WHEN_HTTP_DATA_IS",
  "VR_DEF_WHEN_HTTP_DATA_IS_IN", "VR_DEF_WHEN_HTTP_DATA_EXISTS",
  "VR_DEF_WHEN_HTTP_ANSWER_DATA_IS", "VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN",
  "VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS", "VR_DEF_WHEN_OBS_TAG_IS",
  "VR_DEF_WHEN_OBS_TAG_IS_IN", "VR_DEF_WHEN_OBS_TAG_EXISTS",
  "VR_DEF_WHEN_HTTP_URL_HOST_ID_IS", "VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN",
  "VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS",
  "VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN", "VR_DEF_WHEN_SA_OS_IS",
  "VR_DEF_WHEN_SA_OS_IS_IN", "VR_DEF_WHEN_SA_PROCESS_NAME_IS",
  "VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN", "VR_DEF_WHEN_SA_USER_NAME_IS",
  "VR_DEF_WHEN_SA_USER_NAME_IS_IN", "VR_DEF_WHEN_NET_TCP_IPSRC_IS",
  "VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN", "VR_DEF_WHEN_NET_RESULT_IS",
  "VR_DEF_WHEN_NET_TCP_SESID_IS", "VR_DEF_WHEN_NET_TCP_SESID_IS_IN",
  "VR_DEF_CALL_LISP_FUNCTION", "VR_DEF_WHEN_VREP_GET",
  "VR_DEF_WHEN_VREP_EXISTS", "VR_DEF_WHEN_VREP_FREQ",
  "VR_DEF_WHEN_VREP_COUNT", "VR_DEF_WHEN_VREP_KEY_IS",
  "VR_DEF_WHEN_VREP_KEY_IS_IN", "VR_DEF_ACTION_VREP_CREATE",
  "VR_DEF_ACTION_VREP_MCREATE", "VR_DEF_ACTION_VREP_PLUS_CREATE",
  "VR_DEF_ACTION_VREP_FREQ", "VR_DEF_ACTION_VREP_COUNT",
  "VR_DEF_ACTION_VREP_KEY", "VR_DEF_ACTION_VREP_KV_PAIR",
  "\"KeyValue pair\"", "ACTION_KEY_LIST", "ACTION_DATA_LIST",
  "ACTION_ENABLE_VR", "ACTION_DISABLE_VR", "ACTION_ENABLE_AR",
  "ACTION_DISABLE_AR", "ACTION_SETADD", "ACTION_SETDEL", "ACTION_SETVAR",
  "OBS_EVENT_ACM_LIGHT", "VR_DEF_IS_A_CLAUSES", "VR_DEF_IS_A_EXPR",
  "VR_DEF_IS_A_TERM", "VR_DEF_IS_A_FACTOR", "VR_DEF_IS_A_WHEN_WITH",
  "VR_DEF_IS_LOP_OR", "VR_DEF_IS_LOP_AND", "VR_DEF_IS_LOP_NEXT",
  "VR_DEF_IS_LISTA_WHEN", "VR_DEF_IS_LISTA_WITH", "BASIC_ACCESS_RULES",
  "EXTENDED_ACCESS_RULES", "ACCESS_OPERATIONS", "\"Add header to answer\"",
  "\"Add header to request\"", "\"Add var header to var answer\"",
  "\"Add var header to var request\"", "\"Add cookie to answer\"",
  "\"Add cookie to request\"", "\"Add var cookie to var answer\"",
  "\"Add var cookie to var request\"",
  "\"sostituisce l'header utilizzando un pcre\"",
  "REPLACE_HEADER_IN_ANSWER", "\"pattern\"", "'T'", "'t'", "'F'", "'f'",
  "$accept", "program", "ar_definition", "access_operations",
  "access_operation", "access_rules", "vr_definition", "clauses", "expr",
  "term", "factor", "sezione_lista_with", "lista_with", "observable",
  "observables_list", "cat_observables", "ar_observable",
  "ar_observables_list", "ar_cat_observables", "lista_when",
  "pattern_extractor", "item_when", "basic_item_when",
  "basic_access_rules", "item_basic_access_rule",
  "item_basic_access_rule_with_not", "true_false", "lista_action",
  "item_action", "action_variable_list", "action_variable_item",
  "http_method", "set_definition", "check_or_not_the_server_cert",
  "ssl_server_options", "network_event", "timing_list",
  "ip_with_star_list", "url_with_star_list", "ident_or_string_list",
  "ident_or_string_pairs_list", "observable_list",
  "observable_assign_list", "ident_or_string", "ip_address",
  "ip_or_domain", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
   Parser ::yyrline_[] =
  {
       0,   449,   449,   452,   457,   462,   466,   469,   480,   482,
     486,   498,   501,   504,   507,   510,   514,   518,   521,   524,
     532,   538,   543,   550,   556,   562,   569,   574,   580,   589,
     599,   608,   617,   625,   627,   633,   635,   637,   639,   641,
     643,   645,   647,   654,   657,   670,   672,   675,   677,   680,
     682,   685,   687,   697,   708,   710,   713,   723,   731,   735,
     736,   737,   738,   739,   740,   741,   742,   743,   744,   745,
     746,   747,   748,   749,   750,   751,   752,   753,   754,   755,
     756,   757,   758,   759,   760,   761,   762,   763,   764,   765,
     766,   768,   769,   774,   776,   779,   784,   785,   786,   787,
     788,   789,   790,   791,   792,   793,   794,   795,   796,   797,
     798,   799,   800,   801,   802,   803,   804,   805,   807,   808,
     809,   812,   814,   817,   820,   822,   824,   827,   829,   833,
     835,   839,   841,   843,   845,   847,   849,   851,   853,   855,
     857,   859,   861,   863,   865,   867,   869,   871,   873,   875,
     877,   879,   883,   887,   889,   894,   899,   901,   906,   911,
     915,   920,   923,   928,   933,   937,   944,   948,   953,   958,
     962,   969,   973,   978,   983,   987,   992,   997,   999,  1001,
    1003,  1005,  1007,  1009,  1011,  1013,  1015,  1017,  1019,  1021,
    1023,  1025,  1027,  1029,  1034,  1036,  1045,  1047,  1052,  1054,
    1056,  1058,  1060,  1062,  1066,  1071,  1075,  1080,  1082,  1087,
    1092,  1094,  1099,  1105,  1109,  1114,  1126,  1128,  1130,  1132,
    1134,  1136,  1138,  1143,  1148,  1153,  1158,  1163,  1169,  1174,
    1181,  1183,  1185,  1187,  1189,  1191,  1196,  1198,  1201,  1203,
    1213,  1218,  1237,  1239,  1241,  1243,  1245,  1251,  1256,  1263,
    1269,  1275,  1283,  1288,  1294,  1303,  1311,  1317,  1319,  1322,
    1323,  1324,  1325,  1326,  1327,  1328,  1329,  1330,  1331,  1332,
    1333,  1334,  1335,  1336,  1337,  1338,  1339,  1340,  1341,  1342,
    1343,  1344,  1345,  1346,  1347,  1348,  1349,  1350,  1351,  1352,
    1353,  1356,  1358,  1362,  1364,  1366,  1368,  1370,  1372,  1374,
    1376,  1378,  1382,  1388,  1394,  1415,  1421,  1427,  1465,  1471,
    1492,  1498,  1519,  1525,  1553,  1555,  1557,  1559,  1561,  1564,
    1566,  1576,  1584,  1594,  1596,  1598,  1600,  1602,  1605,  1607,
    1610,  1615,  1622,  1627,  1635,  1640,  1648,  1653,  1661,  1667,
    1674,  1680,  1689,  1691,  1694,  1696,  1699,  1701
  };

  void
   Parser ::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
   Parser ::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 9 "va_parser.y"
} //  VaParser 
#line 4791 "va_parser.cpp"

#line 1703 "va_parser.y"


// Bison expects us to provide implementation - otherwise linker complains
void VaParser::Parser::error(const location &loc , const std::string &message) {
        
    // Location should be initialized inside scanner action, but is not in this example.
    // Let's grab location directly from driver class.
    // cout << "Error: " << message << endl << "Location: " << loc << endl;
    
    cout << "Error: " << message << ". At " << driver.prs_lineno << "(" << driver.location() << ", " << loc << ")" << endl;
}


