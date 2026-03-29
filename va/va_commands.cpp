#include <NumberTheory.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "version.h"

#include <mutex>

#include <mtfa_utils.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>

#include <string>
#include <map>

#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <tuple>
#include <unordered_set>
#include <unordered_map>

//#include <mtfa_Utilities.h>
#include <mtfafs.h>
//#include <mtfa_strings.h>

#include <mtfa_db.h>

//#include <NumberTheory.h>
//#include <LispMachine.h>
#include <ConfigFile.h>

#include "listaeventi.h"

#include "network.h"

#include <ULL_Rng.h>

#include <mtfa_threadpool.h>
#include <base64.h>

#include <mtfa_fs3.h>
#include <mtfa_ph.h>
#include <json.hpp>

#include <libguile.h>
#include <time.h>

#include "lb_user.h"
#include "va_commands.h"
#include "VA_utils.h"
#include "mtfa_utils.h"

#include "VAengine.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;

using json = nlohmann::json;

extern string LbUser_OutIp;

extern string LbUser_Client_PubKey;
extern string LbUser_Client_PrivKey;

extern string LbUser_PassTroughIp;
extern string LbUser_PassTroughPort;

extern string LbUser_LogFile;
extern string glb_BlockPage;        //the complete html answer to send back when blocking pages

//se >0 allora le url sono parziali, all'i-esimo elemento (/aaa/bb/cc => il primo è /aaa)
//se 2 allora /aaa/bb e così via!
extern int LbUser_PartialUrl;
extern bool LbUser_NewCall;       //true: nuova chiamata, false: vecchia chiamata

extern list < string > AS_Rules;         //le regole che invio agli agenti, eventualmente arricchite da altre regole definite a run time nei VReport
extern VAengine vaeng;

extern short int LbUser_VA_CommandPort;
extern string LbUser_VA_CommandIp;
extern short int LbUser_VA_HwAgentPort;
extern string LbUser_VA_HwAgentIp;
extern short int LbUser_VA_SwAgentPort;
extern string LbUser_VA_SwAgentIp;

extern string LbUser_Id_WhoAmI;
extern string LbUser_Hy_VA_Sisters;    //le sorelle di questa VA
extern string LbUser_Hy_KAM_Children;  //I KAM figli di questa VA


extern unordered_map < string, list < string >> VA_remove_from_query;

extern string LbUser_BlockMessage;      //il nome del file!

string RunCommandWithParameters ( const char* cmd );
bool NeedMoreData ( T_EventoStruct* bufs_in_out );

const string HTTP_200 = "HTTP/1.1 200 OK";
const string HTTP_400 = "HTTP/1.1 400 Bad Request";

const json j_system_error = { "answer", "system error" };

const json j_command_not_found = { "answer", "command not found" };

const json j_command_not_implemented = { "answer", "command not implemented" };

const json j_done = { "answer", "done" };

const json j_vr_name_not_given = { "answer", "vr name not given" };

const json j_ar_name_not_given = { "answer", "ar name not given" };

const json j_vr_name_not_found = { "answer", "vr name not found" };

const json j_ar_name_not_found = { "answer", "ar name not found" };

const json j_set_name_not_found = { "answer", "set <name> not found" };

//I comandi hanno una risposta fissa di tipo json: coppia codice risposta, dati risposta

//Una globale che si ricorda delle variazioni dei set ricevute dal presente tool sia esso KAM che VA
unordered_map<string, time_t> glb_set_updated;  //ricorda sia la stringa JSON, sia quando è stata definita la prima volta.

//Chiamate dalle VR. Comunicano verso il mondo esterno!
bool SendItemToSisterAndChildren ( string setname, string key, string value, time_t duration, bool add )
{
    //Costruisce la stringa di aggiornamento, json
    json update;
    update["command"] = add ? "ADD" : "DEL";
    update["set"] = setname;
    update["key"] = key;
    update["value"] = value;
    update["duration"] = duration;
    update["when"] = GetULLTime();
    auto data = update.dump();
    update["uid"] = c_hashes::sha512 ( ( char* ) data.data(), data.size() );

    //ora la invio a tutte le mie sorelle e i miei figli. Prima inserisco in glb_set_updated
    glb_set_updated[update["uid"]] = GetULLTime();

    //Da qui in poi potrebbe essere un thread!!
    auto sisters_and_children = mtfa_string_split ( LbUser_Hy_VA_Sisters + ";" + LbUser_Hy_KAM_Children, ";" );

    for ( auto& s_ipport : sisters_and_children )
    {
        try
        {
            auto ipport = mtfa_string_split ( s_ipport, ":" );
            string page, headers;
            GetPageFromUrlProxy ( ipport[0], ipport[1], "http", "get", "/?json=" + urlencode ( data ), "", "", "", "", 0, page, headers );
        }
        catch ( exception ex )
        {
            mtfa_log ( "Errore: %s\n", ex.what() );
        }
    }

    return true;
}

