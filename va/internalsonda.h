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

//void ISAddBuffer(T_EventoStruct* bufs_in_out, bool client2server, char * data, int datalen);
void SondaHwSimulator_Producer(T_EventoStruct* bufs_in_out, bool client2server, char * data, int datalen, bool terminated=false);
void RunSondaHwSimulator();
