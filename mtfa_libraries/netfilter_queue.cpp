#if 1

#include <list>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <linux/netfilter.h>
#include <linux/types.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <thread>

#include "netfilter_queue.h"

using namespace std;

//#define PRINT 1

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN 6

/* Ethernet header */
struct sniff_ethernet {
     u_char ether_dhost[ETHER_ADDR_LEN]; /* destination host address */
     u_char ether_shost[ETHER_ADDR_LEN]; /* source host address */
     u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
     u_char ip_vhl; /* version << 4 | header length >> 2 */
     u_char ip_tos; /* type of service */
     u_short ip_len; /* total length */
     u_short ip_id; /* identification */
     u_short ip_off; /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
     u_char ip_ttl; /* time to live */
     u_char ip_p; /* protocol */
     u_short ip_sum; /* checksum */
     struct in_addr ip_src, ip_dst; /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
     u_short th_sport; /* source port */
     u_short th_dport; /* destination port */
     tcp_seq th_seq; /* sequence number */
     tcp_seq th_ack; /* acknowledgement number */
     u_char th_offx2; /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
     u_char th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
     u_short th_win; /* window */
     u_short th_sum; /* checksum */
     u_short th_urp; /* urgent pointer */
};

bool queue_manager::check_pkt ( struct nfq_data *tb, unsigned int &id, string & to_check )
{
     struct nfqnl_msg_packet_hdr *ph;
     struct nfqnl_msg_packet_hw *hwph;
     //uint32_t mark, ifi, uid, gid;
     bool ret = false;
     unsigned char *packet;

     to_check.clear();

     ph = nfq_get_msg_packet_hdr ( tb );
     if ( ph ) {
          id = ntohl ( ph->packet_id );
     } else {
          //non so cosa fare!
          id = 0;
          return false;
     }

     int packet_len = nfq_get_payload ( tb, &packet );
     if ( packet_len < 0 ) {
          //payload inaccessibile! Stop.
          return false;
     }

     //ora posso estrarre le informazioni che mi servono
     /*
      * Protocollo TCP!
      * mac address sorgente
      * ip sorgente
      * ip destinazione
      * porta destinazione
      */

     // declare pointers to packet headers
     const struct sniff_ethernet *ethernet; // The ethernet header [1]
     const struct sniff_ip *ip; // The IP header
     const struct sniff_tcp *tcp; // The TCP header
     const char *payload; // Packet payload

     int size_ip;
     int size_tcp;
     int size_payload;

     // define ethernet header
     ethernet = ( struct sniff_ethernet* ) ( packet );

     // define/compute ip header offset
     ip = ( struct sniff_ip* ) ( packet + SIZE_ETHERNET );
     size_ip = IP_HL ( ip ) *4;
     if ( size_ip < 20 ) {
          printf ( "   * Invalid IP header length: %d bytes\n", size_ip );
          return false;
     }

     /* print source and destination IP addresses */
     printf ( "       From: %s\n", inet_ntoa ( ip->ip_src ) );
     printf ( "         To: %s\n", inet_ntoa ( ip->ip_dst ) );

     /* determine protocol */
     switch ( ip->ip_p ) {
     case IPPROTO_TCP:
          printf ( "   Protocol: TCP\n" );
          break;
     case IPPROTO_UDP:
          printf ( "   Protocol: UDP\n" );
          return false;
     case IPPROTO_ICMP:
          printf ( "   Protocol: ICMP\n" );
          return false;
     case IPPROTO_IP:
          printf ( "   Protocol: IP\n" );
          return false;
     default:
          printf ( "   Protocol: unknown\n" );
          return false;
     }

     /*
      *  OK, this packet is TCP.
      */

     /* define/compute tcp header offset */
     tcp = ( struct sniff_tcp* ) ( packet + SIZE_ETHERNET + size_ip );
     size_tcp = TH_OFF ( tcp ) *4;
     if ( size_tcp < 20 ) {
          printf ( "   * Invalid TCP header length: %d bytes\n", size_tcp );
          return false;
     }

     printf ( "   Src port: %d\n", ntohs ( tcp->th_sport ) );
     printf ( "   Dst port: %d\n", ntohs ( tcp->th_dport ) );

     /* define/compute tcp payload (segment) offset */
     payload = ( char * ) ( packet + SIZE_ETHERNET + size_ip + size_tcp );

     /* compute tcp payload (segment) size */
     size_payload = ntohs ( ip->ip_len ) - ( size_ip + size_tcp );

     /*
      * Print payload data; it might be binary, so don't just
      * treat it as a string.
      */
     if ( size_payload > 0 ) {
          printf ( "   Payload (%d bytes):\n", size_payload );
          //print_payload(payload, size_payload);
     }

     //non accade mai!!
     return 0;
}

