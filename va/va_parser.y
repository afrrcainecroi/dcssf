%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define parser_class_name { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { VaParser }
%code requires
{
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
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
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
}

%lex-param { VaParser::Scanner &scanner }
%lex-param { VaParser::Interpreter &driver }
%parse-param { VaParser::Scanner &scanner }
%parse-param { VaParser::Interpreter &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token < string > IDENT  "ident"
%token < string > STRING  "string"
%token < string > VARIABLE  "variable ($string)"
%token < string > I_NUMBER "integer number"
%token < string > IP_WITH_STAR "ip_with_star"
%token < string > IP_WITHOUT_STAR "ip_without_star"
%token < string > URL_WITH_STAR "url_with_star"
%token < string > TIMING "timing"

%token EQUAL "equal"
%token LEFTPAR "leftpar"
%token RIGHTPAR "rightpar"
%token SEMICOLON "semicolon"
%token COMMA "comma"
//%token COLON "colon"
%token LEFTSQPAR "leftsqpar"
%token RIGHTSQPAR "rightsqpar"
%token LEFTGRPAR "leftgrpar"
%token RIGHTGRPAR "rightsgrpar"
%token DOT "."
%token DOLLAR "$"

%token OR " or "
%token AND " and "
%token NEXT " next "
%token ADD " add "
%token DEL " del "
%token TO " to "
%token FROM " from "
//%token EXPIRY "expiry"
%token VEXPIRY "vexpiry"
%token TCP_REDIRECT "TCP_REDIRECT"
%token TIME "time"

%token TCP_REDIRECT_STRING
%token TCP_REDIRECT_VSTRING
%token TCP_REDIRECT_HOST
%token TCP_REDIRECT_VHOST
%token SCM_MANAGE "scm.manage"
%token SCM_GIVE   "scm.give"

%token HTTP_REDIRECT "HTTP_REDIRECT"
%token HTTP_VREDIRECT "HTTP_VREDIRECT"
%token BANDWIDTH "BANDWIDTH"
%token VBANDWIDTH "VBANDWIDTH"
%token ANSWER "ANSWER"
%token VANSWER "VANSWER"
%token VSCM_MANAGE "vscm.manage"
%token VSCM_GIVE   "vscm.give"
%token TRACE   "trace"
%token NOT   "!"

//Access rules
%token WHEN "when"
%token IF "if"
%token THEN "then"
%token ENABLED "enabled"
%token DISABLED "disabled"
%token DEFAULT "default"

%token GT ">"
%token LT "<"
%token GTE ">="
%token LTE "<="
%token PLUS "+"
%token MINUS "-"
%token AT "@"
%token TRUE "true"
%token FALSE "false"

%token SET "set"
%token DICTIONARY "dictionary"
%token IPSET "ipset"
%token URLSET "urlset"
%token TIMESET "timeset"
%token PARSET "parset"
%token DEFINE "define"
%token MYSQL "mysql"
%token AGENT_ID "agent id"

%token VR "validation rule"
%token VREP "validation report"
%token REPORT "report"
%token MREPORT "mreport"
%token CONDITION "condition"
%token SEQUENCE "sequence"
%token OF "of"
%token KEY "key"
%token FREQ "frequence"
%token COUNT "counter"
%token VAR "var"
%token ACTION "action"
%token IS "is"
%token IN "in"
%token ALWAYS "always"
%token HTTP_HOST "HTTP HOST"
%token HTTP_URL "HTTP URL"
%token HTTP_URI "HTTP URI"
%token HTTP_URL_HOST "HTTP host in the url"
%token HTTP_HEADER_HOST "HTTP host in the headers"
%token HTTP_HEADER_REPLACE "In tutti gli header esegue, compresa url, esegue il replace"
%token HTTP_ANSWER_HEADER_REPLACE "In tutti gli header esegue, compresa url, esegue il replace"

%token CALL "Calls a lisp function"
%token WITH "Parameters for a lisp function"

//%type< VaParser::Command > command

%type< string > ident_or_string
%type< string > ip_or_domain

%type< list< string > > ident_or_string_list
%type< list< pair<string, string> > > ident_or_string_pairs_list    //Dove la seconda stringa è strutturata
%type< list< Command > > observable_list
%type< list< Command > > observable_assign_list
%type< list< Command > > action_variable_list
%type < Command > action_variable_item
%type< list< string > > ip_with_star_list
%type< list< string > > url_with_star_list
%type< list< string > > timing_list
%type < string > ip_address

%type< Command > set_definition
%type< Command > vr_definition
%type< Command > ar_definition

%type< list< Command > > lista_when
%type< list< Command > > lista_with
%type< Command > item_when
%type< Command > basic_item_when
%type < string > pattern_extractor
%type< list< Command > > lista_action
%type< Command > item_action

%type < string > http_method
//%type< Command > rel_op
%type < string > true_false
%type< string > network_event
//%type< list< Command > > network_events

%type< Command > clauses
%type< Command > expr
%type< Command > term
%type< Command > factor

//VR
%type< Command > observable
%type< list< Command > > observables_list
%type< Command > cat_observables

//AR
%type< Command > ar_observable
%type< list< Command > > ar_observables_list
%type< Command > ar_cat_observables


%type< list< Command > > sezione_lista_with

%type< Command > access_rules
//%type< Command > extended_access_rules
%type< Command > basic_access_rules
%type< Command > item_basic_access_rule
%type< Command > item_basic_access_rule_with_not
//%type< Command > item_extended_access_rule
%type< Command > access_operations
%type< Command > access_operation

%type< bool > check_or_not_the_server_cert

%type< list< string > > ssl_server_options

//I token che mi servono per lavorare sull'AST in un secondo momento
%token  PROGRAM "program"
%token  SET_LIST_ID
%token  DICTIONARY_LIST_ID
%token  SET_MYSQL
%token  SET_LIST_IPSTAR
%token  SET_LIST_URLSTAR
%token  SET_LIST_TIMING
%token  VR_DEF
%token  VR_DEF_WHEN
//%token  VR_DEF_WHERE
%token  VR_DEF_ACTION

%token  VR_DEF_WHEN_AGENT_ID_IS
%token  VR_DEF_WHEN_AGENT_ID_IS_IN
%token  VR_DEF_WHEN_OBS_NAME_IS
%token  VR_DEF_WHEN_OBS_NAME_IS_IN
%token  VR_DEF_WHEN_OBS_EVENT_IS
%token  VR_DEF_WHEN_OBS_EVENT_IS_IN
%token  VR_DEF_WHEN_IPPORT_IS
%token  VR_DEF_WHEN_IPPORT_IS_IN
%token  VR_DEF_WHEN_HTTP_METHOD_IS
%token  VR_DEF_WHEN_HTTP_METHOD_IS_IN
%token  VR_DEF_WHEN_HTTP_HOST_IS
%token  VR_DEF_WHEN_HTTP_HOST_IS_IN
%token  VR_DEF_WHEN_HTTP_URL_IS
%token  VR_DEF_WHEN_HTTP_URL_IS_IN
%token  VR_DEF_WHEN_HTTP_URI_IS
%token  VR_DEF_WHEN_HTTP_URI_IS_IN

%token  VR_EMPTY

%token  AR_DEF
%token  AR_DEF_WHEN
%token  AR_DEF_ACTION

%token CAT

//Net events
%token OBS_EVENT "OBS_EVENT"
%token NET_TCP_CONNECT "NET_TCP_CONNECT"
%token NET_TCP_ACCEPT "NET_TCP_ACCEPT"
%token NET_TCP_SEND "NET_TCP_SEND"
%token NET_TCP_RECV "NET_TCP_RECEIVE"
%token NET_TCP_CLOSE "NET_TCP_CLOSE"
%token NET_TCP_IPSRC "NET_TCP_IPSRC"
%token NET_TCP_IPDST "NET_TCP_IPDST"
//LE PORTE NON LE USO, STANNO NEGLI IP!
%token NET_TCP_SESID "NET_TCP_SESID"
%token NET_RESULT "NET_RESULT"  //true or false

//Obs info
%token OBS_NAME	"OBS_NAME"
%token OBS_TIME "OBS_TIME"
%token OBS_OBSID "OBS_OBSID"
%token OBS_STIME "OBS_STIME"
%token OBS_ETIME "OBS_ETIME"
%token OBS_TAG "OBS_TAG"

%token DATA "Data"
%token MAKE "Make"
%token AR "Access rule"
%token ENABLE "Enable"
%token DISABLE "Disable"
%token MYSQL_APPEND "Mysql.append"
%token MYSQL_APPEND_VARIABLES

%token MYSQL_UPDATE "Mysql.update"

%token HTTP_METHOD
%token < string > HTTP_METHOD_GET
%token < string > HTTP_METHOD_POST
%token < string > HTTP_METHOD_HEAD
%token < string > HTTP_METHOD_PUT
%token < string > HTTP_METHOD_DELETE
%token < string > HTTP_METHOD_TRACE
%token < string > HTTP_METHOD_OPTIONS
%token < string > HTTP_METHOD_CONNECT
%token < string > HTTP_METHOD_PATCH

%token HTTP_COOKIE
%token HTTP_HEADER
%token HTTP_PORT
%token HTTP_QUERY
%token HTTP_DATA
%token HTTP_ANSWER_DATA
//%token HTTP_DATA_FULL
//%token HTTP_DATA_CRC

%token HTTP_ANSWER
%token HTTP_ANSWER_CODE
%token HTTP_ANSWER_HEADER
%token HTTP_ANSWER_COOKIE
//%token HTTP_ANSWER_DATA
//%token HTTP_ANSWER_DATA_CRC
//%token HTTP_ANSWER_DATA_FULL

%token SA_OS
%token SA_LOGGED_USER
%token SA_PROCESS_NAME
%token SA_PROCESS_ATTACH

%token EXISTS
%token VR_DEF_WHEN_HTTP_COOKIE_IS
%token VR_DEF_WHEN_HTTP_COOKIE_IS_IN
%token VR_DEF_WHEN_HTTP_COOKIE_EXISTS
%token VR_DEF_WHEN_HTTP_HEADER_IS
%token VR_DEF_WHEN_HTTP_HEADER_IS_IN
%token VR_DEF_WHEN_HTTP_HEADER_EXISTS
%token VR_DEF_WHEN_HTTP_QUERY_IS
%token VR_DEF_WHEN_HTTP_QUERY_IS_IN
%token VR_DEF_WHEN_HTTP_QUERY_EXISTS

%token VR_DEF_WHEN_HTTP_ANSWER_CODE_IS
%token VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN
%token VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS
%token VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN
%token VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS
%token VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS
%token VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN
%token VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS

%token VR_DEF_WHEN_HTTP_DATA_IS
%token VR_DEF_WHEN_HTTP_DATA_IS_IN
%token VR_DEF_WHEN_HTTP_DATA_EXISTS

%token VR_DEF_WHEN_HTTP_ANSWER_DATA_IS
%token VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN
%token VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS

%token VR_DEF_WHEN_OBS_TAG_IS
%token VR_DEF_WHEN_OBS_TAG_IS_IN
%token VR_DEF_WHEN_OBS_TAG_EXISTS

%token VR_DEF_WHEN_HTTP_URL_HOST_ID_IS
%token VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN
%token VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS
%token VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN
%token VR_DEF_WHEN_SA_OS_IS
%token VR_DEF_WHEN_SA_OS_IS_IN
%token VR_DEF_WHEN_SA_PROCESS_NAME_IS
%token VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN
%token VR_DEF_WHEN_SA_USER_NAME_IS
%token VR_DEF_WHEN_SA_USER_NAME_IS_IN

%token VR_DEF_WHEN_NET_TCP_IPSRC_IS
%token VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN
%token VR_DEF_WHEN_NET_RESULT_IS
%token VR_DEF_WHEN_NET_TCP_SESID_IS
%token VR_DEF_WHEN_NET_TCP_SESID_IS_IN
%token VR_DEF_CALL_LISP_FUNCTION

%token VR_DEF_WHEN_VREP_GET
%token VR_DEF_WHEN_VREP_EXISTS
%token VR_DEF_WHEN_VREP_FREQ     //num
%token VR_DEF_WHEN_VREP_COUNT    //num
%token VR_DEF_WHEN_VREP_KEY_IS
%token VR_DEF_WHEN_VREP_KEY_IS_IN

%token VR_DEF_ACTION_VREP_CREATE
%token VR_DEF_ACTION_VREP_MCREATE
%token VR_DEF_ACTION_VREP_PLUS_CREATE
%token VR_DEF_ACTION_VREP_FREQ     //num
%token VR_DEF_ACTION_VREP_COUNT    //num
%token VR_DEF_ACTION_VREP_KEY
%token VR_DEF_ACTION_VREP_KV_PAIR
%token KV "KeyValue pair"

%token ACTION_KEY_LIST
%token ACTION_DATA_LIST
%token ACTION_ENABLE_VR
%token ACTION_DISABLE_VR
%token ACTION_ENABLE_AR
%token ACTION_DISABLE_AR
%token ACTION_SETADD
%token ACTION_SETDEL
%token ACTION_SETVAR

%token < string > OBS_EVENT_ACM_LIGHT

%token VR_DEF_IS_A_CLAUSES
%token VR_DEF_IS_A_EXPR
%token VR_DEF_IS_A_TERM
%token VR_DEF_IS_A_FACTOR
%token VR_DEF_IS_A_WHEN_WITH
%token VR_DEF_IS_LOP_OR
%token VR_DEF_IS_LOP_AND
%token VR_DEF_IS_LOP_NEXT
%token VR_DEF_IS_LISTA_WHEN
%token VR_DEF_IS_LISTA_WITH

%token BASIC_ACCESS_RULES
%token EXTENDED_ACCESS_RULES
%token ACCESS_OPERATIONS

//Comandi speciali delle AR
%token ADD_HEADER_TO_ANSWER  "Add header to answer"
%token ADD_HEADER_TO_REQUEST "Add header to request"
%token ADD_V_HEADER_TO_V_ANSWER  "Add var header to var answer"
%token ADD_V_HEADER_TO_V_REQUEST "Add var header to var request"

%token ADD_COOKIE_TO_ANSWER  "Add cookie to answer"
%token ADD_COOKIE_TO_REQUEST "Add cookie to request"
%token ADD_V_COOKIE_TO_V_ANSWER  "Add var cookie to var answer"
%token ADD_V_COOKIE_TO_V_REQUEST "Add var cookie to var request"

%token REPLACE_HEADER_IN_REQUEST "sostituisce l'header utilizzando un pcre"
%token REPLACE_HEADER_IN_ANSWER  "sostituisce l'header utilizzando un pcre"

%token PATTERN "pattern"

%start program

%%

program :   %empty {}
        |
            /*La definizione di insieme*/
            program set_definition {
                driver.addCommand($2);
            }
        |
            /* La definizione di validation rule */
            program vr_definition {
                driver.addCommand($2);
            }
        |
            /* La definizione di access rule */
            program ar_definition {
                driver.addCommand($2);
            }
        |
            END {}
        ;

ar_definition: DEFINE AR ident_or_string
                    access_rules
                    ACTION access_operations
            SEMICOLON
                {
                    $$ = Command(token::TOKEN_AR_DEF, $3);
                    $$.addArg($4);
                    $$.addArg($6);
                }
        ;

access_operations: access_operation access_operations  { $$ = $2; $$.addArg($1);}
    |
                    %empty  { $$ = Command(token::TOKEN_ACCESS_OPERATIONS, ""); }
    ;
    
access_operation:
                    TRACE ident_or_string COMMA ident_or_string COMMA ident_or_string  /* dominio_va, nome cookie, nome insieme */
                    {
                        list<string> lst;
                        lst.push_back($2);
                        lst.push_back($4);
                        lst.push_back($6);
                        $$ = Command(token::TOKEN_TRACE, "", lst);
                    }
                    //Alcuni comandi "SPECIALI" che influenzano tutte le comunicazioni su questo canale.

                    //Questi modificano il bufs_in_out in modo che poi in uscita verso il server o verso il client il bufs sia modificato opportunamente.
    |
                    HTTP_HEADER_REPLACE ident_or_string ident_or_string 
                    { list<string> lst; lst.push_back($2); lst.push_back($3); $$ = Command(token::TOKEN_REPLACE_HEADER_IN_REQUEST, "", lst); }
    |
                    HTTP_ANSWER_HEADER_REPLACE ident_or_string ident_or_string 
                    { list<string> lst; lst.push_back($2); lst.push_back($3); $$ = Command(token::TOKEN_REPLACE_HEADER_IN_ANSWER, "", lst); }
    |
                    HTTP_HEADER         LEFTSQPAR ident_or_string RIGHTSQPAR EQUAL ident_or_string 
                    { list<string> lst; lst.push_back($3); lst.push_back($6); $$ = Command(token::TOKEN_ADD_HEADER_TO_REQUEST, "", lst); }
    |
                    HTTP_ANSWER_HEADER  LEFTSQPAR ident_or_string RIGHTSQPAR EQUAL ident_or_string 
                    { list<string> lst; lst.push_back($3); lst.push_back($6); $$ = Command(token::TOKEN_ADD_HEADER_TO_ANSWER, "", lst); }
    |
                    HTTP_HEADER         LEFTSQPAR ident_or_string EQUAL ident_or_string RIGHTSQPAR EQUAL ident_or_string EQUAL ident_or_string 
                    {   list<string> lst; lst.push_back($3); lst.push_back($5); lst.push_back($8); lst.push_back($10);
                        $$ = Command(token::TOKEN_ADD_V_HEADER_TO_V_REQUEST, "", lst); }
    |
                    HTTP_ANSWER_HEADER  LEFTSQPAR ident_or_string EQUAL ident_or_string RIGHTSQPAR EQUAL ident_or_string EQUAL ident_or_string 
                    {   list<string> lst; lst.push_back($3); lst.push_back($5); lst.push_back($8); lst.push_back($10);
                        $$ = Command(token::TOKEN_ADD_V_HEADER_TO_V_ANSWER, "", lst); }
    |
                    HTTP_COOKIE         LEFTSQPAR ident_or_string RIGHTSQPAR EQUAL ident_or_string 
                    { list<string> lst; lst.push_back($3); lst.push_back($6); $$ = Command(token::TOKEN_ADD_COOKIE_TO_REQUEST, "", lst); }
    |
                    HTTP_ANSWER_COOKIE  LEFTSQPAR ident_or_string RIGHTSQPAR EQUAL ident_or_string 
                    { list<string> lst; lst.push_back($3); lst.push_back($6); $$ = Command(token::TOKEN_ADD_COOKIE_TO_ANSWER, "", lst); }
    |
                    HTTP_ANSWER_COOKIE  LEFTSQPAR ident_or_string EQUAL ident_or_string RIGHTSQPAR EQUAL ident_or_string EQUAL ident_or_string 
                    {   list<string> lst; lst.push_back($3); lst.push_back($5); lst.push_back($8); lst.push_back($10);
                        $$ = Command(token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER, "", lst); }
                        
    //Fine di alcuni comandi "SPECIALI" che influenzano tutte le comunicazioni su questo canale.
    
    //Inizio di comandi che influenzano l'ambiente di valutazione
    
    |               ADD observable TO SET ident_or_string pattern_extractor  {list<Command> lst; 
                        lst.push_back($2); 
                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                        if ($6.size() > 0) $$.addArg($6);
                    }
    |
                    ADD cat_observables TO SET ident_or_string {list<Command> lst; 
                        lst.push_back($2); 
                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                    }
    |
                    ADD observable TO SET ident_or_string I_NUMBER pattern_extractor {list<Command> lst; 
                        lst.push_back($2); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                        if ($7.size() > 0) $$.addArg($7);
                    }
    |
                    ADD cat_observables TO SET ident_or_string I_NUMBER {list<Command> lst; 
                        lst.push_back($2); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                    }
    |
                    DEL observable FROM SET ident_or_string pattern_extractor     {  list<Command> lst; 
                        lst.push_back($2); 
                        $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                        if ($6.size() > 0) $$.addArg($6);
                    }
    |
                    DEL observable FROM SET ident_or_string  I_NUMBER pattern_extractor {   list<Command> lst; 
                        lst.push_back($2); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                        $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                        if ($7.size() > 0) $$.addArg($7);
                    }
    |
                    DEL cat_observables FROM SET ident_or_string {  list<Command> lst; 
                        lst.push_back($2); 
                        $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                    }
    |
                    DEL cat_observables FROM SET ident_or_string  I_NUMBER {   list<Command> lst; 
                        lst.push_back($2); 
                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                        $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                    }
    |
                    CALL ident_or_string WITH ar_observables_list {
                        $$ = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, $2, $4);
                    }
        
    //Fine di comandi che influenzano l'ambiente di valutazione
    
    //Inizio di comandi che influenzano il flusso delle comunicazioni
    
    |
                    TCP_REDIRECT ident_or_string EQUAL ident_or_string ssl_server_options
                    { 
                        list<string> lst; 
                        lst.push_back($2); 
                        lst.push_back($4);
                        for (auto & it: $5)
                            lst.push_back(it);
                        $$ = Command(token::TOKEN_TCP_REDIRECT_VSTRING, "", lst); 
                    }
    |
                    TCP_REDIRECT ident_or_string  ssl_server_options
                    { 
                        list<string> lst; 
                        lst.push_back($2); 
                        for (auto & it: $3)
                            lst.push_back(it);
                        $$ = Command(token::TOKEN_TCP_REDIRECT_STRING, "", lst); 
                    }
    |
                    TCP_REDIRECT ident_or_string EQUAL HTTP_HOST ssl_server_options
                    {
                        list<string> lst; 
                        lst.push_back($2); 
                        for (auto & it: $5)
                            lst.push_back(it);
                        $$ = Command(token::TOKEN_TCP_REDIRECT_VHOST, "", lst); 
                    }
    |
                    TCP_REDIRECT HTTP_HOST  ssl_server_options
                    { 
                        list<string> lst; 
                        for (auto & it: $3)
                            lst.push_back(it);
                        $$ = Command(token::TOKEN_TCP_REDIRECT_HOST, "", lst); 
                    }
    |
                    HTTP_REDIRECT ident_or_string EQUAL ident_or_string { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_HTTP_VREDIRECT, "", lst); }
    |
                    HTTP_REDIRECT ident_or_string { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_HTTP_REDIRECT, "", lst); }
    |
