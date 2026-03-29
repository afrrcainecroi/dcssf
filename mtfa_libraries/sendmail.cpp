
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>
#include <curl/curl.h>

#include <ctime>
#include "mtfa_utils.h"
#include "mtfa_find.h"
#include "sendmail.h"


using namespace std;

static string mtfa_rand_alfanum ( int len, string alphabet, unsigned long long disturber )
{
     mtfa_mat_cypher_ext * rand_mat= new mtfa_mat_cypher_ext ( Ticks::get_micro() + disturber );

     string result;

     unsigned long long ull;
     for ( int i=0; i<len; ++i ) {
          ull = rand_mat->mtfa_mat_rand();
          result += alphabet[ull % alphabet.size()];
     }
     delete rand_mat;
     return result;
}


size_t mtfa_send_mail::payload_source
(
     void * ptr,
     size_t size,
     size_t nmemb,
     void * userp
)
{
     mtfa_send_mail * obj = ( mtfa_send_mail* ) userp;
     //struct upload_status *upload_ctx = (struct upload_status*) userp;

     const char * data;

     if ( ( size == 0 ) || ( nmemb == 0 ) ) {
          return 0;
     }
     int restano = obj->payload_len - obj->bytes_sent;
     int to_be_sent = ( nmemb > restano ) ?restano:nmemb;
     memcpy ( ptr, &obj->payload_text[obj->bytes_sent], to_be_sent );

     obj->bytes_sent += to_be_sent;

     //printf("Space: %zd. Sent: %d\n", nmemb, to_be_sent);
     return to_be_sent;
}

int mtfa_send_mail::sendmail
(
     const char * smtpserver,
     const char * username,
     const char * password,
     const char * from,
     const list<string> params,  //ivi compreso to, cc, ccn eccetera
//    const char * to,
//    const char * cc,
//    const char * ccn,
     const char * msg,
     const bool check_server_cert,
     const char * personal_ca_path,
     const bool check_server_hostname,
    bool dolog
)
{
     CURL *   curl;
     CURLcode res = CURLE_OK;

     struct curl_slist *recipients = NULL;

     // Quante righe sono il messaggio?
     int righe = 1;

     payload_text = ( char* ) msg;
     payload_len = strlen ( msg );

     curl = curl_easy_init();

     if ( curl ) {
          /* Set username and password */
          curl_easy_setopt ( curl, CURLOPT_USERNAME, username ); // "race");
          curl_easy_setopt ( curl, CURLOPT_PASSWORD, password ); // "shetfict123");

          /*
           *  This is the URL for your mailserver. Note the use of smtps:// rather
           * than smtp:// to request a SSL based connection.
           */

          // curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.nestor.uniroma2.it:25");
          curl_easy_setopt ( curl, CURLOPT_URL, smtpserver );    // "smtp://smtp.nestor.uniroma2.it");
          curl_easy_setopt ( curl, CURLOPT_USE_SSL, ( long ) CURLUSESSL_TRY );
          curl_easy_setopt ( curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT );


          /*
           *  If you want to connect to a site who isn't using a certificate that is
           * signed by one of the certs in the CA bundle you have, you can skip the
           * verification of the server's certificate. This makes the connection
           * A LOT LESS SECURE.
           *
           * If you have a CA cert for the server stored someplace else than in the
           * default bundle, then the CURLOPT_CAPATH option might come handy for
           * you.
           */

          if ( personal_ca_path ) {
               curl_easy_setopt ( curl, CURLOPT_CAPATH, personal_ca_path );
          }

          if ( check_server_cert ) {
               curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYPEER, 1L );
          } else {
               curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYPEER, 0L );
          }

          /*
           *  If the site you're connecting to uses a different host name that what
           * they have mentioned in their server certificate's commonName (or
           * subjectAltName) fields, libcurl will refuse to connect. You can skip
           * this check, but this will make the connection less secure.
           */
          if ( check_server_hostname ) {
               curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYHOST, 1L );
          } else {
               curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYHOST, 0L );
          }

          /*
           *  Note that this option isn't strictly required, omitting it will result in
           * libcurl sending the MAIL FROM command with empty sender data. All
           * autoresponses should have an empty reverse-path, and should be directed
           * to the address in the reverse-path which triggered them. Otherwise, they
           * could cause an endless loop. See RFC 5321 Section 4.5.5 for more details.
           */
          curl_easy_setopt ( curl, CURLOPT_MAIL_FROM, from );

          for ( auto it: params ) {
               recipients = curl_slist_append ( recipients, it.c_str() );
          }

          curl_easy_setopt ( curl, CURLOPT_MAIL_RCPT, recipients );

          string s_msg = string ( msg );


          setlocale ( LC_ALL, "C" );
          std::time_t t = std::time ( nullptr );
          std::tm tm = *std::localtime ( &t );

          char * datetime = new char[1024];
          strftime ( datetime, 1023, "Date: %a, %e %b %Y %H:%M:%S %z", &tm );
          s_msg = string ( datetime ) + "\r\n" + s_msg;

          //prende la parte di dominio della from
          string dominio = "@anon.org>";
          mtfa_get_specific_submatching_pattern ( from, "(\\@.*$)", 1, dominio);

          string messageid =
               "Message-ID: <" +
               mtfa_rand_alfanum ( 8, "0123456789ABCDEF", 0 ) +
               "-" +
               mtfa_rand_alfanum ( 4, "0123456789ABCDEF", 999999111111ULL ) +
               "-" +
               mtfa_rand_alfanum ( 4, "0123456789ABCDEF", 888888212121ULL ) +
               "-" +
               mtfa_rand_alfanum ( 4, "0123456789ABCDEF", 888888212121ULL ) +
               "-" +
               mtfa_rand_alfanum ( 12, "0123456789ABCDEF", 777777321123ULL ) +

               dominio;

          s_msg = messageid + "\r\n" + s_msg;

          payload_text = ( char* ) s_msg.c_str();
          payload_len = s_msg.size();

          //printf("MESSAGGIO: <%s>\n", s_msg.c_str());

          /*
          *  We're using a callback function to specify the payload (the headers and
          * body of the message). You could just use the CURLOPT_READDATA option to
          * specify a FILE pointer to read from.
          */
          curl_easy_setopt ( curl, CURLOPT_READFUNCTION, payload_source );
          curl_easy_setopt ( curl, CURLOPT_READDATA, ( void* ) this ); //&upload_ctx);
          curl_easy_setopt ( curl, CURLOPT_UPLOAD, 1L );

          /*
          * Since the traffic will be encrypted, it is very useful to turn on debug
          * information within libcurl to see what is happening during the
          * transfer
          */
          unsigned long uldolog=0;
          if (dolog)
              uldolog=1;
          curl_easy_setopt ( curl, CURLOPT_VERBOSE, uldolog);

          /* Send the message */
          res = curl_easy_perform ( curl );

          /* Check for errors */
          if ( res != CURLE_OK ) {
               // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
          }

          /* Free the list of recipients */
          curl_slist_free_all ( recipients );

          /* Always cleanup */
          curl_easy_cleanup ( curl );
     }

     return ( int ) res;
}