bool SendItemToSisterAndChildren ( string setname, string value, time_t duration, bool add )
{
    //TODO: da riparare
    return true;
    
    
    //Costruisce la stringa di aggiornamento, json
    json update;
    update["command"] = add ? "ADD" : "DEL";
    update["set"] = setname;
    //update["key"] = key;
    update["value"] = value;
    update["duration"] = duration;
    update["when"] = GetULLTime();
    auto data = update.dump();
    update["uid"] = c_hashes::sha512 ( ( char* ) data.data(), data.size() );

    //ora la invio a tutte le mie sorelle e i miei figli. Prima inserisco in glb_set_updated
    glb_set_updated[update["uid"]] = GetULLTime();

    //Da qui in poi potrebbe essere un thread!!
    auto sisters_and_children = mtfa_string_split ( LbUser_Hy_VA_Sisters + ";" + LbUser_Hy_KAM_Children, ";" );

    for ( auto& s_ipport : sisters_and_children )
    {
        try
        {
            auto ipportca = mtfa_string_split ( s_ipport, ":" );
            string page, headers;
            GetPageFromUrlProxy ( ipportca[0], ipportca[1], "https", "get", "/?json=" + urlencode ( data ), "", "", "", "", 0, page, headers,
                                  true,
                                  true,
                                  ipportca[2],
                                  LbUser_Client_PubKey,
                                  LbUser_Client_PrivKey
            );
        }
        catch ( exception ex )
        {
            mtfa_log ( "Errore: %s\n", ex.what() );
        }
    }
    return true;
}

string RunCommandWithParameters ( const char* cmd )
{
    char  buffer[1024];
    std::string result = "";
    std::shared_ptr < FILE > pipe ( popen ( cmd, "r" ), pclose );

    if ( !pipe )
    {
        throw
        std::runtime_error ( "popen() failed!" );
    }

    while ( !feof ( pipe.get() ) )
    {
        if ( fgets ( buffer, 1023, pipe.get() ) != NULL )
        {
            result += buffer;
        }
    }

    return result;
}

bool NeedMoreData ( T_EventoStruct* bufs_in_out )
{
    //verifica la sintassi del file passato nella post
    if ( bufs_in_out->s_body.size() == 0 )
    {
        //Chiede altri dati
        bufs_in_out->last_was_in_0 = false;
        bufs_in_out->need_body = true;

        //Riabilito gli eventi
        if ( bufs_in_out->p_buf_in )
        {
            bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        }

        if ( bufs_in_out->p_buf_out )
        {
            bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        }

        return true;
    }
    else
    {
        return false;
    }
}

enum class VaCommands : std::int8_t
{
    CHECK_SYNTAX = 0,
    VR_ENABLE,
    VR_DISABLE,
    AR_ENABLE,
    AR_DISABLE,
    VR_LIST,
    AR_LIST,
    SET_LIST,
    SET_GET,
    SET_ADD_VALUE,
    SET_DEL_VALUE,
    VR_ADD,
    AR_ADD,
    SET_RESYNC,
    VR_RESYNC,
    AR_RESYNC,
    QUIT,
};

static void CheckSyntax ( /*VAengine* vaeng, */T_EventoStruct* bufs_in_out )
{
    //verifica la sintassi del file passato nella post
    if ( NeedMoreData ( bufs_in_out ) )
    {
        return;
    }

    //il body è disponibile, lo utilizza
    char buf[1024];
    ssize_t iret = readlink ( "/proc/self/exe", buf, 1023 );

    if ( iret > 0 )
    {
        string filename = mtfa_FileIo::MkTemp();
        mtfa_FileIo::Write ( filename, bufs_in_out->s_body );
        string cmd = buf;
        cmd += " compile ";
        cmd += filename;
        string resp = RunCommandWithParameters ( cmd.c_str() );

        //invio la risposta
        json answer;
        answer["answer"] = resp;
        AnswerData ( HTTP_200, "", answer.dump(), bufs_in_out, true );
    }
    else
    {
        AnswerData ( HTTP_400, "", j_system_error.dump(), bufs_in_out, true );
    }
}

