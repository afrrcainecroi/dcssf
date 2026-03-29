#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <stack>

//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"

#include "mtfafs.h"
#include "mtfa_fs3.h"
#include "mtfa_utils.h"
#include "mtfa_db.h"
#include "mtfa_find.h"

#include "mtfa_shared_mutex.h"
#include "listaeventi.h"

using namespace VaParser;
using namespace std;

class PHash
{
private:
    int counter;
    unordered_map<string, int> mapper;
public:
    PHash();
    ~PHash();
    void clear();
    int insert ( const string & );
    int erase ( const string & );
    int find ( const string & );
    int size();
    PHash & operator= ( const PHash & asn );
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class IntSet
{
private:
    unordered_set<int> data;
public:
    IntSet();
    ~IntSet();
    void clear();
    int insert ( const int v );
    int erase ( const int v );
    int find ( const int v );
    int size();
    IntSet & operator= ( const IntSet & asn );
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load* bs );
};

enum class VA_SetTypes : int
{
    Undefined=0, SetOfStrings, SetOfIpS, SetOfUrlS, SetofTimes , Dictionary, /*SetOfPars, SetOfKVStrings*/
};

enum class VA_ActionTypes : int
{
    NOP=0,
    ISSUE_VREP,
    ISSUE_MVREP,    //Il memory validation report
    ENABLE_VR,
    ENABLE_AR,
    DISABLE_VR,
    DISABLE_AR,
    ADD_VALUE_TO_SET,
    DEL_VALUE_FROM_SET,
    SET_VAR_OF_AR,
    MYSQL_APPEND,
    CALL_LISP_FUNCTION,   //per le chiamate al LISP sia nelle condition, sia nelle action
//    RESET_VAR_OF_AR,
};

enum class VA_LogicalTypes : int
{
    BEFORE=0,
    AND,
    OR,
    ITEM,
};

enum class VA_OpTypes : int
{
    NONE=0,
    EQUAL,
    IN,
    EXISTS,
    CALL_LISP_FUNCTION,   //per le chiamate al LISP sia nelle condition, sia nelle action
};

enum class VA_OpModifiers : int
{
    NONE=0,
    INDEX
};
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
*/


enum class VA_ItemTypes : int
{
    NONE=0,
    AGENT_ID,
    SA_OS,
    SA_LOGGED_USER,
    SA_PROCESS_NAME,
    OBS_NAME,
    OBS_TIME,
    OBS_OBSID,
    OBS_STIME,
    OBS_ETIME,
    OBS_TAG,
    OBS_EVENT,
    NET_TCP_IPDST,
    NET_TCP_IPSRC,
    NET_RESULT,
    NET_SESID,
    HTTP_METHOD,
    HTTP_HOST,
    HTTP_URL,
    HTTP_URI,
    HTTP_HEADER_HOST,
    HTTP_URL_HOST,
    HTTP_COOKIES,
    HTTP_HEADERS,
    HTTP_QUERY,
    HTTP_DATA_POST,
    HTTP_ANSWER_CODE,
    HTTP_ANSWER_HEADERS,
    HTTP_ANSWER_COOKIES,
    HTTP_ANSWER_DATA,
    VARIABLE,
//    VR_KV,
    AR_TIME,    //il tempo range delle AR
    CAT,
    CALL_LISP_FUNCTION,

    VA_ItemTypes_MaxIndex   //l'indice massimo!
};

enum class KAM_ActionTypes : int
{
    NOP=0,
    TRACE,
    TCP_REDIRECT_VSTRING,
    TCP_REDIRECT_STRING,
    TCP_REDIRECT_VHOST,
    TCP_REDIRECT_HOST,
    HTTP_VREDIRECT,
    HTTP_REDIRECT,
//     HTTPS_VREDIRECT,
//     HTTPS_REDIRECT,
    VBANDWIDTH,
    BANDWIDTH,
    VANSWER,
    ANSWER,
    SCM_MANAGE,
    SCM_GIVE,
    VSCM_MANAGE,
    VSCM_GIVE,
    HEADER_ANSWER_ADD,
    HEADER_REQUEST_ADD,
    HEADER_ANSWER_VADD,
    HEADER_REQUEST_VADD,
    HEADER_REQUEST_REPLACE,
    HEADER_ANSWER_REPLACE,
    COOKIE_ANSWER_ADD,
    COOKIE_REQUEST_ADD,
    COOKIE_ANSWER_VADD,
    COOKIE_REQUEST_VADD,
    ADD_VALUE_TO_SET,
    DEL_VALUE_FROM_SET,
    CALL_LISP_FUNCTION,
    //    VEXPIRY,
//    EXPIRY,
};