static uint32_t print_pkt ( struct nfq_data *tb )
{
     int id = 0;
     struct nfqnl_msg_packet_hdr *ph;
     struct nfqnl_msg_packet_hw *hwph;
     uint32_t mark, ifi, uid, gid;
     int ret;
     unsigned char *data, *secdata;

     ph = nfq_get_msg_packet_hdr ( tb );
     if ( ph ) {
          id = ntohl ( ph->packet_id );
#ifdef PRINT
          printf ( "hw_protocol=0x%04hx hook=%u id=%u ", ntohs ( ph->hw_protocol ), ( unsigned int ) ph->hook, ( unsigned int ) id );
#endif
     }
     return id;

     hwph = nfq_get_packet_hw ( tb );
     if ( hwph ) {
          int i, hlen = ntohs ( hwph->hw_addrlen );

#ifdef PRINT
          printf ( "hw_src_addr=" );
          for ( i = 0; i < hlen - 1; i++ ) {
               printf ( "%02x:", ( unsigned int ) hwph->hw_addr[i] );
          }
          printf ( "%02x ", ( unsigned int ) hwph->hw_addr[hlen - 1] );
#endif
     }

#ifdef PRINT
     mark = nfq_get_nfmark ( tb );
     if ( mark ) {
          printf ( "mark=%u ", mark );
     }

     ifi = nfq_get_indev ( tb );
     if ( ifi ) {
          printf ( "indev=%u ", ifi );
     }

     ifi = nfq_get_outdev ( tb );
     if ( ifi ) {
          printf ( "outdev=%u ", ifi );
     }
     ifi = nfq_get_physindev ( tb );
     if ( ifi ) {
          printf ( "physindev=%u ", ifi );
     }

     ifi = nfq_get_physoutdev ( tb );
     if ( ifi ) {
          printf ( "physoutdev=%u ", ifi );
     }
#endif


     //    if (nfq_get_uid(tb, &uid))
     //        printf("uid=%u ", uid);
     //
     //    if (nfq_get_gid(tb, &gid))
     //        printf("gid=%u ", gid);
     //
     //    ret = nfq_get_secctx(tb, &secdata);
     //    if (ret > 0)
     //        printf("secctx=\"%.*s\" ", ret, secdata);

#ifdef PRINT
     ret = nfq_get_payload ( tb, &data );
     if ( ret >= 0 ) {
          printf ( "payload_len=%d ", ret );
     }

     fputc ( '\n', stdout );
#endif
     ret = nfq_get_payload ( tb, &data );
     if ( ret >= 0 ) {
          //printf("payload_len=%d\n", ret);
     }

     return id;
}

int queue_manager::cb ( struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data )
{
     uint32_t id;
     T_queue_manager_container * info = ( T_queue_manager_container* ) data;

     auto ph = nfq_get_msg_packet_hdr ( nfa );
     id = ntohl ( ph->packet_id );

     bool do_accept = true; //check_pkt(nfa, id);
     //uint32_t id = print_pkt(nfa);
     if ( do_accept ) {
          return nfq_set_verdict ( qh, id, NF_ACCEPT, 0, NULL );
     } else {
          return nfq_set_verdict ( qh, id, NF_DROP, 0, NULL );
     }
}

queue_manager::queue_manager ( list<int> queues, int _queue_len, int _bufsize )
{
     running_queues = new T_queue_manager_container[queues.size()]();
     num_queues = queues.size();
     queue_len = _queue_len;
     bufsize = _bufsize;
     stop = false;
     ds = new ImplicitDs();

     int id = 0;
     for ( auto it : queues ) {
          thread t ( do_manage_queue, it, id++, this );
          t.detach();
     }
}

