/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   lb_user.h
 * Author: arcieri
 *
 * Created on 30 giugno 2016, 12.56
 */

#pragma once

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
#include <json.hpp>
#include "listaeventi.h"

#include "network.h"
#include <mtfa_ph.h>
#include <crc.h>

#include <libguile.h>
#include <thread>

#include "version.h"

#include "parserds.h"

using namespace std;
using json = nlohmann::json;

void ManageLBUserArgs ( void * vconf );
void accept_cb ( struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *a, int slen, void *v_args );
void CloseOneAndWaitWriteOther ( bool isIn, T_EventoStruct * bufs_in_out );
void SetSocketNoDelay ( long sock );
void SetSocketReuse ( long sock );

void InitPools ( int size );
void ThPoolEnqueueLsp (
    T_EventoStruct * bufs_in_out, //The io pointers
    int direction, //0: recvd data from client, 1: recvd data from server
    struct evbuffer *src,
    void * ctx
);

void SendAnswerAndCloseAll ( T_EventoStruct * bufs_in_out, const char * dati, int len );
void SendAnswerAndNotClose ( T_EventoStruct * bufs_in_out, const char * dati, int len );
void SendFileAndCloseAll ( T_EventoStruct * bufs_in_out, const char * data, int datalen, int filenum, int offset, int length );
void SendFileAndNotClose ( T_EventoStruct * bufs_in_out, const char * data, int datalen, int filenum, int offset, int length );
void CloseOne ( bool isIn, T_EventoStruct * bufs_in_out );

void writecb_b_in ( struct bufferevent *b_in, void *ctx );
void writecb_b_out ( struct bufferevent *b_out, void *ctx );

void close_on_finished_writecb_b_in ( struct bufferevent *bev, void *ctx );
void close_on_finished_writecb_b_out ( struct bufferevent *bev, void *ctx );

void eventcb_b_out ( struct bufferevent *bev, short what, void *ctx );
void eventcb_b_in ( struct bufferevent *bev, short what, void *ctx );

void readcb_b_in ( struct bufferevent *b_in, void *ctx );
void readcb_b_out ( struct bufferevent *b_out, void *ctx );
void readcb_b_in_0 ( struct bufferevent *b_in, void *ctx );

void drained_writecb_b_out ( struct bufferevent *b_out, void *ctx );
void drained_writecb_b_in ( struct bufferevent *b_in, void *ctx );

bool VerificaValiditaBufferIn ( bool isRead, struct bufferevent *b_in, T_EventoStruct * bufs_in_out );
bool VerificaValiditaBufferOut ( bool isRead, struct bufferevent *b_out, T_EventoStruct * bufs_in_out );
void GoHome
(
    T_EventoStruct * bufs_in_out,
    const string &ip_dest,
    const string &port_dest,
    const string &s_loc_host,
    const string &s_loc_method,
    const string &s_loc_url,
    const string &sheader,
    const string &sbody,
    int src_data_to_pass,
    bool isSSL = false,
    const string server_cert = "",
    const string host_cert = "",
    const string client_cert = "",
    const string client_key = "",
    bool verify_server_cert = false,
    const string client_method="dtls",
    const string client_algo="ALL:!aNULL:RC4+RSA:+HIGH:+MEDIUM:+LOW:+EXP:+eNULL" //"HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4"
//        ,
//        struct evbuffer * src,
//        void * ctx
);
bool CloseOneIfEmpty ( bool isIn, T_EventoStruct * bufs_in_out );
string GetNextOutIp();
std::string GetCookieId ( std::string cvt_cookie );
void            AnswerRedirect(const std::string & where,
			       std::string headers,
			       T_EventoStruct * bufs_in_out);
void AnswerHtml ( string answer, string headers, const string &html, T_EventoStruct * bufs_in_out, bool do_close=true );
string BuildXmlAnswer ( const string &  code, const string &  subcode, const string & resto );
void AnswerFile ( string http_answer, string filename, string headers, T_EventoStruct * bufs_in_out, bool do_close=true );
void AnswerData ( string answer, string headers, const string & data, T_EventoStruct * bufs_in_out, bool do_close=true );
void AnswerJsonData ( string answer, string headers, const string & data, T_EventoStruct * bufs_in_out, bool do_close=true );
bool SendAll ( int sockfd, char * dati, int len );
string MySqlResultToString ( std::list<std::list<std::string> > & res_query );
string RunLispCommand ( std::list<string> commands );
string RunLispCommand ( string command );
bool GetHeadersDataSC ( char* headersPtr, int headersLen,
                        std::map<string, string> * current_headers,
                        int * status_code
                      );
bool GetHeadersDataCS ( char* headersPtr, int headersLen,
                        std::map<string, string> * current_query,
                        std::map<string, string> * current_headers,
                        std::map<string, string> * current_cookies,
                        string * s_loc_method,
                        string * s_loc_protocol,
                        string * s_loc_host,
                        string * s_loc_full_host,
                        string * s_loc_port,
                        string * s_loc_url,
                        string * s_loc_pars,
                        string * s_loc_tag,
                        string * s_loc_user_passwd
                      );
std::map<string, string> GetBodyData ( string parms );
void CheckAndLoadRules();
void UpdateEvBufferAnswer ( struct evbuffer* b_out_read, T_EventoStruct* bufs_in_out );
void UpdateEvBufferAnswer ( string& p, T_EventoStruct* bufs_in_out );
void UpdateEvBufferRequest ( string& p, T_EventoStruct* bufs_in_out );
void UpdateEvBufferRequest ( struct evbuffer* b_in_read, T_EventoStruct* bufs_in_out );

//Per la gestione delle comunicazioni con il thread Scheme
int CallDomainServerJson ( json & json_request, json & json_result );
