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
//#include <sys/types.h >
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

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
#include <regex>

#include <ULL_Rng.h>

#include <mtfa_threadpool.h>
#include <mtfa_utils.h>
#include <c_http_parser.h>
#include <base64.h>
#include <mtfa_find.h>
#include <mtfa_db.h>
#include <mtfa_fs3.h>

#include <mtfa_ph.h>
#include <crc.h>

#include "version.h"

using namespace std;

typedef enum __rule_kind__ {
    e_block,
    e_accept,
    e_give,
    e_manage,
    e_redirect,
} T_rule_kind;

typedef struct __a_rule__ {
    T_rule_kind action;
    list<string> action_parameters;
    bool is_change;
    list<std::pair<string, string>> change_parameters_cs; // Il primo carattere mi
    // dice a chi si applica
    // /C/H/B
    list<std::pair<string, string>> change_parameters_sc; // Il primo carattere mi
    // dice a chi si applica
    // /C/H/B
    string ip; // source ip checked
    string host; // dst host checked
    string url; // dst url checked
    map<string, string> mapof_chq; // cookie, headers and queries checked
} T_bar_rule;

typedef struct __cmd_and_pars__ {
    T_rule_kind what;
    list<string *> *parameters;
} T_cmd_and_pars;

typedef struct __rule_change__ {
    int apply_to; //'C': cookie, 'H': headers, 'B': body
    regex *pat_cs;
    regex *pat_sc;
    string *subst_cs;
    string *subst_sc;
    __rule_change__()
    {
        apply_to='C';
        pat_cs = nullptr;
        pat_sc = nullptr;
        subst_cs = nullptr;
        subst_sc = nullptr;
    }
    ~__rule_change__()
    {
        if ( pat_cs ) {
            delete pat_cs;
        }
        if ( pat_sc ) {
            delete pat_sc;
        }
        if ( subst_cs ) {
            delete subst_cs;
        }
        if ( subst_sc ) {
            delete subst_sc;
        }
        pat_cs = nullptr;
        pat_sc = nullptr;
        subst_cs = nullptr;
        subst_sc = nullptr;
    }
} T_rule_change;

typedef list<T_rule_change *> T_list_rule_change;

extern list<T_bar_rule *> parsed_rules;
