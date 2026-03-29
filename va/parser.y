%{
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
	
	%}

/*%define parse.trace*/

%union {
	std::string * stdstring;
	T_bar_rule  * trule;
	T_cmd_and_pars  * tcmd;
	std::list<string*> *slist;
	tuple<string*, string*, string*> * s3tuple;
	list<tuple<string*, string*, string*>*> * ls3tuple;
}

%token TK_RULE
%token <stdstring> TK_IP
%token <stdstring> TK_HOST
%token <stdstring> TK_URL
%token TK_ACTION
%token TK_CHANGE_CS
%token TK_CHANGE_SC
%token TK_HEADER
%token TK_COOKIE
%token TK_BODY
%token TK_I_NUMBER
%token TK_F_NUMBER
%token <stdstring> TK_STRING
%token <stdstring> TK_C_STRING
%token TK_MANAGE
%token TK_REDIRECT
%token TK_ACCEPT
%token TK_GIVE
%token TK_BLOCK
%token TK_QUERY
%token <stdstring> TK_IP_NUMBER
%token <stdstring> TK_IP_PORT_NUMBER


%type <stdstring> ip
%type <stdstring> host
%type <stdstring> url
%type <stdstring> gstring
%type <stdstring> string_or_ip
%type <stdstring> string_or_empty
%type <slist> redirect
%type <stdstring> mg_parameter
%type <slist> mg_parameters
%type <slist> manage
%type <tcmd> specific_action
%type <tcmd> action
%type <s3tuple> replace_item
%type <ls3tuple> replace_list
%type <ls3tuple> change_cs
%type <ls3tuple> change_sc
%type <s3tuple> cookie_or_header_spec
%type <ls3tuple> cookie_or_header_list
%type <trule> rule
%%

start: rules

rules:
rule {parsed_rules.push_back($1);}
|
rule rules {parsed_rules.push_front($1);}
;

rule:
TK_RULE '{' action ';' ip ';' host ';' url ';' cookie_or_header_list '}'
{
	auto p = new T_bar_rule();
	
	//action
	//printf("$3=%X, What: %d, Parm: %X\n", $3, $3->what, $3->parameters);
	p->action = $3->what;
	for (auto it: *($3->parameters))
	{
		p->action_parameters.push_back(*it);
		delete it;
	}
	delete $3->parameters;
	delete $3;
	
	p->ip = *($5); delete $5;
	p->host = *($7); delete $7;
	p->url = *($9); delete $9;
	
	//list<tuple<string*, string*, string*>*> * ls3tuple;
	for (auto it: *($11))
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
	delete $11;
	
	$$=p;
}
|
TK_RULE '{' action ';' change_cs ';' change_sc ';' ip ';' host ';' url ';' cookie_or_header_list '}'
{
	auto p = new T_bar_rule();
	
	//action
	p->action = $3->what;
	for (auto it: *($3->parameters))
	{
		p->action_parameters.push_back(*it);
		delete it;
	}
	delete $3->parameters;
	delete $3;
	
	p->ip = *($9); delete $9;
	p->host = *($11); delete $11;
	p->url = *($13); delete $13;
	
	//list<tuple<string*, string*, string*>*> * ls3tuple;
	for (auto it: *($15))
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
	delete $15;
	
	p->is_change = false;
	if ($5 == nullptr)
	{
		p->change_parameters_cs.clear();
	}
	else
	{
		p->is_change = true;
		for (auto it: *($5))
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
		delete $5;
	}
	if ($7 == nullptr)
	{
		p->change_parameters_sc.clear();
	}
	else
	{
		p->is_change = true;
		for (auto it: *($7))
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
		delete $7;
	}
	$$=p;
}
;

ip  :
TK_IP ':' string_or_ip {$$=$3;}
|
TK_IP ':' {$$=new string();}
;

host:
TK_HOST ':' string_or_ip {$$=$3;}
|
TK_HOST ':' {$$=new string();}
;

