
#include "VAengine.h"
#include "VAengine.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "version.h"

#include <mutex>
#include <thread>

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

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <event2/bufferevent_ssl.h>

#include <string>
#include <map>

#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <thread>

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

#include <libguile.h>
#include <time.h>

#include "lb_user.h"
#include "va_commands.h"

#include "VAengine.h"
#include "internalsonda.h"

#include <openssl/x509.h>

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

extern bool LbUser_VA_LocalSH_on;

extern string LbUser_OutIp;

extern string LbUser_Client_PubKey;
extern string LbUser_Client_PrivKey;

extern string LbUser_PassTroughIp;
extern string LbUser_PassTroughPort;

extern string LbUser_LogFile;
extern string glb_BlockPage; //the complete html answer to send back when blocking pages

//se >0 allora le url sono parziali, all'i-esimo elemento (/aaa/bb/cc => il primo è /aaa)
//se 2 allora /aaa/bb e così via!
extern int LbUser_PartialUrl;

extern list<string> AS_Rules; //le regole che invio agli agenti, eventualmente arricchite da altre regole definite a run time nei VReport
extern VAengine vaeng;

extern string LbUser_Initialize;    //ip and port, per inizializzare le connessioni alle va madri e sorelle
extern string LbUser_SVA_Listen;
extern unsigned short int LbUser_VA_KAM_CommandPort;
extern string LbUser_VA_KAM_CommandIp;
extern string LbUser_FromVACommands;

extern WhoAmI LbUser_Id_WhoAmI;        //VA,KAM,superva
extern string LbUser_Hy_VA_Sisters;    //le sorelle di questa VA
extern string LbUser_Hy_KAM_Children;  //I KAM figli di questa VA

extern unsigned short int LbUser_VA_HwAgentPort;
extern string LbUser_VA_HwAgentIp;
extern unsigned short int LbUser_VA_SwAgentPort;
extern string LbUser_VA_SwAgentIp;

extern string LbUser_VA_AskForUrl;
extern string LbUser_VA_TraceUrl;
extern string LbUser_VA_TraceQuery;
extern string LbUser_VA_mtfa_rnd;   //Utilizzato per sparsificare le query (evitare la cache)
extern string LbUser_VA_TraceDomain;

extern unordered_map<string, list<string>> VA_remove_from_query;

extern SCM U8;  //"u8"


//VA
void VAEvaluateValidationRules ( string& vasaid, string& vaafkey, T_EventoStruct* bufs_in_out );
bool CheckAccessRules ( VAengine& vaeng, T_EventoStruct* pes, string agent_id, string session_id, unordered_map<string, AccessRule>& acrules );
bool CheckAccessRuleItem ( VAengine& vaeng, VAcriteria& bac, T_EventoStruct* pes, string&, string& );
//extern map<string, string> glb_mimetypes;

string GetPartialUrl ( const string& url );
int RecvStream ( int s, string& str, int ms );
string ScmToStringCPP ( SCM s );
//SCM mtfa_fr_call(SCM command, SCM pars);
void* ExecuteLispCode(std::pair< char*, T_EventoStruct* >* params);
string CheckRule (
    T_EventoStruct* bufs_in_out,  //The io pointers
    int direction, //0: recvd data from client, 1: recvd data from server
    struct evbuffer* src,
    void* ctx
);

//se LbUser_PartialUrl >0 prende la url parziale!

string GetPartialUrl ( const string& url )
{
    if ( LbUser_PartialUrl == 0 )
    {
        return url;
    }

    //in caso contrario prende fino allo slash LbUser_PartialUrl+1 non incluso
    int conta = LbUser_PartialUrl;
    int len = url.size();
    const char* p = url.data();

    for ( int i = 0; i < len; ++i )
    {
        if ( *p++ == '/' )
        {
            if ( conta == 0 )
            {
                return url.substr ( 0, i );
            }
            else
            {
                conta--;
            }
        }
    }

    return url;
}

#if (EIS_LEVEL & EIS_LEVEL_2) != 0
extern string LbUser_CookieUnique;
extern string LbUser_CookieId;
extern int LbUser_TimerUnique;
#endif

#if EIS_LEVEL & EIS_LEVEL_3   //versione che genera cookie di sessione unici
extern string LbUser_UniqueSessionId; //cookie che definisce id sessione
#endif

#if EIS_LEVEL & EIS_LEVEL_7   //versione che gestisce il lisp
extern SCM LispManagement;
extern string LbUser_LspCode;
#endif

extern SCM KamRun;

string DoKamRun ( const string& key, const string& old_value, const string& fun, T_EventoStruct* bufs_in_out )
{
    //aggiungo alla funzione gli elementi di chiamata,
    scm_init_guile();
    SCM scm_v;

    scm_v = scm_call_4 (
                KamRun,
                scm_from_latin1_string ( key.c_str() ),
                scm_from_latin1_string ( old_value.c_str() ),
                scm_from_latin1_string ( fun.c_str() ),
                scm_from_pointer ( bufs_in_out, nullptr )
            );

    char* p = scm_to_latin1_string ( scm_v );
    string s = string ( p );
    free ( p );
    mtfa_log ( "lsp return value: %s\n", s.c_str() );
    return s;
}

int RecvStream ( int s, string& str, int ms )
{
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif
    }

    char buffer[256];
    int iRet;

    while ( ( iRet = recv ( s, buffer, 256, 0 ) ) > 0 )
    {
        str += string ( buffer, iRet );

        if ( str.find ( "\r\n\r\n" ) != string::npos )
        {
            break;
        }
    }

    if ( iRet >= 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*Funzioni per prendere info dalle comunicazioni ssl*/
string mtfa_ssl_get_protocol ( const SSL* ssl );
string mtfa_ssl_get_protocol ( const SSL* ssl )
{
    if ( !ssl )
        return "";

    const char* p = SSL_get_version ( ssl );

    if ( !p )
        return "";
    else
        return string ( p );
}

string mtfa_ssl_get_sessionid ( SSL* ssl );
string mtfa_ssl_get_sessionid ( SSL* ssl )
{
    if ( !ssl )
        return "";

    SSL_SESSION* p = SSL_get1_session ( ssl );

    if ( !p )
        return "";

    string id;
    unsigned int id_len;
    const unsigned char * pucid = SSL_SESSION_get0_id_context(p, &id_len);
    id.assign ( ( char* ) pucid, id_len );

    SSL_SESSION_free ( p );

    return id;
}

static string convert_ASN1TIME ( ASN1_TIME* t )
{
    int rc;
    BIO* b = BIO_new ( BIO_s_mem() );
    rc = ASN1_TIME_print ( b, t );

    char buf[128];
    rc = BIO_gets ( b, buf, 127 );
    BIO_free ( b );

    return string ( buf );
}

unordered_map<string, string> mtfa_ssl_get_x509_info ( SSL* ssl, bool localpeer );
unordered_map<string, string> mtfa_ssl_get_x509_info ( SSL* ssl, bool localpeer )
{
    unordered_map<string, string> umss;

    X509* cert;

    if ( localpeer )
        cert = SSL_get_certificate ( ssl );
    else
        cert = SSL_get_peer_certificate ( ssl );

    if ( !cert )
        return umss;

    char* subj = X509_NAME_oneline ( X509_get_subject_name ( cert ), NULL, 0 );
    char* issuer = X509_NAME_oneline ( X509_get_issuer_name ( cert ), NULL, 0 );

    string s_subj = subj;
    string s_issuer = issuer;

    OPENSSL_free ( subj );
    OPENSSL_free ( issuer );

    T_V_Split vs = mtfa_string_split ( s_subj, "/" );

    for ( auto& it : vs )
    {
        T_V_Split b = mtfa_string_split ( it, "=" );

        if ( b.size() > 0 )
            umss["subject:" + b[0]] = ( b.size() > 1 ) ? b[1] : "";
    }

    vs = mtfa_string_split ( s_issuer, "/" );

    for ( auto& it : vs )
    {
        T_V_Split b = mtfa_string_split ( it, "=" );

        if ( b.size() > 0 )
            umss["issuer:" + b[0]] = ( b.size() > 1 ) ? b[1] : "";
    }

    ASN1_INTEGER* serial = X509_get_serialNumber ( cert );
    BIGNUM* bn = ASN1_INTEGER_to_BN ( serial, NULL );
    char* tmp = BN_bn2dec ( bn );
    umss["serial_number"] = tmp;
    BN_free ( bn );
    OPENSSL_free ( tmp );

    ASN1_TIME* not_before = X509_get_notBefore ( cert );
    ASN1_TIME* not_after = X509_get_notAfter ( cert );

    umss["not_after"] = convert_ASN1TIME ( not_after );
    umss["not_before"] = convert_ASN1TIME ( not_before );

    //le altre estensioni del certificato
    const STACK_OF ( X509_EXTENSION ) *exts = X509_get0_extensions(cert); //cert->cert_info->extensions;

    int num_of_exts;

    if ( exts )
    {
        num_of_exts = sk_X509_EXTENSION_num ( exts );
    }
    else
    {
        num_of_exts = 0;
    }

    for ( int i = 0; i < num_of_exts; i++ )
    {

        X509_EXTENSION* ex = sk_X509_EXTENSION_value ( exts, i );
        ASN1_OBJECT* obj = X509_EXTENSION_get_object ( ex );

        BIO* ext_bio = BIO_new ( BIO_s_mem() );

        X509V3_EXT_print ( ext_bio, ex, 0, 0 );
//         if ( ! )
//         {
//             ASN1_OCTET_STRING_print ( ext_bio, X509_EXTENSION_get_data(ex) );
//         }

        BUF_MEM* bptr;
        BIO_get_mem_ptr ( ext_bio, &bptr );
        BIO_set_close ( ext_bio, BIO_NOCLOSE );

        // remove newlines
        int lastchar = bptr->length;

        if ( lastchar > 1 && ( bptr->data[lastchar - 1] == '\n' || bptr->data[lastchar - 1] == '\r' ) )
        {
            bptr->data[lastchar - 1] = ( char ) 0;
        }

        if ( lastchar > 0 && ( bptr->data[lastchar] == '\n' || bptr->data[lastchar] == '\r' ) )
        {
            bptr->data[lastchar] = ( char ) 0;
        }

        BIO_free ( ext_bio );

        unsigned nid = OBJ_obj2nid ( obj );

        string s_ext_name;

        if ( nid == NID_undef )
        {
            // no lookup found for the provided OID so nid came back as undefined.
            char extname[256];
            OBJ_obj2txt ( extname, 256, ( const ASN1_OBJECT* ) obj, 1 );
            s_ext_name = extname;
        }
        else
        {
            // the OID translated to a NID which implies that the OID has a known sn/ln
            const char* c_ext_name = OBJ_nid2ln ( nid );
            s_ext_name = c_ext_name;
        }

        string s_ext_value;
        s_ext_value.assign ( bptr->data, bptr->length );
        umss[s_ext_name] = s_ext_value;
    }

    return umss;
}


//Funziona se where \u00e8 un IP!!!
//int ConnectSendRecvDisconnect(const string where, int port, const string to_be_sent, string &answer) {
//    int s;
//    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (s <= 0)
//        return -1; //Errore di creazione socket!
//
//    sockaddr_in service;
//    memset(&service, 0, sizeof (service));
//
//    service.sin_family = AF_INET;
//    service.sin_addr.s_addr = inet_addr(where.c_str());
//    service.sin_port = htons(port);
//    int iRet = connect(s, (struct sockaddr *) & service, sizeof ( service));
//    if (iRet == -1) {
//        perror("errore: ");
//        close(s);
//        return -1;
//    }
//    int mtfa_SendAll(int s, string b, int ms);
//
//    iRet = mtfa_SendAll(s, to_be_sent, 10000); //Se dopo 10 secondi ancora nulla, esce!
//    if (iRet > 0) {
//        //Errore! non ho inviato tutto, manca ancora qualcosa ma non puoi farci nulla.
//        close(s);
//        return -1;
//    }
//
//    //Ora devo ricevere fino a che non prendo il doppio \r\n! siamo su WEB!
//    iRet = RecvStream(s, answer, 10000);
//    close(s);
//    if (iRet >= 0)
//        return 0;
//    else
//        return -1;
//}

string ScmToStringCPP ( SCM s)
{
    char* p = nullptr;
    size_t len = 0;
    p = ScmToStringn(s, &len);
    string sr;
    if (len > 0)
        sr.assign(p, p+len);
    else
        sr.clear();
    free(p);
    return sr;
}

void * ExecuteLispCode(pair<char*, T_EventoStruct *> * params)
{
    scm_init_guile();
    
    string action = string(params->first);
    T_EventoStruct* bufs_in_out = params->second;
    free(params->first);
    delete params;

    extern SCM LispManagement;
    int ret_len = 0;
    SCM ret_val = SCM_EOL;

    //Split di action
    T_V_Split vs = mtfa_string_split(action, ";");
    
    SCM actlist=SCM_EOL;
    for(int i=1; i<vs.size(); ++i)
    {
        string s = Base64::decode(vs[i]);
        char * p = (char*)malloc(s.size()+1);
        strcpy(p, s.c_str());
        //mtfa_log("%s\n", p);
        SCM scmstr = scm_take_locale_string(p);
        actlist = scm_cons(scmstr, actlist);
    }
    actlist = scm_reverse(actlist);
    
    SCM point = scm_from_pointer ( bufs_in_out, nullptr );

    ret_val = scm_call_2 (
                    LispManagement,
                    actlist,
                    point
            );

    ret_len = scm_to_int ( scm_length ( ret_val ) );
    
    /*La risposta è una lista di stringhe b64 in modo da farle diventare binarie, quando servisse.
     * nell'ordine abbiamo:
     * risposta, una stringa lunga 1 (A, B, M, C, R)
     * A: send answer
     * G: send file AND NOT CLOSE
     * g: send file AND CLOSE
     * B: send answer and not close
     * M: need more data
     * C: continue to another destination. If there are data, replace it completely, headers and body
     * N: continue without sending data
     * O: close channel without sending data
     */

    if ( ret_len == 0 )
    {
        //non riabilito i buffer, sto chiudendo
        static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
        SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
        
        return nullptr;
    }

    SCM scm_result = scm_car ( ret_val );
    string result = ScmToStringCPP ( scm_result );
    
    switch ( result[0] )
    {
        case 'A':   //send the answer and close the channel
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            if ( ret_len != 2 )
            {
                mtfa_log("A-Answer Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";

                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
                return nullptr;
            }

            SCM scm_answer = scm_cadr ( ret_val );
            string answer = Base64::decode ( ScmToStringCPP ( scm_answer ) );
            SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
            return nullptr;
        }
        break;

        case 'G':
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            //Give...fornisco direttamente io il path della pagina richiesta.
            SCM scm_answer = scm_cadr ( ret_val );
            string answer = ScmToStringCPP ( scm_answer );
            vector<string> vans = mtfa_string_split ( answer, ";" );

            if ( vans.size() == 0 )
            {
                mtfa_log("G-Answer Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";

                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
                return nullptr;
            }

            string filename;
            string headers;
            string http_answer;

            if ( vans.size() >= 1 )
            {
                filename = Base64::decode ( vans[0] );
            }

            if ( vans.size() >= 2 )
            {
                headers = Base64::decode ( vans[1] );
            }

            if ( vans.size() >= 3 )
            {
                http_answer = Base64::decode ( vans[2] );
            }

            AnswerFile ( ( http_answer.size() > 0 ) ? http_answer : "HTTP/1.1 200 OK", filename, headers, bufs_in_out, false );

            //mtfa_log("G-Answer\n");
            return nullptr;
        }
        break; //lo metto anche se non serve!

        case 'g':
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            //Give...fornisco direttamente io il path della pagina richiesta.
            SCM scm_answer = scm_cadr ( ret_val );
            string answer = ScmToStringCPP ( scm_answer );
            vector<string> vans = mtfa_string_split ( answer, ";" );

            if ( vans.size() == 0 )
            {
                mtfa_log("g-Answer Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";

                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
                return nullptr;
            }

            string filename;
            string headers;
            string http_answer;

            if ( vans.size() >= 1 )
            {
                filename = Base64::decode ( vans[0] );
            }

            if ( vans.size() >= 2 )
            {
                headers = Base64::decode ( vans[1] );
            }

            if ( vans.size() >= 3 )
            {
                http_answer = Base64::decode ( vans[2] );
            }

            //Sto chiudendo
            AnswerFile ( ( http_answer.size() > 0 ) ? http_answer : "HTTP/1.1 200 OK", filename, headers, bufs_in_out, true );
            return nullptr;
        }
        break; //lo metto anche se non serve!


        case 'B':   //send the answer and do not close the channel!!lo strato lisp deve mettere il keep-alive
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            if ( ret_len != 2 )
            {
                mtfa_log("Break Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
//                 mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
                return nullptr;
            }

            SCM scm_answer = scm_cadr ( ret_val );
            string answer = Base64::decode ( ScmToStringCPP ( scm_answer ) );
            SendAnswerAndNotClose ( bufs_in_out, answer.c_str(), answer.size() );
            //cerr << "Break: <" << answer << ">\n";

//             //Riabilito gli eventi
//             if ( bufs_in_out->p_buf_in )
//             {
//                 bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE );     // | EV_TIMEOUT);
//             }
// 
//             if ( bufs_in_out->p_buf_out )
//             {
//                 bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );     // | EV_TIMEOUT);
//             }

            //mtfa_log("B-Answer\n");
//             mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
            return nullptr;
        }
        break;

        case 'C':   //prosegue verso il server indicato da me!
        {
            //dopo la "C" ci sono 4 parametri: il body è in formato base64!
            if ( ret_len != 8 )
            {
                //le risposte provengono dal lisp
                bufs_in_out->answer_is_needed=false;
                
				cout << "GoHome error. Expected: "  << ret_len << "=8. ip_dest, port_dest, surl, sheaders, shost, sbody, smethod" << endl;
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
//                 mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
                return nullptr;
            }
            
            SCM tmp = scm_cdr ( ret_val );
			string ip_dest = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			string port_dest = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			string surl = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			string sheaders = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			string shost = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			
            mtfa_log("Goto %s:%s\n", ip_dest.c_str(), port_dest.c_str());

            //il body può arrivare anche come bytevector
			string sbody;
			if (scm_is_string(scm_car(tmp)))
			{
				sbody = ScmToStringCPP ( scm_car(tmp));
			} else if ( scm_is_bytevector ( scm_car(tmp) ) )
			{
				//in questo caso la cifra avviene sull'oggetto, senza duplicazioni
				int len = SCM_BYTEVECTOR_LENGTH ( scm_car(tmp) );
				char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_car(tmp) );
				sbody.assign(p, p+len);
			}
			tmp = scm_cdr(tmp);
			
			string smethod = ScmToStringCPP ( scm_car(tmp)); tmp = scm_cdr(tmp);
			
            GoHome ( bufs_in_out, ip_dest, port_dest, shost, smethod, surl, sheaders, sbody, 0, false, "", "", "", "", false, "tls", "ALL:!aNULL:RC4+RSA:+HIGH:+MEDIUM:+LOW:+EXP:+eNULL" );
            //mtfa_log("C-Answer\n");
//             mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
            return nullptr;
        }
        break;

		case 'M':   //need more data
        {
            if ( ret_len != 1 )
            {
                //le risposte provengono dal lisp
                bufs_in_out->answer_is_needed=false;

                mtfa_log("MoreData Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
//                 mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
                return nullptr;
            }

            //cerr << "Need MoreData\n";
            bufs_in_out->last_was_in_0 = false;
            bufs_in_out->need_body = true;

            //Riabilito gli eventi
//             if ( bufs_in_out->p_buf_in )
//                 bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE | EV_TIMEOUT);
//             if ( bufs_in_out->p_buf_out )
//                 bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE | EV_TIMEOUT);
            
//             bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, NULL, eventcb_b_out, bufs_in_out );
//             bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );
// 
//             bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, bufs_in_out );
//             bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
// 
            
            //in questo modo recupero se ci sono dati ancora non letti nel buffer
            readcb_b_in_0(bufs_in_out->p_buf_in, bufs_in_out);


            //se sono qui e chiedo need more data, allora significa che non ho ancora preso tutto
            //ma non serve inizializzare il body: se l'ho preso, non c'e' altro. Se non l'ho preso
            //allora potro' prenderlo.
            //bufs_in_out->need_body = true;
            //bufs_in_out->s_body.clear();
            //mtfa_log("M-Answer\n");
            //mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
            return nullptr;
        }
        break;

        case 'R':   //redirect
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            if ( ret_len != 2 )
            {
                mtfa_log("Redirect Error\n");
                static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
                SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
//                 mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
                return nullptr;
            }

            //Give...fornisco direttamente io il path della pagina richiesta.
            //cout << __func__   << "In GIVE\n";
            SCM scm_data = scm_cadr ( ret_val );
            string redirect_and_headers = ScmToStringCPP ( scm_data );
            vector<string> vred = mtfa_string_split ( redirect_and_headers, ";" );

            string redirect = Base64::decode ( vred[0] );
            string headers="";
            if (vred.size()>1)
                headers = Base64::decode ( vred[1] );
            cerr << "Redirecto to: <" << redirect << ">\n";

            AnswerRedirect ( redirect, headers, bufs_in_out );

//             //Riabilito gli eventi
//             if ( bufs_in_out->p_buf_in )
//             {
//                 bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE );     // | EV_TIMEOUT);
//             }
// 
//             if ( bufs_in_out->p_buf_out )
//             {
//                 bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );     // | EV_TIMEOUT);
//             }

            //mtfa_log("R-Answer\n");
//             mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
            return nullptr;
        }
        break;
        
        case 'N': //continue, do not send data
            return nullptr;

        case 'O': //Close channel!!
            SendAnswerAndCloseAll ( bufs_in_out, "", 0);
            return nullptr;

        default:
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            //Passo e chiudo con errore
            mtfa_log("Bad Answer\n");
            static string answer = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Close\r\n\r\n";
            SendAnswerAndCloseAll ( bufs_in_out, answer.c_str(), answer.size() );
            mtfa_log("Bad Answer return\n");
//             mtfa_log("Exiting thread: %u\n", std::this_thread::get_id());
            return nullptr;
    }
    
//     mtfa_log("!!Exiting thread: %u\n", std::this_thread::get_id());
    mtfa_log("Lisp returns but it does not be here\n");
    return nullptr;
}

