#pragma once

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <openssl/ssl.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>

#include <string>
#include <map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <ULL_Rng.h>
#include <mtfa_utils.h>
#include <mtfa_find.h>

#include "parserds.h"

extern SimpleRng * simple_rng;

using namespace std;

extern std::mutex mutexSetEventoStructAllocati;
extern std::unordered_set<void*> SetEventoStructAllocati;

void WriteString ( int numf, string s );

//per la gestione dei protocolli di comunicazione
enum class Protocols : int
{
    eProtHTTP, eProtTCP, eProtSSL
};

enum class WhoAmI: int
{
    eVA,
    eKAM,
    eSuperVA,
    eNone
};

typedef struct __multipart_result__ //Il multipart http
{
    string name;
    string value;
    string data;
} T_multipart_result;

typedef struct __evento_struct__ {
    WhoAmI whoami;
    list<string> add_header_to_answer;
    list<string> add_header_to_request;
    pair<pcre2_code*, string> replace_header_in_answer;
    pair<pcre2_code*, string> replace_header_in_request;
    bool b_add_header_to_answer;
    bool b_add_header_to_request;
    bool b_replace_header_in_answer;
    bool b_replace_header_in_request;
    string tag_cookie;
    string idsonda;
    int real_data_size;
    Protocols protocollo;

    bool ServerIsSSL; //per quando mi collego a un server eProtSSL
    string server_cert;
    string server_host;
    string client_cert;
    string client_key;
    string client_method;
    string client_algo;
    bool verify_server_cert;
    string server_name;
    SSL_CTX * ssl_ctx;

    string tcpmanage;
    //string still_to_be_sent;

    T_list_rule_change * ltrc; //non viene mai eliminato, viene gestito dalle regole e quindi è staticamente allocato

    bool take_headers_sc;

    bool need_body;
    bool answer_is_needed; //true, la comunicazione proviene dal server. false, la comunicazione proviene dal LISP
    string header_cs;  //l'header completo della richiesta c->s (con \r\n\r\n)
    string header_sc;  //l'header completo della risposta s->c
    string cookie_to_be_set;
    string new_src; //i dati che vanno utilizzati al posto del buffer src.
    int content_length;
    int headersLen;
    bool last_was_in_0;
    unsigned long long UniqueId;
    struct bufferevent * p_buf_in;
    bool buf_in_freed;
    struct bufferevent * p_buf_out;
    bool buf_out_freed;
    bool headerFound_cs;
    bool headerFound_sc;
    bool toBeConnected;

    unsigned long long processing_time;

//    bool server_error;
//    bool client_error;

    unsigned long long accepted_client;
    unsigned long long connected_to_server;

    unsigned long long first_received_from_client;
    unsigned long long first_received_from_server;

    unsigned long long first_sent_to_server;
    unsigned long long last_received_from_server;

    unsigned long long closed; //or released and started the reuse

    //Indicano su quale ip e porta ero in listen
    string listen_ip;
    unsigned short listen_port;

    //indicano da quale client ricevo i dati
    string client_ip;
    unsigned short client_port;

    //Indicano a quali ip e porta mi connetto
    string server_ip;
    unsigned short server_port;

    //Indica a quale IP locale è collegato il socket connesso a server_ip
    string bind_ip;

//     string url;
//     string host;
//     string answer;

    unsigned long long data_in;
    unsigned long long data_out;
    
    unsigned long long data_given_sh_cs;

    std::map<string, string> current_query;
    std::map<string, string> current_headers;
    std::map<string, string> current_cookies;

    string s_loc_method;
    string s_loc_protocol;
    string s_loc_host;
    string s_loc_full_host; //Se nell'host c'era una porta, la tiene
    string s_loc_port;
    string s_loc_url;
    string s_loc_uri;
    string s_loc_pars;
    string s_loc_tag;
    string s_loc_user_passwd;
    string s_body;
    string mac_src;

    list<T_multipart_result> lmr;


    __evento_struct__()
    {
        Init();
//        extern std::unordered_set<void*> SetEventoStructAllocati;
        mutexSetEventoStructAllocati.lock();
        SetEventoStructAllocati.insert((void*)this);
        mutexSetEventoStructAllocati.unlock();
    }

    void Init()
    {
        whoami = WhoAmI::eNone;
        add_header_to_answer.clear();
        add_header_to_request.clear();
        b_add_header_to_answer=false;
        b_add_header_to_request=false;
        replace_header_in_answer.first=nullptr;
        replace_header_in_answer.second.clear();
        replace_header_in_request.first=nullptr;
        replace_header_in_request.second.clear();
        b_replace_header_in_answer=false;
        b_replace_header_in_request=false;

        tag_cookie.clear();
        idsonda.clear();
        real_data_size = 0; //utilizzato dalla gohome per togliere dai dati questa parte e null'altro!
        protocollo = Protocols::eProtHTTP;  //by default è http
        ServerIsSSL = false;
        server_cert.clear();
        server_host.clear();
        client_key.clear();
        client_cert.clear();
        client_method.clear();
        client_algo.clear();
        verify_server_cert = false;
        server_name.clear();
        ssl_ctx = nullptr;

        tcpmanage.clear();
        //still_to_be_sent.clear();
        ltrc = nullptr;

        take_headers_sc=false;

        answer_is_needed = false;  //significa che la comunicazione di risposta è stata generata dal fast responder
        need_body = false;
        header_sc.clear();
        header_cs.clear();
        new_src.clear();
        UniqueId = simple_rng->Next();
        cookie_to_be_set.clear();
        last_was_in_0 = false;
        data_in = 0;
        data_out = 0;
        processing_time = 0;
        p_buf_in = 0;
        buf_in_freed = false;
        p_buf_out = 0;
        buf_out_freed = false;
        headerFound_cs = false;
        headerFound_sc = false;
        toBeConnected = true;
//        server_error = false;
//        client_error = false;
        accepted_client = 0;
        connected_to_server = 0;
        first_received_from_client = 0;
        last_received_from_server = 0;
        first_received_from_server = 0;
        first_sent_to_server = 0;
        closed = 0;
        listen_ip = "";
        listen_port = 0;
        client_ip = "";
        client_port = 0;
        server_ip = "";
        server_port = 0;
        bind_ip = "";
//         url = "";
//         host = "";
//         answer = "";

        current_query.clear();
        current_headers.clear();
        current_cookies.clear();

        s_loc_method.clear();
        s_loc_protocol.clear();
        s_loc_host.clear();
        s_loc_full_host.clear();
        s_loc_port.clear();
        s_loc_url.clear();
        s_loc_uri.clear();
        s_loc_pars.clear();
        s_loc_tag.clear();
        s_loc_user_passwd.clear();
        s_body.clear();
        content_length=0;
        headersLen=0;
        mac_src.clear();
        data_given_sh_cs=0;
    }

    ~__evento_struct__()
    {
//        extern std::unordered_set<void*> SetEventoStructAllocati;
        mutexSetEventoStructAllocati.lock();
        SetEventoStructAllocati.erase((void*)this);
        mutexSetEventoStructAllocati.unlock();
    }

} T_EventoStruct;

typedef std::list<T_EventoStruct*> T_ListaEventi;

class C_ListaEventi
{
public:
    mutex * mutexListaEventi;
    T_ListaEventi * lEventi;
    bool toBeFinished;
    bool finished;
    int timeout;

    C_ListaEventi();

    ~C_ListaEventi();

    bool AddEvento ( T_EventoStruct * evento );

    //Questo \ufffd un thread
    static void PurgeEvents ( void * args );

    void StartPurge ( int secs );
    void StopPurge();

    int Size();
};
