%{
	#include <iostream>
	#include <cstdlib>
	#include "va_scanner.h"
	#include "interpreter.h"
	#include "va_parser.hpp"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() VaParser::Parser::make_END(VaParser::location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION m_driver.increaseLocation(yyleng);
	
	// !!!WARNING!!!
	// Location API is used, but the location is not initialized, 'cause I'm lazy. When making
	// a token with make_{something} method you can pass detailed token location. Current location
	// is accessible with m_driver.location() method. All puzzle elements are there - just
	// pass location value in every action code block below. I'm going to waste more time writing
	// this excuse than putting this boilerplate below...
	//
	// Location class can be found in location.hh and posistion.hh files. It's just a bit too much
	// boilerplate for this small example. Bummer.

%}

/* %option 8bit */
/* %option warn */
/* %option debug */
%option nodefault
%option noyywrap
%option c++
%option case-insensitive
%option yyclass="Scanner"
%option prefix="VaParser_"

WS      [ \t]+
DIG     [0-9]
/*FLOAT   {DIG}*\.{DIG}+*/
INTEGER {DIG}+

%x str

%%

\"	{
          m_driver.stringa_tipo_c.clear();
          BEGIN(str);
        }

<str>\"	{
            BEGIN(INITIAL);
            //printf("Letta stringa tipo c: <%s>\n", stringa_tipo_c.c_str());
            return VaParser::Parser::make_STRING(m_driver.stringa_tipo_c, VaParser::location());
        }

<str>\n {
            printf("Error! unterminated string constant at line %d\n", ++m_driver.prs_lineno);
            exit(0);
        }

<str>\\[0-7]{3} {
		 /* octal escape sequence */
		 int result;
		 (void) sscanf( yytext + 1, "%o", &result );

		 if ( result > 0xff ) {
			 /* error, constant is out-of-bounds */
			printf("La costante %s supera il valore 255\n", yytext);
		 }
        m_driver.stringa_tipo_c.push_back(result);
      }

<str>\\0x[0-9a-fA-F]{2} {
                            /* Hex escape sequence */
                            int result;

                            (void) sscanf( yytext + 3, "%x", &result );

                            if ( result > 0xff ) {
                                            /* error, constant is out-of-bounds */
                                   printf("La costante %s supera il valore 255\n", yytext);
                            }

                            m_driver.stringa_tipo_c.push_back(result);
                          }

<str>\\0d[0-9]{3} {
                        /* Decimal escape sequence */
                        int result;

                        (void) sscanf( yytext + 3, "%x", &result );

                        if ( result > 0xff ) {
                                        /* error, constant is out-of-bounds */
                               printf("La costante %s supera il valore 255\n", yytext);
                        }
                m_driver.stringa_tipo_c.push_back(result);
                  }

<str>\\[0-9]+ {
                 /* generate error - bad escape sequence; something
                  * like '\48' or '\0777777'
                  */
                    cout << "Errore! bad escape sequence at line " <<  VaParser::location() << endl;
                    exit(0);
              }

<str>\\n  { ++m_driver.prs_lineno; m_driver.stringa_tipo_c +="\n"; }
<str>\\r  { ++m_driver.prs_lineno; m_driver.stringa_tipo_c +="\r"; }
<str>\\t  { m_driver.stringa_tipo_c += "\t"; }
<str>\\b  { m_driver.stringa_tipo_c += "\b"; }
<str>\\f  { m_driver.stringa_tipo_c += "\f"; }
<str>\\\\  { m_driver.stringa_tipo_c += "\\"; }

<str>\\\"   { m_driver.stringa_tipo_c += "\""; }
<str>\\.  { m_driver.stringa_tipo_c += string(yytext[1], 1); }
<str>\\\n { ++m_driver.prs_lineno; m_driver.stringa_tipo_c += string(yytext[1], 1); }

<str>[^\\\n\"]+         {
                          m_driver.stringa_tipo_c += yytext;
                        }
                        
"(*"	{	/*Elimina i commenti standard!!*/
                int c;

                while((c = yyinput()) != 0) {
                    if (c == '\n')
                        ++m_driver.prs_lineno;
                    else if(c == '*') {
                            if ((c = yyinput()) == ')')
                                    break;
                            else
                                    unput(c);
                        }
                }
        }

\/\/.*\n    ++m_driver.prs_lineno; /*Elimina i commenti non standard / / */
^\#.*\n     ++m_driver.prs_lineno; /*Elimina i commenti non standard / / */