static void VrEnableDisable ( VAengine* vaeng, T_EventoStruct* bufs_in_out, bool isEnable )
{
    auto p_vr_name = bufs_in_out->current_query.find ( "vr-name" );

    if ( p_vr_name == bufs_in_out->current_query.end() )
    {
        AnswerData ( HTTP_400, "", j_vr_name_not_given.dump(), bufs_in_out,
                     true );
    }
    else
    {
        auto valrule = vaeng->varules.find ( p_vr_name->second );

        if ( valrule == vaeng->varules.end() )
        {
            AnswerData ( HTTP_400, "", json ( j_vr_name_not_found, json::array (
            {
                "name",
                p_vr_name->
                second
            }
            ) ).dump(), bufs_in_out, true );
        }
        else
        {
            valrule->second.enabled = isEnable;
            json answer;
            answer["name"] = p_vr_name->second;
            answer["op"] = isEnable ? "enabled" : "disabled";
            answer.push_back ( j_done );
            AnswerData ( HTTP_200, "", answer.dump(), bufs_in_out, true );
        }
    }
}

static void ArEnableDisable ( VAengine* vaeng, T_EventoStruct* bufs_in_out, bool isEnable )
{
    auto p_ar_name = bufs_in_out->current_query.find ( "ar-name" );

    if ( p_ar_name == bufs_in_out->current_query.end() )
    {
        AnswerData ( HTTP_400, "", j_ar_name_not_given.dump(), bufs_in_out,
                     true );
    }
    else
    {
        auto accrule = vaeng->acrules.find ( p_ar_name->second );

        if ( accrule == vaeng->acrules.end() )
        {
            AnswerData ( HTTP_400, "", json ( j_ar_name_not_found, json::array (
            {
                "name", p_ar_name->second
            }
            ) ).dump(), bufs_in_out, true );
        }
        else
        {
            accrule->second.enabled = isEnable;
            json answer;
            answer["name"] = p_ar_name->second;
            answer["op"] = isEnable ? "enabled" : "disabled";
            answer.push_back ( j_done );
            AnswerData ( HTTP_200, "", answer.dump(), bufs_in_out, true );
        }
    }
}

static void
VrList ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    list < string > names;

    for ( auto r : vaeng->varules )
    {
        names.push_back ( r.first );
    }

    answer["names"] = json ( names );
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void
ArList ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    list < string > names;

    for ( auto r : vaeng->acrules )
    {
        names.push_back ( r.first );
    }

    answer["names"] = json ( names );
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void SetList ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    json names;

    for ( auto r : vaeng->vasets.datasets )
    {
        names.push_back ( r.first );

        switch ( r.second->stype )
        {
            case VA_SetTypes::SetOfIpS:
            {
                json v = { r.first, "SetOfIpS" };
                names.push_back ( v );
            }
            break;
//            case VA_SetTypes::SetOfKVStrings:
//            {
//                json v={r.first, "SetOfKVStrings"};
//                names.push_back(v);
//            }
            break;

//            case VA_SetTypes::SetOfPars:
//            {
//                json v={r.first, "SetOfPars"};
//                names.push_back(v);
//            }
//                break;
            case VA_SetTypes::SetOfStrings:
            {
                json v = { r.first, "SetOfStrings" };
                names.push_back ( v );
            }
            break;

            case VA_SetTypes::Dictionary:
            {
                json v = { r.first, "Dictionary" };
                names.push_back ( v );
            }
            break;

            case VA_SetTypes::SetOfUrlS:
            {
                json v = { r.first, "SetOfUrlS" };
                names.push_back ( v );
            }
            break;

            case VA_SetTypes::SetofTimes:
            {
                json v = { r.first, "SetofTimes" };
                names.push_back ( v );
            }
            break;

            case VA_SetTypes::Undefined:
            {
                json v = { r.first, "Undefined" };
                names.push_back ( v );
            }
            break;
        }
    }

    answer["names"] = json ( names );
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void SetGet ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    json names;

    string set_name = bufs_in_out->current_query["name"];
    auto theset = vaeng->vasets.datasets.find ( set_name );

    if ( theset == vaeng->vasets.datasets.end() )
    {
        AnswerData ( HTTP_400, "", j_set_name_not_found.dump(), bufs_in_out, true );
        return;
    }

    DO_VASET_LOCK ( theset->second->rmtx );

    switch ( theset->second->stype )
    {
        case VA_SetTypes::SetOfIpS:
        {
            for ( auto it : theset->second->operative_data )
                names.push_back ( it );
        }
        break;

        case VA_SetTypes::SetOfStrings:
        {
            for ( auto it : *theset->second->uss )
                names.push_back ( it );
        }
        break;

        case VA_SetTypes::Dictionary:
        {
            for ( auto it : *theset->second->umss )
            {
                names.push_back ( it.first );
                names.push_back ( it.second );
            }
        }
        break;

        case VA_SetTypes::SetOfUrlS:
        {
            for ( auto it : theset->second->operative_data )
                names.push_back ( it );
        }
        break;

        case VA_SetTypes::SetofTimes:
        {
            for ( auto it : theset->second->operative_data )
                names.push_back ( it );
        }
        break;

        case VA_SetTypes::Undefined:
        {
            for ( auto it : theset->second->operative_data )
                names.push_back ( it );
        }
        break;
    }

    answer["values"] = names;
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void SetAddValue ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    json value;

    string set_name = bufs_in_out->current_query["name"];
    string set_key = bufs_in_out->current_query["value"];
    time_t set_time = stoull ( bufs_in_out->current_query["duration"] );
    auto theset = vaeng->vasets.datasets.find ( set_name );

    if ( theset == vaeng->vasets.datasets.end() )
    {
        AnswerData ( HTTP_400, "", j_set_name_not_found.dump(), bufs_in_out, true );
        return;
    }

    vaeng->vasets.AddItem ( set_name, set_key, set_time, true, false );

    answer["values"] = "Done";
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void SetDelValue ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    json answer;
    json value;

    string set_name = bufs_in_out->current_query["name"];
    string set_key = bufs_in_out->current_query["value"];
    time_t set_time = stoull ( bufs_in_out->current_query["duration"] );
    auto theset = vaeng->vasets.datasets.find ( set_name );

    if ( theset == vaeng->vasets.datasets.end() )
    {
        AnswerData ( HTTP_400, "", j_set_name_not_found.dump(), bufs_in_out, true );
        return;
    }

    vaeng->vasets.DelItem ( set_name, set_key, set_time, true, false );

    answer["values"] = "Done";
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}