int queue_manager::do_manage_queue ( unsigned int queue, int id, queue_manager * current_object )
{
     auto & co = current_object->running_queues[id];
     co.ds = current_object->ds;

     //    printf("opening library handle\n");
     co.h = nfq_open();
     if ( !co.h ) {
          fprintf ( stderr, "error during nfq_open()\n" );
          return -1;
     }

     //    printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
     if ( nfq_unbind_pf ( co.h, AF_INET ) < 0 ) {
          fprintf ( stderr, "error during nfq_unbind_pf()\n" );
          nfq_close ( co.h );
          return -1;
     }
     if ( nfq_unbind_pf ( co.h, AF_INET6 ) < 0 ) {
          fprintf ( stderr, "error during nfq_unbind_pf()\n" );
          nfq_close ( co.h );
          return -1;
     }

     //    printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
     if ( nfq_bind_pf ( co.h, AF_INET ) < 0 ) {
          fprintf ( stderr, "error during nfq_bind_pf()\n" );
          nfq_close ( co.h );
          return -1;
     }
     if ( nfq_bind_pf ( co.h, AF_INET6 ) < 0 ) {
          fprintf ( stderr, "error during nfq_bind_pf()\n" );
          nfq_close ( co.h );
          return -1;
     }

     //    printf("binding this socket to queue '%u'\n", queue);

     co.qh = nfq_create_queue ( co.h, queue, cb, ( void* ) &co );
     if ( !co.qh ) {
          fprintf ( stderr, "error during nfq_create_queue()\n" );
          nfq_close ( co.h );
          return -1;
     }

     //    printf("setting copy_packet mode\n");
     if ( nfq_set_mode ( co.qh, NFQNL_COPY_PACKET, 0xffff ) < 0 ) {
          fprintf ( stderr, "can't set copy mode\n" );
          nfq_destroy_queue ( co.qh );
          nfq_close ( co.h );
          return -1;
     }

     //    printf("setting flags to request UID and GID\n");
     //    if (nfq_set_queue_flags(qh, NFQA_CFG_F_UID_GID, NFQA_CFG_F_UID_GID))
     //    {
     //        fprintf(stderr, "This kernel version does not allow to retrieve process UID/GID.\n");
     //    }
     //
     //    printf("setting flags to request security context\n");
     //    if (nfq_set_queue_flags(qh, NFQA_CFG_F_SECCTX, NFQA_CFG_F_SECCTX))
     //    {
     //        fprintf(stderr, "This kernel version does not allow to retrieve security context.\n");
     //    }

     //nfnl_set_rcv_buffer_size(nfq_nfnlh(co.h), current_object->bufsize); //65536); //*20xffff + (MNL_SOCKET_BUFFER_SIZE/2));
     nfnl_rcvbufsiz ( nfq_nfnlh ( co.h ), current_object->bufsize ); //65536); //*20xffff + (MNL_SOCKET_BUFFER_SIZE/2));
     nfq_set_queue_maxlen ( co.qh, current_object->queue_len ); //16384);

     co.fd = nfq_fd ( co.h );

     int to_set = 0;
     //    setsockopt(co.fd, SOL_NETLINK, NETLINK_NO_ENOBUFS, &to_set, sizeof (int));

     to_set = 1;
     //    setsockopt(co.fd, SOL_NETLINK, NETLINK_BROADCAST_SEND_ERROR, &to_set, sizeof(int));

     //    uint32_t flags = NFQA_CFG_F_FAIL_OPEN;
     //    uint32_t mask = NFQA_CFG_F_FAIL_OPEN;
     //    int r = nfq_set_queue_flags(co.qh, mask, flags);

     struct timeval timeout;
     timeout.tv_sec = 0;
     timeout.tv_usec = 100000;

     if ( setsockopt ( co.fd, SOL_SOCKET, SO_RCVTIMEO, ( char * ) &timeout, sizeof ( timeout ) ) < 0 ) {
          perror ( "setsockopt failed\n" );
     }

     //    fd_set rfds;
     //    struct timeval tv;
     //    int retval;

     while ( !current_object->stop ) {
          //        FD_ZERO(&rfds);
          //        FD_SET(co.fd, &rfds);
          //
          //        /* Wait up to 1/10 seconds. */
          //        tv.tv_sec = 0; tv.tv_usec = 100000;
          //
          //        retval = select(1, &rfds, NULL, NULL, &tv);
          //
          //        if (retval == -1)
          //        {
          //            perror("select()");
          //            break;
          //        }
          //        else if (retval)
          //        {
          //            printf("Data is available now.\n");
          //        }
          //        else
          //        {
          //            //printf("No data within 1/10 seconds.\n");
          //            continue;
          //        }

          //Questo non stoppa in quanto la select è andata a buon fine!
          if ( ( co.rv = recv ( co.fd, co.buf, sizeof ( co.buf ), 0 ) ) >= 0 ) {
               //printf("pkt received: %d\n", co.rv);
               //potrei metterlo in una coda e poi gestirlo! Ma io sono molto veloce!
               nfq_handle_packet ( co.h, co.buf, co.rv );
               continue;
          }
          /* if your application is too slow to digest the packets that
           * are sent from kernel-space, the socket buffer that we use
           * to enqueue packets may fill up returning ENOBUFS. Depending
           * on your application, this error may be ignored. Please, see
           * the doxygen documentation of this library on how to improve
           * this situation.
           */
          if ( co.rv < 0 && ( errno == ENOBUFS || errno == EWOULDBLOCK ) ) {
               if ( errno == ENOBUFS ) {
                    printf ( "losing packets!\n" );
               }
               continue;
          }
          perror ( "recv failed" );
          break;
     }

     //    printf("unbinding from queue 0\n");
     nfq_destroy_queue ( co.qh );

#ifdef INSANE
     /* normally, applications SHOULD NOT issue this command, since
      * it detaches other programs/sockets from AF_INET, too ! */
     printf ( "unbinding from AF_INET\n" );
     nfq_unbind_pf ( h, AF_INET );
#endif

     //    printf("closing library handle\n");
     nfq_close ( co.h );

     co.closed = true;

     //printf("Closed\n");

     return 0;
}