url :
TK_URL ':' gstring {$$=$3;}
|
TK_URL ':' {$$=new string();}
;

cookie_or_header_list:
cookie_or_header_spec cookie_or_header_list
{ $2->push_front($1); $$=$2; }
|
/*%empty*/
{ auto ls3 = new list<tuple<string*, string*, string*>*>(); $$=ls3; }
;

cookie_or_header_spec:
TK_COOKIE ':' gstring '=' string_or_empty ';'
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("COOKIE"), $3, $5);
	$$=s3;
}
|
TK_HEADER ':' gstring '=' string_or_empty ';'
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("HEADER"), $3, $5);
	$$=s3;
}
|
TK_QUERY ':' gstring '=' string_or_empty ';'
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("QUERY"), $3, $5);
	$$=s3;
}
;

action:
TK_ACTION ':' specific_action {$$=$3;}
;

change_cs:
TK_CHANGE_CS ':' replace_list
{ $$ = $3; }
;

change_sc:
TK_CHANGE_SC ':' replace_list
{ $$ = $3; }
;

replace_list:
/*%empty*/
{
	auto ls3 = new list<tuple<string*, string*, string*>*>();
	//ls3->push_front($1);
	$$=ls3;
}
|
replace_item  replace_list
{
	$2->push_front($1);
	$$ = $2;
}
;

replace_item:
TK_HEADER '=' gstring ',' gstring
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("HEADER"), $3, $5);
	$$=s3;
}
|
TK_COOKIE '=' gstring ',' gstring
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("COOKIE"), $3, $5);
	$$=s3;
}
|
TK_BODY '=' gstring ',' gstring
{
	auto s3 = new tuple<string*, string*, string*>();
	*s3 = make_tuple(new string("BODY"), $3, $5);
	$$=s3;
}
;

specific_action:
TK_MANAGE '(' manage ')'
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_manage;
	cmd->parameters = $3;
	$$=cmd;
}
|
TK_REDIRECT '(' redirect ')'
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_redirect;
	cmd->parameters = $3;
	$$=cmd;
}
|
TK_ACCEPT
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_accept;
	cmd->parameters = new list<string*>();
	$$=cmd;
}
|
TK_GIVE '(' gstring ')'
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_give;
	cmd->parameters = new list<string*>();
        cmd->parameters->push_back($3);
	$$=cmd;
}
|
TK_BLOCK
{
	auto cmd = new T_cmd_and_pars();
	cmd->what = e_block;
	cmd->parameters = new list<string*>();
	$$=cmd;
}
;

manage: gstring mg_parameters //gstring e' il comando
{ $2->push_front($1); $$=$2;}
;

mg_parameters:
/*%empty*/  {auto ls = new list<string*>(); $$=ls;}
|
mg_parameter mg_parameters
{
	//cerr << "Parametro: " << *($1) << endl;
	$2->push_front($1); $$=$2;
}
;

mg_parameter:
gstring {$$=$1;}
|
TK_I_NUMBER {$$=yytext2string();}
|
TK_F_NUMBER {$$=yytext2string();}
;

redirect:
TK_HOST '=' string_or_ip ';' TK_URL '=' string_or_empty ';' TK_QUERY '=' string_or_empty ';'
{
	auto ls = new list<string*>();
	ls->push_back($3); ls->push_back($7); ls->push_back($11);
	$$=ls;
}
;

string_or_empty:
gstring {$$=$1;}
|
/*%empty*/ {string * s = new string(); $$ = s ; }
;

string_or_ip:
gstring {$$=$1;}
|
TK_IP_NUMBER {$$=yytext2string();}
|
TK_IP_PORT_NUMBER {$$=yytext2string();}
;


gstring:
TK_STRING {$$=yytext2string();}
|
TK_C_STRING
{
	extern string stringa_tipo_c;
	//cerr << "LEtta: <" << stringa_tipo_c << ">" << endl;
	$$=new string(stringa_tipo_c);
}
;


%%

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

