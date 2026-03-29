


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   sendmail.h
 * Author: arcieri
 *
 * Created on 14 marzo 2016, 14.26
 */

#ifndef SENDMAIL_H
#define SENDMAIL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


class mtfa_send_mail
{
    /*
     * #define FROM    "<race@nestor.uniroma2.it>"
     * #define TO      "<franco.arcieri@poste.it>"
     * #define CC      "<arcieri@nestor.uniroma2.it>"
     *
     * static const char *payload_text[] = {
     * "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n",
     * "To: " TO "\r\n",
     * "From: " FROM "(Example User)\r\n",
     * "Cc: " CC "(Another example User)\r\n",
     * "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@rfcpedant.example.org>\r\n",
     * "Subject: SMTP SSL example message\r\n",
     * "\r\n", // empty line to divide headers from body, see RFC5322
     * "The body of the message starts here.\r\n",
     * "\r\n",
     * "It could be a lot of lines, could be MIME encoded, whatever.\r\n",
     * "Check RFC5322.\r\n",
     * NULL
     * };
     */
private:
    char *payload_text;
    int bytes_sent;
    int payload_len;

    static size_t payload_source
    (
        void * ptr,
        size_t size,
        size_t nmemb,
        void * userp    //oggetto corrente
    );


public:

    mtfa_send_mail()
    {
        payload_text = nullptr;
        bytes_sent = 0;
    };

    int sendmail
    (
        const char * smtpserver,
        const char * username,
        const char * password,
        const char * from,
        const list<string> params,
//    const char * to,
//    const char * cc,
//    const char * ccn,
        const char * msg,
        const bool check_server_cert,
        const char * personal_ca_path,
        const bool check_server_hostname,
        bool dolog=false
    );

};


#endif /* SENDMAIL_H */