/*                    HTTPS_REDIRECT ident_or_string EQUAL ident_or_string { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_HTTPS_VREDIRECT, "", lst); }
    |
                    HTTPS_REDIRECT ident_or_string { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_HTTPS_REDIRECT, "", lst); }
    |*/
                    BANDWIDTH  ident_or_string EQUAL I_NUMBER { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_VBANDWIDTH, "", lst); }
    |
                    BANDWIDTH  I_NUMBER { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_BANDWIDTH, "", lst); }
    |
                    ANSWER ident_or_string EQUAL ident_or_string { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_VANSWER, "", lst); }
    |
                    ANSWER ident_or_string { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_ANSWER, "", lst); }
    |
                    SCM_MANAGE observable_list { $$ = Command(token::TOKEN_SCM_MANAGE, "", $2); }
    |
                    SCM_GIVE ident_or_string { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_SCM_GIVE, "", lst); }
    |
                    SCM_MANAGE observable_assign_list { $$ = Command(token::TOKEN_VSCM_MANAGE, "", $2); }
    |
                    SCM_GIVE ident_or_string  EQUAL ident_or_string { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_VSCM_GIVE, "", lst); }
//    |
//                    EXPIRY ident_or_string EQUAL I_NUMBER { list<string> lst; lst.push_back($2); lst.push_back($4); $$ = Command(token::TOKEN_VEXPIRY, "", lst); }
//    |
//                    EXPIRY I_NUMBER { list<string> lst; lst.push_back($2); $$ = Command(token::TOKEN_EXPIRY, "", lst); }
    ;