"!"         return VaParser::Parser::make_NOT(VaParser::location());
"not"         return VaParser::Parser::make_NOT(VaParser::location());
or         return VaParser::Parser::make_OR(VaParser::location());
and         return VaParser::Parser::make_AND(VaParser::location());
set         return VaParser::Parser::make_SET(VaParser::location());
dictionary         return VaParser::Parser::make_DICTIONARY(VaParser::location());
urlset         return VaParser::Parser::make_URLSET(VaParser::location());
ipset         return VaParser::Parser::make_IPSET(VaParser::location());
timeset         return VaParser::Parser::make_TIMESET(VaParser::location());
parset         return VaParser::Parser::make_PARSET(VaParser::location());
next         return VaParser::Parser::make_NEXT(VaParser::location());
var         return VaParser::Parser::make_VAR(VaParser::location());
cat         return VaParser::Parser::make_CAT(VaParser::location());
call        return VaParser::Parser::make_CALL(VaParser::location());
with        return VaParser::Parser::make_WITH(VaParser::location());

enabled      return VaParser::Parser::make_ENABLED(VaParser::location());
disabled      return VaParser::Parser::make_DISABLED(VaParser::location());
default      return VaParser::Parser::make_DEFAULT(VaParser::location());
when      return VaParser::Parser::make_WHEN(VaParser::location());
tcp\.redirect return VaParser::Parser::make_TCP_REDIRECT(VaParser::location());
http\.redirect return VaParser::Parser::make_HTTP_REDIRECT(VaParser::location());
bandwidth return VaParser::Parser::make_BANDWIDTH(VaParser::location());
answer return VaParser::Parser::make_ANSWER(VaParser::location());
time         return VaParser::Parser::make_TIME(VaParser::location());
trace         return VaParser::Parser::make_TRACE(VaParser::location());

pattern return VaParser::Parser::make_PATTERN(VaParser::location());

define      return VaParser::Parser::make_DEFINE(VaParser::location());
mysql       return VaParser::Parser::make_MYSQL(VaParser::location());

true   return VaParser::Parser::make_TRUE(VaParser::location());
false   return VaParser::Parser::make_FALSE(VaParser::location());

vr         return VaParser::Parser::make_VR(VaParser::location());
key         return VaParser::Parser::make_KEY(VaParser::location());
freq        return VaParser::Parser::make_FREQ(VaParser::location());
count        return VaParser::Parser::make_COUNT(VaParser::location());
action         return VaParser::Parser::make_ACTION(VaParser::location());
is         return VaParser::Parser::make_IS(VaParser::location());
condition         return VaParser::Parser::make_CONDITION(VaParser::location());
sequence         return VaParser::Parser::make_SEQUENCE(VaParser::location());
of         return VaParser::Parser::make_OF(VaParser::location());
in         return VaParser::Parser::make_IN(VaParser::location());
always         return VaParser::Parser::make_ALWAYS(VaParser::location());
add       return VaParser::Parser::make_ADD(VaParser::location());
del       return VaParser::Parser::make_DEL(VaParser::location());
to       return VaParser::Parser::make_TO(VaParser::location());
from       return VaParser::Parser::make_FROM(VaParser::location());
http\.host return VaParser::Parser::make_HTTP_HOST(VaParser::location());
http\.url\.host return VaParser::Parser::make_HTTP_URL_HOST(VaParser::location());
http\.header\.host return VaParser::Parser::make_HTTP_HEADER_HOST(VaParser::location());

NET\.SEND      return VaParser::Parser::make_NET_TCP_SEND(VaParser::location());
NET\.RECV      return VaParser::Parser::make_NET_TCP_RECV(VaParser::location());
NET\.ACCEPT    return VaParser::Parser::make_NET_TCP_ACCEPT(VaParser::location());
NET\.CONNECT   return VaParser::Parser::make_NET_TCP_CONNECT(VaParser::location());
NET\.CLOSE     return VaParser::Parser::make_NET_TCP_CLOSE(VaParser::location());
NET\.IPSRC     return VaParser::Parser::make_NET_TCP_IPSRC(VaParser::location());
NET\.IPDST     return VaParser::Parser::make_NET_TCP_IPDST(VaParser::location());
NET\.SESID     return VaParser::Parser::make_NET_TCP_SESID(VaParser::location());
NET\.RESULT     return VaParser::Parser::make_NET_RESULT(VaParser::location());