/* PEr la geatione di map o set con tipi enum
    //Provo a costruire un set di tipo
    unordered_set<int> sva;
    sva.insert(static_cast<std::size_t>(VA_ItemTypes::AGENT_ID));
 */

//enum class VA_Set_Of_ValuesType : int {
//    NONE=0,
//    MSS,
//    VB,
//    USS
//};
//

#define DO_VASET_LOCK(__m) lock_guard<recursive_mutex> lock(__m)
class VASet
{
public:
    recursive_mutex rmtx;
    union {
        mtfa_sequence_star *mss;
        vector<bool> * vb;
        unordered_set<string> * uss;
        unordered_map<string, string> * umss;
    };

    unordered_set<string> operative_data;   //usato solo per le mss e per la restore dei set
    unordered_map<string, string> operative_data_pairs;   //usato solo per la restore dei set
    vector<string> pars;  //for data got from mysql
    
    vector<string> initial_values;  //valido per tutti, per resettare eventualmente
    vector<pair<string, string>> initial_values_pairs;
    
    unordered_map<string, time_t> added_items;
    unordered_map<string, time_t> removed_items;
    
    unordered_map<string, pair<string, time_t>> added_items_pairs;
    unordered_map<string, pair<string, time_t>> removed_items_pairs;
    
    VASet();
    ~VASet();

    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );

    VA_SetTypes stype;
    string sname;
};

class VASets
{
private:
    static const int T=5; //timeout to check for expired items (added or removed from SET)

public:
    PHash ph_pages; //globale per tutti i set
    static const int url_deep = 6;
    //Contenitore degli insiemi gestiti dalla VA
    //Ogni insieme ha un tipo, gli eventuali parametri di creazione (mysql), un nome e i valori che sono sempre stringhe!
    //nota che per costruzione, l'ultimo elemento della tupla corrisponde ai dati messi all'ingresso e quindi c'è sempre.
    //la stringa nella tuple corrisponde ai parametri di query MYSQL
    unordered_map<string, VASet*> datasets;
    VASets();
    ~VASets();

    void RebuildVaSets ( bool redo_queries = false );
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );

    bool AddSetofStringPair ( const string &name, list<pair<string, string>> initial_values, list<string> db_pars );
    bool AddSetofString ( const string &name, list<string> initial_values, list<string> db_pars );
    bool AddSetofIpS ( const string &name, list<string> initial_values, list<string> db_pars );
    bool AddSetofUrlS ( const string &name, list<string> initial_values, list<string> db_pars );
    bool AddSetofTime ( const string &name, list<string> initial_values, list<string> db_pars );

//    bool Add(const string name, const VA_SetTypes itype, const string parameters, const unordered_set<string>& data);
//    bool Add(const string name, const VA_SetTypes itype, const string parameters, const list<string>& data);

    vector<bool> * domaketimerange ( const unordered_set<string>& data );

    mtfa_sequence_star * domakestarip ( const unordered_set< string > & data );
    mtfa_sequence_star * domakestarurl ( const unordered_set<string>& data );
    vector<unsigned short> UrlStringToVS ( const string s, const string splitter="/");
    vector<unsigned short> IpPortStringToVS ( string s );

    static void PurgeAddedAndRemovedValues ( VASets * vasets );

    //Archive flag only used for timeouts conditions!
    void AddItem ( const string &setname, const string &item_value, time_t duration, bool archive ,bool from_vr_action);
    void AddItem ( const string &setname, const string &item_key, const string &item_value, time_t duration, bool archive, bool from_vr_action);
    void DelItem ( const string &setname, const string &item_value, time_t duration, bool archive, bool from_vr_action);
    bool CheckItem ( const string &setname, const string &item_key, string & item_value);
};

typedef list<tuple<VA_ItemTypes, string, pcre2_code*, string>> T_Cat;
typedef tuple<VA_ItemTypes, string, pcre2_code *, string, T_Cat> T_Single_Vrep;
typedef list<T_Single_Vrep> T_Vrep;