access_rules:   CONDITION basic_access_rules { list<Command> lc; lc.push_back($2); $$=Command(token::TOKEN_IF, "", lc); }
    ;

vr_definition: DEFINE VR ident_or_string 
                    CONDITION clauses    /*lista_as*/
                    /*ACTION lista_action*/
            SEMICOLON
                {
                    list<Command> lst1; lst1.push_back($5);
                    list<Command> lst;
                    lst.push_back(Command(token::TOKEN_VR_DEF_WHEN, "", lst1));
                    //lst.push_back(Command(token::TOKEN_VR_DEF_ACTION, "", $7));
                    $$ = Command(token::TOKEN_VR_DEF, $3, lst);
                }
        ;

clauses:    clauses NEXT expr {auto lst = list<Command>(); lst.push_back($1); lst.push_back($3); $$ = Command(token::TOKEN_VR_DEF_IS_LOP_NEXT, "", lst); }
    |
            expr                {$$ = $1;}
    ;

expr:    expr OR term {auto lst = list<Command>(); lst.push_back($1); lst.push_back($3); $$ = Command(token::TOKEN_VR_DEF_IS_LOP_OR, "", lst); }
    |
        term  {$$ = $1; }
    ;

term:    term AND factor { auto lst = list<Command>(); lst.push_back($1); lst.push_back($3); $$ = Command(token::TOKEN_VR_DEF_IS_LOP_AND, "", lst); }
    |
        factor           {$$ = $1;}
    ;