//LA GESTIONE DEL POOL
//Il thread pool generale
mtfa_ThreadPool* glb_tpool;  //lisp

//la gestione del POOL di valutazioni della VA (una alla volta per carità!!!)
//Quindi uso il pool come coda ma viene eseguita da un solo thread per volta.
mtfa_ThreadPool* va_tpool = nullptr;  //C

mtfa_ThreadPool* va_cmd_tpool = nullptr;  //C: Per i comandi VA

void InitPools ( int size1 )
{
    glb_tpool = new mtfa_ThreadPool ( (size_t)size1 ); //lisp
    va_tpool = new mtfa_ThreadPool ( (size_t)1 ); //C. vale 1 per eseguire le validation rules, una per volta
    va_cmd_tpool = new mtfa_ThreadPool ( (size_t)1 ); //C. vale 1 per eseguire in parallelo i comandi, uno per volta
}

//mi dice se ho caricato i codici di base per ogni istanza di lsp
//static std::map<int, bool> codici_base_caricati;

//Gestione regole strumento base!
extern set<string> cookie_header_set;
extern PerfectHash* global_rules;
extern ImplicitDs* rules_action;
extern ImplicitDs* rules_change;
extern string s00;
extern string s01;
extern string s02;
extern string s03;

#if EIS_LEVEL & EIS_LEVEL_2

//Per la gestione delle sessioni (unicità degli username)

typedef struct __active_sessions__
{
    //Two sessions with same user identifier and different session_identifiers cannot be active
    time_t last; //last access
    string session_identifier;
    string user_identifier;
} T_ActiveSession;

static map<string, time_t> sessions_to_be_blocked; //time_t is the last time we recvd this session id
static ImplicitDs* active_sessions = nullptr;
static bool blocked_sessions_purge_stared = false;
static mutex blocked_sessions_purge_mutex;
static map<string, string> ssmap;

static void blocked_sessions_purge_function ( int LbUser_TimerUnique )
{
    while ( true )
    {
        usleep ( 1000000 * 60 * 5 ); //ogni 5 minuti!
        time_t now = time ( nullptr );
        blocked_sessions_purge_mutex.lock();
        auto it = sessions_to_be_blocked.begin();

        while ( it != sessions_to_be_blocked.end() )
        {
            if ( ( it->second - now ) > LbUser_TimerUnique )
            {
                it = sessions_to_be_blocked.erase ( it );
            }
            else
            {
                ++it;
            }
        }

        blocked_sessions_purge_mutex.unlock();
    }
}

bool CheckIfParallelSessions ( std::map<string, string>& current_cookies, T_EventoStruct* bufs_in_out )
{
    //not enabled?
    if ( LbUser_CookieUnique.size() == 0 || LbUser_CookieId.size() == 0 )
    {
        return false;
    }

    if ( active_sessions == nullptr )
    {
        active_sessions = new ImplicitDs();
    }

    if ( !blocked_sessions_purge_stared )
    {
        blocked_sessions_purge_stared = true;
        thread t ( blocked_sessions_purge_function, LbUser_TimerUnique );
        t.detach();
    }

    //cout << __func__   << "Cookies:\n";
    for ( auto it : current_cookies )
    {
        cout << __func__ << it.first << "=" << it.second << endl;
    }

    //Get session id and userid from the current cookies
    auto sessid = current_cookies.find ( LbUser_CookieUnique );

    if ( sessid == current_cookies.end() )
    {
        return false;
    }

    auto userid = current_cookies.find ( LbUser_CookieId );

    if ( userid == current_cookies.end() )
    {
        return false;
    }

    //Verify if the user id is already logged in
    time_t now = time ( nullptr );

    //Se la sessione appartiene a quelle stopped, la stoppa
    blocked_sessions_purge_mutex.lock();
    auto toBlock = sessions_to_be_blocked.find ( sessid->second );

    if (
        toBlock != sessions_to_be_blocked.end() &&
        ( now - toBlock->second ) < LbUser_TimerUnique
    )
    {
        blocked_sessions_purge_mutex.unlock();
        AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out );
        //SendAnswerAndCloseAll(bufs_in_out, glb_BlockPage.c_str(), glb_BlockPage.size());
        toBlock->second = now; //aggiorno il tempo!
        return true;
    }

    blocked_sessions_purge_mutex.unlock();

    //La sessione non è in quelle stopped, vediamo se lo stesso utente si sta presentando con una sessione diversa
    T_ActiveSession* actsess;
    auto found = active_sessions->Find ( ( char* ) userid->second.data(), userid->second.size(), ( void** ) &actsess );

    if ( found )   //utente trovato
    {
        if ( sessid->second.compare ( actsess->session_identifier ) != 0 )   //stesso utente, sessione diversa
        {
            if ( ( now - actsess->last ) < LbUser_TimerUnique )   //intervallo di tempo troppo breve tra una sessione e l'altra
            {
                //trovata una sessione duplicata! Stesso username e due sessioni differenti in un intervallo di tempo troppo breve!
                blocked_sessions_purge_mutex.lock();
                sessions_to_be_blocked[actsess->session_identifier] = now;
                sessions_to_be_blocked[sessid->second] = now;
                blocked_sessions_purge_mutex.unlock();

                AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out );
                //SendAnswerAndCloseAll(bufs_in_out, glb_BlockPage.c_str(), glb_BlockPage.size());
                actsess->last = 0; //Questo canale di sessione può essere riusato. Azzero il session id e metto now a 0.
                actsess->session_identifier.clear();
                return true;
            }
            else
            {
                //Il tempo è ammissibile per un cambio di sessione metto la precedente tra quelle bloccate e aggiorno l'attuale.
                //un thread pulirà la precedente a intervalli regolari.
                blocked_sessions_purge_mutex.lock();
                sessions_to_be_blocked[actsess->session_identifier] = now;
                blocked_sessions_purge_mutex.unlock();

                //aggiorno la sessione associata all'utente!
                actsess->last = now;
                actsess->session_identifier = sessid->second;
                return false;
            }
        }
        else
        {
            //Stesso utente, stessa sessione, posso aggiornare il tempo!
            actsess->last = now;
            return false;
        }
    }
    else
    {
        //utente non trovato, creo la sessione
        actsess = new T_ActiveSession();
        actsess->last = now;
        actsess->session_identifier = sessid->second;
        actsess->user_identifier = userid->second;
        active_sessions->Insert ( ( char* ) userid->second.data(), userid->second.size(), ( void* ) actsess );
        return false;
    }
}

#endif


//Accodamento al pool oppure nel livello base: gestione dei comandi di base