void queue_manager::stop_queue_manager()
{
     stop = true;
     int all_stopped = 0;
     while ( all_stopped != num_queues ) {
          all_stopped = 0;
          for ( int id = 0; id < num_queues; ++id ) {
               //printf("Check queue n. %d\n", id);
               if ( !running_queues[id].closed ) {
                    shutdown ( running_queues[id].fd, SHUT_RD );
               } else {
                    all_stopped++;
               }
          }
          usleep ( 10000 ); //01 millisec
     }
     delete running_queues;
}

queue_manager::~queue_manager()
{
     if ( ds ) {
          delete ds;
     }
     ds = nullptr;
     stop_queue_manager();
}

bool queue_manager::add_rule ( const string &src_ip, const string &dst_ip, const string &dst_port )
{
     //Costruisce la stringa di match
     char p[12];
     unsigned int * psrc = ( unsigned int* ) &p[0];
     unsigned int * pdst = ( unsigned int* ) &p[4];
     unsigned short * pport = ( unsigned short* ) &p[8];

     *psrc = inet_addr ( src_ip.c_str() );
     *pdst = inet_addr ( dst_ip.c_str() );
     unsigned short usport = strtoul ( dst_port.c_str(), nullptr, 10 );
     //    cout << __func__   << "PORT: " << dst_port << ": " << usport << endl;
     *pport = htons ( usport );
     return ds->Insert ( p, 10, ( void* ) 1 );
}

bool queue_manager::del_rule ( const string &src_ip, const string &dst_ip, const string &dst_port )
{
     //Costruisce la stringa di match
     char p[12];
     unsigned int * psrc = ( unsigned int* ) &p[0];
     unsigned int * pdst = ( unsigned int* ) &p[4];
     unsigned short * pport = ( unsigned short* ) &p[8];

     *psrc = inet_addr ( src_ip.c_str() );
     *pdst = inet_addr ( dst_ip.c_str() );
     *pport = htons ( atoi ( dst_port.c_str() ) );
     return ds->Insert ( p, 10, ( void* ) 0 );
}

