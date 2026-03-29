%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include "parserds.h"
#include "parser.tab.hpp"
#include "parserds.h"

using namespace std;

/* put your variables here */

int prs_lineno = 1;

string stringa_tipo_c;

%}


%option 8bit 
%option nomain noyywrap 
%option warn
/*%option debug*/

WS      [ \t]+
DIG     [0-9]
FLOAT   {DIG}*\.{DIG}+
INTEGER {DIG}+


%x str

%%


\"		  {
          stringa_tipo_c.clear();
          BEGIN(str);
        }

<str>\"	{
					BEGIN(INITIAL);
          //printf("Letta stringa tipo c: <%s>\n", stringa_tipo_c.c_str());
					return TK_C_STRING;
        }

<str>\n {
          /* error - unterminated string constant */
          /* generate error message */
				  prs_lineno++;
				  printf("Errore! unterminated string constant at line %d\n", prs_lineno);
				  exit(0);
        }

<str>\\[0-7]{1,3} {
					 /* octal escape sequence */
					 int result;

					 (void) sscanf( yytext + 1, "%o", &result );

					 if ( result > 0xff ) {
							 /* error, constant is out-of-bounds */
						printf("La costante %s supera il valore 255\n", yytext);
					 }

					 stringa_tipo_c += string(result, 1);
          }

<str>\\0x[0-9a-fA-F]{1,2} {
					 /* Hex escape sequence */
					 int result;

					 (void) sscanf( yytext + 1, "%x", &result );

					 if ( result > 0xff ) {
							 /* error, constant is out-of-bounds */
						printf("La costante %s supera il valore 255\n", yytext);
					 }

					 stringa_tipo_c += string(result, 1);
                  }

<str>\\0d[0-9]{1,3} {
					 /* Decimal escape sequence */
					 int result;

					 (void) sscanf( yytext + 3, "%x", &result );

					 if ( result > 0xff ) {
							 /* error, constant is out-of-bounds */
						printf("La costante %s supera il valore 255\n", yytext);
					 }

					 stringa_tipo_c += string(result, 1);
                  }

<str>\\[0-9]+ {
                 /* generate error - bad escape sequence; something
                  * like '\48' or '\0777777'
                  */
				  printf("Errore! bad escape sequence at line %d\n", prs_lineno);
				  exit(0);
              }

<str>\\n  { stringa_tipo_c +="\n";prs_lineno++; }
<str>\\r  { stringa_tipo_c +="\r";prs_lineno++; }
<str>\\t  { stringa_tipo_c += "\t"; }
<str>\\b  { stringa_tipo_c += "\b"; }
<str>\\f  { stringa_tipo_c += "\f"; }

<str>\\.  { stringa_tipo_c += string(yytext[1], 1); }
<str>\\\n { stringa_tipo_c += string(yytext[1], 1); prs_lineno++; }

<str>[^\\\n\"]+        {
              stringa_tipo_c += yytext;
             /*
						 char *yptr = yytext;
						 while ( *yptr ) {
								 *stringa_tipo_c_ptr++ = *yptr++;
								  stringa_tipo_c_len++;
              */
						 }

"/*"	{	/*Elimina i commenti standard!!*/
			int c;

			while((c = yyinput()) != 0) {
				if(c == '\n')
					++prs_lineno;
				else if(c == '*') {
					if ((c = yyinput()) == '/')
						break;
					else
						unput(c);
				}
			}
		}


\/\/.*\n	++prs_lineno; /*Elimina i commenti non standard / / */

^\#.*\n	++prs_lineno; /*Elimina i commenti non standard / / */

[rR][uU][lL][eE]          return TK_RULE;
[iI][pP]                  return TK_IP;
[hH][oO][sS][tT]          return TK_HOST;
[uU][rR][lL]              return TK_URL;
[aA][cC][tT][iI][oO][nN]  return TK_ACTION;
[cC][hH][aA][nN][gG][eE]\_[cC][sS]      return TK_CHANGE_CS;
[cC][hH][aA][nN][gG][eE]\_[sS][cC]      return TK_CHANGE_SC;
[hH][eE][aA][dD][eE][rR]  return TK_HEADER;
[cC][oO][oO][kK][iI][eE]  return TK_COOKIE;
[bB][oO][dD][yY]          return TK_BODY;
[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3} return TK_IP_NUMBER;
[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\:[0-9]+ return TK_IP_PORT_NUMBER;
{INTEGER}                 return TK_I_NUMBER;
{FLOAT}                   return TK_F_NUMBER;
[Mm][aA][nN][aA][gG][eE]  return TK_MANAGE;
[rR][eE][dD][iI][rR][eE][cC][tT]  return TK_REDIRECT;
[aA][cC][cC][eE][pP][tT]  return TK_ACCEPT;
[gG][iI][vV][eE]          return TK_GIVE;
[bB][lL][oO][cC][kK]      return TK_BLOCK;
[qQ][uU][eE][rR][yY]      return TK_QUERY;

[a-zA-Z_.~/*+-][a-zA-Z0-9_.~/*+-]*   return TK_STRING;

{WS}    /* skip blanks and tabs */ ;

\n	++prs_lineno; /*Elimina i commenti non standard / / */

. return yytext[0];

%%