static void SetResync ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    vaeng->vasets.RebuildVaSets ( true ); //Redo queries
    json answer = { "answer", "sets rebuilt" };
    AnswerData ( HTTP_400, "", answer.dump(), bufs_in_out, true );
}


//Per gestire i thread, fare:
////Disabilito gli eventi su questo canale
//bufferevent_disable(bufs_in_out->p_buf_in, EV_READ | EV_WRITE); // | EV_TIMEOUT);
//        cout << "POOLS: " << va_cmd_tpool->size() << endl;
//        va_cmd_tpool->enqueue
//        (
//            []
//            (
//                VAengine * pvaeng,
//                T_EventoStruct * bufs_in_out
//            ) {
//                DoManageCommands(pvaeng, bufs_in_out);
//                return;
//            },
//            &vaeng,
//            bufs_in_out
//        );


/*
 * Gestione delle richieste SVA provenienti da KAM e va
 * Questo canale deve sempre essere cifrato con doppia autenticazione.
 * Se si interrompe è a carico del client di riattivarlo
 * I comandi sono: "{\"wake-up\": {\"dest\": \"kam-01.va.org\"}}"
 * */
typedef struct __p2sva_wakeup__   //Per iniziare un colloquio con la SVA
{
    time_t when;
    string key;
    string client;  //indica il peer client di questa comunicazione
    string server;  //indica il peer server di questa comunicazione
    string cypher_seed; //Il seme di cifra da utilizzare per questa comunicazione
} T_P2SVA_WakeUp;
static unordered_map<string, T_P2SVA_WakeUp> p2sva_wakeup;

typedef struct __sva_p2p__   //Per colloquiare con un altro peer
{
    time_t when;
    string key;
    string peer1;  //indica il peer client di questa comunicazione
    string sva1;    //La sva che ha accettato il peer1
    string peer2;  //indica il peer server di questa comunicazione
    string sva2;    //La sva che ha accettato il peer2
    string cypher_seed; //Il seme di cifra da utilizzare per questa comunicazione
} T_SVA_P2P;
static unordered_map<string, T_SVA_P2P> sva_p2p;

//Tra peers e SVA
#define SVA_WAKEUP "SVA_WAKEUP"
#define SVA_ASKFOR_1 "SVA_ASKFOR_1"
#define SVA_ASKFOR_2 "SVA_ASKFOR_2"
#define P2P_DESTINATION "P2P_DESTINATION"
#define P2P_SOURCE "P2P_SOURCE"
#define SVA_ANSWER_CODE "SVA_ANSWER_CODE"
#define SVA_OK "SVA_OK"
#define SVA_KO "SVA_KO"
#define SVA_REASON "SVA_REASON"
#define SVA_SYNTAX_ERROR "SVA_SYNTAX_ERROR"
#define SVA_SSL_ERROR "SVA_SSL_ERROR"
#define SVA_UNKNOWN_REQUEST "SVA_UNKNOWN_REQUEST"
#define SVA_WAKEUP_NEEDED "SVA_WAKEUP_NEEDED"
#define SVA_KEY_NOTFOUND "SVA_KEY_NOTFOUND"
#define SVA_REQUEST "SVA_REQUEST"
#define SVA2P_KEY "SVA2P_KEY"
#define SVA2P_SEED "SVA2P_SEED"
#define P2P_SEED "P2P_SEED"
#define P2P_KEY "P2P_KEY"
#define REQUEST "REQUEST"

//Tra peer e peer
#define SVA_P2P_WAKEUP "WAKE-UP-P2P"