obs\.event return VaParser::Parser::make_OBS_EVENT(VaParser::location());
obs\.name   return VaParser::Parser::make_OBS_NAME(VaParser::location());
obs\.time   return VaParser::Parser::make_OBS_TIME(VaParser::location());
obs\.obsid   return VaParser::Parser::make_OBS_OBSID(VaParser::location());
obs\.stime   return VaParser::Parser::make_OBS_STIME(VaParser::location());
obs\.etime   return VaParser::Parser::make_OBS_ETIME(VaParser::location());
obs\.tag   return VaParser::Parser::make_OBS_TAG(VaParser::location());

http\.url return VaParser::Parser::make_HTTP_URL(VaParser::location());
http\.uri return VaParser::Parser::make_HTTP_URI(VaParser::location());

manage  return VaParser::Parser::make_SCM_MANAGE(VaParser::location());
give    return VaParser::Parser::make_SCM_GIVE(VaParser::location());

ACM_LIGHT_[0-9]+ return VaParser::Parser::make_OBS_EVENT_ACM_LIGHT(yytext, VaParser::location());

HTTP\.METHOD	return VaParser::Parser::make_HTTP_METHOD(VaParser::location());
HTTP\.GET	return VaParser::Parser::make_HTTP_METHOD_GET(yytext, VaParser::location());
HTTP\.POST	return VaParser::Parser::make_HTTP_METHOD_POST(yytext, VaParser::location());
HTTP\.HEAD	return VaParser::Parser::make_HTTP_METHOD_HEAD(yytext, VaParser::location());
HTTP\.PUT	return VaParser::Parser::make_HTTP_METHOD_PUT(yytext, VaParser::location());
HTTP\.DELETE	return VaParser::Parser::make_HTTP_METHOD_DELETE(yytext, VaParser::location());
HTTP\.TRACE	return VaParser::Parser::make_HTTP_METHOD_TRACE(yytext, VaParser::location());
HTTP\.OPTIONS	return VaParser::Parser::make_HTTP_METHOD_OPTIONS(yytext, VaParser::location());
HTTP\.CONNECT	return VaParser::Parser::make_HTTP_METHOD_CONNECT(yytext, VaParser::location());
HTTP\.PATCH	return VaParser::Parser::make_HTTP_METHOD_PATCH(yytext, VaParser::location());

HTTP\.ANSWER\.HEADER\.REPLACE  return VaParser::Parser::make_HTTP_ANSWER_HEADER_REPLACE(VaParser::location());
HTTP\.HEADER\.REPLACE return VaParser::Parser::make_HTTP_HEADER_REPLACE(VaParser::location());

HTTP\.COOKIE	 return VaParser::Parser::make_HTTP_COOKIE(VaParser::location());
HTTP\.HEADER	 return VaParser::Parser::make_HTTP_HEADER(VaParser::location());
HTTP\.PORT	 return VaParser::Parser::make_HTTP_PORT(VaParser::location());
HTTP\.QUERY	 return VaParser::Parser::make_HTTP_QUERY(VaParser::location());
HTTP\.DATA	 return VaParser::Parser::make_HTTP_DATA(VaParser::location());

HTTP\.ANSWER		 return VaParser::Parser::make_HTTP_ANSWER(VaParser::location());
HTTP\.ANSWER\.DATA		 return VaParser::Parser::make_HTTP_ANSWER_DATA(VaParser::location());
HTTP\.ANSWER\.CODE	 return VaParser::Parser::make_HTTP_ANSWER_CODE(VaParser::location());
HTTP\.ANSWER\.HEADER	 return VaParser::Parser::make_HTTP_ANSWER_HEADER(VaParser::location());
HTTP\.ANSWER\.COOKIE	 return VaParser::Parser::make_HTTP_ANSWER_COOKIE(VaParser::location());

SA\.id   return VaParser::Parser::make_AGENT_ID(VaParser::location());
SA\.OS   return VaParser::Parser::make_SA_OS(VaParser::location());
SA\.USER         return VaParser::Parser::make_SA_LOGGED_USER(VaParser::location());
SA\.PROCESS	 return VaParser::Parser::make_SA_PROCESS_NAME(VaParser::location());
SA\.PROCESS\.ATTACH      return VaParser::Parser::make_SA_PROCESS_ATTACH(VaParser::location());