//Posso avere una lista di azioni!
typedef struct __VAaction__ {
    VA_ActionTypes atype;
    
    //per i validation report ho il problema della CAT. Devo quindi estendere il vrep aggiungendo come nella WITH la CAT e il pattern extractor
    T_Vrep vrep;

    string val1;
    string val2;
    string val3;
    VA_ItemTypes observable;
    string obs_index;
    T_Cat cat;  //e ho aggiunto anche la cat per le VA Actions
    SCM func; //per le chiamate al lisp
    
    //e anche pattern e replace
    pcre2_code * pattern;
    string replace;

    /*
     * 1)   CREATE VREP ident_or_string action_variable_list
     *      val1 = vrep name
     * 2)   ENABLE VR ident_or_string
            DISABLE VR ident_or_string
            ENABLE AR ident_or_string
            DISABLE AR ident_or_string
     *      val1 = vr/ar name
     * 3)   ADD observable TO SET ident_or_string
     *      DEL observable TO SET ident_or_string
     *      observable: observable
     *      var1: set name
     *      var2 if observable is ident or string
     *      obs_index: observable index (when needed)
     * 4)   SET ident_or_string(ar name) AT ident_or_string EQUAL observable
     *      var1: ar name
     *      var2: variable name
     *      observable: observable
     *      obs_index: observable index (when needed)
    */
    __VAaction__(): atype(VA_ActionTypes::NOP), observable(VA_ItemTypes::NONE)
    {
        vrep.clear();
        val1.clear();
        val2.clear();
        val3.clear();
        obs_index.clear();
        cat.clear();
        pattern = nullptr;
        replace.clear();
    }
} VAaction;


typedef struct __VAwith__ {
    string vname;   //nome della variabile
    VA_ItemTypes itype;  //elemento da considerare
    VA_OpModifiers modifier; //c'è un indice?
    string op1; //indice
    pcre2_code * pattern;
    string replace;
    T_Cat cats;
} VAwith;

typedef struct __VAcriteria__
{
    bool donot;
    VA_OpTypes op;
    VA_ItemTypes itype;
    VA_OpModifiers modifier;
    string op1;
    string op2;
    pcre2_code * pattern;
    string replace;
    T_Cat cats;
    SCM func;
    
    __VAcriteria__(): op(VA_OpTypes::NONE), itype(VA_ItemTypes::NONE), modifier(VA_OpModifiers::NONE), donot(false)
    {
        op1.clear();
        op2.clear();
        pattern = nullptr;
        replace.clear();
    }
    ~__VAcriteria__()
    {
//          if (pattern)
//              mtfa_free_pattern(pattern);
//          pattern = nullptr;
    }
} VAcriteria;

class VA_NodeTree;  //forward

typedef struct __VA_NodeTreeRunTime__ {
    VA_NodeTree* original;
    vector<__VA_NodeTreeRunTime__*> children;
    vector<bool> children_done;
    __VA_NodeTreeRunTime__* father;
    int position_on_father;
} VA_NodeTreeRunTime;

//Quest'albero lo rappresentiamo come un vettore che contiene un albero binario completo.
class VA_NodeTree
{
private:
public:
    VA_NodeTree()
    {
        children.clear();
        criteria.clear();
        with.clear();
        op = VA_LogicalTypes::ITEM;
    };
    ~VA_NodeTree()
    {
        for ( auto it: children ) {
            if ( it ) {
                delete it;
            }
        }
        children.clear();
        criteria.clear();
        with.clear();
    };

    //I figli
    vector<VA_NodeTree*> children;

    //Il tipo di nodo
    VA_LogicalTypes op; //Indica il tipo di nodo: and, or, before, criteria

    //in caso di nodo ITEM
    vector<VAcriteria> criteria;     //I criteri
    vector<VAwith> with;             //La generazione delle variabili
    vector<VAaction> actions;       //Le operazioni che devo compiere per ogni match!

    //Le funzioni
    VA_NodeTree* SimplifyNodeTree ( VA_NodeTree*r ); //riduce la complessità delle operazioni (linearizza l'albero divalutazione)
    void Print ( int lev, VA_NodeTree*r );
};

VA_NodeTreeRunTime * CloneForRunTime ( VA_NodeTree*r );
void DeleteRunTimeNodeTRee ( VA_NodeTreeRunTime * );