string queue_manager::list_rules()
{
     string ret = "";
     std::set<std::pair < std::string, void *>> sp = ds->GetAll();
     for ( auto it : sp ) {
          if ( it.second == ( void* ) 1 ) {
               char b[256];
               unsigned char p[10];
               memcpy ( p, it.first.data(), 10 );
               sprintf ( b, "%u.%u.%u.%u -> %u.%u.%u.%u:%hu",
                         p[0],
                         p[1],
                         p[2],
                         p[3],
                         p[4],
                         p[5],
                         p[6],
                         p[7],
                         ( unsigned short ) ( ( p[8] << 8 ) | p[9] )
                       );
               ret.append ( string ( b ) );
               ret.append ( "\n" );
          }
     }
     return ret;
}

#else

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#include <libmnl/libmnl.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nfnetlink.h>

#include <linux/types.h>
#include <linux/netfilter/nfnetlink_queue.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#include <list>
#include <thread>
#include <mtfa_fs3.h>

using namespace std;

#include "netfilter_queue.h"

struct nlmsghdr * queue_manager::nfq_hdr_put ( char *buf, int type, uint32_t queue_num )
{
     struct nlmsghdr *nlh = mnl_nlmsg_put_header ( buf );
     nlh->nlmsg_type = ( NFNL_SUBSYS_QUEUE << 8 ) | type;
     nlh->nlmsg_flags = NLM_F_REQUEST;

     struct nfgenmsg *nfg = ( struct nfgenmsg * ) mnl_nlmsg_put_extra_header ( nlh, sizeof ( *nfg ) );
     nfg->nfgen_family = AF_UNSPEC;
     nfg->version = NFNETLINK_V0;
     nfg->res_id = htons ( queue_num );

     return nlh;
}

int queue_manager::queue_cb ( const struct nlmsghdr *nlh, void *data )
{
     auto co = ( T_queue_manager_container* ) data;

     struct nfqnl_msg_packet_hdr *ph = NULL;
     struct nlattr * attr[NFQA_MAX + 1] = {};
     uint32_t id = 0, skbinfo;
     struct nfgenmsg *nfg;
     uint16_t plen;

     if ( nfq_nlmsg_parse ( nlh, attr ) < 0 ) {
          perror ( "problems parsing" );
          return MNL_CB_ERROR;
     }

     nfg = ( struct nfgenmsg * ) mnl_nlmsg_get_payload ( nlh );

     if ( attr[NFQA_PACKET_HDR] == NULL ) {
          fputs ( "metaheader not set\n", stderr );
          return MNL_CB_ERROR;
     }

     ph = ( struct nfqnl_msg_packet_hdr* ) mnl_attr_get_payload ( attr[NFQA_PACKET_HDR] );

     plen = mnl_attr_get_payload_len ( attr[NFQA_PAYLOAD] );
     /* void *payload = mnl_attr_get_payload(attr[NFQA_PAYLOAD]); */

     skbinfo = attr[NFQA_SKB_INFO] ? ntohl ( mnl_attr_get_u32 ( attr[NFQA_SKB_INFO] ) ) : 0;

     if ( attr[NFQA_CAP_LEN] ) {
          uint32_t orig_len = ntohl ( mnl_attr_get_u32 ( attr[NFQA_CAP_LEN] ) );
          if ( orig_len != plen ) {
               printf ( "truncated\n" );
          }
     }

     if ( skbinfo & NFQA_SKB_GSO ) {
          printf ( "GSO\n" );
     }

     id = ntohl ( ph->packet_id );
     //	printf("packet received (id=%u hw=0x%04x hook=%u, payload len %u", id, ntohs(ph->hw_protocol), ph->hook, plen);

     /*
      * ip/tcp checksums are not yet valid, e.g. due to GRO/GSO.
      * The application should behave as if the checksums are correct.
      *
      * If these packets are later forwarded/sent out, the checksums will
      * be corrected by kernel/hardware.
      */
     //	if (skbinfo & NFQA_SKB_CSUMNOTREADY)
     //		printf(", checksum not ready");
     //	puts(")");

     //queue_manager::nfq_send_verdict(ntohs(nfg->res_id), id);

     //void nfq_send_verdict(int queue_num, uint32_t id)
     {
          int queue_num = ntohs ( nfg->res_id );
          char buf[MNL_SOCKET_BUFFER_SIZE];
          struct nlmsghdr *nlh;

          nlh = nfq_hdr_put ( buf, NFQNL_MSG_VERDICT, queue_num );
          nfq_nlmsg_verdict_put ( nlh, id, NF_ACCEPT );

          if ( mnl_socket_sendto ( co->nl, nlh, nlh->nlmsg_len ) < 0 ) {
               perror ( "mnl_socket_send" );
               //exit(EXIT_FAILURE);
               return MNL_CB_ERROR;
          }
     }

     return MNL_CB_OK;
}