void DoManageSvaRequests ( /*VAengine* vaeng, */T_EventoStruct* bufs_in_out )
{
    //Questi sono tutti comandi tipo POST con content-length. Se manca il dato, chiedo di proseguire
    if ( bufs_in_out->s_body.size() == 0 )
    {
        mtfa_log ( "Need MoreData\n" );
        bufs_in_out->last_was_in_0 = false;
        bufs_in_out->need_body = true;
        return;
    }

    //Il body c'è, lo interpreto in json
    //qui io non chiudo mai la comunicazione ma lascio sempre il canale aperto
    json jparsed;

    try
    {
        jparsed = json::parse ( bufs_in_out->s_body );
    }
    catch ( exception ex )
    {
        mtfa_log ( "JSON error" );
        AnswerJsonData ( HTTP_400, "", json ( {{SVA_ANSWER_CODE, SVA_KO}, {SVA_REASON, SVA_SYNTAX_ERROR}} ).dump(), bufs_in_out, false );
        return;
    }

    /*Parsing corretto! comandi ammessi:
     * - wakeup: inizio la connessione con un mio pari
     *  - parametri: chi sono io, chi è il mio pari,
     * */
    //bufs_in_out->ssl_ctx->
    struct ssl_st* thessl = bufferevent_openssl_get_ssl ( bufs_in_out->p_buf_in );

    X509 * server_cert = SSL_get_certificate(thessl);

    auto ssl_session = SSL_get0_session(thessl);
    auto ssl_peer = SSL_SESSION_get0_peer(ssl_session);

    if ( !thessl || !ssl_session || !ssl_peer )
    {
        mtfa_log ( "SSL error" );
        AnswerJsonData ( HTTP_400, "", json ( {{SVA_ANSWER_CODE, SVA_KO}, {SVA_REASON, SVA_SSL_ERROR}} ).dump(), bufs_in_out, false );
        return;
    }

    char* cn = new char[256]();
    X509_NAME_get_text_by_NID ( X509_get_subject_name ( ssl_peer ), NID_commonName, cn, 255 );
/*
    if ( strlen ( cn ) == 0 )
    {
        mtfa_log ( "SSL error" );
        AnswerJsonData ( HTTP_400, "", json ( {{SVA_ANSWER_CODE, SVA_KO}, {SVA_REASON, SVA_SSL_ERROR}} ).dump(), bufs_in_out, false );
        return;
    }
*/
    string client = cn;
    delete[] cn;

    cn = new char[256]();
    X509_NAME_get_text_by_NID ( X509_get_subject_name ( server_cert ), NID_commonName, cn, 255 );
    string server = cn;
    delete[] cn;
    
    static unordered_map<string, int> requests2idx = {{SVA_WAKEUP, 0}, {SVA_ASKFOR_1, 1}, {SVA_ASKFOR_2, 2}, /*Solo peer 2 peer*/{SVA_P2P_WAKEUP, 3}};
    unordered_map<string, int>::iterator request_idx;
    
    auto request = jparsed.find ( SVA_REQUEST );
    if ( request == jparsed.end() || (request_idx = requests2idx.find(request.value()))==requests2idx.end())
    {
        AnswerJsonData ( HTTP_400, "", json ({
            {SVA_ANSWER_CODE, SVA_KO}, 
            {SVA_REASON, SVA_UNKNOWN_REQUEST}}
        ).dump(), bufs_in_out, false );
        return;
    }

    switch (request_idx->second)
    {
        case 0: //SVA_WAKEUP (peer1 -> sva)
        {
            /*  curl --cacert va-root/cacert.pem --cert va-01/va-01.cert.pem --key va-01/va-01.key.pem   \
                --data "{\"REQUEST\": \"SVA_WAKEUP\"}" \
                https://sva-01.va.org:60000/
             * */
            //Siamo in wakeup di un peer alla sva
            //la sorgente è indicata nel certificato
            T_P2SVA_WakeUp tcd;
            tcd.client = client;    //Il peer che ha chieto il wakeup alla SVA
            tcd.server = server;    //la SVA che ha ricevuto la richiesta (io stessa)
            tcd.key = mtfa_rand_string(32, 62); //Il nome della sessione
            tcd.when = time(nullptr);           //quando la sessione è stata attivata
            tcd.cypher_seed = mtfa_rand_string(32, 62); //la chiave di cifra da utilizzare
            p2sva_wakeup[tcd.client + "|" + tcd.server] = tcd;
            
            mtfa_log ( "%s fa wakeup con %s\n", client.c_str(), server.c_str());
            AnswerJsonData ( HTTP_200, "", json (
                {
                    {SVA_ANSWER_CODE, SVA_OK},
                    {SVA_REQUEST, SVA_WAKEUP},
                    {SVA2P_KEY, tcd.key},
                    {SVA2P_SEED, tcd.cypher_seed}
                }
            ).dump(), bufs_in_out, false );
            return;
        }
        break;
        case 1: //SVA_ASKFOR_1 (peer1 -> sva)
        {
            try
            {
                //un peer chede alla SVA di parlare con un secondo peer
                /*  curl --cacert va-root/cacert.pem --cert va-01/va-01.cert.pem --key va-01/va-01.key.pem   \
                                 --data "{\
                                 \"REQUEST\": \"SVA_ASKFOR_1\",\
                                 \"SVA2P-KEY\": \"xxxx\",\
                                 \"P2P_DESTINATION\": \"va-02.va.org\" \
                                }" \
                    https://sva-01.va.org:60000/
                */
                //C'è già una comunicazione tra peer e sva?
                auto prec = p2sva_wakeup.find(client + "|" + server);
                if (prec == p2sva_wakeup.end())
                {
                    AnswerJsonData ( HTTP_400, "", json ({
                        {SVA_ANSWER_CODE, SVA_KO}, 
                        {SVA_REASON, SVA_WAKEUP_NEEDED}}
                    ).dump(), bufs_in_out, false );
                    return;
                }
                //la chiave è corretta?
                if((*prec).second.key != jparsed[SVA2P_KEY])
                {
                    AnswerJsonData ( HTTP_400, "", json ({
                        {SVA_ANSWER_CODE, SVA_KO}, 
                        {SVA_REASON, SVA_KEY_NOTFOUND}}
                    ).dump(), bufs_in_out, false );
                    return;
                }
                //ora che ho capito che il wakeup è stato fatto, e nemmemo troppo tempo fa, prendo la chiave di cifra e la utilizzo.
                //per ora no cifra, solo protocollo SVA
                auto destination = jparsed[P2P_DESTINATION];
                T_SVA_P2P tc;
                tc.when = time(nullptr);
                tc.key = mtfa_rand_string(32, 62);;
                tc.peer1 = client;  //indica il peer client di questa comunicazione
                tc.sva1 = server;    //La sva che ha accettato il peer1
                tc.peer2 = destination;  //indica il peer server di questa comunicazione
                tc.sva2.clear();    //La sva che ha accettato il peer2
                tc.cypher_seed=mtfa_rand_string(32, 62); //Il seme di cifra da utilizzare per questa comunicazione
                sva_p2p[tc.key] = tc;
                AnswerJsonData ( HTTP_200, "", json (
                    {
                        {SVA_ANSWER_CODE, SVA_OK},
                        {SVA_REQUEST, SVA_ASKFOR_1},
                        {P2P_KEY, tc.key},
                        {P2P_SEED, tc.cypher_seed}
                    }
                ).dump(), bufs_in_out, false );
                return;
            }
            catch (exception ex)
            {
                mtfa_log("Errore: %s\n", ex.what());
                AnswerJsonData ( HTTP_400, "", json ({
                    {SVA_ANSWER_CODE, SVA_KO}, 
                    {SVA_REASON, SVA_UNKNOWN_REQUEST}}
                ).dump(), bufs_in_out, false );
                return;
            }
        }
        break;
        case 2: //SVA_ASKFOR_2 (peer2 -> sva)
        {
            try
            {
                //un peer chede alla SVA di parlare con un secondo peer che gli ha inviato un wakeup
                /*  curl --cacert va-root/cacert.pem --cert va-02/va-02.cert.pem --key va-02/va-02.key.pem   \
                                 --data "{\
                                 \"REQUEST\": \"SVA_ASKFOR_2\",\
                                 \"SVA2P-KEY\": \"xxxxxx\",\
                                 \"P2P-KEY\": \"xxxxxx\",\
                                 \"P2P_SOURCE\": \"va-01.va.org\", \
                                }" \
                                https://sva-01.va.org:60000/
            * */
                //C'è già una comunicazione tra peer e sva?
                auto prec = p2sva_wakeup.find(client + "|" + server);
                if (prec == p2sva_wakeup.end())
                {
                    AnswerJsonData ( HTTP_400, "", json ({
                        {SVA_ANSWER_CODE, SVA_KO}, 
                        {SVA_REASON, SVA_WAKEUP_NEEDED}}
                    ).dump(), bufs_in_out, false );
                    return;
                }
                //ora che ho capito che il wakeup è stato fatto, e nemmemo troppo tempo fa, prendo la chiave di cifra e la utilizzo.
                //per ora no cifra, solo protocollo SVA
                //la chiave è corretta?
                if((*prec).second.key != jparsed[SVA2P_KEY])
                {
                    AnswerJsonData ( HTTP_400, "", json ({
                        {SVA_ANSWER_CODE, SVA_KO}, 
                        {SVA_REASON, SVA_KEY_NOTFOUND}}
                    ).dump(), bufs_in_out, false );
                    return;
                }
                //Il peer chiamante ha ricevuto una key da un peer. Con questa accedo alle altre informazioni
                auto destination = jparsed[P2P_DESTINATION];
                T_SVA_P2P tc;
                tc.when = time(nullptr);
                tc.key = mtfa_rand_string(32, 62);;
                tc.peer1 = client;  //indica il peer client di questa comunicazione
                tc.sva1 = server;    //La sva che ha accettato il peer1
                tc.peer2 = destination;  //indica il peer server di questa comunicazione
                tc.sva2.clear();    //La sva che ha accettato il peer2
                tc.cypher_seed=mtfa_rand_string(32, 62); //Il seme di cifra da utilizzare per questa comunicazione
                sva_p2p[tc.key] = tc;
                AnswerJsonData ( HTTP_200, "", json (
                    {
                        {SVA_ANSWER_CODE, SVA_OK},
                        {SVA_REQUEST, SVA_ASKFOR_1},
                        {P2P_KEY, tc.key},
                        {P2P_SEED, tc.cypher_seed}
                    }
                ).dump(), bufs_in_out, false );
                return;
            }
            catch (exception ex)
            {
                mtfa_log("Errore: %s\n", ex.what());
                AnswerJsonData ( HTTP_400, "", json ({
                    {SVA_ANSWER_CODE, SVA_KO}, 
                    {SVA_REASON, SVA_UNKNOWN_REQUEST}}
                ).dump(), bufs_in_out, false );
                return;
            }
        }
        break;
        
//         //Questo caso non è gestito dalla SVA ma dai singoli peers
//         case 3: //SVA_P2P_WAKEUP (peer1 -> peer2)
//         {
//             try
//             {
//                 /*  curl --cacert va-root/cacert.pem --cert va-01/va-01.cert.pem --key va-01/va-01.key.pem   \
//                 *                --data "{\"REQUEST\": \"WAKE-UP-P2P\", \"KEY\": \"xxxxx\"}" \
//                 *                https://sva-01.va.org:60000/
//                 * */
//                 //Siamo in wakeup di un peer a un altro peer
//                 //Non ci sono certificati.
//                 mtfa_log ( "%s fa wakeup con %s\n", client.c_str(), server.c_str() );
//                 AnswerJsonData ( HTTP_200, "", json (
//                 {
//                     {SVA_ANSWER_CODE, SVA_OK},
//                     {SVA_REQUEST, SVA_WAKEUP},
//                     {SVA2P_KEY, tcd.key},
//                     {SVA2P_SEED, tcd.cypher_seed}
//                 }
//                 ).dump(), bufs_in_out, false );
//                 return;
//             }
//             catch ( exception ex )
//             {
//                 mtfa_log ( "Errore: %s\n", ex.what() );
//                 AnswerJsonData ( HTTP_400, "", json (
//                 {
//                     {SVA_ANSWER_CODE, SVA_KO},
//                     {SVA_REASON, SVA_UNKNOWN_REQUEST}
//                 }
//                 ).dump(), bufs_in_out, false );
//                 return;
//             }
//         }
//         break;
        default:
        {
            AnswerJsonData ( HTTP_400, "", json ({
                {SVA_ANSWER_CODE, SVA_KO}, 
                {SVA_REASON, SVA_UNKNOWN_REQUEST}}
            ).dump(), bufs_in_out, false );
            return;
        }          
        break;
    }
}