factor: LEFTPAR clauses RIGHTPAR {$$=$2;}
    |
        lista_when sezione_lista_with ACTION lista_action{
                                        auto c_when =   Command(token::TOKEN_VR_DEF_IS_LISTA_WHEN, "", $1);
                                        auto c_with =   Command(token::TOKEN_VR_DEF_IS_LISTA_WITH, "", $2);
                                        auto c_action = Command(token::TOKEN_VR_DEF_ACTION, "", $4);
                                        $$=Command(token::TOKEN_VR_DEF_IS_A_WHEN_WITH, "");
                                        $$.addArg(c_when);
                                        $$.addArg(c_with);
                                        $$.addArg(c_action);
                                    }
    |
        lista_when sezione_lista_with {
                                        auto c_when =   Command(token::TOKEN_VR_DEF_IS_LISTA_WHEN, "", $1);
                                        auto c_with =   Command(token::TOKEN_VR_DEF_IS_LISTA_WITH, "", $2);
                                        auto c_action = Command(token::TOKEN_VR_DEF_ACTION, "");
                                        $$=Command(token::TOKEN_VR_DEF_IS_A_WHEN_WITH, "");
                                        $$.addArg(c_when);
                                        $$.addArg(c_with);
                                        $$.addArg(c_action);
                                    }
    ;

sezione_lista_with: VAR lista_with { $$ = $2; }
    |
                    %empty {$$=list<Command>();}
    ;
    
lista_with: ident_or_string EQUAL observable pattern_extractor lista_with {   
cout << "lista_with: legge " << $1 << " = " << $3.m_name << endl;
                                                            auto lst = list<Command>(); 
                                                            lst.push_back($3); 
                                                            auto c = Command(token::TOKEN_EQUAL, $1, lst);
                                                            c.addArg($4);
                                                            $$=$5;
                                                            $$.push_back(c); 
                                                        }
    |
            ident_or_string EQUAL cat_observables lista_with {   
                                                            auto lst = list<Command>(); 
                                                            lst.push_back($3); 
                                                            auto c = Command(token::TOKEN_EQUAL, $1, lst);
                                                            $$=$4;
                                                            $$.push_back(c); 
                                                        }
    |
            %empty { $$ = list<Command>(); cout << "lista_with: empty\n";}
    ;

observable:
                        AGENT_ID { $$=Command(token::TOKEN_AGENT_ID, ""); }
    |                   SA_OS { $$ = Command(token::TOKEN_SA_OS, ""); }
    |                   SA_LOGGED_USER { $$ = Command(token::TOKEN_SA_LOGGED_USER, ""); }
    |                   SA_PROCESS_NAME { $$ = Command(token::TOKEN_SA_PROCESS_NAME, ""); }
    |                   OBS_NAME  { $$=Command(token::TOKEN_OBS_NAME, ""); }
    |                   OBS_TIME { $$ = Command(token::TOKEN_OBS_TIME, ""); }
    |                   OBS_OBSID { $$ = Command(token::TOKEN_OBS_OBSID, ""); }
    |                   OBS_STIME { $$ = Command(token::TOKEN_OBS_STIME, ""); }
    |                   OBS_ETIME { $$ = Command(token::TOKEN_OBS_ETIME, ""); }
    |                   OBS_TAG { $$ = Command(token::TOKEN_OBS_TAG, ""); }
    |                   OBS_EVENT { $$=Command(token::TOKEN_OBS_EVENT, ""); }
    |                   NET_TCP_IPDST { $$ = Command(token::TOKEN_NET_TCP_IPDST, ""); }
    |                   NET_TCP_IPSRC { $$ = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
    |                   NET_RESULT { $$ = Command(token::TOKEN_NET_RESULT, ""); }
    |                   NET_TCP_SESID { $$ = Command(token::TOKEN_NET_TCP_SESID, ""); }
    |                   HTTP_METHOD { $$ = Command(token::TOKEN_HTTP_METHOD, ""); }
    |                   HTTP_HOST { $$ = Command(token::TOKEN_HTTP_HOST, ""); }
    |                   HTTP_URL { $$ = Command(token::TOKEN_HTTP_URL, ""); }
    |                   HTTP_URI { $$ = Command(token::TOKEN_HTTP_URI, ""); }
    |                   HTTP_HEADER_HOST { $$ = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
    |                   HTTP_URL_HOST { $$ = Command(token::TOKEN_HTTP_URL_HOST, ""); }
    |                   HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_COOKIE, $3); }
    |                   HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_HEADER, $3); }
    |                   HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_QUERY, $3); }
    |                   HTTP_QUERY { $$ = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
    |                   HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_DATA, $3); }
    |                   HTTP_DATA  { $$ = Command(token::TOKEN_HTTP_DATA, "0_FULL_QUERY"); }
    |                   HTTP_ANSWER_CODE { $$ = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
    |                   HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_COOKIE, $3); }
    |                   HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_HEADER, $3); }
    |                   HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, $3); }
    |                   HTTP_ANSWER_DATA { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }

    |                   IDENT  { $$ = Command(token::TOKEN_IDENT, $1); }
    |                   STRING { 
                                    $$ = Command(token::TOKEN_STRING, $1); 
                                }
    ;

observables_list:       observable pattern_extractor {$$ = list<Command>(); auto a = $1; if ($2.size() > 0) a.addArg($2); $$.push_back(a);}
    |
                        observables_list COMMA observable pattern_extractor {$$=$1; auto a = $3; if ($4.size() > 0) a.addArg($4);  $$.push_back(a);}
    ;
    
