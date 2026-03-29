#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <mtfa_utils.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#ifndef __APPLE__
#include <linux/sockios.h>
#endif
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
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

#include <mtfa_utils.h>
#include <mtfafs.h>
//#include <mtfa_strings.h>

#include <mtfa_db.h>

//#include <NumberTheory.h>
//#include <LispMachine.h>
#include <ConfigFile.h>

#include "listaeventi.h"

#include <ULL_Rng.h>

#include <mtfa_threadpool.h>
#include "network.h"

using std::cout;
using std::cerr;
using std::cin;
using std::endl;


using namespace std;

std::map<string, string> GetIpAndInterfaces()
{
     std::map<string, string> ip_ifname;
     //Per prima cosa prendo la lista degli IP e dei nomi delle interfacce
     {
          struct ifaddrs *ifaddr, *ifa;
          int family, s;
          char host[NI_MAXHOST];

          if ( getifaddrs ( &ifaddr ) == -1 ) {
               perror ( "getifaddrs" );
               return ip_ifname;
          }

          /* Walk through linked list, maintaining head pointer so we
              can free list later */
          for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next ) {
               if ( ifa->ifa_addr == NULL ) {
                    continue;
               }

               family = ifa->ifa_addr->sa_family;

               // Gets only AF_INET* interface addresses
               if ( family == AF_INET || family == AF_INET6 ) {
                    s = getnameinfo ( ifa->ifa_addr,
                                      ( family == AF_INET ) ? sizeof ( struct sockaddr_in ) :
                                      sizeof ( struct sockaddr_in6 ),
                                      host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
                    if ( s != 0 ) {
                         printf ( "getnameinfo() failed: %s\n", gai_strerror ( s ) );
                         continue;
                    }
                    ip_ifname[host] = ifa->ifa_name;
               }
          }
          freeifaddrs ( ifaddr );
     }
     return ip_ifname;
}

string GetIp ( int a_socket, bool local )
{
     //local true: ip locale, false: ip remoto
     char iplocstr[INET6_ADDRSTRLEN];
     socklen_t len;
     struct sockaddr_storage addr;
     int port;

     len = sizeof addr;

     if ( local ) {
          getsockname ( a_socket, ( struct sockaddr* ) &addr, &len );
     } else {
          getpeername ( a_socket, ( struct sockaddr* ) &addr, &len );
     }

     // deal with both IPv4 and IPv6:
     if ( addr.ss_family == AF_INET ) {
          struct sockaddr_in *s = ( struct sockaddr_in * ) &addr;
          port = ntohs ( s->sin_port );
          inet_ntop ( AF_INET, &s->sin_addr, iplocstr, sizeof iplocstr );
     } else { // AF_INET6
          struct sockaddr_in6 *s = ( struct sockaddr_in6 * ) &addr;
          port = ntohs ( s->sin6_port );
          inet_ntop ( AF_INET6, &s->sin6_addr, iplocstr, sizeof iplocstr );
     }
     return string ( iplocstr );
}

//Data la lista degli ip della macchina e delle interfacce di rete, dimmi qual'\u00e8 il mac address di origine di questa comunicazione
string GetRemotePeerMac ( std::map<string, string> & ip_ifname, int the_socket )
{
#ifdef __APPLE__
     return "";
#else
     struct sockaddr_in client;
     int returnstatus = 0;

     //devo calcolare ip locale
     string local_ip = GetIp ( the_socket, true ); //local ip

     //Ok, conosco IP, cerco ora il primo device a cui questo IP \u00e8 collegato
     string device = ip_ifname[local_ip];

     int len = sizeof ( client );
     getpeername ( the_socket, ( struct sockaddr* ) &client, ( socklen_t* ) &len );

     struct arpreq arpreq_;

     bzero ( &arpreq_, sizeof ( struct arpreq ) );

     struct sockaddr_in *sin;
     sin = ( struct sockaddr_in * ) &arpreq_.arp_pa;
     sin->sin_family = AF_INET;
     sin->sin_addr = client.sin_addr;
     sin = ( struct sockaddr_in * ) &arpreq_.arp_ha;
     sin->sin_family = ARPHRD_ETHER;

     strcpy ( arpreq_.arp_dev, device.c_str() );

     if ( ioctl ( the_socket, SIOCGARP, &arpreq_ ) < 0 ) {
          //perror("ioctl");
          return "";
     }

     unsigned char *ptr = ( unsigned char * ) & ( arpreq_.arp_ha.sa_data[0] );
     char vmac[128];
     sprintf ( vmac, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr, * ( ptr+1 ), * ( ptr+2 ), * ( ptr+3 ), * ( ptr+4 ), * ( ptr+5 ) );
     //printf("MAC: %x:%x:%x:%x:%x:%x\n", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));

     string result = vmac;;
     //result.assign(ptr, &ptr[6]);
     return result;
#endif
}