queue_manager::queue_manager ( list<int> _queues, int _queue_len, int _bufsize )
{
     running_queues = new T_queue_manager_container[_queues.size()]();
     num_queues = _queues.size();
     queue_len = _queue_len;
     bufsize = _bufsize;
     stop = false;
     ds = new ImplicitDs();

     int id = 0;
     for ( auto it : _queues ) {
          thread t ( do_manage_queue, it, id++, this );
          t.detach();
     }
}

void queue_manager::do_manage_queue ( unsigned int queue_num, int id, queue_manager * current_object )
{
     auto & co = current_object->running_queues[id];
     co.ds = current_object->ds;

     char *buf;
     /* largest possible packet payload, plus netlink data overhead: */
     size_t sizeof_buf = 0xffff + ( MNL_SOCKET_BUFFER_SIZE / 2 );
     struct nlmsghdr *nlh;
     int ret;
     unsigned int portid;

     co.nl = mnl_socket_open ( NETLINK_NETFILTER );
     if ( co.nl == NULL ) {
          perror ( "mnl_socket_open" );
          return;
     }

     if ( mnl_socket_bind ( co.nl, 0, MNL_SOCKET_AUTOPID ) < 0 ) {
          perror ( "mnl_socket_bind" );
          mnl_socket_close ( co.nl );
          return;
     }
     portid = mnl_socket_get_portid ( co.nl );

     buf = ( char* ) malloc ( sizeof_buf );
     if ( !buf ) {
          perror ( "allocate receive buffer" );
          mnl_socket_close ( co.nl );
          return;
     }

     /* PF_(UN)BIND is not needed with kernels 3.8 and later */
     nlh = nfq_hdr_put ( buf, NFQNL_MSG_CONFIG, 0 );
     nfq_nlmsg_cfg_put_cmd ( nlh, AF_INET, NFQNL_CFG_CMD_PF_UNBIND );

     if ( mnl_socket_sendto ( co.nl, nlh, nlh->nlmsg_len ) < 0 ) {
          perror ( "mnl_socket_send" );
          free ( buf );
          mnl_socket_close ( co.nl );
          return;
     }

     nlh = nfq_hdr_put ( buf, NFQNL_MSG_CONFIG, 0 );
     nfq_nlmsg_cfg_put_cmd ( nlh, AF_INET, NFQNL_CFG_CMD_PF_BIND );

     if ( mnl_socket_sendto ( co.nl, nlh, nlh->nlmsg_len ) < 0 ) {
          perror ( "mnl_socket_send" );
          free ( buf );
          mnl_socket_close ( co.nl );
          return;
     }

     nlh = nfq_hdr_put ( buf, NFQNL_MSG_CONFIG, queue_num );
     nfq_nlmsg_cfg_put_cmd ( nlh, AF_INET, NFQNL_CFG_CMD_BIND );

     if ( mnl_socket_sendto ( co.nl, nlh, nlh->nlmsg_len ) < 0 ) {
          perror ( "mnl_socket_send" );
          free ( buf );
          mnl_socket_close ( co.nl );
          return;
     }

     nlh = nfq_hdr_put ( buf, NFQNL_MSG_CONFIG, queue_num );
     nfq_nlmsg_cfg_put_params ( nlh, NFQNL_COPY_PACKET, 0xffff );

     mnl_attr_put_u32 ( nlh, NFQA_CFG_QUEUE_MAXLEN, htonl ( current_object->num_queues ) );

     //    nfnl_rcvbufsiz(nfq_nfnlh(nlh), current_object->bufsize); //65536); //*20xffff + (MNL_SOCKET_BUFFER_SIZE/2));
     //    nfq_set_queue_maxlen(nlh, current_object->queue_len); //16384);

     mnl_attr_put_u32 ( nlh, NFQA_CFG_FLAGS, htonl ( NFQA_CFG_F_GSO ) );
     mnl_attr_put_u32 ( nlh, NFQA_CFG_MASK, htonl ( NFQA_CFG_F_GSO ) );

     if ( mnl_socket_sendto ( co.nl, nlh, nlh->nlmsg_len ) < 0 ) {
          perror ( "mnl_socket_send" );
          free ( buf );
          mnl_socket_close ( co.nl );
          return;
     }

     /* ENOBUFS is signalled to userspace when packets were lost
      * on kernel side.  In most cases, userspace isn't interested
      * in this information, so turn it off.
      */
     //    ret = 1;
     //    mnl_socket_setsockopt(co.nl, NETLINK_NO_ENOBUFS, &ret, sizeof (int));

     co.fd = mnl_socket_get_fd ( co.nl );

     for ( ;; ) {
          ret = mnl_socket_recvfrom ( co.nl, buf, sizeof_buf );
          if ( ret == -1 && errno == ENOBUFS ) {
               perror ( "mnl_socket ENOBUFS" );
               continue;
          }

          if ( ret == -1 ) {
               perror ( "mnl_socket_recvfrom" );
               free ( buf );
               mnl_socket_close ( co.nl );
               return;
          }

          ret = mnl_cb_run ( buf, ret, 0, portid, queue_cb, &co );
          if ( ret < 0 ) {
               perror ( "mnl_cb_run" );
               free ( buf );
               mnl_socket_close ( co.nl );
               return;
          }
     }

     co.closed = true;
     free ( buf );
     mnl_socket_close ( co.nl );
     return;
}