/**
 * Questo include sia i comandi inviati da console, sia i comandi inviati da altre VA!
 * Per ora in chiaro, poi vedremo.
 * */
void DoManageCommands ( VAengine* vaeng, T_EventoStruct* bufs_in_out )
{
    static unordered_map < string, VaCommands > command_mapper =
    {
        {"check-syntax", VaCommands::CHECK_SYNTAX},
        {"ar-add", VaCommands::AR_ADD},
        {"ar-disable", VaCommands::AR_DISABLE},
        {"ar-enable", VaCommands::AR_ENABLE},
        {"ar-list", VaCommands::AR_LIST},
        {"vr-add", VaCommands::VR_ADD},
        {"vr-disable", VaCommands::VR_DISABLE},
        {"vr-enable", VaCommands::VR_ENABLE},
        {"vr-list", VaCommands::VR_LIST},
        {"set-list", VaCommands::SET_LIST},
        {"set-get", VaCommands::SET_GET},
        {"set-add-value", VaCommands::SET_ADD_VALUE},
        {"set-del-value", VaCommands::SET_DEL_VALUE},
        {"set-resync", VaCommands::SET_RESYNC},
        {"quit", VaCommands::QUIT},
    };

    //Primo parametro: CMD
    string comando = bufs_in_out->current_query["CMD"];
    auto mapped = command_mapper[comando];

    switch ( mapped )
    {
        case VaCommands::QUIT:
            AnswerData ( HTTP_200, "", j_done.dump(), bufs_in_out, true );
            exit ( 0 );       //end, fine, fin, ...
            break;

        case VaCommands::CHECK_SYNTAX:
            CheckSyntax ( /*vaeng, */bufs_in_out );
            break;

        case VaCommands::VR_DISABLE:
        case VaCommands::VR_ENABLE:
            VrEnableDisable ( vaeng, bufs_in_out, mapped == VaCommands::VR_ENABLE );
            break;

        case VaCommands::AR_DISABLE:
        case VaCommands::AR_ENABLE:
            ArEnableDisable ( vaeng, bufs_in_out, mapped == VaCommands::VR_ENABLE );
            break;

        case VaCommands::VR_LIST:
            VrList ( vaeng, bufs_in_out );
            break;

        case VaCommands::AR_LIST:
            ArList ( vaeng, bufs_in_out );
            break;

        case VaCommands::SET_LIST:
            SetList ( vaeng, bufs_in_out );
            break;

        case VaCommands::SET_GET:
            SetGet ( vaeng, bufs_in_out );
            break;

        case VaCommands::SET_ADD_VALUE:
            SetAddValue ( vaeng, bufs_in_out );
            break;

        case VaCommands::SET_DEL_VALUE:
            SetDelValue ( vaeng, bufs_in_out );
            break;

        case VaCommands::SET_RESYNC:
            SetResync ( vaeng, bufs_in_out );
            break;

        //TODO: aggiungere comandi add per ar e vr
        case VaCommands::AR_ADD:
        case VaCommands::VR_ADD:
            AnswerData ( HTTP_400, "", j_command_not_found.dump(), bufs_in_out, true );
            break;

        //TODO: aggiungere i comandi per la resync di tutto
        case VaCommands::VR_RESYNC:
        case VaCommands::AR_RESYNC:
            AnswerData ( HTTP_400, "", j_command_not_found.dump(), bufs_in_out, true );
            break;

        default:
        {
            //E questo deve essere l'ultimo else dei comandi
            AnswerData ( HTTP_400, "", j_command_not_found.dump(), bufs_in_out, true );
        }
    }

//    //Se hai disabilitato gli eventi poiché hai chiamato i thread, allora riabilitali!
//    if (bufs_in_out->p_buf_in)
//        bufferevent_enable(bufs_in_out->p_buf_in, EV_READ | EV_WRITE); // | EV_TIMEOUT);
    return;
}

