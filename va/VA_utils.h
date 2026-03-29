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
#include "listaeventi.h"

#include "network.h"
#include <mtfa_ph.h>
#include <crc.h>

#include <libguile.h>
#include <thread>

#include "version.h"

#include "parserds.h"

using namespace std;

string MakeHttpDateTime ( time_t now=time ( 0 ) );
extern unordered_map<string, string> glb_mimetypes;
void InitMimetypes();

//Dichiarate in main
void SetSocketOptions ( long sock );
void SetSocketNoDelay ( long sock );
void SetSocketReuse ( long sock );
void SetSocketUnblocking(int sock_descriptor);


//Il parser per il multipart boundary
namespace multipart {
    #include <stdlib.h>
    #include <ctype.h>
    
    typedef struct multipart_parser multipart_parser;
    typedef struct multipart_parser_settings multipart_parser_settings;
    typedef struct multipart_parser_state multipart_parser_state;
    
    typedef int (*multipart_data_cb) (multipart_parser*, const char *at, size_t length);
    typedef int (*multipart_notify_cb) (multipart_parser*);
    
    struct multipart_parser
    {
        void* data;
        
        size_t index;
        size_t boundary_length;
        
        unsigned char state;
        
        multipart_parser_settings* settings;
        
        char* lookbehind;
        char multipart_boundary[1];
    };
    
    
    struct multipart_parser_settings {
        multipart_data_cb on_header_field;
        multipart_data_cb on_header_value;
        multipart_data_cb on_part_data;
        
        multipart_notify_cb on_part_data_begin;
        multipart_notify_cb on_headers_complete;
        multipart_notify_cb on_part_data_end;
        multipart_notify_cb on_body_end;
        
        //La parte dati che viene caricata dalle callback
        //é una tupla che contiene: HeaderName, headervalue, Data e sono tre stringhe
        list<T_multipart_result> lmr;
    };
    
    multipart_parser* multipart_parser_init (const char *boundary, multipart_parser_settings* settings);
    
    void multipart_parser_free(multipart_parser* p);
    
    size_t multipart_parser_execute(multipart_parser* p, const char *buf, size_t len);
    
    void multipart_parser_set_data(multipart_parser* p, void* data);
    void * multipart_parser_get_data(multipart_parser* p);
    
    int read_data(multipart_parser* p, const char *at, size_t lenght);
    int read_header_name(multipart_parser* p, const char *at, size_t lenght);
    int read_header_value(multipart_parser* p, const char *at, size_t lenght);
};