string CheckRule (
    T_EventoStruct* bufs_in_out,  //The io pointers
    int direction, //0: recvd data from client, 1: recvd data from server
    struct evbuffer* src,
    void* ctx
) //A: accept, M: manage, B: block, R: redirect, N: not matched
{
    string ip = bufs_in_out->client_ip;
    int next = global_rules->Find ( ( char* ) ip.data(), ip.size() );
    int newnext;

    if ( next == -1 )
    {
        ip = s00; //provo se esiste regola "any" ip
        next = global_rules->Find ( ( char* ) ip.data(), ip.size() );

        if ( next == -1 )
        {
            return "N";
        }
    }

    string host = to_string ( next ) + s01 + bufs_in_out->s_loc_host;
    newnext = global_rules->Find ( ( char* ) host.data(), host.size() );

    if ( newnext == -1 )
    {
        host = to_string ( next ) + s01 + s00;
        next = global_rules->Find ( ( char* ) host.data(), host.size() );

        if ( next == -1 )
        {
            return "N";
        }
    }
    else
    {
        next = newnext;
    }

    //cerr << "Check url: " << bufs_in_out->s_loc_url << endl;
    string url = to_string ( next ) + s01 + GetPartialUrl ( bufs_in_out->s_loc_url );
    newnext = global_rules->Find ( ( char* ) url.data(), url.size() );

    if ( newnext == -1 )
    {
        url = to_string ( next ) + s01 + s00;
        next = global_rules->Find ( ( char* ) url.data(), url.size() );

        if ( next == -1 )
        {
            return "N";
        }
    }
    else
    {
        next = newnext;
    }

    //Ora vanno cercati header e cookie.
    for ( auto corh : cookie_header_set )
    {
        string key, value;

        if ( tolower ( corh[0] ) == 'c' )
        {
            auto found = bufs_in_out->current_cookies.find ( corh.substr ( 1 ) );

            if ( found != bufs_in_out->current_cookies.end() )
            {
                //Cerco key e valore nella struttura dati
                //se valore è vuoto, vale s00
                key = found->first;
                value = found->second;

                //cout << __func__ << "Key: " << key << ". Value: " << value << endl;
                if ( value.size() == 0 )
                {
                    value = s00;     //questo valore va messo quando il campo è vuoto nell'http e non per indicare qualsiasi valore (campo vuoto nella specifica)
                }
            }
            else
            {
                //in questo caso key non deve esserci necessariamente
                key = s00;
                value = s00;
            }
        }
        else if ( tolower ( corh[0] ) == 'q' )
        {
            auto found = bufs_in_out->current_query.find ( corh.substr ( 1 ) );

            if ( found != bufs_in_out->current_query.end() )
            {
                //Cerco key e valore nella struttura dati
                //se valore è vuoto, vale s00
                key = found->first;
                value = found->second;

                //cout << __func__ << "Key: " << key << ". Value: " << value << endl;
                if ( value.size() == 0 )
                {
                    value = s00;     //questo valore va messo quando il campo è vuoto nell'http e non per indicare qualsiasi valore (campo vuoto nella specifica)
                }
            }
            else
            {
                //in questo caso key non deve esserci necessariamente
                key = s00;
                value = s00;
            }
        }
        else
        {
            auto found = bufs_in_out->current_headers.find ( corh.substr ( 1 ) );

            if ( found != bufs_in_out->current_headers.end() )
            {
                //Cerco key e valore nella struttura dati
                //se valore è vuoto, vale s00
                key = found->first;
                value = found->second;

                if ( value.size() == 0 )
                {
                    value = s00;     //questo valore va messo quando il campo è vuoto nell'http e non per indicare qualsiasi valore (campo vuoto nella specifica)
                }
            }
            else
            {
                //in questo caso key non deve esserci necessariamente
                key = s00;
                value = s00;
            }
        }

        string key_value = key + s02 + value;

        key_value = to_string ( next ) + s01 + key_value;
        newnext = global_rules->Find ( ( char* ) key_value.data(), key_value.size() );

        if ( newnext == -1 )
        {
            key_value = key + s02 + s00;
            key_value = to_string ( next ) + s01 + key_value;
            newnext = global_rules->Find ( ( char* ) key_value.data(), key_value.size() );

            if ( newnext == -1 )
            {
                return "N";
            }
        }

        next = newnext;
    }

    //prima delle azioni, devo memorizzare le rule changes
    T_list_rule_change* ltrc;
    bool bret = rules_change->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &ltrc );

    if ( bret )
    {
        bufs_in_out->ltrc = ltrc;

        //Se una delle ltrc riguarda il body, allora need_body va a true.
        bufs_in_out->need_body = false;

        for ( auto it : *ltrc )
        {
            if ( it->apply_to == 'B' )
            {
                bufs_in_out->need_body = true;
                break;
            }
        }
    }
    else
    {
        bufs_in_out->ltrc = nullptr;
        bufs_in_out->need_body = false;
    }

    //Ora da verificare le azioni...
    string* action;
    bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &action );

    if ( !bret )
    {
        cerr << "Got action error: " << endl;
        return "N";
    }
    else
    {
        if ( action->compare ( "ACCEPT" ) == 0 || action->compare ( "BLOCK" ) == 0 )
        {
            //cout << __func__   << "Siamo in accept\n";
            return action->substr ( 0, 1 );
        }

        if ( action->substr ( 0, 4 ).compare ( "GIVE" ) == 0 )
        {
            //cout << __func__   << "Siamo in give\n";
            vector<string> vs = mtfa_string_split ( *action, s02 );
            string sres = "G" + vs[1];
            return sres;
        }

        if ( action->substr ( 0, 6 ).compare ( "MANAGE" ) == 0 )
        {
            vector<string> vs = mtfa_string_split ( *action, s02 );
            //concateno i tre elementi dopo il comando e li metto in base64
            //cerr << "Items: " << vs.size() << endl;
            //for (auto it: vs)
            //{
            //    cerr << "<" << it << ">" << endl;
            //}
            //                if (vs.size() != 4)
            //                {
            //                    return "N";
            //                }
            string sres = "M";

            for ( auto i = 1; i < vs.size(); ++i )
            {
                sres += ";" + Base64::encode ( vs[i] );
            }

            return sres; //"M" ";" + Base64::encode(vs[1]) + ";" + Base64::encode(vs[2]) + ";" + Base64::encode(vs[3]);
            //host: azione da eseguire. R: run external command, ...
            //url:  path del comando da eseguire
            //query: not used.
        }

        //Quando non c'è prefisso allora è una redirect!
        vector<string> vs = mtfa_string_split ( *action, s02 );

        if ( vs.size() != 3 )
        {
            return "N";
        }
        else
        {
            if ( vs[1].size() == 0 && vs[2].size() == 0 )
            {
                //è una redirect particolare. l'host contiene ip e port a cui connettersi
                return "I" + vs[0];
            }
            else
            {
                if ( vs[2].size() > 0 )
                {
                    return "R" /*"http://"*/ + vs[0] + vs[1] + "?" + vs[2];
                }
                else
                {
                    return "R" /*"http://"*/ + vs[0] + vs[1];
                }
            }
        }
    }
};

