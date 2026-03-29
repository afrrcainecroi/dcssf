#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <thread>

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>


//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"

#include "mtfafs.h"
#include "mtfa_fs3.h"
#include "mtfa_utils.h"
#include "mtfa_db.h"

#include "VA_ast_parsing.h"
#include "VAengine.h"
#include "base64.h"

string tabs ( int i )
{
    return string ( i, '\t' );
}

using namespace VaParser;
using namespace std;

//SimpleRng * rng;
static ULL_Rng* rng;

void Stampa ( int l, const Command cmd )
{
    if ( cmd.m_args_c.size() > 2 )
    {
        cout << __func__   << tabs ( l ) << totok ( cmd.m_id ) << endl;
        if ( Parser::token::TOKEN_VR_DEF_IS_LISTA_WHEN != cmd.m_id )
        {
            cout << __func__   << __func__ << ": "  << "Errore" << endl;
        }
    }
    else
    {
        auto it = cmd.m_args_c.begin();
        if ( cmd.m_args_c.size() == 0 )
        {
            cout << __func__   << tabs ( l ) << totok ( cmd.m_id ) << endl;
        }
        else if ( cmd.m_args_c.size() == 1 )
        {
            cout << __func__   << tabs ( l ) << totok ( cmd.m_id ) << endl;
            Stampa ( l + 1, *it );
        }
        else
        {
            Stampa ( l + 1, *it );
            cout << __func__   << tabs ( l ) << totok ( cmd.m_id ) << endl;
            ++it;
            Stampa ( l + 1, *it );
        }
    }
}

VA_ItemTypes VRCvt ( const int t )
{
    switch ( t )
    {
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS:
            return VA_ItemTypes::OBS_NAME;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN:
            return VA_ItemTypes::OBS_NAME;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS:
            return VA_ItemTypes::OBS_EVENT;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN:
            return VA_ItemTypes::OBS_EVENT;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS:
            return VA_ItemTypes::NET_TCP_IPDST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN:
            return VA_ItemTypes::NET_TCP_IPDST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS:
            return VA_ItemTypes::HTTP_METHOD;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN:
            return VA_ItemTypes::HTTP_METHOD;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS:
            return VA_ItemTypes::HTTP_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN:
            return VA_ItemTypes::HTTP_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN:
            return VA_ItemTypes::HTTP_URL;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS:
            return VA_ItemTypes::HTTP_URL;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN:
            return VA_ItemTypes::HTTP_URI;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS:
            return VA_ItemTypes::HTTP_URI;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN:
            return VA_ItemTypes::AGENT_ID;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS:
            return VA_ItemTypes::AGENT_ID;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS:
            return VA_ItemTypes::HTTP_HEADER_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN:
            return VA_ItemTypes::HTTP_HEADER_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS:
            return VA_ItemTypes::HTTP_URL_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN:
            return VA_ItemTypes::HTTP_URL_HOST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS:
            return VA_ItemTypes::HTTP_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS:
            return VA_ItemTypes::HTTP_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN:
            return VA_ItemTypes::HTTP_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS:
            return VA_ItemTypes::HTTP_HEADERS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS:
            return VA_ItemTypes::HTTP_HEADERS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN:
            return VA_ItemTypes::HTTP_HEADERS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS:
            return VA_ItemTypes::HTTP_QUERY;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS:
            return VA_ItemTypes::HTTP_QUERY;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN:
            return VA_ItemTypes::HTTP_QUERY;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS:
            return VA_ItemTypes::HTTP_DATA_POST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS:
            return VA_ItemTypes::HTTP_DATA_POST;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN:
            return VA_ItemTypes::HTTP_DATA_POST;
            break;

        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS:
            return VA_ItemTypes::HTTP_ANSWER_DATA;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS:
            return VA_ItemTypes::HTTP_ANSWER_DATA;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN:
            return VA_ItemTypes::HTTP_ANSWER_DATA;
            break;

        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS:
            return VA_ItemTypes::OBS_TAG;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS:
            return VA_ItemTypes::OBS_TAG;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN:
            return VA_ItemTypes::OBS_TAG;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS:
            return VA_ItemTypes::SA_OS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN:
            return VA_ItemTypes::SA_OS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS:
            return VA_ItemTypes::SA_PROCESS_NAME;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN:
            return VA_ItemTypes::SA_PROCESS_NAME;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS:
            return VA_ItemTypes::SA_LOGGED_USER;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN:
            return VA_ItemTypes::SA_LOGGED_USER;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS:
            return VA_ItemTypes::NET_TCP_IPSRC;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN:
            return VA_ItemTypes::NET_TCP_IPSRC;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS:
            return VA_ItemTypes::NET_RESULT;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS:
            return VA_ItemTypes::NET_SESID;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN:
            return VA_ItemTypes::NET_SESID;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS:
            return VA_ItemTypes::HTTP_ANSWER_CODE;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN:
            return VA_ItemTypes::HTTP_ANSWER_CODE;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS:
            return VA_ItemTypes::HTTP_ANSWER_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN:
            return VA_ItemTypes::HTTP_ANSWER_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS:
            return VA_ItemTypes::HTTP_ANSWER_COOKIES;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS:
            return VA_ItemTypes::HTTP_ANSWER_HEADERS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN:
            return VA_ItemTypes::HTTP_ANSWER_HEADERS;
            break;
        case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS:
            return VA_ItemTypes::HTTP_ANSWER_HEADERS;
            break;
        case Parser::token::token::TOKEN_IDENT:
            return VA_ItemTypes::VARIABLE;
        case Parser::token::token::TOKEN_STRING:
            return VA_ItemTypes::VARIABLE;
            break;
        case Parser::token::TOKEN_TIME:
            return VA_ItemTypes::AR_TIME;
            break;


        //E ora gli elementi diretti che sono osservati
        case Parser::token::TOKEN_AGENT_ID:
            return VA_ItemTypes::AGENT_ID;
            break;
        case Parser::token::TOKEN_SA_OS:
            return VA_ItemTypes::SA_OS;
            break;
        case Parser::token::TOKEN_SA_LOGGED_USER:
            return VA_ItemTypes::SA_LOGGED_USER;
            break;
        case Parser::token::TOKEN_SA_PROCESS_NAME:
            return VA_ItemTypes::SA_PROCESS_NAME;
            break;
        case Parser::token::TOKEN_OBS_NAME:
            return VA_ItemTypes::OBS_NAME;
            break;
        case Parser::token::TOKEN_OBS_TIME:
            return VA_ItemTypes::OBS_TIME;
            break;
        case Parser::token::TOKEN_OBS_OBSID:
            return VA_ItemTypes::OBS_OBSID;
            break;
        case Parser::token::TOKEN_OBS_STIME:
            return VA_ItemTypes::OBS_STIME;
            break;
        case Parser::token::TOKEN_OBS_ETIME:
            return VA_ItemTypes::OBS_ETIME;
            break;
        case Parser::token::TOKEN_OBS_TAG:
            return VA_ItemTypes::OBS_TAG;
            break;
        case Parser::token::TOKEN_OBS_EVENT:
            return VA_ItemTypes::OBS_EVENT;
            break;
        case Parser::token::TOKEN_NET_TCP_IPDST:
            return VA_ItemTypes::NET_TCP_IPDST;
            break;
        case Parser::token::TOKEN_NET_TCP_IPSRC:
            return VA_ItemTypes::NET_TCP_IPSRC;
            break;
        case Parser::token::TOKEN_NET_RESULT:
            return VA_ItemTypes::NET_RESULT;
            break;
        case Parser::token::TOKEN_NET_TCP_SESID:
            return VA_ItemTypes::NET_SESID;
            break;
        case Parser::token::TOKEN_HTTP_METHOD:
            return VA_ItemTypes::HTTP_METHOD;
            break;
        case Parser::token::TOKEN_HTTP_HOST:
            return VA_ItemTypes::HTTP_HOST;
            break;
        case Parser::token::TOKEN_HTTP_URL:
            return VA_ItemTypes::HTTP_URL;
            break;
        case Parser::token::TOKEN_HTTP_URI:
            return VA_ItemTypes::HTTP_URI;
            break;
        case Parser::token::TOKEN_HTTP_HEADER_HOST:
            return VA_ItemTypes::HTTP_HEADER_HOST;
            break;
        case Parser::token::TOKEN_HTTP_URL_HOST:
            return VA_ItemTypes::HTTP_URL_HOST;
            break;
        case Parser::token::TOKEN_HTTP_COOKIE:
            return VA_ItemTypes::HTTP_COOKIES;
            break;
        case Parser::token::TOKEN_HTTP_HEADER:
            return VA_ItemTypes::HTTP_HEADERS;
            break;
        case Parser::token::TOKEN_HTTP_QUERY:
            return VA_ItemTypes::HTTP_QUERY;
            break;
        case Parser::token::TOKEN_HTTP_DATA:
            return VA_ItemTypes::HTTP_DATA_POST;
            break;
        case Parser::token::TOKEN_HTTP_ANSWER_CODE:
            return VA_ItemTypes::HTTP_ANSWER_CODE;
            break;
        case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
            return VA_ItemTypes::HTTP_ANSWER_COOKIES;
            break;
        case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
            return VA_ItemTypes::HTTP_ANSWER_HEADERS;
            break;
        case Parser::token::TOKEN_HTTP_ANSWER_DATA:
            return VA_ItemTypes::HTTP_ANSWER_DATA;
            break;
        case Parser::token::TOKEN_CAT:
            return VA_ItemTypes::CAT;
            break;
        case Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION:
            return VA_ItemTypes::CALL_LISP_FUNCTION;
            break;

//        //da utilizzare con cautela. Per ora aggiungo una stampa
//        case Parser::token::TOKEN_IDENT: cout << __func__   << __func__ << ": TOKEN_IDENT\n"; return VA_ItemTypes::VARIABLE; break;
//        case Parser::token::TOKEN_STRING:  cout << __func__   << __func__ << ": TOKEN_STRING\n"; return VA_ItemTypes::VARIABLE; break;

        default:
            cout << __func__   << __func__ << ": " << "Token not found: " << t << endl;
            return VA_ItemTypes::NONE;
    }
}

std::string VA_ItemTypesToString ( VA_ItemTypes t )
{
    switch ( t )
    {
        case VA_ItemTypes::AGENT_ID:
            return "VA_ItemTypes::AGENT_ID";
        case VA_ItemTypes::AR_TIME:
            return "VA_ItemTypes::AR_TIME";
        case VA_ItemTypes::HTTP_ANSWER_CODE:
            return "VA_ItemTypes::HTTP_ANSWER_CODE";
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            return "VA_ItemTypes::HTTP_ANSWER_COOKIES";
        case VA_ItemTypes::HTTP_ANSWER_DATA:
            return "VA_ItemTypes::HTTP_ANSWER_DATA";
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
            return "VA_ItemTypes::HTTP_ANSWER_HEADERS";
        case VA_ItemTypes::HTTP_COOKIES:
            return "VA_ItemTypes::HTTP_COOKIES";
        case VA_ItemTypes::HTTP_DATA_POST:
            return "VA_ItemTypes::HTTP_DATA_POST";
        case VA_ItemTypes::HTTP_HEADERS:
            return "VA_ItemTypes::HTTP_HEADERS";
        case VA_ItemTypes::HTTP_HEADER_HOST:
            return "VA_ItemTypes::HTTP_HEADER_HOST";
        case VA_ItemTypes::HTTP_HOST:
            return "VA_ItemTypes::HTTP_HOST";
        case VA_ItemTypes::HTTP_METHOD:
            return "VA_ItemTypes::HTTP_METHOD";
        case VA_ItemTypes::HTTP_QUERY:
            return "VA_ItemTypes::HTTP_QUERY";
        case VA_ItemTypes::HTTP_URL:
            return "VA_ItemTypes::HTTP_URL";
        case VA_ItemTypes::HTTP_URI:
            return "VA_ItemTypes::HTTP_URI";
        case VA_ItemTypes::HTTP_URL_HOST:
            return "VA_ItemTypes::HTTP_URL_HOST";
        case VA_ItemTypes::NET_RESULT:
            return "VA_ItemTypes::NET_RESULT";
        case VA_ItemTypes::NET_SESID:
            return "VA_ItemTypes::NET_SESID";
        case VA_ItemTypes::NET_TCP_IPDST:
            return "VA_ItemTypes::NET_TCP_IPDST";
        case VA_ItemTypes::NET_TCP_IPSRC:
            return "VA_ItemTypes::NET_TCP_IPSRC";
        case VA_ItemTypes::NONE:
            return "VA_ItemTypes::NONE";
        case VA_ItemTypes::OBS_ETIME:
            return "VA_ItemTypes::OBS_ETIME";
        case VA_ItemTypes::OBS_EVENT:
            return "VA_ItemTypes::OBS_EVENT";
        case VA_ItemTypes::OBS_NAME:
            return "VA_ItemTypes::OBS_NAME";
        case VA_ItemTypes::OBS_OBSID:
            return "VA_ItemTypes::OBS_OBSID";
        case VA_ItemTypes::OBS_STIME:
            return "VA_ItemTypes::OBS_STIME";
        case VA_ItemTypes::OBS_TAG:
            return "VA_ItemTypes::OBS_TAG";
        case VA_ItemTypes::OBS_TIME:
            return "VA_ItemTypes::OBS_TIME";
        case VA_ItemTypes::SA_LOGGED_USER:
            return "VA_ItemTypes::SA_LOGGED_USER";
        case VA_ItemTypes::SA_OS:
            return "VA_ItemTypes::SA_OS";
        case VA_ItemTypes::SA_PROCESS_NAME:
            return "VA_ItemTypes::SA_PROCESS_NAME";
        case VA_ItemTypes::VARIABLE:
            return "VA_ItemTypes::VARIABLE";
        default:
            cout << __func__   << "SWITCH ERROR AT " << __func__ << endl;
            return "";
    }
}