typedef struct __ValidationRule__ {
    bool enabled;
    string name;
    VA_NodeTree* root;
    __ValidationRule__()
    {
        enabled = true;
        name.clear();
        root=nullptr;
    };
    //Mi ricordo, ora per ora, quante volte la VR è stata verificata
    //unsigned long long matched[]
} ValidationRule;

typedef struct __KAMaction__ {
    KAM_ActionTypes action;
    string var_name;
    string default_value;
    string last_value;
    string last_value_1;
    //string last_value_2;

    //Per ssl
    string server_cert;
    string client_method;
    string client_algo;
    string hostname;
    string client_cert;
    string client_key;
    bool check_server_cert;

    //Per la gestione della SCM_MANAGE
    list<Command> scm_manage_params;

    //per la gestione degli insiemi in insert
    string val1;
    string val2;
    string val3;
    VA_ItemTypes observable;
    string obs_index;
    T_Cat cat;  //quando un osservable è tcat
    SCM func;

    //per la gestione delle replace
    pcre2_code * replace_pattern;
    string replace;
    __KAMaction__ ()
    {
        replace_pattern = nullptr;
    }
} KAMaction;

class AccessRule
{
//private:
//    time_t start_time;
public:
    //In fase di scrittura, posso lockare in modo esclusivo, altrimenti è uno shared lock
    mtfa::shared_mutex ar_lock;

    bool enabled;
    string name;
    vector<VAcriteria> basic_access_criteria;

    unordered_map<string, string> ar_vars;

    vector<KAMaction> access_operations;

    AccessRule();
    ~AccessRule();
    AccessRule ( const AccessRule& );

    //Mi ricordo, ora per ora, quante volte la AR è stata verificata
    //vector<unsigned long long> matched; //organizzato per minuti. Il vettore cresce indefinitivamente!!
};

typedef struct __VAobservation__ {
    VA_ItemTypes itype;
    string value;
    unordered_map<string, string> values; //per i valori unici, allora solo value, per i doppi questa struttura

     //per i valori unici, allora solo value, per i doppi questa struttura
    __VAobservation__ ( VA_ItemTypes _itype, const string & _value, const unordered_map<string, string> & _values ): itype(_itype), value(_value), values(_values)
    {
    }
} VAobservation;

typedef unordered_map<int /*VA_ItemTypes*/, VAobservation> VAobservations;

typedef struct __VAengineExecutionEnvironment__
{
    bool runned;    //per sapere se l'ho eseguito almeno una volta e quindi devo crearne uno nuovo
    time_t starting_time;
    time_t last_matching_time;
    unordered_map<string, string> varwith;     //variabili di istanza

    VA_NodeTreeRunTime * root;
    VA_NodeTreeRunTime * current_node;

    __VAengineExecutionEnvironment__()
    {
        starting_time = time ( nullptr );
        runned = false;
        current_node = nullptr;
        root = nullptr;
        varwith.clear();
        last_matching_time=0;
    }
} VAengineExecutionEnvironment;


/*
 * Vediamo in modo definitivo cosa fare per i report.
 * considera che ancora non hai messo e quindi devi farlo ora
 * le validation rules per verificare i report.
 * Ricorda infine che non hai ancora fatto la verifica dei timing.
 * Comunque questa classe non serve. Metto il famoso vettore su base minuto in ogni validation rule.
 * */
class MemoryVRData
{
public:
    time_t first_added;         //quando ho messo il primo
    long long counter;          //quanti in tutto
    time_t last_time_added;     //quando l'ultimo

    time_t last_time_1000;          //Quando è arrivato il primo degli ultimi mille
    long long last_counter_1000;    //incrementato quando counter supera questo di 1000

    time_t last_time_5000;
    long long last_counter_5000;

    time_t last_time_10000;
    long long last_counter_10000;

    MemoryVRData()
    {
        first_added=0;         //quando ho messo il primo
        counter=0;          //quanti in tutto
        last_time_added=0;     //quando l'ultimo

        last_time_1000=0;          //Quando è arrivato il primo degli ultimi mille
        last_counter_1000=0;    //incrementato quando counter supera questo di 1000

        last_time_5000=0;
        last_counter_5000=0;

        last_time_10000=0;
        last_counter_10000=0;
    }
};

typedef unordered_map<string, string> T_um_ss;

//TODO: aggiungere gestione della concorrenza in scrittura per tutte le strutture (separatamente?)