static list<VAobservations*>* BuildObservationFromSAData ( VAengine& vaeng, string agent_id, string agent_key, string& data, T_EventoStruct* bufs_in_out )
{
    list<VAobservations*>* lobss = new list<VAobservations*>();

    //preparo la prima osservazione e la inserisco, vuota, nella lista!
    VAobservations* obss = new VAobservations();
    lobss->push_back ( obss );

    string body = data.erase ( data.find_last_not_of ( " :;" ) + 1 );

    static pcre2_code* reBase64 = mtfa_compile_pattern ( "^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{4}|[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)$" );
    //Questi mi arrivano da andrea!!!
    //Prima di splittare su :, splitto su ;
    //se non ci sono ;, nulla accade! altrimenti si!
    T_V_Split vs_semicolon = mtfa_string_split ( body, ";" );

    for ( auto& element : vs_semicolon )
    {
        string ipsrc;
        string ipdst;
        string s_data[2]; //primo per headers e secondo per dati
        string response_code;
        string method;
        string url;
        string uri;
        string query;
        string host;
        unordered_map<string, string> umcookie;
        unordered_map<string, string> umanswercookie; //Da gestire
        unordered_map<string, string> umhttpdata;
        unordered_map<string, string> umhttpquery;
        unordered_map<string, string> umhttpheader;
        unordered_map<string, string> umhttpanswerdata;
        unordered_map<string, string> umhttpanswerheader;

        T_V_Split vs = mtfa_string_split ( element, ":" );

        if ( vs.size() < 6 )
        {
            //Errore nella ricezione dei dati.
            cerr << __func__ << ": wrong data structure received from agent. Not accepted. " << element << endl;
            return lobss; //acmlight non lo gestisco, per ora??
        }
        if ( !mtfa_find_pattern ( vs[0], reBase64 ) ) continue;
        string s_process_name ( Base64::decode ( vs[0] ) );
        if ( !mtfa_find_pattern ( vs[1], reBase64 ) ) continue;
        string s_net_sesid ( Base64::decode ( vs[1] ) );
        if ( !mtfa_find_pattern ( vs[2], reBase64 ) ) continue;
        string s_etime ( Base64::decode ( vs[2] ) );
        if ( !mtfa_find_pattern ( vs[3], reBase64 ) ) continue;
        string s_obs_id ( Base64::decode ( vs[3] ) );
        if ( !mtfa_find_pattern ( vs[4], reBase64 ) ) continue;
        string s_time ( Base64::decode ( vs[4] ) );
        if ( !mtfa_find_pattern ( vs[5], reBase64 ) ) continue;
        string s_event ( Base64::decode ( vs[5] ) );
        //se nell'evento c'è uno spazio bianco, prendo fino allo spazio bianco
        size_t pos = s_event.find ( " " );

        if ( pos != string::npos )
        {
            s_event = s_event.substr ( 0, pos );     //GET|POST|HTTP response
        }

        //Ok, proseguo per analisi. non elaboro ACMLIGHT
        if ( s_event.compare ( "ACMLIGHT_EVENT" ) == 0 )
        {
            return lobss; //acmlight non lo gestisco, per ora??
        }

        if ( s_process_name.compare ( "MonAgent" ) == 0 )
        {
            //acquisizione dei dati da parte di sonda hardware
            //sonda hardware mgmt
            //Dagli header prendo vasaid e vaafkey

            //1, 6 e 11 recv, 2 e 7, 12 sono send
            //Questo è i localtime: 14 14 32 546  14 14 32 656 - 192.168.163.130 192.168.163.012  39943 00082  - 6 -  17046 17046 1000
            //La cosa migliore è utilizzare lo spazio come separatore dei token
            static pcre2_code* reCompiled = mtfa_compile_pattern ( "( |\\t)+" );
            s_time = mtfa_replace_matching_pattern ( s_time, reCompiled, " ", true );
            cerr << s_time << endl; //2016 10 03 16 36 51 625 16 36 51 625 -  192.168.163.134 192.168.163.012 62996 00082 -  11 -  7279 7279 1000
            //0    1  2  3  4  5  6   7  8  9  10  11 12              13              14    15    16 17 18 19
            T_V_Split vs_lt = mtfa_string_split ( s_time, " " );

            if ( vs_lt.size() < 20 )
            {
                //Errore nella ricezione dei dati.
                cerr << __func__ << ": wrong data structure received from agent. Not accepted. " << s_time << endl;
                return lobss; //acmlight non lo gestisco, per ora??
            }

            struct tm tm_inizio_op;
            int inizio_ms;
            int fine_ms;
            struct tm tm_fine_op;
            tm_inizio_op.tm_year = mtfa_atoull ( vs_lt[0] ) - 1900;
            tm_inizio_op.tm_mon = mtfa_atoull ( vs_lt[1] ) - 1;
            tm_inizio_op.tm_mday = mtfa_atoull ( vs_lt[2] );
            tm_inizio_op.tm_hour = mtfa_atoull ( vs_lt[3] );
            tm_inizio_op.tm_min = mtfa_atoull ( vs_lt[4] );
            tm_inizio_op.tm_sec = mtfa_atoull ( vs_lt[5] );
            inizio_ms = mtfa_atoull ( vs_lt[6] );
            tm_fine_op = tm_inizio_op;
            tm_fine_op.tm_hour = mtfa_atoull ( vs_lt[7] );
            tm_fine_op.tm_min = mtfa_atoull ( vs_lt[8] );
            tm_fine_op.tm_sec = mtfa_atoull ( vs_lt[9] );
            fine_ms = mtfa_atoull ( vs_lt[10] );
            
            //Toglie eventuali 0 davanti agli elementi dell'ip number
            auto removelead0 = [] ( string s ) -> string
            {
                T_V_Split vs = mtfa_string_split ( s, "." );
                for ( auto& it : vs )
                    while ( it[0] == '0' && it.length() > 1)
                    {
                        it = it.substr ( 1 );
                    }

                return vs[0] + "." + vs[1] + "." + vs[2] + "." + vs[3];
            };
            string s_ipsrc = removelead0 ( vs_lt[12] );
            string s_ipdst = removelead0 ( vs_lt[13] );
            int port_src = mtfa_atoull ( vs_lt[14] );
            int port_dst = mtfa_atoull ( vs_lt[15] );
            int opcode = mtfa_atoull ( vs_lt[17] );
            int datasize = mtfa_atoull ( vs_lt[19] );
            s_time = to_string ( mktime ( &tm_inizio_op ) );
            s_data[0].clear();
            s_data[1].clear();
            for ( int i = 6; i < vs.size(); ++i )
            {
                if ( i == 6 )
                {
                    if ( mtfa_find_pattern ( vs[i], reBase64 ) )
                        s_data[0] = Base64::decode ( vs[i] );
                    else
                        s_data[0].clear();
                }
                else
                {
                    string stmp;
                    if ( mtfa_find_pattern ( vs[i], reBase64 ) )
                        stmp = Base64::decode ( vs[i] );
                    else
                        stmp.clear();
                    if (stmp.size() >= 4)
                        s_data[1] += stmp.substr ( 4 ); //in testa ci sono \r\n\r\n
                    //cout << "s_data[1]: " << s_data[1] << endl;
                }
            }

            ipsrc = s_ipsrc + ":" + to_string ( port_src );
            ipdst = s_ipdst + ":" + to_string ( port_dst );
        }
        
        
        //codice congiunto tra ssw e shw

        //Questa è la parte fissa delle osservazioni!
        VAobservation obs_agent_id ( VA_ItemTypes::AGENT_ID, agent_id, unordered_map<string, string>() );
        VAobservation obs_process_name ( VA_ItemTypes::SA_PROCESS_NAME, s_process_name, unordered_map<string, string>() );
        VAobservation obs_net_sesid ( VA_ItemTypes::NET_SESID, s_net_sesid, unordered_map<string, string>() );
        VAobservation obs_etime ( VA_ItemTypes::OBS_ETIME, s_etime, unordered_map<string, string>() );
        VAobservation obs_obs_id ( VA_ItemTypes::OBS_OBSID, s_obs_id, unordered_map<string, string>() );
        VAobservation obs_time ( VA_ItemTypes::OBS_TIME, s_time, unordered_map<string, string>() );
        VAobservation obs_event ( VA_ItemTypes::OBS_EVENT, s_event, unordered_map<string, string>() );

        bool isASend = ( s_event.compare ( "POSTSEND_EVENT" ) == 0 ) || s_event.compare ( "GET" ) || s_event.compare ( "POST" );

        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::AGENT_ID ), obs_agent_id ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_PROCESS_NAME ), obs_process_name ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_SESID ), obs_net_sesid ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_ETIME ), obs_etime ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_OBSID ), obs_obs_id ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_TIME ), obs_time ) );
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_EVENT ), obs_event ) );

        //per sbrigarsi facciamo uno switch_case e un unordered_map
        static unordered_map<string, int> simplify_sa_events =
        {
            {"PROC_ATTACH_EVENT", 0},
            {"POSTSEND_EVENT", 1},
            {"POSTRECV_EVENT", 2},
            {"POSTCONNECT_EVENT", 3},
            {"PRECLOSE_EVENT", 4},
            {"ACMLIGHT_EVENT", 5},
            {"GET", 6}, //send
            {"POST", 7}, //recv
            {"HTTP", 8}, //recv
        };

        int simplified_sa_events = simplify_sa_events[obs_event.value];

        switch ( simplified_sa_events )
        {
            case 0:   //PROC_ATTACH_EVENT
            {
                VAobservation obs_logged_user ( VA_ItemTypes::SA_LOGGED_USER, Base64::decode ( vs[6] ), unordered_map<string, string>() );

                if ( !mtfa_find_pattern ( vs[7], reBase64 ) ) continue;
                VAobservation obs_name ( VA_ItemTypes::OBS_NAME, Base64::decode ( vs[7] ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_LOGGED_USER ), obs_logged_user ) );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_NAME ), obs_name ) );
            }
            break;

            case 1:   //POSTSEND_EVENT
            {
                if ( !mtfa_find_pattern ( vs[6], reBase64 ) ) continue;
                string ips = Base64::decode ( vs[6] );

                T_V_Split vips = mtfa_string_split ( ips, ":" );
                T_V_Split ip1 = mtfa_string_split ( vips[0], "." );
                T_V_Split ip2 = mtfa_string_split ( vips[2], "." );

                string ipsrc = to_string ( atoi ( ip1[0].c_str() ) ) + "." + to_string ( atoi ( ip1[1].c_str() ) ) + "." +
                               to_string ( atoi ( ip1[2].c_str() ) ) + "." + to_string ( atoi ( ip1[3].c_str() ) ) + ":" + to_string ( atoi ( vips[1].c_str() ) );
                string ipdst = to_string ( atoi ( ip2[0].c_str() ) ) + "." + to_string ( atoi ( ip2[1].c_str() ) ) + "." +
                               to_string ( atoi ( ip2[2].c_str() ) ) + "." + to_string ( atoi ( ip2[3].c_str() ) ) + ":" + to_string ( atoi ( vips[3].c_str() ) );

                VAobservation obs_ip_src ( VA_ItemTypes::NET_TCP_IPSRC, ipsrc, unordered_map<string, string>() );
                VAobservation obs_ip_dst ( VA_ItemTypes::NET_TCP_IPDST, ipdst, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPSRC ), obs_ip_src ) );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPDST ), obs_ip_dst ) );

                if ( !mtfa_find_pattern ( vs[8], reBase64 ) ) continue;
                VAobservation obs_net_result ( VA_ItemTypes::NET_RESULT, Base64::decode ( vs[8] ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_RESULT ), obs_net_result ) );

                if ( !mtfa_find_pattern ( vs[9], reBase64 ) ) continue;
                VAobservation obs_logged_user ( VA_ItemTypes::SA_LOGGED_USER, Base64::decode ( vs[9] ), unordered_map<string, string>() );
                if ( !mtfa_find_pattern ( vs[10], reBase64 ) ) continue;
                VAobservation obs_name ( VA_ItemTypes::OBS_NAME, Base64::decode ( vs[10] ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_LOGGED_USER ), obs_logged_user ) );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_NAME ), obs_name ) );
                cerr << __func__ << ". OBSNAME: " << Base64::decode ( vs[10] ) << endl;
            }
            break;

            case 2:   //POSTRECV_EVENT
            {
                if ( !mtfa_find_pattern ( vs[6], reBase64 ) ) continue;
                string ips = Base64::decode ( vs[6] );
                T_V_Split vips = mtfa_string_split ( ips, ":" );
                T_V_Split ip1 = mtfa_string_split ( vips[0], "." );
                T_V_Split ip2 = mtfa_string_split ( vips[2], "." );

                string ipsrc = to_string ( atoi ( ip1[0].c_str() ) ) + "." + to_string ( atoi ( ip1[1].c_str() ) ) + "." +
                               to_string ( atoi ( ip1[2].c_str() ) ) + "." + to_string ( atoi ( ip1[3].c_str() ) ) + ":" + to_string ( atoi ( vips[1].c_str() ) );
                string ipdst = to_string ( atoi ( ip2[0].c_str() ) ) + "." + to_string ( atoi ( ip2[1].c_str() ) ) + "." +
                               to_string ( atoi ( ip2[2].c_str() ) ) + "." + to_string ( atoi ( ip2[3].c_str() ) ) + ":" + to_string ( atoi ( vips[3].c_str() ) );

                VAobservation obs_ip_src ( VA_ItemTypes::NET_TCP_IPSRC, ipsrc, unordered_map<string, string>() );
                VAobservation obs_ip_dst ( VA_ItemTypes::NET_TCP_IPDST, ipdst, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPSRC ), obs_ip_src ) );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPDST ), obs_ip_dst ) );

                if ( !mtfa_find_pattern ( vs[8], reBase64 ) ) continue;
                VAobservation obs_net_result ( VA_ItemTypes::NET_RESULT, Base64::decode ( vs[8] ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_RESULT ), obs_net_result ) );

                if ( !mtfa_find_pattern ( vs[9], reBase64 ) ) continue;
                VAobservation obs_logged_user ( VA_ItemTypes::SA_LOGGED_USER, Base64::decode ( vs[9] ), unordered_map<string, string>() );

                if ( !mtfa_find_pattern ( vs[10], reBase64 ) ) continue;
                VAobservation obs_name ( VA_ItemTypes::OBS_NAME, Base64::decode ( vs[10] ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_LOGGED_USER ), obs_logged_user ) );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_NAME ), obs_name ) );
            }
            break;

            case 3:   //POSTCONNECT_EVENT
            {
            }
            break;

            case 4:   //PRECLOSE_EVENT
            {
            }
            break;

            case 5:   //ACMLIGHT_EVENT
            {
            }
            break;

            case 6: //GET event (sonda hw)
            case 7: //POST event (sonda hw)
            case 8:   //Recv (sonda hw)
            {
                VAobservation obs_ip_src ( VA_ItemTypes::NET_TCP_IPSRC, ipsrc, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPSRC ), obs_ip_src ) );

                VAobservation obs_ip_dst ( VA_ItemTypes::NET_TCP_IPDST, ipdst, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPDST ), obs_ip_dst ) );
                //ricordati che devi formare i cookie, headers e query...
            }
            break;
        }

        //se siamo in sonda software, gli eventi vanno fino a 5 incluso, altrimenti siamo in sonda hardware
        if ( simplified_sa_events <= 5 )
        {
            //sonda software!! costruisco gli altri elementi del gioco
            //tolgo ; finali dai dati, se ci sono
        if ( !mtfa_find_pattern ( vs.back(), reBase64 ) ) continue;
            string data = Base64::decode ( vs.back() );
            data = data.erase ( data.find_last_not_of ( ";" ) + 1 );

            T_V_Split vs_dynamic = mtfa_string_split ( data, ";" );
            //per sbrigarsi facciamo uno switch_case e un unordered_map
            static unordered_map<string, VA_ItemTypes> from_sa_to_va =
            {
                {"Cookie", VA_ItemTypes::HTTP_COOKIES},
                {"HOST", VA_ItemTypes::HTTP_HOST},
                {"METHOD", VA_ItemTypes::HTTP_METHOD},
                {"URI_HOST", VA_ItemTypes::HTTP_URL_HOST},
                {"URI_URL", VA_ItemTypes::HTTP_URL},
                {"@OPERATIVE_SYSTEM", VA_ItemTypes::SA_OS},
                {"RESPONSE_CODE", VA_ItemTypes::HTTP_ANSWER_CODE}
            };

            for ( int i = 0; i < vs_dynamic.size(); ++i )
            {
                //Splitto sull'= in due parti fisse
                if ( !mtfa_find_pattern ( vs_dynamic[i], reBase64 ) ) 
                    continue;
                string val = Base64::decode ( vs_dynamic[i] );
                int pos = val.find ( '=' );
                string key = val.substr ( 0, pos );
                string value = val.substr ( 1 + pos );

                //cout << __func__   << ". Given: " << val << " => " << key << "=" << value << endl;
                switch ( from_sa_to_va[key] )
                {
                    case VA_ItemTypes::HTTP_COOKIES:
                    {
                        //Questi vanno splittati ancora, prima sul "; " e poi su =
                        list<string> lck1;
                        int pos = value.find ( "; " );

                        while ( pos != string::npos )
                        {
                            lck1.push_back ( value.substr ( 0, pos ) );
                            value = value.substr ( pos + 2 );
                            pos = value.find ( "; " );
                        }

                        if ( value.size() > 0 )
                        {
                            lck1.push_back ( value );
                        }

                        //ora, cookie per cookie, lo splitto e lo archivio
                        for ( auto it : lck1 )
                        {
                            int pos = it.find ( "=" );

                            if ( pos != string::npos )
                            {
                                string ckn = it.substr ( 0, pos );
                                string ckv = urldecode ( it.substr ( pos + 1 ) );
                                umcookie[ckn] = ckv;
                            }
                        }
                    }
                    break;

                    case VA_ItemTypes::HTTP_HOST:
                    {
                        VAobservation obs ( VA_ItemTypes::HTTP_HOST, /*urldecode*/ ( value ), unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_HOST ), obs ) );
                    }
                    break;

                    case VA_ItemTypes::HTTP_METHOD:
                    {
                        VAobservation obs ( VA_ItemTypes::HTTP_METHOD, value, unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_METHOD ), obs ) );
                    }
                    break;

                    case VA_ItemTypes::HTTP_URL_HOST:
                    {
                        VAobservation obs ( VA_ItemTypes::HTTP_URL_HOST, /*urldecode*/ ( value ), unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL_HOST ), obs ) );
                    }
                    break;

                    case VA_ItemTypes::HTTP_URL:
                    {
                        VAobservation obs ( VA_ItemTypes::HTTP_URL, urldecode ( value ), unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL ), obs ) );
                    }
                    break;

                    case VA_ItemTypes::SA_OS:
                    {
                        VAobservation obs ( VA_ItemTypes::SA_OS, value, unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_OS ), obs ) );
                    }
                    break;

                    case VA_ItemTypes::HTTP_ANSWER_CODE:
                    {
                        VAobservation obs ( VA_ItemTypes::HTTP_ANSWER_CODE, value, unordered_map<string, string>() );
                        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_CODE ), obs ) );
                    }
                    break;

                    default:   //qualche altra cosa è accaduta. La stampo e la interpreto.
                    {
                        //cout << __func__ << ". " << key << "=" << value << endl;

                        if ( key.size() > 10 && key.substr ( 0, 10 ).compare ( "HTTP_DATA_" ) == 0 )
                        {
                            //E' HTTP_DATA
                            if ( isASend )
                            {
                                cerr << __func__ << ": " << StrTime() << " => " << key << ": " << value << endl;
                                umhttpdata[key.substr ( 10 )] = value; //urldecode(value);
                            }
                            else
                            {
                                umhttpanswerdata[key.substr ( 10 )] = value;     //urldecode(value);
                            }
                        }
                        else if ( key.compare ( "HTTP_DATA" ) == 0 )
                        {
                            //é HTTP_DATA completo
                            if ( isASend )
                            {
                                cerr << __func__ << ": " << StrTime() << " => " << key << ": " << value << endl;
                                umhttpdata["0_FULL_DATA"] = value; //urldecode(value);
                            }
                            else
                            {
                                umhttpanswerdata["0_FULL_DATA"] = value;     //urldecode(value);
                            }
                        }
                        else if ( key.size() > 9 && key.substr ( 0, 9 ).compare ( "URI_DATA_" ) == 0 )
                        {
                            //é URI_DATA singolo
                            umhttpquery[key.substr ( 9 )] = value; //urldecode(value);
                        }
                        else if ( key.compare ( "URI_DATA" ) == 0 )
                        {
                            //E' URI_DATA completo.
                            umhttpquery["0_FULL_QUERY"] = value; //urldecode(value);  //indica che siamo in presenza di un solo valore
                        }
                        else
                        {
                            //é HTTP_HEADER, con solo il campo header
                            if ( isASend )
                            {
                                umhttpheader[key] = value;     //urldecode(value);
                            }
                            else
                            {
                                umhttpanswerheader[key] = value;     //urldecode(value);
                            }
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            //costruzione per dati da sonda hardware
            //            unordered_map<string, string> umcookie;
            //            unordered_map<string, string> umanswercookie; //Da gestire
            //            unordered_map<string, string> umhttpdata;
            //            unordered_map<string, string> umhttpquery;
            //            unordered_map<string, string> umhttpheader;
            //            unordered_map<string, string> umhttpanswerdata;
            //            unordered_map<string, string> umhttpanswerheader;
            umcookie.clear();
            umanswercookie.clear();
            umhttpdata.clear();
            umhttpquery.clear();
            umhttpheader.clear();
            umhttpanswerdata.clear();
            umhttpanswerheader.clear();

            method.clear();
            url.clear();
            query.clear();
            host.clear();

            //Se send una cosa, se recv, altra cosa
            if ( simplified_sa_events == 8 )
            {
                //ricevuta risposta dal server
#ifndef DO_NOT_LOG
                cerr << __func__ << ":" << StrTime() << " => " << "s_data[0] = " << s_data[0] << endl;
                cerr << __func__ << ":" << StrTime() << " => " << "s_data[1] = " << s_data[1] << endl;
#endif
                cout << __func__ << ":" << StrTime() << " => " << "s_data[0] = " << s_data[0] << endl;
                cout << __func__ << ":" << StrTime() << " => " << "s_data[1] = " << s_data[1] << endl;
                if ( s_data[0].size() > 0 )
                {
                    //abbiamo i dati di risposta nel formato HTTP/1.1 200 OK!
                    //splitto per righe e tolgo i doppi fine riga
                    T_V_Split all_answer = mtfa_string_split2 ( s_data[0], "\r\n" );

                    //essendo una risposta, dalla prima riga tolgo "HTTP/1.x "
                    response_code = all_answer[0].substr ( 9 );

                    //Cerchiamo gli answer cookie, ecc
                    for ( int i = 1; i < all_answer.size(); ++i )
                    {
                        //i cookie vanno a parte
                        static pcre2_code* reCompiled = mtfa_compile_pattern ( "[Ss][eE][tT]-[Cc][oO][oO][kK][iI][eE]: " );

                        if ( mtfa_find_pattern ( all_answer[i], reCompiled ) )
                        {
                            //è una set-cookie, prendo la seconda parte fino al ;
                            string sc = all_answer[i].substr ( 12 ); //Tolgo "Set-Cookie: "
                            size_t pos = sc.find ( ";" );

                            if ( pos == string::npos )
                            {
                                //+ una setcookie pura
                            }
                            else
                            {
                                //tolgo quello che viene doopo
                                sc = sc.substr ( 0, pos );
                            }

                            //ora prendo il "="
                            pos = sc.find ( "=" );

                            if ( pos == string::npos )
                            {
                                umanswercookie[sc] = "";     //no so cosa dire, prendo tutto come nome
                            }
                            else
                            {
                                umanswercookie[sc.substr ( 0, pos )] = sc.substr ( pos + 1 );
                            }
                        }
                        else
                        {
                            //è un normalissimo header!
                            cerr << __func__ << ": " << StrTime() << " => " << all_answer[i] << endl;
                            size_t pos = all_answer[i].find ( ": " );

                            if ( pos == string::npos )
                            {
                                umhttpanswerheader[all_answer[i]] = "";     //no so cosa dire, prendo tutto come nome
                            }
                            else
                            {
                                umhttpanswerheader[all_answer[i].substr ( 0, pos )] = all_answer[i].substr ( pos + 2 );
                            }
                        }
                    }

                    //e ora answer data
                    umhttpanswerdata["0_FULL_DATA"] = ( s_data[1].size() > 0 ) ? s_data[1] : "";
                    
                    cout << "umhttpanswerdata[\"0_FULL_DATA\"]: " << umhttpanswerdata["0_FULL_DATA"] << endl;

                    //poi splitto pure!!
                    T_V_Split asvs = mtfa_string_split ( s_data[1], "&" );

                    for ( auto& it : asvs )
                    {
                        if ( it.size() > 2 )
                        {
                            T_V_Split vs = mtfa_string_split ( it, "=" );
                            if (vs.size() == 2)
                                umhttpanswerdata[vs[0]] = vs[1];
                        }
                    }
                }

                VAobservation obs ( VA_ItemTypes::HTTP_ANSWER_CODE, response_code, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_CODE ), obs ) );
            }
            else
            {
                //Trasmessi dati al server
                if ( s_data[0].size() > 0 )
                {
                    //abbiamo i dati di richiesta nel formato GET /... HTTP/1.X\r\n
                    //splitto per righe e tolgo i doppi fine riga
                    cout << "s_data: " << s_data[0] << endl;
                    T_V_Split all_reqs = mtfa_string_split2 ( s_data[0], "\r\n" );

                    //Dalla prima riga prendo url e metodo
                    //cerco il primo " "
                    size_t pos = all_reqs[0].find ( " " );
                    method = all_reqs[0].substr ( 0, pos );

                    //ora uri e url insieme
                    string fulluri = all_reqs[0].substr ( pos + 1 );

                    pos = fulluri.find ( " " );
                    fulluri = fulluri.substr ( 0, pos );

                    //ora, se presente, prendo url e query
                    pos = fulluri.find ( "?" );

                    if ( pos == string::npos )
                    {
                        url = fulluri;
                    }
                    else
                    {
                        url = fulluri.substr ( 0, pos );
                        query = fulluri.substr ( pos + 1 );
                        umhttpquery["0_FULL_QUERY"] = query;
                        //e qui splitto la query
                        T_V_Split vkv = mtfa_string_split ( query, "&" );

                        for ( auto& it : vkv )
                        {
                            T_V_Split kv = mtfa_string_split ( it, "=" );

                            if ( kv.size() > 0 )
                            {
                                if ( kv.size() == 2 )
                                {
                                    umhttpquery[kv[0]] = kv[1];
                                }
                                else
                                {
                                    umhttpquery[kv[0]] = "";
                                }
                            }
                        }
                    }

                    //Esaurita la prima riga, proseguiamo con le successive
                    //Cerchiamo cookie, host, ecc
                    for ( int i = 1; i < all_reqs.size(); ++i )
                    {
                        //I cookie vanno a parte
                        //cout << "Header: " << all_reqs[i] << endl;
                        static pcre2_code* reCookie = mtfa_compile_pattern ( "Cookie: " );
                        static pcre2_code* reHost = mtfa_compile_pattern ( "Host: " );

                        if ( mtfa_find_pattern ( all_reqs[i], reCookie ) )
                        {
                            string cookies = all_reqs[i].substr ( 8 );
                            //splitto e prendo i valori
                            T_V_Split vsck = mtfa_string_split2 ( cookies, "; " );

                            for ( auto& it : vsck )
                            {
                                size_t pos = it.find ( "=" );

                                if ( pos == string::npos )
                                {
                                    umcookie[it] = "";
                                }
                                else
                                {
                                    umcookie[it.substr ( 0, pos )] = it.substr ( pos + 1 );
                                }
                            }
                        }
                        else if ( mtfa_find_pattern ( all_reqs[i], reHost ) )
                        {
                            //"Host: "
                            host = all_reqs[i].substr ( 6 );
                        }
                        else
                        {
                            //è un header, normale
                            T_V_Split vs = mtfa_string_split2 ( all_reqs[i], ": " );

                            //cout << "Header normale: " << vs[0] << ": " << vs[1] << endl;
                            if ( vs.size() == 1 )
                            {
                                umhttpheader[vs[0]] = "";
                            }
                            else
                            {
                                umhttpheader[vs[0]] = vs[1];
                            }
                        }
                    }
                }

                //E ora la parte dati

                umhttpdata["HTTP_DATA"] = ( s_data[1].size() > 0 ) ? s_data[1] : "";

                //poi splitto pure!!
                T_V_Split asvs = mtfa_string_split ( s_data[1], "&" );

                for ( auto& it : asvs )
                {
                    if ( it.size() > 2 )
                    {
                        T_V_Split vs = mtfa_string_split ( it, "=" );
                        umhttpdata[vs[0]] = ( vs.size() > 1 ) ? vs[1] : "";
                    }
                }

                for ( auto it : umhttpdata )
                {
                    cerr << "umhttpdata:" << it.first << "=" << it.second << endl;
                }

                VAobservation obs1 ( VA_ItemTypes::HTTP_HOST, host, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_HOST ), obs1 ) );
                VAobservation obs2 ( VA_ItemTypes::HTTP_URL_HOST, host, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL_HOST ), obs2 ) );
                VAobservation obs3 ( VA_ItemTypes::HTTP_METHOD, method, unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_METHOD ), obs3 ) );
                VAobservation obs4 ( VA_ItemTypes::HTTP_URL, urldecode ( url ), unordered_map<string, string>() );
                obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL ), obs4 ) );
            }
        }

        //QUI LA COSTRUZIONE DELLE OBS UMAP

        //Prima mi chiedo se ho insieme sia url che uri
        auto p_url = obss->find ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL ) );

        //Se non c'è URL, allora uri non serve
        string s_uri;

        if ( p_url != obss->end() )
        {
            s_uri = p_url->second.value;

            //Se c'è uri, lo appendo
            auto p_uri = umhttpquery.find ( "0_FULL_QUERY" );

            if ( p_uri != umhttpquery.end() && p_uri->second.size() > 0 )
            {
                s_uri = p_uri->second;
            }
        }

        if ( s_uri.size() > 0 )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_URI, s_uri, unordered_map<string, string>() );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URI ), obs ) );
        }

        //Ci siamo. Ho per caso dei valori da eliminare in query/uri associati a hosts?
