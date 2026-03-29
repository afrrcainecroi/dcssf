/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   va_commands.h
 * Author: arcieri
 *
 * Created on 6 ottobre 2016, 13.24
 */

#pragma once

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

#include <libguile.h>
#include <time.h>

#include "lb_user.h"
#include "VAengine.h"

using namespace std;

void DoManageCommands ( VAengine * vaeng, T_EventoStruct * bufs_in_out );
void DoManageSvaRequests ( /*VAengine * vaeng, */T_EventoStruct * bufs_in_out );

//Una globale che si ricorda delle variazioni dei set ricevute dal presente KAM o VA
//indirizzo è un'identificativo unico (uuid) della transazione. Generato dalla VA che ha matchato il 
extern unordered_map<string, time_t> glb_set_updated;  //ricorda sia la stringa JSON, sia quando è stata definita la prima volta.

//Chiamate dalle VR
bool SendItemToSisterAndChildren(string setname, string item_key, string item_value, time_t duration, bool add);
bool SendItemToSisterAndChildren(string setname, string item_value, time_t duration, bool add);