/*
    Per comunicare tra VA e KAM uso la nostra cifra.
    Il canale cifrato deve essere sempre attivo. Se si chiude, lo riapro in automatico e resetto la cifra alla data più recente tra i due peer.

    Ogni KAM partecipante conosce l'indirizzo delle sue VA di riferimento (alcune o tutte).
    Il KAM resta sempre in attesa su un canale di comunicazione che lui attiva con le sue VA per ricevere comandi di ADD o DEL.

    Una VA che con le sue VR deve alterare delle access rule (add o del di valori dagli insiemi) allora comunica questa informazione alle altre VA che la distribuiscono verso i loro KAM.
    Se un KAM riceve da una VA una comunicazione che è già stata processata (sha512 per identificare una richiesta), non la processa ulteriormente.

    Ogni VA partecipante conosce l'indirizzo delle sue VA sorelle (alcune o tutte).
    Quando le VR di una VA cambiano i set (add o del), la VA aggiorna i propri KAM e segnala alle altre VA l'avvenuta operazione.

    Resta il problema della persistenza ma questo può essere risolto dalla esistenza di più di una VA che fanno da backup delle operazioni e quindi consentono
    ai KAM di ripristinare completamente il loro stato

    Partiamo con un KAM e con una VA (separati).
 */

// void DoSynchronizeParents(VaCommands command, string setname, string value, unsigned long long duration)
// {
//     //Mette in una coda specializzata la richiesta da inviare agli altri suoi fratelli e parenti
//     //Ma come fa a sapere a chi inviare se sono tutti peer?
//     bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
//     bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
//
//     //All'interno il comando si aspetta di trovare i dati in certe variabili
//     //cout << __func__   << "Ci sono: " << glb_tpool->size() << "threads in esecuzione" << endl;
//
//     auto result = glb_tpool->enqueue (
//         []
//         (
//             mtfa_ThreadPool * p,
//          string action, //The command parameters
//          T_EventoStruct * bufs_in_out //The io pointers
//         )
//         {
//             ExecuteLispCode (
//                 action,
//                 bufs_in_out
//             );
//             return;
//         },
//         glb_tpool,
//         action, //The command parameters
//         bufs_in_out //The io pointers
//     );
// }

