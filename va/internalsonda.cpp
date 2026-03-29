#include "VA_utils.h"
//#define DUMP_ALL 1

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/bufferevent_ssl.h>
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
#include <mutex>
//#include <pcrecpp.h>

#include <ConfigFile.h>
#include <ULL_Rng.h>
#include <mtfa_threadpool.h>
#include <mtfa_utils.h>
#include <c_http_parser.h>
#include <base64.h>
#include <mtfa_find.h>
#include <mtfa_db.h>
#include <mtfa_fs3.h>
#include "listaeventi.h"

#include "network.h"
#include "mtfa_ph.h"
#include "crc.h"

#include "libguile.h"
#include <thread>

#include "version.h"

#include "parserds.h"

#include "lb_user.h"
#include "rulemgr.h"
#include "VA_utils.h"

#include <json.hpp>
using json = nlohmann::json;

#include "internalsonda.h"


//5 minuti, trascorsi i quali la comunicazione la elaboro lo stesso
#define CONN_TIMEOUT 60

typedef struct __connection__
{
    bool terminated;
    time_t time;
    bool client2server;
    string data;
    string ipsrc;
    string ipdst;
    string portsrc;
    string portdst;
    string sesid;
    bool isContinuation;
} TConnection;

extern VAengine vaeng;
//extern mtfa_thread_pool_x* va_tpool;
extern string LbUser_VA_LocalSH_HTTPMethods;

static queue<pair<unsigned long long, TConnection>> connections;
static mutex connsMutex;
//static mutex dataAvailableMutex;
static mutex aggregatedConnectionsMutex;

static void SondaHWSimulator_AggregatedConsumer();
static void SondaHwSimulator_Consumer();
static VAobservations* BuildObservationsFromInternalData(TConnection tc);

void RunSondaHwSimulator()
{
    //questi sono due thread
    thread consumer (SondaHwSimulator_Consumer);
    thread aggregatedConsumer(SondaHWSimulator_AggregatedConsumer);
    consumer.detach();
    aggregatedConsumer.detach();
}


//Questa è la funzione più veloce che abbiamo!
void SondaHwSimulator_Producer(T_EventoStruct* bufs_in_out, bool client2server, char * data, int datalen, bool terminated)
{
    //Quando arriva il server2client allora il client2server è inviato.
    //Oppure quanto il server2client non arriva, per errore/chiusura o per timeout!
    //Il server2client è inviato quando arriva di nuovo un client2server oppure per errore/chiusura.
    //Un secondo processo si occupa di elaborare la coda e processarla per le VRules.
    //UniqueId è l'identificatore di sessione tcp
    
    if (!datalen)
        return;
    
//     if (!terminated)
//         cout << "Not terminated\n";
//     else
//         cout << "Terminated\n";
//     cout << "\tArrivato da " << client2server << ": " << string(data, datalen) << endl;
    
    //Arriva un nuovo pacco dati, lo appendo
    TConnection tc;
    tc.terminated = terminated;
    tc.time = time(nullptr);
    if (!terminated)
    {
        tc.client2server = client2server;
        tc.data.assign(data, datalen);
        tc.ipsrc=bufs_in_out->client_ip;
        tc.portsrc=to_string(bufs_in_out->client_port);
        tc.ipdst=(bufs_in_out->server_ip.size()==0)?bufs_in_out->listen_ip:bufs_in_out->server_ip;
        tc.portdst=to_string((bufs_in_out->server_ip.size()==0)?bufs_in_out->listen_port:bufs_in_out->server_port);
        tc.sesid=to_string(bufs_in_out->UniqueId);
        tc.isContinuation = false; //lo valuto dopo
    }
    
    connsMutex.lock();
    connections.push(make_pair(bufs_in_out->UniqueId, tc));
    connsMutex.unlock();
}