cat_observables:        CAT LEFTGRPAR observables_list RIGHTGRPAR {$$=Command(token::TOKEN_CAT, "", $3);}
    ;
    
    
    
ar_observable:          OBS_TAG { $$ = Command(token::TOKEN_OBS_TAG, ""); }
    |                   NET_TCP_IPDST { $$ = Command(token::TOKEN_NET_TCP_IPDST, ""); }
    |                   NET_TCP_IPSRC { $$ = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
    |                   NET_RESULT { $$ = Command(token::TOKEN_NET_RESULT, ""); }
    |                   NET_TCP_SESID { $$ = Command(token::TOKEN_NET_TCP_SESID, ""); }
    |                   HTTP_METHOD { $$ = Command(token::TOKEN_HTTP_METHOD, ""); }
    |                   HTTP_HOST { $$ = Command(token::TOKEN_HTTP_HOST, ""); }
    |                   HTTP_URL { $$ = Command(token::TOKEN_HTTP_URL, ""); }
    |                   HTTP_URI { $$ = Command(token::TOKEN_HTTP_URI, ""); }
    |                   HTTP_HEADER_HOST { $$ = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
    |                   HTTP_URL_HOST { $$ = Command(token::TOKEN_HTTP_URL_HOST, ""); }
    |                   HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_COOKIE, $3); }
    |                   HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_HEADER, $3); }
    |                   HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_QUERY, $3); }
    |                   HTTP_QUERY { $$ = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
    |                   HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_DATA, $3); }
    |                   HTTP_DATA  { $$ = Command(token::TOKEN_HTTP_DATA, "0_FULL_QUERY"); }
    |                   HTTP_ANSWER_CODE { $$ = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
    |                   HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_COOKIE, $3); }
    |                   HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_HEADER, $3); }
    |                   HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, $3); }
    |                   HTTP_ANSWER_DATA { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }

    |                   IDENT  { $$ = Command(token::TOKEN_IDENT, $1);  }
    |                   STRING { $$ = Command(token::TOKEN_STRING, $1); }
    |                   TIME   { $$ = Command(token::TOKEN_TIME, "");   }
    ;

ar_observables_list:    ar_observable pattern_extractor {$$ = list<Command>(); auto a = $1; if ($2.size() > 0) a.addArg($2); $$.push_back(a);}
    |
                        ar_observables_list COMMA ar_observable pattern_extractor {$$=$1; auto a = $3; if ($4.size() > 0) a.addArg($4);  $$.push_back(a);}
    ;

ar_cat_observables:     CAT LEFTGRPAR ar_observables_list RIGHTGRPAR {$$=Command(token::TOKEN_CAT, "", $3);}
    ;

lista_when:    item_when { $$=list<Command>(); $$.push_back($1); }
    |
                lista_when item_when { $$=$1; $$.push_back($2); }
    |
                %empty { $$ = list<Command>(); }
    ;
    
pattern_extractor:  STRING STRING { $$=Base64::encode($1) + ":" + Base64::encode($2); } /*pattern substitute*/
    |
                    %empty            { $$=string(); }
    ;

item_when:
        NOT basic_item_when {auto negate = Command(token::TOKEN_NOT, "NOT"); $$=$2; $$.addArg(negate);}
    |
        basic_item_when {$$=$1;}
    ;
    
basic_item_when:
                OBS_NAME IS IN ident_or_string pattern_extractor { $$=Command(token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |               
                OBS_NAME IS ident_or_string pattern_extractor { $$=Command(token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                OBS_EVENT IS network_event pattern_extractor { $$=Command(token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS, $3); if ($4.size() > 0) $$.addArg($4); }
    |
                NET_TCP_IPDST IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                NET_TCP_IPDST IS ip_address  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_METHOD IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_METHOD IS http_method pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_HOST IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_HOST IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_URL IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_URL IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_URI IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_URI IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                AGENT_ID IS IN ident_or_string pattern_extractor { $$=Command(token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |               
                AGENT_ID IS ident_or_string pattern_extractor { $$=Command(token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_HEADER_HOST IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_HEADER_HOST IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_URL_HOST IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_URL_HOST IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                EXISTS HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, $4); if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, $3, lst);if ($7.size() > 0) $$.addArg($7); 
                                                                                }
    |
                HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, $3, lst); if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                EXISTS HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, $4); if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, $3, lst); 
                                                                                        if ($7.size() > 0) $$.addArg($7);
                                                                                    }
    |
                HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                EXISTS HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, $4); if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_QUERY IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, "0_FULL_QUERY", lst);  if ($4.size() > 0) $$.addArg($4);
                                                                                }
    |
                HTTP_QUERY  IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }
    |
                EXISTS HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, $4); 
                                                                                            if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_DATA IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, "0_FULL_QUERY", lst);  if ($4.size() > 0) $$.addArg($4);
                                                                                }
    |
                HTTP_DATA IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }
    |
    
    
                EXISTS HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, $4); 
                                                                                                    if ($6.size() > 0) $$.addArg($6);
                                                                                                }
    |
                HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_ANSWER_DATA IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, "0_FULL_DATA", lst);  if ($4.size() > 0) $$.addArg($4);
                                                                                }
    |
                HTTP_ANSWER_DATA IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, "0_FULL_DATA", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }

                                                                                    
                                                                                    |
                EXISTS HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS, $4);
                                                                                                    if ($6.size() > 0) $$.addArg($6);
                                                                                                    }
    |
                HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                EXISTS HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS, $4);
                                                                                                    if ($6.size() > 0) $$.addArg($6);
                                                                                                }
    |
                HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor { list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_ANSWER_CODE IS ident_or_string                              pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_ANSWER_CODE IS IN ident_or_string                           pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                EXISTS OBS_TAG LEFTSQPAR ident_or_string RIGHTSQPAR  pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS, $4); if ($6.size() > 0) $$.addArg($6);}
    |
                OBS_TAG IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, $3);  if ($4.size() > 0) $$.addArg($4);} 
    |
                OBS_TAG IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                SA_OS IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_OS_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                SA_OS IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                SA_PROCESS_NAME IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                SA_PROCESS_NAME IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                SA_LOGGED_USER IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                SA_LOGGED_USER IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                NET_TCP_IPSRC IS IN ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, $4); if ($5.size() > 0) $$.addArg($5);}
    |
                NET_TCP_IPSRC IS IP_WITH_STAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                NET_RESULT IS true_false pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                NET_TCP_SESID IS ident_or_string pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, $3); if ($4.size() > 0) $$.addArg($4);}
    |
                cat_observables IS IN ident_or_string {$$ = $1; $$.m_name = $4;}
    |
                CALL ident_or_string WITH observables_list {
                    $$ = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, $2, $4);
                }
    ;

basic_access_rules: item_basic_access_rule basic_access_rules  {$$=$2; $$.addArg($1);}
    |
                    item_basic_access_rule {$$=Command(token::TOKEN_BASIC_ACCESS_RULES, ""); $$.addArg($1);}
    ;

//extended_access_rules: item_extended_access_rule extended_access_rules   {$$=$2; $$.addArg($1);}
//    |
//                    %empty  {$$=Command(token::TOKEN_EXTENDED_ACCESS_RULES, "");}
//    ;

item_basic_access_rule:
        NOT item_basic_access_rule_with_not {auto negate = Command(token::TOKEN_NOT, "NOT"); $$=$2; $$.addArg(negate);}
    |
        item_basic_access_rule_with_not {$$=$1;}
    ;