list<string> GetFromDb ( const list<string> sql_pars )
{
    list<string> data_from_db;

    auto par = sql_pars.begin();
    T_V_Split vipp = mtfa_string_split ( *par, ":" );
    short port = 3306;
    if ( vipp.size() == 2 )
    {
        port = atoi ( vipp[1].c_str() );
    }

    ++par;
    string dbname = *par;
    ++par;
    string username = *par;
    ++par;
    string password = *par;
    ++par;
    string thequery = *par;
    mysql_db_class* db = new mysql_db_class ( vipp[0], username, password, dbname, port );
    bool bret = db->do_connect();
    if ( !bret )
    {
        cout << __func__ << "Unable to connect to the db: " << dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
        exit ( -1 );
    }

    list<list < string>> dbresult;
    bret = db->doMySqlQuery ( &dbresult, thequery );
    if ( !bret )
    {
        cout << __func__ << "Unable to execute query: " << thequery << endl << "on the db: " <<
             dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
        exit ( -1 );
    }
    if ( db->getCols() == 1 )
    {
        //Sono nella versione a 1 colonna
        for ( auto it : dbresult )
        {
            data_from_db.push_back ( it.front() );
        }
    }
    else
    {
        cout << __func__ << "The query: " << thequery << " returns " << db->getCols() << " columns instead than 1 column. Error." << endl;
    }
    db->do_disconnect();
    delete db;
    return data_from_db;
}

list<pair<string, string>> GetFromDbPair ( const list<string> sql_pars )
{
    list<pair<string, string>> data_from_db;
    
    auto par = sql_pars.begin();
    T_V_Split vipp = mtfa_string_split ( *par, ":" );
    short port = 3306;
    if ( vipp.size() == 2 )
    {
        port = atoi ( vipp[1].c_str() );
    }
    
    ++par;
    string dbname = *par;
    ++par;
    string username = *par;
    ++par;
    string password = *par;
    ++par;
    string thequery = *par;
    mysql_db_class* db = new mysql_db_class ( vipp[0], username, password, dbname, port );
    bool bret = db->do_connect();
    if ( !bret )
    {
        cout << __func__ << "Unable to connect to the db: " << dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
        exit ( -1 );
    }
    
    list<list < string>> dbresult;
    bret = db->doMySqlQuery ( &dbresult, thequery );
    if ( !bret )
    {
        cout << __func__ << "Unable to execute query: " << thequery << endl << "on the db: " <<
        dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
        exit ( -1 );
    }
    if ( db->getCols() == 2 )
    {
        //Sono nella versione a 1 colonna
        for ( auto it : dbresult )
        {
            auto jt = it.begin();
            auto primo = (*jt); ++jt;
            auto secondo = (*jt);
            data_from_db.push_back (make_pair(primo, secondo));
        }
    }
    else
    {
        cout << __func__ << "The query: " << thequery << " returns " << db->getCols() << " columns instead than 1 column. Error." << endl;
    }
    db->do_disconnect();
    delete db;
    return data_from_db;
}