//In questo modo, aggregatedConnections, non mantiene l'ordinamento delle osservazioni!!! Lo devo fare come lista/coda
static list<pair<unsigned long long, queue<TConnection>>> aggregatedConnections;
static void SondaHwSimulator_Consumer()
{
    while (true)
    {
        //consumo fino al termine, uno alla volta
        connsMutex.lock();
        //Se ci sono dati, li prendo, altrimenti nulla
        if (connections.size() == 0)
        {
            connsMutex.unlock();
            sleep(1);
            continue;
        }
        
        //prendo il primo elemento e poi lo elaboro
        auto first = connections.front();
        connections.pop();
        connsMutex.unlock();  //Rilascio il più in fretta possibile!!

        //vediamo se è continuation
        bool domandaRispostaCorretta = false;
        {
            //cerco di capire se domanda, risposta o prosecuzione di comunicazione o chiusura
            if (first.second.client2server)
            {
                static pcre2_code* checkMethod = mtfa_compile_pattern ( "^(GET|POST|HEAD|PUT|DELETE|CONNECT|OPTIONS|TRACE|PATCH) " );
                if (mtfa_find_pattern(first.second.data.c_str(), checkMethod))
                {
                    //sembra "ragionevolmente" essere una richiesta corretta di accesso
                    //proseguo con l'analisi del dato
                    domandaRispostaCorretta = true;
                }
                else
                {
                    //cout << "ninso\n";
                }
            }
            else
            {
                static pcre2_code* checkAnswer = mtfa_compile_pattern ( "^HTTP/.\\.. [0-9]+" );
                if (mtfa_find_pattern(first.second.data.c_str(), checkAnswer))
                {
                    //sembra "ragionevolmente" essere una risposta corretta HTTP
                    //proseguo con l'analisi del dato
                    domandaRispostaCorretta = true;
                }
                else
                {
                    //cout << "ninso\n";
                }
            }
        }
        if (domandaRispostaCorretta)
            first.second.isContinuation=false;
        else
            first.second.isContinuation=true;

//         if (first.second.isContinuation)
//             cout << "Quine  \n";
        
        //Elaboro il primo elemento
        aggregatedConnectionsMutex.lock();
        auto found = aggregatedConnections.end();
        for (auto it=aggregatedConnections.begin(); it != aggregatedConnections.end(); it++ ) {
            if (it->first == first.first) {
                found = it;
                break;
            }
        }
        
        //auto found = aggregatedConnections.find(first.first); //Lo unique id
        if (found == aggregatedConnections.end())
        {
            //cout << "IIIIIIIIIIIInserisco: " << first.second.data.c_str() << "\n";
            queue<TConnection> ltc;
            ltc.push(first.second);
            //aggregatedConnections[first.first] = ltc;
            aggregatedConnections.push_back(make_pair(first.first, ltc));
        }
        else
        {
            //Se l'elemento che sto inserendo è la prosecuzione dell'ultimo elemento che avevo inserito nella coda, appendo
            auto & ltc = found->second;
            
            //cout << "AAAAAAAAAAAAAAppendo: " << first.second.data.c_str() << "\n";
            
            if (ltc.back().client2server == first.second.client2server)
            {
                //Appendo i dati Ma se la lunghezza dei dati è troppo elevata, non appendo nulla
                if (ltc.back().data.size() < 1000000)
                    ltc.back().data.append(first.second.data); //Gli altri campi sono inalterati tra due comunicazioni nello stesso verso
            } else {
                //if (domandaRispostaCorretta)
                    ltc.push(first.second);
            }
        }    
        aggregatedConnectionsMutex.unlock();
    }
}

static VAobservations* BuildObservationsFromInternalData(TConnection tc);