item_basic_access_rule_with_not:
                NET_TCP_IPDST IS ip_address pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_METHOD IS http_method pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_HOST IS ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_URL IS ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                HTTP_URI IS ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                EXISTS HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS, $4);  
                                                                                            if ($6.size() > 0) $$.addArg($6);
                                                                                            }
    |
                HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                EXISTS HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS, $4);  
                                                                                            if ($6.size() > 0) $$.addArg($6);
                                                                                            }
    |
                HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor{ list<string> lst; lst.push_back($6); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS, $3, lst); 
                                                                                        if ($7.size() > 0) $$.addArg($7);
                                                                                    }
    |
                EXISTS HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS, $4);  if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_QUERY  IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS, "0_FULL_QUERY", lst); 
                                                                                    if ($4.size() > 0) $$.addArg($4);
                                                                                }
    |
                EXISTS HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS, $4);  if ($6.size() > 0) $$.addArg($6);}
    |
                HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_DATA IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS, "0_FULL_QUERY", lst); 
                                                                                    if ($4.size() > 0) $$.addArg($4);
                                                                                }

    |
                EXISTS HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR pattern_extractor { $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS, $4);  
                                                                                                if ($6.size() > 0) $$.addArg($6);
                                                                                                }
    |
                HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($6); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, $3, lst); 
                                                                                    if ($7.size() > 0) $$.addArg($7);
                                                                                }
    |
                HTTP_ANSWER_DATA IS ident_or_string pattern_extractor { list<string> lst; lst.push_back($3); 
                                                                                    $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS, "0_FULL_DATA", lst); 
                                                                                    if ($4.size() > 0) $$.addArg($4);
                                                                                }
                                                                                
                                                                                
                                                                                
                                                                                //    |
//                EXISTS OBS_TAG LEFTSQPAR ident_or_string RIGHTSQPAR  { $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS, $4); }
//    |
//                OBS_TAG IS ident_or_string { $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, $3); } 
    |
                NET_TCP_SESID IS ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                NET_TCP_IPSRC IS ip_address pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS, $3);  if ($4.size() > 0) $$.addArg($4);}
    |
                NET_TCP_IPDST IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_HOST IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_URL IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_URI IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_QUERY IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }
    |
                HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_DATA IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN, "0_FULL_QUERY", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }

    |
                HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($7); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, $3, lst); 
                                                                                        if ($8.size() > 0) $$.addArg($8);
                                                                                    }
    |
                HTTP_ANSWER_DATA IS IN ident_or_string pattern_extractor{ list<string> lst; lst.push_back($4); 
                                                                                        $$ = Command(token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN, "0_FULL_DATA", lst); 
                                                                                        if ($5.size() > 0) $$.addArg($5);
                                                                                    }
                                                                                    
                                                                                    
                                                                                    |
                OBS_TAG IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                NET_TCP_IPSRC IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                TIME IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_TIME, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                NET_TCP_SESID IS IN ident_or_string pattern_extractor{ $$ = Command(token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN, $4);  if ($5.size() > 0) $$.addArg($5);}
    |
                ar_cat_observables IS IN ident_or_string {$$=$1; $$.addArg($4);}
    |
                CALL ident_or_string WITH ar_observables_list {
                    $$ = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, $2, $4);
                }
    ;

true_false: TRUE { $$ = "TRUE"; }
    |
            FALSE { $$ = "FALSE"; }
    ;

lista_action:   item_action pattern_extractor { $$=list<Command>(); auto a = $1; if ($2.size() > 0) a.addArg($2); $$.push_back(a); }
    |
                lista_action item_action pattern_extractor { $$=$1; auto itac = $2; if ($3.size() > 0) itac.addArg($3); $$.push_back(itac); }
    ;
    
item_action:    //Creo un VReport. Uso e concateno tutti i dati che mi servono e che prendo dall'osservazione
//                REPORT VREP ident_or_string PLUS observable action_variable_list { 
//                                                                    list<Command> lc; lc.push_back($5);
//                                                                    lc.push_back(Command(token::TOKEN_ACTION_KEY_LIST, "", $6));
//                                                                    $$ = Command(token::TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE, $3, lc);
//                                                                  }
//    |
                REPORT ident_or_string LEFTGRPAR action_variable_list RIGHTGRPAR { 
                                                                    $$ = Command(token::TOKEN_VR_DEF_ACTION_VREP_CREATE, $2, $4);
                                                                }
    |
                //Appendo a un DB
                MYSQL_APPEND LEFTPAR
                    ip_or_domain COMMA      //S: IP e porta eventuali
                    ident_or_string COMMA   //S: DB
                    ident_or_string COMMA   //S: USER
                    ident_or_string COMMA   //S: PASSWORD
                    ident_or_string COMMA   //S: the query (with $xxx internally)
                    LEFTGRPAR action_variable_list RIGHTGRPAR   //C: Valore da appendere
                RIGHTPAR
            {
                $$=Command(token::TOKEN_MYSQL_APPEND, "");
                $$.addArg(Command(token::TOKEN_IDENT, $3)); 
                $$.addArg(Command(token::TOKEN_IDENT, $5)); 
                $$.addArg(Command(token::TOKEN_IDENT, $7)); 
                $$.addArg(Command(token::TOKEN_IDENT, $9)); 
                $$.addArg(Command(token::TOKEN_IDENT, $11));
                for (auto cmd: $14)
                    $$.addArg(cmd);
            }
    |
            ENABLE VR ident_or_string {$$=Command(token::TOKEN_ACTION_ENABLE_VR, $3);}
    |
            DISABLE VR ident_or_string {$$=Command(token::TOKEN_ACTION_DISABLE_VR, $3);}
    |
            ENABLE AR ident_or_string {$$=Command(token::TOKEN_ACTION_ENABLE_AR, $3);}
    |
            DISABLE AR ident_or_string {$$=Command(token::TOKEN_ACTION_DISABLE_AR, $3);}
    |
            ADD observable TO SET ident_or_string pattern_extractor  {list<Command> lst; 
                                                        lst.push_back($2); 
                                                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                                                        if ($6.size() > 0) $$.addArg($6);
                                                        }
    |
            ADD cat_observables TO SET ident_or_string {list<Command> lst; 
                                                        lst.push_back($2); 
                                                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                                                    }
    |
            ADD observable TO SET ident_or_string I_NUMBER pattern_extractor {list<Command> lst; 
                                                        lst.push_back($2); 
                                                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                                                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                                                        if ($7.size() > 0) $$.addArg($7);
                                                        }
    |
            ADD cat_observables TO SET ident_or_string I_NUMBER {list<Command> lst; 
                                                        lst.push_back($2); 
                                                        lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                                                        $$=Command(token::TOKEN_ACTION_SETADD, $5, lst);
                                                        }
    |
            DEL observable FROM SET ident_or_string pattern_extractor     {  list<Command> lst; 
                                                            lst.push_back($2); 
                                                            $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                                                            if ($6.size() > 0) $$.addArg($6);
                                                        }
    |
            DEL observable FROM SET ident_or_string  I_NUMBER pattern_extractor {   list<Command> lst; 
                                                                    lst.push_back($2); 
                                                                    lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                                                                    $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                                                                    if ($7.size() > 0) $$.addArg($7);
                                                        }
    |

            DEL cat_observables FROM SET ident_or_string {  list<Command> lst; 
                                                            lst.push_back($2); 
                                                            $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                                                        }
    |
            DEL cat_observables FROM SET ident_or_string  I_NUMBER {   list<Command> lst; 
                                                                    lst.push_back($2); 
                                                                    lst.push_back(Command(token::TOKEN_I_NUMBER, $6));
                                                                    $$=Command(token::TOKEN_ACTION_SETDEL, $5, lst);
                                                                    }
    |
            SET ident_or_string AT ident_or_string EQUAL observable pattern_extractor {
                                                            list<Command> lst;
                                                            lst.push_back(Command(token::TOKEN_STRING, $2));
                                                            lst.push_back(Command(token::TOKEN_STRING, $4));
                                                            lst.push_back($6);
                                                            $$=Command(token::TOKEN_ACTION_SETVAR, "", lst);
                                                            if ($7.size() > 0) $$.addArg($7);
                                                        }
    |
            SET ident_or_string AT ident_or_string EQUAL cat_observables {
                                                            list<Command> lst;
                                                            lst.push_back(Command(token::TOKEN_STRING, $2));
                                                            lst.push_back(Command(token::TOKEN_STRING, $4));
                                                            lst.push_back($6);
                                                            $$=Command(token::TOKEN_ACTION_SETVAR, "", lst);
                                                            }
    |
            CALL ident_or_string WITH observables_list {
                $$ = Command(token::TOKEN_VR_DEF_CALL_LISP_FUNCTION, $2, $4);
            }
    ;