#if 0
        {
            string thehost = ( *obss ) [VA_ItemTypes::HTTP_HOST].value;
            auto found = VA_remove_from_query.find ( thehost );

            if ( found != VA_remove_from_query.end() )
            {
                //per ogni elemento in found, lo cerco in uri e in query e lo tolgo
                string& uri = ( *obss ) [VA_ItemTypes::HTTP_HOST].value;
                string& query = umhttpquery["0_FULL_QUERY"];

                for ( auto& elem : found->second )
                {
                    {
                        int pos_uri = uri.find ( elem );

                        if ( pos_uri != string::npos )
                        {
                            //trovata la prima, la tolgo
                            int fine_value = uri.substr ( pos_uri ).find ( "&" );

                            if ( fine_value != string::npos )
                            {
                                uri = uri.erase ( pos_uri, fine_value + 1 );
                            }
                            else     //fine stringa
                            {
                                uri = uri.erase ( pos_uri, string::npos );
                            }
                        }
                    }
                    {
                        int pos_query = query.find ( elem );

                        if ( pos_query != string::npos )
                        {
                            //trovata la prima, la tolgo
                            int fine_value = query.substr ( pos_query ).find ( "&" );

                            if ( fine_value != string::npos )
                            {
                                query = query.erase ( pos_query, fine_value + 1 );
                            }
                            else     //fine stringa
                            {
                                query = query.erase ( pos_query, string::npos );
                            }
                        }
                    }
                }
            }
        }
#endif

        if ( umcookie.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_COOKIES, "", umcookie );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_COOKIES ), obs ) );
        }

        if ( umhttpheader.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_HEADERS, "", umhttpheader );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_HEADERS ), obs ) );
        }

        if ( umhttpquery.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_QUERY, "", umhttpquery );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_QUERY ), obs ) );
        }

        if ( umhttpdata.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_DATA_POST, "", umhttpdata );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_DATA_POST ), obs ) );
        }

        if ( umanswercookie.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_ANSWER_COOKIES, "", umanswercookie );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_COOKIES ), obs ) );
        }

        if ( umhttpanswerdata.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_ANSWER_DATA, "", umhttpanswerdata );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_DATA ), obs ) );
        }

        if ( umhttpanswerheader.size() )
        {
            VAobservation obs ( VA_ItemTypes::HTTP_ANSWER_HEADERS, "", umhttpanswerheader );
            obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_HEADERS ), obs ) );
        }

        //Vai con la prossima osservazione, se ci sarà!
        obss = new VAobservations();
        lobss->push_back ( obss );
    }

    return lobss;
}

//Questo è per costruire le regole da inviare agli agenti.

template<class InputIt, class T, class BinaryOperation>
T myaccumulate ( InputIt first, InputIt last, T init,
                 BinaryOperation op )
{
    for ( ; first != last; ++first )
    {
        init = op ( init, *first );
    }

    return init;
}

static string BuildASRules ( list<string>& rules )
{
    string s = myaccumulate ( rules.begin(), rules.end(), std::string(),
                              [] ( const std::string & a, const std::string & b ) -> std::string
    {
        return a + ( a.length() > 0 ? "\n" : "" ) + b;
    } );
    return s + "\n";
}

void ThPoolEnqueueLsp (
    T_EventoStruct* bufs_in_out,  //The io pointers
    int direction, //0: recvd data from client, 1: recvd data from server
    struct evbuffer* src,
    void* ctx
)
{
    //Prima ancora di gestire il thread pool, faccio le verifiche di ammissibilità in base alle regole
    static mutex id_direct_access_mtx;
    static bool LbUser_VA_KAM_CommandIp_use_only_port=false;

    //Se è solo management oppure se è un protocollo TCP, inutile fare le verifiche
    string action;
    action.clear();

    //Sostituisco una catena di IF con un accesso diretto!
    //In questo modo utilizzo sempre uno switch case nel seguito
    static ImplicitDs* id_direct_access = nullptr;

    //per evitare di fare lok e unlock per ogni comunicazione!!
    if ( !id_direct_access ) //se no l'ho ancora inizializzato, allora metto il lock ma se all'uscita del lok qualcuno lo ha già inizializzato, non proseguo
    {
        id_direct_access_mtx.lock();
        if ( !id_direct_access )
        {

            //cout << "SONO QUI\n";
            if (LbUser_VA_KAM_CommandIp.compare("0.0.0.0")==0)
                LbUser_VA_KAM_CommandIp_use_only_port=true;
        
            id_direct_access = new ImplicitDs();
            string commands = LbUser_VA_KAM_CommandIp + ":" + to_string ( LbUser_VA_KAM_CommandPort );
            
            string hw_agents = LbUser_VA_HwAgentIp + ":" + to_string ( LbUser_VA_HwAgentPort );
            string sw_agents = LbUser_VA_SwAgentIp + ":" + to_string ( LbUser_VA_SwAgentPort );

            id_direct_access->Insert ( commands.data(), commands.length(), ( void* ) 1 );

            //lo stesso ma in SSL
            id_direct_access->Insert(LbUser_FromVACommands.data(), LbUser_FromVACommands.size(), (void*)1); //Tutti i comandi del KAM

            id_direct_access->Insert ( hw_agents.data(), hw_agents.length(), ( void* ) 2 );
            
            id_direct_access->Insert ( sw_agents.data(), sw_agents.length(), ( void* ) 2 );
            id_direct_access->Insert(LbUser_Initialize.data(), LbUser_Initialize.size(), (void*)3); //per far partire le connessioni alle varie VA
            
            if (LbUser_Id_WhoAmI == WhoAmI::eSuperVA)
            {
                //se sono SuperVA, allora tutto quello che arriva QUI lo gestisco!
                id_direct_access->Insert(LbUser_SVA_Listen.data(), LbUser_SVA_Listen.size(), (void*)4); //Tutti i comandi della SuperVA
            }
        }
        id_direct_access_mtx.unlock();
    }

    /*
    * Qui posso essere in tre modi
    * modo comando: LbUser_VA_CommandPort, LbUser_VA_CommandIp
    * modo swgent: LbUser_VA_SwAgentIp, LbUser_VA_SwAgentPort
    * modo hwagent: LbUser_VA_HwAgentIp, LbUser_VA_HwAgentPort
    *
    * Il modo comando è WebServices JSON: Dati passati con GET e query, gli altri modi sono WEB standard
    *
    * */
    
    if (LbUser_VA_KAM_CommandIp_use_only_port)
    {
        if (bufs_in_out->listen_port == LbUser_VA_KAM_CommandPort)
        {
            DoManageCommands ( &vaeng, bufs_in_out );
            return;
        }
    }

    unsigned long long thecase = 0xFFFFFFFFFFFFFFFF;
    string to_be_searched = bufs_in_out->listen_ip + ":" + to_string ( bufs_in_out->listen_port );
    //mtfa_log ( "to_be_searched: %s(&%d)\n", to_be_searched.c_str(), to_be_searched.length() );
    id_direct_access->Find ( to_be_searched.data(), to_be_searched.length(), ( void** ) &thecase );
    //cout << "thecase: " << thecase << endl;
    //cout << bufs_in_out->listen_ip << ":" << bufs_in_out->listen_port << endl;
    switch ( thecase )
    {
        case 1:
        {
            //Questo è gestito, quando necessario, tramite thread che non bloccano il funzionamento della VA e del KAM.
            DoManageCommands ( &vaeng, bufs_in_out );
            return;
        }

        case 2:
        {
            //cout << "Da un agente: " << bufs_in_out->s_loc_url << " => " << LbUser_VA_AskForUrl  << endl;
            //Gestione delle comunicazioni con gli agenti!
            if ( bufs_in_out->s_loc_url.size() >= LbUser_VA_AskForUrl.size() && bufs_in_out->s_loc_url.substr ( 0, LbUser_VA_AskForUrl.size() ).compare ( LbUser_VA_AskForUrl ) == 0 )
            {
                //No POST no Party!!
                if ( bufs_in_out->s_loc_method.compare ( "POST" ) != 0 )
                {
                    AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out, true );
                    return;
                }

                cerr << bufs_in_out->header_cs << endl;

                for ( auto& it : bufs_in_out->current_headers )
                {
                    cerr << it.first << ": " << it.second << endl;
                }

                //POST con body vuoto? riparte
                if ( bufs_in_out->s_body.size() == 0 )
                {
                    cerr << "Need MoreData\n";
                    bufs_in_out->last_was_in_0 = false;
                    bufs_in_out->need_body = true;
                    return;
                }

                //se non ci sono i due parametri passati nel campo query, esco
                auto vasaid = bufs_in_out->current_query.find ( "vasaid" );
                auto vaafkey = bufs_in_out->current_query.find ( "vaafkey" );

                if ( vasaid == bufs_in_out->current_query.end() || vaafkey == bufs_in_out->current_query.end() )
                {
                    AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out, true );
                    return;
                }

                //Qui dentro sono chiamati i threads
                cerr << __func__ << " - " << vasaid->first << ": " << vasaid->second << endl;
                cerr << __func__ << " - " << vaafkey->first << ": " << vaafkey->second << endl;
                VAEvaluateValidationRules ( vasaid->second, vaafkey->second, bufs_in_out );
                return;
            }

            //Se sono qui, allora blocco!
            AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out, true );
            return;
        }
        break;
        case 3:
        {
            //Qui costruisco le connessioni con le VA e SVA.
            //Per ora esco e basta
            AnswerData("HTTP/1.1 200 OK\r\n", "", "OK", bufs_in_out);
            return;
        }
        break;
        case 4:
        {
            //Qui gestisco le richieste SVA provenienti da VA e KAM
            DoManageSvaRequests ( /*&vaeng, */bufs_in_out );
            return;
        }
        break;
        default:
            ;
    }

    //La KAM che attiva la TRACE, gestisce anche l'unificazione delle trace
    if ( LbUser_Id_WhoAmI == WhoAmI::eKAM && LbUser_VA_TraceDomain.compare ( bufs_in_out->s_loc_full_host ) == 0 && bufs_in_out->s_loc_url.compare ( LbUser_VA_TraceUrl ) == 0 )
    {
        auto id = bufs_in_out->current_query.find ( LbUser_VA_TraceQuery );

        if ( id != bufs_in_out->current_query.end() )
        {
            pair<string, time_t> rec;
            bool found = vaeng.trace_ds.GetValue ( id->second, rec );

            if ( found )
            {
                T_V_Split data = mtfa_string_split ( rec.first, ":" );

                if ( data.size() == 5 )
                {
//                     if ( !mtfa_find_pattern ( data[0], reBase64 ) ) continue;
//                     if ( !mtfa_find_pattern ( data[1], reBase64 ) ) continue;
//                     if ( !mtfa_find_pattern ( data[2], reBase64 ) ) continue;
//                     if ( !mtfa_find_pattern ( data[3], reBase64 ) ) continue;
//                     if ( !mtfa_find_pattern ( data[4], reBase64 ) ) continue;
                    auto vadomain  = Base64::decode ( data[0] );
                    auto ckname    = Base64::decode ( data[1] );
                    auto setname   = Base64::decode ( data[2] );
                    auto uri       = Base64::decode ( data[3] );
                    auto host      = Base64::decode ( data[4] );

                    //Allora! c'è il cookie per la VA?
                    string newcookie;
                    string newckvalue;
                    auto ckvalue = bufs_in_out->current_cookies.find ( ckname );

                    if ( ckvalue == bufs_in_out->current_cookies.end() )
                    {
                        //Il cookie non c'è lo creo come cookie VA e me lo ricordo
                        //Non serve che io me lo ricordi anche se potrei tentare di ricordarlo!
                        newckvalue = mtfa_rand_string ( 32, 62 );
                    }
                    else
                    {
                        //Se sono qui l'utente ha già oppure glielo abbiamo appena costruito il cookie per la VA,
                        //Glielo devo dare anche per il sito originale (per tracciarlo)
                        //Gli fornisco il cookie che ha già
                        newckvalue = ckvalue->second;
                    }

                    newcookie = mtfa_make_cookie ( ckname, newckvalue, CookieDurations::NeverExpires );

                    //in ogni caso devo ricordarmi il valore del cookie, per settarlo anche rispetto al sito originale
                    vaeng.trace_ds.Delete ( id->second );
                    vaeng.trace_ds.Insert ( id->second, make_pair ( rec.first + ":" + Base64::encode ( newckvalue ), time ( nullptr ) ) );

                    //devo fare la redirect verso il sito originale ma con l'attenzione di aggiungere un parametro che corrisponde al cookie
                    AnswerRedirect ( "http://" + host + LbUser_VA_TraceUrl + "?" + LbUser_VA_TraceQuery + "=" + id->second + "&" + LbUser_VA_mtfa_rnd + "=" + mtfa_rand_string ( 8, 62 ), newcookie, bufs_in_out );
                    return;
                }
            }
        }
    }

    //Questo, signori, è il KAM! Ma potrei ancora avere le redirect per le setcookie del servizio di tracking
    auto id_trace_query = bufs_in_out->current_query.find ( LbUser_VA_TraceQuery );
    auto id_mtfa_random = bufs_in_out->current_query.find ( LbUser_VA_mtfa_rnd );

    if ( id_trace_query != bufs_in_out->current_query.end() && id_mtfa_random != bufs_in_out->current_query.end() )
    {
        //Siamo ancora in preparazione del tracking
        pair<string, time_t> rec;
        bool found = vaeng.trace_ds.GetValue ( id_trace_query->second, rec );

        if ( found )
        {
            T_V_Split data = mtfa_string_split ( rec.first, ":" );

            if ( data.size() == 6 )
            {
                auto va_domain = Base64::decode ( data[0] );
                auto ckname    = Base64::decode ( data[1] );
                auto setname   = Base64::decode ( data[2] );
                auto uri       = Base64::decode ( data[3] );
                auto host      = Base64::decode ( data[4] );
                auto ckvalue   = Base64::decode ( data[5] ); //é stato aggiornato quando la va ha ottenuto il valore del cookie

                vaeng.trace_ds.Delete ( id_trace_query->second );

                //Allora, se il cookie non c'è oppure non ha lo stesso valore, lo setto, altrimenti faccio passare
                auto cerca_cookie = bufs_in_out->current_cookies.find ( ckname );

                if ( cerca_cookie == bufs_in_out->current_cookies.end() || cerca_cookie->second.compare ( ckvalue ) != 0 )
                {
                    string newcookie = mtfa_make_cookie ( ckname, ckvalue, CookieDurations::NeverExpires );
                    //Inserisco il valore del cookie dentro il set
                    vaeng.vasets.AddItem ( setname, ckvalue, 3600, true, false);
                    //devo fare la redirect verso il sito originale
                    AnswerRedirect ( "http://" + host + uri, newcookie, bufs_in_out );
                    return;
                }
                else
                {
                    //proseguo!
                }
            }
        }
        else
        {
            //non c'è molto da fare, potrei riprovarci ma non ho più i dati!
            //Forse c'è da fare in qualche altro modo!
            //Faccio una redirect alla home
            cerr << __func__ << ". " << "trace_ds deleted" << endl;
            AnswerRedirect ( "http://" + bufs_in_out->s_loc_host + "/", "", bufs_in_out );
            return;
        }
    }

    //Se sono qui è poiché da qui si passa.
    //Costruisco la struttura di osservazione per l'access rule e questa struttura la costruisce il KAM

    //Prendo agent id e session id, se presenti
    string agent_id;
    string session_id;
    auto pagentid = bufs_in_out->current_cookies.find ( "VAHDR100" );

    if ( pagentid == bufs_in_out->current_cookies.end() || ( *pagentid ).second.size() < 40 )
    {
        agent_id.clear();
        session_id.clear();
    }
    else
    {
        agent_id = ( *pagentid ).second.substr ( 0, 20 );
        session_id = ( *pagentid ).second.substr ( 20 );
    }

    //cout << "Check: " << bufs_in_out->s_loc_url << "     : " << bufs_in_out->listen_ip << "     :" << bufs_in_out->listen_port << endl;
    //static mutex * CheckAccessRules_mtx = new mutex();
    //CheckAccessRules_mtx->lock();
    bool bret = CheckAccessRules ( vaeng, bufs_in_out, agent_id, session_id, vaeng.acrules );