//Questo consuma i dati aggregati, se hanno un termine (timeout oppure close oppure comunicazione in verso opposto)
static void SondaHWSimulator_AggregatedConsumer()
{
    while (true)
    {
        aggregatedConnectionsMutex.lock();
        if (aggregatedConnections.size() == 0)
        {
            aggregatedConnectionsMutex.unlock();
            usleep(2000000); //Aspetto 2 secondi
            continue;
        }
        list<VAobservations*>* lobss = new list<VAobservations*>();

        auto pconn = aggregatedConnections.begin();
        while (pconn != aggregatedConnections.end())
        {
//             if (pconn->second.size() > 0 && pconn->second.front().data.size() > 0)
//                 cout << "<" << string(pconn->second.front().data.data(), pconn->second.front().data.size()) << ">" << endl;
            if (pconn->second.size() == 0)
            {
                //eliminata!
                pconn = aggregatedConnections.erase(pconn);
                continue;
            }
            if (pconn->second.front().terminated)
            {
                //Posso chiuderla qui! elimino la connessione o no? Potrei ma voglio comunque comunicare un terminated come osservazioni
                if (pconn->second.size() == 1)
                {
                    //eliminata!
                    //ho tutti i dati che servono, li utilizzo!
                    //cout << "1-osservazione: " << pconn->second.front().data.c_str() << endl;
                    VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                    if (obss)
                        lobss->push_back(obss);
                    
                    //elimino l'osservazione corrente
                    pconn = aggregatedConnections.erase(pconn);
                } else {
                    //no, ci sono altri dati, quindi due connessioni con lo stesso id
                    //elimino solo il corrente
                    //ho tutti i dati che servono, li utilizzo!
                    //cout << "2-osservazione: " << pconn->second.front().data.c_str() << endl;
                    VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                    if (obss)
                        lobss->push_back(obss);
                    
                    //elimino l'osservazione corrente
                    pconn->second.pop();
                }
                continue;
            }
            if (pconn->second.size() == 1) //ne ho solo una in coda.
            {
                if ((time(nullptr)-pconn->second.front().time)<=CONN_TIMEOUT)
                {
                    //il timeout non è ancora scaduto ed non ho un blocco di risposta. Posso provare ma solo se la comunicazione è completa
                    //verificare se la comunicazione è completa.
                    //siamo in http, allora dobbiamo avere un content-length e un header completo
                    static pcre2_code* getAllHeader= mtfa_compile_pattern ( "\r\n\r\n" );
                    if (mtfa_find_pattern(pconn->second.front().data, getAllHeader))
                    {
                        static pcre2_code* getContentLength= mtfa_compile_pattern ( "([Cc][Oo][Nn][Tt][Ee][Nn][Tt]-[Ll][Ee][Nn][Gg][Tt][Hh]: )([0-9]+)\r\n" );
                        list<string> ls = mtfa_get_all_submatching_patterns(pconn->second.front().data, getContentLength);
                        if (ls.size()==2)
                        {
                            //il content-length c'è, devo avere tot dati dopo il termine dell'header
                            int cl = stoi(ls.back());
                            //prendo tutto quello che c'è dopo header
                            int pos = pconn->second.front().data.find("\r\n\r\n");
                            if ((pconn->second.front().data.size()-pos-4)!= cl)
                            {
                                //cout << "I dati sono insufficienti: <\n" << pconn->second.front().data << "\n>" << endl;
                                //devo proseguire, non ho dati a sufficienza 
                            }
                            else
                            {
                                //ho tutti i dati che servono, li utilizzo!
                                //cout << "3-osservazione: " << pconn->second.front().data.c_str() << endl;
                                VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                                if (obss)
                                    lobss->push_back(obss);
                                
                                //elimino l'osservazione corrente
                                pconn->second.pop();
                                continue;
                            }
                        }
                        else
                        {
                            //il content-length non c'è, ma l'header c'è tutto. mi accontento? ma se ci fosse una parte dati?
                            //cout << "4-osservazione: " << pconn->second.front().data.c_str() << endl;
                            VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                            if (obss)
                                lobss->push_back(obss);
                            
                            //elimino l'osservazione corrente
                            pconn->second.pop();
                            continue;
                        }
                    }
                    else
                    {
                        //header non terminato, nulla posso, devo andare al prossimo blocco di osservazioni
                    }
                }
                else
                {
                    //Il timeout è scaduto, la elaboro comunque!! Non ho ricevuto la chiusura del canale
                    //cout << "5-osservazione: " << pconn->second.front().data.c_str() << endl;
                    VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                    if (obss)
                        lobss->push_back(obss);
                    
                    //elimino l'osservazione corrente
                    pconn->second.pop();
                    continue;
                }
            }
            else
            {   //ne ho più di una in coda, la elaboro
                //cout << "6-osservazione: " << pconn->second.front().data.c_str() << endl;
                VAobservations* obss = BuildObservationsFromInternalData(pconn->second.front());
                if (obss)
                    lobss->push_back(obss);
                
                //elimino l'osservazione corrente
                pconn->second.pop();
                continue;
            }
            
            //Se sono qui, allora devo andare alla prossima pconn poiché quella corrente non è valutabile o non più esistente
            pconn++;
        }
        aggregatedConnectionsMutex.unlock();


        //Per ogni osservazione verifico le validation rules ed eseguo le azioni corrispondenti
        for ( auto &it : *lobss )
        {
            //Stampo l'osservazione
//             {
//                 for (auto &it1: *it)
//                 {
//                     cout << "OBS: " << it1.first << "\t" <<  VA_ItemTypesToString(it1.second.itype) << "\t" << it1.second.value << endl;
//                 }
//                 cout << endl << endl;
//             }
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
        usleep(1000000); //Attendo 1 secondo!
    }
}

list<pair<string, string>> GetkeysFromJson(string sjson)
{
    function<list<pair<list<string>, string>>(const json &js, list<pair<list<string>, string>> & lls, list<string>&ls)> jparse = 
        [&jparse] (const json & js, list<pair<list<string>, string>> & lls, list<string>&ls)
    {
        if (js.is_object())
        {
            for (auto & it: js.items())
            {
                ls.push_back(it.key());
                jparse(it.value(), lls, ls);
                ls.pop_back();
            }
        } else if (js.is_array())
        {
            for (auto & it: js.items())
            {
                ls.push_back("["+it.key()+"]");
                jparse(it.value(), lls, ls);
                ls.pop_back();
            }
        } else {
            //questo è il valore!!!  //cout << js<< endl;
            pair<list<string>, string> pss;
            if (js.is_boolean())
                pss.second = js?"true":"false";
            else if (js.is_null())
                pss.second = "null";
            else if (js.is_number())
                pss.second = to_string(js);
            else
                pss.second = js;
                
            pss.first = ls;
            lls.push_back(pss);
        }
        return lls;
    };

    json js;
    try
    {
        //ogni volta che arrivo a un valore, allora termina la stringa delle chiavi di accesso
        //mtfa_log("La query JSON: %s\n", sjson.c_str());
        js = json::parse(sjson);
    } catch (exception ex)
    {
//         cout << "Errore json: " << ex.what() << endl;
//         for (int i=0; i<sjson.size(); ++i)
//         {
//             printf("%02X ", sjson[i]);
//         }
//         cout << endl;
//         cout << "<" << sjson << ">" << endl;
        return (list<pair<string, string>>());
    }
    list<pair<list<string>, string>> lls;
    list<string> ls;
    jparse(js, lls, ls);
    
    list<pair<string, string>> lp;
    for (auto it: lls)
    {
        string s;
        for (auto it1: it.first)
        {
            s += it1 + ".";
        }
        if (s.size()>0)
        {
            s.pop_back();
            lp.push_back(make_pair(s, it.second));
        }
    }
    return lp;
}

static pcre2_code* getMethod = mtfa_compile_pattern ( "^([^ ]+) " );
static pcre2_code* getHttpHost = mtfa_compile_pattern ( "[Hh][Oo][Ss][Tt]: (.*)\r\n$" );
static pcre2_code* getHttpUrlHost = mtfa_compile_pattern ( "[Hh][Tt][Tt][Pp]://([^/]*)" );

static pcre2_code* getHttpHeader = mtfa_compile_pattern ( "^(.*\r\n)*\r\n" );
static pcre2_code* getHttpData= mtfa_compile_pattern ( "^.*\r\n\r\n([\\x00-\\xFF]*)" );
static pcre2_code* getHttpCookie = mtfa_compile_pattern ( "[Cc][Oo][Oo][Kk][Ii][Ee]: (.*)[\r\n]+" );  //da header
static pcre2_code* getHttpSetCookie = mtfa_compile_pattern ( "([Ss][Ee][Tt]-[Cc][Oo][Oo][Kk][Ii][Ee]: .*)[\r\n]+" );

static VAobservations* BuildObservationsFromInternalData(TConnection tc)
{
    //preparo la prima osservazione e la inserisco, vuota, nella lista!
    VAobservations* obss = new VAobservations();
    
    //Questa è la parte fissa delle osservazioni!
    VAobservation obs_agent_id ( VA_ItemTypes::AGENT_ID, "InternalAgent", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::AGENT_ID ), obs_agent_id ));

    VAobservation obs_process_name ( VA_ItemTypes::SA_PROCESS_NAME, "InternalAgent", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_PROCESS_NAME ), obs_process_name ) );
    
    VAobservation obs ( VA_ItemTypes::SA_OS, "InternalAgent", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_OS ), obs ) );

    VAobservation obs_net_sesid ( VA_ItemTypes::NET_SESID, tc.sesid, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_SESID ), obs_net_sesid ) );
    
    VAobservation obs_etime ( VA_ItemTypes::OBS_ETIME, to_string(tc.time), unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_ETIME ), obs_etime ) );

    VAobservation obs_obs_id ( VA_ItemTypes::OBS_OBSID, "InternalObservation", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_OBSID ), obs_obs_id ) );
    
    VAobservation obs_time ( VA_ItemTypes::OBS_TIME, to_string(time(nullptr)), unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_TIME ), obs_time ) );
    
    VAobservation obs_logged_user ( VA_ItemTypes::SA_LOGGED_USER, "InternalAgent", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::SA_LOGGED_USER ), obs_logged_user ) );

    VAobservation obs_name ( VA_ItemTypes::OBS_NAME, "InternalObservation", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_NAME ), obs_name ) );
        
    VAobservation obs_ip_src ( VA_ItemTypes::NET_TCP_IPSRC, tc.ipsrc+":"+tc.portsrc, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPSRC ), obs_ip_src ) );

    VAobservation obs_ip_dst ( VA_ItemTypes::NET_TCP_IPDST, tc.ipdst+":"+tc.portdst, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_TCP_IPDST ), obs_ip_dst ) );

    VAobservation obs_net_result ( VA_ItemTypes::NET_RESULT, "TRUE", unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::NET_RESULT ), obs_net_result ) );

    if (tc.terminated)
    {
        VAobservation obs ( VA_ItemTypes::OBS_EVENT, "NET_TCP_CLOSE", unordered_map<string, string>() );  //"NET_TCP_SEND", "NET_TCP_RECV"
        obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_EVENT ), obs ) );
        return nullptr; //inutile proseguire.
    }
    if (tc.data.size()==0)
    {
        return nullptr; //inutile proseguire.
    }


    string s_method;
    string s_http_host;
    string s_http_url_host;
    string s_http_uri;
    string s_http_url;
    string s_http_query;
    string s_obs_event; //NET_TCP_RECV, NET_TCP_SEND
    unordered_map<string, string> umcookie;
    unordered_map<string, string> umhttpdata;
    unordered_map<string, string> umhttpquery;
    unordered_map<string, string> umhttpheader;
    
    string s_answercode;
    unordered_map<string, string> umanswercookie;
    unordered_map<string, string> umhttpanswerdata;
    unordered_map<string, string> umhttpanswerheader;
    
    if (tc.client2server)
    {
        //esaurisco: evemt, metodo, host, urlhost, uri, url, query, cookie, headers e data
        //cout << "DA CLIET A SERVER: " << tc.data << endl;
        T_V_Split all_requests = mtfa_string_split2 ( tc.data, "\r\n" );
        if (all_requests.size() == 0)
        {
            delete obss;
            return nullptr; //inutile proseguire.
        }
        s_obs_event="NET_TCP_SEND";

        //la prima riga è la parte metodo e host, le altre sono la parte header, la penultima è vuota, l'ultima è la parte data
        bool bRet = mtfa_get_specific_submatching_pattern(all_requests[0], getMethod, 1, s_method);

        mtfa_get_specific_submatching_pattern(all_requests[0], getHttpUrlHost, 1, s_http_url_host);
        
        //prendo uri e url, parto dalla posizione metod+1
        static pcre2_code* getHttpUri = mtfa_compile_pattern ( "^([^ ]+) " ); //da dopo metodo
	//Ma se non c'è URI?
	if (all_requests[0].size()==0) {
		s_http_uri="/";
	} else {
        	mtfa_get_specific_submatching_pattern(all_requests[0].substr(s_method.size()+1), getHttpUri, 1, s_http_uri);
	}
        auto pos = s_http_uri.find("?");
        if (pos != string::npos)
            s_http_url = s_http_uri.substr(0, pos);
        else
            s_http_url = s_http_uri;
        
        //E qui prendo la parte query
        if (pos != string::npos)
            s_http_query = s_http_uri.substr(pos+1);
        
        //Splitto la parte query, se presente (da gestire la presenza di json)
        if (s_http_query.size() > 0)
        {
            //Provo con il parsing del body
            list<pair<string, string>> lkv = GetkeysFromJson(s_http_query);
            if (lkv.size() > 0)
            {
                //è un json
                for (auto it: lkv)
                {
                    umhttpquery[it.first] = it.second;
                    //cout << it.first << ": " << it.second << endl;
                }
            } else {
                T_V_Split vs = mtfa_string_split(s_http_query, "&");
                for (int i=0; i<vs.size(); ++i)
                {
                    T_V_Split kv = mtfa_string_split(vs[i], "=");
                    if (kv.size() > 1)
                    {
                        umhttpquery[kv[0]] = urldecode(kv[1]);
                    }
                    else if (kv.size() > 0)
                    {
                        umhttpquery[kv[0]] = "";
                    }
                }
            }
            umhttpquery["0_FULL_QUERY"] = urldecode(s_http_query);
        }

        //per sapere se c'è una parte data, allora dobbiamo chiederci se l'ultima riga è vuota oppure no
        //errore! devo prendere la prima \r\n\r\n che trovo, se non la trovo, non ho parte dati e se la trovo e c'è qualcosa dopo, allora ho la parte chiave
        
        int dataIndex;
        int endHeaderIndex;
        dataIndex = -1;
        endHeaderIndex = all_requests.size();
        for (auto i=0; i<all_requests.size(); ++i)
        {
            if (all_requests[i].size() == 0)
            {
                dataIndex = i+1;
                endHeaderIndex = i-1;
                break;
            }
        }

        if ( all_requests.size() > 1)
        {
            try {
                umhttpheader["0_FULL_DATA"]="";
                umcookie["0_FULL_DATA"]="";
                for (int i=1; i<(endHeaderIndex+1); ++i)
                {
                    //Se inizia con Host o con Cookie, allora lo tratto diversamente dagli altri
                    //cout << "SIZEALLREQ: " << all_requests.size() << ". I: " << i << endl;
                    //cout << "ALLREQ: " << all_requests[i] << endl;
                    T_V_Split kv = mtfa_string_split2(all_requests[i], ": ");
                    if (kv.size() > 1)
                    {
                        umhttpheader[kv[0]] = kv[1];  //questa è la fine dei normali headers...
                        umhttpheader["0_FULL_DATA"] += all_requests[i] + "\n";
                        //mtfa_log("Got so far: %s\n", umhttpheader["0_FULL_DATA"].c_str());
                        if (kv[0].compare("Cookie")==0)
                        {
                            string value = kv[1];
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
                                    umcookie["0_FULL_DATA"] += it + "\n";
                                }
                            }
                        } else if (kv[0].compare("Host")==0)
                        {
                            s_http_host = kv[1];
                        }
                    }
		}
            } catch (...)
            {
                ;
            }
        }
        
        if (dataIndex>0)
        {
            //static pcre2_code* getContentDisposition = mtfa_compile_pattern ( "name=\"([^\"]*)\"[ \r\n]+([^\r\n]*)[\r\n]+" );
            //static pcre2_code* checkMultipart= mtfa_compile_pattern ( "multipart\\/form-data; *boundary=" );
            static pcre2_code * multipart_decode = mtfa_compile_pattern ( "multipart/([^;]*); *boundary=(.*)$");

            //tieni presente che la parte dati potrebbe essere più unga di una riga, quindi dataIndex è solo l'inizio
            string fullData;
            for (auto i=dataIndex; i<all_requests.size(); ++i)
                fullData += all_requests[i] + "\r\n";
            while (fullData.size() > 1 && fullData.substr(fullData.size()-2).compare("\r\n")==0)
            {
                fullData.pop_back();
                fullData.pop_back();
            }
            umhttpdata["0_FULL_DATA"] = fullData;
            
            auto intestazione = mtfa_get_all_submatching_patterns(umhttpheader["Content-Type"], multipart_decode);
            //Se ha matchato allor abbiamo sia type di multipart, sia il boundary!!!
            if (intestazione.size() == 3)
            {
                //è un multipart!
                auto it=intestazione.begin(); ++it;
                string tipo_multipart = *it; ++it;
                string boundary = "--" + *it;

                multipart::multipart_parser_settings callbacks;
                callbacks.lmr.clear();
                callbacks.on_body_end=nullptr;
                callbacks.on_header_field=nullptr;
                callbacks.on_header_value=nullptr;
                callbacks.on_headers_complete=nullptr;
                callbacks.on_part_data=nullptr;
                callbacks.on_part_data_begin=nullptr;
                callbacks.on_part_data_end=nullptr;

                callbacks.on_header_field = multipart::read_header_name;
                callbacks.on_header_value = multipart::read_header_value;
                callbacks.on_part_data = multipart::read_data;

                multipart::multipart_parser* parser = multipart::multipart_parser_init(boundary.c_str(), &callbacks);
                multipart::multipart_parser_execute(parser, fullData.c_str(), fullData.size());

                auto lmr = move(parser->settings->lmr);
                multipart::multipart_parser_free(parser);

                for (auto it: lmr)
                {
                    auto name=mtfa_string_split(it.value, "=");
                    if (name.size() > 1) //serve la parte dopo il simbolo "="
                    {
                        umhttpdata[name[1].substr(1, name[1].size() - 2)] = it.data;
                        umhttpdata[name[1].substr(1, name[1].size() - 2)+"DISPOSITION"] = it.name;
                    }
                }
            }
            else   //é un json???
            {
                //Provo con il parsing del body
                list<pair<string, string>> lkv = GetkeysFromJson(fullData);
                if (lkv.size() > 0)
                {
                    //è un json
                    for (auto it: lkv)
                    {
                        umhttpdata[it.first] = it.second;
                        //cout << it.first << ": " << it.second << endl;
                    }
                } else {
                    T_V_Split vs = mtfa_string_split(fullData, "&");
                    if (vs.size() > 0)
                    {
                        for (int i=0; i<vs.size(); ++i)
                        {
                            T_V_Split kv = mtfa_string_split(vs[i], "=");
                            if (kv.size()>1)
                            {
                                umhttpdata[kv[0]] = kv[1];
                            }
                            else if (kv.size() > 0)
                            {
                                umhttpdata[kv[0]] = "";
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        //esaurisco: answercode, answerheader, set-cookie e answerdata
        T_V_Split all_requests = mtfa_string_split2 ( tc.data, "\r\n" );
        if (all_requests.size() == 0)
        {
            delete obss;
            return nullptr; //inutile proseguire.
        }

        s_obs_event="NET_TCP_RECV";
        
        //la prima riga è la parte answer code
        static pcre2_code* getAnswerCode = mtfa_compile_pattern ( "^HTTP[^ ]* (.*)$" );
        bool bRet = mtfa_get_specific_submatching_pattern(all_requests[0], getAnswerCode, 1, s_answercode);
        //if (s_answercode.size()==0)
        //    cout << "QUI1\n";
        
        //Poi splittiamo in headers e data
        int dataIndex = -1;
        int endHeaderIndex = all_requests.size()-2;
        for (auto i=0; i<all_requests.size(); ++i)
        {
            if (all_requests[i].size() == 0)
            {
                dataIndex = i+1;
                endHeaderIndex = i-1;
                break;
            }
        }

        //facciamo gli headers
        umhttpanswerheader["0_FULL_DATA"]="";
        umanswercookie["0_FULL_DATA"]="";
        for (int i=1; i<endHeaderIndex; ++i)
        {
            T_V_Split kv = mtfa_string_split2(all_requests[i], ": ");
            umhttpanswerheader["0_FULL_DATA"] += all_requests[i] + "\n";
            if (kv.size() > 1)
            {
                umhttpanswerheader[kv[0]] = kv[1];
                if (kv[0].compare("Set-Cookie")==0)
                {
                    umanswercookie["0_FULL_DATA"] += kv[1] + "\n";
                    T_V_Split ck = mtfa_string_split(kv[1], "=");
                    if (ck.size() > 1)
                    {
                        umanswercookie[ck[0]] = ck[1];
                    }
                    else if (ck.size() > 0)
                    {
                        umanswercookie[ck[0]] = "";
                    }
                }
            }
        }

        //facciamo i dati
        if (dataIndex>0)
        {
            //static pcre2_code* getContentDisposition = mtfa_compile_pattern ( "name=\"([^\"]*)\"[ \r\n]+([^\r\n]*)[\r\n]+" );
            //static pcre2_code* checkMultipart= mtfa_compile_pattern ( "multipart\\/form-data; *boundary=" );
            static pcre2_code * multipart_decode = mtfa_compile_pattern ( "multipart/([^;]*); *boundary=(.*)$");

            //tieni presente che la parte dati potrebbe essere più unga di una riga, quindi dataIndex è solo l'inizio
            string fullData;
            for (auto i=dataIndex; i<all_requests.size(); ++i)
                fullData += all_requests[i] + "\r\n";
            while (fullData.size() > 1 && fullData.substr(fullData.size()-2).compare("\r\n")==0)
            {
                fullData.pop_back();
                fullData.pop_back();
            }
            umhttpanswerdata["0_FULL_DATA"] = fullData;
            
            auto intestazione = mtfa_get_all_submatching_patterns(umhttpheader["Content-Type"], multipart_decode);
            //Se ha matchato allor abbiamo sia type di multipart, sia il boundary!!!
            if (intestazione.size() == 3)
            {
                //è un multipart!
                auto it=intestazione.begin(); ++it;
                string tipo_multipart = *it; ++it;
                string boundary = "--" + *it;

                multipart::multipart_parser_settings callbacks;
                callbacks.lmr.clear();
                callbacks.on_body_end=nullptr;
                callbacks.on_header_field=nullptr;
                callbacks.on_header_value=nullptr;
                callbacks.on_headers_complete=nullptr;
                callbacks.on_part_data=nullptr;
                callbacks.on_part_data_begin=nullptr;
                callbacks.on_part_data_end=nullptr;

                callbacks.on_header_field = multipart::read_header_name;
                callbacks.on_header_value = multipart::read_header_value;
                callbacks.on_part_data = multipart::read_data;

                multipart::multipart_parser* parser = multipart::multipart_parser_init(boundary.c_str(), &callbacks);
                multipart::multipart_parser_execute(parser, fullData.c_str(), fullData.size());

                auto lmr = move(parser->settings->lmr);
                multipart::multipart_parser_free(parser);

                for (auto it: lmr)
                {
                    auto name=mtfa_string_split(it.value, "=");
                    if (name.size() > 1) //serve la parte dopo il simbolo "="
                    {
                        umhttpanswerdata[name[1].substr(1, name[1].size() - 2)] = it.data;
                        umhttpanswerdata[name[1].substr(1, name[1].size() - 2)+"DISPOSITION"] = it.name;
                    }
                }
            }
            else   //é un json???
            {
                //Provo con il parsing del body
                list<pair<string, string>> lkv = GetkeysFromJson(fullData);
                if (lkv.size() > 0)
                {
                    //è un json
                    for (auto it: lkv)
                    {
                        umhttpanswerdata[it.first] = it.second;
                        //cout << it.first << ": " << it.second << endl;
                    }
                } else {
                    T_V_Split vs = mtfa_string_split(fullData, "&");
                    if (vs.size() > 0)
                    {
                        for (int i=0; i<vs.size(); ++i)
                        {
                            T_V_Split kv = mtfa_string_split(vs[i], "=");
                            if (kv.size()>1)
                            {
                                umhttpanswerdata[kv[0]] = kv[1];
                            }
                            else if (kv.size() > 0)
                            {
                                umhttpanswerdata[kv[0]] = "";
                            }
                        }
                    }
                }
            }
        }
    }
    
    
    //event, metodo, host, urlhost, uri, url, query, cookie, headers e data
    VAobservation obs_1 ( VA_ItemTypes::OBS_EVENT, s_obs_event, unordered_map<string, string>() );  //"NET_TCP_SEND", "NET_TCP_RECV"
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::OBS_EVENT ), obs_1 ) );
    
    VAobservation obs_2 ( VA_ItemTypes::HTTP_METHOD, s_method, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_METHOD ), obs_2 ) );

    VAobservation obs_3 ( VA_ItemTypes::HTTP_HOST, s_http_host, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_HOST ), obs_3 ) );

    VAobservation obs_4 ( VA_ItemTypes::HTTP_URL_HOST, s_http_url_host, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL_HOST ), obs_4 ) );

    VAobservation obs_5 ( VA_ItemTypes::HTTP_URI, s_http_uri, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URI ), obs_5 ) );

    VAobservation obs_6 ( VA_ItemTypes::HTTP_URL, s_http_url, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_URL ), obs_6 ) );

    VAobservation obs_7 ( VA_ItemTypes::HTTP_QUERY, "", umhttpquery );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_QUERY), obs_7 ) );

    VAobservation obs_8 ( VA_ItemTypes::HTTP_COOKIES, "", umcookie);
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_COOKIES), obs_8 ) );

    VAobservation obs_9 ( VA_ItemTypes::HTTP_HEADERS, "", umhttpheader);
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_HEADERS), obs_9 ) );

    VAobservation obs_10 ( VA_ItemTypes::HTTP_DATA_POST, "", umhttpdata);
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_DATA_POST), obs_10 ) );


    //ora le osservazioni di ritorno (answer)
    VAobservation obs11 ( VA_ItemTypes::HTTP_ANSWER_CODE, s_answercode, unordered_map<string, string>() );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_CODE ), obs11 ) );

    VAobservation obs12 ( VA_ItemTypes::HTTP_ANSWER_HEADERS, "", umhttpanswerheader );
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_HEADERS ), obs12 ) );

    VAobservation obs13 ( VA_ItemTypes::HTTP_ANSWER_COOKIES, "", umanswercookie);
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_COOKIES ), obs13 ) );

    VAobservation obs14 ( VA_ItemTypes::HTTP_ANSWER_DATA, "", umhttpanswerdata);
    obss->insert ( make_pair ( mtfa_enum_to_int ( VA_ItemTypes::HTTP_ANSWER_DATA ), obs14 ) );
    
    return obss;
}
