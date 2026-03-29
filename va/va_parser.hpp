// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file va_parser.hpp
 ** Define the  VaParser ::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_VA_PARSER_HPP_INCLUDED
# define YY_YY_VA_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 11 "va_parser.y"

    #include <iostream>
    #include <string>
    #include <list>
    #include <stdint.h>
    #include "command.h"
    #include <base64.h>

    using namespace std;

    namespace VaParser {
        class Scanner;
        class Interpreter;
    }

#line 65 "va_parser.hpp"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"
#include <typeinfo>
#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
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

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 9 "va_parser.y"
namespace  VaParser  {
#line 206 "va_parser.hpp"




  /// A Bison parser.
  class  Parser 
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {
      YY_ASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // ar_definition
      // access_operations
      // access_operation
      // access_rules
      // vr_definition
      // clauses
      // expr
      // term
      // factor
      // observable
      // cat_observables
      // ar_observable
      // ar_cat_observables
      // item_when
      // basic_item_when
      // basic_access_rules
      // item_basic_access_rule
      // item_basic_access_rule_with_not
      // item_action
      // action_variable_item
      // set_definition
      char dummy1[sizeof ( Command )];

      // check_or_not_the_server_cert
      char dummy2[sizeof ( bool )];

      // sezione_lista_with
      // lista_with
      // observables_list
      // ar_observables_list
      // lista_when
      // lista_action
      // action_variable_list
      // observable_list
      // observable_assign_list
      char dummy3[sizeof ( list< Command > )];

      // ident_or_string_pairs_list
      char dummy4[sizeof ( list< pair<string, string> > )];

      // ssl_server_options
      // timing_list
      // ip_with_star_list
      // url_with_star_list
      // ident_or_string_list
      char dummy5[sizeof ( list< string > )];

      // "ident"
      // "string"
      // "variable ($string)"
      // "integer number"
      // "ip_with_star"
      // "ip_without_star"
      // "url_with_star"
      // "timing"
      // HTTP_METHOD_GET
      // HTTP_METHOD_POST
      // HTTP_METHOD_HEAD
      // HTTP_METHOD_PUT
      // HTTP_METHOD_DELETE
      // HTTP_METHOD_TRACE
      // HTTP_METHOD_OPTIONS
      // HTTP_METHOD_CONNECT
      // HTTP_METHOD_PATCH
      // OBS_EVENT_ACM_LIGHT
      // pattern_extractor
      // true_false
      // http_method
      // network_event
      // ident_or_string
      // ip_address
      // ip_or_domain
      char dummy6[sizeof ( string )];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOKEN_YYEMPTY = -2,
    TOKEN_END = 0,                 // "end of file"
    TOKEN_YYerror = 256,           // error
    TOKEN_YYUNDEF = 257,           // "invalid token"
    TOKEN_IDENT = 258,             // "ident"
    TOKEN_STRING = 259,            // "string"
    TOKEN_VARIABLE = 260,          // "variable ($string)"
    TOKEN_I_NUMBER = 261,          // "integer number"
    TOKEN_IP_WITH_STAR = 262,      // "ip_with_star"
    TOKEN_IP_WITHOUT_STAR = 263,   // "ip_without_star"
    TOKEN_URL_WITH_STAR = 264,     // "url_with_star"
    TOKEN_TIMING = 265,            // "timing"
    TOKEN_EQUAL = 266,             // "equal"
    TOKEN_LEFTPAR = 267,           // "leftpar"
    TOKEN_RIGHTPAR = 268,          // "rightpar"
    TOKEN_SEMICOLON = 269,         // "semicolon"
    TOKEN_COMMA = 270,             // "comma"
    TOKEN_LEFTSQPAR = 271,         // "leftsqpar"
    TOKEN_RIGHTSQPAR = 272,        // "rightsqpar"
    TOKEN_LEFTGRPAR = 273,         // "leftgrpar"
    TOKEN_RIGHTGRPAR = 274,        // "rightsgrpar"
    TOKEN_DOT = 275,               // "."
    TOKEN_DOLLAR = 276,            // "$"
    TOKEN_OR = 277,                // " or "
    TOKEN_AND = 278,               // " and "
    TOKEN_NEXT = 279,              // " next "
    TOKEN_ADD = 280,               // " add "
    TOKEN_DEL = 281,               // " del "
    TOKEN_TO = 282,                // " to "
    TOKEN_FROM = 283,              // " from "
    TOKEN_VEXPIRY = 284,           // "vexpiry"
    TOKEN_TCP_REDIRECT = 285,      // "TCP_REDIRECT"
    TOKEN_TIME = 286,              // "time"
    TOKEN_TCP_REDIRECT_STRING = 287, // TCP_REDIRECT_STRING
    TOKEN_TCP_REDIRECT_VSTRING = 288, // TCP_REDIRECT_VSTRING
    TOKEN_TCP_REDIRECT_HOST = 289, // TCP_REDIRECT_HOST
    TOKEN_TCP_REDIRECT_VHOST = 290, // TCP_REDIRECT_VHOST
    TOKEN_SCM_MANAGE = 291,        // "scm.manage"
    TOKEN_SCM_GIVE = 292,          // "scm.give"
    TOKEN_HTTP_REDIRECT = 293,     // "HTTP_REDIRECT"
    TOKEN_HTTP_VREDIRECT = 294,    // "HTTP_VREDIRECT"
    TOKEN_BANDWIDTH = 295,         // "BANDWIDTH"
    TOKEN_VBANDWIDTH = 296,        // "VBANDWIDTH"
    TOKEN_ANSWER = 297,            // "ANSWER"
    TOKEN_VANSWER = 298,           // "VANSWER"
    TOKEN_VSCM_MANAGE = 299,       // "vscm.manage"
    TOKEN_VSCM_GIVE = 300,         // "vscm.give"
    TOKEN_TRACE = 301,             // "trace"
    TOKEN_NOT = 302,               // "!"
    TOKEN_WHEN = 303,              // "when"
    TOKEN_IF = 304,                // "if"
    TOKEN_THEN = 305,              // "then"
    TOKEN_ENABLED = 306,           // "enabled"
    TOKEN_DISABLED = 307,          // "disabled"
    TOKEN_DEFAULT = 308,           // "default"
    TOKEN_GT = 309,                // ">"
    TOKEN_LT = 310,                // "<"
    TOKEN_GTE = 311,               // ">="
    TOKEN_LTE = 312,               // "<="
    TOKEN_PLUS = 313,              // "+"
    TOKEN_MINUS = 314,             // "-"
    TOKEN_AT = 315,                // "@"
    TOKEN_TRUE = 316,              // "true"
    TOKEN_FALSE = 317,             // "false"
    TOKEN_SET = 318,               // "set"
    TOKEN_DICTIONARY = 319,        // "dictionary"
    TOKEN_IPSET = 320,             // "ipset"
    TOKEN_URLSET = 321,            // "urlset"
    TOKEN_TIMESET = 322,           // "timeset"
    TOKEN_PARSET = 323,            // "parset"
    TOKEN_DEFINE = 324,            // "define"
    TOKEN_MYSQL = 325,             // "mysql"
    TOKEN_AGENT_ID = 326,          // "agent id"
    TOKEN_VR = 327,                // "validation rule"
    TOKEN_VREP = 328,              // "validation report"
    TOKEN_REPORT = 329,            // "report"
    TOKEN_MREPORT = 330,           // "mreport"
    TOKEN_CONDITION = 331,         // "condition"
    TOKEN_SEQUENCE = 332,          // "sequence"
    TOKEN_OF = 333,                // "of"
    TOKEN_KEY = 334,               // "key"
    TOKEN_FREQ = 335,              // "frequence"
    TOKEN_COUNT = 336,             // "counter"
    TOKEN_VAR = 337,               // "var"
    TOKEN_ACTION = 338,            // "action"
    TOKEN_IS = 339,                // "is"
    TOKEN_IN = 340,                // "in"
    TOKEN_ALWAYS = 341,            // "always"
    TOKEN_HTTP_HOST = 342,         // "HTTP HOST"
    TOKEN_HTTP_URL = 343,          // "HTTP URL"
    TOKEN_HTTP_URI = 344,          // "HTTP URI"
    TOKEN_HTTP_URL_HOST = 345,     // "HTTP host in the url"
    TOKEN_HTTP_HEADER_HOST = 346,  // "HTTP host in the headers"
    TOKEN_HTTP_HEADER_REPLACE = 347, // "In tutti gli header esegue, compresa url, esegue il replace"
    TOKEN_HTTP_ANSWER_HEADER_REPLACE = 348, // HTTP_ANSWER_HEADER_REPLACE
    TOKEN_CALL = 349,              // "Calls a lisp function"
    TOKEN_WITH = 350,              // "Parameters for a lisp function"
    TOKEN_PROGRAM = 351,           // "program"
    TOKEN_SET_LIST_ID = 352,       // SET_LIST_ID
    TOKEN_DICTIONARY_LIST_ID = 353, // DICTIONARY_LIST_ID
    TOKEN_SET_MYSQL = 354,         // SET_MYSQL
    TOKEN_SET_LIST_IPSTAR = 355,   // SET_LIST_IPSTAR
    TOKEN_SET_LIST_URLSTAR = 356,  // SET_LIST_URLSTAR
    TOKEN_SET_LIST_TIMING = 357,   // SET_LIST_TIMING
    TOKEN_VR_DEF = 358,            // VR_DEF
    TOKEN_VR_DEF_WHEN = 359,       // VR_DEF_WHEN
    TOKEN_VR_DEF_ACTION = 360,     // VR_DEF_ACTION
    TOKEN_VR_DEF_WHEN_AGENT_ID_IS = 361, // VR_DEF_WHEN_AGENT_ID_IS
    TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN = 362, // VR_DEF_WHEN_AGENT_ID_IS_IN
    TOKEN_VR_DEF_WHEN_OBS_NAME_IS = 363, // VR_DEF_WHEN_OBS_NAME_IS
    TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN = 364, // VR_DEF_WHEN_OBS_NAME_IS_IN
    TOKEN_VR_DEF_WHEN_OBS_EVENT_IS = 365, // VR_DEF_WHEN_OBS_EVENT_IS
    TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN = 366, // VR_DEF_WHEN_OBS_EVENT_IS_IN
    TOKEN_VR_DEF_WHEN_IPPORT_IS = 367, // VR_DEF_WHEN_IPPORT_IS
    TOKEN_VR_DEF_WHEN_IPPORT_IS_IN = 368, // VR_DEF_WHEN_IPPORT_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS = 369, // VR_DEF_WHEN_HTTP_METHOD_IS
    TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN = 370, // VR_DEF_WHEN_HTTP_METHOD_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_HOST_IS = 371, // VR_DEF_WHEN_HTTP_HOST_IS
    TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN = 372, // VR_DEF_WHEN_HTTP_HOST_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_URL_IS = 373, // VR_DEF_WHEN_HTTP_URL_IS
    TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN = 374, // VR_DEF_WHEN_HTTP_URL_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_URI_IS = 375, // VR_DEF_WHEN_HTTP_URI_IS
    TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN = 376, // VR_DEF_WHEN_HTTP_URI_IS_IN
    TOKEN_VR_EMPTY = 377,          // VR_EMPTY
    TOKEN_AR_DEF = 378,            // AR_DEF
    TOKEN_AR_DEF_WHEN = 379,       // AR_DEF_WHEN
    TOKEN_AR_DEF_ACTION = 380,     // AR_DEF_ACTION
    TOKEN_CAT = 381,               // CAT
    TOKEN_OBS_EVENT = 382,         // "OBS_EVENT"
    TOKEN_NET_TCP_CONNECT = 383,   // "NET_TCP_CONNECT"
    TOKEN_NET_TCP_ACCEPT = 384,    // "NET_TCP_ACCEPT"
    TOKEN_NET_TCP_SEND = 385,      // "NET_TCP_SEND"
    TOKEN_NET_TCP_RECV = 386,      // "NET_TCP_RECEIVE"
    TOKEN_NET_TCP_CLOSE = 387,     // "NET_TCP_CLOSE"
    TOKEN_NET_TCP_IPSRC = 388,     // "NET_TCP_IPSRC"
    TOKEN_NET_TCP_IPDST = 389,     // "NET_TCP_IPDST"
    TOKEN_NET_TCP_SESID = 390,     // "NET_TCP_SESID"
    TOKEN_NET_RESULT = 391,        // "NET_RESULT"
    TOKEN_OBS_NAME = 392,          // "OBS_NAME"
    TOKEN_OBS_TIME = 393,          // "OBS_TIME"
    TOKEN_OBS_OBSID = 394,         // "OBS_OBSID"
    TOKEN_OBS_STIME = 395,         // "OBS_STIME"
    TOKEN_OBS_ETIME = 396,         // "OBS_ETIME"
    TOKEN_OBS_TAG = 397,           // "OBS_TAG"
    TOKEN_DATA = 398,              // "Data"
    TOKEN_MAKE = 399,              // "Make"
    TOKEN_AR = 400,                // "Access rule"
    TOKEN_ENABLE = 401,            // "Enable"
    TOKEN_DISABLE = 402,           // "Disable"
    TOKEN_MYSQL_APPEND = 403,      // "Mysql.append"
    TOKEN_MYSQL_APPEND_VARIABLES = 404, // MYSQL_APPEND_VARIABLES
    TOKEN_MYSQL_UPDATE = 405,      // "Mysql.update"
    TOKEN_HTTP_METHOD = 406,       // HTTP_METHOD
    TOKEN_HTTP_METHOD_GET = 407,   // HTTP_METHOD_GET
    TOKEN_HTTP_METHOD_POST = 408,  // HTTP_METHOD_POST
    TOKEN_HTTP_METHOD_HEAD = 409,  // HTTP_METHOD_HEAD
    TOKEN_HTTP_METHOD_PUT = 410,   // HTTP_METHOD_PUT
    TOKEN_HTTP_METHOD_DELETE = 411, // HTTP_METHOD_DELETE
    TOKEN_HTTP_METHOD_TRACE = 412, // HTTP_METHOD_TRACE
    TOKEN_HTTP_METHOD_OPTIONS = 413, // HTTP_METHOD_OPTIONS
    TOKEN_HTTP_METHOD_CONNECT = 414, // HTTP_METHOD_CONNECT
    TOKEN_HTTP_METHOD_PATCH = 415, // HTTP_METHOD_PATCH
    TOKEN_HTTP_COOKIE = 416,       // HTTP_COOKIE
    TOKEN_HTTP_HEADER = 417,       // HTTP_HEADER
    TOKEN_HTTP_PORT = 418,         // HTTP_PORT
    TOKEN_HTTP_QUERY = 419,        // HTTP_QUERY
    TOKEN_HTTP_DATA = 420,         // HTTP_DATA
    TOKEN_HTTP_ANSWER_DATA = 421,  // HTTP_ANSWER_DATA
    TOKEN_HTTP_ANSWER = 422,       // HTTP_ANSWER
    TOKEN_HTTP_ANSWER_CODE = 423,  // HTTP_ANSWER_CODE
    TOKEN_HTTP_ANSWER_HEADER = 424, // HTTP_ANSWER_HEADER
    TOKEN_HTTP_ANSWER_COOKIE = 425, // HTTP_ANSWER_COOKIE
    TOKEN_SA_OS = 426,             // SA_OS
    TOKEN_SA_LOGGED_USER = 427,    // SA_LOGGED_USER
    TOKEN_SA_PROCESS_NAME = 428,   // SA_PROCESS_NAME
    TOKEN_SA_PROCESS_ATTACH = 429, // SA_PROCESS_ATTACH
    TOKEN_EXISTS = 430,            // EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS = 431, // VR_DEF_WHEN_HTTP_COOKIE_IS
    TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN = 432, // VR_DEF_WHEN_HTTP_COOKIE_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS = 433, // VR_DEF_WHEN_HTTP_COOKIE_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS = 434, // VR_DEF_WHEN_HTTP_HEADER_IS
    TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN = 435, // VR_DEF_WHEN_HTTP_HEADER_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS = 436, // VR_DEF_WHEN_HTTP_HEADER_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS = 437, // VR_DEF_WHEN_HTTP_QUERY_IS
    TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN = 438, // VR_DEF_WHEN_HTTP_QUERY_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS = 439, // VR_DEF_WHEN_HTTP_QUERY_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS = 440, // VR_DEF_WHEN_HTTP_ANSWER_CODE_IS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN = 441, // VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS = 442, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN = 443, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS = 444, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS = 445, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN = 446, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS = 447, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_DATA_IS = 448, // VR_DEF_WHEN_HTTP_DATA_IS
    TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN = 449, // VR_DEF_WHEN_HTTP_DATA_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS = 450, // VR_DEF_WHEN_HTTP_DATA_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS = 451, // VR_DEF_WHEN_HTTP_ANSWER_DATA_IS
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN = 452, // VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS = 453, // VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS
    TOKEN_VR_DEF_WHEN_OBS_TAG_IS = 454, // VR_DEF_WHEN_OBS_TAG_IS
    TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN = 455, // VR_DEF_WHEN_OBS_TAG_IS_IN
    TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS = 456, // VR_DEF_WHEN_OBS_TAG_EXISTS
    TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS = 457, // VR_DEF_WHEN_HTTP_URL_HOST_ID_IS
    TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN = 458, // VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN
    TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS = 459, // VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS
    TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN = 460, // VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN
    TOKEN_VR_DEF_WHEN_SA_OS_IS = 461, // VR_DEF_WHEN_SA_OS_IS
    TOKEN_VR_DEF_WHEN_SA_OS_IS_IN = 462, // VR_DEF_WHEN_SA_OS_IS_IN
    TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS = 463, // VR_DEF_WHEN_SA_PROCESS_NAME_IS
    TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN = 464, // VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN
    TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS = 465, // VR_DEF_WHEN_SA_USER_NAME_IS
    TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN = 466, // VR_DEF_WHEN_SA_USER_NAME_IS_IN
    TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS = 467, // VR_DEF_WHEN_NET_TCP_IPSRC_IS
    TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN = 468, // VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN
    TOKEN_VR_DEF_WHEN_NET_RESULT_IS = 469, // VR_DEF_WHEN_NET_RESULT_IS
    TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS = 470, // VR_DEF_WHEN_NET_TCP_SESID_IS
    TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN = 471, // VR_DEF_WHEN_NET_TCP_SESID_IS_IN
    TOKEN_VR_DEF_CALL_LISP_FUNCTION = 472, // VR_DEF_CALL_LISP_FUNCTION
    TOKEN_VR_DEF_WHEN_VREP_GET = 473, // VR_DEF_WHEN_VREP_GET
    TOKEN_VR_DEF_WHEN_VREP_EXISTS = 474, // VR_DEF_WHEN_VREP_EXISTS
    TOKEN_VR_DEF_WHEN_VREP_FREQ = 475, // VR_DEF_WHEN_VREP_FREQ
    TOKEN_VR_DEF_WHEN_VREP_COUNT = 476, // VR_DEF_WHEN_VREP_COUNT
    TOKEN_VR_DEF_WHEN_VREP_KEY_IS = 477, // VR_DEF_WHEN_VREP_KEY_IS
    TOKEN_VR_DEF_WHEN_VREP_KEY_IS_IN = 478, // VR_DEF_WHEN_VREP_KEY_IS_IN
    TOKEN_VR_DEF_ACTION_VREP_CREATE = 479, // VR_DEF_ACTION_VREP_CREATE
    TOKEN_VR_DEF_ACTION_VREP_MCREATE = 480, // VR_DEF_ACTION_VREP_MCREATE
    TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE = 481, // VR_DEF_ACTION_VREP_PLUS_CREATE
    TOKEN_VR_DEF_ACTION_VREP_FREQ = 482, // VR_DEF_ACTION_VREP_FREQ
    TOKEN_VR_DEF_ACTION_VREP_COUNT = 483, // VR_DEF_ACTION_VREP_COUNT
    TOKEN_VR_DEF_ACTION_VREP_KEY = 484, // VR_DEF_ACTION_VREP_KEY
    TOKEN_VR_DEF_ACTION_VREP_KV_PAIR = 485, // VR_DEF_ACTION_VREP_KV_PAIR
    TOKEN_KV = 486,                // "KeyValue pair"
    TOKEN_ACTION_KEY_LIST = 487,   // ACTION_KEY_LIST
    TOKEN_ACTION_DATA_LIST = 488,  // ACTION_DATA_LIST
    TOKEN_ACTION_ENABLE_VR = 489,  // ACTION_ENABLE_VR
    TOKEN_ACTION_DISABLE_VR = 490, // ACTION_DISABLE_VR
    TOKEN_ACTION_ENABLE_AR = 491,  // ACTION_ENABLE_AR
    TOKEN_ACTION_DISABLE_AR = 492, // ACTION_DISABLE_AR
    TOKEN_ACTION_SETADD = 493,     // ACTION_SETADD
    TOKEN_ACTION_SETDEL = 494,     // ACTION_SETDEL
    TOKEN_ACTION_SETVAR = 495,     // ACTION_SETVAR
    TOKEN_OBS_EVENT_ACM_LIGHT = 496, // OBS_EVENT_ACM_LIGHT
    TOKEN_VR_DEF_IS_A_CLAUSES = 497, // VR_DEF_IS_A_CLAUSES
    TOKEN_VR_DEF_IS_A_EXPR = 498,  // VR_DEF_IS_A_EXPR
    TOKEN_VR_DEF_IS_A_TERM = 499,  // VR_DEF_IS_A_TERM
    TOKEN_VR_DEF_IS_A_FACTOR = 500, // VR_DEF_IS_A_FACTOR
    TOKEN_VR_DEF_IS_A_WHEN_WITH = 501, // VR_DEF_IS_A_WHEN_WITH
    TOKEN_VR_DEF_IS_LOP_OR = 502,  // VR_DEF_IS_LOP_OR
    TOKEN_VR_DEF_IS_LOP_AND = 503, // VR_DEF_IS_LOP_AND
    TOKEN_VR_DEF_IS_LOP_NEXT = 504, // VR_DEF_IS_LOP_NEXT
    TOKEN_VR_DEF_IS_LISTA_WHEN = 505, // VR_DEF_IS_LISTA_WHEN
    TOKEN_VR_DEF_IS_LISTA_WITH = 506, // VR_DEF_IS_LISTA_WITH
    TOKEN_BASIC_ACCESS_RULES = 507, // BASIC_ACCESS_RULES
    TOKEN_EXTENDED_ACCESS_RULES = 508, // EXTENDED_ACCESS_RULES
    TOKEN_ACCESS_OPERATIONS = 509, // ACCESS_OPERATIONS
    TOKEN_ADD_HEADER_TO_ANSWER = 510, // "Add header to answer"
    TOKEN_ADD_HEADER_TO_REQUEST = 511, // "Add header to request"
    TOKEN_ADD_V_HEADER_TO_V_ANSWER = 512, // "Add var header to var answer"
    TOKEN_ADD_V_HEADER_TO_V_REQUEST = 513, // "Add var header to var request"
    TOKEN_ADD_COOKIE_TO_ANSWER = 514, // "Add cookie to answer"
    TOKEN_ADD_COOKIE_TO_REQUEST = 515, // "Add cookie to request"
    TOKEN_ADD_V_COOKIE_TO_V_ANSWER = 516, // "Add var cookie to var answer"
    TOKEN_ADD_V_COOKIE_TO_V_REQUEST = 517, // "Add var cookie to var request"
    TOKEN_REPLACE_HEADER_IN_REQUEST = 518, // "sostituisce l'header utilizzando un pcre"
    TOKEN_REPLACE_HEADER_IN_ANSWER = 519, // REPLACE_HEADER_IN_ANSWER
    TOKEN_PATTERN = 520            // "pattern"
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 270, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_IDENT = 3,                             // "ident"
        S_STRING = 4,                            // "string"
        S_VARIABLE = 5,                          // "variable ($string)"
        S_I_NUMBER = 6,                          // "integer number"
        S_IP_WITH_STAR = 7,                      // "ip_with_star"
        S_IP_WITHOUT_STAR = 8,                   // "ip_without_star"
        S_URL_WITH_STAR = 9,                     // "url_with_star"
        S_TIMING = 10,                           // "timing"
        S_EQUAL = 11,                            // "equal"
        S_LEFTPAR = 12,                          // "leftpar"
        S_RIGHTPAR = 13,                         // "rightpar"
        S_SEMICOLON = 14,                        // "semicolon"
        S_COMMA = 15,                            // "comma"
        S_LEFTSQPAR = 16,                        // "leftsqpar"
        S_RIGHTSQPAR = 17,                       // "rightsqpar"
        S_LEFTGRPAR = 18,                        // "leftgrpar"
        S_RIGHTGRPAR = 19,                       // "rightsgrpar"
        S_DOT = 20,                              // "."
        S_DOLLAR = 21,                           // "$"
        S_OR = 22,                               // " or "
        S_AND = 23,                              // " and "
        S_NEXT = 24,                             // " next "
        S_ADD = 25,                              // " add "
        S_DEL = 26,                              // " del "
        S_TO = 27,                               // " to "
        S_FROM = 28,                             // " from "
        S_VEXPIRY = 29,                          // "vexpiry"
        S_TCP_REDIRECT = 30,                     // "TCP_REDIRECT"
        S_TIME = 31,                             // "time"
        S_TCP_REDIRECT_STRING = 32,              // TCP_REDIRECT_STRING
        S_TCP_REDIRECT_VSTRING = 33,             // TCP_REDIRECT_VSTRING
        S_TCP_REDIRECT_HOST = 34,                // TCP_REDIRECT_HOST
        S_TCP_REDIRECT_VHOST = 35,               // TCP_REDIRECT_VHOST
        S_SCM_MANAGE = 36,                       // "scm.manage"
        S_SCM_GIVE = 37,                         // "scm.give"
        S_HTTP_REDIRECT = 38,                    // "HTTP_REDIRECT"
        S_HTTP_VREDIRECT = 39,                   // "HTTP_VREDIRECT"
        S_BANDWIDTH = 40,                        // "BANDWIDTH"
        S_VBANDWIDTH = 41,                       // "VBANDWIDTH"
        S_ANSWER = 42,                           // "ANSWER"
        S_VANSWER = 43,                          // "VANSWER"
        S_VSCM_MANAGE = 44,                      // "vscm.manage"
        S_VSCM_GIVE = 45,                        // "vscm.give"
        S_TRACE = 46,                            // "trace"
        S_NOT = 47,                              // "!"
        S_WHEN = 48,                             // "when"
        S_IF = 49,                               // "if"
        S_THEN = 50,                             // "then"
        S_ENABLED = 51,                          // "enabled"
        S_DISABLED = 52,                         // "disabled"
        S_DEFAULT = 53,                          // "default"
        S_GT = 54,                               // ">"
        S_LT = 55,                               // "<"
        S_GTE = 56,                              // ">="
        S_LTE = 57,                              // "<="
        S_PLUS = 58,                             // "+"
        S_MINUS = 59,                            // "-"
        S_AT = 60,                               // "@"
        S_TRUE = 61,                             // "true"
        S_FALSE = 62,                            // "false"
        S_SET = 63,                              // "set"
        S_DICTIONARY = 64,                       // "dictionary"
        S_IPSET = 65,                            // "ipset"
        S_URLSET = 66,                           // "urlset"
        S_TIMESET = 67,                          // "timeset"
        S_PARSET = 68,                           // "parset"
        S_DEFINE = 69,                           // "define"
        S_MYSQL = 70,                            // "mysql"
        S_AGENT_ID = 71,                         // "agent id"
        S_VR = 72,                               // "validation rule"
        S_VREP = 73,                             // "validation report"
        S_REPORT = 74,                           // "report"
        S_MREPORT = 75,                          // "mreport"
        S_CONDITION = 76,                        // "condition"
        S_SEQUENCE = 77,                         // "sequence"
        S_OF = 78,                               // "of"
        S_KEY = 79,                              // "key"
        S_FREQ = 80,                             // "frequence"
        S_COUNT = 81,                            // "counter"
        S_VAR = 82,                              // "var"
        S_ACTION = 83,                           // "action"
        S_IS = 84,                               // "is"
        S_IN = 85,                               // "in"
        S_ALWAYS = 86,                           // "always"
        S_HTTP_HOST = 87,                        // "HTTP HOST"
        S_HTTP_URL = 88,                         // "HTTP URL"
        S_HTTP_URI = 89,                         // "HTTP URI"
        S_HTTP_URL_HOST = 90,                    // "HTTP host in the url"
        S_HTTP_HEADER_HOST = 91,                 // "HTTP host in the headers"
        S_HTTP_HEADER_REPLACE = 92,              // "In tutti gli header esegue, compresa url, esegue il replace"
        S_HTTP_ANSWER_HEADER_REPLACE = 93,       // HTTP_ANSWER_HEADER_REPLACE
        S_CALL = 94,                             // "Calls a lisp function"
        S_WITH = 95,                             // "Parameters for a lisp function"
        S_PROGRAM = 96,                          // "program"
        S_SET_LIST_ID = 97,                      // SET_LIST_ID
        S_DICTIONARY_LIST_ID = 98,               // DICTIONARY_LIST_ID
        S_SET_MYSQL = 99,                        // SET_MYSQL
        S_SET_LIST_IPSTAR = 100,                 // SET_LIST_IPSTAR
        S_SET_LIST_URLSTAR = 101,                // SET_LIST_URLSTAR
        S_SET_LIST_TIMING = 102,                 // SET_LIST_TIMING
        S_VR_DEF = 103,                          // VR_DEF
        S_VR_DEF_WHEN = 104,                     // VR_DEF_WHEN
        S_VR_DEF_ACTION = 105,                   // VR_DEF_ACTION
        S_VR_DEF_WHEN_AGENT_ID_IS = 106,         // VR_DEF_WHEN_AGENT_ID_IS
        S_VR_DEF_WHEN_AGENT_ID_IS_IN = 107,      // VR_DEF_WHEN_AGENT_ID_IS_IN
        S_VR_DEF_WHEN_OBS_NAME_IS = 108,         // VR_DEF_WHEN_OBS_NAME_IS
        S_VR_DEF_WHEN_OBS_NAME_IS_IN = 109,      // VR_DEF_WHEN_OBS_NAME_IS_IN
        S_VR_DEF_WHEN_OBS_EVENT_IS = 110,        // VR_DEF_WHEN_OBS_EVENT_IS
        S_VR_DEF_WHEN_OBS_EVENT_IS_IN = 111,     // VR_DEF_WHEN_OBS_EVENT_IS_IN
        S_VR_DEF_WHEN_IPPORT_IS = 112,           // VR_DEF_WHEN_IPPORT_IS
        S_VR_DEF_WHEN_IPPORT_IS_IN = 113,        // VR_DEF_WHEN_IPPORT_IS_IN
        S_VR_DEF_WHEN_HTTP_METHOD_IS = 114,      // VR_DEF_WHEN_HTTP_METHOD_IS
        S_VR_DEF_WHEN_HTTP_METHOD_IS_IN = 115,   // VR_DEF_WHEN_HTTP_METHOD_IS_IN
        S_VR_DEF_WHEN_HTTP_HOST_IS = 116,        // VR_DEF_WHEN_HTTP_HOST_IS
        S_VR_DEF_WHEN_HTTP_HOST_IS_IN = 117,     // VR_DEF_WHEN_HTTP_HOST_IS_IN
        S_VR_DEF_WHEN_HTTP_URL_IS = 118,         // VR_DEF_WHEN_HTTP_URL_IS
        S_VR_DEF_WHEN_HTTP_URL_IS_IN = 119,      // VR_DEF_WHEN_HTTP_URL_IS_IN
        S_VR_DEF_WHEN_HTTP_URI_IS = 120,         // VR_DEF_WHEN_HTTP_URI_IS
        S_VR_DEF_WHEN_HTTP_URI_IS_IN = 121,      // VR_DEF_WHEN_HTTP_URI_IS_IN
        S_VR_EMPTY = 122,                        // VR_EMPTY
        S_AR_DEF = 123,                          // AR_DEF
        S_AR_DEF_WHEN = 124,                     // AR_DEF_WHEN
        S_AR_DEF_ACTION = 125,                   // AR_DEF_ACTION
        S_CAT = 126,                             // CAT
        S_OBS_EVENT = 127,                       // "OBS_EVENT"
        S_NET_TCP_CONNECT = 128,                 // "NET_TCP_CONNECT"
        S_NET_TCP_ACCEPT = 129,                  // "NET_TCP_ACCEPT"
        S_NET_TCP_SEND = 130,                    // "NET_TCP_SEND"
        S_NET_TCP_RECV = 131,                    // "NET_TCP_RECEIVE"
        S_NET_TCP_CLOSE = 132,                   // "NET_TCP_CLOSE"
        S_NET_TCP_IPSRC = 133,                   // "NET_TCP_IPSRC"
        S_NET_TCP_IPDST = 134,                   // "NET_TCP_IPDST"
        S_NET_TCP_SESID = 135,                   // "NET_TCP_SESID"
        S_NET_RESULT = 136,                      // "NET_RESULT"
        S_OBS_NAME = 137,                        // "OBS_NAME"
        S_OBS_TIME = 138,                        // "OBS_TIME"
        S_OBS_OBSID = 139,                       // "OBS_OBSID"
        S_OBS_STIME = 140,                       // "OBS_STIME"
        S_OBS_ETIME = 141,                       // "OBS_ETIME"
        S_OBS_TAG = 142,                         // "OBS_TAG"
        S_DATA = 143,                            // "Data"
        S_MAKE = 144,                            // "Make"
        S_AR = 145,                              // "Access rule"
        S_ENABLE = 146,                          // "Enable"
        S_DISABLE = 147,                         // "Disable"
        S_MYSQL_APPEND = 148,                    // "Mysql.append"
        S_MYSQL_APPEND_VARIABLES = 149,          // MYSQL_APPEND_VARIABLES
        S_MYSQL_UPDATE = 150,                    // "Mysql.update"
        S_HTTP_METHOD = 151,                     // HTTP_METHOD
        S_HTTP_METHOD_GET = 152,                 // HTTP_METHOD_GET
        S_HTTP_METHOD_POST = 153,                // HTTP_METHOD_POST
        S_HTTP_METHOD_HEAD = 154,                // HTTP_METHOD_HEAD
        S_HTTP_METHOD_PUT = 155,                 // HTTP_METHOD_PUT
        S_HTTP_METHOD_DELETE = 156,              // HTTP_METHOD_DELETE
        S_HTTP_METHOD_TRACE = 157,               // HTTP_METHOD_TRACE
        S_HTTP_METHOD_OPTIONS = 158,             // HTTP_METHOD_OPTIONS
        S_HTTP_METHOD_CONNECT = 159,             // HTTP_METHOD_CONNECT
        S_HTTP_METHOD_PATCH = 160,               // HTTP_METHOD_PATCH
        S_HTTP_COOKIE = 161,                     // HTTP_COOKIE
        S_HTTP_HEADER = 162,                     // HTTP_HEADER
        S_HTTP_PORT = 163,                       // HTTP_PORT
        S_HTTP_QUERY = 164,                      // HTTP_QUERY
        S_HTTP_DATA = 165,                       // HTTP_DATA
        S_HTTP_ANSWER_DATA = 166,                // HTTP_ANSWER_DATA
        S_HTTP_ANSWER = 167,                     // HTTP_ANSWER
        S_HTTP_ANSWER_CODE = 168,                // HTTP_ANSWER_CODE
        S_HTTP_ANSWER_HEADER = 169,              // HTTP_ANSWER_HEADER
        S_HTTP_ANSWER_COOKIE = 170,              // HTTP_ANSWER_COOKIE
        S_SA_OS = 171,                           // SA_OS
        S_SA_LOGGED_USER = 172,                  // SA_LOGGED_USER
        S_SA_PROCESS_NAME = 173,                 // SA_PROCESS_NAME
        S_SA_PROCESS_ATTACH = 174,               // SA_PROCESS_ATTACH
        S_EXISTS = 175,                          // EXISTS
        S_VR_DEF_WHEN_HTTP_COOKIE_IS = 176,      // VR_DEF_WHEN_HTTP_COOKIE_IS
        S_VR_DEF_WHEN_HTTP_COOKIE_IS_IN = 177,   // VR_DEF_WHEN_HTTP_COOKIE_IS_IN
        S_VR_DEF_WHEN_HTTP_COOKIE_EXISTS = 178,  // VR_DEF_WHEN_HTTP_COOKIE_EXISTS
        S_VR_DEF_WHEN_HTTP_HEADER_IS = 179,      // VR_DEF_WHEN_HTTP_HEADER_IS
        S_VR_DEF_WHEN_HTTP_HEADER_IS_IN = 180,   // VR_DEF_WHEN_HTTP_HEADER_IS_IN
        S_VR_DEF_WHEN_HTTP_HEADER_EXISTS = 181,  // VR_DEF_WHEN_HTTP_HEADER_EXISTS
        S_VR_DEF_WHEN_HTTP_QUERY_IS = 182,       // VR_DEF_WHEN_HTTP_QUERY_IS
        S_VR_DEF_WHEN_HTTP_QUERY_IS_IN = 183,    // VR_DEF_WHEN_HTTP_QUERY_IS_IN
        S_VR_DEF_WHEN_HTTP_QUERY_EXISTS = 184,   // VR_DEF_WHEN_HTTP_QUERY_EXISTS
        S_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS = 185, // VR_DEF_WHEN_HTTP_ANSWER_CODE_IS
        S_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN = 186, // VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN
        S_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS = 187, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS
        S_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN = 188, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN
        S_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS = 189, // VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS
        S_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS = 190, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS
        S_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN = 191, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN
        S_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS = 192, // VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS
        S_VR_DEF_WHEN_HTTP_DATA_IS = 193,        // VR_DEF_WHEN_HTTP_DATA_IS
        S_VR_DEF_WHEN_HTTP_DATA_IS_IN = 194,     // VR_DEF_WHEN_HTTP_DATA_IS_IN
        S_VR_DEF_WHEN_HTTP_DATA_EXISTS = 195,    // VR_DEF_WHEN_HTTP_DATA_EXISTS
        S_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS = 196, // VR_DEF_WHEN_HTTP_ANSWER_DATA_IS
        S_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN = 197, // VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN
        S_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS = 198, // VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS
        S_VR_DEF_WHEN_OBS_TAG_IS = 199,          // VR_DEF_WHEN_OBS_TAG_IS
        S_VR_DEF_WHEN_OBS_TAG_IS_IN = 200,       // VR_DEF_WHEN_OBS_TAG_IS_IN
        S_VR_DEF_WHEN_OBS_TAG_EXISTS = 201,      // VR_DEF_WHEN_OBS_TAG_EXISTS
        S_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS = 202, // VR_DEF_WHEN_HTTP_URL_HOST_ID_IS
        S_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN = 203, // VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN
        S_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS = 204, // VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS
        S_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN = 205, // VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN
        S_VR_DEF_WHEN_SA_OS_IS = 206,            // VR_DEF_WHEN_SA_OS_IS
        S_VR_DEF_WHEN_SA_OS_IS_IN = 207,         // VR_DEF_WHEN_SA_OS_IS_IN
        S_VR_DEF_WHEN_SA_PROCESS_NAME_IS = 208,  // VR_DEF_WHEN_SA_PROCESS_NAME_IS
        S_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN = 209, // VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN
        S_VR_DEF_WHEN_SA_USER_NAME_IS = 210,     // VR_DEF_WHEN_SA_USER_NAME_IS
        S_VR_DEF_WHEN_SA_USER_NAME_IS_IN = 211,  // VR_DEF_WHEN_SA_USER_NAME_IS_IN
        S_VR_DEF_WHEN_NET_TCP_IPSRC_IS = 212,    // VR_DEF_WHEN_NET_TCP_IPSRC_IS
        S_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN = 213, // VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN
        S_VR_DEF_WHEN_NET_RESULT_IS = 214,       // VR_DEF_WHEN_NET_RESULT_IS
        S_VR_DEF_WHEN_NET_TCP_SESID_IS = 215,    // VR_DEF_WHEN_NET_TCP_SESID_IS
        S_VR_DEF_WHEN_NET_TCP_SESID_IS_IN = 216, // VR_DEF_WHEN_NET_TCP_SESID_IS_IN
        S_VR_DEF_CALL_LISP_FUNCTION = 217,       // VR_DEF_CALL_LISP_FUNCTION
        S_VR_DEF_WHEN_VREP_GET = 218,            // VR_DEF_WHEN_VREP_GET
        S_VR_DEF_WHEN_VREP_EXISTS = 219,         // VR_DEF_WHEN_VREP_EXISTS
        S_VR_DEF_WHEN_VREP_FREQ = 220,           // VR_DEF_WHEN_VREP_FREQ
        S_VR_DEF_WHEN_VREP_COUNT = 221,          // VR_DEF_WHEN_VREP_COUNT
        S_VR_DEF_WHEN_VREP_KEY_IS = 222,         // VR_DEF_WHEN_VREP_KEY_IS
        S_VR_DEF_WHEN_VREP_KEY_IS_IN = 223,      // VR_DEF_WHEN_VREP_KEY_IS_IN
        S_VR_DEF_ACTION_VREP_CREATE = 224,       // VR_DEF_ACTION_VREP_CREATE
        S_VR_DEF_ACTION_VREP_MCREATE = 225,      // VR_DEF_ACTION_VREP_MCREATE
        S_VR_DEF_ACTION_VREP_PLUS_CREATE = 226,  // VR_DEF_ACTION_VREP_PLUS_CREATE
        S_VR_DEF_ACTION_VREP_FREQ = 227,         // VR_DEF_ACTION_VREP_FREQ
        S_VR_DEF_ACTION_VREP_COUNT = 228,        // VR_DEF_ACTION_VREP_COUNT
        S_VR_DEF_ACTION_VREP_KEY = 229,          // VR_DEF_ACTION_VREP_KEY
        S_VR_DEF_ACTION_VREP_KV_PAIR = 230,      // VR_DEF_ACTION_VREP_KV_PAIR
        S_KV = 231,                              // "KeyValue pair"
        S_ACTION_KEY_LIST = 232,                 // ACTION_KEY_LIST
        S_ACTION_DATA_LIST = 233,                // ACTION_DATA_LIST
        S_ACTION_ENABLE_VR = 234,                // ACTION_ENABLE_VR
        S_ACTION_DISABLE_VR = 235,               // ACTION_DISABLE_VR
        S_ACTION_ENABLE_AR = 236,                // ACTION_ENABLE_AR
        S_ACTION_DISABLE_AR = 237,               // ACTION_DISABLE_AR
        S_ACTION_SETADD = 238,                   // ACTION_SETADD
        S_ACTION_SETDEL = 239,                   // ACTION_SETDEL
        S_ACTION_SETVAR = 240,                   // ACTION_SETVAR
        S_OBS_EVENT_ACM_LIGHT = 241,             // OBS_EVENT_ACM_LIGHT
        S_VR_DEF_IS_A_CLAUSES = 242,             // VR_DEF_IS_A_CLAUSES
        S_VR_DEF_IS_A_EXPR = 243,                // VR_DEF_IS_A_EXPR
        S_VR_DEF_IS_A_TERM = 244,                // VR_DEF_IS_A_TERM
        S_VR_DEF_IS_A_FACTOR = 245,              // VR_DEF_IS_A_FACTOR
        S_VR_DEF_IS_A_WHEN_WITH = 246,           // VR_DEF_IS_A_WHEN_WITH
        S_VR_DEF_IS_LOP_OR = 247,                // VR_DEF_IS_LOP_OR
        S_VR_DEF_IS_LOP_AND = 248,               // VR_DEF_IS_LOP_AND
        S_VR_DEF_IS_LOP_NEXT = 249,              // VR_DEF_IS_LOP_NEXT
        S_VR_DEF_IS_LISTA_WHEN = 250,            // VR_DEF_IS_LISTA_WHEN
        S_VR_DEF_IS_LISTA_WITH = 251,            // VR_DEF_IS_LISTA_WITH
        S_BASIC_ACCESS_RULES = 252,              // BASIC_ACCESS_RULES
        S_EXTENDED_ACCESS_RULES = 253,           // EXTENDED_ACCESS_RULES
        S_ACCESS_OPERATIONS = 254,               // ACCESS_OPERATIONS
        S_ADD_HEADER_TO_ANSWER = 255,            // "Add header to answer"
        S_ADD_HEADER_TO_REQUEST = 256,           // "Add header to request"
        S_ADD_V_HEADER_TO_V_ANSWER = 257,        // "Add var header to var answer"
        S_ADD_V_HEADER_TO_V_REQUEST = 258,       // "Add var header to var request"
        S_ADD_COOKIE_TO_ANSWER = 259,            // "Add cookie to answer"
        S_ADD_COOKIE_TO_REQUEST = 260,           // "Add cookie to request"
        S_ADD_V_COOKIE_TO_V_ANSWER = 261,        // "Add var cookie to var answer"
        S_ADD_V_COOKIE_TO_V_REQUEST = 262,       // "Add var cookie to var request"
        S_REPLACE_HEADER_IN_REQUEST = 263,       // "sostituisce l'header utilizzando un pcre"
        S_REPLACE_HEADER_IN_ANSWER = 264,        // REPLACE_HEADER_IN_ANSWER
        S_PATTERN = 265,                         // "pattern"
        S_266_T_ = 266,                          // 'T'
        S_267_t_ = 267,                          // 't'
        S_268_F_ = 268,                          // 'F'
        S_269_f_ = 269,                          // 'f'
        S_YYACCEPT = 270,                        // $accept
        S_program = 271,                         // program
        S_ar_definition = 272,                   // ar_definition
        S_access_operations = 273,               // access_operations
        S_access_operation = 274,                // access_operation
        S_access_rules = 275,                    // access_rules
        S_vr_definition = 276,                   // vr_definition
        S_clauses = 277,                         // clauses
        S_expr = 278,                            // expr
        S_term = 279,                            // term
        S_factor = 280,                          // factor
        S_sezione_lista_with = 281,              // sezione_lista_with
        S_lista_with = 282,                      // lista_with
        S_observable = 283,                      // observable
        S_observables_list = 284,                // observables_list
        S_cat_observables = 285,                 // cat_observables
        S_ar_observable = 286,                   // ar_observable
        S_ar_observables_list = 287,             // ar_observables_list
        S_ar_cat_observables = 288,              // ar_cat_observables
        S_lista_when = 289,                      // lista_when
        S_pattern_extractor = 290,               // pattern_extractor
        S_item_when = 291,                       // item_when
        S_basic_item_when = 292,                 // basic_item_when
        S_basic_access_rules = 293,              // basic_access_rules
        S_item_basic_access_rule = 294,          // item_basic_access_rule
        S_item_basic_access_rule_with_not = 295, // item_basic_access_rule_with_not
        S_true_false = 296,                      // true_false
        S_lista_action = 297,                    // lista_action
        S_item_action = 298,                     // item_action
        S_action_variable_list = 299,            // action_variable_list
        S_action_variable_item = 300,            // action_variable_item
        S_http_method = 301,                     // http_method
        S_set_definition = 302,                  // set_definition
        S_check_or_not_the_server_cert = 303,    // check_or_not_the_server_cert
        S_ssl_server_options = 304,              // ssl_server_options
        S_network_event = 305,                   // network_event
        S_timing_list = 306,                     // timing_list
        S_ip_with_star_list = 307,               // ip_with_star_list
        S_url_with_star_list = 308,              // url_with_star_list
        S_ident_or_string_list = 309,            // ident_or_string_list
        S_ident_or_string_pairs_list = 310,      // ident_or_string_pairs_list
        S_observable_list = 311,                 // observable_list
        S_observable_assign_list = 312,          // observable_assign_list
        S_ident_or_string = 313,                 // ident_or_string
        S_ip_address = 314,                      // ip_address
        S_ip_or_domain = 315                     // ip_or_domain
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
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
        value.move<  Command  > (std::move (that.value));
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.move<  bool  > (std::move (that.value));
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
        value.move<  list< Command >  > (std::move (that.value));
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.move<  list< pair<string, string> >  > (std::move (that.value));
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.move<  list< string >  > (std::move (that.value));
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
        value.move<  string  > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  Command && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  Command & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  bool && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  bool & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  list< Command > && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  list< Command > & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  list< pair<string, string> > && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  list< pair<string, string> > & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  list< string > && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  list< string > & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t,  string && v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const  string & v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
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
        value.template destroy<  Command  > ();
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.template destroy<  bool  > ();
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
        value.template destroy<  list< Command >  > ();
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.template destroy<  list< pair<string, string> >  > ();
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.template destroy<  list< string >  > ();
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
        value.template destroy<  string  > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return  Parser ::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT (tok == token::TOKEN_END
                   || (token::TOKEN_YYerror <= tok && tok <= token::TOKEN_YYUNDEF)
                   || (token::TOKEN_EQUAL <= tok && tok <= token::TOKEN_HTTP_METHOD)
                   || (token::TOKEN_HTTP_COOKIE <= tok && tok <= token::TOKEN_ACTION_SETVAR)
                   || (token::TOKEN_VR_DEF_IS_A_CLAUSES <= tok && tok <= token::TOKEN_PATTERN)
                   || tok == 84
                   || tok == 116
                   || tok == 70
                   || tok == 102);
#endif
      }
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok,  string  v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const  string & v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {
#if !defined _MSC_VER || defined __clang__
        YY_ASSERT ((token::TOKEN_IDENT <= tok && tok <= token::TOKEN_TIMING)
                   || (token::TOKEN_HTTP_METHOD_GET <= tok && tok <= token::TOKEN_HTTP_METHOD_PATCH)
                   || tok == token::TOKEN_OBS_EVENT_ACM_LIGHT);
#endif
      }
    };

    /// Build a parser object.
     Parser  (VaParser::Scanner &scanner_yyarg, VaParser::Interpreter &driver_yyarg);
    virtual ~ Parser  ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
     Parser  (const  Parser &) = delete;
    /// Non copyable.
     Parser & operator= (const  Parser &) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_END (location_type l)
      {
        return symbol_type (token::TOKEN_END, std::move (l));
      }