//     CheckAccessRules_mtx->unlock();

    if ( bret )
    {
        return;
    }

    cerr << __func__ << "Block request: " << bufs_in_out->s_loc_host << "- " << bufs_in_out->s_loc_uri << endl;
    AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out, true );
    return;

#if 0
    //questa istruzione la raggiungo solo nella versione EIS_LEVEL_7: i valori sono N e M
    //N: non ha matchato
    //M: match per manage

    //Disabilito gli eventi su questo canale
    //cerr << "Entra in ThPoolEnqueueLsp" << endl;
    bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
    bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);

//All'interno il comando si aspetta di trovare i dati in certe variabili
//cout << __func__   << "Ci sono: " << glb_tpool->size() << "threads in esecuzione" << endl;

    auto result = glb_tpool->enqueue (
                      []
                      (
                          mtfa_ThreadPool * p,
                          string action, //The command parameters
                          T_EventoStruct * bufs_in_out //The io pointers
                      )
    {
        ExecuteLispCode (
            action,
            bufs_in_out
        );
        return;
    },
    glb_tpool,
    action, //The command parameters
    bufs_in_out //The io pointers
                  );
//cerr << "Esce da ThPoolEnqueueLsp" << endl;
#endif

}

string GetValueFromRunningData ( string& agent_id, string& session_id, T_EventoStruct* pes, VA_ItemTypes tipo, const string &name)
{
    switch ( tipo )
    {
        case VA_ItemTypes::AGENT_ID:
            return agent_id;

        case VA_ItemTypes::SA_OS:
        case VA_ItemTypes::SA_LOGGED_USER:
        case VA_ItemTypes::SA_PROCESS_NAME:
        case VA_ItemTypes::OBS_NAME:
        case VA_ItemTypes::OBS_TIME:
        case VA_ItemTypes::OBS_OBSID:
        case VA_ItemTypes::OBS_STIME:
        case VA_ItemTypes::OBS_ETIME:
        case VA_ItemTypes::OBS_TAG:
        case VA_ItemTypes::OBS_EVENT:
        case VA_ItemTypes::NET_RESULT:
        case VA_ItemTypes::HTTP_ANSWER_CODE:
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
        case VA_ItemTypes::HTTP_ANSWER_DATA:
        case VA_ItemTypes::AR_TIME:
        case VA_ItemTypes::NONE:
            cout << __func__ <<"." << __LINE__ << ": tag not available." << mtfa_enum_to_int<VA_ItemTypes>(tipo) << endl;
            return "";
            
        case VA_ItemTypes::VARIABLE:
            //se è variable, torna sé stesso
            return name;            
            
        case VA_ItemTypes::NET_SESID:
            return session_id;
            
        case VA_ItemTypes::HTTP_METHOD:
            return pes->s_loc_method;
            
        case VA_ItemTypes::HTTP_HOST:
            return pes->s_loc_host;
            
        case VA_ItemTypes::HTTP_HEADER_HOST:
            return pes->s_loc_host;
            
        case VA_ItemTypes::HTTP_URL_HOST:
            return pes->server_host;

        case VA_ItemTypes::NET_TCP_IPDST:
            return pes->server_ip + ":" + to_string(pes->server_port);
            
        case VA_ItemTypes::NET_TCP_IPSRC:
            return pes->client_ip + ":" + to_string(pes->client_port);

        case VA_ItemTypes::HTTP_URL:
            return pes->s_loc_url;
            
        case VA_ItemTypes::HTTP_URI:
            return pes->s_loc_uri;
        
        case VA_ItemTypes::HTTP_COOKIES:
        {
            string sname=name;
            std::transform(sname.begin(), sname.end(), sname.begin(), ::tolower);
            return pes->current_cookies[sname];
        }
        case VA_ItemTypes::HTTP_HEADERS:
        {
            string sname=name;
            std::transform(sname.begin(), sname.end(), sname.begin(), ::tolower);
            return pes->current_headers[sname];
        }
        case VA_ItemTypes::HTTP_QUERY:
        {
            string sname=name;
            std::transform(sname.begin(), sname.end(), sname.begin(), ::tolower);
            return pes->current_query[sname];
        }   
        case VA_ItemTypes::HTTP_DATA_POST:
            return pes->s_body;
            
        default:
            cout << __func__ <<"." << __LINE__ << ": tag not available." << mtfa_enum_to_int<VA_ItemTypes>(tipo) << endl;
            break;
    }
}