REPORT        return VaParser::Parser::make_REPORT(VaParser::location());
MREPORT        return VaParser::Parser::make_MREPORT(VaParser::location());
EXISTS        return VaParser::Parser::make_EXISTS(VaParser::location());
DATA        return VaParser::Parser::make_DATA(VaParser::location());
MAKE        return VaParser::Parser::make_MAKE(VaParser::location());
AR return VaParser::Parser::make_AR(VaParser::location());
mysql\.append       return VaParser::Parser::make_MYSQL_APPEND(VaParser::location());
mysql\.update       return VaParser::Parser::make_MYSQL_UPDATE(VaParser::location());
ENABLE       return VaParser::Parser::make_ENABLE(VaParser::location());
DISABLE       return VaParser::Parser::make_DISABLE(VaParser::location());
KV return VaParser::Parser::make_KV(VaParser::location());

[a-zA-Z_][a-zA-Z0-9_+.-]*  return VaParser::Parser::make_IDENT(yytext, VaParser::location());

(\/([a-zA-Z0-9_.-~-]*)|\*)+ { return VaParser::Parser::make_URL_WITH_STAR(yytext, VaParser::location());}

[0-9]{2}\.[0-9]{2}:[0-9]{2}\.[0-9]{2}  return VaParser::Parser::make_TIMING(yytext, VaParser::location());

[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}(:[0-9]+)?  return VaParser::Parser::make_IP_WITHOUT_STAR(yytext, VaParser::location());

([0-9]{1,3}|\*)\.([0-9]{1,3}|\*)\.([0-9]{1,3}|\*)\.([0-9]{1,3}|\*)(:([0-9]+|\*))?  return VaParser::Parser::make_IP_WITH_STAR(yytext, VaParser::location());

{INTEGER}   return VaParser::Parser::make_I_NUMBER(yytext, VaParser::location());

\(          return VaParser::Parser::make_LEFTPAR(VaParser::location());
\)          return VaParser::Parser::make_RIGHTPAR(VaParser::location());
\[          return VaParser::Parser::make_LEFTSQPAR(VaParser::location());
\]          return VaParser::Parser::make_RIGHTSQPAR(VaParser::location());
\{          return VaParser::Parser::make_LEFTGRPAR(VaParser::location());
\}          return VaParser::Parser::make_RIGHTGRPAR(VaParser::location());
;           return VaParser::Parser::make_SEMICOLON(VaParser::location());
,           return VaParser::Parser::make_COMMA(VaParser::location());
=           return VaParser::Parser::make_EQUAL(VaParser::location());
\>          return VaParser::Parser::make_GT(VaParser::location());
\<          return VaParser::Parser::make_LT(VaParser::location());
\>=         return VaParser::Parser::make_GTE(VaParser::location());
\<=         return VaParser::Parser::make_LTE(VaParser::location());
\+          return VaParser::Parser::make_PLUS(VaParser::location());
\-          return VaParser::Parser::make_MINUS(VaParser::location());
\.          return VaParser::Parser::make_DOT(VaParser::location());
\$          return VaParser::Parser::make_DOLLAR(VaParser::location());
\@          return VaParser::Parser::make_AT(VaParser::location());

\n          ++m_driver.prs_lineno;

{WS}        ;

.           cout << "Scanner: unknown character [" << yytext << "]" << endl;
            
<<EOF>>     { return yyterminate(); }

%%

/*
 \$[a-zA-Z_][a-zA-Z0-9_.\[\]-]* return VaParser::Parser::make_VARIABLE(yytext, VaParser::location());
HTTP\.DATA\.FULL return VaParser::Parser::make_HTTP_DATA_FULL(VaParser::location());
HTTP\.DATA\.SIGN  return VaParser::Parser::make_HTTP_DATA_CRC(VaParser::location());
HTTP\.answer\.DATA\.SUGN  return VaParser::Parser::make_HTTP_ANSWER_DATA_CRC(VaParser::location());
HTTP\.answer\.DATA\.FULL return VaParser::Parser::make_HTTP_ANSWER_DATA_FULL(VaParser::location());
HTTP\.ANSWER\.DATA	 return VaParser::Parser::make_HTTP_ANSWER_DATA(VaParser::location());
vrep         return VaParser::Parser::make_VREP(VaParser::location());
expiry      return VaParser::Parser::make_EXPIRY(VaParser::location());
if      return VaParser::Parser::make_IF(VaParser::location());
then      return VaParser::Parser::make_THEN(VaParser::location());

 */