//Qui ci va proprio tutto quanto potrebbe avere un valore!!!
action_variable_list:   action_variable_item pattern_extractor { $$ = list<Command>(); auto a=$1; if ($2.size() > 0) a.addArg($2); $$.push_back(a);}
    |
                        action_variable_list COMMA action_variable_item pattern_extractor { $$=$1; auto a=$3; if ($4.size() > 0) a.addArg($4); $$.push_back(a); }
    ;

action_variable_item:   AGENT_ID { $$=Command(token::TOKEN_AGENT_ID, ""); }
    |                   OBS_NAME  { $$=Command(token::TOKEN_OBS_NAME, ""); }
    |                   OBS_EVENT { $$=Command(token::TOKEN_OBS_EVENT, ""); }
    |                   NET_TCP_IPDST { $$ = Command(token::TOKEN_NET_TCP_IPDST, ""); }
    |                   HTTP_METHOD { $$ = Command(token::TOKEN_HTTP_METHOD, ""); }
    |                   HTTP_HOST { $$ = Command(token::TOKEN_HTTP_HOST, ""); }
    |                   HTTP_URL { $$ = Command(token::TOKEN_HTTP_URL, ""); }
    |                   HTTP_URI { $$ = Command(token::TOKEN_HTTP_URI, ""); }
    |                   HTTP_HEADER_HOST { $$ = Command(token::TOKEN_HTTP_HEADER_HOST, ""); }
    |                   HTTP_URL_HOST { $$ = Command(token::TOKEN_HTTP_URL_HOST, ""); }
    |                   HTTP_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_COOKIE, $3); }
    |                   HTTP_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_HEADER, $3); }
    |                   HTTP_QUERY { $$ = Command(token::TOKEN_HTTP_QUERY, "0_FULL_QUERY"); }
    |                   HTTP_QUERY LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_QUERY, $3); }
    |                   HTTP_DATA { $$ = Command(token::TOKEN_HTTP_DATA, "0_FULL_DATA"); }
    |                   HTTP_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_DATA, $3); }
    |                   HTTP_ANSWER_DATA { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, "0_FULL_DATA"); }
    |                   HTTP_ANSWER_DATA LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_DATA, $3); }
    |                   HTTP_ANSWER_HEADER LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_HEADER, $3); }
    |                   HTTP_ANSWER_COOKIE LEFTSQPAR ident_or_string RIGHTSQPAR { $$ = Command(token::TOKEN_HTTP_ANSWER_COOKIE, $3); }
    |                   HTTP_ANSWER_CODE { $$ = Command(token::TOKEN_HTTP_ANSWER_CODE, ""); }
    |                   OBS_TAG { $$ = Command(token::TOKEN_OBS_TAG, ""); }
    |                   SA_OS { $$ = Command(token::TOKEN_SA_OS, ""); }
    |                   SA_PROCESS_NAME { $$ = Command(token::TOKEN_SA_PROCESS_NAME, ""); }
    |                   SA_LOGGED_USER { $$ = Command(token::TOKEN_SA_LOGGED_USER, ""); }
    |                   NET_TCP_IPSRC { $$ = Command(token::TOKEN_NET_TCP_IPSRC, ""); }
    |                   NET_RESULT { $$ = Command(token::TOKEN_NET_RESULT, ""); }
    |                   NET_TCP_SESID { $$ = Command(token::TOKEN_NET_TCP_SESID, ""); }
    |                   OBS_TIME { $$ = Command(token::TOKEN_OBS_TIME, ""); }
    |                   OBS_OBSID { $$ = Command(token::TOKEN_OBS_OBSID, ""); }
    |                   OBS_STIME { $$ = Command(token::TOKEN_OBS_STIME, ""); }
    |                   OBS_ETIME { $$ = Command(token::TOKEN_OBS_ETIME, ""); }
    
                        //il nome di una variabile (che è stata settata con le VAR!!!
    |                   ident_or_string {$$ = Command(token::TOKEN_IDENT, $1);}
    
    |                   cat_observables {$$ = $1;}
    
    ;
    
http_method:    HTTP_METHOD_GET {$$=$1.substr(5);}
    |
                HTTP_METHOD_POST {$$=$1.substr(5);}
    |
                HTTP_METHOD_HEAD {$$=$1.substr(5);}
    |
                HTTP_METHOD_PUT {$$=$1.substr(5);}
    |
                HTTP_METHOD_DELETE {$$=$1.substr(5);}
    |
                HTTP_METHOD_TRACE {$$=$1.substr(5);}
    |
                HTTP_METHOD_OPTIONS {$$=$1.substr(5);}
    |
                HTTP_METHOD_CONNECT {$$=$1.substr(5);}
    |
                HTTP_METHOD_PATCH {$$=$1.substr(5);}
    ;

set_definition:
            DEFINE DICTIONARY ident_or_string EQUAL LEFTGRPAR ident_or_string_pairs_list RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, $3, $6);
                $$=cmd;
            }
        |
            DEFINE DICTIONARY ident_or_string EQUAL LEFTGRPAR RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, $3);
                $$=cmd;
            }
        |
            DEFINE DICTIONARY ident_or_string EQUAL MYSQL LEFTGRPAR
                ip_or_domain COMMA   //IP e porta eventuali
                ident_or_string COMMA   //DB
                ident_or_string COMMA   //USER
                ident_or_string COMMA   //PASSWORD
                ident_or_string         //QUERY
            RIGHTGRPAR SEMICOLON
            {
                list<string> lst;
                lst.push_back($7);
                lst.push_back($9);
                lst.push_back($11);
                lst.push_back($13);
                lst.push_back($15);
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_DICTIONARY_LIST_ID, $3, lc);
                $$=cmd;
            }
        |
            DEFINE SET ident_or_string EQUAL LEFTGRPAR ident_or_string_list RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_SET_LIST_ID, $3, $6);
                $$=cmd;
            }
        |
            DEFINE SET ident_or_string EQUAL LEFTGRPAR  RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_SET_LIST_ID, $3);
                $$=cmd;
            }
        |
            DEFINE SET ident_or_string EQUAL MYSQL LEFTGRPAR
                ip_or_domain COMMA   //IP e porta eventuali
                ident_or_string COMMA   //DB
                ident_or_string COMMA   //USER
                ident_or_string COMMA   //PASSWORD
                ident_or_string         //QUERY
            RIGHTGRPAR SEMICOLON
            {
                list<string> lst;
                lst.push_back($7);
                lst.push_back($9);
                lst.push_back($11);
                lst.push_back($13);
                lst.push_back($15);
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_ID, $3, lc);
                $$=cmd;
            }
        |