bool CheckAccessRuleItem ( VAengine& vaeng, VAcriteria& bac, T_EventoStruct* pes, string& agent_id, string& session_id )
{
    //usata localmente per eliminare i dati che soddisfano il mtfa_compile_pattern
    switch ( bac.itype )
    {
        case VA_ItemTypes::CALL_LISP_FUNCTION:
        {
            scm_init_guile();
            
            //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
            list<string> lvalues;
            for (auto & ti: bac.cats)
            {
                auto st = GetValueFromRunningData ( agent_id, session_id, pes, get<0>(ti), get<1>(ti));
                lvalues.push_back(remove_pattern(get<2>(ti), st, get<3>(ti)) );
            }
            
            //Ora posso chiamare la funzione!!! nota che potrei aver voluto associare, e l'ho fatto, a tempo di compilazione il nome della funzione al suo puntatore all'interno del LISP
            //costruisco la lista delle variabili (necessariamente in formato lista)
            SCM lista = SCM_EOL;
            for (auto &s: lvalues)
            {
                char * p = (char*)malloc(s.size());
                std::copy(s.begin(), s.end(), p);
                SCM ptr = scm_pointer_to_bytevector (
                    scm_from_pointer ( ( void* ) p, free ),
                    scm_from_int ( s.size() ),
                    scm_from_int ( 0 ),
                    scm_string_to_symbol ( U8 ) );
                lista = scm_cons(ptr, lista);
            }

            lista = scm_reverse(lista);
            //Ora posso chiamare la funzione, passandogli i parametri e verificare il risultato. Se falso, allora torna false
            SCM scm_ret = scm_call_2(bac.func, lista, scm_from_pointer(pes, nullptr));
            if (scm_ret == SCM_BOOL_F)
                return false;
        }
        break;

        case VA_ItemTypes::AGENT_ID:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                {
                    if ( remove_pattern ( bac.pattern, agent_id, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                case VA_OpTypes::EXISTS:
                {
                    if ( remove_pattern ( bac.pattern, agent_id, bac.replace ).size() == 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    {
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( remove_pattern ( bac.pattern, agent_id, bac.replace ) ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::NET_SESID:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, session_id, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::EXISTS:
                    if ( remove_pattern ( bac.pattern, session_id, bac.replace ).size() == 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    {
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( remove_pattern ( bac.pattern, session_id, bac.replace ) ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }
                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

//        case VA_ItemTypes::AR_TIME:
//        {
//            switch (bac.op) {
//                case VA_OpTypes::EQUAL:
//                    if (agent_id.compare(bac.op1) != 0) return false;
//                    break;
//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
//                case VA_OpTypes::IN:
//                    auto ds = vaeng.vasets.dataset().find(bac.op1);
//                    if (ds == vaeng.vasets.dataset().end()) return false;
//                    if (ds->second->vb()[agent_id] == ds->second->uss().end()) return false;
//                    break;
//            }
//        }
//            break;
        case VA_ItemTypes::NET_TCP_IPSRC:
        {
            string ipsrc = pes->client_ip + ":" + to_string ( pes->client_port );
            ipsrc = remove_pattern ( bac.pattern, ipsrc, bac.replace );

            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( ipsrc.compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    unsigned int ident;

                    if ( ds->second->stype == VA_SetTypes::SetOfIpS )
                    {
                        vector<unsigned short> item = vaeng.vasets.IpPortStringToVS ( ipsrc );
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( !ds->second->mss->Search ( item,  ident ) )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }
                    else
                    {
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( ipsrc ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << "- " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::NET_TCP_IPDST:
        {
            string ipdst = pes->listen_ip + ":" + to_string ( pes->listen_port );
            ipdst = remove_pattern ( bac.pattern, ipdst, bac.replace );

            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( ipdst.compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    unsigned int ident;

                    if ( ds->second->stype == VA_SetTypes::SetOfIpS )
                    {
                        vector<unsigned short> item = vaeng.vasets.IpPortStringToVS ( ipdst );
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( !ds->second->mss->Search ( item,  ident ) )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }
                    else
                    {   DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( ipdst ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << "- " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_METHOD:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->s_loc_method, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                    DO_VASET_LOCK(ds->second->rmtx);
                    if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->s_loc_method, bac.replace ) ) == ds->second->uss->end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_HOST:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->s_loc_host, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                    DO_VASET_LOCK(ds->second->rmtx);
                    if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->s_loc_host, bac.replace ) ) == ds->second->uss->end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_URL:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->s_loc_url, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

//                case VA_OpTypes::EXISTS:
//                    if (agent_id.size() == 0) return false;
//                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    unsigned int ident;

                    if ( ds->second->stype == VA_SetTypes::SetOfUrlS )
                    {
                        //preparo il path per la sua ricerca!
                        vector<unsigned short> item = vaeng.vasets.UrlStringToVS ( remove_pattern ( bac.pattern, pes->s_loc_url, bac.replace ) );
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( !ds->second->mss->Search ( item, ident ) )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }
                    else
                    {   DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->s_loc_url, bac.replace ) ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_URI:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->s_loc_uri, bac.replace ).compare ( bac.op1 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::IN:
                {
                    unsigned int ident;
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( ds->second->stype == VA_SetTypes::SetOfUrlS )
                    {
                        vector<unsigned short> item = vaeng.vasets.UrlStringToVS ( remove_pattern ( bac.pattern, pes->s_loc_uri, bac.replace ) );
                        DO_VASET_LOCK(ds->second->rmtx);
                        if ( !ds->second->mss->Search ( item, ident ) )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }
                    else
                    {   DO_VASET_LOCK(ds->second->rmtx);
                        if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->s_loc_uri, bac.replace ) ) == ds->second->uss->end() )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_COOKIES:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->current_cookies[bac.op1], bac.replace ).compare ( bac.op2 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::EXISTS:
                    if ( pes->current_cookies.find ( bac.op1 ) == pes->current_cookies.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                    DO_VASET_LOCK(ds->second->rmtx);
                    if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->current_cookies[bac.op2], bac.replace ) ) == ds->second->uss->end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                }

                if ( bac.donot )
                    return false;

                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_HEADERS:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->current_headers[bac.op1], bac.replace ).compare ( bac.op2 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::EXISTS:
                    if ( pes->current_headers.find ( bac.op1 ) == pes->current_headers.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                    DO_VASET_LOCK(ds->second->rmtx);
                    if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->current_headers[bac.op2], bac.replace ) ) == ds->second->uss->end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
            }
        }
        break;

        case VA_ItemTypes::HTTP_QUERY:
        {
            switch ( bac.op )
            {
                case VA_OpTypes::EQUAL:
                    if ( remove_pattern ( bac.pattern, pes->current_query[bac.op1], bac.replace ).compare ( bac.op2 ) != 0 )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::EXISTS:
                    if ( pes->current_query.find ( bac.op1 ) == pes->current_query.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;

                    break;

                case VA_OpTypes::IN:
                {
                    auto ds = vaeng.vasets.datasets.find ( bac.op1 );

                    if ( ds == vaeng.vasets.datasets.end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }
                    DO_VASET_LOCK(ds->second->rmtx);
                    if ( ds->second->uss->find ( remove_pattern ( bac.pattern, pes->current_query[bac.op2], bac.replace ) ) == ds->second->uss->end() )
                    {
                        if ( bac.donot )
                            break;
                        else
                            return false;
                    }

                    if ( bac.donot )
                        return false;
                }
                break;

                default:
                    cout << __func__ << " - " << __LINE__ << endl;
                    break;
            }
        }
        break;
        
        //Ora il cat
                case VA_ItemTypes::CAT:
                {
                    //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
                    string value;
                    for (auto & ti: bac.cats)
                    {
                        auto st = GetValueFromRunningData ( agent_id, session_id, pes, get<0>(ti), get<1>(ti));
                        value += (remove_pattern(get<2>(ti), st, get<3>(ti)) );
                    }
                    
                    //Ora questo valore devo trovarlo nell'insieme
                    if ( bac.op == VA_OpTypes::IN )
                    {
                        if ( bac.op1.size() == 0 )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                        
                        auto theset = vaeng.vasets.datasets.find ( bac.op1 );
                        
                        if ( !theset->second )
                        {
                            if ( bac.donot )
                                break;
                            else
                                return false;
                        }
                        DO_VASET_LOCK(theset->second->rmtx);
                        if (theset->second->stype==VA_SetTypes::SetOfStrings) {
                            if ( !theset->second->uss )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }

                            if ( theset->second->uss->size() == 0 )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }
                            auto found = theset->second->uss->find(value);

    //                         for (auto it: *theset->second->uss)
    //                         {
    //                             cout << it.size() << ": " << "<" << it << ">" << endl;
    //                         }

                            bool isin = (found != theset->second->uss->end());

                            if ( !isin )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }
                        } else if (theset->second->stype==VA_SetTypes::Dictionary) {
                            if ( !theset->second->umss )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }

                            if ( theset->second->umss->size() == 0 )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }
                            T_V_Split key_value = mtfa_string_split ( value, "|" );
                            if (key_value.size()!=2) {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }
                            auto found = theset->second->umss->find(key_value[0]);
                            bool isin=false;
                            if (found != theset->second->umss->end()) {
                                auto value = found->second;
                                if (value.compare(key_value[1])==0)
                                    isin = true;
                                else
                                    isin=false;
                            } else {
                                isin = false;
                            }

                            if ( !isin )
                            {
                                if ( bac.donot )
                                    break;
                                else
                                    return false;
                            }
                        }
                    }
                    
                    if ( bac.donot )
                        return false;
                }
                break;
                
        default:
            cout << __func__ << " - " << __LINE__ << endl;
    }

//                         if (bac.donot)
//                             break;
//                         else
//                             return false;
    return true;
}

string EvalArObservation( const VAengine& vaeng, const Command &cmd, T_EventoStruct* pes, const string& agent_id, const string& session_id );
string EvalArObservation( const VAengine& vaeng, const Command &cmd, T_EventoStruct* pes, const string& agent_id, const string& session_id )
{
    switch ( cmd.id() )
    {
        case Parser::token::TOKEN_AGENT_ID:
            return agent_id;
            break;

        case Parser::token::TOKEN_NET_TCP_SESID:
            return session_id;
            break;

        case Parser::token::TOKEN_NET_TCP_IPSRC:
            return pes->client_ip + ":" + to_string ( pes->client_port );;
            break;

        case Parser::token::TOKEN_NET_TCP_IPDST:
            return pes->listen_ip + ":" + to_string ( pes->listen_port );
            break;

        case Parser::token::TOKEN_HTTP_METHOD:
            return pes->s_loc_method;
            break;

        case Parser::token::TOKEN_HTTP_HOST:
            return pes->s_loc_host;
            break;

        case Parser::token::TOKEN_HTTP_URL:
            return pes->s_loc_url;
            break;

        case Parser::token::TOKEN_HTTP_URI:
            return pes->s_loc_uri;
            break;

        case Parser::token::TOKEN_HTTP_COOKIE:
            return pes->current_cookies[cmd.args_s().front()];
            break;

        case Parser::token::TOKEN_HTTP_HEADER:
            return pes->current_cookies[cmd.args_s().front()];
            break;

        case Parser::token::TOKEN_HTTP_QUERY:
            return pes->current_cookies[cmd.args_s().front()];
            break;

        case Parser::token::TOKEN_STRING:
            return cmd.name();
            break;

        case Parser::token::TOKEN_IDENT:
            return cmd.name();
            break;
            
        default:
            mtfa_log ( "Switch case undefined!\n" );
            return "";
    }
}

bool CheckAccessRules ( VAengine& vaeng, T_EventoStruct* bufs_in_out, string agent_id, string session_id, unordered_map<string, AccessRule>& acrules )
{
    //Se il protocollo è TCP-IP, allora lo gestisco direttamente da LISP, tramite MANAGE
    //Questo significa che il LISP deve gestire tutto il protocollo di comunicazione
    //Per i WebSocket, invece, il protocollo base è il HTTP e quindi
    //Si gestiscono da HTTP: in questo caso per poter consentire il recupero della comunicazione e per poter chiamare sempre la stessa funzione
    //gestendo un fiber pool, allora va inserito nella pbuf anche la chiave del websocket...
    if (bufs_in_out->protocollo == Protocols::eProtTCP) {
//         if ( bufs_in_out->p_buf_in )
//             bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE | EV_TIMEOUT);
//         if ( bufs_in_out->p_buf_out )
//             bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE | EV_TIMEOUT);  //questo, tanto, non c'è!!

        //bufs_in_out->UniqueId
        char * pcommand = (char*)malloc(bufs_in_out->tcpmanage.size()+1);
        strcpy(pcommand, bufs_in_out->tcpmanage.c_str());
        pair<char*, T_EventoStruct *> *params = new pair<char*, T_EventoStruct*>();
        *params = make_pair(pcommand, bufs_in_out);
        glb_tpool->push_task( //enqueue(
            []
            (pair<char*, T_EventoStruct *>*params)
            {
                ExecuteLispCode(params); //ho accodato la richiesta TCP
                return;
            },
            params
        );
        return true;
    }

    
    
    for ( auto& name_ar : vaeng.acrules_ordering )
    {
        auto iar = acrules.find ( name_ar );
        auto ar = *iar;

        if ( !ar.second.enabled )
        {
            continue;
        }

        bool ok = true;

        for ( auto& bac : ar.second.basic_access_criteria )
        {
            if ( !CheckAccessRuleItem ( vaeng, bac, bufs_in_out, agent_id, session_id ) )
            {
                ok = false;
                break; //non matcha!
            }
        }

        if ( !ok )
        {
            continue;
        }

        T_um_ss temporary_kv_assoc;
        KAM_ActionTypes theAction = KAM_ActionTypes::NOP;
        string theanswer;
        //bool b_answer = false;

        string thebandwidth;
        bool b_bandwidth = false;

        //         string theexpiry;
        //         bool b_expiry = false;

        string thehttpredirect;

        //string thehttpsredirect;
        //bool b_httpredirect = false;

        string thetcpredirect;
        string server_cert;
        string host_cert;
        string client_cert;
        string client_key;
        string client_method;
        string client_algo;
        bool check_server_cert;
        //bool b_tcpredirect = false;

        string thescmgive;
        //bool b_scmgive = false;

        list<string> thescmmanage;
        //bool b_scmmanage = false;

        string thevadomain;
        string theset;
        string thecookiename;

        //Qui applico le azioni relative alla comunicazione!
        for ( KAMaction& arop : ar.second.access_operations )
        {
            switch ( arop.action )
            {
            case KAM_ActionTypes::CALL_LISP_FUNCTION:
            {
                scm_init_guile();
                //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
                list<string> lvalues;
                for (auto & ti: arop.cat)
                {
                    auto st = GetValueFromRunningData ( agent_id, session_id, bufs_in_out, get<0>(ti), get<1>(ti));
                    lvalues.push_back(remove_pattern(get<2>(ti), st, get<3>(ti)) );
                }
                
                //Ora posso chiamare la funzione!!! nota che potrei aver voluto associare, e l'ho fatto, a tempo di compilazione il nome della funzione al suo puntatore all'interno del LISP
                //costruisco la lista delle variabili (necessariamente in formato lista)
                SCM lista = SCM_EOL;
                for (auto &s: lvalues)
                {
                    char * p = (char*)malloc(s.size());
                    std::copy(s.begin(), s.end(), p);
                    SCM ptr = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) p, free ),
                        scm_from_int ( s.size() ),
                        scm_from_int ( 0 ),
                        scm_string_to_symbol ( U8 ) );
                    lista = scm_cons(ptr, lista);
                }

                lista = scm_reverse(lista);
                //Ora posso chiamare la funzione, passandogli i parametri e verificare il risultato. Se falso, allora torna false
                SCM scm_ret = scm_call_2(arop.func, lista, scm_from_pointer(bufs_in_out, nullptr));
                if (scm_ret != SCM_BOOL_F)
                    theAction = KAM_ActionTypes::CALL_LISP_FUNCTION;
            }
            break;
            case KAM_ActionTypes::HEADER_ANSWER_REPLACE:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                pcre2_code * pattern = arop.replace_pattern;
                string replace = arop.replace;
                bufs_in_out->replace_header_in_answer=make_pair(pattern, replace);
                bufs_in_out->b_replace_header_in_answer= true;
            }
            break;

            case KAM_ActionTypes::HEADER_REQUEST_REPLACE:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                pcre2_code * pattern = arop.replace_pattern;
                string replace = arop.replace;
                bufs_in_out->replace_header_in_request=make_pair(pattern, replace);
                bufs_in_out->b_replace_header_in_request= true;
            }
            break;

            case KAM_ActionTypes::HEADER_ANSWER_ADD:
            case KAM_ActionTypes::HEADER_ANSWER_VADD:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                //Dobbiamo aggiungere una coppia nome=valore alla comunicazione da (answer) o verso (request) il server
                //la modifica la facciamo direttamente qui dentro!
                //Allora, se siamo qui è perché abiamo letto almeno la parte header. Se fossimo nella parte dati non
                //staremmo passando di qui, l'operazione sarebbe già stata effettuata.
                //Quindi tolgo solo la prima riga e aggiungo quello che serve
                string name;
                string value;

                if (!arop.var_name.size())
                    name  = arop.default_value;
                else {
                    auto val = ar.second.ar_vars.find ( arop.var_name );
                    if (val != ar.second.ar_vars.end())
                        name  = val->second;
                }

                if (!arop.last_value.size())
                    value = arop.last_value_1;
                else {
                    auto val = ar.second.ar_vars.find ( arop.last_value );
                    if (val != ar.second.ar_vars.end())
                        value = val->second;
                }                

                bufs_in_out->add_header_to_answer.push_back ( KamStringEval(name, bufs_in_out, temporary_kv_assoc) + ": " + KamStringEval(value, bufs_in_out, temporary_kv_assoc));
                bufs_in_out->b_add_header_to_answer = true;
            }
            break;

            case KAM_ActionTypes::HEADER_REQUEST_ADD:
            case KAM_ActionTypes::HEADER_REQUEST_VADD:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                //Dobbiamo aggiungere una coppia nome=valore alla comunicazione da (answer) o verso (request) il server
                string name;
                string value;
                if (!arop.var_name.size())
                    name  = arop.default_value;
                else {
                    auto val = ar.second.ar_vars.find ( arop.var_name );
                    if (val != ar.second.ar_vars.end())
                        name  = val->second;
                }

                if (!arop.last_value.size())
                    value = arop.last_value_1;
                else {
                    auto val = ar.second.ar_vars.find ( arop.last_value );
                    if (val != ar.second.ar_vars.end())
                        value = val->second;
                }                
                bufs_in_out->add_header_to_request.push_back ( KamStringEval(name, bufs_in_out, temporary_kv_assoc)+ ": " + KamStringEval(value, bufs_in_out, temporary_kv_assoc));
                bufs_in_out->b_add_header_to_request = true;
            }
            break;

            case KAM_ActionTypes::COOKIE_ANSWER_ADD:
            case KAM_ActionTypes::COOKIE_ANSWER_VADD:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                //Dobbiamo aggiungere una coppia nome=valore alla comunicazione da (answer) o verso (request) il server
                //la modifica la facciamo direttamente qui dentro!
                //Allora, se siamo qui è perché abiamo letto almeno la parte header. Se fossimo nella parte dati non
                //staremmo passando di qui, l'operazione sarebbe già stata effettuata.
                //Quindi tolgo solo la prima riga e aggiungo quello che serve
                string name;
                string value;
                if (!arop.var_name.size())
                    name  = arop.default_value;
                else {
                    auto val = ar.second.ar_vars.find ( arop.var_name );
                    if (val != ar.second.ar_vars.end())
                        name  = val->second;
                }

                if (!arop.last_value.size())
                    value = arop.last_value_1;
                else {
                    auto val = ar.second.ar_vars.find ( arop.last_value );
                    if (val != ar.second.ar_vars.end())
                        value = val->second;
                }                
                bufs_in_out->add_header_to_answer.push_back ( "Set-Cookie: " + KamStringEval(name, bufs_in_out, temporary_kv_assoc) + "=" + KamStringEval(value, bufs_in_out, temporary_kv_assoc));
                bufs_in_out->b_add_header_to_answer = true;
            }
            break;

            case KAM_ActionTypes::COOKIE_REQUEST_ADD:
            case KAM_ActionTypes::COOKIE_REQUEST_VADD:
            {
                //Qui devo modificare esclusivamente il bufs_inout!!
                //Dobbiamo aggiungere una coppia nome=valore alla comunicazione da (answer) o verso (request) il server
                string name;
                string value;
                if (!arop.var_name.size())
                    name  = arop.default_value;
                else {
                    auto val = ar.second.ar_vars.find ( arop.var_name );
                    if (val != ar.second.ar_vars.end())
                        name  = val->second;
                }

                if (!arop.last_value.size())
                    value = arop.last_value_1;
                else {
                    auto val = ar.second.ar_vars.find ( arop.last_value );
                    if (val != ar.second.ar_vars.end())
                        value = val->second;
                }                
                bufs_in_out->add_header_to_request.push_back ( "Cookie: " + KamStringEval(name, bufs_in_out, temporary_kv_assoc) + "=" + KamStringEval(value, bufs_in_out, temporary_kv_assoc));
                bufs_in_out->b_add_header_to_request = true;
            }
            break;

            case KAM_ActionTypes::TRACE:
            {
                thevadomain = KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc);
                thecookiename = KamStringEval(arop.default_value, bufs_in_out, temporary_kv_assoc);
                theset = KamStringEval(arop.last_value, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::TRACE;
            }
            break;

            case KAM_ActionTypes::ANSWER:
                theanswer = KamStringEval(arop.default_value, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::ANSWER;
                break;

            case KAM_ActionTypes::VANSWER:
            {
                //se non trova torna init del tipo (stirnga vuota).
                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    theanswer = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    theanswer = arop.default_value;
                }
                theanswer=KamStringEval(theanswer, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::ANSWER;
            }
            break;

            case KAM_ActionTypes::BANDWIDTH:
                thebandwidth = KamStringEval(arop.default_value, bufs_in_out, temporary_kv_assoc);
                b_bandwidth = true;
                break;

            case KAM_ActionTypes::VBANDWIDTH:
            {
                //se non trova torna init del tipo (stirnga vuota).
                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    thebandwidth = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    thebandwidth = arop.default_value;
                }
                thebandwidth=KamStringEval(thebandwidth, bufs_in_out, temporary_kv_assoc);
                b_bandwidth = true;;
            }
            break;

            //case KAM_ActionTypes::EXPIRY:
            //theexpiry = arop.default_value;
            //b_expiry = true;
            //break;
            //case KAM_ActionTypes::VEXPIRY:
            //theexpiry = (ar.second.ar_vars[arop.var_name].size()==0)?arop.default_value:ar.second.ar_vars[arop.var_name];
            //b_expiry = true;
            //break;
            case KAM_ActionTypes::HTTP_REDIRECT:
                thehttpredirect = KamStringEval(arop.default_value, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::HTTP_REDIRECT;
                break;

            case KAM_ActionTypes::HTTP_VREDIRECT:
            {
                //se non trova torna init del tipo (stirnga vuota).
                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    thehttpredirect = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    thehttpredirect = arop.default_value;
                }

                thehttpredirect = KamStringEval(thehttpredirect, bufs_in_out, temporary_kv_assoc);

                theAction = KAM_ActionTypes::HTTP_REDIRECT;
            }
            break;

            case KAM_ActionTypes::TCP_REDIRECT_HOST:
                thetcpredirect = bufs_in_out->s_loc_host;
                theAction = KAM_ActionTypes::TCP_REDIRECT_HOST;
                client_cert = KamStringEval(arop.client_cert, bufs_in_out, temporary_kv_assoc);
                server_cert = KamStringEval(arop.server_cert, bufs_in_out, temporary_kv_assoc);
                host_cert = KamStringEval(arop.hostname, bufs_in_out, temporary_kv_assoc);
                client_key = KamStringEval(arop.client_key, bufs_in_out, temporary_kv_assoc);
                client_method = KamStringEval(arop.client_method, bufs_in_out, temporary_kv_assoc);
                client_algo = KamStringEval(arop.client_algo, bufs_in_out, temporary_kv_assoc);
                check_server_cert = arop.check_server_cert;
                break;

            case KAM_ActionTypes::TCP_REDIRECT_VHOST:
            {
                //se non trova torna init del tipo (stirnga vuota).

                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    thetcpredirect = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    thetcpredirect = arop.default_value;
                }

                thetcpredirect = KamStringEval(thetcpredirect, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::TCP_REDIRECT_HOST;
                client_cert = arop.client_cert;
                server_cert = arop.server_cert;
                host_cert = arop.hostname;
                client_key = arop.client_key;
                client_algo = arop.client_algo;
                client_method = arop.client_method;
                check_server_cert = arop.check_server_cert;
            }
            break;

            case KAM_ActionTypes::TCP_REDIRECT_STRING:
            {
                thetcpredirect = arop.default_value;
                thetcpredirect = KamStringEval(thetcpredirect, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::TCP_REDIRECT_HOST;
                client_cert = KamStringEval(arop.client_cert, bufs_in_out, temporary_kv_assoc);
                server_cert = KamStringEval(arop.server_cert, bufs_in_out, temporary_kv_assoc);
                host_cert = KamStringEval(arop.hostname, bufs_in_out, temporary_kv_assoc);
                client_key = KamStringEval(arop.client_key, bufs_in_out, temporary_kv_assoc);
                client_method = KamStringEval(arop.client_method, bufs_in_out, temporary_kv_assoc);
                client_algo = KamStringEval(arop.client_algo, bufs_in_out, temporary_kv_assoc);
                check_server_cert = arop.check_server_cert;
            }
            break;

            case KAM_ActionTypes::TCP_REDIRECT_VSTRING:
            {
                //se non trova torna init del tipo (stirnga vuota).
                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    thetcpredirect = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    thetcpredirect = arop.default_value;
                }

                thetcpredirect = KamStringEval(thetcpredirect, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::TCP_REDIRECT_HOST;
                client_cert = KamStringEval(arop.client_cert, bufs_in_out, temporary_kv_assoc);
                server_cert = KamStringEval(arop.server_cert, bufs_in_out, temporary_kv_assoc);
                host_cert = KamStringEval(arop.hostname, bufs_in_out, temporary_kv_assoc);
                client_key = KamStringEval(arop.client_key, bufs_in_out, temporary_kv_assoc);
                client_method = KamStringEval(arop.client_method, bufs_in_out, temporary_kv_assoc);
                client_algo = KamStringEval(arop.client_algo, bufs_in_out, temporary_kv_assoc);
                check_server_cert = arop.check_server_cert;
            }
            break;

            case KAM_ActionTypes::SCM_GIVE:
            {
                thescmgive = KamStringEval(arop.default_value, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::SCM_GIVE;
            }
            break;

            case KAM_ActionTypes::VSCM_GIVE:
            {
                //se non trova torna init del tipo (stirnga vuota).
                if (arop.var_name.size()) {
                    auto val = ar.second.ar_vars.find ( KamStringEval(arop.var_name, bufs_in_out, temporary_kv_assoc));
                    thescmgive = ( val == ar.second.ar_vars.end())? arop.default_value : val->second;
                } else {
                    thescmgive = arop.default_value;
                }
                thescmgive= KamStringEval(thescmgive, bufs_in_out, temporary_kv_assoc);
                theAction = KAM_ActionTypes::SCM_GIVE;
            }
            break;

            case KAM_ActionTypes::SCM_MANAGE:
            {
                //abbiamo una lista di stringhe al posto di una semplice stringa
                for (auto & it: arop.scm_manage_params)
                {
                    string vs = EvalArObservation( vaeng, it, bufs_in_out, agent_id, session_id );
                    thescmmanage.push_back(KamStringEval(vs, bufs_in_out, temporary_kv_assoc));
                }
                theAction = KAM_ActionTypes::SCM_MANAGE;
            }
            break;

            case KAM_ActionTypes::VSCM_MANAGE:
            {
                //abbiamo una lista di assegnazioni! variabile = valore di default se non assegnata, altrimenti valore!
                for (auto it = arop.scm_manage_params.begin(); it != arop.scm_manage_params.end(); ++it)
                {
                    string v1 = (*it).args_s().front();
                    ++it;
                    string vs = EvalArObservation( vaeng, *it, bufs_in_out, agent_id, session_id );
                    auto val = ar.second.ar_vars.find ( v1 );
                    thescmmanage.push_back(KamStringEval((val == ar.second.ar_vars.end() || val->second.size() == 0) ? vs : val->second, bufs_in_out, temporary_kv_assoc));
                }
                theAction = KAM_ActionTypes::SCM_MANAGE;
            }
            break;

            case KAM_ActionTypes::ADD_VALUE_TO_SET:
            case KAM_ActionTypes::DEL_VALUE_FROM_SET:
            {
                vaeng.AddDelValueToSet ( arop, ar.second.ar_vars, vaeng, bufs_in_out, agent_id, session_id, temporary_kv_assoc );
            }
            break;

            default:
                //case KAM_ActionTypes::NOP:
                mtfa_log("Errore. Codice non trovato: %d\n", mtfa_enum_to_int<KAM_ActionTypes>(arop.action));
                break;
            }
        }

        // ora esegue le azioni!
        if ( b_bandwidth )
        {
            int cs = bufferevent_getfd ( bufs_in_out->p_buf_in );
            int sc = bufferevent_getfd ( bufs_in_out->p_buf_out );
            int dim = atoi ( thebandwidth.c_str() );
            setsockopt ( cs, SOL_SOCKET, SO_RCVBUF, &dim, sizeof ( dim ) );
            setsockopt ( sc, SOL_SOCKET, SO_RCVBUF, &dim, sizeof ( dim ) );
        }

        switch ( theAction )
        {
        case KAM_ActionTypes::TRACE:
        {
            /* Devo spostare la comunicazione sulla VA, indicando alla VA sia
             * il nome dell'insieme di stringhe da aggiornare, sia il nome del cookie che
             * intendo utilizzare
             * per questo
             * 1) memorizzo in via temporanea, solo per alcuni secondi, la comunicazione completa.
             *      Per ora supponiamo che la comunicazione completa non contempli la parte dati e quindi sia solo header!
             * 2) faccio una redirect alla VA su una pagina speciale a cui ho associato i dati
             * 3) la VA se non trova un cookie, lo crea, aggiorna la pagina speciale e poi fa la redirect al sito originale
             * 4) AM,
                    thevadomain
                    thecookiename
                    theset
             * */

            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            string id = mtfa_rand_string ( 16, 62 );
            //cout << __func__ << "<" << bufs_in_out->header_cs << ">" << endl;
            vaeng.trace_ds.Insert ( id, make_pair (
                                        Base64::encode ( thevadomain ) + ":" +
                                        Base64::encode ( thecookiename ) + ":" +
                                        Base64::encode ( theset ) + ":" +
                                        Base64::encode ( bufs_in_out->s_loc_uri ) + ":" +
                                        Base64::encode ( bufs_in_out->s_loc_full_host ),
                                        time ( nullptr ) ) );
            cout << __func__ << "." << "Set " << LbUser_VA_TraceQuery << "=" << id << endl;
            AnswerRedirect ( "http://" + thevadomain + LbUser_VA_TraceUrl + "?" + LbUser_VA_TraceQuery + "=" + id, "", bufs_in_out );

            return true;
        }
        break;

        case KAM_ActionTypes::CALL_LISP_FUNCTION:
        {
            return true;
        }
        break;
        case KAM_ActionTypes::HTTP_REDIRECT:
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            //redirect http, quindi fatto con la url
            AnswerRedirect ( thehttpredirect, "", bufs_in_out );

            return true;
        }
        break;

        case KAM_ActionTypes::TCP_REDIRECT_HOST:
        {
            //Redirect tcp, quindi fatto con la gohome
            T_V_Split vip = mtfa_string_split ( thetcpredirect, ":" );
            string ip_dest = vip[0];
            string port_dest = ( vip.size() > 1 ) ? ( vip[1] ) : "80";

            //cout << "Fa la redirect: " << ip_dest << ":" << port_dest << endl;
            GoHome ( bufs_in_out, ip_dest, port_dest, "", "", "", "", "", 0, ( server_cert.length() > 0 ) ? true : false, server_cert, host_cert, client_cert, client_key, check_server_cert );

            return true;
        }
        break;

        case KAM_ActionTypes::ANSWER:
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            //se answer è un file, lo leggo e rispondo con il file, altrimenti è una stringa!
            if ( mtfa_FileIo::Exists ( theanswer ) )
            {
                AnswerFile ( "HTTP/1.1 200 OK", theanswer, "", bufs_in_out, true );
            }
            else
            {
                AnswerHtml ( "HTTP/1.1 200 OK", "", theanswer, bufs_in_out, true );
            }

            return true;
        }
        break;

        case KAM_ActionTypes::SCM_GIVE:
        {
            //le risposte provengono dal lisp
            bufs_in_out->answer_is_needed=false;

            // in this case the data passed is the path of the url (document root in apache terms)
            AnswerFile ( "HTTP/1.1 200 OK", thescmgive + bufs_in_out->s_loc_url, "", bufs_in_out, true );

            return true;
        }
        break;

        case KAM_ActionTypes::SCM_MANAGE:
        {
            bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE | EV_TIMEOUT);
            bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE | EV_TIMEOUT);
            string command = "M;";
            for (auto & it: thescmmanage)
                command += Base64::encode(it) + ";";
            command.pop_back();

            char * pcommand = (char*)malloc(command.size()+1);
            strcpy(pcommand, command.c_str());
            pair<char*, T_EventoStruct *> *params = new pair<char*, T_EventoStruct*>();
            *params = make_pair(pcommand, bufs_in_out);
