/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   netfilter_queue.h
 * Author: arcieri
 *
 * Created on 5 marzo 2016, 16.03
 */

#ifndef NETFILTER_QUEUE_H
#define NETFILTER_QUEUE_H

#include <list>
#include <string>

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

#include "mtfa_fs3.h"

using namespace std;

#if 1
class queue_manager;

typedef struct __queue_manager_container__ {
    //le variabili locali della queue
    struct nfq_handle *h;
    struct nfq_q_handle *qh;
    int fd;
    int rv;
    char buf[4096] __attribute__ ( ( aligned ) );
    bool closed;
    ImplicitDs * ds;
    __queue_manager_container__ ()
    {
        h = nullptr;
        qh = nullptr;
        ds = nullptr;
        fd=0;
        rv=0;
        closed=false;
    }
} T_queue_manager_container;

class queue_manager
{
private:
    T_queue_manager_container * running_queues;
    int num_queues;
    int queue_len;
    int bufsize;
    bool stop;
    ImplicitDs * ds;
public:
    bool check_pkt ( struct nfq_data *tb, unsigned int &id, string & to_check );
    //static uint32_t print_pkt(struct nfq_data *tb);
    static int cb ( struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data );
    queue_manager ( list<int> queues, int queue_len, int bufsize );
    ~queue_manager();
    static int do_manage_queue ( unsigned int queue, int id, queue_manager * current_object );
    void stop_queue_manager();
    bool add_rule ( const string &src_ip, const string &dst_ip, const string &dst_port );
    bool del_rule ( const string &src_ip, const string &dst_ip, const string &dst_port );
    string list_rules();
};

#else

class queue_manager;

typedef struct __queue_manager_container__ {
    //le variabili locali della queue
//    struct nfq_handle *h;
//    struct nfq_q_handle *qh;
    int fd;
//    int rv;
//    char buf[4096] __attribute__((aligned));
    bool closed;
    ImplicitDs * ds;
    struct mnl_socket *nl;
    __queue_manager_container__ ()
    {
//        h = nullptr;
//        qh = nullptr;
        ds = nullptr;
        fd=0;
//        rv=0;
        nl = nullptr;
        closed=false;
    }
} T_queue_manager_container;

class queue_manager
{
public:
    T_queue_manager_container * running_queues;
    int num_queues;
    int queue_len;
    int bufsize;
    bool stop;
    ImplicitDs * ds;

    static struct nlmsghdr * nfq_hdr_put ( char *buf, int type, uint32_t queue_num );
    static int queue_cb ( const struct nlmsghdr *nlh, void *data );
    queue_manager ( list<int> _queues, int _queue_len, int _bufsize );
    static void do_manage_queue ( unsigned int queue_num, int id, queue_manager * current_object );
    ~queue_manager();

    void stop_queue_manager();
    bool add_rule ( const string src_ip, const string dst_ip, const string dst_port );
    bool del_rule ( const string src_ip, const string dst_ip, const string dst_port );
    string list_rules();
};
#endif


#endif /* NETFILTER_QUEUE_H */