void queue_manager::stop_queue_manager()
{
     stop = true;
     int all_stopped = 0;
     while ( all_stopped != num_queues ) {
          all_stopped = 0;
          for ( int id = 0; id < num_queues; ++id ) {
               //printf("Check queue n. %d\n", id);
               if ( !running_queues[id].closed ) {
                    shutdown ( running_queues[id].fd, SHUT_RD );
               } else {
                    all_stopped++;
               }
          }
          usleep ( 10000 ); //01 millisec
     }
     delete running_queues;
}

queue_manager::~queue_manager()
{
     if ( ds ) {
          delete ds;
     }
     ds = nullptr;
     stop_queue_manager();
}

bool queue_manager::add_rule ( const string src_ip, const string dst_ip, const string dst_port )
{
     //Costruisce la stringa di match
     char p[12];
     unsigned int * psrc = ( unsigned int* ) &p[0];
     unsigned int * pdst = ( unsigned int* ) &p[4];
     unsigned short * pport = ( unsigned short* ) &p[8];

     *psrc = inet_addr ( src_ip.c_str() );
     *pdst = inet_addr ( dst_ip.c_str() );
     unsigned short usport = strtoul ( dst_port.c_str(), nullptr, 10 );
     //    cout << __func__   << "PORT: " << dst_port << ": " << usport << endl;
     *pport = htons ( usport );
     return ds->Insert ( p, 10, ( void* ) 1 );
}

bool queue_manager::del_rule ( const string src_ip, const string dst_ip, const string dst_port )
{
     //Costruisce la stringa di match
     char p[12];
     unsigned int * psrc = ( unsigned int* ) &p[0];
     unsigned int * pdst = ( unsigned int* ) &p[4];
     unsigned short * pport = ( unsigned short* ) &p[8];

     *psrc = inet_addr ( src_ip.c_str() );
     *pdst = inet_addr ( dst_ip.c_str() );
     *pport = htons ( atoi ( dst_port.c_str() ) );
     return ds->Insert ( p, 10, ( void* ) 0 );
}

string queue_manager::list_rules()
{
     string ret = "";
     std::set<std::pair < std::string, void *>> sp = ds->GetAll();
     for ( auto it : sp ) {
          if ( it.second == ( void* ) 1 ) {
               char b[256];
               unsigned char p[10];
               memcpy ( p, it.first.data(), 10 );
               sprintf ( b, "%u.%u.%u.%u -> %u.%u.%u.%u:%hu",
                         p[0],
                         p[1],
                         p[2],
                         p[3],
                         p[4],
                         p[5],
                         p[6],
                         p[7],
                         ( p[8] << 8 ) | p[9]
                       );
               ret.append ( string ( b ) );
               ret.append ( "\n" );
          }
     }
     return ret;
}


#endif