#else
      static
      symbol_type
      make_END (const location_type& l)
      {
        return symbol_type (token::TOKEN_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::TOKEN_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::TOKEN_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENT ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_IDENT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENT (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_IDENT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VARIABLE ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_VARIABLE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_VARIABLE (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_VARIABLE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_I_NUMBER ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_I_NUMBER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_I_NUMBER (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_I_NUMBER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IP_WITH_STAR ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_IP_WITH_STAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IP_WITH_STAR (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_IP_WITH_STAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IP_WITHOUT_STAR ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_IP_WITHOUT_STAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IP_WITHOUT_STAR (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_IP_WITHOUT_STAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_URL_WITH_STAR ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_URL_WITH_STAR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_URL_WITH_STAR (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_URL_WITH_STAR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TIMING ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_TIMING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TIMING (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_TIMING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQUAL (location_type l)
      {
        return symbol_type (token::TOKEN_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_EQUAL (const location_type& l)
      {
        return symbol_type (token::TOKEN_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFTPAR (location_type l)
      {
        return symbol_type (token::TOKEN_LEFTPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFTPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFTPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHTPAR (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHTPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHTPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHTPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::TOKEN_SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::TOKEN_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::TOKEN_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFTSQPAR (location_type l)
      {
        return symbol_type (token::TOKEN_LEFTSQPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFTSQPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFTSQPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHTSQPAR (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHTSQPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHTSQPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHTSQPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LEFTGRPAR (location_type l)
      {
        return symbol_type (token::TOKEN_LEFTGRPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_LEFTGRPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_LEFTGRPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RIGHTGRPAR (location_type l)
      {
        return symbol_type (token::TOKEN_RIGHTGRPAR, std::move (l));
      }
#else
      static
      symbol_type
      make_RIGHTGRPAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_RIGHTGRPAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOT (location_type l)
      {
        return symbol_type (token::TOKEN_DOT, std::move (l));
      }
#else
      static
      symbol_type
      make_DOT (const location_type& l)
      {
        return symbol_type (token::TOKEN_DOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOLLAR (location_type l)
      {
        return symbol_type (token::TOKEN_DOLLAR, std::move (l));
      }
#else
      static
      symbol_type
      make_DOLLAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_DOLLAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR (location_type l)
      {
        return symbol_type (token::TOKEN_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_OR (const location_type& l)
      {
        return symbol_type (token::TOKEN_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND (location_type l)
      {
        return symbol_type (token::TOKEN_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_AND (const location_type& l)
      {
        return symbol_type (token::TOKEN_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NEXT (location_type l)
      {
        return symbol_type (token::TOKEN_NEXT, std::move (l));
      }
#else
      static
      symbol_type
      make_NEXT (const location_type& l)
      {
        return symbol_type (token::TOKEN_NEXT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD (location_type l)
      {
        return symbol_type (token::TOKEN_ADD, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEL (location_type l)
      {
        return symbol_type (token::TOKEN_DEL, std::move (l));
      }
#else
      static
      symbol_type
      make_DEL (const location_type& l)
      {
        return symbol_type (token::TOKEN_DEL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TO (location_type l)
      {
        return symbol_type (token::TOKEN_TO, std::move (l));
      }
#else
      static
      symbol_type
      make_TO (const location_type& l)
      {
        return symbol_type (token::TOKEN_TO, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FROM (location_type l)
      {
        return symbol_type (token::TOKEN_FROM, std::move (l));
      }
#else
      static
      symbol_type
      make_FROM (const location_type& l)
      {
        return symbol_type (token::TOKEN_FROM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VEXPIRY (location_type l)
      {
        return symbol_type (token::TOKEN_VEXPIRY, std::move (l));
      }
#else
      static
      symbol_type
      make_VEXPIRY (const location_type& l)
      {
        return symbol_type (token::TOKEN_VEXPIRY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TCP_REDIRECT (location_type l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT, std::move (l));
      }
#else
      static
      symbol_type
      make_TCP_REDIRECT (const location_type& l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TIME (location_type l)
      {
        return symbol_type (token::TOKEN_TIME, std::move (l));
      }
#else
      static
      symbol_type
      make_TIME (const location_type& l)
      {
        return symbol_type (token::TOKEN_TIME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TCP_REDIRECT_STRING (location_type l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_STRING, std::move (l));
      }
#else
      static
      symbol_type
      make_TCP_REDIRECT_STRING (const location_type& l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_STRING, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TCP_REDIRECT_VSTRING (location_type l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_VSTRING, std::move (l));
      }
#else
      static
      symbol_type
      make_TCP_REDIRECT_VSTRING (const location_type& l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_VSTRING, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TCP_REDIRECT_HOST (location_type l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_HOST, std::move (l));
      }
#else
      static
      symbol_type
      make_TCP_REDIRECT_HOST (const location_type& l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_HOST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TCP_REDIRECT_VHOST (location_type l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_VHOST, std::move (l));
      }
#else
      static
      symbol_type
      make_TCP_REDIRECT_VHOST (const location_type& l)
      {
        return symbol_type (token::TOKEN_TCP_REDIRECT_VHOST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SCM_MANAGE (location_type l)
      {
        return symbol_type (token::TOKEN_SCM_MANAGE, std::move (l));
      }
#else
      static
      symbol_type
      make_SCM_MANAGE (const location_type& l)
      {
        return symbol_type (token::TOKEN_SCM_MANAGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SCM_GIVE (location_type l)
      {
        return symbol_type (token::TOKEN_SCM_GIVE, std::move (l));
      }
#else
      static
      symbol_type
      make_SCM_GIVE (const location_type& l)
      {
        return symbol_type (token::TOKEN_SCM_GIVE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_REDIRECT (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_REDIRECT, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_REDIRECT (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_REDIRECT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_VREDIRECT (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_VREDIRECT, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_VREDIRECT (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_VREDIRECT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BANDWIDTH (location_type l)
      {
        return symbol_type (token::TOKEN_BANDWIDTH, std::move (l));
      }
#else
      static
      symbol_type
      make_BANDWIDTH (const location_type& l)
      {
        return symbol_type (token::TOKEN_BANDWIDTH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VBANDWIDTH (location_type l)
      {
        return symbol_type (token::TOKEN_VBANDWIDTH, std::move (l));
      }
#else
      static
      symbol_type
      make_VBANDWIDTH (const location_type& l)
      {
        return symbol_type (token::TOKEN_VBANDWIDTH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_VANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_VANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_VANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VSCM_MANAGE (location_type l)
      {
        return symbol_type (token::TOKEN_VSCM_MANAGE, std::move (l));
      }
#else
      static
      symbol_type
      make_VSCM_MANAGE (const location_type& l)
      {
        return symbol_type (token::TOKEN_VSCM_MANAGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VSCM_GIVE (location_type l)
      {
        return symbol_type (token::TOKEN_VSCM_GIVE, std::move (l));
      }
#else
      static
      symbol_type
      make_VSCM_GIVE (const location_type& l)
      {
        return symbol_type (token::TOKEN_VSCM_GIVE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRACE (location_type l)
      {
        return symbol_type (token::TOKEN_TRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_TRACE (const location_type& l)
      {
        return symbol_type (token::TOKEN_TRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NOT (location_type l)
      {
        return symbol_type (token::TOKEN_NOT, std::move (l));
      }
#else
      static
      symbol_type
      make_NOT (const location_type& l)
      {
        return symbol_type (token::TOKEN_NOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHEN (location_type l)
      {
        return symbol_type (token::TOKEN_WHEN, std::move (l));
      }
#else
      static
      symbol_type
      make_WHEN (const location_type& l)
      {
        return symbol_type (token::TOKEN_WHEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::TOKEN_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::TOKEN_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_THEN (location_type l)
      {
        return symbol_type (token::TOKEN_THEN, std::move (l));
      }
#else
      static
      symbol_type
      make_THEN (const location_type& l)
      {
        return symbol_type (token::TOKEN_THEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENABLED (location_type l)
      {
        return symbol_type (token::TOKEN_ENABLED, std::move (l));
      }
#else
      static
      symbol_type
      make_ENABLED (const location_type& l)
      {
        return symbol_type (token::TOKEN_ENABLED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DISABLED (location_type l)
      {
        return symbol_type (token::TOKEN_DISABLED, std::move (l));
      }
#else
      static
      symbol_type
      make_DISABLED (const location_type& l)
      {
        return symbol_type (token::TOKEN_DISABLED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEFAULT (location_type l)
      {
        return symbol_type (token::TOKEN_DEFAULT, std::move (l));
      }
#else
      static
      symbol_type
      make_DEFAULT (const location_type& l)
      {
        return symbol_type (token::TOKEN_DEFAULT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GT (location_type l)
      {
        return symbol_type (token::TOKEN_GT, std::move (l));
      }
#else
      static
      symbol_type
      make_GT (const location_type& l)
      {
        return symbol_type (token::TOKEN_GT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LT (location_type l)
      {
        return symbol_type (token::TOKEN_LT, std::move (l));
      }
#else
      static
      symbol_type
      make_LT (const location_type& l)
      {
        return symbol_type (token::TOKEN_LT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GTE (location_type l)
      {
        return symbol_type (token::TOKEN_GTE, std::move (l));
      }
#else
      static
      symbol_type
      make_GTE (const location_type& l)
      {
        return symbol_type (token::TOKEN_GTE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LTE (location_type l)
      {
        return symbol_type (token::TOKEN_LTE, std::move (l));
      }
#else
      static
      symbol_type
      make_LTE (const location_type& l)
      {
        return symbol_type (token::TOKEN_LTE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PLUS (location_type l)
      {
        return symbol_type (token::TOKEN_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_PLUS (const location_type& l)
      {
        return symbol_type (token::TOKEN_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MINUS (location_type l)
      {
        return symbol_type (token::TOKEN_MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_MINUS (const location_type& l)
      {
        return symbol_type (token::TOKEN_MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AT (location_type l)
      {
        return symbol_type (token::TOKEN_AT, std::move (l));
      }
#else
      static
      symbol_type
      make_AT (const location_type& l)
      {
        return symbol_type (token::TOKEN_AT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TRUE (location_type l)
      {
        return symbol_type (token::TOKEN_TRUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TRUE (const location_type& l)
      {
        return symbol_type (token::TOKEN_TRUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FALSE (location_type l)
      {
        return symbol_type (token::TOKEN_FALSE, std::move (l));
      }
#else
      static
      symbol_type
      make_FALSE (const location_type& l)
      {
        return symbol_type (token::TOKEN_FALSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET (location_type l)
      {
        return symbol_type (token::TOKEN_SET, std::move (l));
      }
#else
      static
      symbol_type
      make_SET (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DICTIONARY (location_type l)
      {
        return symbol_type (token::TOKEN_DICTIONARY, std::move (l));
      }
#else
      static
      symbol_type
      make_DICTIONARY (const location_type& l)
      {
        return symbol_type (token::TOKEN_DICTIONARY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IPSET (location_type l)
      {
        return symbol_type (token::TOKEN_IPSET, std::move (l));
      }
#else
      static
      symbol_type
      make_IPSET (const location_type& l)
      {
        return symbol_type (token::TOKEN_IPSET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_URLSET (location_type l)
      {
        return symbol_type (token::TOKEN_URLSET, std::move (l));
      }
#else
      static
      symbol_type
      make_URLSET (const location_type& l)
      {
        return symbol_type (token::TOKEN_URLSET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TIMESET (location_type l)
      {
        return symbol_type (token::TOKEN_TIMESET, std::move (l));
      }
#else
      static
      symbol_type
      make_TIMESET (const location_type& l)
      {
        return symbol_type (token::TOKEN_TIMESET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PARSET (location_type l)
      {
        return symbol_type (token::TOKEN_PARSET, std::move (l));
      }
#else
      static
      symbol_type
      make_PARSET (const location_type& l)
      {
        return symbol_type (token::TOKEN_PARSET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEFINE (location_type l)
      {
        return symbol_type (token::TOKEN_DEFINE, std::move (l));
      }
#else
      static
      symbol_type
      make_DEFINE (const location_type& l)
      {
        return symbol_type (token::TOKEN_DEFINE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MYSQL (location_type l)
      {
        return symbol_type (token::TOKEN_MYSQL, std::move (l));
      }
#else
      static
      symbol_type
      make_MYSQL (const location_type& l)
      {
        return symbol_type (token::TOKEN_MYSQL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AGENT_ID (location_type l)
      {
        return symbol_type (token::TOKEN_AGENT_ID, std::move (l));
      }
#else
      static
      symbol_type
      make_AGENT_ID (const location_type& l)
      {
        return symbol_type (token::TOKEN_AGENT_ID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR (location_type l)
      {
        return symbol_type (token::TOKEN_VR, std::move (l));
      }
#else
      static
      symbol_type
      make_VR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VREP (location_type l)
      {
        return symbol_type (token::TOKEN_VREP, std::move (l));
      }
#else
      static
      symbol_type
      make_VREP (const location_type& l)
      {
        return symbol_type (token::TOKEN_VREP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REPORT (location_type l)
      {
        return symbol_type (token::TOKEN_REPORT, std::move (l));
      }
#else
      static
      symbol_type
      make_REPORT (const location_type& l)
      {
        return symbol_type (token::TOKEN_REPORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MREPORT (location_type l)
      {
        return symbol_type (token::TOKEN_MREPORT, std::move (l));
      }
#else
      static
      symbol_type
      make_MREPORT (const location_type& l)
      {
        return symbol_type (token::TOKEN_MREPORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONDITION (location_type l)
      {
        return symbol_type (token::TOKEN_CONDITION, std::move (l));
      }
#else
      static
      symbol_type
      make_CONDITION (const location_type& l)
      {
        return symbol_type (token::TOKEN_CONDITION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEQUENCE (location_type l)
      {
        return symbol_type (token::TOKEN_SEQUENCE, std::move (l));
      }
#else
      static
      symbol_type
      make_SEQUENCE (const location_type& l)
      {
        return symbol_type (token::TOKEN_SEQUENCE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OF (location_type l)
      {
        return symbol_type (token::TOKEN_OF, std::move (l));
      }
#else
      static
      symbol_type
      make_OF (const location_type& l)
      {
        return symbol_type (token::TOKEN_OF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_KEY (location_type l)
      {
        return symbol_type (token::TOKEN_KEY, std::move (l));
      }
#else
      static
      symbol_type
      make_KEY (const location_type& l)
      {
        return symbol_type (token::TOKEN_KEY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FREQ (location_type l)
      {
        return symbol_type (token::TOKEN_FREQ, std::move (l));
      }
#else
      static
      symbol_type
      make_FREQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_FREQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COUNT (location_type l)
      {
        return symbol_type (token::TOKEN_COUNT, std::move (l));
      }
#else
      static
      symbol_type
      make_COUNT (const location_type& l)
      {
        return symbol_type (token::TOKEN_COUNT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAR (location_type l)
      {
        return symbol_type (token::TOKEN_VAR, std::move (l));
      }
#else
      static
      symbol_type
      make_VAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IS (location_type l)
      {
        return symbol_type (token::TOKEN_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IN (location_type l)
      {
        return symbol_type (token::TOKEN_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALWAYS (location_type l)
      {
        return symbol_type (token::TOKEN_ALWAYS, std::move (l));
      }
#else
      static
      symbol_type
      make_ALWAYS (const location_type& l)
      {
        return symbol_type (token::TOKEN_ALWAYS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_HOST (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_HOST, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_HOST (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_HOST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_URL (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_URL, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_URL (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_URL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_URI (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_URI, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_URI (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_URI, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_URL_HOST (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_URL_HOST, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_URL_HOST (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_URL_HOST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_HEADER_HOST (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER_HOST, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_HEADER_HOST (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER_HOST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_HEADER_REPLACE (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER_REPLACE, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_HEADER_REPLACE (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER_REPLACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER_HEADER_REPLACE (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_HEADER_REPLACE, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER_HEADER_REPLACE (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_HEADER_REPLACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CALL (location_type l)
      {
        return symbol_type (token::TOKEN_CALL, std::move (l));
      }
#else
      static
      symbol_type
      make_CALL (const location_type& l)
      {
        return symbol_type (token::TOKEN_CALL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WITH (location_type l)
      {
        return symbol_type (token::TOKEN_WITH, std::move (l));
      }
#else
      static
      symbol_type
      make_WITH (const location_type& l)
      {
        return symbol_type (token::TOKEN_WITH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PROGRAM (location_type l)
      {
        return symbol_type (token::TOKEN_PROGRAM, std::move (l));
      }
#else
      static
      symbol_type
      make_PROGRAM (const location_type& l)
      {
        return symbol_type (token::TOKEN_PROGRAM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET_LIST_ID (location_type l)
      {
        return symbol_type (token::TOKEN_SET_LIST_ID, std::move (l));
      }
#else
      static
      symbol_type
      make_SET_LIST_ID (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET_LIST_ID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DICTIONARY_LIST_ID (location_type l)
      {
        return symbol_type (token::TOKEN_DICTIONARY_LIST_ID, std::move (l));
      }
#else
      static
      symbol_type
      make_DICTIONARY_LIST_ID (const location_type& l)
      {
        return symbol_type (token::TOKEN_DICTIONARY_LIST_ID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET_MYSQL (location_type l)
      {
        return symbol_type (token::TOKEN_SET_MYSQL, std::move (l));
      }
#else
      static
      symbol_type
      make_SET_MYSQL (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET_MYSQL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET_LIST_IPSTAR (location_type l)
      {
        return symbol_type (token::TOKEN_SET_LIST_IPSTAR, std::move (l));
      }
#else
      static
      symbol_type
      make_SET_LIST_IPSTAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET_LIST_IPSTAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET_LIST_URLSTAR (location_type l)
      {
        return symbol_type (token::TOKEN_SET_LIST_URLSTAR, std::move (l));
      }
#else
      static
      symbol_type
      make_SET_LIST_URLSTAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET_LIST_URLSTAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SET_LIST_TIMING (location_type l)
      {
        return symbol_type (token::TOKEN_SET_LIST_TIMING, std::move (l));
      }
#else
      static
      symbol_type
      make_SET_LIST_TIMING (const location_type& l)
      {
        return symbol_type (token::TOKEN_SET_LIST_TIMING, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_AGENT_ID_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_AGENT_ID_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_AGENT_ID_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_AGENT_ID_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_NAME_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_NAME_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_NAME_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_NAME_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_EVENT_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_EVENT_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_EVENT_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_EVENT_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_IPPORT_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_IPPORT_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_IPPORT_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_IPPORT_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_IPPORT_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_IPPORT_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_METHOD_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_METHOD_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_METHOD_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_METHOD_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HOST_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HOST_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HOST_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HOST_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URI_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URI_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URI_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URI_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_EMPTY (location_type l)
      {
        return symbol_type (token::TOKEN_VR_EMPTY, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_EMPTY (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_EMPTY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AR_DEF (location_type l)
      {
        return symbol_type (token::TOKEN_AR_DEF, std::move (l));
      }
#else
      static
      symbol_type
      make_AR_DEF (const location_type& l)
      {
        return symbol_type (token::TOKEN_AR_DEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AR_DEF_WHEN (location_type l)
      {
        return symbol_type (token::TOKEN_AR_DEF_WHEN, std::move (l));
      }
#else
      static
      symbol_type
      make_AR_DEF_WHEN (const location_type& l)
      {
        return symbol_type (token::TOKEN_AR_DEF_WHEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AR_DEF_ACTION (location_type l)
      {
        return symbol_type (token::TOKEN_AR_DEF_ACTION, std::move (l));
      }
#else
      static
      symbol_type
      make_AR_DEF_ACTION (const location_type& l)
      {
        return symbol_type (token::TOKEN_AR_DEF_ACTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CAT (location_type l)
      {
        return symbol_type (token::TOKEN_CAT, std::move (l));
      }
#else
      static
      symbol_type
      make_CAT (const location_type& l)
      {
        return symbol_type (token::TOKEN_CAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_EVENT (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_EVENT, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_EVENT (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_EVENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_CONNECT (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_CONNECT, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_CONNECT (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_CONNECT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_ACCEPT (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_ACCEPT, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_ACCEPT (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_ACCEPT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_SEND (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_SEND, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_SEND (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_SEND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_RECV (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_RECV, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_RECV (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_RECV, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_CLOSE (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_CLOSE, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_CLOSE (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_CLOSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_IPSRC (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_IPSRC, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_IPSRC (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_IPSRC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_IPDST (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_IPDST, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_IPDST (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_IPDST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_TCP_SESID (location_type l)
      {
        return symbol_type (token::TOKEN_NET_TCP_SESID, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_TCP_SESID (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_TCP_SESID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NET_RESULT (location_type l)
      {
        return symbol_type (token::TOKEN_NET_RESULT, std::move (l));
      }
#else
      static
      symbol_type
      make_NET_RESULT (const location_type& l)
      {
        return symbol_type (token::TOKEN_NET_RESULT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_NAME (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_NAME, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_NAME (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_NAME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_TIME (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_TIME, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_TIME (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_TIME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_OBSID (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_OBSID, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_OBSID (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_OBSID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_STIME (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_STIME, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_STIME (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_STIME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_ETIME (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_ETIME, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_ETIME (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_ETIME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_TAG (location_type l)
      {
        return symbol_type (token::TOKEN_OBS_TAG, std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_TAG (const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_TAG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DATA (location_type l)
      {
        return symbol_type (token::TOKEN_DATA, std::move (l));
      }
#else
      static
      symbol_type
      make_DATA (const location_type& l)
      {
        return symbol_type (token::TOKEN_DATA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MAKE (location_type l)
      {
        return symbol_type (token::TOKEN_MAKE, std::move (l));
      }
#else
      static
      symbol_type
      make_MAKE (const location_type& l)
      {
        return symbol_type (token::TOKEN_MAKE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AR (location_type l)
      {
        return symbol_type (token::TOKEN_AR, std::move (l));
      }
#else
      static
      symbol_type
      make_AR (const location_type& l)
      {
        return symbol_type (token::TOKEN_AR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ENABLE (location_type l)
      {
        return symbol_type (token::TOKEN_ENABLE, std::move (l));
      }
#else
      static
      symbol_type
      make_ENABLE (const location_type& l)
      {
        return symbol_type (token::TOKEN_ENABLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DISABLE (location_type l)
      {
        return symbol_type (token::TOKEN_DISABLE, std::move (l));
      }
#else
      static
      symbol_type
      make_DISABLE (const location_type& l)
      {
        return symbol_type (token::TOKEN_DISABLE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MYSQL_APPEND (location_type l)
      {
        return symbol_type (token::TOKEN_MYSQL_APPEND, std::move (l));
      }
#else
      static
      symbol_type
      make_MYSQL_APPEND (const location_type& l)
      {
        return symbol_type (token::TOKEN_MYSQL_APPEND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MYSQL_APPEND_VARIABLES (location_type l)
      {
        return symbol_type (token::TOKEN_MYSQL_APPEND_VARIABLES, std::move (l));
      }
#else
      static
      symbol_type
      make_MYSQL_APPEND_VARIABLES (const location_type& l)
      {
        return symbol_type (token::TOKEN_MYSQL_APPEND_VARIABLES, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MYSQL_UPDATE (location_type l)
      {
        return symbol_type (token::TOKEN_MYSQL_UPDATE, std::move (l));
      }
#else
      static
      symbol_type
      make_MYSQL_UPDATE (const location_type& l)
      {
        return symbol_type (token::TOKEN_MYSQL_UPDATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_GET ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_GET, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_GET (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_GET, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_POST ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_POST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_POST (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_POST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_HEAD ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_HEAD, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_HEAD (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_HEAD, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_PUT ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_PUT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_PUT (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_PUT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_DELETE ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_DELETE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_DELETE (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_DELETE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_TRACE ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_TRACE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_TRACE (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_TRACE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_OPTIONS ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_OPTIONS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_OPTIONS (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_OPTIONS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_CONNECT ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_CONNECT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_CONNECT (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_CONNECT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_METHOD_PATCH ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_PATCH, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_METHOD_PATCH (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_METHOD_PATCH, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_COOKIE (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_COOKIE, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_COOKIE (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_COOKIE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_HEADER (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_HEADER (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_HEADER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_PORT (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_PORT, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_PORT (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_PORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_QUERY (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_QUERY, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_QUERY (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_QUERY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_DATA (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_DATA, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_DATA (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_DATA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER_DATA (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_DATA, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER_DATA (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_DATA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER_CODE (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_CODE, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER_CODE (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_CODE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER_HEADER (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_HEADER, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER_HEADER (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_HEADER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HTTP_ANSWER_COOKIE (location_type l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_COOKIE, std::move (l));
      }
#else
      static
      symbol_type
      make_HTTP_ANSWER_COOKIE (const location_type& l)
      {
        return symbol_type (token::TOKEN_HTTP_ANSWER_COOKIE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SA_OS (location_type l)
      {
        return symbol_type (token::TOKEN_SA_OS, std::move (l));
      }
#else
      static
      symbol_type
      make_SA_OS (const location_type& l)
      {
        return symbol_type (token::TOKEN_SA_OS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SA_LOGGED_USER (location_type l)
      {
        return symbol_type (token::TOKEN_SA_LOGGED_USER, std::move (l));
      }
#else
      static
      symbol_type
      make_SA_LOGGED_USER (const location_type& l)
      {
        return symbol_type (token::TOKEN_SA_LOGGED_USER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SA_PROCESS_NAME (location_type l)
      {
        return symbol_type (token::TOKEN_SA_PROCESS_NAME, std::move (l));
      }
#else
      static
      symbol_type
      make_SA_PROCESS_NAME (const location_type& l)
      {
        return symbol_type (token::TOKEN_SA_PROCESS_NAME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SA_PROCESS_ATTACH (location_type l)
      {
        return symbol_type (token::TOKEN_SA_PROCESS_ATTACH, std::move (l));
      }
#else
      static
      symbol_type
      make_SA_PROCESS_ATTACH (const location_type& l)
      {
        return symbol_type (token::TOKEN_SA_PROCESS_ATTACH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_COOKIE_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_QUERY_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_DATA_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_OBS_TAG_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_OS_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_OS_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_OS_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_OS_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_OS_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_OS_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_PROCESS_NAME_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_PROCESS_NAME_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_USER_NAME_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_USER_NAME_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_SA_USER_NAME_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_SA_USER_NAME_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_IPSRC_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_IPSRC_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_NET_RESULT_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_NET_RESULT_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_SESID_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_SESID_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_SESID_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_NET_TCP_SESID_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_CALL_LISP_FUNCTION (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_CALL_LISP_FUNCTION (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_GET (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_GET, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_GET (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_GET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_EXISTS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_EXISTS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_EXISTS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_EXISTS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_FREQ (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_FREQ, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_FREQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_FREQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_COUNT (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_COUNT, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_COUNT (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_COUNT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_KEY_IS (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_KEY_IS (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_KEY_IS_IN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS_IN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_WHEN_VREP_KEY_IS_IN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS_IN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_CREATE (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_CREATE, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_CREATE (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_CREATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_MCREATE (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_MCREATE, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_MCREATE (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_MCREATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_PLUS_CREATE (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_PLUS_CREATE (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_FREQ (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_FREQ, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_FREQ (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_FREQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_COUNT (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_COUNT, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_COUNT (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_COUNT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_KEY (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_KEY, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_KEY (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_KEY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_KV_PAIR (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_KV_PAIR, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_ACTION_VREP_KV_PAIR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_ACTION_VREP_KV_PAIR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_KV (location_type l)
      {
        return symbol_type (token::TOKEN_KV, std::move (l));
      }
#else
      static
      symbol_type
      make_KV (const location_type& l)
      {
        return symbol_type (token::TOKEN_KV, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_KEY_LIST (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_KEY_LIST, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_KEY_LIST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_KEY_LIST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_DATA_LIST (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_DATA_LIST, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_DATA_LIST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_DATA_LIST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_ENABLE_VR (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_ENABLE_VR, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_ENABLE_VR (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_ENABLE_VR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_DISABLE_VR (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_DISABLE_VR, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_DISABLE_VR (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_DISABLE_VR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_ENABLE_AR (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_ENABLE_AR, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_ENABLE_AR (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_ENABLE_AR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_DISABLE_AR (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_DISABLE_AR, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_DISABLE_AR (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_DISABLE_AR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_SETADD (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_SETADD, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_SETADD (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_SETADD, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_SETDEL (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_SETDEL, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_SETDEL (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_SETDEL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACTION_SETVAR (location_type l)
      {
        return symbol_type (token::TOKEN_ACTION_SETVAR, std::move (l));
      }
#else
      static
      symbol_type
      make_ACTION_SETVAR (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACTION_SETVAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBS_EVENT_ACM_LIGHT ( string  v, location_type l)
      {
        return symbol_type (token::TOKEN_OBS_EVENT_ACM_LIGHT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OBS_EVENT_ACM_LIGHT (const  string & v, const location_type& l)
      {
        return symbol_type (token::TOKEN_OBS_EVENT_ACM_LIGHT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_A_CLAUSES (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_CLAUSES, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_A_CLAUSES (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_CLAUSES, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_A_EXPR (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_EXPR, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_A_EXPR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_EXPR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_A_TERM (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_TERM, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_A_TERM (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_TERM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_A_FACTOR (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_FACTOR, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_A_FACTOR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_FACTOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_A_WHEN_WITH (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_WHEN_WITH, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_A_WHEN_WITH (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_A_WHEN_WITH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_LOP_OR (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_LOP_OR (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_LOP_AND (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_LOP_AND (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_LOP_NEXT (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_NEXT, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_LOP_NEXT (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LOP_NEXT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_LISTA_WHEN (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LISTA_WHEN, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_LISTA_WHEN (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LISTA_WHEN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VR_DEF_IS_LISTA_WITH (location_type l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LISTA_WITH, std::move (l));
      }
#else
      static
      symbol_type
      make_VR_DEF_IS_LISTA_WITH (const location_type& l)
      {
        return symbol_type (token::TOKEN_VR_DEF_IS_LISTA_WITH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BASIC_ACCESS_RULES (location_type l)
      {
        return symbol_type (token::TOKEN_BASIC_ACCESS_RULES, std::move (l));
      }
#else
      static
      symbol_type
      make_BASIC_ACCESS_RULES (const location_type& l)
      {
        return symbol_type (token::TOKEN_BASIC_ACCESS_RULES, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXTENDED_ACCESS_RULES (location_type l)
      {
        return symbol_type (token::TOKEN_EXTENDED_ACCESS_RULES, std::move (l));
      }
#else
      static
      symbol_type
      make_EXTENDED_ACCESS_RULES (const location_type& l)
      {
        return symbol_type (token::TOKEN_EXTENDED_ACCESS_RULES, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCESS_OPERATIONS (location_type l)
      {
        return symbol_type (token::TOKEN_ACCESS_OPERATIONS, std::move (l));
      }
#else
      static
      symbol_type
      make_ACCESS_OPERATIONS (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCESS_OPERATIONS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_HEADER_TO_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_HEADER_TO_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_HEADER_TO_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_HEADER_TO_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_HEADER_TO_REQUEST (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_HEADER_TO_REQUEST, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_HEADER_TO_REQUEST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_HEADER_TO_REQUEST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_V_HEADER_TO_V_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_V_HEADER_TO_V_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_V_HEADER_TO_V_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_V_HEADER_TO_V_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_V_HEADER_TO_V_REQUEST (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_V_HEADER_TO_V_REQUEST, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_V_HEADER_TO_V_REQUEST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_V_HEADER_TO_V_REQUEST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_COOKIE_TO_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_COOKIE_TO_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_COOKIE_TO_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_COOKIE_TO_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_COOKIE_TO_REQUEST (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_COOKIE_TO_REQUEST, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_COOKIE_TO_REQUEST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_COOKIE_TO_REQUEST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_V_COOKIE_TO_V_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_V_COOKIE_TO_V_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADD_V_COOKIE_TO_V_REQUEST (location_type l)
      {
        return symbol_type (token::TOKEN_ADD_V_COOKIE_TO_V_REQUEST, std::move (l));
      }
#else
      static
      symbol_type
      make_ADD_V_COOKIE_TO_V_REQUEST (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADD_V_COOKIE_TO_V_REQUEST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REPLACE_HEADER_IN_REQUEST (location_type l)
      {
        return symbol_type (token::TOKEN_REPLACE_HEADER_IN_REQUEST, std::move (l));
      }
#else
      static
      symbol_type
      make_REPLACE_HEADER_IN_REQUEST (const location_type& l)
      {
        return symbol_type (token::TOKEN_REPLACE_HEADER_IN_REQUEST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REPLACE_HEADER_IN_ANSWER (location_type l)
      {
        return symbol_type (token::TOKEN_REPLACE_HEADER_IN_ANSWER, std::move (l));
      }
#else
      static
      symbol_type
      make_REPLACE_HEADER_IN_ANSWER (const location_type& l)
      {
        return symbol_type (token::TOKEN_REPLACE_HEADER_IN_ANSWER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PATTERN (location_type l)
      {
        return symbol_type (token::TOKEN_PATTERN, std::move (l));
      }
#else
      static
      symbol_type
      make_PATTERN (const location_type& l)
      {
        return symbol_type (token::TOKEN_PATTERN, l);
      }
#endif


    class context
    {
    public:
      context (const  Parser & yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const  Parser & yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
     Parser  (const  Parser &);
    /// Non copyable.
     Parser & operator= (const  Parser &);
#endif


    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const short yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const short yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const short yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const short yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 1768,     ///< Last index in yytable_.
      yynnts_ = 46,  ///< Number of nonterminal symbols.
      yyfinal_ = 3 ///< Termination state number.
    };


    // User arguments.
    VaParser::Scanner &scanner;
    VaParser::Interpreter &driver;

  };

  inline
   Parser ::symbol_kind_type
   Parser ::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const short
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     268,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   266,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,   269,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   267,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265
    };
    // Last valid token kind.
    const int code_max = 520;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
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
        value.copy<  Command  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.copy<  bool  > (YY_MOVE (that.value));
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
        value.copy<  list< Command >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.copy<  list< pair<string, string> >  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.copy<  list< string >  > (YY_MOVE (that.value));
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
        value.copy<  string  > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
   Parser ::symbol_kind_type
   Parser ::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
   Parser ::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
   Parser ::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
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
        value.move<  Command  > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_check_or_not_the_server_cert: // check_or_not_the_server_cert
        value.move<  bool  > (YY_MOVE (s.value));
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
        value.move<  list< Command >  > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ident_or_string_pairs_list: // ident_or_string_pairs_list
        value.move<  list< pair<string, string> >  > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ssl_server_options: // ssl_server_options
      case symbol_kind::S_timing_list: // timing_list
      case symbol_kind::S_ip_with_star_list: // ip_with_star_list
      case symbol_kind::S_url_with_star_list: // url_with_star_list
      case symbol_kind::S_ident_or_string_list: // ident_or_string_list
        value.move<  list< string >  > (YY_MOVE (s.value));
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
        value.move<  string  > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
   Parser ::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
   Parser ::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
   Parser ::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
   Parser ::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
   Parser ::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
   Parser ::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
   Parser ::symbol_kind_type
   Parser ::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
   Parser ::symbol_kind_type
   Parser ::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 9 "va_parser.y"
} //  VaParser 
#line 6318 "va_parser.hpp"




#endif // !YY_YY_VA_PARSER_HPP_INCLUDED