#if 0
            ExecuteLispCode(params);
#else

            glb_tpool->push_task( //enqueue(
                []
                (pair<char*, T_EventoStruct *>*params)
                {
                    ExecuteLispCode(params);
                    return;
                },
                params
            );
#endif

            return true;
        }
        break;

        default:
            ; //mtfa_log ( "switch case not expected: %d\n", mtfa_enum_to_int ( theAction ) );
        }
    }

    return false;
}

void VAEvaluateValidationRules ( string& vasaid, string& vaafkey, T_EventoStruct* bufs_in_out )
{
//Se mi arriva da ACMLIGHT, inutile elaborarla. Non parte nemmeno il thread.

//prima decifro!, ma solo per ssw
    string dec_data = "";
    mtfa_mat_cypher_ext* decyfrator = nullptr;
    if ( vasaid.compare ( "sondaagent1234567890" ) != 0 )
    {
        //Concateno idsonda con disturbatore per ottenere la chiave di decifra
        unsigned long long ullkey = strtoull ( vaafkey.c_str(), nullptr, 10 );
        unsigned long long crcullkey = fastcrc64::crc64speed_native ( 0, &ullkey, 8 );
        string kseed_h = vasaid;
        string kseed_l = string ( ( char* ) &crcullkey, ( ( char* ) &crcullkey ) + 8 );
        string kseed = kseed_h + kseed_l;
        decyfrator = new mtfa_mat_cypher_ext ( kseed.data(), kseed.size() );
        char* decyphered = decyfrator->cypher ( bufs_in_out->s_body.data(), bufs_in_out->s_body.size() );
        dec_data = string ( decyphered, decyphered + bufs_in_out->s_body.size() );
        //Libero la stringa decifrata. Il decifratore lo libero dopo la risposta
        free ( decyphered );
    }
    else
    {
        dec_data = bufs_in_out->s_body;
    }

    //cout << "DEC_DATA: " << dec_data << endl;

    list<VAobservations*>* lobss = BuildObservationFromSAData ( vaeng, vasaid, vaafkey, dec_data, bufs_in_out );

    if ( lobss->back()->size() == 0 )
    {
        auto obss = lobss->back();
        lobss->pop_back();
        delete obss;
    }

#if 0  //non threaded

    for ( auto it : *lobss )
    {
        if ( it )
        {
            if ( it->size() > 0 )
            {
                vaeng.Runner ( it );
                delete ( it );
            }
        }
    }

    //Svuoto!
    //    auto it = lobss->begin();
    //    while (it != lobss->end())
    //    {
    //        if (*it)
    //        {
    //            delete (*it);
    //            (*it)=nullptr;
    //        }
    //        ++it;
    //    }
    delete lobss;
#else
    //Per ogni osservazione verifico le validation rules ed eseguo le azioni corrispondenti
    auto runasthread = [] ( list<VAobservations*>* lobss ) -> void
    {
        //Questa è per la generazione dei validation report e le azioni da eseguire sulle variabili delle access rule
        for ( auto it : *lobss )
        {
            if ( it )
            {
                if ( it->size() > 0 )
                {
                    vaeng.Runner ( it );
                }

                delete it;
            }
        }

        delete lobss;
    };

    va_tpool->push_task( //enqueue(
        [runasthread]
        (list<VAobservations*>* lobss)
        {
            runasthread ( lobss );
        },
        lobss
    );
#endif

    //OK, la risposta per ora dipendente dalle VR! Potrei personalizzare anche in base all'id sonda
    if ( vasaid.compare ( "sondaagent1234567890" ) != 0 )
    {
        string answer = BuildASRules ( AS_Rules );
        //Cifro la risposta e metto lìheader che serve
        char* cyphered = decyfrator->cypher ( answer.data(), answer.size() );
        string c_answer = string ( cyphered, cyphered + answer.size() );
        delete cyphered;
        delete decyfrator;
        AnswerHtml ( "HTTP/1.1 200 OK\r\n", "Content-Type: application/octet-stream", c_answer, bufs_in_out, true );
        //cout << __func__ << ". given OBSs: " << answer << endl;
    }
    else
    {
        AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", "", bufs_in_out, true );
    }

    return;
}

