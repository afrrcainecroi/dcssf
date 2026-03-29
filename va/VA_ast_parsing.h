/*
 * VA_ast_parsing.h
 *
 *  Created on: 03 ott 2016
 *      Author: arcieri
 */

#pragma once

#include <iostream>
#include <string>
#include <list>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>

//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"

#include "mtfafs.h"
#include "mtfa_fs3.h"
#include "mtfa_utils.h"
#include "mtfa_db.h"
#include "VAengine.h"

class VR_BASE_TYPE;

enum class VR_TYPE_NAMES : int
{
    NONE=0,
    //il simbolo //+ indica che è presente nella check rule
    //agente e agente remoto (lato client)
    AGENT_ID, //+
    SA_OS, //+
    SA_PROCESS_NAME, //+
    SA_LOGGED_USER, //+

    //network data (tcp), sessione e risultato operazione (send, recv, connect, accept, ...)
    NET_TCP_IPDST, //+
    NET_TCP_IPSRC, //+
    NET_RESULT, //+
    NET_SESID,

    //dati relativi all'osservazione
    OBS_NAME, //+
    OBS_EVENT, //+
    OBS_TIME,
    OBS_OBSID,
    OBS_STIME,
    OBS_ETIME,
    OBS_TAG, //+

    //http send (client -> server))
    HTTP_METHOD, //+
    HTTP_HOST, //+
    HTTP_URL, //+
    HTTP_HEADER_HOST, //+
    HTTP_URL_HOST, //+
    HTTP_COOKIES, //+
    HTTP_HEADERS, //+
    HTTP_QUERY, //+
    HTTP_DATA_POST, //+
    HTTP_DATA_SIGN,

    //http recv(server -> client)
    HTTP_ANSWER_CODE, //+
    HTTP_ANSWER_HEADERS,  //+
    HTTP_ANSWER_COOKIES,  //+
    HTTP_ANSWER_DATA,     //+
    HTTP_ANSWER_DATA_SIGN,

    //indica che siamo in presenza di una coppia di valori generici
    //che quindi a tempo di costruzione vanno forniti insieme
    VR_KV,
    AR_TIME,    //il tempo range delle AR
};

string VR_TYPE_NAMES_ToString ( VR_TYPE_NAMES t );
VR_TYPE_NAMES StringToVR_TYPE_NAMES ( string s );

/*  Componenti del validation report: è una struttura costituita come segue
 *      - nome del vrep
 *      - gli elementi interi
 *          - freq e count
 *      - con associati gli operatori e i valori che possono assumere
 *      - e un insieme di coppie (key, value)
 * */
enum class VR_VREPS_OPERATORS : int
{
    DC=0,
    GT,
    GE,
    LT,
    LE,
    EQ,
    NE
};

//Questo per indicare cosa deve contenere un VREP che una VR genera
class VR_VREP_GENERATOR
{
public:
    string name;    //il nome del validation report da generare
    list<tuple<const VR_TYPE_NAMES, const string, const string>> items;
    void Add ( const VR_TYPE_NAMES &t, const string &s="", const string &v="" )
    {
        items.push_back ( make_tuple ( t, s, v ) );
    };
    VR_VREP_GENERATOR ( const string n )
    {
        name = n;
    };
    ~VR_VREP_GENERATOR()
    {
        name.clear();
        items.clear();
    };
};

//Ogni VREP si porta appresso i suoi valori
class VR_VREP
{
public:
    string name;

    time_t inizio;

    int durata;

    //la parte frequenza
    long long freq_attuale;

    //la parte contatore
    long long cont_attuale;

    unordered_map<string, string> kv_values;
    VR_VREP()
    {
        name.clear();
        durata=0;
        freq_attuale=0;
        cont_attuale=0;
        kv_values.clear();
    };
    ~VR_VREP()
    {
        name.clear();
        freq_attuale=0;
        cont_attuale=0;
        kv_values.clear();
    };
};

//per convertire i token in stringhe
string totok ( int i );
string tabs ( int i );
void Stampa ( int l, const Command cmd );
VA_ItemTypes VRCvt ( const int t );
void ParseSetOfStrings ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfIpStar ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfMySql ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfTimings ( VAengine & vaeng, const string & name, const list<string> &args_s );

void ParseSetOfUrlStar ( VAengine & vaeng, const string & name, const list<string> &args_s );

list<VAcriteria> ParseTOKEN_VR_DEF_IS_LISTA_WHEN ( VAengine & vaeng, const Command cmd );
list<VAwith> ParseTOKEN_VR_DEF_IS_LISTA_WITH ( VAengine & vaeng, const Command cmd );
VA_NodeTree * ParseTOKEN_VR_DEF_CLAUSES ( VAengine & vaeng, const Command cmd );
VA_NodeTree * ParseTOKEN_VR_DEF ( VAengine & vaeng, const Command cmd );
vector<VAcriteria> ParseTOKEN_BASIC_ACCESS_RULES ( VAengine & vaeng, const Command cmd );
//vector<VAcriteria> ParseTOKEN_EXTENDED_ACCESS_RULES ( VAengine & vaeng, const Command cmd );
vector<VAcriteria> ParseTOKEN_IF ( VAengine & vaeng, const Command cmd );
vector<KAMaction> ParseTOKEN_ACCESS_OPERATIONS ( VAengine & vaeng, const Command cmd );
void ParseTOKEN_AR_DEF ( VAengine & vaeng, const Command cmd );
void ParseCommand ( VAengine & vaeng, const Command & cmd );
void AstParse ( VAengine & vaeng, Interpreter & interpreter );
vector<VAaction> ParseTOKEN_VR_DEF_ACTION ( VAengine & vaeng, const Command cmd );
list<string> GetFromDb ( const list<string> sql_pars );
list<pair<string, string>> GetFromDbPair ( const list<string> sql_pars );