//            DEFINE PARSET ident_or_string EQUAL MYSQL LEFTGRPAR
//                ip_or_domain COMMA   //IP e porta eventuali
//                ident_or_string COMMA   //DB
//                ident_or_string COMMA   //USER
//                ident_or_string COMMA   //PASSWORD
//                ident_or_string         //QUERY
//            RIGHTGRPAR SEMICOLON {
//                list<string> lst;
//                lst.push_back($7);
//                lst.push_back($9);
//                lst.push_back($11);
//                lst.push_back($13);
//                lst.push_back($15);
//                Command cmd(token::TOKEN_SET_MYSQL, $3, lst);
//                $$=cmd;
//            }
//        |
            DEFINE IPSET ident_or_string EQUAL LEFTGRPAR ip_with_star_list RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_SET_LIST_IPSTAR, $3, $6);
                $$=cmd;
            }
        |
            DEFINE IPSET ident_or_string EQUAL MYSQL LEFTGRPAR
                ip_or_domain COMMA   //IP e porta eventuali
                ident_or_string COMMA   //DB
                ident_or_string COMMA   //USER
                ident_or_string COMMA   //PASSWORD
                ident_or_string         //QUERY
            RIGHTGRPAR SEMICOLON
            {
                list<string> lst;
                lst.push_back($7);
                lst.push_back($9);
                lst.push_back($11);
                lst.push_back($13);
                lst.push_back($15);
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_IPSTAR, $3, lc);
                $$=cmd;
            }
        |
            DEFINE TIMESET ident_or_string EQUAL LEFTGRPAR timing_list RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_SET_LIST_TIMING, $3, $6);
                $$=cmd;
            }
        |
            DEFINE TIMESET ident_or_string EQUAL MYSQL LEFTGRPAR
                ip_or_domain COMMA   //IP e porta eventuali
                ident_or_string COMMA   //DB
                ident_or_string COMMA   //USER
                ident_or_string COMMA   //PASSWORD
                ident_or_string         //QUERY
            RIGHTGRPAR SEMICOLON
            {
                list<string> lst;
                lst.push_back($7);
                lst.push_back($9);
                lst.push_back($11);
                lst.push_back($13);
                lst.push_back($15);
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_TIMING, $3, lc);
                $$=cmd;
            }
        |
            DEFINE URLSET ident_or_string EQUAL LEFTGRPAR url_with_star_list RIGHTGRPAR SEMICOLON
            {
                Command cmd(token::TOKEN_SET_LIST_URLSTAR, $3, $6);
                $$=cmd;
            }
        |
            DEFINE URLSET ident_or_string EQUAL MYSQL LEFTGRPAR
                ip_or_domain COMMA   //IP e porta eventuali
                ident_or_string COMMA   //DB
                ident_or_string COMMA   //USER
                ident_or_string COMMA   //PASSWORD
                ident_or_string         //QUERY
            RIGHTGRPAR SEMICOLON
            {
                list<string> lst;
                lst.push_back($7);
                lst.push_back($9);
                lst.push_back($11);
                lst.push_back($13);
                lst.push_back($15);
                Command cmdsql(token::TOKEN_SET_MYSQL, "", lst);
                list<Command> lc; lc.push_back(cmdsql);

                Command cmd(token::TOKEN_SET_LIST_URLSTAR, $3, lc);
                $$=cmd;
            }
        ;

//network_events: network_event    { list<Command> lst; lst.push_back($1); $$ = lst; }
//    |
//                network_events COMMA network_event {$$.push_back($3);}
//    ;


check_or_not_the_server_cert: %empty { $$ = false; }
    |
                                LEFTPAR 'T' RIGHTPAR { $$ = true; }
    |
                                LEFTPAR 't' RIGHTPAR { $$ = true; }
    |
                                LEFTPAR 'F' RIGHTPAR { $$ = false; }
    |
                                LEFTPAR 'f' RIGHTPAR { $$ = false; }
    ;

ssl_server_options: %empty { $$ = list<string>(); }
    |
            LEFTSQPAR STRING COMMA STRING COMMA STRING COMMA STRING check_or_not_the_server_cert RIGHTSQPAR  { //server_ca server_hostname client_cert client_key
                auto ls = list<string>();
                ls.push_back($2);
                ls.push_back($4);
                ls.push_back($6);
                ls.push_back($8);
                ls.push_back($9?"CHECK":"DONT CHECK");
                $$=ls;
            }
    |
            LEFTSQPAR STRING COMMA STRING check_or_not_the_server_cert RIGHTSQPAR { //server_ca server_hostname
                auto ls = list<string>();
                ls.push_back($2);
                ls.push_back($4);
                ls.push_back($5?"CHECK":"DONT CHECK");
                $$=ls;
            }
    |
            LEFTSQPAR STRING check_or_not_the_server_cert RIGHTSQPAR { //server_ca
                auto ls = list<string>();
                ls.push_back($2);
                ls.push_back($3?"CHECK":"DONT CHECK");
                $$=ls;
            }
    ;



network_event:  NET_TCP_CONNECT {$$=string("NET_TCP_CONNECT");}
    |
                NET_TCP_ACCEPT {$$=string("NET_TCP_ACCEPT");}
    |
                NET_TCP_SEND {$$=string("NET_TCP_SEND");}
    |
                NET_TCP_RECV {$$=string("NET_TCP_RECV");}
    |
                NET_TCP_CLOSE {$$=string("NET_TCP_CLOSE");}
    ;
        
timing_list: TIMING {$$ = list<string>(); $$.push_back($1); }
    |
            timing_list COMMA TIMING {$$=$1; $$.push_back($3);}
    ;

ip_with_star_list:  ip_address    {
                                        $$ = list<string>();
                                        $$.push_back($1);
                                    }
        |
                    ip_with_star_list COMMA ip_address
                                    {
                                        $$=$1;
                                        $$.push_back($3);
                    }
    ;

url_with_star_list:  URL_WITH_STAR   {
                                        $$ = list<string>();
                                        $$.push_back($1);
                                    }
        |
                    url_with_star_list COMMA URL_WITH_STAR
                                    {
                                        $$=$1;
                                        $$.push_back($3);
                                    }
    ;
    

ident_or_string_list:  ident_or_string  {
                                            $$ = list<string>();
                                            $$.push_back($1);
                                        }
    |
                        ident_or_string_list COMMA ident_or_string
                                        {
                                            $$=$1;
                                            $$.push_back($3);
                                        }
    ;        

    
ident_or_string_pairs_list: LEFTPAR ident_or_string COMMA ident_or_string RIGHTPAR {
                                            $$ = list<pair<string, string>>();
                                            $$.push_back(make_pair($2, $4));
                                        }
    |
                        ident_or_string_pairs_list COMMA LEFTPAR ident_or_string COMMA ident_or_string RIGHTPAR
                                        {
                                            $$=$1;
                                            $$.push_back(make_pair($4, $6));
                                        }
    ;        


observable_list:  observable
                            {
                                $$ = list<Command>();
                                $$.push_back($1);
                            }
    |
                        observable_list COMMA observable
                        {
                            $$=$1;
                            $$.push_back($3);
                        }
    ;        

observable_assign_list: ident_or_string EQUAL observable {
                                            $$ = list<Command>();
                                            $$.push_back(Command(token::TOKEN_STRING, $1));
                                            $$.push_back($3);
                                        }
    |
                                observable_assign_list COMMA ident_or_string EQUAL observable
                                        {
                                            $$=$1;
                                            $$.push_back(Command(token::TOKEN_STRING, $3));
                                            $$.push_back($5);
                                        }
    ;

ident_or_string:
                    IDENT   { $$ = $1; }
    | 
                    STRING   { $$ = $1; }
    ;

ip_address: IP_WITH_STAR {$$=$1;}
        |
            IP_WITHOUT_STAR  {$$=$1;}
        ;
        
ip_or_domain:   ip_address {$$=$1;}
    |
                ident_or_string {$$=$1;}
    ;
%%

// Bison expects us to provide implementation - otherwise linker complains
void VaParser::Parser::error(const location &loc , const std::string &message) {
        
    // Location should be initialized inside scanner action, but is not in this example.
    // Let's grab location directly from driver class.
    // cout << "Error: " << message << endl << "Location: " << loc << endl;
    
    cout << "Error: " << message << ". At " << driver.prs_lineno << "(" << driver.location() << ", " << loc << ")" << endl;
}