/*
 * Come si fanno i lock
 * 
 * Lock della classe vaengine 
 *      la classe è normalmente aperta in shared lock (tutti possono leggere)
 *      quando devo modificare qualcosa di generale (aggiungere/eliminare/stoppare una vr oppure una ar oppure un set)
 *      allora metto il lock in scrittura.
 * 
 * */
class VAengine
{
public:
    
    //In fase di scrittura, posso lockare in modo esclusivo, altrimenti è uno shared lock
    mtfa::shared_mutex VAengine_lock;

    //archivia tutti gli elementi.
    //valuta le VR
    //unordered_map<string,
    VAengine() {}; //vasets=new VAset();};
    ~VAengine() {}; //delete vasets;};

    //Archivio degli insiemi di dati utilizzati come verificatori a run time
    VASets vasets;

    //unordered_map<string, MemoryVRData*> issued_memory_validation_reports;

    unordered_map<string, ValidationRule> varules;

    //PHash acrules_ph_pages;
    unordered_map<string, AccessRule> acrules;
    list<string> acrules_ordering;
    
    //insieme alle acces rules ho un array che mi consente di gestire la trace
    mtfa_unordered_map<string, pair<string, time_t> > trace_ds;

    static void PurgeTracedValues(mtfa_unordered_map<string, pair<string, time_t> > *);

    //Ora, a run time, posso avere più validation rules con lo stesso nome ma in stati differenti.
    mtfa_unordered_map<string, list<VAengineExecutionEnvironment>> runtime_varules;

    //Qui abbiamo le funzioni per l'engine vero e proprio. Arriva una Observation e viene verificata a fronte delle VR.

    //la verifica di coerenza!
    bool CheckCoherence();

    bool RebuildARDataSet();

    //bool Insert_For_Existence ( VA_ItemTypes t, const string & optional_index=string() );

    bool Runner ( VAobservations *obs );
    void InitRun();

    bool ExecuteCurrentInstance ( VAengineExecutionEnvironment & execenv, VAobservations *obs );

    bool CheckCriteria ( VA_NodeTree * node, T_um_ss & varwith, VAobservations *obs );

    void ExecuteActions ( VA_NodeTree * node, unordered_map<string, string> & varwith, VAobservations *obs, T_um_ss & temporary_kv_assoc);
    void AssignWiths ( VA_NodeTree * node, unordered_map<string, string> & varwith, VAobservations *obs );

    void IssueValidationReport ( string & name, T_Vrep & lvrep, unordered_map<string, string>&varwith, VAobservations *obs );
    //void IssueMemoryValidationReport ( string & name, list<pair<VA_ItemTypes, string>> & lvrep, unordered_map<string, string>&varwith, VAobservations *obs );
    //void InsertIntoTable(const string & connection_pars, list<pair<VA_ItemTypes, string>> &lvrep, unordered_map<string, string>&varwith, VAobservations *obs);
    void InsertIntoTable ( const string & connection_pars, T_Vrep & lvrep, T_um_ss & varwith, VAobservations *obs );


    void AddDelValueToSet ( VAaction action, unordered_map<string, string>&varwith, VAobservations *obs, T_um_ss & temporary_kv_assoc);
    void AddDelValueToSet ( KAMaction &action, unordered_map<string, string>&varwith, VAengine& vaeng, T_EventoStruct* bufs_in_out, string agent_id, string session_id, T_um_ss & temporary_kv_assoc);
    
    
    void SetVarOfAccessRule ( VAaction action, unordered_map<string, string>&varwith, VAobservations *obs );

    string validation_report_directory_name;

    //TODO completare serializzazione e deserializzazione
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

string VA_ItemTypesToString ( VA_ItemTypes t );
string  VA_ActionTypesToString ( VA_ActionTypes t );

string BuildObservations ( VAengine &vaeng );

set<string> BuildHostsListFromSetsAndHosts ( VAengine &vaeng );
pair<set<string>, set<string>> BuildOtherListFromCriteria ( VAengine &vaeng );

string remove_pattern(pcre2_code * pattern, const string & s, const string &r);

string DoKamRun(const string & key, const string& old_value, const string & fun, T_EventoStruct* bufs_in_out);
string KamStringEval(const string &s, T_EventoStruct* bufs_in_out, T_um_ss & temporary_kv_assoc);
string GetValueFromRunningData ( string& agent_id, string& session_id, T_EventoStruct* pes, VA_ItemTypes tipo, const string &name="" );