void ParseSetOfIpStar ( VAengine& vaeng, const string& name, const list<string>& args_s )
{
    if ( !vaeng.vasets.AddSetofIpS ( name, args_s, list<string>() ) )
    {
        cout << "ParseSetOfIpStar: set " << name << "already defined." << endl;
        exit(-1);
    }
}
void ParseSetOfIpStar ( VAengine& vaeng, const string& name, const list<Command>& args_c )
{
    //prende i dati dal DB
    list<string> slist = GetFromDb ( args_c.front().m_args_s );
    if ( !vaeng.vasets.AddSetofIpS ( name, slist, args_c.front().m_args_s ) )
    {
        cout << "ParseSetOfIpStar: set " << name << "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfStringsPairs ( VAengine& vaeng, const string& name, const list<pair<string, string>>& args_spair  )
{
    if ( !vaeng.vasets.AddSetofStringPair ( name, args_spair, list<string>() ) )
    {
        cout <<  "ParseSetOfStrings: set " << name << "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfStringsPairs ( VAengine& vaeng, const string& name, const list<Command>& args_c )
{
    //prende i dati dal DB
    list<pair<string, string>> slist = GetFromDbPair ( args_c.front().m_args_s );
    if ( !vaeng.vasets.AddSetofStringPair ( name, slist, args_c.front().m_args_s ) )
    {
        cout <<  "ParseSetOfStrings: set " << name << "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfStrings ( VAengine& vaeng, const string& name, const list<string>& m_args)
{
    if ( !vaeng.vasets.AddSetofString ( name, m_args, list<string>() ) )
    {
        cout <<  "ParseSetOfStrings: set " << name << "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfStrings ( VAengine& vaeng, const string& name, const list<Command>& args_c )
{
    //prende i dati dal DB
    list<string> slist = GetFromDb ( args_c.front().m_args_s );
    if ( !vaeng.vasets.AddSetofString ( name, slist, args_c.front().m_args_s ) )
    {
        cout <<  "ParseSetOfStrings: set " << name << "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfTimings ( VAengine& vaeng, const string& name, const list<string>& args_s )
{
    if ( !vaeng.vasets.AddSetofTime ( name, args_s, list<string>() ) )
    {
        cout <<  "ParseSetofTimes: set " << name << "already defined." << endl;
        exit(-1);
    }
}
void ParseSetOfTimings ( VAengine& vaeng, const string& name, const list<Command>& args_c )
{
    //prende i dati dal DB
    list<string> slist = GetFromDb ( args_c.front().m_args_s );
    if ( !vaeng.vasets.AddSetofTime ( name, slist, args_c.front().m_args_s ) )
    {
        cout <<  "ParseSetofTimes: set " + name + "already defined." << endl;
        exit(-1);
    }
}

void ParseSetOfUrlStar ( VAengine& vaeng, const string& name, const list<string>& args_s )
{
    if ( !vaeng.vasets.AddSetofUrlS ( name, args_s, list<string>() ) )
    {
        cout <<  "ParseSetOfUrlS: set " + name + "already defined." << endl;
        exit(-1);
    }
}
void ParseSetOfUrlStar ( VAengine& vaeng, const string& name, const list<Command>& args_c )
{
    //prende i dati dal DB
    list<string> slist = GetFromDb ( args_c.front().m_args_s );
    if ( !vaeng.vasets.AddSetofUrlS ( name, slist, args_c.front().m_args_s ) )
    {
        cout <<  "ParseSetOfUrlS: set " + name + "already defined." << endl;
        exit(-1);
    }
}

list<VAcriteria> ParseTOKEN_VR_DEF_IS_LISTA_WHEN ( VAengine& vaeng, const Command cmd )
{
    //devo aggiungere un elenco di criteri alla VR corrente, nel posto
    //vaeng.varules[vaeng.varules.size()-1].criteria
    list<VAcriteria> criteria;
    cout << __func__  << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
    for ( auto it : cmd.m_args_c )
    {
        cout << __func__   << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
        switch ( it.m_id )
        {
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_CAT:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                cr.pattern=nullptr;
                cr.replace.clear();
                for (auto & ti: it.args_c())
                {
                    pcre2_code * pattern;
                    string replace;
                    if (ti.args_s().size() > 0 && ti.args_s().back().size() > 0)
                    {
                        T_V_Split vs = mtfa_string_split(ti.args_s().back(), ":");
                        pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        replace = Base64::decode(vs[1]);
                    }
                    else
                    {
                        pattern = nullptr;
                        replace.clear();
                    }
                    if (ti.m_id==Parser::token::TOKEN_NOT)
                        cr.donot=true;
                    else
                        cr.cats.push_back(make_tuple(VRCvt(ti.m_id), ti.name(), pattern, replace));
                }
                
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::CALL_LISP_FUNCTION;
                cr.modifier = VA_OpModifiers::NONE;
                SCM func_symbol = scm_c_lookup ( it.name().c_str() );
                cr.func = scm_variable_ref ( func_symbol );
                for (auto & ti: it.args_c())
                {
                    pcre2_code * pattern;
                    string replace;
                    if (ti.args_s().size() > 0 && ti.args_s().back().size() > 0)
                    {
                        T_V_Split vs = mtfa_string_split(ti.args_s().back(), ":");
                        pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        replace = Base64::decode(vs[1]);
                    }
                    else
                    {
                        pattern = nullptr;
                        replace.clear();
                    }
                    if (ti.m_id==Parser::token::TOKEN_NOT)
                        cr.donot=true;
                    else
                        cr.cats.push_back(make_tuple(VRCvt(ti.m_id), ti.name(), pattern, replace));
                }
                criteria.push_back ( cr );
            }
            break;


            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                cr.op2 = it.m_args_s.front();   //valore assoluto o nome della variabile WITH di riferimento
                if (it.args_s().size() > 1 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EQUAL;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //valore assoluto o nome della variabile WITH di riferimento
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;
            
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EQUAL;
                cr.modifier = VA_OpModifiers::INDEX;
                cr.op1 = it.m_name;             //indice dell'elemento obs
                cr.op2 = it.m_args_s.front();   //valore assoluto o nome della variabile WITH di riferimento
                if (it.args_s().size() > 1 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EXISTS;
                cr.modifier = VA_OpModifiers::INDEX;
                cr.op1 = it.m_name; //indice dell'elemento obs
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;
            default:
            {
                cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << it.m_id << "\n";
                exit ( 0 );
            }
        }
    }
    return criteria;
}

list<VAwith> ParseTOKEN_VR_DEF_IS_LISTA_WITH ( VAengine& vaeng, const Command cmd )
{
    list<VAwith> lvaw;
    cout << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
    for ( auto it : cmd.m_args_c )
    {
        cout << __func__   << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
        cout << __func__   << "\tWITH: " << totok ( it.m_args_c.front().m_id ) << it.m_args_c.front().m_name << endl;
        //it.m_id é sempre il simbolo =. Quindi scendo di un livello
        switch ( it.m_args_c.front().m_id )
        {
            case Parser::token::TOKEN_AGENT_ID:
            case Parser::token::TOKEN_SA_OS:
            case Parser::token::TOKEN_SA_LOGGED_USER:
            case Parser::token::TOKEN_SA_PROCESS_NAME:
            case Parser::token::TOKEN_OBS_NAME:
            case Parser::token::TOKEN_OBS_TIME:
            case Parser::token::TOKEN_OBS_OBSID:
            case Parser::token::TOKEN_OBS_STIME:
            case Parser::token::TOKEN_OBS_ETIME:
            case Parser::token::TOKEN_OBS_TAG:
            case Parser::token::TOKEN_OBS_EVENT:
            case Parser::token::TOKEN_NET_TCP_IPDST:
            case Parser::token::TOKEN_NET_TCP_IPSRC:
            case Parser::token::TOKEN_NET_RESULT:
            case Parser::token::TOKEN_NET_TCP_SESID:
            case Parser::token::TOKEN_HTTP_METHOD:
            case Parser::token::TOKEN_HTTP_HOST:
            case Parser::token::TOKEN_HTTP_URL:
            case Parser::token::TOKEN_HTTP_URI:
            case Parser::token::TOKEN_HTTP_HEADER_HOST:
            case Parser::token::TOKEN_HTTP_URL_HOST:
            {
                VAwith cr;
                cr.itype = VRCvt ( it.m_args_c.front().m_id );
                cr.modifier = VA_OpModifiers::NONE;
                cr.vname = it.m_name; //nome della variabile
                cr.pattern = nullptr;
                cr.replace.clear();
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    if (vs.size() == 2 && vs[1].size() > 0) {
                        cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        cr.replace = Base64::decode(vs[1]);
                    }
                }
                lvaw.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_CAT:
            {
                VAwith cr;
                cr.itype = VRCvt ( it.m_id );
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                cr.pattern=nullptr;
                cr.replace.clear();
                
                for (auto & ti: it.args_c())
                {
                    pcre2_code * pattern;
                    string replace;
                    if (ti.args_s().size() > 0 && ti.args_s().back().size() > 0)
                    {
                        T_V_Split vs = mtfa_string_split(ti.args_s().back(), ":");
                        pattern = nullptr;
                        replace.clear();
                        if (vs.size() == 2 && vs[1].size() > 0) {
                            pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                            replace = Base64::decode(vs[1]);
                        }
                    }
                    else
                    {
                        pattern = nullptr;
                        replace.clear();
                    }
                    cr.cats.push_back(make_tuple(VRCvt(ti.m_id), ti.name(), pattern, replace));
                }
                
                lvaw.push_back ( cr );
            }
            break;
            
            
            
            case Parser::token::TOKEN_HTTP_COOKIE:
            case Parser::token::TOKEN_HTTP_HEADER:
            case Parser::token::TOKEN_HTTP_QUERY:
            case Parser::token::TOKEN_HTTP_DATA:
            case Parser::token::TOKEN_HTTP_ANSWER_DATA:
            case Parser::token::TOKEN_HTTP_ANSWER_CODE:
            case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
            case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
//            case Parser::token::TOKEN_HTTP_ANSWER_DATA:
            {
                VAwith cr;
                cr.itype = VRCvt ( it.m_args_c.front().m_id );
                cr.op1 = it.m_args_c.front().m_name;
                cr.modifier = VA_OpModifiers::INDEX;
                cr.vname = it.m_name; //nome della variabile
                cr.pattern = nullptr;
                cr.replace.clear();
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                lvaw.push_back ( cr );
            }
            break;
            case Parser::token::TOKEN_IDENT:
            case Parser::token::TOKEN_STRING:
            {
                VAwith cr;
                cr.vname = it.m_name; //nome della variabile
                cr.itype = VRCvt ( it.m_args_c.front().m_id );
                cr.op1 = it.m_args_c.front().m_name;
                cr.modifier = VA_OpModifiers::INDEX;
                cr.pattern = nullptr;
                cr.replace.clear();
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    if (vs.size() == 2 && vs[1].size() > 0) {
                        cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        cr.replace = Base64::decode(vs[1]);
                    }
                }
                lvaw.push_back ( cr );
            }
            break;
            default:
            {
                cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << it.m_id << "\n";
                exit ( 0 );
            }
        }
    }
    return lvaw;
}

VA_NodeTree* ParseTOKEN_VR_DEF_CLAUSES ( VAengine& vaeng, const Command cmd )
{
    auto it = cmd.m_args_c.begin();
    switch ( cmd.m_id )
    {
        case Parser::token::TOKEN_VR_DEF_IS_LOP_NEXT:
        {
            VA_NodeTree* vant1 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );
            cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
            ++it;
            VA_NodeTree* vant2 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );

            VA_NodeTree* vant = new VA_NodeTree();
            vant->children.push_back ( vant1 );
            vant->children.push_back ( vant2 );
            vant->op = VA_LogicalTypes::BEFORE;
            vant->criteria.clear();
            return vant;
        }
        break;
        case Parser::token::TOKEN_VR_DEF_IS_LOP_OR:
        {
            VA_NodeTree* vant1 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );
            cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
            ++it;
            VA_NodeTree* vant2 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );

            VA_NodeTree* vant = new VA_NodeTree();
            vant->children.push_back ( vant1 );
            vant->children.push_back ( vant2 );
            vant->op = VA_LogicalTypes::OR;
            vant->criteria.clear();
            return vant;
        }
        break;
        case Parser::token::TOKEN_VR_DEF_IS_LOP_AND:
        {
            VA_NodeTree* vant1 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );
            cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
            ++it;
            VA_NodeTree* vant2 = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, *it );

            VA_NodeTree* vant = new VA_NodeTree();
            vant->children.push_back ( vant1 );
            vant->children.push_back ( vant2 );
            vant->op = VA_LogicalTypes::AND;
            vant->criteria.clear();
            return vant;
        }
        break;
        case Parser::token::TOKEN_VR_DEF_IS_A_WHEN_WITH:
        {
            cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
            //ogni nuova listawen viene aggiunto un vettore di criteri
            //whenwith ha due figli: TOKEN_VR_DEF_IS_LISTA_WHEN e TOKEN_VR_DEF_IS_LISTA_WITH
            list<VAcriteria> lvacr = ParseTOKEN_VR_DEF_IS_LISTA_WHEN ( vaeng, *it );
            ++it;
            list<VAwith> lwith = ParseTOKEN_VR_DEF_IS_LISTA_WITH ( vaeng, *it );
            ++it;
            vector<VAaction> actions = ParseTOKEN_VR_DEF_ACTION ( vaeng, *it );

            //Costruisco backward la struttura di un nodo con criteri al suo interno
            VA_NodeTree* vant = new VA_NodeTree();
            vant->children.clear();
            vant->op = VA_LogicalTypes::ITEM;
            vant->criteria.assign ( lvacr.begin(), lvacr.end() ); //!!!attento agli with!!!
            vant->with.assign ( lwith.begin(), lwith.end() );
            vant->actions.assign ( actions.begin(), actions.end() );
            return vant;
        }
        break;
        default:
            cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
            cout << __func__   << "errore: " << cmd.m_id << endl;
            return nullptr;
    }
}

vector<VAaction> ParseTOKEN_VR_DEF_ACTION ( VAengine& vaeng, const Command cmd )
{
    cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
    vector<VAaction> actions; // = new vector<VAaction>();
    for ( auto it : cmd.m_args_c )
    {
        cout << __func__   << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
        VAaction action;
        switch ( it.m_id )
        {
            case Parser::token::TOKEN_MYSQL_APPEND:
            {
                action.atype = VA_ActionTypes::MYSQL_APPEND;
                //ora i 6 parametri. i primi 5 mysql e il sesto l'item da archiviare
                auto pars = it.m_args_c.begin();
                string ip_port = pars->m_name;
                ++pars;
                string s_db = pars->m_name;
                ++pars;
                string s_user = pars->m_name;
                ++pars;
                string s_password = pars->m_name;
                ++pars;
                string s_query = pars->m_name;
                ++pars;

                //metto tutto in val1
                action.val1 =   Base64::encode ( ip_port ) + ":" +
                                Base64::encode ( s_db ) + ":" +
                                Base64::encode ( s_user ) + ":" +
                                Base64::encode ( s_password ) + ":" +
                                Base64::encode ( s_query );
                //e ora vedo da quale oggetto devo prendere i valore ma è una lista!
                while ( pars != it.m_args_c.end() )
                {
                    switch ( pars->m_id )
                    {
                        case Parser::token::TOKEN_AGENT_ID:
                        case Parser::token::TOKEN_SA_OS:
                        case Parser::token::TOKEN_SA_LOGGED_USER:
                        case Parser::token::TOKEN_SA_PROCESS_NAME:
                        case Parser::token::TOKEN_OBS_NAME:
                        case Parser::token::TOKEN_OBS_TIME:
                        case Parser::token::TOKEN_OBS_OBSID:
                        case Parser::token::TOKEN_OBS_STIME:
                        case Parser::token::TOKEN_OBS_ETIME:
                        case Parser::token::TOKEN_OBS_TAG:
                        case Parser::token::TOKEN_OBS_EVENT:
                        case Parser::token::TOKEN_NET_TCP_IPDST:
                        case Parser::token::TOKEN_NET_TCP_IPSRC:
                        case Parser::token::TOKEN_NET_RESULT:
                        case Parser::token::TOKEN_NET_TCP_SESID:
                        case Parser::token::TOKEN_HTTP_METHOD:
                        case Parser::token::TOKEN_HTTP_HOST:
                        case Parser::token::TOKEN_HTTP_URL:
                        case Parser::token::TOKEN_HTTP_URI:
                        case Parser::token::TOKEN_HTTP_HEADER_HOST:
                        case Parser::token::TOKEN_HTTP_URL_HOST:
    
                        //l'unica differenza sta che in quelli sopra il name è vuoto, in quelli sotto il name è l'indice
                        case Parser::token::TOKEN_HTTP_COOKIE:
                        case Parser::token::TOKEN_HTTP_HEADER:
                        case Parser::token::TOKEN_HTTP_QUERY:
                        case Parser::token::TOKEN_HTTP_DATA:
                        case Parser::token::TOKEN_HTTP_ANSWER_DATA:
                        case Parser::token::TOKEN_HTTP_ANSWER_CODE:
                        case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
                        case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
                            
                        //qui invece la distinzione è nel tipo (ident o string)
                        case Parser::token::TOKEN_IDENT:
                        case Parser::token::TOKEN_STRING:
                        {
                            pcre2_code * pattern=nullptr;
                            string replace;
                            if (pars->args_s().size() > 0)
                            {
                                auto p = pars->args_s().front();
                                T_V_Split vs = mtfa_string_split(p, ":");
                                if (vs.size() > 0)
                                {
                                    pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                    replace = Base64::decode(vs[1]);
                                }
                            }
                            action.vrep.push_back ( make_tuple ( VRCvt(pars->m_id), pars->m_name, pattern, replace, T_Cat()) );
                        }
                        break;
                        case Parser::token::TOKEN_CAT:
                        {
                            T_Cat tcat;
                            for (auto &ti: pars->args_c())
                            {
                                pcre2_code * pattern=nullptr;
                                string replace;
                                if (ti.args_s().size() > 0)
                                {
                                    auto p = ti.args_s().front();
                                    T_V_Split vs = mtfa_string_split(p, ":");
                                    if (vs.size() > 0)
                                    {
                                        pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                        replace = Base64::decode(vs[1]);
                                    }
                                }
                                tcat.push_back(make_tuple(VRCvt(ti.id()), ti.name(), pattern, replace));
                            }
                            action.vrep.push_back ( make_tuple ( VRCvt(pars->m_id), pars->m_name, nullptr, "", tcat) );
                        }
                        break;
                        default:
                        {
                            cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << pars->m_id << "\n";
                            exit ( 0 );
                        }
                    }
                    ++pars;
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION:
            {
                action.atype = VA_ActionTypes::CALL_LISP_FUNCTION;
                SCM func_symbol = scm_c_lookup ( it.name().c_str() );
                action.func = scm_variable_ref ( func_symbol );
                T_Cat tcat;
                for (auto &ti: it.args_c())
                {
                    pcre2_code * pattern=nullptr;
                    string replace;
                    if (ti.args_s().size() > 0)
                    {
                        auto p = ti.args_s().front();
                        T_V_Split vs = mtfa_string_split(p, ":");
                        if (vs.size() > 0)
                        {
                            pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                            replace = Base64::decode(vs[1]);
                        }
                    }
                    tcat.push_back(make_tuple(VRCvt(ti.id()), ti.name(), pattern, replace));
                }
                action.cat = tcat;
                actions.push_back ( action );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_ACTION_VREP_CREATE:
            {
                action.atype = VA_ActionTypes::ISSUE_VREP;
                action.val1 = it.m_name;
                for ( auto vri : it.args_c() )
                {
                    switch ( vri.m_id )
                    {
                        case Parser::token::TOKEN_AGENT_ID:
                        case Parser::token::TOKEN_SA_OS:
                        case Parser::token::TOKEN_SA_LOGGED_USER:
                        case Parser::token::TOKEN_SA_PROCESS_NAME:
                        case Parser::token::TOKEN_OBS_NAME:
                        case Parser::token::TOKEN_OBS_TIME:
                        case Parser::token::TOKEN_OBS_OBSID:
                        case Parser::token::TOKEN_OBS_STIME:
                        case Parser::token::TOKEN_OBS_ETIME:
                        case Parser::token::TOKEN_OBS_TAG:
                        case Parser::token::TOKEN_OBS_EVENT:
                        case Parser::token::TOKEN_NET_TCP_IPDST:
                        case Parser::token::TOKEN_NET_TCP_IPSRC:
                        case Parser::token::TOKEN_NET_RESULT:
                        case Parser::token::TOKEN_NET_TCP_SESID:
                        case Parser::token::TOKEN_HTTP_METHOD:
                        case Parser::token::TOKEN_HTTP_HOST:
                        case Parser::token::TOKEN_HTTP_URL:
                        case Parser::token::TOKEN_HTTP_URI:
                        case Parser::token::TOKEN_HTTP_HEADER_HOST:
                        case Parser::token::TOKEN_HTTP_URL_HOST:
                            
                        case Parser::token::TOKEN_HTTP_COOKIE:
                        case Parser::token::TOKEN_HTTP_HEADER:
                        case Parser::token::TOKEN_HTTP_QUERY:
                        case Parser::token::TOKEN_HTTP_DATA:
                        case Parser::token::TOKEN_HTTP_ANSWER_DATA:
                        case Parser::token::TOKEN_HTTP_ANSWER_CODE:
                        case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
                        case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
                            
                        case Parser::token::TOKEN_IDENT:
                        case Parser::token::TOKEN_STRING:
                        {
                            pcre2_code * pattern=nullptr;
                            string replace;
                            if (vri.args_s().size() > 0)
                            {
                                auto p = vri.args_s().front();
                                T_V_Split vs = mtfa_string_split(p, ":");
                                if (vs.size() > 0)
                                {
                                    pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                    replace = Base64::decode(vs[1]);
                                }
                            }
                            action.vrep.push_back ( make_tuple ( VRCvt(vri.m_id), vri.m_name, pattern, replace, T_Cat()) );
                        }
                        break;
                        case Parser::token::TOKEN_CAT:
                        {
                            T_Cat tcat;
                            for (auto &ti: vri.args_c())
                            {
                                pcre2_code * pattern=nullptr;
                                string replace;
                                if (ti.args_s().size() > 0)
                                {
                                    auto p = ti.args_s().front();
                                    T_V_Split vs = mtfa_string_split(p, ":");
                                    if (vs.size() > 0)
                                    {
                                        pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                        replace = Base64::decode(vs[1]);
                                    }
                                }
                                tcat.push_back(make_tuple(VRCvt(ti.id()), ti.name(), pattern, replace));
                            }
                            action.vrep.push_back ( make_tuple ( VRCvt(vri.m_id), vri.m_name, nullptr, "", tcat) );
                        }
                        break;
                        default:
                        {
                            cout << __func__   << "Error in " << __func__ << "." << __LINE__ << ": switch not satisfied: " << vri.m_id << "\n";
                            exit ( 0 );
                        }
                    }
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ACTION_ENABLE_VR:
            {
                action.atype = VA_ActionTypes::ENABLE_VR;
                action.val1 = it.m_name;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ACTION_DISABLE_VR:
            {
                action.atype = VA_ActionTypes::DISABLE_VR;
                action.val1 = it.m_name;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ACTION_ENABLE_AR:
            {
                action.atype = VA_ActionTypes::ENABLE_AR;
                action.val1 = it.m_name;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ACTION_DISABLE_AR:
            {
                action.atype = VA_ActionTypes::DISABLE_AR;
                action.val1 = it.m_name;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ACTION_SETADD:
            case Parser::token::TOKEN_ACTION_SETDEL:
            {
                if ( Parser::token::TOKEN_ACTION_SETADD == it.m_id )
                {
                    action.atype = VA_ActionTypes::ADD_VALUE_TO_SET;
                }
                else
                {
                    action.atype = VA_ActionTypes::DEL_VALUE_FROM_SET;
                }
                switch ( it.m_args_c.front().m_id )
                {
                    case Parser::token::TOKEN_AGENT_ID:
                    case Parser::token::TOKEN_SA_OS:
                    case Parser::token::TOKEN_SA_LOGGED_USER:
                    case Parser::token::TOKEN_SA_PROCESS_NAME:
                    case Parser::token::TOKEN_OBS_NAME:
                    case Parser::token::TOKEN_OBS_TIME:
                    case Parser::token::TOKEN_OBS_OBSID:
                    case Parser::token::TOKEN_OBS_STIME:
                    case Parser::token::TOKEN_OBS_ETIME:
                    case Parser::token::TOKEN_OBS_TAG:
                    case Parser::token::TOKEN_OBS_EVENT:
                    case Parser::token::TOKEN_NET_TCP_IPDST:
                    case Parser::token::TOKEN_NET_TCP_IPSRC:
                    case Parser::token::TOKEN_NET_RESULT:
                    case Parser::token::TOKEN_NET_TCP_SESID:
                    case Parser::token::TOKEN_HTTP_METHOD:
                    case Parser::token::TOKEN_HTTP_HOST:
                    case Parser::token::TOKEN_HTTP_URL:
                    case Parser::token::TOKEN_HTTP_URI:
                    case Parser::token::TOKEN_HTTP_HEADER_HOST:
                    case Parser::token::TOKEN_HTTP_URL_HOST:
                    {
                        action.observable = VRCvt ( it.m_args_c.front().m_id );
                        action.val1 = it.m_name;    //set name
                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val2 = pit->m_name;
                        }
                        else
                        {
                            action.val2 = "1000000000";
                        }
                        
//                         action.pattern=nullptr;
//                         action.replace.clear();
                        if (it.args_s().size() > 0)
                        {
                            auto p = it.args_s().front();
                            T_V_Split vs = mtfa_string_split(p, ":");
                            if (vs.size() > 0)
                            {
                                action.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                action.replace = Base64::decode(vs[1]);
                            }
                        }
                        
                        actions.push_back ( action );
                    }
                    break;

                    case Parser::token::TOKEN_HTTP_COOKIE:
                    case Parser::token::TOKEN_HTTP_HEADER:
                    case Parser::token::TOKEN_HTTP_QUERY:
                    case Parser::token::TOKEN_HTTP_DATA:
                    case Parser::token::TOKEN_HTTP_ANSWER_DATA:
                    case Parser::token::TOKEN_HTTP_ANSWER_CODE:
                    case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
                    case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
                    {
                        action.observable = VRCvt ( it.m_args_c.front().m_id );
                        action.val1 = it.m_name;    //set name
                        action.obs_index = it.m_args_c.front().m_name;
                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val2 = pit->m_name;
                        }
                        else
                        {
                            action.val2 = "1000000000";
                        }
                        
//                         pcre2_code * pattern=nullptr;
//                         string replace;
                        if (it.args_s().size() > 0)
                        {
                            auto p = it.args_s().front();
                            T_V_Split vs = mtfa_string_split(p, ":");
                            if (vs.size() > 0)
                            {
                                action.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                action.replace = Base64::decode(vs[1]);
                            }
                        }
                        
                        actions.push_back ( action );
                    }
                    break;
                    case Parser::token::TOKEN_STRING:
                    case Parser::token::TOKEN_IDENT:
                    {
                        action.observable = VA_ItemTypes::VARIABLE;
                        action.val1 = it.m_name;    //set name
                        action.val2 = it.m_args_c.front().m_name;
                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val3 = pit->m_name;
                        }
                        else
                        {
                            action.val3 = "1000000000";
                        }

                        actions.push_back ( action );
                    }
                    break;
                    
                    case Parser::token::TOKEN_CAT:
                    {
                        action.observable = VA_ItemTypes::CAT;
                        action.val1 = it.m_name;    //set name
                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val2 = pit->m_name;
                        }
                        else
                        {
                            action.val2 = "1000000000";
                        }

//                         std::function<int(VaParser::Command, string s)> fun;
//                         fun = [&fun] (Command it, string s) {
//                             cout << s << it.id() << ", " << it.str() << ", " << it.name() << endl;
//                             cout << s << "args_c" << endl;
//                             for (auto i1: it.args_c())
//                             {
//                                 fun(i1, s+"\t");
//                             }
//                             cout << s << "args_s" << endl;
//                             for (auto i1: it.m_args_s)
//                             {
//                                 cout << "\t" << i1 << endl;
//                             }
//                             return 0;
//                         };
//                         
//                         fun(it, "");
                        

                        for (auto &ti: it.args_c().front().args_c())
                        {
                            pcre2_code * pattern=nullptr;
                            string replace;
                            if (ti.args_s().size() > 0)
                            {
                                auto p = ti.args_s().front();
                                T_V_Split vs = mtfa_string_split(p, ":");
                                if (vs.size() > 0)
                                {
                                    pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                    replace = Base64::decode(vs[1]);
                                }
                            }
                            //cout << ti.id() << " => " << ti.name() << endl;
                            action.cat.push_back(make_tuple(VRCvt(ti.id()), ti.name(), pattern, replace));
                        }

                        actions.push_back ( action );
                    }
                    break;
                    
                    default:
                    {
                        cout << __func__   << "Error in " << __func__ << "." << __LINE__ << ": switch not satisfied: " << it.m_args_c.front().m_id << "\n";
                        exit ( 0 );
                    }
                }
            }
            break;
            case Parser::token::TOKEN_ACTION_SETVAR:
            {
                action.atype = VA_ActionTypes::SET_VAR_OF_AR;
                switch ( it.m_args_c.front().m_id )
                {
                    case Parser::token::TOKEN_AGENT_ID:
                    case Parser::token::TOKEN_SA_OS:
                    case Parser::token::TOKEN_SA_LOGGED_USER:
                    case Parser::token::TOKEN_SA_PROCESS_NAME:
                    case Parser::token::TOKEN_OBS_NAME:
                    case Parser::token::TOKEN_OBS_TIME:
                    case Parser::token::TOKEN_OBS_OBSID:
                    case Parser::token::TOKEN_OBS_STIME:
                    case Parser::token::TOKEN_OBS_ETIME:
                    case Parser::token::TOKEN_OBS_TAG:
                    case Parser::token::TOKEN_OBS_EVENT:
                    case Parser::token::TOKEN_NET_TCP_IPDST:
                    case Parser::token::TOKEN_NET_TCP_IPSRC:
                    case Parser::token::TOKEN_NET_RESULT:
                    case Parser::token::TOKEN_NET_TCP_SESID:
                    case Parser::token::TOKEN_HTTP_METHOD:
                    case Parser::token::TOKEN_HTTP_HOST:
                    case Parser::token::TOKEN_HTTP_URL:
                    case Parser::token::TOKEN_HTTP_URI:
                    case Parser::token::TOKEN_HTTP_HEADER_HOST:
                    case Parser::token::TOKEN_HTTP_URL_HOST:
                    {
                        auto iter = it.m_args_c.begin();
                        auto _ar = *iter;
                        ++iter;
                        auto _vr = *iter;
                        ++iter;
                        auto obs = *iter;
                        action.observable = VRCvt ( obs.m_id );
                        action.val1 = _ar.m_name;
                        action.val2 = _vr.m_name;

                        if (it.args_s().size() > 0)
                        {
                            auto p = it.args_s().front();
                            T_V_Split vs = mtfa_string_split(p, ":");
                            if (vs.size() > 0)
                            {
                                action.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                action.replace = Base64::decode(vs[1]);
                            }
                        }

                        actions.push_back ( action );
                    }
                    break;

                    case Parser::token::TOKEN_HTTP_COOKIE:
                    case Parser::token::TOKEN_HTTP_HEADER:
                    case Parser::token::TOKEN_HTTP_QUERY:
                    case Parser::token::TOKEN_HTTP_DATA:
                    case Parser::token::TOKEN_HTTP_ANSWER_DATA:
                    case Parser::token::TOKEN_HTTP_ANSWER_CODE:
                    case Parser::token::TOKEN_HTTP_ANSWER_COOKIE:
                    case Parser::token::TOKEN_HTTP_ANSWER_HEADER:
                        //            case Parser::token::TOKEN_HTTP_ANSWER_DATA:
                    {
                        auto iter = it.m_args_c.begin();
                        auto _ar = *iter;
                        ++iter;
                        auto _vr = *iter;
                        ++iter;
                        auto obs = *iter;
                        action.observable = VRCvt ( obs.m_id );
                        action.val1 = _ar.m_name;
                        action.val2 = _vr.m_name;
                        action.obs_index = obs.m_name;
                        
                        //pattern e replace sono nella lista s
                        if (it.args_s().size() > 0)
                        {
                            auto p = it.args_s().front();
                            T_V_Split vs = mtfa_string_split(p, ":");
                            if (vs.size() > 0)
                            {
                                action.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                action.replace = Base64::decode(vs[1]);
                            }
                        }
                        actions.push_back ( action );
                    }
                    break;
                    case Parser::token::TOKEN_STRING:
                    case Parser::token::TOKEN_IDENT:
                    {
                        auto iter = it.m_args_c.begin();
                        auto _ar = *iter;
                        ++iter;
                        auto _vr = *iter;
                        ++iter;
                        action.observable = VA_ItemTypes::VARIABLE;
                        action.val1 = _ar.m_name;
                        action.val2 = _vr.m_name;
                        action.val3 = iter->m_name;
                        if (it.args_s().size() > 0)
                        {
                            auto p = it.args_s().front();
                            T_V_Split vs = mtfa_string_split(p, ":");
                            if (vs.size() > 0)
                            {
                                action.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                action.replace = Base64::decode(vs[1]);
                            }
                        }
                        actions.push_back ( action );
                    }
                    break;

                    case Parser::token::TOKEN_CAT:
                    {
                        auto iter = it.m_args_c.begin();
                        auto _ar = *iter;
                        ++iter;
                        auto _vr = *iter;
                        ++iter;
                        action.observable = VA_ItemTypes::CAT;
                        action.val1 = _ar.m_name;
                        action.val2 = _vr.m_name;

                        for (auto &ti: it.args_c().front().args_c())
                        {
                            pcre2_code * pattern=nullptr;
                            string replace;
                            if (ti.args_s().size() > 0)
                            {
                                auto p = ti.args_s().front();
                                T_V_Split vs = mtfa_string_split(p, ":");
                                if (vs.size() > 0)
                                {
                                    pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                                    replace = Base64::decode(vs[1]);
                                }
                            }
                            action.cat.push_back(make_tuple(VRCvt(it.id()), it.name(), pattern, replace));
                        }
                        
                        actions.push_back ( action );
                    }
                    break;
                    
                    default:
                    {
                        cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << it.m_args_c.front().m_id << "\n";
                        exit ( 0 );
                    }
                }
            }
            break;
            default:
                cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
                cout << __func__   << "errore: " << cmd.m_id << endl;
                exit ( 0 );
        }
    }
    return actions;
}

VA_NodeTree* ParseTOKEN_VR_DEF ( VAengine& vaeng, const Command cmd )
{
    //questa è una VR.aggiungo una VR al vettore delle VR
    if (vaeng.varules.find(cmd.m_name) != vaeng.varules.end())
    {
        mtfa_log("VR %s already defined\n", cmd.m_name.c_str());
        exit(-1);
    }
    
    mtfa_log(". VR: <%s>\n", cmd.m_name.c_str());
    //la lista è fatta di due comandi: TOKEN_VR_DEF_WHEN e TOKEN_VR_DEF_ACTION

    auto it = cmd.m_args_c.begin();
    if ( 1 != it->m_args_c.size() )
    {
        mtfa_log("Errore!\n");
    }

    VA_NodeTree* vant = ParseTOKEN_VR_DEF_CLAUSES ( vaeng, it->m_args_c.front() );
    //Ma la posso semplificare? Certo, se trovo un operatore che ha come figlio lo stesso operatore: or -> or, allora il figlio or di or lo sussumo.

//    ++it;
//    vector<VAaction> actions = ParseTOKEN_VR_DEF_ACTION(vaeng, *it);

    //Aggiungo la validation rule al vettore delle validation rule.
    ValidationRule vr;
    vr.name = cmd.m_name;
    vr.root = vant;
    vr.enabled = true; 
    //vr.actions = actions;
    vaeng.varules[cmd.m_name] = vr;

    return vant;   //non serve
}

vector<VAcriteria> ParseTOKEN_BASIC_ACCESS_RULES ( VAengine& vaeng, const Command cmd )
{
    cout << __func__  << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;

    vector<VAcriteria> criteria;
    for ( auto it : cmd.m_args_c )
    {
        cout << __func__  << ": " << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
        switch ( it.m_id )
        {
            case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN:   //lo prende dal tag
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN:  //il tag contiene agent e sessionid
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }

                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op2 = it.m_name; //nome del cookie
                cr.op1 = it.m_args_s.front(); //insieme di riferimento
                if (it.args_s().size() > 1 && it.args_s().back().size() > 1)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EQUAL;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EQUAL;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                cr.op2 = it.m_args_s.front();
                if (it.args_s().size() > 1 && it.args_s().back().size() > 1)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;

            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS:
            case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::EXISTS;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.m_name; //nome dell'insieme di riferimento
                if (it.args_s().size() > 0 && it.args_s().back().size() > 0)
                {
                    T_V_Split vs = mtfa_string_split(it.args_s().back(), ":");
                    cr.pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                    cr.replace = Base64::decode(vs[1]);
                }
                if (it.args_c().back().m_id==Parser::token::TOKEN_NOT)
                    cr.donot=true;
                criteria.push_back ( cr );
            }
            break;
            case Parser::token::TOKEN_CAT:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::IN;
                cr.modifier = VA_OpModifiers::NONE;
                cr.op1 = it.args_s().front(); //nome dell'insieme di riferimento
                cr.pattern=nullptr;
                cr.replace.clear();
                for (auto & ti: it.args_c())
                {
                    pcre2_code * pattern= nullptr;
                    string replace;
                    if (ti.args_s().size() > 0 && ti.args_s().back().size() > 0)
                    {
                        T_V_Split vs = mtfa_string_split(ti.args_s().back(), ":");
                        pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        replace = Base64::decode(vs[1]);
                    }
                    if (ti.m_id==Parser::token::TOKEN_NOT)
                        cr.donot=true;
                    cr.cats.push_back(make_tuple(VRCvt(ti.m_id), ti.name(), pattern, replace));
                }
                
                criteria.push_back ( cr );
            }
            break;
            case Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION:
            {
                VAcriteria cr;
                cr.itype = VRCvt ( it.m_id );
                cr.op = VA_OpTypes::CALL_LISP_FUNCTION;
                SCM func_symbol = scm_c_lookup ( it.name().c_str() );
                cr.func = scm_variable_ref ( func_symbol );
                cr.modifier = VA_OpModifiers::NONE;
                cr.pattern=nullptr;
                cr.replace.clear();
                for (auto & ti: it.args_c())
                {
                    pcre2_code * pattern= nullptr;;
                    string replace;
                    cr.cats.push_back(make_tuple(VRCvt(ti.m_id), ti.name(), pattern, replace));
                }
                
                criteria.push_back ( cr );
            }
            break;
            default:
            {
                cout << __func__  << ": " << "Error. switch not satisfied: " << it.m_id << "\n";
                exit ( 0 );
            }
        }
    }
    std::reverse(std::begin(criteria), std::end(criteria));
    return criteria;
}

// vector<VAcriteria> ParseTOKEN_EXTENDED_ACCESS_RULES ( VAengine& vaeng, const Command cmd )
// {
//     cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
// 
//     vector<VAcriteria> criteria;
//     for ( auto it : cmd.m_args_c )
//     {
//         cout << __func__   << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
//         switch ( it.m_id )
//         {
// //            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN:   //from tag
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN: //from tag
//             case Parser::token::TOKEN_TIME:
//             {
//                 VAcriteria cr;
//                 cr.itype = VRCvt ( it.m_id );
//                 cr.op = VA_OpTypes::IN;
//                 cr.modifier = VA_OpModifiers::NONE;
//                 cr.op1 = it.m_name; //nome dell'insieme di riferimento
//                 criteria.push_back ( cr );
//             }
//             break;
// 
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN:
//             {
//                 VAcriteria cr;
//                 cr.itype = VRCvt ( it.m_id );
//                 cr.op = VA_OpTypes::IN;
//                 cr.modifier = VA_OpModifiers::NONE;
//                 cr.op2 = it.m_name;
//                 cr.op1 = it.m_args_s.front(); //nome dell'insieme di riferimento
//                 criteria.push_back ( cr );
//             }
//             break;
// 
// //            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS:  //from tag
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS:
// //            case Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS: //from tag
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS:
//             {
//                 VAcriteria cr;
//                 cr.itype = VRCvt ( it.m_id );
//                 cr.op = VA_OpTypes::EQUAL;
//                 cr.modifier = VA_OpModifiers::NONE;
//                 cr.op1 = it.m_name; //nome dell'insieme di riferimento
//                 criteria.push_back ( cr );
//             }
//             break;
// 
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS:
//             {
//                 VAcriteria cr;
//                 cr.itype = VRCvt ( it.m_id );
//                 cr.op = VA_OpTypes::EQUAL;
//                 cr.modifier = VA_OpModifiers::NONE;
//                 cr.op1 = it.m_name; //nome dell'insieme di riferimento
//                 cr.op2 = it.m_args_s.front();
//                 criteria.push_back ( cr );
//             }
//             break;
// 
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS:
//             case Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS:
//             {
//                 VAcriteria cr;
//                 cr.itype = VRCvt ( it.m_id );
//                 cr.op = VA_OpTypes::EXISTS;
//                 cr.modifier = VA_OpModifiers::NONE;
//                 cr.op1 = it.m_name; //nome dell'insieme di riferimento
//                 criteria.push_back ( cr );
//             }
//             break;
//             default:
//             {
//                 cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << it.m_id << "\n";
//                 exit ( 0 );
//             }
//         }
//     }
//     return criteria;
// }

//pair<vector<VAcriteria>, vector<VAcriteria>>  ParseTOKEN_IF(VAengine & vaeng, const Command cmd) {
vector<VAcriteria> ParseTOKEN_IF ( VAengine& vaeng, const Command cmd )
{
    cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;

    vector<VAcriteria> basic_ar;
    //vector<VAcriteria> extended_ar;

    auto it = cmd.m_args_c.begin();
    basic_ar = ParseTOKEN_BASIC_ACCESS_RULES ( vaeng, *it );
//    ++it;
//    if (it != cmd.m_args_c.end())
//        extended_ar = ParseTOKEN_EXTENDED_ACCESS_RULES(vaeng, *it);

    //return make_pair(basic_ar, extended_ar);
    return basic_ar;
}

vector<KAMaction> ParseTOKEN_ACCESS_OPERATIONS ( VAengine& vaeng, const Command cmd )
{
    cout << __func__   << __func__ << ": " << totok ( cmd.m_id ) << "(" << cmd.m_id << ")" << ", " << cmd.m_name << ", " << cmd.m_args_s.size() << ", " << cmd.m_args_c.size() << endl;
    vector<KAMaction> actions;
    for ( auto it : cmd.m_args_c )
    {
        cout << __func__   << totok ( it.m_id ) << "(" << it.m_id << ")" << ", " << it.m_name << ", " << it.m_args_s.size() << ", " << it.m_args_c.size() << endl;
        switch ( it.m_id )
        {
            case Parser::token::TOKEN_REPLACE_HEADER_IN_REQUEST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_REQUEST_REPLACE;
                auto p = it.m_args_s.begin();

                action.replace_pattern = mtfa_compile_pattern(*p); ++p; //replace pattern
                action.replace = *p;                               //replace string
                actions.push_back ( action );
            }
            break;

            case Parser::token::TOKEN_REPLACE_HEADER_IN_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_ANSWER_REPLACE;
                auto p = it.m_args_s.begin();
                
                action.replace_pattern = mtfa_compile_pattern(*p); ++p; //replace pattern
                action.replace = *p;                               //replace string
                actions.push_back ( action );
            }
            break;

            case Parser::token::TOKEN_ADD_HEADER_TO_REQUEST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_REQUEST_ADD;
                auto p = it.m_args_s.begin();
                
                action.var_name.clear();   //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value.clear(); //variabile value header
                action.last_value_1 = *p;       //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_HEADER_TO_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_ANSWER_ADD;
                auto p = it.m_args_s.begin();
                action.var_name.clear();   //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value.clear(); //variabile value header
                action.last_value_1 = *p;       //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_V_HEADER_TO_V_REQUEST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_REQUEST_VADD;
                auto p = it.m_args_s.begin();
                action.var_name = *p;      ++p; //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value = *p;    ++p; //variabile value header
                action.last_value_1 = *p;  ++p; //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_V_HEADER_TO_V_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HEADER_ANSWER_VADD;
                auto p = it.m_args_s.begin();
                action.var_name = *p;      ++p; //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value = *p;    ++p; //variabile value header
                action.last_value_1 = *p;  ++p; //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_COOKIE_TO_REQUEST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::COOKIE_REQUEST_ADD;
                auto p = it.m_args_s.begin();
                
                action.var_name.clear();   //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value.clear(); //variabile value header
                action.last_value_1 = *p;       //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_COOKIE_TO_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::COOKIE_ANSWER_ADD;
                auto p = it.m_args_s.begin();
                action.var_name.clear();   //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value.clear(); //variabile value header
                action.last_value_1 = *p;       //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_V_COOKIE_TO_V_REQUEST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::COOKIE_REQUEST_VADD;
                auto p = it.m_args_s.begin();
                action.var_name = *p;      ++p; //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value = *p;    ++p; //variabile value header
                action.last_value_1 = *p;  ++p; //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::COOKIE_ANSWER_VADD;
                auto p = it.m_args_s.begin();
                action.var_name = *p;      ++p; //variabile nome header
                action.default_value = *p; ++p; //default nome header
                action.last_value = *p;    ++p; //variabile value header
                action.last_value_1 = *p;  ++p; //default value header
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_TRACE:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::TRACE;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;        //dominio VA
                action.default_value = *p++;   //nome cookie
                action.last_value = *p;   //nome insieme
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_TCP_REDIRECT_VSTRING:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::TCP_REDIRECT_VSTRING;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.default_value = *p++;
                action.server_cert.clear();
                action.hostname.clear();
                action.client_cert.clear();
                action.client_key.clear();
                action.check_server_cert=false;
                if (p != it.m_args_s.end())
                {
                    action.server_cert = *p++;
                    if (p != it.m_args_s.end())
                    {
                        if (*p != "CHECK" && *p != "DONT CHECK")
                        {
                            action.hostname = *p++;
                            if (p != it.m_args_s.end())
                            {
                                if (*p != "CHECK" && *p != "DONT CHECK")
                                {
                                    action.client_cert = *p++;
                                    action.client_key = *p++;
                                    if (p != it.m_args_s.end() && (*p == "CHECK" || *p == "DONT CHECK"))
                                        action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                                else
                                {
                                    action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                            }
                        }
                        else
                        {
                            action.check_server_cert = (*p == "CHECK")?true:false;
                        }
                    }
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_TCP_REDIRECT_STRING:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::TCP_REDIRECT_STRING;
                auto p = it.m_args_s.begin();
                action.var_name.clear();
                action.default_value = *p++;
                action.server_cert.clear();
                action.hostname.clear();
                action.client_cert.clear();
                action.client_key.clear();
                action.check_server_cert=false;
                if (p != it.m_args_s.end())
                {
                    action.server_cert = *p++;
                    if (p != it.m_args_s.end())
                    {
                        if (*p != "CHECK" && *p != "DONT CHECK")
                        {
                            action.hostname = *p++;
                            if (p != it.m_args_s.end())
                            {
                                if (*p != "CHECK" && *p != "DONT CHECK")
                                {
                                    action.client_cert = *p++;
                                    action.client_key = *p++;
                                    if (p != it.m_args_s.end() && (*p == "CHECK" || *p == "DONT CHECK"))
                                        action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                                else
                                {
                                    action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                            }
                        }
                        else
                        {
                            action.check_server_cert = (*p == "CHECK")?true:false;
                        }
                    }
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_TCP_REDIRECT_VHOST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::TCP_REDIRECT_VHOST;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.server_cert.clear();
                action.hostname.clear();
                action.client_cert.clear();
                action.client_key.clear();
                action.check_server_cert=false;
                if (p != it.m_args_s.end())
                {
                    action.server_cert = *p++;
                    if (p != it.m_args_s.end())
                    {
                        if (*p != "CHECK" && *p != "DONT CHECK")
                        {
                            action.hostname = *p++;
                            if (p != it.m_args_s.end())
                            {
                                if (*p != "CHECK" && *p != "DONT CHECK")
                                {
                                    action.client_cert = *p++;
                                    action.client_key = *p++;
                                    if (p != it.m_args_s.end() && (*p == "CHECK" || *p == "DONT CHECK"))
                                        action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                                else
                                {
                                    action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                            }
                        }
                        else
                        {
                            action.check_server_cert = (*p == "CHECK")?true:false;
                        }
                    }
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_TCP_REDIRECT_HOST:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::TCP_REDIRECT_HOST;
                auto p = it.m_args_s.begin();
                action.server_cert.clear();
                action.hostname.clear();
                action.client_cert.clear();
                action.client_key.clear();
                action.check_server_cert=false;
                if (p != it.m_args_s.end())
                {
                    action.server_cert = *p++;
                    if (p != it.m_args_s.end())
                    {
                        if (*p != "CHECK" && *p != "DONT CHECK")
                        {
                            action.hostname = *p++;
                            if (p != it.m_args_s.end())
                            {
                                if (*p != "CHECK" && *p != "DONT CHECK")
                                {
                                    action.client_cert = *p++;
                                    action.client_key = *p++;
                                    if (p != it.m_args_s.end() && (*p == "CHECK" || *p == "DONT CHECK"))
                                        action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                                else
                                {
                                    action.check_server_cert = (*p == "CHECK")?true:false;
                                }
                            }
                        }
                        else
                        {
                            action.check_server_cert = (*p == "CHECK")?true:false;
                        }
                    }
                    T_V_Split sslv = mtfa_string_split(action.server_cert, "|");
                    if (sslv.size() > 1)
                    {
                        action.server_cert   = sslv[0];
                        action.client_method = sslv[1];
                        if (sslv.size() > 2)
                            action.client_algo = mtfa_replace_matching_pattern(sslv[2], ",", ":", true);
                    }
                }
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_HTTP_VREDIRECT:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HTTP_VREDIRECT;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_HTTP_REDIRECT:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::HTTP_REDIRECT;
                auto p = it.m_args_s.begin();
                action.var_name.clear();
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_VBANDWIDTH:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::VBANDWIDTH;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_BANDWIDTH:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::BANDWIDTH;
                auto p = it.m_args_s.begin();
                action.var_name.clear();
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_VANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::VANSWER;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_ANSWER:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::ANSWER;
                auto p = it.m_args_s.begin();
                action.var_name.clear();
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_SCM_GIVE:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::SCM_GIVE;
                auto p = it.m_args_s.begin();
                action.var_name.clear();
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_SCM_MANAGE:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::SCM_MANAGE;
                action.var_name.clear();
                action.scm_manage_params = it.m_args_c;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_VSCM_GIVE:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::VSCM_GIVE;
                auto p = it.m_args_s.begin();
                action.var_name = *p++;
                action.default_value = *p;
                actions.push_back ( action );
            }
            break;
            case Parser::token::TOKEN_VSCM_MANAGE:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::VSCM_MANAGE;
                action.var_name.clear();
                action.scm_manage_params = it.m_args_c;
                actions.push_back ( action );
            }
            break;
//            case Parser::token::TOKEN_VEXPIRY:
//            {
//                KAMaction action;
//                action.action = KAM_ActionTypes::VEXPIRY;
//                auto p=it.m_args_s.begin();
//                action.var_name = *p; ++p;
//                action.default_value = *p;
//                actions.push_back(action);
//            }
//            break;
//            case Parser::token::TOKEN_EXPIRY:
//            {
//                KAMaction action;
//                action.action = KAM_ActionTypes::EXPIRY;
//                auto p=it.m_args_s.begin();
//                action.var_name = *p;
//                actions.push_back(action);
//            }
//            break;
            
            //Le recenti aggiunte (15 marzo 2017)
            case Parser::token::TOKEN_ACTION_SETADD:
            case Parser::token::TOKEN_ACTION_SETDEL:
            {
                KAMaction action;
                if ( Parser::token::TOKEN_ACTION_SETADD == it.m_id )
                {
                    action.action = KAM_ActionTypes::ADD_VALUE_TO_SET;
                }
                else
                {
                    action.action = KAM_ActionTypes::DEL_VALUE_FROM_SET;
                }

                switch ( it.m_args_c.front().m_id )
                {
                    case Parser::token::TOKEN_STRING:
                    case Parser::token::TOKEN_IDENT:
                    {
                        //Per ora aggiungo esclusivamente variabili stringa. Se servisse altro potrò aggiungerlo in seguito
                        action.observable = VA_ItemTypes::VARIABLE;
                        action.val1 = it.m_name;    //set name
                        action.val2 = it.m_args_c.front().m_name;
                        
                        T_V_Split vs = mtfa_string_split(it.m_args_s.front(), ":");
                        action.replace_pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        action.replace = Base64::decode(vs[1]);

                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val3 = pit->m_name;
                        }
                        else
                        {
                            action.val3 = "1000000000";
                        }

                        actions.push_back ( action );
                    }
                    break;

                    case Parser::token::TOKEN_CAT:
                    {
                        action.observable = VA_ItemTypes::CAT;
                        action.val1 = it.m_name;    //set name

                        if ( it.m_args_c.size() > 1 )
                        {
                            auto pit = it.m_args_c.begin();
                            ++pit;
                            action.val2 = pit->m_name;
                        }
                        else
                        {
                            action.val2 = "1000000000";
                        }

                        T_V_Split vs = mtfa_string_split(it.m_args_s.front(), ":");
                        action.replace_pattern = mtfa_compile_pattern(Base64::decode(vs[0]));
                        action.replace = Base64::decode(vs[1]);

                        T_Cat tcat;

                        for ( auto& ti : it.args_c().front().args_c() )
                        {
                            pcre2_code* pattern = nullptr;
                            string replace;

                            if ( ti.args_s().size() > 0 )
                            {
                                auto p = ti.args_s().front();
                                T_V_Split vs = mtfa_string_split ( p, ":" );

                                if ( vs.size() > 0 )
                                {
                                    pattern = mtfa_compile_pattern ( Base64::decode ( vs[0] ) );
                                    replace = Base64::decode ( vs[1] );
                                }
                            }
                            action.cat.push_back ( make_tuple ( VRCvt ( ti.id() ), ti.name(), pattern, replace ) );
                        }

                        actions.push_back ( action );
                    }
                    break;

                    default:
                    {
                        cout << __func__   << "Error in " << __func__ << "." << __LINE__ << ": Not implemented. ask FA: " << it.m_args_c.front().m_id << "\n";
                        exit ( 0 );
                    }
                }
            }
            break;
            
            
            case Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION:
            {
                KAMaction action;
                action.action = KAM_ActionTypes::CALL_LISP_FUNCTION;
                SCM func_symbol = scm_c_lookup ( it.name().c_str() );
                action.func = scm_variable_ref ( func_symbol );
                for ( auto& ti : it.args_c() )
                {
                    pcre2_code* pattern = nullptr;
                    string replace;

                    if ( ti.args_s().size() > 0 )
                    {
                        auto p = ti.args_s().front();
                        T_V_Split vs = mtfa_string_split ( p, ":" );

                        if ( vs.size() > 0 )
                        {
                            pattern = mtfa_compile_pattern ( Base64::decode ( vs[0] ) );
                            replace = Base64::decode ( vs[1] );
                        }
                    }
                    action.cat.push_back ( make_tuple ( VRCvt ( ti.id() ), ti.name(), pattern, replace ) );
                }
                
                actions.push_back ( action );
            }
            break;


            default:
            {
                cout << __func__   << "Error in " << __func__ << ": switch not satisfied: " << it.m_id << "\n";
                exit ( 0 );
            }
        }
    }
    std::reverse(std::begin(actions), std::end(actions));
    return actions;
}

void ParseTOKEN_AR_DEF ( VAengine& vaeng, const Command cmd )
{
    if (vaeng.acrules.find(cmd.m_name) != vaeng.acrules.end())
    {
        cout << "AR " << cmd.m_name << " already defined" << endl;
        exit(-1);
    }

    cout << __func__   << endl << endl << endl << "AR: <" << cmd.m_name << ">" << cmd.m_args_c.size() << endl;
    //la lista è fatta di due comandi: TOKEN_VR_DEF_WHEN e TOKEN_VR_DEF_ACTION

    //ar name = cmd.m_name
    AccessRule ar;
    ar.name = cmd.m_name;
    ar.enabled = true;

    auto it = cmd.m_args_c.begin();
    //pair<vector<VAcriteria>, vector<VAcriteria>> res = ParseTOKEN_IF(vaeng, *it);
    vector<VAcriteria>res = ParseTOKEN_IF ( vaeng, *it );
    ar.basic_access_criteria = res; //.first;
    //ar.extended_access_criteria = res.second;

    ++it;
    ar.access_operations = ParseTOKEN_ACCESS_OPERATIONS ( vaeng, *it );

    if (vaeng.acrules.find(ar.name) != vaeng.acrules.end())
        vaeng.acrules.erase(ar.name);
    vaeng.acrules.insert(make_pair(ar.name, ar));
    vaeng.acrules_ordering.push_back ( ar.name );
    return;
}

void ParseCommand ( VAengine& vaeng, const Command& cmd )
{
    switch ( cmd.m_id )
    {
        //gli insiemi
        case Parser::token::TOKEN_DICTIONARY_LIST_ID:
            if ( cmd.m_args_c.size() > 0 )
            {
                ParseSetOfStringsPairs ( vaeng, cmd.m_name, cmd.m_args_c );
            }
            else
            {
                ParseSetOfStringsPairs ( vaeng, cmd.m_name, cmd.m_args_spair );
            }
            break;
        case Parser::token::TOKEN_SET_LIST_ID:
            if ( cmd.m_args_c.size() > 0 )
            {
                ParseSetOfStrings ( vaeng, cmd.m_name, cmd.m_args_c );
            }
            else
            {
                ParseSetOfStrings ( vaeng, cmd.m_name, cmd.m_args_s );
            }
            break;
        case Parser::token::TOKEN_SET_LIST_IPSTAR:
            if ( cmd.m_args_c.size() > 0 )
            {
                ParseSetOfIpStar ( vaeng, cmd.m_name, cmd.m_args_c );
            }
            else
            {
                ParseSetOfIpStar ( vaeng, cmd.m_name, cmd.m_args_s );
            }
            break;
//        case Parser::token::TOKEN_SET_MYSQL:
//            ParseSetOfMySql(vaeng, cmd.m_name, cmd.m_args_s);
//            break;
        case Parser::token::TOKEN_SET_LIST_TIMING:
            if ( cmd.m_args_c.size() > 0 )
            {
                ParseSetOfTimings ( vaeng, cmd.m_name, cmd.m_args_c );
            }
            else
            {
                ParseSetOfTimings ( vaeng, cmd.m_name, cmd.m_args_s );
            }
            break;
        case Parser::token::TOKEN_SET_LIST_URLSTAR:
            if ( cmd.m_args_c.size() > 0 )
            {
                ParseSetOfUrlStar ( vaeng, cmd.m_name, cmd.m_args_c );
            }
            else
            {
                ParseSetOfUrlStar ( vaeng, cmd.m_name, cmd.m_args_s );
            }
            break;

        //Le validation rule
        case Parser::token::TOKEN_VR_DEF:
            ParseTOKEN_VR_DEF ( vaeng, cmd );
            break;

        //Le access rule
        case Parser::token::TOKEN_AR_DEF:
            ParseTOKEN_AR_DEF ( vaeng, cmd );
            break;
            
        default:
            mtfa_log("Errore. Switch-Case not found: %d\n", cmd.m_id);
            exit(0);
            break;
    }
}

void AstParse ( VAengine& vaeng, Interpreter& interpreter )
{
    cout << __func__   << ". Interpreter: " << interpreter.m_commands.size() << " commands received." << endl;
    for ( int i = 0; i < interpreter.m_commands.size(); i++ )
    {
        ParseCommand ( vaeng, interpreter.m_commands[i] );
    }
}

string totok ( int i )
{
    static map<int, string> tok;
    static bool done = false;
    if ( !done )
    {
        done = true;
tok[Parser::token::TOKEN_END]=string("TOKEN_END"); 
tok[Parser::token::TOKEN_IDENT]=string("TOKEN_IDENT"); 
tok[Parser::token::TOKEN_STRING]=string("TOKEN_STRING"); 
tok[Parser::token::TOKEN_VARIABLE]=string("TOKEN_VARIABLE"); 
tok[Parser::token::TOKEN_I_NUMBER]=string("TOKEN_I_NUMBER"); 
tok[Parser::token::TOKEN_IP_WITH_STAR]=string("TOKEN_IP_WITH_STAR"); 
tok[Parser::token::TOKEN_IP_WITHOUT_STAR]=string("TOKEN_IP_WITHOUT_STAR"); 
tok[Parser::token::TOKEN_URL_WITH_STAR]=string("TOKEN_URL_WITH_STAR"); 
tok[Parser::token::TOKEN_TIMING]=string("TOKEN_TIMING"); 
tok[Parser::token::TOKEN_EQUAL]=string("TOKEN_EQUAL"); 
tok[Parser::token::TOKEN_LEFTPAR]=string("TOKEN_LEFTPAR"); 
tok[Parser::token::TOKEN_RIGHTPAR]=string("TOKEN_RIGHTPAR"); 
tok[Parser::token::TOKEN_SEMICOLON]=string("TOKEN_SEMICOLON"); 
tok[Parser::token::TOKEN_COMMA]=string("TOKEN_COMMA"); 
tok[Parser::token::TOKEN_LEFTSQPAR]=string("TOKEN_LEFTSQPAR"); 
tok[Parser::token::TOKEN_RIGHTSQPAR]=string("TOKEN_RIGHTSQPAR"); 
tok[Parser::token::TOKEN_LEFTGRPAR]=string("TOKEN_LEFTGRPAR"); 
tok[Parser::token::TOKEN_RIGHTGRPAR]=string("TOKEN_RIGHTGRPAR"); 
tok[Parser::token::TOKEN_DOT]=string("TOKEN_DOT"); 
tok[Parser::token::TOKEN_DOLLAR]=string("TOKEN_DOLLAR"); 
tok[Parser::token::TOKEN_OR]=string("TOKEN_OR"); 
tok[Parser::token::TOKEN_AND]=string("TOKEN_AND"); 
tok[Parser::token::TOKEN_NEXT]=string("TOKEN_NEXT"); 
tok[Parser::token::TOKEN_ADD]=string("TOKEN_ADD"); 
tok[Parser::token::TOKEN_DEL]=string("TOKEN_DEL"); 
tok[Parser::token::TOKEN_TO]=string("TOKEN_TO"); 
tok[Parser::token::TOKEN_FROM]=string("TOKEN_FROM"); 
tok[Parser::token::TOKEN_VEXPIRY]=string("TOKEN_VEXPIRY"); 
tok[Parser::token::TOKEN_TCP_REDIRECT]=string("TOKEN_TCP_REDIRECT"); 
tok[Parser::token::TOKEN_TIME]=string("TOKEN_TIME"); 
tok[Parser::token::TOKEN_TCP_REDIRECT_STRING]=string("TOKEN_TCP_REDIRECT_STRING"); 
tok[Parser::token::TOKEN_TCP_REDIRECT_VSTRING]=string("TOKEN_TCP_REDIRECT_VSTRING"); 
tok[Parser::token::TOKEN_TCP_REDIRECT_HOST]=string("TOKEN_TCP_REDIRECT_HOST"); 
tok[Parser::token::TOKEN_TCP_REDIRECT_VHOST]=string("TOKEN_TCP_REDIRECT_VHOST"); 
tok[Parser::token::TOKEN_SCM_MANAGE]=string("TOKEN_SCM_MANAGE"); 
tok[Parser::token::TOKEN_SCM_GIVE]=string("TOKEN_SCM_GIVE"); 
tok[Parser::token::TOKEN_HTTP_REDIRECT]=string("TOKEN_HTTP_REDIRECT"); 
tok[Parser::token::TOKEN_HTTP_VREDIRECT]=string("TOKEN_HTTP_VREDIRECT"); 
tok[Parser::token::TOKEN_BANDWIDTH]=string("TOKEN_BANDWIDTH"); 
tok[Parser::token::TOKEN_VBANDWIDTH]=string("TOKEN_VBANDWIDTH"); 
tok[Parser::token::TOKEN_ANSWER]=string("TOKEN_ANSWER"); 
tok[Parser::token::TOKEN_VANSWER]=string("TOKEN_VANSWER"); 
tok[Parser::token::TOKEN_VSCM_MANAGE]=string("TOKEN_VSCM_MANAGE"); 
tok[Parser::token::TOKEN_VSCM_GIVE]=string("TOKEN_VSCM_GIVE"); 
tok[Parser::token::TOKEN_TRACE]=string("TOKEN_TRACE"); 
tok[Parser::token::TOKEN_NOT]=string("TOKEN_NOT"); 
tok[Parser::token::TOKEN_WHEN]=string("TOKEN_WHEN"); 
tok[Parser::token::TOKEN_IF]=string("TOKEN_IF"); 
tok[Parser::token::TOKEN_THEN]=string("TOKEN_THEN"); 
tok[Parser::token::TOKEN_ENABLED]=string("TOKEN_ENABLED"); 
tok[Parser::token::TOKEN_DISABLED]=string("TOKEN_DISABLED"); 
tok[Parser::token::TOKEN_DEFAULT]=string("TOKEN_DEFAULT"); 
tok[Parser::token::TOKEN_GT]=string("TOKEN_GT"); 
tok[Parser::token::TOKEN_LT]=string("TOKEN_LT"); 
tok[Parser::token::TOKEN_GTE]=string("TOKEN_GTE"); 
tok[Parser::token::TOKEN_LTE]=string("TOKEN_LTE"); 
tok[Parser::token::TOKEN_PLUS]=string("TOKEN_PLUS"); 
tok[Parser::token::TOKEN_MINUS]=string("TOKEN_MINUS"); 
tok[Parser::token::TOKEN_AT]=string("TOKEN_AT"); 
tok[Parser::token::TOKEN_TRUE]=string("TOKEN_TRUE"); 
tok[Parser::token::TOKEN_FALSE]=string("TOKEN_FALSE"); 
tok[Parser::token::TOKEN_SET]=string("TOKEN_SET"); 
tok[Parser::token::TOKEN_IPSET]=string("TOKEN_IPSET"); 
tok[Parser::token::TOKEN_URLSET]=string("TOKEN_URLSET"); 
tok[Parser::token::TOKEN_TIMESET]=string("TOKEN_TIMESET"); 
tok[Parser::token::TOKEN_PARSET]=string("TOKEN_PARSET"); 
tok[Parser::token::TOKEN_DEFINE]=string("TOKEN_DEFINE"); 
tok[Parser::token::TOKEN_MYSQL]=string("TOKEN_MYSQL"); 
tok[Parser::token::TOKEN_AGENT_ID]=string("TOKEN_AGENT_ID"); 
tok[Parser::token::TOKEN_VR]=string("TOKEN_VR"); 
tok[Parser::token::TOKEN_VREP]=string("TOKEN_VREP"); 
tok[Parser::token::TOKEN_REPORT]=string("TOKEN_REPORT"); 
tok[Parser::token::TOKEN_MREPORT]=string("TOKEN_MREPORT"); 
tok[Parser::token::TOKEN_CONDITION]=string("TOKEN_CONDITION"); 
tok[Parser::token::TOKEN_SEQUENCE]=string("TOKEN_SEQUENCE"); 
tok[Parser::token::TOKEN_OF]=string("TOKEN_OF"); 
tok[Parser::token::TOKEN_KEY]=string("TOKEN_KEY"); 
tok[Parser::token::TOKEN_FREQ]=string("TOKEN_FREQ"); 
tok[Parser::token::TOKEN_COUNT]=string("TOKEN_COUNT"); 
tok[Parser::token::TOKEN_VAR]=string("TOKEN_VAR"); 
tok[Parser::token::TOKEN_ACTION]=string("TOKEN_ACTION"); 
tok[Parser::token::TOKEN_IS]=string("TOKEN_IS"); 
tok[Parser::token::TOKEN_IN]=string("TOKEN_IN"); 
tok[Parser::token::TOKEN_ALWAYS]=string("TOKEN_ALWAYS"); 
tok[Parser::token::TOKEN_HTTP_HOST]=string("TOKEN_HTTP_HOST"); 
tok[Parser::token::TOKEN_HTTP_URL]=string("TOKEN_HTTP_URL"); 
tok[Parser::token::TOKEN_HTTP_URI]=string("TOKEN_HTTP_URI"); 
tok[Parser::token::TOKEN_HTTP_URL_HOST]=string("TOKEN_HTTP_URL_HOST"); 
tok[Parser::token::TOKEN_HTTP_HEADER_HOST]=string("TOKEN_HTTP_HEADER_HOST"); 
tok[Parser::token::TOKEN_PROGRAM]=string("TOKEN_PROGRAM"); 
tok[Parser::token::TOKEN_SET_LIST_ID]=string("TOKEN_SET_LIST_ID"); 
tok[Parser::token::TOKEN_SET_MYSQL]=string("TOKEN_SET_MYSQL"); 
tok[Parser::token::TOKEN_SET_LIST_IPSTAR]=string("TOKEN_SET_LIST_IPSTAR"); 
tok[Parser::token::TOKEN_SET_LIST_URLSTAR]=string("TOKEN_SET_LIST_URLSTAR"); 
tok[Parser::token::TOKEN_SET_LIST_TIMING]=string("TOKEN_SET_LIST_TIMING"); 
tok[Parser::token::TOKEN_VR_DEF]=string("TOKEN_VR_DEF"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN]=string("TOKEN_VR_DEF_WHEN"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION]=string("TOKEN_VR_DEF_ACTION"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS]=string("TOKEN_VR_DEF_WHEN_AGENT_ID_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN]=string("TOKEN_VR_DEF_WHEN_AGENT_ID_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS]=string("TOKEN_VR_DEF_WHEN_OBS_NAME_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN]=string("TOKEN_VR_DEF_WHEN_OBS_NAME_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS]=string("TOKEN_VR_DEF_WHEN_OBS_EVENT_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN]=string("TOKEN_VR_DEF_WHEN_OBS_EVENT_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS]=string("TOKEN_VR_DEF_WHEN_IPPORT_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_IPPORT_IS_IN]=string("TOKEN_VR_DEF_WHEN_IPPORT_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_METHOD_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_HOST_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_HOST_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_URL_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_URL_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_URI_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_URI_IS_IN"); 
tok[Parser::token::TOKEN_VR_EMPTY]=string("TOKEN_VR_EMPTY"); 
tok[Parser::token::TOKEN_AR_DEF]=string("TOKEN_AR_DEF"); 
tok[Parser::token::TOKEN_AR_DEF_WHEN]=string("TOKEN_AR_DEF_WHEN"); 
tok[Parser::token::TOKEN_AR_DEF_ACTION]=string("TOKEN_AR_DEF_ACTION"); 
tok[Parser::token::TOKEN_CAT]=string("TOKEN_CAT"); 
tok[Parser::token::TOKEN_OBS_EVENT]=string("TOKEN_OBS_EVENT"); 
tok[Parser::token::TOKEN_NET_TCP_CONNECT]=string("TOKEN_NET_TCP_CONNECT"); 
tok[Parser::token::TOKEN_NET_TCP_ACCEPT]=string("TOKEN_NET_TCP_ACCEPT"); 
tok[Parser::token::TOKEN_NET_TCP_SEND]=string("TOKEN_NET_TCP_SEND"); 
tok[Parser::token::TOKEN_NET_TCP_RECV]=string("TOKEN_NET_TCP_RECV"); 
tok[Parser::token::TOKEN_NET_TCP_CLOSE]=string("TOKEN_NET_TCP_CLOSE"); 
tok[Parser::token::TOKEN_NET_TCP_IPSRC]=string("TOKEN_NET_TCP_IPSRC"); 
tok[Parser::token::TOKEN_NET_TCP_IPDST]=string("TOKEN_NET_TCP_IPDST"); 
tok[Parser::token::TOKEN_NET_TCP_SESID]=string("TOKEN_NET_TCP_SESID"); 
tok[Parser::token::TOKEN_NET_RESULT]=string("TOKEN_NET_RESULT"); 
tok[Parser::token::TOKEN_OBS_NAME]=string("TOKEN_OBS_NAME"); 
tok[Parser::token::TOKEN_OBS_TIME]=string("TOKEN_OBS_TIME"); 
tok[Parser::token::TOKEN_OBS_OBSID]=string("TOKEN_OBS_OBSID"); 
tok[Parser::token::TOKEN_OBS_STIME]=string("TOKEN_OBS_STIME"); 
tok[Parser::token::TOKEN_OBS_ETIME]=string("TOKEN_OBS_ETIME"); 
tok[Parser::token::TOKEN_OBS_TAG]=string("TOKEN_OBS_TAG"); 
tok[Parser::token::TOKEN_DATA]=string("TOKEN_DATA"); 
tok[Parser::token::TOKEN_MAKE]=string("TOKEN_MAKE"); 
tok[Parser::token::TOKEN_AR]=string("TOKEN_AR"); 
tok[Parser::token::TOKEN_ENABLE]=string("TOKEN_ENABLE"); 
tok[Parser::token::TOKEN_DISABLE]=string("TOKEN_DISABLE"); 
tok[Parser::token::TOKEN_MYSQL_APPEND]=string("TOKEN_MYSQL_APPEND"); 
tok[Parser::token::TOKEN_MYSQL_APPEND_VARIABLES]=string("TOKEN_MYSQL_APPEND_VARIABLES"); 
tok[Parser::token::TOKEN_MYSQL_UPDATE]=string("TOKEN_MYSQL_UPDATE"); 
tok[Parser::token::TOKEN_HTTP_METHOD]=string("TOKEN_HTTP_METHOD"); 
tok[Parser::token::TOKEN_HTTP_METHOD_GET]=string("TOKEN_HTTP_METHOD_GET"); 
tok[Parser::token::TOKEN_HTTP_METHOD_POST]=string("TOKEN_HTTP_METHOD_POST"); 
tok[Parser::token::TOKEN_HTTP_METHOD_HEAD]=string("TOKEN_HTTP_METHOD_HEAD"); 
tok[Parser::token::TOKEN_HTTP_METHOD_PUT]=string("TOKEN_HTTP_METHOD_PUT"); 
tok[Parser::token::TOKEN_HTTP_METHOD_DELETE]=string("TOKEN_HTTP_METHOD_DELETE"); 
tok[Parser::token::TOKEN_HTTP_METHOD_TRACE]=string("TOKEN_HTTP_METHOD_TRACE"); 
tok[Parser::token::TOKEN_HTTP_METHOD_OPTIONS]=string("TOKEN_HTTP_METHOD_OPTIONS"); 
tok[Parser::token::TOKEN_HTTP_METHOD_CONNECT]=string("TOKEN_HTTP_METHOD_CONNECT"); 
tok[Parser::token::TOKEN_HTTP_METHOD_PATCH]=string("TOKEN_HTTP_METHOD_PATCH"); 
tok[Parser::token::TOKEN_HTTP_COOKIE]=string("TOKEN_HTTP_COOKIE"); 
tok[Parser::token::TOKEN_HTTP_HEADER]=string("TOKEN_HTTP_HEADER"); 
tok[Parser::token::TOKEN_HTTP_PORT]=string("TOKEN_HTTP_PORT"); 
tok[Parser::token::TOKEN_HTTP_QUERY]=string("TOKEN_HTTP_QUERY"); 
tok[Parser::token::TOKEN_HTTP_DATA]=string("TOKEN_HTTP_DATA"); 
tok[Parser::token::TOKEN_HTTP_ANSWER_DATA]=string("TOKEN_HTTP_ANSWER_DATA"); 
tok[Parser::token::TOKEN_HTTP_ANSWER]=string("TOKEN_HTTP_ANSWER"); 
tok[Parser::token::TOKEN_HTTP_ANSWER_CODE]=string("TOKEN_HTTP_ANSWER_CODE"); 
tok[Parser::token::TOKEN_HTTP_ANSWER_HEADER]=string("TOKEN_HTTP_ANSWER_HEADER"); 
tok[Parser::token::TOKEN_HTTP_ANSWER_COOKIE]=string("TOKEN_HTTP_ANSWER_COOKIE"); 
tok[Parser::token::TOKEN_SA_OS]=string("TOKEN_SA_OS"); 
tok[Parser::token::TOKEN_SA_LOGGED_USER]=string("TOKEN_SA_LOGGED_USER"); 
tok[Parser::token::TOKEN_SA_PROCESS_NAME]=string("TOKEN_SA_PROCESS_NAME"); 
tok[Parser::token::TOKEN_SA_PROCESS_ATTACH]=string("TOKEN_SA_PROCESS_ATTACH"); 
tok[Parser::token::TOKEN_EXISTS]=string("TOKEN_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_COOKIE_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_COOKIE_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_HEADER_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_HEADER_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_QUERY_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_QUERY_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_CODE_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_COOKIE_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_HEADER_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_DATA_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_DATA_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_DATA_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS]=string("TOKEN_VR_DEF_WHEN_HTTP_ANSWER_DATA_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS]=string("TOKEN_VR_DEF_WHEN_OBS_TAG_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN]=string("TOKEN_VR_DEF_WHEN_OBS_TAG_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS]=string("TOKEN_VR_DEF_WHEN_OBS_TAG_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_URL_HOST_ID_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS]=string("TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN]=string("TOKEN_VR_DEF_WHEN_HTTP_HEADER_HOST_ID_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS]=string("TOKEN_VR_DEF_WHEN_SA_OS_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_OS_IS_IN]=string("TOKEN_VR_DEF_WHEN_SA_OS_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS]=string("TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN]=string("TOKEN_VR_DEF_WHEN_SA_PROCESS_NAME_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS]=string("TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN]=string("TOKEN_VR_DEF_WHEN_SA_USER_NAME_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS]=string("TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN]=string("TOKEN_VR_DEF_WHEN_NET_TCP_IPSRC_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_NET_RESULT_IS]=string("TOKEN_VR_DEF_WHEN_NET_RESULT_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS]=string("TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN]=string("TOKEN_VR_DEF_WHEN_NET_TCP_SESID_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_GET]=string("TOKEN_VR_DEF_WHEN_VREP_GET"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_EXISTS]=string("TOKEN_VR_DEF_WHEN_VREP_EXISTS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_FREQ]=string("TOKEN_VR_DEF_WHEN_VREP_FREQ"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_COUNT]=string("TOKEN_VR_DEF_WHEN_VREP_COUNT"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS]=string("TOKEN_VR_DEF_WHEN_VREP_KEY_IS"); 
tok[Parser::token::TOKEN_VR_DEF_WHEN_VREP_KEY_IS_IN]=string("TOKEN_VR_DEF_WHEN_VREP_KEY_IS_IN"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_CREATE]=string("TOKEN_VR_DEF_ACTION_VREP_CREATE"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_MCREATE]=string("TOKEN_VR_DEF_ACTION_VREP_MCREATE"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE]=string("TOKEN_VR_DEF_ACTION_VREP_PLUS_CREATE"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_FREQ]=string("TOKEN_VR_DEF_ACTION_VREP_FREQ"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_COUNT]=string("TOKEN_VR_DEF_ACTION_VREP_COUNT"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_KEY]=string("TOKEN_VR_DEF_ACTION_VREP_KEY"); 
tok[Parser::token::TOKEN_VR_DEF_ACTION_VREP_KV_PAIR]=string("TOKEN_VR_DEF_ACTION_VREP_KV_PAIR"); 
tok[Parser::token::TOKEN_KV]=string("TOKEN_KV"); 
tok[Parser::token::TOKEN_ACTION_KEY_LIST]=string("TOKEN_ACTION_KEY_LIST"); 
tok[Parser::token::TOKEN_ACTION_DATA_LIST]=string("TOKEN_ACTION_DATA_LIST"); 
tok[Parser::token::TOKEN_ACTION_ENABLE_VR]=string("TOKEN_ACTION_ENABLE_VR"); 
tok[Parser::token::TOKEN_ACTION_DISABLE_VR]=string("TOKEN_ACTION_DISABLE_VR"); 
tok[Parser::token::TOKEN_ACTION_ENABLE_AR]=string("TOKEN_ACTION_ENABLE_AR"); 
tok[Parser::token::TOKEN_ACTION_DISABLE_AR]=string("TOKEN_ACTION_DISABLE_AR"); 
tok[Parser::token::TOKEN_ACTION_SETADD]=string("TOKEN_ACTION_SETADD"); 
tok[Parser::token::TOKEN_ACTION_SETDEL]=string("TOKEN_ACTION_SETDEL"); 
tok[Parser::token::TOKEN_ACTION_SETVAR]=string("TOKEN_ACTION_SETVAR"); 
tok[Parser::token::TOKEN_OBS_EVENT_ACM_LIGHT]=string("TOKEN_OBS_EVENT_ACM_LIGHT"); 
tok[Parser::token::TOKEN_VR_DEF_IS_A_CLAUSES]=string("TOKEN_VR_DEF_IS_A_CLAUSES"); 
tok[Parser::token::TOKEN_VR_DEF_IS_A_EXPR]=string("TOKEN_VR_DEF_IS_A_EXPR"); 
tok[Parser::token::TOKEN_VR_DEF_IS_A_TERM]=string("TOKEN_VR_DEF_IS_A_TERM"); 
tok[Parser::token::TOKEN_VR_DEF_IS_A_FACTOR]=string("TOKEN_VR_DEF_IS_A_FACTOR"); 
tok[Parser::token::TOKEN_VR_DEF_IS_A_WHEN_WITH]=string("TOKEN_VR_DEF_IS_A_WHEN_WITH"); 
tok[Parser::token::TOKEN_VR_DEF_IS_LOP_OR]=string("TOKEN_VR_DEF_IS_LOP_OR"); 
tok[Parser::token::TOKEN_VR_DEF_IS_LOP_AND]=string("TOKEN_VR_DEF_IS_LOP_AND"); 
tok[Parser::token::TOKEN_VR_DEF_IS_LOP_NEXT]=string("TOKEN_VR_DEF_IS_LOP_NEXT"); 
tok[Parser::token::TOKEN_VR_DEF_IS_LISTA_WHEN]=string("TOKEN_VR_DEF_IS_LISTA_WHEN"); 
tok[Parser::token::TOKEN_VR_DEF_IS_LISTA_WITH]=string("TOKEN_VR_DEF_IS_LISTA_WITH"); 
tok[Parser::token::TOKEN_BASIC_ACCESS_RULES]=string("TOKEN_BASIC_ACCESS_RULES"); 
tok[Parser::token::TOKEN_EXTENDED_ACCESS_RULES]=string("TOKEN_EXTENDED_ACCESS_RULES"); 
tok[Parser::token::TOKEN_ACCESS_OPERATIONS]=string("TOKEN_ACCESS_OPERATIONS"); 
tok[Parser::token::TOKEN_ADD_HEADER_TO_ANSWER]=string("TOKEN_ADD_HEADER_TO_ANSWER"); 
tok[Parser::token::TOKEN_ADD_HEADER_TO_REQUEST]=string("TOKEN_ADD_HEADER_TO_REQUEST");

tok[Parser::token::TOKEN_REPLACE_HEADER_IN_REQUEST]=string("TOKEN_REPLACE_HEADER_IN_REQUEST");
tok[Parser::token::TOKEN_REPLACE_HEADER_IN_ANSWER]=string("TOKEN_REPLACE_HEADER_IN_ANSWER");

tok[Parser::token::TOKEN_ADD_V_HEADER_TO_V_ANSWER]=string("TOKEN_ADD_V_HEADER_TO_V_ANSWER"); 
tok[Parser::token::TOKEN_ADD_V_HEADER_TO_V_REQUEST]=string("TOKEN_ADD_V_HEADER_TO_V_REQUEST"); 
tok[Parser::token::TOKEN_ADD_COOKIE_TO_ANSWER]=string("TOKEN_ADD_COOKIE_TO_ANSWER"); 
tok[Parser::token::TOKEN_ADD_COOKIE_TO_REQUEST]=string("TOKEN_ADD_COOKIE_TO_REQUEST"); 
tok[Parser::token::TOKEN_ADD_V_COOKIE_TO_V_ANSWER]=string("TOKEN_ADD_V_COOKIE_TO_V_ANSWER"); 
tok[Parser::token::TOKEN_ADD_V_COOKIE_TO_V_REQUEST]=string("TOKEN_ADD_V_COOKIE_TO_V_REQUEST"); 
tok[Parser::token::TOKEN_PATTERN]=string("TOKEN_PATTERN"); 
tok[Parser::token::TOKEN_VR_DEF_CALL_LISP_FUNCTION]=string("TOKEN_CALL_LISP"); 
    }
    auto found = tok.find ( i );
    if ( found == tok.end() )
    {
        cout << __func__   << ": " << "error searching token: " << i << endl;
        exit ( -1 );
    }
    else
    {
        return tok[i];
    }
}

//string VA_ItemTypesToAGENTS(VA_ItemTypes t)
//{
/*
    AGENT_ID //Non serve sempre presente
    SA_OS,   //@OPERATIVE_SYSTEM                        @OPERATIVE_SYSTEM con il simbolo <<
    SA_LOGGED_USER, //non serve, arriva sempre       @LOGGED_USR
    SA_PROCESS_NAME, //non serve arriva sempre       @PROCESS_NAME
    OBS_NAME,        //non serve attiva sempre
    OBS_TIME,       //quello prima dell'evento
    OBS_OBSID,      //id-sessione
    OBS_STIME,      //non le manda più
    OBS_ETIME,      //non le manda più
    OBS_TAG,        //messo nei tag
    OBS_EVENT,      //@EVENT==PROCESS_ATTACH => PROC_ATTACH_EVENT, @EVENT==SEND => POSTSEND_EVENT @EVENT==RECV => POSTRECV_EVENT, @EVENT==CONNECT => POSTCONNECT_EVENT, @EVENT=CLOSE => PRECLOSE_EVENT + acmlight
    NET_TCP_IPDST,  //SEMPRE             nella cond: IP_ADDRESS oppure IP_ADDRESS_1 o 2=>lungo 2 o 3=>lungo 3 IP_SRC (1,2,3,4 indicano la lunghezza) IP_DST (sempre 1,2,3,4)
    NET_TCP_IPSRC,  //SEMPRE                e poi la porta è TCP_SERV (una delle due porte)
    NET_RESULT,     //SEMPRE                        @ESITO==0/1
    NET_SESID,      //SEMPRE(come OBSID)
    HTTP_METHOD,    //@HTTP_METHOD GET POST ...
    HTTP_HOST,      //@HTTP_HOST
    HTTP_URL,           //@HTTP_URI_URL
    HTTP_HEADER_HOST,   //@HTTP_HOST
    HTTP_URL_HOST,      //@HTTP_URI_HOST
    HTTP_COOKIES,       //@HTTP_COOKIE //Quello che usa il browser
    HTTP_HEADERS,       //@HTTP_<nome header> == o nulla
    HTTP_QUERY,         //@HTTP_URI_DATA_<nome var> == o nulla
    HTTP_DATA_POST,     //@HTTP_DATA_<nome var> ==  o nulla
    HTTP_ANSWER_CODE,       //@HTTP_RESPONSE_CODE
    HTTP_ANSWER_HEADERS,    //@HTTP_<nome-header> sull'event recv
    HTTP_ANSWER_COOKIES,    //@HTTP_Set-Cookie
    HTTP_ANSWER_DATA,       //@HTTP_DATA_<nomevar> con event recv

    poi in action abbiamo tag e win
*/
//}

//set<string> GetHosts(VAengine & vaeng, VA_NodeTree * r)
//{
//    set<string> hosts;
//    for (auto &it: r->criteria)
//    {
//        switch (it.itype)
//        {
//            case VA_ItemTypes::HTTP_HOST:
//            {
//                if (it.op == VA_OpTypes::EQUAL)
//                {
//                    hosts.insert(it.op1);
//                }
//                else if (it.op == VA_OpTypes::IN)
//                {
//                    auto uss = vaeng.vasets.dataset()[it.op1];
//                    for (auto jt: uss->original_data())
//                    {
//                        hosts.insert(jt);
//                    }
//                }
//            }
//        }
//    }
//    for (int i=0; i<r->children.size(); ++i)
//    {
//        set<string> ss = GetHosts(vaeng, r->children[i]);
//        hosts.insert(ss.begin(), ss.end());
//    }
//    return hosts;
//}

//set<string> BuildObservationsFromTree(VAengine &vaeng, VA_NodeTree * r)
//{
//    set<string> hosts;
//    for (auto &it: r->criteria)
//    {
//        switch (it.itype)
//        {
//            case VA_ItemTypes::HTTP_HOST:
//            {
//                if (it.op == VA_OpTypes::EQUAL)
//                {
//                    hosts.insert(it.op1);
//                }
//                else if (it.op == VA_OpTypes::IN)
//                {
//                    auto uss = vaeng.vasets.dataset()[it.op1];
//                    for (auto jt: uss->original_data())
//                    {
//                        hosts.insert(jt);
//                    }
//                }
//            }
//        }
//    }
//    for (int i=0; i<r->children.size(); ++i)
//    {
//        BuildObservationsFromTree(vaeng, r->children[i]);
//    }
//    return hosts;
//    switch (it.itype)
//    {
//
//        case VA_ItemTypes::AGENT_ID: return "@AGENT_ID";
//        case VA_ItemTypes::HTTP_ANSWER_CODE: return "@HTTP_RESPONSE_CODE";
//        case VA_ItemTypes::HTTP_ANSWER_COOKIES: return "@HTTP_Set-Cookie";
//        case VA_ItemTypes::HTTP_ANSWER_DATA: return "@HTTP_DATA";
//        case VA_ItemTypes::HTTP_ANSWER_HEADERS: return "HTTP_ANSWER_HEADERS";
//        case VA_ItemTypes::HTTP_COOKIES: return "VA_ItemTypes::HTTP_COOKIES";
//        case VA_ItemTypes::HTTP_DATA_POST: return "VA_ItemTypes::HTTP_DATA_POST";
//        case VA_ItemTypes::HTTP_HEADERS: return "VA_ItemTypes::HTTP_HEADERS";
//        case VA_ItemTypes::HTTP_HEADER_HOST: return "VA_ItemTypes::HTTP_HEADER_HOST";
//        case VA_ItemTypes::HTTP_HOST: return "VA_ItemTypes::HTTP_HOST";
//        case VA_ItemTypes::HTTP_METHOD: return "VA_ItemTypes::HTTP_METHOD";
//        case VA_ItemTypes::HTTP_QUERY: return "VA_ItemTypes::HTTP_QUERY";
//        case VA_ItemTypes::HTTP_URL: return "VA_ItemTypes::HTTP_URL";
//        case VA_ItemTypes::HTTP_URL_HOST: return "VA_ItemTypes::HTTP_URL_HOST";
//        case VA_ItemTypes::NET_RESULT: return "VA_ItemTypes::NET_RESULT";
//        case VA_ItemTypes::NET_SESID: return "VA_ItemTypes::NET_SESID";
//        case VA_ItemTypes::NET_TCP_IPDST: return "VA_ItemTypes::NET_TCP_IPDST";
//        case VA_ItemTypes::NET_TCP_IPSRC: return "VA_ItemTypes::NET_TCP_IPSRC";
//        case VA_ItemTypes::NONE: return "VA_ItemTypes::NONE";
//        case VA_ItemTypes::OBS_ETIME: return "VA_ItemTypes::OBS_ETIME";
//        case VA_ItemTypes::OBS_EVENT: return "VA_ItemTypes::OBS_EVENT";
//        case VA_ItemTypes::OBS_NAME: return "VA_ItemTypes::OBS_NAME";
//        case VA_ItemTypes::OBS_OBSID: return "VA_ItemTypes::OBS_OBSID";
//        case VA_ItemTypes::OBS_STIME: return "VA_ItemTypes::OBS_STIME";
//        case VA_ItemTypes::OBS_TAG: return "VA_ItemTypes::OBS_TAG";
//        case VA_ItemTypes::OBS_TIME: return "VA_ItemTypes::OBS_TIME";
//        case VA_ItemTypes::SA_LOGGED_USER: return "VA_ItemTypes::SA_LOGGED_USER";
//        case VA_ItemTypes::SA_OS: return "VA_ItemTypes::SA_OS";
//        case VA_ItemTypes::SA_PROCESS_NAME: return "VA_ItemTypes::SA_PROCESS_NAME";
//        case VA_ItemTypes::VARIABLE: return "VA_ItemTypes::VARIABLE";
//        default:
//            cout << __func__   << "SWITCH ERROR AT " << __func__ << endl;
//            return "";
//    }
//
//    }
//}

//string BuildObservations(VAengine &vaeng)
//{
//    set<string> hosts;
//    for (auto & vr: vaeng.varules)
//    {
//        set<string> ss = GetHosts(vaeng, vr.second.root);
//        hosts.insert(ss.begin(), ss.end());
//    }
//
//    for (auto it: hosts)
//        cout << __func__   << it << endl;
//
//    return "";
//}

