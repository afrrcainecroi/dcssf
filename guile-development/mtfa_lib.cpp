#include <stdlib.h>
#include <libguile.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/stat.h>
#include <unordered_map>
#include <vector>
#include <cstring>

#include <string>
#include <list>
#include <functional>
#include <zlib.h>

#include <mtfa_utils.h>
#include <ULL_Rng.h>
#include <base64.h>
#include <runextprog.h>
#include <sendmail.h>
#include <NumberTheory.h>
#include <gmpxx.h>
#include <gmp.h>

#include <c_http_parser.h>
#include <mtfa_qt.h>
#include <mtfa_db.h>
#include <mtfa_utils.h>
#include <mtfa_find.h>
#include <morton.h>
#include <rlpvalue.h>
#include <univalue.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <magic.h>

#include "/volume1/sources/NEW_DEVS/LISP/va/listaeventi.h"

#include "mtfa_fuzzyfind.h"

#include <zlib.h>

#include <VAengine.h>


using namespace std;

//void myfree(void * p)
//{
//  //cout << "Chiamata la free\n";
//  free(p);
//}

std::list<int> li;

static mtfa_fastrng1024* simple_generator = nullptr;
static ULL_Rng* random_generator = nullptr;
static mtfa_mat_cypher_ext* rand_mat = nullptr;
static unsigned char* __rmatrix__ = nullptr;
const int __dim_rmatrix__ = 4194304;
static magic_t magic_cookie = nullptr;
static ImplicitDs * s2_extended = nullptr;


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_lib_type;  //il tipo della libreria generica che utilizziamo

static void finalize_mtfa_lib ( SCM mtfa_lib )
{
	if (simple_generator)
	{
		delete simple_generator;
	}
	simple_generator = nullptr;
	
	
    if ( random_generator != nullptr )
    {
        delete random_generator;
    }

    random_generator = nullptr;


    if ( __rmatrix__ )
    {
        free ( __rmatrix__ );
    }

    __rmatrix__ = nullptr;

    if ( rand_mat != nullptr )
    {
        delete rand_mat;
    }

    rand_mat = nullptr;
	
	if (magic_cookie)
		magic_close(magic_cookie);
	
	magic_cookie = nullptr;
	
	if (s2_extended)
		delete s2_extended;
	s2_extended = nullptr;
}


static SCM mtfa_lib_p ( SCM mtfa_lib )
{
//     //il body \u00e8 la funzione che intendo svolgere
//     auto const body ( cify<SCM ( * ) ( void* ) > ( [&] ( void* data )
//     {
//         scm_assert_foreign_object_type ( mtfa_lib_type, mtfa_lib );
//         return SCM_BOOL_T;
//     } ) );
//     //l'handler \u00e8 il codice che viene chiamato in caso di errore
//     auto const handler ( cify<SCM ( * ) ( void*, SCM, SCM ) > ( [&] ( void* data, SCM key, SCM args )
//     {
//         return SCM_BOOL_F;
//     } ) );

    auto const body = [] (void * data) {
        scm_assert_foreign_object_type ( mtfa_lib_type, *(SCM*)data);
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void *data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_lib, handler, nullptr );

    return ret;
}

static SCM mtfa_rand ()
{
    if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(GetULLTime());
/*
    if ( random_generator == nullptr )
    {
        random_generator = new ULL_Rng ( Ticks::get_micro() );
    }*/

    return scm_from_ulong_long ( simple_generator->next() );
}


static SCM mtfa_strong_random (SCM bits)
{
	if (scm_is_integer(bits)==0)
		scm_wrong_type_arg_msg("mtfa-linux-random", 0, bits, "integer");
	mpz_class r;
	Random::StrongRandom(scm_to_int(bits), r);
	return scm_from_mpz(r.get_mpz_t());
}

static SCM mtfa_rand_ui ()
{
    if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(GetULLTime());
//     if ( random_generator == nullptr )
//     {
//         random_generator = new ULL_Rng ( Ticks::get_micro() );
//     }

    unsigned int v = simple_generator->next(); // % 18446744073709551616;
    return scm_from_uint ( v );
}

static SCM mtfa_rand_d ()
{
    if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(GetULLTime());
//     if ( random_generator == nullptr )
//     {
//         random_generator = new ULL_Rng ( Ticks::get_micro() );
//     }

    double v = simple_generator->next(); // % 18446744073709551616;
    v /= 18446744073709551616.0;

    return scm_from_double ( v );
}

static SCM mtfa_rand_seed ( SCM intero )
{
    unsigned long long num = scm_to_ulong_long ( intero );

    if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(num);
    else{
        delete simple_generator;
        simple_generator = new mtfa_fastrng1024(num);
    }
//     if ( random_generator == nullptr )
//     {
//         random_generator = new ULL_Rng ( num );
//     }
//     else
//     {
//         delete random_generator;
//         random_generator = new ULL_Rng ( num );
//     }

    return SCM_BOOL_T;
}

//Da cifrare \u00e8 un bytevector u8
static SCM mtfa_cypher ( SCM magic, SCM disturber, SCM da_cifrare )
{
    unsigned long long ull_magic = scm_to_ulong_long ( magic );
    unsigned long long ull_disturber = scm_to_ulong_long ( disturber );

    //Ammetto che da_cifrare possa essere una stringa oppure un bytevector
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_cifrare ) )
    {
        p = ScmToStringn ( da_cifrare, &len );
        mtfa_mat_cypher ( ull_magic, ull_disturber, ( unsigned char* ) p, len );
        SCM scm_ret = scm_pointer_to_bytevector (
                          scm_from_pointer ( ( void* ) p, free ),
                          scm_from_int ( len ),
                          scm_from_int ( 0 ),
                          scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
    else
    {
        if ( scm_is_bytevector ( da_cifrare ) )
        {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            len = SCM_BYTEVECTOR_LENGTH ( da_cifrare );
            p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( da_cifrare );
            mtfa_mat_cypher ( ull_magic, ull_disturber, ( unsigned char* ) p, len );
            return da_cifrare;
        }
        else
        {
            return SCM_EOL;
        }
    }
}

static SCM local_mtfa_rand_string ( SCM scm_len )
{
    if ( rand_mat == nullptr )
    {
        rand_mat = new mtfa_mat_cypher_ext ( Ticks::get_micro() );
    }

    int len = scm_to_int ( scm_len );

    string sret = "";

    while ( sret.size() < len )
    {
        string stmp = to_string ( rand_mat->mtfa_mat_rand() );
        sret += stmp;
    }

    SCM scmret =  ScmFromString ( sret.substr ( 0, len ).c_str() );
    return scmret;
}

static SCM local_mtfa_rand_alfanum ( SCM scm_len, SCM alphabet )
{
    if ( rand_mat == nullptr )
    {
        rand_mat = new mtfa_mat_cypher_ext ( Ticks::get_micro() );
        rand_mat->mtfa_mat_rand();
    }

    int len = scm_to_int ( scm_len );
	
	if (scm_is_string(alphabet))
	{
		size_t dim;
		char* alp = scm_to_utf8_stringn ( alphabet, &dim);

		char* result = ( char* ) malloc ( len + 1 );
		result[len] = 0;

		unsigned long long ull;

		for ( int i = 0; i < len; i += 8 )
		{
			ull = rand_mat->mtfa_mat_rand();
			int lim = len - i;

			for ( int j = 0; ( j < 8 && j < lim ); ++j )
			{
				result[i + j] = alp[ ( ( unsigned char* ) &ull ) [j] % dim];
			}
		}

		SCM scmret =  scm_from_utf8_stringn( result, len );
		free ( result );
		free ( alp );
		return scmret;
	} else if (scm_is_bytevector(alphabet)) {
		
		int alpha_len = SCM_BYTEVECTOR_LENGTH ( alphabet );
		unsigned char * alpha_data = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( alphabet );
		
		char* result = ( char* ) malloc ( len );
		
		unsigned long long ull;
		
		for ( int i = 0; i < len; i += 8 )
		{
			ull = rand_mat->mtfa_mat_rand();
			int lim = len - i;
			
			for ( int j = 0; ( j < 8 && j < lim ); ++j )
			{
				result[i + j] = alpha_data[ ( ( unsigned char* ) &ull ) [j] % alpha_len];
			}
		}

		return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) result, free ),
										   scm_from_int ( len ),
										   scm_from_int ( 0 ),
										   scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
	}
	return SCM_BOOL_F;
}

static SCM mtfa_rand_mat ()
{
    if ( rand_mat == nullptr )
    {
        rand_mat = new mtfa_mat_cypher_ext ( Ticks::get_micro() );
    }

    long long ull = rand_mat->mtfa_mat_rand();
    return scm_from_ulong_long ( ull );
}

static SCM mtfa_rand_mat_d ()
{
    if ( rand_mat == nullptr )
    {
        rand_mat = new mtfa_mat_cypher_ext ( Ticks::get_micro() );
    }

    double v = rand_mat->mtfa_mat_rand();
    v /= 18446744073709551616.0;
    return scm_from_double ( v );
}

static SCM mtfa_rand_mat_ui ()
{
    if ( rand_mat == nullptr )
    {
        rand_mat = new mtfa_mat_cypher_ext ( Ticks::get_micro() );
    }

    unsigned int ull = rand_mat->mtfa_mat_rand();
    return scm_from_uint ( ull );
}

static SCM mtfa_rand_mat_seed ( SCM scm_ull )
{
    if ( rand_mat != nullptr )
    {
        delete rand_mat;
    }

    rand_mat = new mtfa_mat_cypher_ext ( scm_to_ulong_long ( scm_ull ) );

    return SCM_BOOL_T;
}


static SCM mtfa_http_parse ( SCM an_scm_string )
{
    //Ammetto che l'http possa essere una stringa oppure un bytevector
    size_t http_len = 0;
    char* http_data = nullptr;
    bool to_be_freed = false;

    if ( scm_is_string ( an_scm_string ) )
    {
        to_be_freed = true;
        http_data = ScmToStringn ( an_scm_string, &http_len );
    }
    else
    {
        if ( scm_is_bytevector ( an_scm_string ) )
        {
            http_len = SCM_BYTEVECTOR_LENGTH ( an_scm_string );
            http_data = ( char* ) SCM_BYTEVECTOR_CONTENTS ( an_scm_string );
        }
        else
        {
            return SCM_EOL;
        }
    }

    c_http_parser* cp = new c_http_parser();
    int iret = cp->Parse ( http_data, http_len );

    if ( iret < 0 )
    {
        delete cp;
        return SCM_EOL;
    }

    //torno una lista fatta nel seguente modo: ( method/nil surl/lisp->pnil headers/nil body/nil )
    SCM tcomm = SCM_EOL;
    SCM tipo = SCM_EOL; //request/answer
    SCM method = SCM_EOL;
    SCM surl = SCM_EOL;
    SCM headers = SCM_EOL;
    SCM body = SCM_EOL;
    SCM complete = SCM_EOL;
    SCM ans = SCM_EOL;

    if ( cp->headers_complete )
    {
        //Se gli headers sono completi li prendo!
        if ( cp->parser->flags & F_CHUNKED )
        {
            tcomm = scm_cons ( ScmFromString ( "CHUNKED" ), tcomm );
        }

        if ( cp->parser->flags & F_CONNECTION_KEEP_ALIVE )
        {
            tcomm = scm_cons ( ScmFromString ( "CONNECTION_KEEP_ALIVE" ), tcomm );
        }

        if ( cp->parser->flags & F_CONNECTION_CLOSE )
        {
            tcomm = scm_cons ( ScmFromString ( "CONNECTION_CLOSE" ), tcomm );
        }

        if ( cp->parser->flags & F_TRAILING )
        {
            tcomm = scm_cons ( ScmFromString ( "TRAILING" ), tcomm );
        }

        if ( cp->parser->flags & F_UPGRADE )
        {
            tcomm = scm_cons ( ScmFromString ( "UPGRADE" ), tcomm );
        }

        if ( cp->parser->flags & F_SKIPBODY )
        {
            tcomm = scm_cons ( ScmFromString ( "SKIPBODY" ), tcomm );
        }

        if ( cp->parser->type == HTTP_REQUEST )
        {
            tipo = ScmFromString ( "REQUEST" );
        }
        else if ( cp->parser->type == HTTP_RESPONSE )
        {
            tipo = ScmFromString ( "ANSWER" );
        }
        else
        {
            tipo = ScmFromString ( "BOTH" );
        }

        const char* p = http_method_str ( ( enum http_method ) cp->parser->method );
        method = ScmFromString ( p );

        //Parto con surl e poi proseguo con headers
        for ( int i = 0; i < UF_MAX; ++i )
        {
            if ( cp->parsed_url.field_set & ( 1 << i ) )
            {
                surl = scm_cons ( ScmFromStringn ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len ), surl );
            }
            else
            {
                surl = scm_cons ( SCM_EOL, surl );
            }
        }

        for ( auto it = cp->headers.begin(); it != cp->headers.end(); ++it )
        {
            SCM coppia = SCM_EOL;
            coppia = scm_cons ( ScmFromStringn ( std::get<2> ( *it ), std::get<3> ( *it ) ), coppia );
            coppia = scm_cons ( ScmFromStringn ( std::get<0> ( *it ), std::get<1> ( *it ) ), coppia );
            headers = scm_cons ( coppia, headers );
        }
    }

    if ( cp->message_complete )
    {
        if ( cp->body_len > 0 )
        {
            body = ScmFromStringn ( cp->body_from, cp->body_len );
        }

        complete = SCM_BOOL_T;
    }
    else
    {
        complete = SCM_EOL;
    }

    if ( to_be_freed )
    {
        free ( http_data );
    }

    ans = scm_cons ( scm_reverse ( tcomm ), ans );
    ans = scm_cons ( tipo, ans );
    ans = scm_cons ( method, ans );
    ans = scm_cons ( scm_reverse ( surl ), ans );
    ans = scm_cons ( scm_reverse ( headers ), ans );
    ans = scm_cons ( body, ans );
    ans = scm_cons ( complete, ans );
    delete cp;

    return ( scm_reverse ( ans ) );

    //devo farla da C a GUILE!
    //Fast responder che chiama guile

    //std::list<std::tuple<char *, int, char *, int>> headers;
    //struct http_parser_url parsed_url;
    //attr = punt->lisp->jn(Sexp::vsexpGet(it->value(), it->value_size()), attr);

    // http-parse &GET http://user:pwd@www.alfa.com:1234//a/b/c/d?b=10\&c=20#aaa HTTP/1.1\r\nHost: www.google.it\r\nConnection: KeepAlive\r\nContent-Length: 10\r\n\r\n1111111111&

    //http-parse &HTTP/1.1 200 OK\r\nConnection: close\r\nHost: www.sian.it\r\nUser-Agent: gSOAP/2.7\r\nContent-Type: text/xml; charset=utf-8; action="http://SecAgentServer.RCSSServer.org/WhatToDo01"\r\nContent-Length: 10\r\nConnection: Close\r\nSOAPAction: "http://SecAgentServer.RCSSServer.org/WhatToDo01"\r\nCache-Control: no-cache\r\nCookie: CTV-OhR1Kg..-4Vr8=BB01_IDTetQvdmMt/F+86i3_DTc3y5UQ..-mdO5UQ..-c3y5UQ.._DMLnNpYW4uaXQ.\r\n\r\n1234567890&

    // http-parse &GET / HTTP/1.1\\r\\nHost: www.google.it\\r\\nContent-Length: 10\\r\\n\\r\\n1234567890&
    //http-parse &GET http://franco:arcieri@www.uno.it:123/aaa?b=10#ciccia HTTP/1.1\\r\\nHost: www.sian.it\\r\\nUser-Agent: gSOAP/2.7\\r\\nContent-Type: text/xml; charset=utf-8; action=\\"http://SecAgentServer.RCSSServer.org/WhatToDo01\\"\\r\\nContent-Length: 10\\r\\nConnection: Close\\r\\nSOAPAction: \\"http://SecAgentServer.RCSSServer.org/WhatToDo01\\"\\r\\nCache-Control: no-cache\\r\\nCookie: CTV-OhR1Kg..-4Vr8=BB01_IDTetQvdmMt/F+86i3_DTc3y5UQ..-mdO5UQ..-c3y5UQ.._DMLnNpYW4uaXQ.\\r\\n\\r\\n1234567890&
    //http-parse &HTTP 200 OK\\r\\nConnection: close\\r\\nHost: www.sian.it\\r\\nUser-Agent: gSOAP/2.7\\r\\nContent-Type: text/xml; charset=utf-8; action=\\"http://SecAgentServer.RCSSServer.org/WhatToDo01\\"\\r\\nContent-Length: 10\\r\\nConnection: Close\\r\\nSOAPAction: \\"http://SecAgentServer.RCSSServer.org/WhatToDo01\\"\\r\\nCache-Control: no-cache\\r\\nCookie: CTV-OhR1Kg..-4Vr8=BB01_IDTetQvdmMt/F+86i3_DTc3y5UQ..-mdO5UQ..-c3y5UQ.._DMLnNpYW4uaXQ.\\r\\n\\r\\n1234567890&
}

static const char * const codes_base64 =    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char * const codes_base64url = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const unsigned char map_base64[256] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 253, 255,
255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255 };

static const unsigned char map_base64url[] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 253, 255,
255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255,
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255
};

static int _base64_encode_internal(const unsigned char *in,  unsigned long inlen,
                                 char *out, unsigned long *outlen,
                                 const char *codes, int pad)
{
   unsigned long i, len2, leven;
   char *p;

   /* valid output size ? */
   len2 = 4 * ((inlen + 2) / 3);
   if (*outlen < len2 + 1) {
      *outlen = len2 + 1;
      return -1;
   }
   p = out;
   leven = 3*(inlen / 3);
   for (i = 0; i < leven; i += 3) {
       *p++ = codes[(in[0] >> 2) & 0x3F];
       *p++ = codes[(((in[0] & 3) << 4) + (in[1] >> 4)) & 0x3F];
       *p++ = codes[(((in[1] & 0xf) << 2) + (in[2] >> 6)) & 0x3F];
       *p++ = codes[in[2] & 0x3F];
       in += 3;
   }
   /* Pad it if necessary...  */
   if (i < inlen) {
       unsigned a = in[0];
       unsigned b = (i+1 < inlen) ? in[1] : 0;

       *p++ = codes[(a >> 2) & 0x3F];
       *p++ = codes[(((a & 3) << 4) + (b >> 4)) & 0x3F];
       if (pad) {
         *p++ = (i+1 < inlen) ? codes[(((b & 0xf) << 2)) & 0x3F] : '=';
         *p++ = '=';
       }
       else {
         if (i+1 < inlen) *p++ = codes[(((b & 0xf) << 2)) & 0x3F];
       }
   }

   /* append a NULL byte */
   *p = '\0';

   /* return ok */
   *outlen = (unsigned long)(p - out); /* the length without terminating NUL */
   return 0;
}

static int base64_encode(const unsigned char *in,  unsigned long inlen,
                                 char *out, unsigned long *outlen)
{
    return _base64_encode_internal(in, inlen, out, outlen, codes_base64, 1);
}

static int base64url_encode(const unsigned char *in,  unsigned long inlen,
                                    char *out, unsigned long *outlen)
{
    return _base64_encode_internal(in, inlen, out, outlen, codes_base64url, 0);
}

static int base64url_strict_encode(const unsigned char *in,  unsigned long inlen,
                                           char *out, unsigned long *outlen)
{
    return _base64_encode_internal(in, inlen, out, outlen, codes_base64url, 1);
}

enum {
   insane = 0,
   strict = 1,
   relaxed = 2
};

static int _base64_decode_internal(const char *in,  unsigned long inlen,
                                 unsigned char *out, unsigned long *outlen,
                           const unsigned char *map, int mode)
{
   unsigned long t, x, y, z;
   unsigned char c;
   int           g;

   g = 0; /* '=' counter */
   for (x = y = z = t = 0; x < inlen; x++) {
       if ((in[x] == 0) && (x == (inlen - 1)) && (mode != strict)) {
          continue; /* allow the last byte to be NUL (relaxed+insane) */
       }
       c = map[(unsigned char)in[x]&0xFF];
       if (c == 254) {
          g++;
          continue;
       }
       if (c == 253) {
          if (mode == strict) {
             return -1;
          }
          continue; /* allow to ignore white-spaces (relaxed+insane) */
       }
       if (c == 255) {
          if (mode == insane) {
             continue; /* allow to ignore invalid garbage (insane) */
          }
          return -1;
       }
       if ((g > 0) && (mode != insane)) {
          /* we only allow '=' to be at the end (strict+relaxed) */
          return -1;
       }

       t = (t<<6)|c;

       if (++y == 4) {
          if (z + 3 > *outlen) return -1;
          out[z++] = (unsigned char)((t>>16)&255);
          out[z++] = (unsigned char)((t>>8)&255);
          out[z++] = (unsigned char)(t&255);
          y = t = 0;
       }
   }

   if (y != 0) {
      if (y == 1) return -1;
      if (((y + g) != 4) && (mode == strict) && (map != map_base64url)) return -1;
      t = t << (6 * (4 - y));
      if (z + y - 1 > *outlen) return -1;
      if (y >= 2) out[z++] = (unsigned char) ((t >> 16) & 255);
      if (y == 3) out[z++] = (unsigned char) ((t >> 8) & 255);
   }
   *outlen = z;
   return 0;
}
static int base64_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64, insane);
}
static int base64_strict_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
   return _base64_decode_internal(in, inlen, out, outlen, map_base64, strict);
}
static int base64_sane_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
   return _base64_decode_internal(in, inlen, out, outlen, map_base64, relaxed);
}

static int base64url_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, insane);
}

static int base64url_strict_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, strict);
}

static int base64url_sane_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, relaxed);
}

// static unsigned int base64_get_decoded_length( unsigned int size )
// {
//      unsigned int b64_in_size  = 4;
//      unsigned int b64_out_size = 3;
// 
//      return ( ( ( size + b64_in_size - 1 ) / b64_in_size ) * b64_out_size );
// }
// 
// unsigned int base64_get_encoded_length( unsigned int size )
// {
//      unsigned int b64_in_size  = 3;
//      unsigned int b64_out_size = 4;
// 
//      return ( ( ( size + b64_in_size - 1 ) / b64_in_size ) * b64_out_size );
// }


static SCM mtfa_b64_encode ( SCM da_cifrare )
{
    //Ammetto che da_cifrare possa essere una stringa oppure un bytevector
    char* p;
    int len;

    if ( scm_is_string ( da_cifrare ) )
    {
        p = ScmToString ( da_cifrare );
        len = strlen(p);

        if (p)
        {
            unsigned long encodedLenght = Base64::GetEncodedLength(len)+1; //per lo 0 terminale
            char * encoded = new char[encodedLenght];
            int iRet = base64_encode((const unsigned char*)p, len, encoded, &encodedLenght);
            if (iRet != 0)
            {
                delete [] encoded;
                free(p);
                return SCM_BOOL_F;
            }
            SCM scm_ret = ScmFromString ( encoded );
            delete [] encoded;
            free ( p );
            return scm_ret;
        }
        else
        {
            return SCM_BOOL_F;
        }
    }
    else
    {
        if ( scm_is_bytevector ( da_cifrare ) )
        {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            len = SCM_BYTEVECTOR_LENGTH ( da_cifrare );
            p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( da_cifrare );

            unsigned long encodedLenght = Base64::GetEncodedLength(len)+1;
            char * encoded = new char[encodedLenght];
            int iRet = base64_encode((unsigned char*)p, len, encoded, &encodedLenght);
            if (iRet != 0)
            {
                delete [] encoded;
                return SCM_BOOL_F;
            }
            SCM scm_ret = ScmFromString ( encoded );
            delete [] encoded;
            return scm_ret;
        }
        else
        {
            return SCM_EOL;
        }
    }
}

static SCM mtfa_b64_url_encode ( SCM da_cifrare )
{
    //Ammetto che da_cifrare possa essere una stringa oppure un bytevector
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_cifrare ) )
    {
        p = ScmToStringn ( da_cifrare, &len );

        if (p)
        {
            unsigned long encodedLenght = Base64::GetEncodedLength(len)+1;
            char * encoded = new char[encodedLenght];
            int iRet = base64url_encode((unsigned char*)p, len, encoded, &encodedLenght);
            if (iRet != 0)
            {
                delete [] encoded;
                free(p);
                return SCM_BOOL_F;
            }
            SCM scm_ret = ScmFromString ( encoded );
            delete [] encoded;
            free ( p );
            return scm_ret;
        }
        else
        {
            return SCM_BOOL_F;
        }
    }
    else
    {
        if ( scm_is_bytevector ( da_cifrare ) )
        {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            len = SCM_BYTEVECTOR_LENGTH ( da_cifrare );
            p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( da_cifrare );

            unsigned long encodedLenght = Base64::GetEncodedLength(len)+1;
            char * encoded = new char[encodedLenght];
            int iRet = base64url_encode((unsigned char*)p, len, encoded, &encodedLenght);
            if (iRet != 0)
            {
                delete [] encoded;
                return SCM_BOOL_F;
            }
            SCM scm_ret = ScmFromString ( encoded );
            delete [] encoded;
            return scm_ret;
        }
        else
        {
            return SCM_EOL;
        }
    }
}

//Torna una stringa
static SCM mtfa_b64_decode_s ( SCM da_decifrare )
{
    //Da_decifrare può essere solo una stringa
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_decifrare ) )
    {
        p = ScmToStringn ( da_decifrare, &len );
        unsigned long decodedLenght = len+4; //tanto è senz'altro meno!!
        
        char * decoded = new char[decodedLenght];
        int iRet = base64_decode((const char*)p, len, (unsigned char*)decoded, &decodedLenght);
        if (iRet != 0)
        {
            delete [] decoded;
            free(p);
            return SCM_BOOL_F;
        }
        SCM scm_ret = ScmFromStringn( decoded, decodedLenght );
        delete [] decoded;
        free(p);
        return scm_ret;
    }
    else
    {
        return SCM_BOOL_F;
    }
}

//torna un bytevector
static SCM mtfa_b64_decode_bv ( SCM da_decifrare )
{
    //Da_decifrare può essere solo una stringa
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_decifrare ) )
    {
        p = ScmToStringn ( da_decifrare, &len );
        unsigned long decodedLenght = len+4;

        unsigned char * decoded = (unsigned char *) malloc(decodedLenght);
        int iRet = base64_decode((const char*)p, len, decoded, &decodedLenght);
        if (iRet != 0)
        {
            free(decoded);
            free(p);
            return SCM_BOOL_F;
        }
        
        free(p);
        SCM scmRet = scm_pointer_to_bytevector (
                       scm_from_pointer ( ( void* ) decoded, free ),
                       scm_from_int ( decodedLenght ),
                       scm_from_int ( 0 ),
                       scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scmRet;
    }
    else
    {
        return SCM_BOOL_F;
    }
}

//Torna una stringa
static SCM mtfa_b64_url_decode_s ( SCM da_decifrare )
{
    //Da_decifrare può essere solo una stringa
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_decifrare ) )
    {
        p = ScmToStringn ( da_decifrare, &len );
        unsigned long decodedLenght = len+4;
        
        char * decoded = new char[decodedLenght];
        int iRet = base64url_decode((const char*)p, len, (unsigned char*)decoded, &decodedLenght);
        if (iRet != 0)
        {
            delete [] decoded;
            free(p);
            return SCM_BOOL_F;
        }
        SCM scm_ret = ScmFromStringn( decoded, decodedLenght );
        delete [] decoded;
        free(p);
        return scm_ret;
    }
    else
    {
        return SCM_BOOL_F;
    }
}

//torna un bytevector
static SCM mtfa_b64_url_decode_bv ( SCM da_decifrare )
{
    //Da_decifrare può essere solo una stringa
    size_t len = 0;
    char* p;

    if ( scm_is_string ( da_decifrare ) )
    {
        p = ScmToStringn ( da_decifrare, &len );
        unsigned long decodedLenght = len+4;

        unsigned char * decoded = (unsigned char *) malloc(decodedLenght);
        int iRet = base64url_decode((const char*)p, len, decoded, &decodedLenght);
        if (iRet != 0)
        {
            free(decoded);
            free(p);
            return SCM_BOOL_F;
        }
        
        free(p);
        SCM scmRet = scm_pointer_to_bytevector (
                       scm_from_pointer ( ( void* ) decoded, free ),
                       scm_from_int ( decodedLenght ),
                       scm_from_int ( 0 ),
                       scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scmRet;
    }
    else
    {
        return SCM_BOOL_F;
    }
}

#define USE_PSTREAM 1

#ifdef USE_PSTREAM
#include "pstream.h"
template class redi::basic_pstreambuf<char>;
template class redi::pstream_common<char>;
template class redi::basic_pstream<char>;
template class redi::basic_ipstream<char>;
template class redi::basic_opstream<char>;
template class redi::basic_rpstream<char>;
#endif

static SCM mtfa_run_ext_prog ( SCM prog, SCM pars, SCM cmd )
{
    //due stringhe
    size_t len = 0;
    char* p;

    if ( !scm_is_string ( prog )  || ! ( scm_is_null_or_nil ( pars ) || scm_is_string ( pars ) ) )
    {
        return SCM_EOL;
    }

    string sprog, spars;
    vector<string> vscmd;
    p = ScmToStringn ( prog, &len );
    sprog.assign ( p, len );
    free ( p );

    p = ScmToStringn ( pars, &len );
    spars.assign ( p, len );
    free ( p );

    if ( scm_is_string ( cmd ) )
    {
        p = ScmToStringn ( cmd, &len );
        vscmd.push_back(string( p, len ));
        free ( p );
    }
    else if (scm_is_true(scm_list_p(cmd)))
    {
        while (!scm_is_null_or_nil(cmd))
        {
            p = nullptr;
            p = ScmToStringn ( scm_car(cmd), &len );
            if (len > 0)
                vscmd.push_back(string( p, len ));
            if (p)
                free ( p );
            cmd = scm_cdr(cmd);
        }
    }
    else //if (scm_is_null_or_nil(cmd))
    {
        vscmd.clear();
    }

#ifndef USE_PSTREAM
    T_popen_plus_process* process = popen_plus ( ( sprog + " " + spars ).c_str() );

    if ( !process )
    {
        /* Failed do something and return or exit */
        return SCM_EOL;
    }

    if ( scmd.size() > 0 )
    {
        fprintf ( process->write_fp, "%s", scmd.c_str() );
        fflush ( process->write_fp );
    }

    //Ora leggo il risultato
    string sres;
    int ch;

    while ( ( ch = getc ( process->read_fp ) ) != EOF )
    {
        sres += ch;
    }

    while ( ( ch = getc ( process->read_err_fp ) ) != EOF )
    {
        sres += ch;
    }

    popen_plus_close ( process );
    return ScmFromString ( sres.c_str() );
#else
    vector<std::string> sout, serr;
    std::string buf;
    const redi::pstreams::pmode all3streams = redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr;

    redi::pstream ps ( sprog + " " + spars, all3streams );
//     while (getline(ps.out(), buf)) sout += buf; ps.clear();
//     while (getline(ps.err(), buf)) serr += buf; ps.clear();

    sout.clear(); serr.clear();

    for ( int i=0; i < vscmd.size(); ++i )
    {
        ps << vscmd[i];
        if (i < (vscmd.size()-1))
            ps << "\r\n";
        else
            ps << redi::peof;
        //tra una riga e l'altra aspetto 1/10 secondo
        usleep(100000);
    }
    
    //Chiudo input prima di procedere
    if (vscmd.size()==0)
        ps << redi::peof;

    //Leggo il risultato
    while ( getline ( ps.out(), buf ) )
        sout.push_back(buf);

    ps.clear();

    while ( getline ( ps.err(), buf ) )
        serr.push_back(buf);

    ps.clear();

    SCM lista_out = SCM_EOL;
    SCM lista_err = SCM_EOL;
    SCM lista = SCM_EOL;

    for (auto it= sout.rbegin(); it != sout.rend(); ++it)
    {
        SCM str = ScmFromString ( (*it).c_str() );
        lista_out = scm_cons ( str, lista_out );
    }
    for (auto it= serr.rbegin(); it != serr.rend(); ++it)
    {
        SCM str = ScmFromString ( (*it).c_str() );
        lista_err = scm_cons ( str, lista_err );
    }
    lista = scm_cons ( lista_err, lista );
    lista = scm_cons ( lista_out, lista );
    return lista;
#endif
}

static SCM mtfa_http_get_page (
    SCM host,
    SCM port,
    SCM protocol,
    SCM method,
    SCM url,
    SCM headers,
    SCM data,
    SCM proxy_name,
    SCM proxy_port,
    SCM timeout
)
{
    char* s_host;
    char* s_port;
    char* s_protocol;
    char* s_method;
    char* s_url;
    char* s_headers;
    char* s_data;
    char* s_proxy_name;
    char* s_proxy_port;
    int i_timeout;

    s_host = ScmToString ( host );
    s_port = ScmToString ( port );
    s_protocol = ScmToString ( protocol );
    s_method = ScmToString ( method );
    s_url = ScmToString ( url );
    s_headers = ScmToString ( headers );
    s_data = ScmToString ( data );
    s_proxy_name = ScmToString ( proxy_name );
    s_proxy_port = ScmToString ( proxy_port );
    i_timeout = scm_to_int ( timeout );

    string pret;
    string hret;
    GetPageFromUrlProxy (
        s_host,
        s_port,
        s_protocol,
        s_method,
        s_url,
        s_headers,
        s_data,
        s_proxy_name,
        s_proxy_port,
        i_timeout,
        pret, hret );

    SCM scm_ret = SCM_EOL;
    SCM scm_page = SCM_EOL;
    SCM scm_headers = SCM_EOL;

    if ( pret.size() > 0 )
    {
        //std::cout << "Preso: " << pret << std::endl;
        char* p = ( char* ) malloc ( pret.size() );
        memcpy ( p, pret.data(), pret.size() );
        scm_page = scm_pointer_to_bytevector (
                       scm_from_pointer ( ( void* ) p, free ),
                       scm_from_int ( pret.size() ),
                       scm_from_int ( 0 ),
                       scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    }

    if ( hret.size() > 0 )
    {
        scm_headers = ScmFromString ( hret.c_str() );
    }

    scm_ret = scm_cons ( scm_page, scm_ret );
    scm_ret = scm_cons ( scm_headers, scm_ret );

    free ( s_host );
    free ( s_port );
    free ( s_protocol );
    free ( s_method );
    free ( s_url );
    free ( s_headers );
    free ( s_data );
    free ( s_proxy_name );
    free ( s_proxy_port );

    return scm_ret;
}


static SCM ex_mtfa_send_mail
(
    SCM scm_smtpserver,
    SCM scm_username,
    SCM scm_password,
    SCM scm_from,
    SCM scm_params,
    SCM scm_msg,
    SCM scm_personal_ca_path,
    SCM scm_check_server_cert_and_or_hostname //0 no, 1 solo server 2 solo hostname, 3 server e hostname
)
{
    char* smtpserver;
    char* username;
    char* password;
    char* from;
    list<string> params;
    char* msg;
    bool check_server_cert;
    char* personal_ca_path;
    bool check_server_hostname;

    smtpserver = ScmToString ( scm_smtpserver );
    username = ScmToString ( scm_username );
    password = ScmToString ( scm_password );

    while ( scm_params != SCM_EOL )
    {
        char* param = ScmToString ( SCM_CAR ( scm_params ) );
        params.push_back ( string ( param ) );
        free ( param );
        scm_params = SCM_CDR ( scm_params );
    }

    from = ScmToString ( scm_from );

    msg = ScmToString ( scm_msg );

    if ( scm_is_null_or_nil ( scm_personal_ca_path ) )
    {
        personal_ca_path = nullptr;
    }
    else
    {
        personal_ca_path = ScmToString ( scm_personal_ca_path );

        if ( strlen ( personal_ca_path ) == 0 )
        {
            free ( personal_ca_path );
            personal_ca_path = nullptr;
        }
    }

    unsigned int check_server_cert_and_or_hostname = scm_to_uint ( scm_check_server_cert_and_or_hostname ); //0 no, 1 solo server 2 solo hostname, 3 server e hostname

    if ( check_server_cert_and_or_hostname == 0 )
    {
        check_server_cert = false;
        check_server_hostname = false;
    }
    else if ( check_server_cert_and_or_hostname == 1 )
    {
        check_server_cert = true;
        check_server_hostname = false;
    }
    else if ( check_server_cert_and_or_hostname == 2 )
    {
        check_server_cert = false;
        check_server_hostname = true;
    }
    else
    {
        check_server_cert = true;
        check_server_hostname = true;
    }

    mtfa_send_mail* sm = new mtfa_send_mail();

    bool dolog = false;
    int iret = sm->sendmail (
                   smtpserver,
                   username,
                   password,
                   from,
                   params,
                   msg,
                   check_server_cert,
                   personal_ca_path,
                   check_server_hostname,
                   dolog
               );

    delete sm;

    free ( smtpserver );
    free ( username );
    free ( password );
    free ( from );
    free ( msg );

    if ( personal_ca_path )
    {
        free ( personal_ca_path );
    }

    return scm_from_int ( iret );
}

static void loadmatrix()
{
    extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];

    if ( __rmatrix__ )
    {
        return;
    }

    __rmatrix__ = ( unsigned char* ) malloc ( 65536 * 8 * 8 );
    int pos = 0;

    for ( int i = 0; i < 8; ++i )
    {
        string b64 = Base64::encode ( 65536 * 8, ( unsigned char* ) &mtfa_mat_cypher_base_ext[i] );
        string bin = Base64::decode ( b64 );
        memcpy ( &__rmatrix__[pos], bin.data(), bin.size() );
        pos += bin.size();
    }
}

static mpz_class UnsignedPChar2Mpz ( unsigned char* p, unsigned  int len )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), len, 1, 1, 1, 0, p );
    return value;
}

static void Mpz2UnsignedPChar ( mpz_class * value, unsigned char*& p, int& len )
{
    if ( value == 0 )
    {
        p = ( unsigned char* ) malloc ( 1 );
        *p = 0;
        len = 1;
    }
    else
    {
        int size = 1;
        int numb = 8 * size - 0;
        int count = ( mpz_sizeinbase ( value->get_mpz_t(), 2 ) + numb - 1 ) / numb;
        count += 16;  //per ogni evenienza!
        p = ( unsigned char* ) malloc ( count * size );

        int countp = 0;

        mpz_export ( p, ( size_t* ) &countp, 1, size, 1, 0, value->get_mpz_t() );
        len = countp;
    }
}


/*
static SCM mtfa_strong_random ( SCM bits )
{
    mpz_class mret;
    Random::StrongRandom ( scm_to_int ( bits ), mret );

    return scm_from_mpz ( mret.get_mpz_t() );
}
*/

static void blum_finalizer ( void* vblum)
{
	if ( vblum)
		delete ( BlumInteger* ) vblum;
	
	vblum = nullptr;
}
static SCM mtfa_blum_make(SCM bits)
{
	int nbit = scm_to_int ( bits );
	mpz_class mret1;
    
	BlumInteger * bi;
    if (nbit == 0)
        bi = new BlumInteger();
    else
        bi = new BlumInteger(nbit);
	return scm_from_pointer ( ( void* ) bi, &blum_finalizer);
}
static SCM mtfa_blum_get_values (SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class m = blum->getM();
	mpz_class p = blum->getP();
	mpz_class q = blum->getQ();
	
	SCM l = SCM_EOL;
	l = scm_cons(scm_from_mpz(q.get_mpz_t()), l);
	l = scm_cons(scm_from_mpz(p.get_mpz_t()), l);
	l = scm_cons(scm_from_mpz(m.get_mpz_t()), l);
	return scm_values(l);
}

static SCM mtfa_blum_set_values (SCM sblum, SCM sm, SCM sp, SCM sq)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class  m,p,q;

	scm_to_mpz(sm, m.get_mpz_t());
	blum->setM(m);
	
	if (sp != SCM_EOL)
	{
		scm_to_mpz(sp, p.get_mpz_t());
		blum->setP(p);
	}
		
	if (sq != SCM_EOL)
	{
		scm_to_mpz(sq, q.get_mpz_t());
		blum->setQ(q);
	}
	
	return SCM_BOOL_T;
}

static SCM mtfa_bv_to_num (SCM buffer)
{
	mpz_class x;

/*	if (scm_is_string(buffer)) {
		unsigned char* p;
		size_t len;
		p = ( unsigned char* ) ScmToStringn ( buffer, &len );
		x = UnsignedPChar2Mpz(p, len);
		free(p);
		return scm_from_mpz(x.get_mpz_t());
	} else */
	if (scm_is_bytevector(buffer)) {
		int len = SCM_BYTEVECTOR_LENGTH ( buffer );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( buffer );
		x = UnsignedPChar2Mpz(p, len);
		return scm_from_mpz(x.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}

static SCM mtfa_bv_to_hex (SCM buffer)
{
    static const vector<string> ntoh = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af", "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf", "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df", "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
    };
	if (scm_is_bytevector(buffer)) {
		int len = SCM_BYTEVECTOR_LENGTH ( buffer );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( buffer );
        char *s=(char*)malloc(len*2+1);
        s[len*2]=0;
        char *ps = s;
        for (int i=0; i<len; ++i)
        {
            memcpy(ps, &p[i], 2);
            ps += 2;
        }
        SCM sRet = ScmFromString(s);
        free(s);
        return sRet;
	}
	else
		return SCM_BOOL_F;
}

static SCM mtfa_num_to_bv(SCM num)
{
	mpz_class x;
	
	if (scm_is_number(num)) {
		scm_to_mpz(num, x.get_mpz_t());
		unsigned char* p;
		int len;
		Mpz2UnsignedPChar(&x, p, len);
		return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) p, free ),
										   scm_from_int ( len ),
										   scm_from_int ( 0 ),
										   scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
		
	} else
		return SCM_BOOL_F;
}

static SCM mtfa_blum_get_next (SCM smpz_x, SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class x;
	//x potrebbe anche essere un bytevector
	if (scm_is_number(smpz_x)) {
		scm_to_mpz(smpz_x, x.get_mpz_t());
		mpz_class ret = BlumAlgo::getNext(x, *blum);
		return scm_from_mpz(ret.get_mpz_t());
	} else if (scm_is_bytevector(smpz_x)) {
		int len = SCM_BYTEVECTOR_LENGTH ( smpz_x );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( smpz_x );
		x = UnsignedPChar2Mpz(p, len);
		mpz_class ret = BlumAlgo::getNext(x, *blum);
		return scm_from_mpz(ret.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}
static SCM mtfa_blum_get_prev (SCM smpz_x, SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class x;
	//x potrebbe anche essere un bytevector
	if (scm_is_number(smpz_x)) {
		scm_to_mpz(smpz_x, x.get_mpz_t());
		mpz_class ret = BlumAlgo::getPrevious(x, *blum);
		return scm_from_mpz(ret.get_mpz_t());
	} else if (scm_is_bytevector(smpz_x)) {
		int len = SCM_BYTEVECTOR_LENGTH ( smpz_x );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( smpz_x );
		x = UnsignedPChar2Mpz(p, len);
		mpz_class ret = BlumAlgo::getPrevious(x, *blum);
		return scm_from_mpz(ret.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}

static SCM mtfa_blum_get_next_ith (SCM smpz_x, SCM sint_i, SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class x;
	//x potrebbe anche essere un bytevector
	if (scm_is_number(smpz_x)) {
		scm_to_mpz(smpz_x, x.get_mpz_t());
		mpz_class ret = BlumAlgo::getI_esimo_fast(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	} else if (scm_is_bytevector(smpz_x)) {
		int len = SCM_BYTEVECTOR_LENGTH ( smpz_x );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( smpz_x );
		x = UnsignedPChar2Mpz(p, len);
		mpz_class ret = BlumAlgo::getI_esimo_fast(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}
static SCM mtfa_blum_get_prev_ith (SCM smpz_x, SCM sint_i, SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class x;
	//x potrebbe anche essere un bytevector
	if (scm_is_number(smpz_x)) {
		scm_to_mpz(smpz_x, x.get_mpz_t());
		mpz_class ret = BlumAlgo::getK_Previous(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	} else if (scm_is_bytevector(smpz_x)) {
		int len = SCM_BYTEVECTOR_LENGTH ( smpz_x );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( smpz_x );
		x = UnsignedPChar2Mpz(p, len);
		mpz_class ret = BlumAlgo::getK_Previous(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}
static SCM mtfa_blum_get_next_ith_slow (SCM smpz_x, SCM sint_i, SCM sblum)
{
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	mpz_class x;
	//x potrebbe anche essere un bytevector
	if (scm_is_number(smpz_x)) {
		scm_to_mpz(smpz_x, x.get_mpz_t());
		mpz_class ret = BlumAlgo::getI_esimo_slow(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	} else if (scm_is_bytevector(smpz_x)) {
		int len = SCM_BYTEVECTOR_LENGTH ( smpz_x );
		unsigned char * p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( smpz_x );
		x = UnsignedPChar2Mpz(p, len);
		mpz_class ret = BlumAlgo::getI_esimo_slow(x, scm_to_int(sint_i), *blum);
		return scm_from_mpz(ret.get_mpz_t());
	}
	else
		return SCM_BOOL_F;
}

//Gestione di cifra e decifra utilizzando esclusivamente blum e i residui quadratici di M
static SCM mtfa_blum_make_token(SCM smessage, SCM sint_i, SCM sblum)
{
	//x: stringa, i: secondi, sblum: blum
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );

	if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(GetULLTime());
	
	//allora, vediamo quanti bit posso utilizzare
	mpz_class m = blum->getM();
	int numbits = mpz_sizeinbase(m.get_mpz_t(), 2);
	
	//Dalla lunghezza della chiave togliamo 1+2(short)+msglen chars+8 (secondi)+8(parte finale)
	int maxmsglen = 512;
// 	if (numbits > 2048)
// 	{
// 		maxmsglen = 512-(1+2+8+8);
// 	}
// 	else if (numbits > 1024)
// 	{
// 		maxmsglen = 256-(1+2+8+8);
// 	}
// 	else if (numbits > 512)
// 	{
// 		maxmsglen = 128-(1+2+8+8);
// 	}
// 	else if (numbits > 256)
// 	{
// 		maxmsglen = 64-(1+2+8+8);
// 	}
// 	else if (numbits > 128)
// 	{
// 		maxmsglen = 32-(1+2+8+8);
// 	}
// 	else
// 		return scm_from_utf8_string("blum key too short");
	
	if (numbits<128)
	{
		return scm_from_utf8_string("blum key too short");
	}
	else
	{
		maxmsglen = (numbits>>3)-(1+2+8+8);
	}
	
	if (scm_c_string_length(smessage) > maxmsglen)
	{
		char msg[128];
		sprintf(msg, "Message cannot be longer that %d chars", maxmsglen);
		return scm_from_utf8_string(msg);
	}

	unsigned char code[8192]; //maximum acceptable
	//memset(code, 0, 8192); //non serve, quello che uso lo inizializzo, il resto può essere random

	char * message; //uso un messaggio fisso
	unsigned short int messageLen;
	size_t messageLenSizet;
	message = scm_to_utf8_stringn(smessage, &messageLenSizet);
	messageLen = messageLenSizet;

	//tutto è organizzato in lunghezza seguito dai dati
	//il primo byte vale 0x1
	code[0] = 1;
	*(size_t*)&code[1] = messageLen;
	memcpy(&code[1+sizeof(unsigned short)], message, messageLen);

	//rilascio la memoria a mano a mano che non serve più
	free(message);

	//Aggiungo i byte della durata
	unsigned long timeout = scm_to_ulong(sint_i);
	*(unsigned long*)&code[1+sizeof(unsigned short)+messageLen] = timeout;

	//ora, per quanto riguarda la parte finale, metto gli 8 bytes del codice random
	*(unsigned long long*)&code[maxmsglen+(1+2+8+8)-8] = simple_generator->next();

	//converto il tutto in mpz e faccio i test, sommando 1 per cambiare base
	mpz_class x = UnsignedPChar2Mpz(&code[0], maxmsglen+(1+2+8+8));

	mpz_class next;
	mpz_class prec;
	do {
		//cout << "Test number " << x << endl;
		x++;
		next = BlumAlgo::getNext(x, *blum);
		prec = BlumAlgo::getK_Previous(next, 1, *blum);
	} while (x != prec);
	
	return scm_from_mpz(next.get_mpz_t());
}

static SCM mtfa_blum_identify_token(SCM token, SCM sblum)
{
	//x: stringa, i: secondi, sblum: blum
	BlumInteger* blum = ( BlumInteger* ) scm_to_pointer ( sblum );
	
	if (!simple_generator)
		simple_generator = new mtfa_fastrng1024(GetULLTime());
	
	//converto prec in code
	mpz_class next, prec;
	scm_to_mpz(token, next.get_mpz_t());
	
	//ne faccio il prec
	prec = BlumAlgo::getK_Previous(next, 1, *blum);
	
	//Estraggo i due dati che servono!
	unsigned char * code;
	int lenCode;
	Mpz2UnsignedPChar(&prec, code, lenCode);
	
	if (!code || code[0] != 1)
	{
		return SCM_BOOL_F;
	}
	
	unsigned short messageLen;
	unsigned char * message;
	
	messageLen = *(unsigned short*)&code[1];
	message = (unsigned char*)malloc(messageLen);
	memcpy(message, &code[1+sizeof(unsigned short)], messageLen);

	//Ora il tempo di scadenza (i secondi)
	unsigned long timeout;
	timeout = *(unsigned long*)&code[1+sizeof(unsigned short)+messageLen];
	
	SCM smsg;
	SCM stimeout;
	
	smsg = scm_from_utf8_stringn((const char*)message, messageLen);
	free(message);
	
	stimeout = scm_from_ulong(timeout);

	return scm_cons(smsg, stimeout);
}


static void rsa_finalizer ( void* vrsa )
{
    if ( vrsa )
        delete ( c_rsa* ) vrsa;

    vrsa = nullptr;
}
static SCM mtfa_make_rsa()
{
    c_rsa* rsa = new c_rsa();
    return scm_from_pointer ( ( void* ) rsa, &rsa_finalizer );
}

/*Padding types:
#define RSA_PKCS1_PADDING	1
#define RSA_SSLV23_PADDING	2
#define RSA_NO_PADDING		3
#define RSA_PKCS1_OAEP_PADDING	4
#define RSA_X931_PADDING	5

*/
SCM rsa_PubCifra ( SCM scm_rsa, SCM messaggio, SCM padding );
SCM rsa_PubCifra ( SCM scm_rsa, SCM messaggio, SCM padding )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* cifrato;
    int lencifrato;
    bool bret;
    
    int ipadding = scm_to_int(padding);

    if ( scm_is_string ( messaggio ) )
    {
        unsigned char* p;
        size_t len;
        p = ( unsigned char* ) ScmToStringn ( messaggio, &len );
        bret = rsa->PubCifra ( &cifrato, &lencifrato, p, len, ipadding );
        //Qui posso fare la free di p
        free ( p );
    }
    else if ( scm_is_bytevector ( messaggio ) )
    {
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->PubCifra ( &cifrato, &lencifrato, p, len, ipadding );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) cifrato, free ),
                                           scm_from_int ( lencifrato ),
                                           scm_from_int ( 0 ),
                                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

SCM rsa_PrivCifra ( SCM scm_rsa, SCM messaggio, SCM padding );
SCM rsa_PrivCifra ( SCM scm_rsa, SCM messaggio, SCM padding )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* cifrato;
    int lencifrato;
    bool bret;
    
    int ipadding = scm_to_int(padding);


    if ( scm_is_string ( messaggio ) )
    {
        unsigned char* p;
        size_t len;
        p = ( unsigned char* ) ScmToStringn ( messaggio, &len );
        bret = rsa->PrivCifra ( &cifrato, &lencifrato, p, len, ipadding );
        //Qui posso fare la free di p
        free ( p );
    }
    else if ( scm_is_bytevector ( messaggio ) )
    {
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->PrivCifra ( &cifrato, &lencifrato, p, len, ipadding );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) cifrato, free ),
                                           scm_from_int ( lencifrato ),
                                           scm_from_int ( 0 ),
                                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

SCM rsa_PubDecifra ( SCM scm_rsa, SCM messaggio, SCM padding );
SCM rsa_PubDecifra ( SCM scm_rsa, SCM messaggio, SCM padding )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* decifrato;
    int lendecifrato;
    bool bret;
    int ipadding = scm_to_int(padding);

    if ( scm_is_bytevector ( messaggio ) )
    {
        //in questo caso la decifra avviene sull'oggetto, senza duplicazioni
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->PubDecifra ( &decifrato, &lendecifrato, p, len, ipadding );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) decifrato, free ),
                                           scm_from_int ( lendecifrato ),
                                           scm_from_int ( 0 ),
                                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

SCM rsa_PrivDecifra ( SCM scm_rsa, SCM messaggio, SCM padding );
SCM rsa_PrivDecifra ( SCM scm_rsa, SCM messaggio, SCM padding )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* decifrato;
    int lendecifrato;
    bool bret;
    int ipadding = scm_to_int(padding);

    if ( scm_is_bytevector ( messaggio ) )
    {
        //in questo caso la decifra avviene sull'oggetto, senza duplicazioni
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->PrivDecifra ( &decifrato, &lendecifrato, p, len, ipadding );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) decifrato, free ),
                                           scm_from_int ( lendecifrato ),
                                           scm_from_int ( 0 ),
                                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

/*Sign types:
        0= NID_sha1;
        1= NID_ripemd160;
        2= NID_md5;
        3= NID_md5_sha1;
        4= NID_sha224;
        5= NID_sha256;
        6= NID_sha384;
        7= NID_sha512;
*/
SCM rsa_Sign ( SCM scm_rsa, SCM messaggio );
SCM rsa_Sign ( SCM scm_rsa, SCM messaggio )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* bsigned;
    int lensigned;
    bool bret;

    if ( scm_is_string ( messaggio ) )
    {
        unsigned char* p;
        size_t len;
        p = ( unsigned char* ) ScmToStringn ( messaggio, &len );
        bret = rsa->Sign ( &bsigned, &lensigned, p, ( int ) len );
        //Qui posso fare la free di p
        free ( p );
    }
    else if ( scm_is_bytevector ( messaggio ) )
    {
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->Sign ( &bsigned, &lensigned, p, len );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return scm_pointer_to_bytevector ( scm_from_pointer ( ( void* ) bsigned, free ),
                                           scm_from_int ( lensigned ),
                                           scm_from_int ( 0 ),
                                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

SCM rsa_Verify ( SCM scm_rsa, SCM scm_signed, SCM messaggio );
SCM rsa_Verify ( SCM scm_rsa, SCM scm_signed, SCM messaggio )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* bsigned;
    int lensigned;

    lensigned = SCM_BYTEVECTOR_LENGTH ( scm_signed );
    bsigned = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( scm_signed );

    bool bret;

    if ( scm_is_string ( messaggio ) )
    {
        unsigned char* p;
        size_t len;
        p = ( unsigned char* ) ScmToStringn ( messaggio, &len );
        bret = rsa->Verify ( bsigned, lensigned, p, len );
        //Qui posso fare la free di p
        free ( p );
    }
    else if ( scm_is_bytevector ( messaggio ) )
    {
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( messaggio );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( messaggio );
        bret = rsa->Verify ( bsigned, lensigned, p, len );
    }
    else return SCM_EOL;

    if ( !bret )
        return SCM_EOL;
    else
        return SCM_BOOL_T;
}

SCM rsa_MakeKey ( SCM scm_rsa, SCM bits );
SCM rsa_MakeKey ( SCM scm_rsa, SCM bits )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    rsa->MakeKey ( scm_to_int ( bits ) );
    return SCM_EOL;
}

SCM rsa_MakeKey_p_q ( SCM scm_rsa, SCM bits, SCM p, SCM q );
SCM rsa_MakeKey_p_q ( SCM scm_rsa, SCM bits, SCM p, SCM q )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    mpz_class cp, cq;
    scm_to_mpz ( p, cp.get_mpz_t() );
    scm_to_mpz ( q, cq.get_mpz_t() );
    rsa->MakeKey ( scm_to_int ( bits ), cp.get_mpz_t(), cq.get_mpz_t() );
    return SCM_EOL;
}

SCM rsa_GetPublic ( SCM scm_rsa );
SCM rsa_GetPublic ( SCM scm_rsa )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    char* p = rsa->GetPublic();

    if ( !p )
        return SCM_EOL;

    SCM ret = ScmFromString ( p );
    free ( p );
    return ret;
}

SCM rsa_GetPrivate ( SCM scm_rsa );
SCM rsa_GetPrivate ( SCM scm_rsa )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    char* p = rsa->GetPrivate();

    if ( !p )
        return SCM_EOL;

    SCM ret = ScmFromString ( p );
    free ( p );
    return ret;
}

SCM rsa_SetPubKey ( SCM scm_rsa, SCM pub );
SCM rsa_SetPubKey ( SCM scm_rsa, SCM pub )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* p;
    size_t len;
    p = ( unsigned char* ) ScmToStringn ( scm_rsa, &len );
    rsa->SetPubKey ( ( const char* ) p );
    free ( p );
    return SCM_BOOL_T;
}

SCM rsa_SetPrivKey ( SCM scm_rsa, SCM pub );
SCM rsa_SetPrivKey ( SCM scm_rsa, SCM pub )
{
    c_rsa* rsa = ( c_rsa* ) scm_to_pointer ( scm_rsa );
    unsigned char* p;
    size_t len;
    p = ( unsigned char* ) ScmToStringn ( scm_rsa, &len );
    rsa->SetPrivKey ( ( const char* ) p );
    free ( p );
    return SCM_BOOL_T;
}

static SCM mtfa_run_generator_string
(
    SCM scm_string_pin,              //il pin
    SCM scm_string_sprovider,        //il service provider
    SCM scm_lnum,                    //una sequenza di numeri (disturber o altro)
    SCM scm_len                      //the desired lenght of the output
    //,SCM scm_rules                    //Lista di liste fatta da "alfabeto" min_num max_num
)
{
    loadmatrix();   //per caricarla nel caso in cui non lo fosse
    char* pin = ScmToString ( scm_string_pin );
    char* sprov = ScmToString ( scm_string_sprovider );
    int outlen = 2 * scm_to_int ( scm_len );

    if ( scm_lnum == SCM_EOL )
    {
        scm_lnum = scm_cons ( scm_from_long_long ( 961756489961756487 ), scm_lnum );
    }

    //Ora devo espandere il PIN e il SPROVIDER
    mpz_class npin = UnsignedPChar2Mpz ( ( unsigned char* ) pin, strlen ( pin ) );
    free(pin);

    if ( npin == 0 )
    {
        npin = 3486104543266482133;
    }

    mpz_class nsprov = UnsignedPChar2Mpz ( ( unsigned char* ) sprov, strlen ( sprov ) );
    free(sprov);

    if ( nsprov == 0 )
    {
        nsprov = 5409171536436789259;
    }

    //faccio tutto al quadrato???
    npin *= (13+npin);
    nsprov *= (19+nsprov);

    //il modulo è rapprensentato da 256 bytes presi sulla matrice a partire dagli elementi della lista
    //Questo dovrei farlo dipendere da pin e provider
    unsigned char modulo[256];
    mpz_class somma = npin*8192+nsprov*4096+outlen*2048;
    somma %= ( __dim_rmatrix__ - 256 );
    memcpy ( modulo, &__rmatrix__[somma.get_ui()], 256 );

    //Altero il modulo con la lista disturber e operazione xor
    SCM lista = scm_lnum;
    unsigned long long idx = 0;
    while ( lista != SCM_EOL )
    {
        unsigned long long pos = scm_to_long_long ( SCM_CAR ( lista ) );
        idx = (idx * pos * (unsigned long long)outlen) % __dim_rmatrix__;
        lista = SCM_CDR ( lista );
    }
    //ora idx è inizializzato
    
    mpz_class gmpres = modulo[0];
    mpz_class base = 1;
    base = (base << 2048) - 1; //256 byte di modulo
    
    while ( mpz_sizeinbase(gmpres.get_mpz_t(), 16) < outlen )
    {
        lista = scm_lnum;
        while ( lista != SCM_EOL )
        {
            long long pos = scm_to_long_long ( SCM_CAR ( lista ) );

            for ( int i = 0; i < 256; ++i )
            {
                idx = 1021 + idx + __rmatrix__[idx % __dim_rmatrix__];
                modulo[ ( idx++ + i + pos ) % 256] ^= __rmatrix__[ ( idx + i + pos ) % __dim_rmatrix__];
            }

            lista = SCM_CDR ( lista );
        }

        //Converto in mpz
        mpz_class mod = UnsignedPChar2Mpz ( modulo, 256 );
//         if (mod < 2)  //tanto per evitare situazioni strane
//             mod = npin;
// 
//         mpz_class res;
//         mpz_class add = mod*8192+npin*4096+nsprov*2048+outlen*1024;
//         mpz_powm (  res.get_mpz_t(), mod.get_mpz_t(), add.get_mpz_t(),   base.get_mpz_t() );
//         
//         //cout << "Genera: " << res << ". precedente: " << gmpres << endl;
// 
         gmpres = (gmpres << (mpz_sizeinbase(mod.get_mpz_t(), 2))) + mod;
    }

    string sres = gmpres.get_str(16);
    sres = sres.substr ( 0, outlen);
    SCM scm_ret = ScmFromString ( sres.c_str() );
    return scm_ret;
}

static SCM mtfa_run_generator_bv
(
    SCM scm_string_pin,              //il pin
    SCM scm_string_sprovider,        //il service provider
    SCM scm_lnum,                    //una sequenza di numeri (disturber o altro)
    SCM scm_len                     //the desired lenght of the output
    //,SCM scm_rules                    //Lista di liste fatta da "alfabeto" min_num max_num
)
{
    loadmatrix();   //per caricarla nel caso in cui non lo fosse
    char* pin = ScmToString ( scm_string_pin );
    char* sprov = ScmToString ( scm_string_sprovider );
    int outlen = 2*scm_to_int ( scm_len );

    if ( scm_lnum == SCM_EOL )
    {
        scm_lnum = scm_cons ( scm_from_long_long ( 961756489961756487 ), scm_lnum );
    }

    //Ora devo espandere il PIN e il SPROVIDER
    mpz_class npin = UnsignedPChar2Mpz ( ( unsigned char* ) pin, strlen ( pin ) );

    if ( npin == 0 )
    {
        npin = 3486104543266482133;
    }

    mpz_class nsprov = UnsignedPChar2Mpz ( ( unsigned char* ) sprov, strlen ( sprov ) );

    if ( nsprov == 0 )
    {
        nsprov = 5409171536436789259;
    }

    //faccio tutto al quadrato
    npin *= npin;
    nsprov *= nsprov;

    string sres = string ( outlen, 0x57 );
    for ( int i = 0; i < outlen; ++i )
    {
        SCM lista = scm_lnum;
        unsigned long long idx = 0;
        while ( lista != SCM_EOL )
        {
            long long pos = scm_to_long_long ( SCM_CAR ( lista ) );
            idx = ((idx + pos)<<1) % __dim_rmatrix__;
            lista = SCM_CDR ( lista );
        }
        
        //ora idx è inizializzato

        lista = scm_lnum;

        while ( lista != SCM_EOL )
        {
            long long pos = scm_to_long_long ( SCM_CAR ( lista ) );
            idx += * ( unsigned int* ) ( &__rmatrix__[ ( idx > 4 ) ? ( idx - 4 ) : idx] );
            idx %= __dim_rmatrix__;
            sres[i] ^= __rmatrix__[ ( idx + i + pos ) % __dim_rmatrix__];
            lista = SCM_CDR ( lista );
        }
    }

    unsigned char* p = ( unsigned char* ) malloc ( outlen );
    memcpy ( p, sres.data(), outlen );
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) p, free ),
                      scm_from_int ( outlen ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}


static SCM local_mtfa_base_62 ( SCM scm_to_be_based )
{
    mpz_class cvalue;
    string sret;

    if ( scm_is_number( scm_to_be_based ) )
    {
		mpz_class value;
		scm_to_mpz(scm_to_be_based, value.get_mpz_t());
		sret = "I" + value.get_str ( 62 );
    }
    else if ( scm_is_string ( scm_to_be_based ) )
    {
        unsigned char* p;
        size_t len;
        p = ( unsigned char* ) ScmToStringn ( scm_to_be_based, &len );
        cvalue = UnsignedPChar2Mpz ( p, len );
        free ( p );
        sret = "S" + cvalue.get_str ( 62 );
    }
    else if ( scm_is_bytevector ( scm_to_be_based ) )
    {
        //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
        unsigned char* p;
        int len;
        len = SCM_BYTEVECTOR_LENGTH ( scm_to_be_based );
        p = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( scm_to_be_based );
        cvalue = UnsignedPChar2Mpz ( p, len );
        sret = "V" + cvalue.get_str ( 62 );
    }
    else
    {
        return SCM_EOL;
    }

    return ScmFromString ( sret.c_str() );
}

static SCM local_mtfa_debase_62 ( SCM scm_to_be_debased )
{
    unsigned char* p;
    size_t len;
    p = ( unsigned char* ) ScmToStringn ( scm_to_be_debased, &len );
    string sp ( ( const char* ) &p[1], len - 1 );
    char kind = p[0];
    mpz_class cvalue;
    cvalue.set_str ( sp, 62 );
    free ( p );
	if (kind == 'I')
	{
		//il numero è già OK
		return scm_from_mpz(cvalue.get_mpz_t());
	}

    p = NULL;
    int ilen;
    Mpz2UnsignedPChar ( &cvalue, p, ilen );

    switch ( kind )
    {
//         case 'I':   //int
//         {
//             //Gli interi li tratta in modo differente
//             free ( p );
// 			mpz_class val;
// 			val.set_str(cvalue, 62);
//             string sval = cvalue.get_str ( 10 );
//             unsigned long long ull = strtoull ( sval.c_str(), 0, 10 );
//             SCM scm_ret = scm_from_ulong_long ( ull );
//             return scm_ret;
//         }

        case 'S':   //string
        {
            SCM scm_ret = ScmFromStringn ( ( const char* ) p, ilen );
            free ( p );
            return scm_ret;
        }

        case 'V':   //bytevector
        {
            SCM scm_ret = scm_pointer_to_bytevector (
                              scm_from_pointer ( ( void* ) p, free ),
                              scm_from_int ( ilen ),
                              scm_from_int ( 0 ),
                              scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
            return scm_ret;
        }

        default:
            return SCM_EOL;
    }
}

static SCM mtfa_cypher_generator
(
    SCM scm_to_be_cyphered,   //il bv/stringa da cifrare
    SCM scm_string_start,        //il pin che sblocca la cifra
    SCM scm_string_step          //come proseguo con la cifra
)
{
    loadmatrix();   //Per caricarla nel caso in cui non lo fosse

    //Ammetto che da_cifrare possa essere una stringa oppure un bytevector
    size_t len = 0;
    unsigned char* p;

    if ( scm_is_string ( scm_to_be_cyphered ) )
    {
        p = ( unsigned char* ) ScmToStringn ( scm_to_be_cyphered, &len );
    }
    else
    {
        if ( scm_is_bytevector ( scm_to_be_cyphered ) )
        {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            len = SCM_BYTEVECTOR_LENGTH ( scm_to_be_cyphered );
            p = ( unsigned char* ) malloc ( len );
            memcpy ( p, ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_to_be_cyphered ), len );
        }
        else
        {
            return SCM_EOL;
        }
    }

    size_t len1, len2;
    unsigned char* p1 = ( unsigned char* ) ScmToStringn ( scm_string_start, &len1 );
    unsigned char* p2 = ( unsigned char* ) ScmToStringn ( scm_string_step,  &len2 );

    mpz_class mpz_pin  = UnsignedPChar2Mpz ( p1, len1 );
    mpz_class mpz_step = UnsignedPChar2Mpz ( p2, len2 );

    free ( p1 );
    free ( p2 );

    long long istart  = mpz_fdiv_ui ( mpz_pin.get_mpz_t(),  __dim_rmatrix__ );
    long long istep   = mpz_fdiv_ui ( mpz_step.get_mpz_t(), __dim_rmatrix__ );

//    long long istart = scm_to_long_long(scm_int_start) % __dim_rmatrix__;
//    long long istep  = scm_to_long_long(scm_int_step);

//    unsigned char * pret = (unsigned char *) malloc(len);
    SimpleRng srng;
    srng.Next ( istep );
//    memcpy(pret, p, len);
//    free(p);

    for ( int i = 0; i < len; ++i )
    {
        p[i] ^= __rmatrix__[istart];
        istart = ( istart + srng.Next() ) % __dim_rmatrix__;
    }

    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) p, free ),
                      scm_from_int ( len ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_sqlite3_do ( SCM dbname, SCM statement )
{
    size_t len;

    char* p = ( char* ) ScmToStringn ( dbname, &len );
    string s_dbname = string ( p, len );
    free ( p );

    p = ( char* ) ScmToStringn ( statement, &len );
    string s_statement = string ( p, len );
    free ( p );

    mtfa_db::TDynamicTable result;    // A list of list of strings
    mtfa_db::Execute ( s_dbname, s_statement, result );

    SCM scm_res = SCM_EOL;

    for ( auto line = result.rbegin(); line != result.rend(); ++line )
    {
        SCM scm_ls = SCM_EOL;

        for ( auto it = line->rbegin(); it != line->rend(); ++it )
        {
            scm_ls = scm_cons ( ScmFromString ( it->c_str() ), scm_ls );
        }

        scm_res = scm_cons ( scm_ls, scm_res );
    }

    return scm_res;
}

static void finalizer ( void* punt )
{
    mtfa_db::Close ( ( sqlite3* ) punt );
}
static SCM mtfa_sqlite3_open ( SCM dbname )
{
    size_t len;
    char* p = ( char* ) ScmToStringn ( dbname, &len );
    sqlite3* db = mtfa_db::Open ( string ( p, len ) );
    free ( p );

    SCM scm_ret = scm_from_pointer ( db, finalizer );
    return scm_ret;
}
static SCM mtfa_sqlite3_close ( SCM scm_db )
{
    mtfa_db::Close ( ( sqlite3* ) scm_to_pointer ( scm_db ) );
    return SCM_BOOL_T;
}
static SCM mtfa_sqlite3_execute ( SCM scm_db, SCM scm_statement )
{
    size_t len;
    char* p = ( char* ) ScmToStringn ( scm_statement, &len );
    string statement = string ( p, len );
    free ( p );

    sqlite3* db = ( sqlite3* ) scm_to_pointer ( scm_db );
    mtfa_db::TDynamicTable result;    // A list of list of strings
    mtfa_db::Execute ( db, statement, result );

    SCM scm_res = SCM_EOL;

    for ( auto line = result.rbegin(); line != result.rend(); ++line )
    {
        SCM scm_ls = SCM_EOL;

        for ( auto it = line->rbegin(); it != line->rend(); ++it )
        {
            scm_ls = scm_cons ( ScmFromString ( it->c_str() ), scm_ls );
        }

        scm_res = scm_cons ( scm_ls, scm_res );
    }

    return scm_res;
}


/*cifra e hashes*/
static bool ScmToPUChar ( SCM scm_data, unsigned char*& p, size_t& len )
{
    len = 0;

    if ( scm_is_string ( scm_data ) )
    {
        p = ( unsigned char* ) ScmToStringn ( scm_data, ( size_t* ) &len );
    }
    else
    {
        if ( scm_is_bytevector ( scm_data ) )
        {
            //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
            len = SCM_BYTEVECTOR_LENGTH ( scm_data );
            p = ( unsigned char* ) malloc ( len );
            memcpy ( p, ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_data ), len );
        }
        else
        {
            return false;
        }
    }

    return true;
}

static SCM mtfa_hmac ( SCM scm_hash, SCM scm_key, SCM scm_message )
{
    //static string hmac(const unsigned char * key, int len_key, const unsigned char * message, int len_message)
    if ( !scm_is_integer ( scm_hash ) )
    {
        SCM l = SCM_EOL;
        l = scm_cons ( scm_cons ( ScmFromString ( "RIPEMD160" ), scm_from_int ( 6 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "MD5" ), scm_from_int ( 5 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA512" ), scm_from_int ( 4 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA384" ), scm_from_int ( 3 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA256" ), scm_from_int ( 2 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA224" ), scm_from_int ( 1 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA1" ), scm_from_int ( 0 ) ), l );
        return l;
    }

    //Ammetto che sia key che message possano essere stringhe o bytevector
    size_t len_key = 0;
    unsigned char* p_key;
    bool bret = ScmToPUChar ( scm_key, p_key, len_key );

    if ( !bret )
    {
        return SCM_EOL;
    }

    size_t len_message = 0;
    unsigned char* p_message;
    bret = ScmToPUChar ( scm_message, p_message, len_message );

    if ( !bret )
    {
        free ( p_key );
        return SCM_EOL;
    }

    string result = c_hashes::hmac ( scm_to_int ( scm_hash ), p_key, len_key, p_message, len_message );

    free ( p_key );
    free ( p_message );
    unsigned char* p_res = ( unsigned char* ) malloc ( result.size() );
    memcpy ( p_res, result.data(), result.size() );

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) p_res, free ),
                      scm_from_int ( result.size() ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_sha ( SCM scm_hash, SCM scm_message )
{
    //static string hmac(const unsigned char * key, int len_key, const unsigned char * message, int len_message)
    if ( !scm_is_integer ( scm_hash ) )
    {
        SCM l = SCM_EOL;
        l = scm_cons ( scm_cons ( ScmFromString ( "RIPEMD160" ), scm_from_int ( 6 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "MD5" ), scm_from_int ( 5 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA512" ), scm_from_int ( 4 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA384" ), scm_from_int ( 3 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA256" ), scm_from_int ( 2 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA224" ), scm_from_int ( 1 ) ), l );
        l = scm_cons ( scm_cons ( ScmFromString ( "SHA1" ), scm_from_int ( 0 ) ), l );
        return l;
    }

    size_t len_message = 0;
    unsigned char* p_message;
    bool bret = ScmToPUChar ( scm_message, p_message, len_message );

    if ( !bret )
    {
        return SCM_EOL;
    }

    string result;

    switch ( scm_to_int ( scm_hash ) )
    {
        case 0:
            result = c_hashes::sha1 ( ( char* ) p_message, len_message );
            break;

        case 1:
            result = c_hashes::sha224 ( ( char* ) p_message, len_message );
            break;

        case 2:
            result = c_hashes::sha256 ( ( char* ) p_message, len_message );
            break;

        case 3:
            result = c_hashes::sha384 ( ( char* ) p_message, len_message );
            break;

        case 4:
            result = c_hashes::sha512 ( ( char* ) p_message, len_message );
            break;

        case 5:
            result = c_hashes::md5 ( ( char* ) p_message, len_message );
            break;

        case 6:
            result = c_hashes::ripemd160 ( ( char* ) p_message, len_message );
            break;

        default:
            result = c_hashes::sha256 ( ( char* ) p_message, len_message );
            break;
    }

    free ( p_message );
    unsigned char* p_res = ( unsigned char* ) malloc ( result.size() );
    memcpy ( p_res, result.data(), result.size() );

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) p_res, free ),
                      scm_from_int ( result.size() ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_cifra_aes_js ( SCM scm_message, SCM scm_key )
{

    size_t len_message = 0;
    unsigned char* p_message;
    bool bret = ScmToPUChar ( scm_message, p_message, len_message );

    if ( !bret )
    {
        return SCM_EOL;
    }

    size_t len_key = 0;
    unsigned char* p_key;
    bret = ScmToPUChar ( scm_key, p_key, len_key );

    if ( !bret )
    {
        free ( p_message );
        return SCM_EOL;
    }

    unsigned char* cifrato;
    int len_cifrato;
    bret = c_scyphers::aes_cifra_js ( &cifrato, &len_cifrato, p_message, len_message, p_key, len_key );

    free ( p_key );
    free ( p_message );

    if ( !bret )
    {
        return SCM_EOL;
    }

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) cifrato, free ),
                      scm_from_int ( len_cifrato ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_decifra_aes_js ( SCM scm_message, SCM scm_key )
{

    size_t len_message = 0;
    unsigned char* p_message;
    bool bret = ScmToPUChar ( scm_message, p_message, len_message );

    if ( !bret )
    {
        return SCM_EOL;
    }

    size_t len_key = 0;
    unsigned char* p_key;
    bret = ScmToPUChar ( scm_key, p_key, len_key );

    if ( !bret )
    {
        free ( p_message );
        return SCM_EOL;
    }

    unsigned char* cifrato;
    int len_cifrato;
    bret = c_scyphers::aes_decifra_js ( &cifrato, &len_cifrato, p_message, len_message, p_key, len_key );

    free ( p_key );
    free ( p_message );

    if ( !bret )
    {
        return SCM_EOL;
    }

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) cifrato, free ),
                      scm_from_int ( len_cifrato ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

//base32 encoding e decoding
/*
Encoding Data
In order to use encode a byte array, find out first how long the output base32 data buffer needs to be. Given that 32 values are less than 256 values, the encoded array will be somehow longer. Finding the encoded buffer length can be done as follows:

#define INPUT_LEN 123

unsigned char data256[INPUT_LEN] ;
// fill the data buffer with data
// ...

int encodeLength = Base32.GetEncode32Length(INPUT_LEN);
unsigned char data32[] = new char[encodeLength];
Then the data can be encoded in base32:

if(!Base32.Encode32(data256, INPUT_LEN, data32))
{
 //error
}
Mapping To A Base32 Alphabet
After the base32 encoding of the data, a mapping to an alphabet can be done as follows:

const char alphabet[] = "123456789ABCDEFGHJKMNPQRSTUVWXYZ";
Base32.Map32(data32, encodeLength, alphabet);
data32 data values are mapped in place.

Reversing The Process
To reverse the process just repeat the symmetrical steps:

Base32.Unmap32(data32, encodeLength, alphabet);
As with mapping, the unmap is done in place. The decoding code follows:

int decodeLength = Base32.GetDecode32Length(data32);
char decode256[] = new char[decodeLength];

Base32.Decode32(data32, encodeLength, decode256);
Finally, when finished, free the buffers:

delete[] data32;
delete[] decode256;
*/
static SCM mtfa_base32_encode ( SCM scm_data );
SCM mtfa_base32_encode ( SCM scm_data )
{
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    size_t len_data = 0;
    unsigned char* p_data;
    bool bret = ScmToPUChar ( scm_data, p_data, len_data );

    if ( !bret )
    {
        return SCM_EOL;
    }

    //Ora posso codificare
    int encodeLength = Base32::GetEncode32Length ( len_data );
    unsigned char* data32 = ( unsigned char* ) malloc ( encodeLength );

    if ( !Base32::Encode32 ( p_data, len_data, data32 ) )
    {
        free ( data32 );
        free ( p_data );
        return SCM_EOL;
    }

    Base32::Map32 ( data32, encodeLength, ( unsigned char* ) alphabet );

    //p_data non serve più
    free ( p_data );

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) data32, free ),
                      scm_from_int ( encodeLength ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_base32_decode ( SCM scm_data );
SCM mtfa_base32_decode ( SCM scm_data )
{
    /*Base32.Unmap32(data32, encodeLength, alphabet);
    As with mapping, the unmap is done in place. The decoding code follows:

    int decodeLength = Base32.GetDecode32Length(data32);
    char decode256[] = new char[decodeLength];

    Base32.Decode32(data32, encodeLength, decode256);
    Finally, when finished, free the buffers:

    delete[] data32;
    delete[] decode256;
    */
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    size_t len_data = 0;
    unsigned char* p_data;
    bool bret = ScmToPUChar ( scm_data, p_data, len_data );

    if ( !bret )
    {
        return SCM_EOL;
    }

    //Ora posso decodificare
    Base32::Unmap32 ( p_data, len_data, ( unsigned char* ) alphabet );

    int decodeLength = Base32::GetDecode32Length ( len_data );
    unsigned char* data32 = ( unsigned char* ) malloc ( decodeLength );

    if ( !Base32::Decode32 ( p_data, len_data, data32 ) )
    {
        free ( data32 );
        free ( p_data );
        return SCM_EOL;
    }

    //p_data non serve più
    free ( p_data );

    //Il risultato è sempre un bytevector
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( ( void* ) data32, free ),
                      scm_from_int ( decodeLength ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}



//#include <stdlib.h>
//#include <stdio.h>
//#include <string>
//#include <string.h>
//#include <iostream>
//#include <unistd.h>

/*
 * static SCM mtfa_fs_io_to_vector (SCM fname)
{
    //Prende un file e, riga per riga, lo mette in un vettore.

    #define DIM 2048000

    int linee=0;
    if (argc == 2)
    {
            FILE * fin = fopen(argv[1], "r");
            char *linea = (char*)malloc(DIM);
            string last_s = "";
            int letti;
            while ((letti=read(fileno(fin), linea, DIM)) > 0)
            {
                    char * p = linea;
                    char * last = p;
                    while ((p=strchr(p, '\n'))!=NULL)
                    {
                            //la linea va da last a p
                            last = p;
                            linee++;
                            p++;
                    }
                    //Last punta all'ultimo \n e poi ci sono ancora dati dopo di lui
                    //se p-linea != letti, allora mi manca qualcosa da leggere e
                    //lo metto in last_s
            }
    }
    cout << "Linee: " << linee << endl;
}
*/


/*
 * PER INTEGRARE COOKIE ECC
static SCM mtfa_map_string (SCM pointer, SCM stringa)
{
    //Da_decifrare può essere solo una stringa
    size_t len = 0;
    char * p;
    if (scm_string_p(stringa) == SCM_BOOL_T)
    {
        p = scm_to_latin1_stringn(da_decifrare, &len);
        string inb64 = Base64::decode(string(p, len));
        free(p);
        return scm_from_latin1_string(inb64.c_str());
    }
    else
        return SCM_EOL;
}
*/


//
//QUESTE SONO PER EIS!!!
//
static SCM mtfa_eis_get_value_current_query ( SCM scm_bufs_in_out, SCM s_key );
static SCM mtfa_eis_get_value_current_headers ( SCM scm_bufs_in_out, SCM s_key );
static SCM mtfa_eis_get_value_current_cookies ( SCM scm_bufs_in_out, SCM s_key );
static SCM mtfa_eis_get_current_body ( SCM scm_bufs_in_out, SCM scm_asString );
static SCM mtfa_eis_get_current_method ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_protocol ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_host ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_port ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_pars ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_tag ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_user_passwd ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_url ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_mac_src ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_ip_src ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_current_network_protocol ( SCM scm_bufs_in_out );
static SCM mtfa_eis_set_real_data_size ( SCM scm_bufs_in_out, SCM scm_rds );
static SCM mtfa_eis_get_tcp_data_len ( SCM scm_bufs_in_out );
static SCM mtfa_eis_get_id_sonda ( SCM scm_bufs_in_out );
//SCM mtfa_eis_is_from_client (SCM scm_bufs_in_out);
//SCM mtfa_eis_is_command_channel (SCM scm_bufs_in_out);
//SCM mtfa_eis_get_tcp_event (SCM scm_bufs_in_out);

SCM mtfa_eis_get_value_current_query ( SCM scm_bufs_in_out, SCM s_key )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    char* key;
    size_t len;
    key = ( char* ) ScmToString ( s_key );
    auto found = bufs_in_out->current_query.find ( key );
    free ( key );

    if ( found == bufs_in_out->current_query.end() )
    {
        return SCM_EOL;
    }
    else
    {
        return ScmFromString ( ( *found ).second.c_str() );
    }
}

//la torna come una assoc list ordinata
SCM mtfa_eis_get_current_query ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );

    SCM alist = SCM_EOL;
    for (auto it: bufs_in_out->current_query)
    {
        alist = scm_cons(scm_cons(ScmFromString(it.first.c_str()), ScmFromString(it.second.c_str())), alist);
    }
    return scm_reverse(alist);
}

static SCM mtfa_eis_get_current_headers ( SCM scm_bufs_in_out);
SCM mtfa_eis_get_current_headers ( SCM scm_bufs_in_out)
{
	T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
	return ScmFromString ( bufs_in_out->header_cs.c_str() );
}

static SCM mtfa_eis_get_current_uri ( SCM scm_bufs_in_out);
SCM mtfa_eis_get_current_uri ( SCM scm_bufs_in_out)
{
	T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
	return ScmFromString ( bufs_in_out->s_loc_uri.c_str());
}

static SCM mtfa_eis_get_value_current_headers ( SCM scm_bufs_in_out, SCM s_key )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    char* key;
    size_t len;
    key = ( char* ) ScmToString ( s_key );
    auto found = bufs_in_out->current_headers.find ( key );
    free ( key );

    if ( found == bufs_in_out->current_headers.end() )
    {
        return SCM_EOL;
    }
    else
    {
        return ScmFromString ( ( *found ).second.c_str() );
    }
}

static SCM mtfa_eis_get_value_current_cookies ( SCM scm_bufs_in_out, SCM s_key )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    char* key;
    size_t len;
    key = ( char* ) ScmToString ( s_key );
    auto found = bufs_in_out->current_cookies.find ( key );
    free ( key );

    if ( found == bufs_in_out->current_cookies.end() )
    {
        return SCM_EOL;
    }
    else
    {
        return ScmFromString ( ( *found ).second.c_str() );
    }
}

static SCM mtfa_eis_enable_pbuf ( SCM scm_bufs_in_out, SCM scm_enable_in_for_reading, SCM scm_enable_in_for_writing) {
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    
    //Ora posso abilitare e disabilitare!
    if (bufs_in_out && bufs_in_out->p_buf_in && !bufs_in_out->buf_in_freed && !bufs_in_out->closed && scm_is_bool(scm_enable_in_for_reading)) {
        //std::cout << "Enable/Disable READ" << endl;
        auto enabled = bufferevent_get_enabled(bufs_in_out->p_buf_in);
        if (scm_is_bool(scm_enable_in_for_reading)) {
            bool enable_in = scm_to_bool(scm_enable_in_for_reading);
            if (enable_in) {
                bufferevent_enable( bufs_in_out->p_buf_in, enabled | EV_READ);
            } else {
                bufferevent_disable( bufs_in_out->p_buf_in, enabled ^ EV_READ);
            }
        }
    }
    
    if (bufs_in_out && bufs_in_out->p_buf_in && !bufs_in_out->buf_in_freed && !bufs_in_out->closed && scm_is_bool(scm_enable_in_for_writing)) {
        //std::cout << "Enable/Disable WRITE" << endl;
        auto enabled = bufferevent_get_enabled(bufs_in_out->p_buf_in);
        if (scm_is_bool(scm_enable_in_for_writing)) {
            bool enable_out = scm_to_bool(scm_enable_in_for_writing);
            if (enable_out)
                bufferevent_enable( bufs_in_out->p_buf_in, enabled | EV_WRITE ); // | EV_TIMEOUT);
            else
                bufferevent_disable( bufs_in_out->p_buf_in, enabled ^ EV_WRITE ); // | EV_TIMEOUT);
        }
    }
    return SCM_BOOL_T;
}

static SCM mtfa_eis_get_raw_data ( SCM scm_bufs_in_out ) {
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );

    struct evbuffer * src;
    int srcLen;
    unsigned char* orig;
    char* p;
    SCM scm_ret;

    if (!bufs_in_out || !bufs_in_out->p_buf_in) {
        goto ko;
    }
    src = bufferevent_get_input ( bufs_in_out->p_buf_in );
    if (!src) {
        goto ko;
    }
    srcLen = evbuffer_get_length ( src );
    if (!srcLen) {
        goto ko;
    }
    orig = evbuffer_pullup ( src, srcLen );
    if (!orig) {
        goto ko;
    }

    p = ( char* ) malloc ( srcLen );
    if (!p) {
        goto ko;
    }
    memcpy ( p, orig, srcLen );
    
    //Or che ho preso i dati, li posso eliminare dal buffer di input
    evbuffer_drain(src, srcLen);

    scm_ret = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) p, free ),
                        scm_from_int ( srcLen ),
                        scm_from_int ( 0 ),
                        scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
ko:
    return SCM_BOOL_F;
}

// static mutex mtfa_eis_put_raw_data_mtx;
static SCM mtfa_eis_put_raw_data ( SCM scm_bufs_in_out, SCM data ) {
    // const std::lock_guard<std::mutex> lock(mtfa_eis_put_raw_data_mtx);
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );

    auto data_len = SCM_BYTEVECTOR_LENGTH ( data );
    auto data_p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( data );
    
    if (bufs_in_out && bufs_in_out->p_buf_in && !bufs_in_out->buf_in_freed && !bufs_in_out->closed) {

        // bufferevent_lock(bufs_in_out->p_buf_in);
        if (bufs_in_out && bufs_in_out->p_buf_in) {
            struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_in );
            if (dst != nullptr) 
                evbuffer_add(dst, data_p, data_len);
        }
        // bufferevent_unlock(bufs_in_out->p_buf_in);
        return SCM_BOOL_T;
    } else
        return SCM_BOOL_F;
}


static SCM mtfa_eis_get_current_body ( SCM scm_bufs_in_out, SCM scm_asString )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    //il body è l'unico che torno in due modi
    bool asString = scm_to_bool ( scm_asString );

    if ( asString )
    {
        return ScmFromString ( bufs_in_out->s_body.c_str() );
    }
    else
    {
        char* p = ( char* ) malloc ( bufs_in_out->s_body.size() );
        memcpy ( p, bufs_in_out->s_body.data(), bufs_in_out->s_body.size() );
        SCM scm_ret = scm_pointer_to_bytevector (
                          scm_from_pointer ( ( void* ) p, free ),
                          scm_from_int ( bufs_in_out->s_body.size() ),
                          scm_from_int ( 0 ),
                          scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
}
static SCM mtfa_eis_get_current_method ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_method.c_str() );
}
static SCM mtfa_eis_get_current_protocol ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_protocol.c_str() );
}
static SCM mtfa_eis_get_current_host ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_host.c_str() );
}
static SCM mtfa_eis_get_current_port ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_port.c_str() );
}
static SCM mtfa_eis_get_current_url ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_url.c_str() );
}
static SCM mtfa_eis_get_current_pars ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_pars.c_str() );
}
static SCM mtfa_eis_get_current_tag ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_tag.c_str() );
}
static SCM mtfa_eis_get_current_user_passwd ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->s_loc_user_passwd.c_str() );
}

static SCM mtfa_eis_get_current_mac_src ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->mac_src.c_str() );
}

static SCM mtfa_eis_get_current_ip_src ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->client_ip.c_str() );
}

static SCM mtfa_eis_get_current_ip_dst ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->listen_ip.c_str() );
}

static SCM mtfa_eis_get_current_port_src ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return scm_from_ushort ( bufs_in_out->client_port);
}

static SCM mtfa_eis_get_current_port_dst ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return scm_from_ushort ( bufs_in_out->listen_port );
}

static SCM mtfa_eis_get_current_multipart ( SCM scm_bufs_in_out );
SCM mtfa_eis_get_current_multipart ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    SCM l = SCM_EOL;

    for ( auto& it : bufs_in_out->lmr )
    {
        char* p = ( char* ) malloc ( it.data.size() );
        memcpy ( p, it.data.data(), it.data.size() );
        SCM scm_data = scm_pointer_to_bytevector (
                           scm_from_pointer ( ( void* ) p, free ),
                           scm_from_int ( it.data.size() ),
                           scm_from_int ( 0 ),
                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        //Un fine riga nel nome (e poi nel valore) indica che ci sono due attributi, o più, per i dati
        T_V_Split names  = mtfa_string_split(it.name, "\n");
        T_V_Split values = mtfa_string_split(it.value, "\n");

        SCM l3 = SCM_EOL;
        for (int i=0; i<max(names.size(), values.size()); ++i)
        {
            string name;
            string value;
            name.clear();
            value.clear();
            
            if (i<names.size())
                name = names[i];
            
            if (i<values.size())
                value = values[i];

            SCM l2 = SCM_EOL;
            l2 = scm_cons(ScmFromString(value.c_str()), l2);
            l2 = scm_cons(ScmFromString(name.c_str()), l2);
            l3 = scm_cons(l2, l3);
        }

        l3 = scm_cons ( scm_data, l3);
        l3 = scm_reverse(l3);
        l  = scm_cons ( l3, l );
    }

    return scm_reverse ( l );
}

static SCM mtfa_eis_get_current_ssl_servername ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString(bufs_in_out->server_name.c_str());
}

static SCM mtfa_eis_get_current_network_protocol ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );

    switch ( bufs_in_out->protocollo )
    {
        case Protocols::eProtHTTP:
            return ScmFromString ( "HTTP" );
            break;

        case Protocols::eProtSSL:
            return ScmFromString ( "HTTPS" );
            break;

        case Protocols::eProtTCP:
            return ScmFromString ( "TCP" );
            break;

        default:
            return SCM_EOL;
    }
}

//per settare il data size reale quando uso gohome con il tcp
static SCM mtfa_eis_set_real_data_size ( SCM scm_bufs_in_out, SCM scm_rds )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    bufs_in_out->real_data_size = scm_to_int ( scm_rds );
    return SCM_BOOL_T;
}

static SCM mtfa_eis_get_tcp_data_len ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return scm_from_int ( bufs_in_out->content_length );
}

static SCM mtfa_eis_get_id_sonda ( SCM scm_bufs_in_out )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) scm_to_pointer ( scm_bufs_in_out );
    return ScmFromString ( bufs_in_out->idsonda.c_str() );
}

//SCM mtfa_eis_is_from_client (SCM scm_bufs_in_out)
//{
//    T_EventoStruct * bufs_in_out = (T_EventoStruct*)scm_to_pointer(scm_bufs_in_out);
////    if (bufs_in_out->from_client)
////        printf("mtfa_eis_is_from_client: true\n");
////    else
////        printf("mtfa_eis_is_from_client: false\n");
//    return scm_from_bool(bufs_in_out->from_client);
//}
//
//SCM mtfa_eis_is_command_channel (SCM scm_bufs_in_out)
//{
//    T_EventoStruct * bufs_in_out = (T_EventoStruct*)scm_to_pointer(scm_bufs_in_out);
//    return scm_from_bool(bufs_in_out->is_a_command_channel);
//}

//SCM mtfa_eis_get_tcp_event (SCM scm_bufs_in_out)
//{
//    T_EventoStruct * bufs_in_out = (T_EventoStruct*)scm_to_pointer(scm_bufs_in_out);
//    string s;
//    switch (bufs_in_out->tcp_current_event)
//    {
//    case TcpEvents::eTcpAccepted:
//        s.assign("ACCEPT");
//        break;
//    case TcpEvents::eTcpConnected:
//        s.assign("CONNECT");
//        break;
//    case TcpEvents::eTcpDataReceived:
//        s.assign("DATAREADY");
//        break;
//    case TcpEvents::eTcpNone:
//        s.assign("NONE");
//        break;
//    }
//    return scm_from_latin1_string(s.c_str());
//}

//Generiche funzioni di sistema operativo
static SCM mtfa_get_current_thread ();
SCM mtfa_get_current_thread ()
{
    pthread_t p = pthread_self();
    SCM ret = scm_from_long_long ( ( unsigned long long ) p );
    return ret;
}


/********************NUMBER THEORY**********************/

// Factoring with Pollard's rho method.


static unsigned char primes_diff[] = {
#define P(a,b,c) a,
P( 1, 0xaaaaaaaaaaaaaaabUL, 0x5555555555555555UL) /* 3 */
P( 2, 0xcccccccccccccccdUL, 0x3333333333333333UL) /* 5 */
P( 2, 0x6db6db6db6db6db7UL, 0x2492492492492492UL) /* 7 */
P( 4, 0x2e8ba2e8ba2e8ba3UL, 0x1745d1745d1745d1UL) /* 11 */
P( 2, 0x4ec4ec4ec4ec4ec5UL, 0x13b13b13b13b13b1UL) /* 13 */
P( 4, 0xf0f0f0f0f0f0f0f1UL, 0x0f0f0f0f0f0f0f0fUL) /* 17 */
P( 2, 0x86bca1af286bca1bUL, 0x0d79435e50d79435UL) /* 19 */
P( 4, 0xd37a6f4de9bd37a7UL, 0x0b21642c8590b216UL) /* 23 */
P( 6, 0x34f72c234f72c235UL, 0x08d3dcb08d3dcb08UL) /* 29 */
P( 2, 0xef7bdef7bdef7bdfUL, 0x0842108421084210UL) /* 31 */
P( 6, 0x14c1bacf914c1badUL, 0x06eb3e45306eb3e4UL) /* 37 */
P( 4, 0x8f9c18f9c18f9c19UL, 0x063e7063e7063e70UL) /* 41 */
P( 2, 0x82fa0be82fa0be83UL, 0x05f417d05f417d05UL) /* 43 */
P( 4, 0x51b3bea3677d46cfUL, 0x0572620ae4c415c9UL) /* 47 */
P( 6, 0x21cfb2b78c13521dUL, 0x04d4873ecade304dUL) /* 53 */
P( 6, 0xcbeea4e1a08ad8f3UL, 0x0456c797dd49c341UL) /* 59 */
P( 2, 0x4fbcda3ac10c9715UL, 0x04325c53ef368eb0UL) /* 61 */
P( 6, 0xf0b7672a07a44c6bUL, 0x03d226357e16ece5UL) /* 67 */
P( 4, 0x193d4bb7e327a977UL, 0x039b0ad12073615aUL) /* 71 */
P( 2, 0x7e3f1f8fc7e3f1f9UL, 0x0381c0e070381c0eUL) /* 73 */
P( 6, 0x9b8b577e613716afUL, 0x033d91d2a2067b23UL) /* 79 */
P( 4, 0xa3784a062b2e43dbUL, 0x03159721ed7e7534UL) /* 83 */
P( 6, 0xf47e8fd1fa3f47e9UL, 0x02e05c0b81702e05UL) /* 89 */
P( 8, 0xa3a0fd5c5f02a3a1UL, 0x02a3a0fd5c5f02a3UL) /* 97 */
P( 4, 0x3a4c0a237c32b16dUL, 0x0288df0cac5b3f5dUL) /* 101 */
P( 2, 0xdab7ec1dd3431b57UL, 0x027c45979c95204fUL) /* 103 */
P( 4, 0x77a04c8f8d28ac43UL, 0x02647c69456217ecUL) /* 107 */
P( 2, 0xa6c0964fda6c0965UL, 0x02593f69b02593f6UL) /* 109 */
P( 4, 0x90fdbc090fdbc091UL, 0x0243f6f0243f6f02UL) /* 113 */
P(14, 0x7efdfbf7efdfbf7fUL, 0x0204081020408102UL) /* 127 */
P( 4, 0x03e88cb3c9484e2bUL, 0x01f44659e4a42715UL) /* 131 */
P( 6, 0xe21a291c077975b9UL, 0x01de5d6e3f8868a4UL) /* 137 */
P( 2, 0x3aef6ca970586723UL, 0x01d77b654b82c339UL) /* 139 */
P(10, 0xdf5b0f768ce2cabdUL, 0x01b7d6c3dda338b2UL) /* 149 */
P( 2, 0x6fe4dfc9bf937f27UL, 0x01b2036406c80d90UL) /* 151 */
P( 6, 0x5b4fe5e92c0685b5UL, 0x01a16d3f97a4b01aUL) /* 157 */
P( 6, 0x1f693a1c451ab30bUL, 0x01920fb49d0e228dUL) /* 163 */
P( 4, 0x8d07aa27db35a717UL, 0x01886e5f0abb0499UL) /* 167 */
P( 6, 0x882383b30d516325UL, 0x017ad2208e0ecc35UL) /* 173 */
P( 6, 0xed6866f8d962ae7bUL, 0x016e1f76b4337c6cUL) /* 179 */
P( 2, 0x3454dca410f8ed9dUL, 0x016a13cd15372904UL) /* 181 */
P(10, 0x1d7ca632ee936f3fUL, 0x01571ed3c506b39aUL) /* 191 */
P( 2, 0x70bf015390948f41UL, 0x015390948f40feacUL) /* 193 */
P( 4, 0xc96bdb9d3d137e0dUL, 0x014cab88725af6e7UL) /* 197 */
P( 2, 0x2697cc8aef46c0f7UL, 0x0149539e3b2d066eUL) /* 199 */
P(12, 0xc0e8f2a76e68575bUL, 0x013698df3de07479UL) /* 211 */
P(12, 0x687763dfdb43bb1fUL, 0x0125e22708092f11UL) /* 223 */
P( 4, 0x1b10ea929ba144cbUL, 0x0120b470c67c0d88UL) /* 227 */
P( 2, 0x1d10c4c0478bbcedUL, 0x011e2ef3b3fb8744UL) /* 229 */
P( 4, 0x63fb9aeb1fdcd759UL, 0x0119453808ca29c0UL) /* 233 */
P( 6, 0x64afaa4f437b2e0fUL, 0x0112358e75d30336UL) /* 239 */
P( 2, 0xf010fef010fef011UL, 0x010fef010fef010fUL) /* 241 */
P(10, 0x28cbfbeb9a020a33UL, 0x0105197f7d734041UL) /* 251 */
P( 6, 0xff00ff00ff00ff01UL, 0x00ff00ff00ff00ffUL) /* 257 */
P( 6, 0xd624fd1470e99cb7UL, 0x00f92fb2211855a8UL) /* 263 */
P( 6, 0x8fb3ddbd6205b5c5UL, 0x00f3a0d52cba8723UL) /* 269 */
P( 2, 0xd57da36ca27acdefUL, 0x00f1d48bcee0d399UL) /* 271 */
P( 6, 0xee70c03b25e4463dUL, 0x00ec979118f3fc4dUL) /* 277 */
P( 4, 0xc5b1a6b80749cb29UL, 0x00e939651fe2d8d3UL) /* 281 */
P( 2, 0x47768073c9b97113UL, 0x00e79372e225fe30UL) /* 283 */
P(10, 0x2591e94884ce32adUL, 0x00dfac1f74346c57UL) /* 293 */
P(14, 0xf02806abc74be1fbUL, 0x00d578e97c3f5fe5UL) /* 307 */
P( 4, 0x7ec3e8f3a7198487UL, 0x00d2ba083b445250UL) /* 311 */
P( 2, 0x58550f8a39409d09UL, 0x00d161543e28e502UL) /* 313 */
P( 4, 0xec9e48ae6f71de15UL, 0x00cebcf8bb5b4169UL) /* 317 */
P(14, 0x2ff3a018bfce8063UL, 0x00c5fe740317f9d0UL) /* 331 */
P( 6, 0x7f9ec3fcf61fe7b1UL, 0x00c2780613c0309eUL) /* 337 */
P(10, 0x89f5abe570e046d3UL, 0x00bcdd535db1cc5bUL) /* 347 */
P( 2, 0xda971b23f1545af5UL, 0x00bbc8408cd63069UL) /* 349 */
P( 4, 0x79d5f00b9a7862a1UL, 0x00b9a7862a0ff465UL) /* 353 */
P( 6, 0x4dba1df32a128a57UL, 0x00b68d31340e4307UL) /* 359 */
P( 8, 0x87530217b7747d8fUL, 0x00b2927c29da5519UL) /* 367 */
P( 6, 0x30baae53bb5e06ddUL, 0x00afb321a1496fdfUL) /* 373 */
P( 6, 0xee70206c12e9b5b3UL, 0x00aceb0f891e6551UL) /* 379 */
P( 4, 0xcdde9462ec9dbe7fUL, 0x00ab1cbdd3e2970fUL) /* 383 */
P( 6, 0xafb64b05ec41cf4dUL, 0x00a87917088e262bUL) /* 389 */
P( 8, 0x02944ff5aec02945UL, 0x00a513fd6bb00a51UL) /* 397 */
P( 4, 0x2cb033128382df71UL, 0x00a36e71a2cb0331UL) /* 401 */
P( 8, 0x1ccacc0c84b1c2a9UL, 0x00a03c1688732b30UL) /* 409 */
P(10, 0x19a93db575eb3a0bUL, 0x009c69169b30446dUL) /* 419 */
P( 2, 0xcebeef94fa86fe2dUL, 0x009baade8e4a2f6eUL) /* 421 */
P(10, 0x6faa77fb3f8df54fUL, 0x00980e4156201301UL) /* 431 */
P( 2, 0x68a58af00975a751UL, 0x00975a750ff68a58UL) /* 433 */
P( 6, 0xd56e36d0c3efac07UL, 0x009548e4979e0829UL) /* 439 */
P( 4, 0xd8b44c47a8299b73UL, 0x0093efd1c50e726bUL) /* 443 */
P( 6, 0x02d9ccaf9ba70e41UL, 0x0091f5bcb8bb02d9UL) /* 449 */
P( 8, 0x0985e1c023d9e879UL, 0x008f67a1e3fdc261UL) /* 457 */
P( 4, 0x2a343316c494d305UL, 0x008e2917e0e702c6UL) /* 461 */
P( 2, 0x70cb7916ab67652fUL, 0x008d8be33f95d715UL) /* 463 */
P( 4, 0xd398f132fb10fe5bUL, 0x008c55841c815ed5UL) /* 467 */
P(12, 0x6f2a38a6bf54fa1fUL, 0x0088d180cd3a4133UL) /* 479 */
P( 8, 0x211df689b98f81d7UL, 0x00869222b1acf1ceUL) /* 487 */
P( 4, 0x0e994983e90f1ec3UL, 0x0085797b917765abUL) /* 491 */
P( 8, 0xad671e44bed87f3bUL, 0x008355ace3c897dbUL) /* 499 */
P( 4, 0xf9623a0516e70fc7UL, 0x00824a4e60b3262bUL) /* 503 */
P( 6, 0x4b7129be9dece355UL, 0x0080c121b28bd1baUL) /* 509 */
P(12, 0x190f3b7473f62c39UL, 0x007dc9f3397d4c29UL) /* 521 */
P( 2, 0x63dacc9aad46f9a3UL, 0x007d4ece8fe88139UL) /* 523 */
P(18, 0xc1108fda24e8d035UL, 0x0079237d65bcce50UL) /* 541 */
P( 6, 0xb77578472319bd8bUL, 0x0077cf53c5f7936cUL) /* 547 */
P(10, 0x473d20a1c7ed9da5UL, 0x0075a8accfbdd11eUL) /* 557 */
P( 6, 0xfbe85af0fea2c8fbUL, 0x007467ac557c228eUL) /* 563 */
P( 6, 0x58a1f7e6ce0f4c09UL, 0x00732d70ed8db8e9UL) /* 569 */
P( 2, 0x1a00e58c544986f3UL, 0x0072c62a24c3797fUL) /* 571 */
P( 6, 0x7194a17f55a10dc1UL, 0x007194a17f55a10dUL) /* 577 */
P(10, 0x7084944785e33763UL, 0x006fa549b41da7e7UL) /* 587 */
P( 6, 0xba10679bd84886b1UL, 0x006e8419e6f61221UL) /* 593 */
P( 6, 0xebe9c6bb31260967UL, 0x006d68b5356c207bUL) /* 599 */
P( 2, 0x97a3fe4bd1ff25e9UL, 0x006d0b803685c01bUL) /* 601 */
P( 6, 0x6c6388395b84d99fUL, 0x006bf790a8b2d207UL) /* 607 */
P( 6, 0x8c51da6a1335df6dUL, 0x006ae907ef4b96c2UL) /* 613 */
P( 4, 0x46f3234475d5add9UL, 0x006a37991a23aeadUL) /* 617 */
P( 2, 0x905605ca3c619a43UL, 0x0069dfbdd4295b66UL) /* 619 */
P(12, 0xcee8dff304767747UL, 0x0067dc4c45c8033eUL) /* 631 */
P(10, 0xff99c27f00663d81UL, 0x00663d80ff99c27fUL) /* 641 */
P( 2, 0xacca407f671ddc2bUL, 0x0065ec17e3559948UL) /* 643 */
P( 4, 0xe71298bac1e12337UL, 0x00654ac835cfba5cUL) /* 647 */
P( 6, 0xfa1e94309cd09045UL, 0x00645c854ae10772UL) /* 653 */
P( 6, 0xbebccb8e91496b9bUL, 0x006372990e5f901fUL) /* 659 */
P( 2, 0x312fa30cc7d7b8bdUL, 0x006325913c07beefUL) /* 661 */
P(12, 0x6160ff9e9f006161UL, 0x006160ff9e9f0061UL) /* 673 */
P( 4, 0x6b03673b5e28152dUL, 0x0060cdb520e5e88eUL) /* 677 */
P( 6, 0xfe802ffa00bfe803UL, 0x005ff4017fd005ffUL) /* 683 */
P( 8, 0xe66fe25c9e907c7bUL, 0x005ed79e31a4dccdUL) /* 691 */
P(10, 0x3f8b236c76528895UL, 0x005d7d42d48ac5efUL) /* 701 */
P( 8, 0xf6f923bf01ce2c0dUL, 0x005c6f35ccba5028UL) /* 709 */
P(10, 0x6c3d3d98bed7c42fUL, 0x005b2618ec6ad0a5UL) /* 719 */
P( 8, 0x30981efcd4b010e7UL, 0x005a2553748e42e7UL) /* 727 */
P( 6, 0x6f691fc81ebbe575UL, 0x0059686cf744cd5bUL) /* 733 */
P( 6, 0xb10480ddb47b52cbUL, 0x0058ae97bab79976UL) /* 739 */
P( 4, 0x74cd59ed64f3f0d7UL, 0x0058345f1876865fUL) /* 743 */
P( 8, 0x0105cb81316d6c0fUL, 0x005743d5bb24795aUL) /* 751 */
P( 6, 0x9be64c6d91c1195dUL, 0x005692c4d1ab74abUL) /* 757 */
P( 4, 0x71b3f945a27b1f49UL, 0x00561e46a4d5f337UL) /* 761 */
P( 8, 0x77d80d50e508fd01UL, 0x005538ed06533997UL) /* 769 */
P( 4, 0xa5eb778e133551cdUL, 0x0054c807f2c0bec2UL) /* 773 */
P(14, 0x18657d3c2d8a3f1bUL, 0x005345efbc572d36UL) /* 787 */
P(10, 0x2e40e220c34ad735UL, 0x00523a758f941345UL) /* 797 */
P(12, 0xa76593c70a714919UL, 0x005102370f816c89UL) /* 809 */
P( 2, 0x1eef452124eea383UL, 0x0050cf129fb94acfUL) /* 811 */
P(10, 0x38206dc242ba771dUL, 0x004fd31941cafdd1UL) /* 821 */
P( 2, 0x4cd4c35807772287UL, 0x004fa1704aa75945UL) /* 823 */
P( 4, 0x83de917d5e69ddf3UL, 0x004f3ed6d45a63adUL) /* 827 */
P( 2, 0x882ef0403b4a6c15UL, 0x004f0de57154ebedUL) /* 829 */
P(10, 0xf8fb6c51c606b677UL, 0x004e1cae8815f811UL) /* 839 */
P(14, 0xb4abaac446d3e1fdUL, 0x004cd47ba5f6ff19UL) /* 853 */
P( 4, 0xa9f83bbe484a14e9UL, 0x004c78ae734df709UL) /* 857 */
P( 2, 0x0bebbc0d1ce874d3UL, 0x004c4b19ed85cfb8UL) /* 859 */
P( 4, 0xbd418eaf0473189fUL, 0x004bf093221d1218UL) /* 863 */
P(14, 0x44e3af6f372b7e65UL, 0x004aba3c21dc633fUL) /* 877 */
P( 4, 0xc87fdace4f9e5d91UL, 0x004a6360c344de00UL) /* 881 */
P( 2, 0xec93479c446bd9bbUL, 0x004a383e9f74d68aUL) /* 883 */
P( 4, 0xdac4d592e777c647UL, 0x0049e28fbabb9940UL) /* 887 */
P(20, 0xa63ea8c8f61f0c23UL, 0x0048417b57c78cd7UL) /* 907 */
P( 4, 0xe476062ea5cbbb6fUL, 0x0047f043713f3a2bUL) /* 911 */
P( 8, 0xdf68761c69daac27UL, 0x00474ff2a10281cfUL) /* 919 */
P(10, 0xb813d737637aa061UL, 0x00468b6f9a978f91UL) /* 929 */
P( 8, 0xa3a77aac1fb15099UL, 0x0045f13f1caff2e2UL) /* 937 */
P( 4, 0x17f0c3e0712c5825UL, 0x0045a5228cec23e9UL) /* 941 */
P( 6, 0xfd912a70ff30637bUL, 0x0045342c556c66b9UL) /* 947 */
P( 6, 0xfbb3b5dc01131289UL, 0x0044c4a23feeced7UL) /* 953 */
P(14, 0x856d560a0f5acdf7UL, 0x0043c5c20d3c9fe6UL) /* 967 */
P( 4, 0x96472f314d3f89e3UL, 0x00437e494b239798UL) /* 971 */
P( 6, 0xa76f5c7ed2253531UL, 0x0043142d118e47cbUL) /* 977 */
P( 6, 0x816eae7c7bf69fe7UL, 0x0042ab5c73a13458UL) /* 983 */
P( 8, 0xb6a2bea4cfb1781fUL, 0x004221950db0f3dbUL) /* 991 */
P( 6, 0xa3900c53318e81edUL, 0x0041bbb2f80a4553UL) /* 997 */
P(12, 0x60aa7f5d9f148d11UL, 0x0040f391612c6680UL) /* 1009 */
P( 4, 0x6be8c0102c7a505dUL, 0x0040b1e94173fefdUL) /* 1013 */
P( 6, 0x8ff3f0ed28728f33UL, 0x004050647d9d0445UL) /* 1019 */
P( 2, 0x680e0a87e5ec7155UL, 0x004030241b144f3bUL) /* 1021 */
P(10, 0xbbf70fa49fe829b7UL, 0x003f90c2ab542cb1UL) /* 1031 */
P( 2, 0xd69d1e7b6a50ca39UL, 0x003f71412d59f597UL) /* 1033 */
P( 6, 0x1a1e0f46b6d26aefUL, 0x003f137701b98841UL) /* 1039 */
P(10, 0x7429f9a7a8251829UL, 0x003e79886b60e278UL) /* 1049 */
P( 2, 0xd9c2219d1b863613UL, 0x003e5b1916a7181dUL) /* 1051 */
P(10, 0x91406c1820d077adUL, 0x003dc4a50968f524UL) /* 1061 */
P( 2, 0x521f4ec02e3d2b97UL, 0x003da6e4c9550321UL) /* 1063 */
P( 6, 0xbb8283b63dc8eba5UL, 0x003d4e4f06f1def3UL) /* 1069 */
P(18, 0x431eda153229ebbfUL, 0x003c4a6bdd24f9a4UL) /* 1087 */
P( 4, 0xaf0bf78d7e01686bUL, 0x003c11d54b525c73UL) /* 1091 */
P( 2, 0xa9ced0742c086e8dUL, 0x003bf5b1c5721065UL) /* 1093 */
P( 4, 0xc26458ad9f632df9UL, 0x003bbdb9862f23b4UL) /* 1097 */
P( 6, 0xbbff1255dff892afUL, 0x003b6a8801db5440UL) /* 1103 */
P( 6, 0xcbd49a333f04d8fdUL, 0x003b183cf0fed886UL) /* 1109 */
P( 8, 0xec84ed6f9cfdeff5UL, 0x003aabe394bdc3f4UL) /* 1117 */
P( 6, 0x97980cc40bda9d4bUL, 0x003a5ba3e76156daUL) /* 1123 */
P( 6, 0x777f34d524f5cbd9UL, 0x003a0c3e953378dbUL) /* 1129 */
P(22, 0x2797051d94cbbb7fUL, 0x0038f03561320b1eUL) /* 1151 */
P( 2, 0xea769051b4f43b81UL, 0x0038d6ecaef5908aUL) /* 1153 */
P(10, 0xce7910f3034d4323UL, 0x003859cf221e6069UL) /* 1163 */
P( 8, 0x92791d1374f5b99bUL, 0x0037f7415dc9588aUL) /* 1171 */
P(10, 0x89a5645cc68ea1b5UL, 0x00377df0d3902626UL) /* 1181 */
P( 6, 0x5f8aacf796c0cf0bUL, 0x00373622136907faUL) /* 1187 */
P( 6, 0xf2e90a15e33edf99UL, 0x0036ef0c3b39b92fUL) /* 1193 */
P( 8, 0x8e99e5feb897c451UL, 0x0036915f47d55e6dUL) /* 1201 */
P(12, 0xaca2eda38fb91695UL, 0x0036072cf3f866fdUL) /* 1213 */
P( 4, 0x5d9b737be5ea8b41UL, 0x0035d9b737be5ea8UL) /* 1217 */
P( 6, 0x4aefe1db93fd7cf7UL, 0x0035961559cc81c7UL) /* 1223 */
P( 6, 0xa0994ef20b3f8805UL, 0x0035531c897a4592UL) /* 1229 */
P( 2, 0x103890bda912822fUL, 0x00353ceebd3e98a4UL) /* 1231 */
P( 6, 0xb441659d13a9147dUL, 0x0034fad381585e5eUL) /* 1237 */
P(12, 0x1e2134440c4c3f21UL, 0x00347884d1103130UL) /* 1249 */
P(10, 0x263a27727a6883c3UL, 0x00340dd3ac39bf56UL) /* 1259 */
P(18, 0x78e221472ab33855UL, 0x003351fdfecc140cUL) /* 1277 */
P( 2, 0x95eac88e82e6faffUL, 0x00333d72b089b524UL) /* 1279 */
P( 4, 0xf66c258317be8dabUL, 0x0033148d44d6b261UL) /* 1283 */
P( 6, 0x09ee202c7cb91939UL, 0x0032d7aef8412458UL) /* 1289 */
P( 2, 0x8d2fca1042a09ea3UL, 0x0032c3850e79c0f1UL) /* 1291 */
P( 6, 0x82779c856d8b8bf1UL, 0x00328766d59048a2UL) /* 1297 */
P( 4, 0x3879361cba8a223dUL, 0x00325fa18cb11833UL) /* 1301 */
P( 2, 0xf23f43639c3182a7UL, 0x00324bd659327e22UL) /* 1303 */
P( 4, 0xa03868fc474bcd13UL, 0x0032246e784360f4UL) /* 1307 */
P(12, 0x651e78b8c5311a97UL, 0x0031afa5f1a33a08UL) /* 1319 */
P( 2, 0x8ffce639c00c6719UL, 0x00319c63ff398e70UL) /* 1321 */
P( 6, 0xf7b460754b0b61cfUL, 0x003162f7519a86a7UL) /* 1327 */
P(34, 0x7b03f3359b8e63b1UL, 0x0030271fc9d3fc3cUL) /* 1361 */
P( 6, 0xa55c5326041eb667UL, 0x002ff104ae89750bUL) /* 1367 */
P( 6, 0x647f88ab896a76f5UL, 0x002fbb62a236d133UL) /* 1373 */
P( 8, 0x8fd971434a55a46dUL, 0x002f74997d2070b4UL) /* 1381 */
P(18, 0x9fbf969958046447UL, 0x002ed84aa8b6fce3UL) /* 1399 */
P(10, 0x9986feba69be3a81UL, 0x002e832df7a46dbdUL) /* 1409 */
P(14, 0xa668b3e6d053796fUL, 0x002e0e0846857cabUL) /* 1423 */
P( 4, 0x97694e6589f4e09bUL, 0x002decfbdfb55ee6UL) /* 1427 */
P( 2, 0x37890c00b7721dbdUL, 0x002ddc876f3ff488UL) /* 1429 */
P( 4, 0x5ac094a235f37ea9UL, 0x002dbbc1d4c482c4UL) /* 1433 */
P( 6, 0x31cff775f2d5d65fUL, 0x002d8af0e0de0556UL) /* 1439 */
P( 8, 0xddad8e6b36505217UL, 0x002d4a7b7d14b30aUL) /* 1447 */
P( 4, 0x5a27df897062cd03UL, 0x002d2a85073bcf4eUL) /* 1451 */
P( 2, 0xe2396fe0fdb5a625UL, 0x002d1a9ab13e8be4UL) /* 1453 */
P( 6, 0xb352a4957e82317bUL, 0x002ceb1eb4b9fd8bUL) /* 1459 */
P(12, 0xd8ab3f2c60c2ea3fUL, 0x002c8d503a79794cUL) /* 1471 */
P(10, 0x6893f702f0452479UL, 0x002c404d708784edUL) /* 1481 */
P( 2, 0x9686fdc182acf7e3UL, 0x002c31066315ec52UL) /* 1483 */
P( 4, 0x6854037173dce12fUL, 0x002c1297d80f2664UL) /* 1487 */
P( 2, 0x7f0ded1685c27331UL, 0x002c037044c55f6bUL) /* 1489 */
P( 4, 0xeeda72e1fe490b7dUL, 0x002be5404cd13086UL) /* 1493 */
P( 6, 0x9e7bfc959a8e6e53UL, 0x002bb845adaf0cceUL) /* 1499 */
P(12, 0x49b314d6d4753dd7UL, 0x002b5f62c639f16dUL) /* 1511 */
P(12, 0x2e8f8c5ac4aa1b3bUL, 0x002b07e6734f2b88UL) /* 1523 */
P( 8, 0xb8ef723481163d33UL, 0x002ace569d8342b7UL) /* 1531 */
P(12, 0x6a2ec96a594287b7UL, 0x002a791d5dbd4dcfUL) /* 1543 */
P( 6, 0xdba41c6d13aab8c5UL, 0x002a4eff8113017cUL) /* 1549 */
P( 4, 0xc2adbe648dc3aaf1UL, 0x002a3319e156df32UL) /* 1553 */
P( 6, 0x87a2bade565f91a7UL, 0x002a0986286526eaUL) /* 1559 */
P( 8, 0x4d6fe8798c01f5dfUL, 0x0029d29551d91e39UL) /* 1567 */
P( 4, 0x3791310c8c23d98bUL, 0x0029b7529e109f0aUL) /* 1571 */
P( 8, 0xf80e446b01228883UL, 0x00298137491ea465UL) /* 1579 */
P( 4, 0x9aed1436fbf500cfUL, 0x0029665e1eb9f9daUL) /* 1583 */
P(14, 0x7839b54cc8b24115UL, 0x002909752e019a5eUL) /* 1597 */
P( 4, 0xc128c646ad0309c1UL, 0x0028ef35e2e5efb0UL) /* 1601 */
P( 6, 0x14de631624a3c377UL, 0x0028c815aa4b8278UL) /* 1607 */
P( 2, 0x3f7b9fe68b0ecbf9UL, 0x0028bb1b867199daUL) /* 1609 */
P( 4, 0x284ffd75ec00a285UL, 0x0028a13ff5d7b002UL) /* 1613 */
P( 6, 0x37803cb80dea2ddbUL, 0x00287ab3f173e755UL) /* 1619 */
P( 2, 0x86b63f7c9ac4c6fdUL, 0x00286dead67713bdUL) /* 1621 */
P( 6, 0x8b6851d1bd99b9d3UL, 0x002847bfcda6503eUL) /* 1627 */
P(10, 0xb62fda77ca343b6dUL, 0x002808c1ea6b4777UL) /* 1637 */
P(20, 0x1f0dc009e34383c9UL, 0x00278d0e0f23ff61UL) /* 1657 */
P( 6, 0x496dc21ddd35b97fUL, 0x002768863c093c7fUL) /* 1663 */
P( 4, 0xb0e96ce17090f82bUL, 0x0027505115a73ca8UL) /* 1667 */
P( 2, 0xaadf05acdd7d024dUL, 0x00274441a61dc1b9UL) /* 1669 */
P(24, 0xcb138196746eafb5UL, 0x0026b5c166113cf0UL) /* 1693 */
P( 4, 0x347f523736755d61UL, 0x00269e65ad07b18eUL) /* 1697 */
P( 2, 0xd14a48a051f7dd0bUL, 0x002692c25f877560UL) /* 1699 */
P(10, 0x474d71b1ce914d25UL, 0x002658fa7523cd11UL) /* 1709 */
P(12, 0x386063f5e28c1f89UL, 0x0026148710cf0f9eUL) /* 1721 */
P( 2, 0x1db7325e32d04e73UL, 0x002609363b22524fUL) /* 1723 */
P(10, 0xfef748d3893b880dUL, 0x0025d1065a1c1122UL) /* 1733 */
P( 8, 0x2f3351506e935605UL, 0x0025a48a382b863fUL) /* 1741 */
P( 6, 0x7a3637fa2376415bUL, 0x0025837190eccdbcUL) /* 1747 */
P( 6, 0x4ac525d2baa21969UL, 0x00256292e95d510cUL) /* 1753 */
P( 6, 0x3a11c16b42cd351fUL, 0x002541eda98d068cUL) /* 1759 */
P(18, 0x6c7abde0049c2a11UL, 0x0024e15087fed8f5UL) /* 1777 */
P( 6, 0x54dad0303e069ac7UL, 0x0024c18b20979e5dUL) /* 1783 */
P( 4, 0xebf1ac9fdfe91433UL, 0x0024ac7b336de0c5UL) /* 1787 */
P( 2, 0xfafdda8237cec655UL, 0x0024a1fc478c60bbUL) /* 1789 */
P(12, 0xdce3ff6e71ffb739UL, 0x002463801231c009UL) /* 1801 */
P(10, 0xbed5737d6286db1bUL, 0x0024300fd506ed33UL) /* 1811 */
P(12, 0xe479e431fe08b4dfUL, 0x0023f314a494da81UL) /* 1823 */
P( 8, 0x9dd9b0dd7742f897UL, 0x0023cadedd2fad3aUL) /* 1831 */
P(16, 0x8f09d7402c5a5e87UL, 0x00237b7ed2664a03UL) /* 1847 */
P(14, 0x9216d5c4d958738dUL, 0x0023372967dbaf1dUL) /* 1861 */
P( 6, 0xb3139ba11d34ca63UL, 0x00231a308a371f20UL) /* 1867 */
P( 4, 0x47d54f7ed644afafUL, 0x002306fa63e1e600UL) /* 1871 */
P( 2, 0x92a81d85cf11a1b1UL, 0x0022fd6731575684UL) /* 1873 */
P( 4, 0x754b26533253bdfdUL, 0x0022ea507805749cUL) /* 1877 */
P( 2, 0xbbe0efc980bfd467UL, 0x0022e0cce8b3d720UL) /* 1879 */
P(10, 0xc0d8d594f024dca1UL, 0x0022b1887857d161UL) /* 1889 */
P(12, 0x8238d43bcaac1a65UL, 0x00227977fcc49cc0UL) /* 1901 */
P( 6, 0x27779c1fae6175bbUL, 0x00225db37b5e5f4fUL) /* 1907 */
P( 6, 0xa746ca9af708b2c9UL, 0x0022421b91322ed6UL) /* 1913 */
P(18, 0x93f3cd9f389be823UL, 0x0021f05b35f52102UL) /* 1931 */
P( 2, 0x5cb4a4c04c489345UL, 0x0021e75de5c70d60UL) /* 1933 */
P(16, 0xbf6047743e85b6b5UL, 0x0021a01d6c19be96UL) /* 1949 */
P( 2, 0x61c147831563545fUL, 0x0021974a6615c81aUL) /* 1951 */
P(22, 0xedb47c0ae62dee9dUL, 0x00213767697cf36aUL) /* 1973 */
P( 6, 0x0a3824386673a573UL, 0x00211d9f7fad35f1UL) /* 1979 */
P( 8, 0xa4a77d19e575a0ebUL, 0x0020fb7d9dd36c18UL) /* 1987 */
P( 6, 0xa2bee045e066c279UL, 0x0020e2123d661e0eUL) /* 1993 */
P( 4, 0xc23618de8ab43d05UL, 0x0020d135b66ae990UL) /* 1997 */
P( 2, 0x266b515216cb9f2fUL, 0x0020c8cded4d7a8eUL) /* 1999 */
P( 4, 0xe279edd9e9c2e85bUL, 0x0020b80b3f43ddbfUL) /* 2003 */
P( 8, 0xd0c591c221dc9c53UL, 0x002096b9180f46a6UL) /* 2011 */
P( 6, 0x06da8ee9c9ee7c21UL, 0x00207de7e28de5daUL) /* 2017 */
P(10, 0x9dfebcaf4c27e8c3UL, 0x002054dec8cf1fb3UL) /* 2027 */
P( 2, 0x49aeff9f19dd6de5UL, 0x00204cb630b3aab5UL) /* 2029 */
P(10, 0x86976a57a296e9c7UL, 0x00202428adc37bebUL) /* 2039 */
P(14, 0xa3b9abf4872b84cdUL, 0x001fec0c7834def4UL) /* 2053 */
P(10, 0x34fca6483895e6efUL, 0x001fc46fae98a1d0UL) /* 2063 */
P( 6, 0x34b5a333988f873dUL, 0x001facda430ff619UL) /* 2069 */
P(12, 0xd9dd4f19b5f17be1UL, 0x001f7e17dd8e15e5UL) /* 2081 */
P( 2, 0xb935b507fd0ce78bUL, 0x001f765a3556a4eeUL) /* 2083 */
P( 4, 0xb450f5540660e797UL, 0x001f66ea49d802f1UL) /* 2087 */
P( 2, 0x63ff82831ffc1419UL, 0x001f5f3800faf9c0UL) /* 2089 */
P(10, 0x8992f718c22a32fbUL, 0x001f38f4e6c0f1f9UL) /* 2099 */
P(12, 0x5f3253ad0d37e7bfUL, 0x001f0b8546752578UL) /* 2111 */
P( 2, 0x007c0ffe0fc007c1UL, 0x001f03ff83f001f0UL) /* 2113 */
P(16, 0x4d8ebadc0c0640b1UL, 0x001ec853b0a3883cUL) /* 2129 */
P( 2, 0xe2729af831037bdbUL, 0x001ec0ee573723ebUL) /* 2131 */
P( 6, 0xb8f64bf30feebfe9UL, 0x001eaad38e6f6894UL) /* 2137 */
P( 4, 0xda93124b544c0bf5UL, 0x001e9c28a765fe53UL) /* 2141 */
P( 2, 0x9cf7ff0b593c539fUL, 0x001e94d8758c2003UL) /* 2143 */
P(10, 0xd6bd8861fa0e07d9UL, 0x001e707ba8f65e68UL) /* 2153 */
P( 8, 0x5cfe75c0bd8ab891UL, 0x001e53a2a68f574eUL) /* 2161 */
P(18, 0x43e808757c2e862bUL, 0x001e1380a56b438dUL) /* 2179 */
P(24, 0x90caa96d595c9d93UL, 0x001dbf9f513a3802UL) /* 2203 */
P( 4, 0x8fd550625d07135fUL, 0x001db1d1d58bc600UL) /* 2207 */
P( 6, 0x76b010a86e209f2dUL, 0x001d9d358f53de38UL) /* 2213 */
P( 8, 0xecc0426447769b25UL, 0x001d81e6df6165c7UL) /* 2221 */
P(16, 0xe381339caabe3295UL, 0x001d4bdf7fd40e30UL) /* 2237 */
P( 2, 0xd1b190a2d0c7673fUL, 0x001d452c7a1c958dUL) /* 2239 */
P( 4, 0xc3bce3cf26b0e7ebUL, 0x001d37cf9b902659UL) /* 2243 */
P( 8, 0x5f87e76f56c61ce3UL, 0x001d1d3a5791e97bUL) /* 2251 */
P(16, 0xc06c6857a124b353UL, 0x001ce89fe6b47416UL) /* 2267 */
P( 2, 0x38c040fcba630f75UL, 0x001ce219f3235071UL) /* 2269 */
P( 4, 0xd078bc4fbd533b21UL, 0x001cd516dcf92139UL) /* 2273 */
P( 8, 0xde8e15c5dd354f59UL, 0x001cbb33bd1c2b8bUL) /* 2281 */
P( 6, 0xca61d53d7414260fUL, 0x001ca7e7d2546688UL) /* 2287 */
P( 6, 0xb56bf5ba8eae635dUL, 0x001c94b5c1b3dbd3UL) /* 2293 */
P( 4, 0x44a72cb0fb6e3949UL, 0x001c87f7f9c241c1UL) /* 2297 */
P(12, 0x879839a714f45bcdUL, 0x001c6202706c35a9UL) /* 2309 */
P( 2, 0x02a8994fde5314b7UL, 0x001c5bb8a9437632UL) /* 2311 */
P(22, 0xb971920cf2b90135UL, 0x001c174343b4111eUL) /* 2333 */
P( 6, 0x8a8fd0b7df9a6e8bUL, 0x001c04d0d3e46b42UL) /* 2339 */
P( 2, 0xb31f9a84c1c6eaadUL, 0x001bfeb00fbf4308UL) /* 2341 */
P( 6, 0x92293b02823c6d83UL, 0x001bec5dce0b202dUL) /* 2347 */
P( 4, 0xeee77ff20fe5ddcfUL, 0x001be03444620037UL) /* 2351 */
P( 6, 0x0e1ea0f6c496c11dUL, 0x001bce09c66f6fc3UL) /* 2357 */
P(14, 0xfdf2d3d6f88ccb6bUL, 0x001ba40228d02b30UL) /* 2371 */
P( 6, 0xfa9d74a3457738f9UL, 0x001b9225b1cf8919UL) /* 2377 */
P( 4, 0xefc3ca3db71a5785UL, 0x001b864a2ff3f53fUL) /* 2381 */
P( 2, 0x8e2071718d0d6dafUL, 0x001b80604150e49bUL) /* 2383 */
P( 6, 0xbc0fdbfeb6cfabfdUL, 0x001b6eb1aaeaacf3UL) /* 2389 */
P( 4, 0x1eeab613e5e5aee9UL, 0x001b62f48da3c8ccUL) /* 2393 */
P( 6, 0x2d2388e90e9e929fUL, 0x001b516babe96092UL) /* 2399 */
P(12, 0x81dbafba588ddb43UL, 0x001b2e9cef1e0c87UL) /* 2411 */
P( 6, 0x52eebc51c4799791UL, 0x001b1d56bedc849bUL) /* 2417 */
P( 6, 0x1c6bc4693b45a047UL, 0x001b0c267546aec0UL) /* 2423 */
P(14, 0x06eee0974498874dUL, 0x001ae45f62024fa0UL) /* 2437 */
P( 4, 0xd85b7377a9953cb9UL, 0x001ad917631b5f54UL) /* 2441 */
P( 6, 0x4b6df412d4caf56fUL, 0x001ac83d18cb608fUL) /* 2447 */
P(12, 0x6b8afbbb4a053493UL, 0x001aa6c7ad8c063fUL) /* 2459 */
P( 8, 0xcc5299c96ac7720bUL, 0x001a90a7b1228e2aUL) /* 2467 */
P( 6, 0xadce84b5c710aa99UL, 0x001a8027c03ba059UL) /* 2473 */
P( 4, 0x9d673f5aa3804225UL, 0x001a7533289deb89UL) /* 2477 */
P(26, 0xe6541268efbce7f7UL, 0x001a2ed7ce16b49fUL) /* 2503 */
P(18, 0xfcf41e76cf5be669UL, 0x0019fefc0a279a73UL) /* 2521 */
P(10, 0x5c3eb5dc31c383cbUL, 0x0019e4b0cd873b5fUL) /* 2531 */
P( 8, 0x301832d11d8ad6c3UL, 0x0019cfcdfd60e514UL) /* 2539 */
P( 4, 0x2e9c0942f1ce450fUL, 0x0019c56932d66c85UL) /* 2543 */
P( 6, 0x97f3f2be37a39a5dUL, 0x0019b5e1ab6fc7c2UL) /* 2549 */
P( 2, 0xe8b7d8a9654187c7UL, 0x0019b0b8a62f2a73UL) /* 2551 */
P( 6, 0xb5d024d7da5b1b55UL, 0x0019a149fc98942cUL) /* 2557 */
P(22, 0xb8ba9d6e7ae3501bUL, 0x001969517ec25b85UL) /* 2579 */
P(12, 0xf50865f71b90f1dfUL, 0x00194b3083360ba8UL) /* 2591 */
P( 2, 0x739c1682847df9e1UL, 0x00194631f4bebdc1UL) /* 2593 */
P(16, 0xc470a4d842b90ed1UL, 0x00191e84127268fdUL) /* 2609 */
P( 8, 0x1fb1be11698cc409UL, 0x00190adbb543984fUL) /* 2617 */
P( 4, 0xd8d5512a7cd35d15UL, 0x001901130bd18200UL) /* 2621 */
P(12, 0xa5496821723e07f9UL, 0x0018e3e6b889ac94UL) /* 2633 */
P(14, 0xbcc8c6d7abaa8167UL, 0x0018c233420e1ec1UL) /* 2647 */
P(10, 0x52c396c95eb619a1UL, 0x0018aa5872d92bd6UL) /* 2657 */
P( 2, 0x6eb7e380878ec74bUL, 0x0018a5989945ccf9UL) /* 2659 */
P( 4, 0x3d5513b504537157UL, 0x00189c1e60b57f60UL) /* 2663 */
P( 8, 0x314391f8862e948fUL, 0x0018893fbc8690b9UL) /* 2671 */
P( 6, 0xdc0b17cfcd81f5ddUL, 0x00187b2bb3e1041cUL) /* 2677 */
P( 6, 0x2f6bea3ec89044b3UL, 0x00186d27c9cdcfb8UL) /* 2683 */
P( 4, 0xce13a05869f1b57fUL, 0x001863d8bf4f2c1cUL) /* 2687 */
P( 2, 0x7593474e8ace3581UL, 0x00185f33e2ad7593UL) /* 2689 */
P( 4, 0x07fc329295a05e4dUL, 0x001855ef75973e13UL) /* 2693 */
P( 6, 0xb05377cba4908d23UL, 0x001848160153f134UL) /* 2699 */
P( 8, 0xe7b2131a628aa39bUL, 0x001835b72e6f0656UL) /* 2707 */
P( 4, 0x9031dbed7de01527UL, 0x00182c922d83eb39UL) /* 2711 */
P( 2, 0x76844b1c670aa9a9UL, 0x0018280243c0365aUL) /* 2713 */
P( 6, 0x6a03f4533b08915fUL, 0x00181a5cd5898e73UL) /* 2719 */
P(10, 0x1dbca579db0a3999UL, 0x001803c0961773aaUL) /* 2729 */
P( 2, 0x002ffe800bffa003UL, 0x0017ff4005ffd001UL) /* 2731 */
P(10, 0x478ab1a3e936139dUL, 0x0017e8d670433edbUL) /* 2741 */
P( 8, 0x66e722bc4c5cc095UL, 0x0017d7066cf4bb5dUL) /* 2749 */
P( 4, 0x7a8f63c717278541UL, 0x0017ce285b806b1fUL) /* 2753 */
P(14, 0xdf6eee24d292bc2fUL, 0x0017af52cdf27e02UL) /* 2767 */
P(10, 0x9fc20d17237dd569UL, 0x0017997d47d01039UL) /* 2777 */
P(12, 0xcdf9932356bda2edUL, 0x00177f7ec2c6d0baUL) /* 2789 */
P( 2, 0x97b5e332e80f68d7UL, 0x00177b2f3cd00756UL) /* 2791 */
P( 6, 0x46eee26fd875e2e5UL, 0x00176e4a22f692a0UL) /* 2797 */
P( 4, 0x3548a8e65157a611UL, 0x001765b94271e11bUL) /* 2801 */
P( 2, 0xc288d03be9b71e3bUL, 0x001761732b044ae4UL) /* 2803 */
P(16, 0x8151186db38937abUL, 0x00173f7a5300a2bcUL) /* 2819 */
P(14, 0x7800b910895a45f1UL, 0x001722112b48be1fUL) /* 2833 */
P( 4, 0xaee0b024182eec3dUL, 0x001719b7a16eb843UL) /* 2837 */
P( 6, 0x96323eda173b5713UL, 0x00170d3c99cc5052UL) /* 2843 */
P( 8, 0x0ed0dbd03ae77c8bUL, 0x0016fcad7aed3bb6UL) /* 2851 */
P( 6, 0xf73800b7828dc119UL, 0x0016f051b8231ffdUL) /* 2857 */
P( 4, 0x1b61715ec22b7ca5UL, 0x0016e81beae20643UL) /* 2861 */
P(18, 0xa8533a991ead64bfUL, 0x0016c3721584c1d8UL) /* 2879 */
P( 8, 0x7f6c7290e46c2e77UL, 0x0016b34c2ba09663UL) /* 2887 */
P(10, 0x6325e8d907b01db1UL, 0x00169f3ce292ddcdUL) /* 2897 */
P( 6, 0x28909f70152a1067UL, 0x00169344b2220a0dUL) /* 2903 */
P( 6, 0xea7077af0997a0f5UL, 0x001687592593c1b1UL) /* 2909 */
P( 8, 0x7e605cad10c32e6dUL, 0x00167787f1418ec9UL) /* 2917 */
P(10, 0x471b33570635b38fUL, 0x001663e190395ff2UL) /* 2927 */
P(12, 0xab559fa997a61bb3UL, 0x00164c7a4b6eb5b3UL) /* 2939 */
P(14, 0xad4bdae562bddab9UL, 0x0016316a061182fdUL) /* 2953 */
P( 4, 0x055e1b2f2ed62f45UL, 0x001629ba914584e4UL) /* 2957 */
P( 6, 0x03cd328b1a2dca9bUL, 0x00161e3d57de21b2UL) /* 2963 */
P( 6, 0xd28f4e08733218a9UL, 0x001612cc01b977f0UL) /* 2969 */
P( 2, 0xb6800b077f186293UL, 0x00160efe30c525ffUL) /* 2971 */
P(28, 0x6fbd138c3fd9c207UL, 0x0015da45249ec5deUL) /* 2999 */
P( 2, 0xb117ccd12ae88a89UL, 0x0015d68ab4acff92UL) /* 3001 */
P(10, 0x2f1a1a044046bcebUL, 0x0015c3f989d1eb15UL) /* 3011 */
P( 8, 0x548aba0b060541e3UL, 0x0015b535ad11b8f0UL) /* 3019 */
P( 4, 0xcf4e808cea111b2fUL, 0x0015addb3f424ec1UL) /* 3023 */
P(14, 0xdbec1b4fa855a475UL, 0x00159445cb91be6bUL) /* 3037 */
P( 4, 0xe3f794eb600d7821UL, 0x00158d0199771e63UL) /* 3041 */
P( 8, 0x34fae0d9a11f7c59UL, 0x00157e87d9b69e04UL) /* 3049 */
P(12, 0xf006b0ccbbac085dUL, 0x001568f58bc01ac3UL) /* 3061 */
P( 6, 0x3f45076dc3114733UL, 0x00155e3c993fda9bUL) /* 3067 */
P(12, 0xeef49bfa58a1a1b7UL, 0x001548eacc5e1e6eUL) /* 3079 */
P( 4, 0x12c4218bea691fa3UL, 0x001541d8f91ba6a7UL) /* 3083 */
P( 6, 0xbc7504e3bd5e64f1UL, 0x00153747060cc340UL) /* 3089 */
P(20, 0x4ee21c292bb92fadUL, 0x001514569f93f7c4UL) /* 3109 */
P(10, 0x34338b7327a4bacfUL, 0x00150309705d3d79UL) /* 3119 */
P( 2, 0x3fe5c0833d6fccd1UL, 0x0014ff97020cf5bfUL) /* 3121 */
P(16, 0xb1e70743535203c1UL, 0x0014e42c114cf47eUL) /* 3137 */
P(26, 0xefbb5dcdfb4e43d3UL, 0x0014b835bdcb6447UL) /* 3163 */
P( 4, 0xca68467ca5394f9fUL, 0x0014b182b53a9ab7UL) /* 3167 */
P( 2, 0x8c51c081408b97a1UL, 0x0014ae2ad094a3d3UL) /* 3169 */
P(12, 0x3275a899dfa5dd65UL, 0x00149a320ea59f96UL) /* 3181 */
P( 6, 0x9e674cb62e1b78bbUL, 0x001490441de1a2fbUL) /* 3187 */
P( 4, 0xa37ff5bb2a998d47UL, 0x001489aacce57200UL) /* 3191 */
P(12, 0x792a999db131a22bUL, 0x001475f82ad6ff99UL) /* 3203 */
P( 6, 0x1b48841bc30d29b9UL, 0x00146c2cfe53204fUL) /* 3209 */
P( 8, 0xf06721d2011d3471UL, 0x00145f2ca490d4a1UL) /* 3217 */
P( 4, 0x93fd2386dff85ebdUL, 0x001458b2aae0ec87UL) /* 3221 */
P( 8, 0x4ce72f54c07ed9b5UL, 0x00144bcb0a3a3150UL) /* 3229 */
P(22, 0xd6d0fd3e71dd827bUL, 0x001428a1e65441d4UL) /* 3251 */
P( 2, 0x856405fb1eed819dUL, 0x00142575a6c210d7UL) /* 3253 */
P( 4, 0x8ea8aceb7c443989UL, 0x00141f2025ba5c46UL) /* 3257 */
P( 2, 0x34a13026f62e5873UL, 0x00141bf6e35420fdUL) /* 3259 */
P(12, 0x1eea0208ec0af4f7UL, 0x001409141d1d313aUL) /* 3271 */
P(28, 0x63679853cea598cbUL, 0x0013dd8bc19c3513UL) /* 3299 */
P( 2, 0xc30b3ebd61f2d0edUL, 0x0013da76f714dc8fUL) /* 3301 */
P( 6, 0x7eb9037bc7f43bc3UL, 0x0013d13e50f8f49eUL) /* 3307 */
P( 6, 0xa583e6f6ce016411UL, 0x0013c80e37ca3819UL) /* 3313 */
P( 6, 0xf1938d895f1a74c7UL, 0x0013bee69fa99ccfUL) /* 3319 */
P( 4, 0x80cf1491c1e81e33UL, 0x0013b8d0ede55835UL) /* 3323 */
P( 6, 0x3c0f12886ba8f301UL, 0x0013afb7680bb054UL) /* 3329 */
P( 2, 0x0e4b786e0dfcc5abUL, 0x0013acb0c3841c96UL) /* 3331 */
P(12, 0x672684c93f2d41efUL, 0x00139a9c5f434fdeUL) /* 3343 */
P( 4, 0xe00757badb35c51bUL, 0x0013949cf33a0d9dUL) /* 3347 */
P(12, 0xd6d84afe66472edfUL, 0x001382b4a00c31b0UL) /* 3359 */
P( 2, 0xfbbc0eedcbbfb6e1UL, 0x00137fbbc0eedcbbUL) /* 3361 */
P(10, 0x250f43aa08a84983UL, 0x001370ecf047b069UL) /* 3371 */
P( 2, 0x04400e927b1acaa5UL, 0x00136df9790e3155UL) /* 3373 */
P(16, 0x56572be34b9d3215UL, 0x0013567dd8defd5bUL) /* 3389 */
P( 2, 0x87964ef7781c62bfUL, 0x0013539261fdbc34UL) /* 3391 */
P(16, 0x29ed84051c06e9afUL, 0x00133c564292d28aUL) /* 3407 */
P( 6, 0xb00acd11ed3f87fdUL, 0x001333ae178d6388UL) /* 3413 */
P(20, 0x06307881744152d9UL, 0x0013170ad00d1fd7UL) /* 3433 */
P(16, 0x7a786459f5c1ccc9UL, 0x0013005f01db0947UL) /* 3449 */
P( 8, 0x1308125d74563281UL, 0x0012f51d40342210UL) /* 3457 */
P( 4, 0x395310a480b3e34dUL, 0x0012ef815e4ed950UL) /* 3461 */
P( 2, 0x35985baa8b202837UL, 0x0012ecb4abccd827UL) /* 3463 */
P( 4, 0x96304a6e052b3223UL, 0x0012e71dc1d3d820UL) /* 3467 */
P( 2, 0xbd8265fc9af8fd45UL, 0x0012e45389a16495UL) /* 3469 */
P(22, 0x1b6d0b383ec58e0bUL, 0x0012c5d9226476ccUL) /* 3491 */
P( 8, 0xc21a7c3b68b28503UL, 0x0012badc391156fdUL) /* 3499 */
P(12, 0x236fa180fbfd6007UL, 0x0012aa78e412f522UL) /* 3511 */
P( 6, 0xc42accd440ed9595UL, 0x0012a251f5f47fd1UL) /* 3517 */
P(10, 0x7acf7128236ba3f7UL, 0x001294cb85c53534UL) /* 3527 */
P( 2, 0xf909367a987b9c79UL, 0x0012921963beb65eUL) /* 3529 */
P( 4, 0xb64efb252bfba705UL, 0x00128cb777c69ca8UL) /* 3533 */
P( 6, 0x980d4f5a7e4cd25bUL, 0x001284aa6cf07294UL) /* 3539 */
P( 2, 0xe1ecc4ef27b0c37dUL, 0x001281fcf6ac7f87UL) /* 3541 */
P( 6, 0x9111aebb81d72653UL, 0x001279f937367db9UL) /* 3547 */
P(10, 0x8951f985cb2c67edUL, 0x00126cad0488be94UL) /* 3557 */
P( 2, 0xc439d4fc54e0b5d7UL, 0x00126a06794646a2UL) /* 3559 */
P(12, 0xe857bf31896d533bUL, 0x00125a2f2bcd3e95UL) /* 3571 */
P(10, 0xb614bb4cb5023755UL, 0x00124d108389e6b1UL) /* 3581 */
P( 2, 0x938a89e5473bf1ffUL, 0x00124a73083771acUL) /* 3583 */
P(10, 0xeac481aca34de039UL, 0x00123d6acda0620aUL) /* 3593 */
P(14, 0x14b961badf4809a7UL, 0x00122b4b2917eafdUL) /* 3607 */
P( 6, 0x76784fecba352435UL, 0x00122391bfce1e2fUL) /* 3613 */
P( 4, 0xefa689bb58aef5e1UL, 0x00121e6f1ea579f2UL) /* 3617 */
P( 6, 0xb2b2c4db9c3a8197UL, 0x001216c09e471568UL) /* 3623 */
P( 8, 0x2503bc992279f8cfUL, 0x00120c8cb9d93909UL) /* 3631 */
P( 6, 0xd2ab9aec5ca1541dUL, 0x001204ed58e64ef9UL) /* 3637 */
P( 6, 0x3e78ba1460f99af3UL, 0x0011fd546578f00cUL) /* 3643 */
P(16, 0x0a01426572cfcb63UL, 0x0011e9310b8b4c9cUL) /* 3659 */
P(12, 0xbea857968f3cbd67UL, 0x0011da3405db9911UL) /* 3671 */
P( 2, 0x78db213eefe659e9UL, 0x0011d7b6f4eb055dUL) /* 3673 */
P( 4, 0x963e8541a74d35f5UL, 0x0011d2bee748c145UL) /* 3677 */
P(14, 0x9e22d152776f2e43UL, 0x0011c1706ddce7a7UL) /* 3691 */
P( 6, 0x05d10d39d1e1f291UL, 0x0011ba0fed2a4f14UL) /* 3697 */
P( 4, 0x374468dccaced1ddUL, 0x0011b528538ed64aUL) /* 3701 */
P( 8, 0x8d145c7d110c5ad5UL, 0x0011ab61404242acUL) /* 3709 */
P(10, 0x3251a39f5acb5737UL, 0x00119f378ce81d2fUL) /* 3719 */
P( 8, 0xa66e50171443506fUL, 0x001195889ece79daUL) /* 3727 */
P( 6, 0x124f69ad91dd4cbdUL, 0x00118e4c65387077UL) /* 3733 */
P( 6, 0xec24f8f2a61a2793UL, 0x001187161d70e725UL) /* 3739 */
P(22, 0xb472148e656b7a51UL, 0x00116cd6d1c85239UL) /* 3761 */
P( 6, 0x0adf9570e1142f07UL, 0x001165bbe7ce86b1UL) /* 3767 */
P( 2, 0x89bf33b065119789UL, 0x0011635ee344ce36UL) /* 3769 */
P(10, 0x8f0149803cb291ebUL, 0x0011579767b6d679UL) /* 3779 */
P(14, 0x8334b63afd190a31UL, 0x00114734711e2b54UL) /* 3793 */
P( 4, 0x920908d50d6aba7dUL, 0x0011428b90147f05UL) /* 3797 */
P( 6, 0x57d8b018c5a33d53UL, 0x00113b92f3021636UL) /* 3803 */
P(18, 0xea1773092dc27ee5UL, 0x001126cabc886884UL) /* 3821 */
P( 2, 0xcae5f38b7bf2e00fUL, 0x0011247eb1b85976UL) /* 3823 */
P(10, 0x2bd02df34f695349UL, 0x0011190bb01efd65UL) /* 3833 */
P(14, 0xddfecd5be62e2eb7UL, 0x0011091de0fd679cUL) /* 3847 */
P( 4, 0xdbf849ebec96c4a3UL, 0x001104963c7e4e0bUL) /* 3851 */
P( 2, 0xda31d4d0187357c5UL, 0x00110253516420b0UL) /* 3853 */
P(10, 0xe34e21cc2d5418a7UL, 0x0010f70db7c41797UL) /* 3863 */
P(14, 0x68ca5137a9e574adUL, 0x0010e75ee2bf9ecdUL) /* 3877 */
P( 4, 0x3eaa0d0f804bfd19UL, 0x0010e2e91c6e0676UL) /* 3881 */
P( 8, 0x554fb753cc20e9d1UL, 0x0010da049b9d428dUL) /* 3889 */
P(18, 0x797afcca1300756bUL, 0x0010c6248fe3b1a2UL) /* 3907 */
P( 4, 0x8b8d950b52eeea77UL, 0x0010c1c03ed690ebUL) /* 3911 */
P( 6, 0xfb6cd166acabc185UL, 0x0010bb2e1379e3a2UL) /* 3917 */
P( 2, 0x4eb6c5ed9437a7afUL, 0x0010b8fe7f61228eUL) /* 3919 */
P( 4, 0xd1eddbd91b790cdbUL, 0x0010b4a10d60a4f7UL) /* 3923 */
P( 6, 0x93d714ea4d8948e9UL, 0x0010ae192681ec0fUL) /* 3929 */
P( 2, 0x3ca13ed8145188d3UL, 0x0010abecfbe5b0aeUL) /* 3931 */
P(12, 0x829086016da89c57UL, 0x00109eefd568b96dUL) /* 3943 */
P( 4, 0xd7da1f432124a543UL, 0x00109a9ff178b40cUL) /* 3947 */
P(20, 0x7ead5581632fb07fUL, 0x00108531e22f9ff9UL) /* 3967 */
P(22, 0x35443837f63ec3bdUL, 0x00106ddec1af4417UL) /* 3989 */

#undef FIRST_OMITTED_PRIME
#define FIRST_OMITTED_PRIME 4001
#undef P
};
#define PRIMES_PTAB_ENTRIES (sizeof(primes_diff) / sizeof(primes_diff[0]))

// int flag_verbose = 0;
// 
// /* Prove primality or run probabilistic tests.  */
// int flag_prove_primality = 1;

/* Number of Miller-Rabin tests to run when not proving primality. */
#define MR_REPS 25

struct factors
{
  mpz_t         *p;
  unsigned long *e;
  long nfactors;
};

static void factor (mpz_t t, struct factors *factors, int flag_prove_primality);

static void factor_init (struct factors *factors)
{
  factors->p = (mpz_t*)malloc (1);
  factors->e = (unsigned long*)malloc (1);
  factors->nfactors = 0;
}

static void
factor_clear (struct factors *factors)
{
  int i;

  for (i = 0; i < factors->nfactors; i++)
    mpz_clear (factors->p[i]);

  free (factors->p);
  free (factors->e);
}

static void
factor_insert (struct factors *factors, mpz_t prime)
{
  long    nfactors  = factors->nfactors;
  mpz_t         *p  = factors->p;
  unsigned long *e  = factors->e;
  long i, j;

  /* Locate position for insert new or increment e.  */
  for (i = nfactors - 1; i >= 0; i--)
    {
      if (mpz_cmp (p[i], prime) <= 0)
	break;
    }

  if (i < 0 || mpz_cmp (p[i], prime) != 0)
    {
      p = (mpz_t*)realloc (p, (nfactors + 1) * sizeof p[0]);
      e = (unsigned long*)realloc (e, (nfactors + 1) * sizeof e[0]);

      mpz_init (p[nfactors]);
      for (j = nfactors - 1; j > i; j--)
	{
	  mpz_set (p[j + 1], p[j]);
	  e[j + 1] = e[j];
	}
      mpz_set (p[i + 1], prime);
      e[i + 1] = 1;

      factors->p = p;
      factors->e = e;
      factors->nfactors = nfactors + 1;
    }
  else
    {
      e[i] += 1;
    }
}

static void
factor_insert_ui (struct factors *factors, unsigned long prime)
{
  mpz_t pz;

  mpz_init_set_ui (pz, prime);
  factor_insert (factors, pz);
  mpz_clear (pz);
}


void
factor_using_division (mpz_t t, struct factors *factors)
{
  mpz_t q;
  unsigned long int p;
  int i;

  mpz_init (q);

  p = mpz_scan1 (t, 0);
  mpz_fdiv_q_2exp (t, t, p);
  while (p)
    {
      factor_insert_ui (factors, 2);
      --p;
    }

  p = 3;
  for (i = 1; i <= PRIMES_PTAB_ENTRIES;)
    {
      if (! mpz_divisible_ui_p (t, p))
	{
	  p += primes_diff[i++];
	  if (mpz_cmp_ui (t, p * p) < 0)
	    break;
	}
      else
	{
	  mpz_tdiv_q_ui (t, t, p);
	  factor_insert_ui (factors, p);
	}
    }

  mpz_clear (q);
}

static int
mp_millerrabin (mpz_srcptr n, mpz_srcptr nm1, mpz_ptr x, mpz_ptr y,
		mpz_srcptr q, unsigned long int k)
{
  unsigned long int i;

  mpz_powm (y, x, q, n);

  if (mpz_cmp_ui (y, 1) == 0 || mpz_cmp (y, nm1) == 0)
    return 1;

  for (i = 1; i < k; i++)
    {
      mpz_powm_ui (y, y, 2, n);
      if (mpz_cmp (y, nm1) == 0)
	return 1;
      if (mpz_cmp_ui (y, 1) == 0)
	return 0;
    }
  return 0;
}

static int
mp_prime_p (mpz_t n, int flag_prove_primality)
{
  int k, r, is_prime;
  mpz_t q, a, nm1, tmp;
  struct factors factors;

  if (mpz_cmp_ui (n, 1) <= 0)
    return 0;

  /* We have already casted out small primes. */
  if (mpz_cmp_ui (n, (long) FIRST_OMITTED_PRIME * FIRST_OMITTED_PRIME) < 0)
    return 1;

  mpz_inits (q, a, nm1, tmp, NULL);

  /* Precomputation for Miller-Rabin.  */
  mpz_sub_ui (nm1, n, 1);

  /* Find q and k, where q is odd and n = 1 + 2**k * q.  */
  k = mpz_scan1 (nm1, 0);
  mpz_tdiv_q_2exp (q, nm1, k);

  mpz_set_ui (a, 2);

  /* Perform a Miller-Rabin test, finds most composites quickly.  */
  if (!mp_millerrabin (n, nm1, a, tmp, q, k))
    {
      is_prime = 0;
      goto ret2;
    }

  if (flag_prove_primality)
    {
      /* Factor n-1 for Lucas.  */
      mpz_set (tmp, nm1);
      factor (tmp, &factors, flag_prove_primality);
    }

  /* Loop until Lucas proves our number prime, or Miller-Rabin proves our
     number composite.  */
  for (r = 0; r < PRIMES_PTAB_ENTRIES; r++)
    {
      int i;

      if (flag_prove_primality)
	{
	  is_prime = 1;
	  for (i = 0; i < factors.nfactors && is_prime; i++)
	    {
	      mpz_divexact (tmp, nm1, factors.p[i]);
	      mpz_powm (tmp, a, tmp, n);
	      is_prime = mpz_cmp_ui (tmp, 1) != 0;
	    }
	}
      else
	{
	  /* After enough Miller-Rabin runs, be content. */
	  is_prime = (r == MR_REPS - 1);
	}

      if (is_prime)
	goto ret1;

      mpz_add_ui (a, a, primes_diff[r]);	/* Establish new base.  */

      if (!mp_millerrabin (n, nm1, a, tmp, q, k))
	{
	  is_prime = 0;
	  goto ret1;
	}
    }

    return 0;  //Ci provo
//   fprintf (stderr, "Lucas prime test failure.  This should not happen\n");
//   abort ();

 ret1:
  if (flag_prove_primality)
    factor_clear (&factors);
 ret2:
  mpz_clears (q, a, nm1, tmp, NULL);

  return is_prime;
}

static void
factor_using_pollard_rho (mpz_t n, unsigned long a, struct factors *factors, int flag_prove_primality)
{
  mpz_t x, z, y, P;
  mpz_t t, t2;
  unsigned long long k, l, i;

  mpz_inits (t, t2, NULL);
  mpz_init_set_si (y, 2);
  mpz_init_set_si (x, 2);
  mpz_init_set_si (z, 2);
  mpz_init_set_ui (P, 1);
  k = 1;
  l = 1;

  while (mpz_cmp_ui (n, 1) != 0)
    {
      for (;;)
	{
	  do
	    {
	      mpz_mul (t, x, x);
	      mpz_mod (x, t, n);
	      mpz_add_ui (x, x, a);

	      mpz_sub (t, z, x);
	      mpz_mul (t2, P, t);
	      mpz_mod (P, t2, n);

	      if (k % 32 == 1)
		{
		  mpz_gcd (t, P, n);
		  if (mpz_cmp_ui (t, 1) != 0)
		    goto factor_found;
		  mpz_set (y, x);
		}
	    }
	  while (--k != 0);

	  mpz_set (z, x);
	  k = l;
	  l = 2 * l;
	  for (i = 0; i < k; i++)
	    {
	      mpz_mul (t, x, x);
	      mpz_mod (x, t, n);
	      mpz_add_ui (x, x, a);
	    }
	  mpz_set (y, x);
	}

    factor_found:
      do
	{
	  mpz_mul (t, y, y);
	  mpz_mod (y, t, n);
	  mpz_add_ui (y, y, a);

	  mpz_sub (t, z, y);
	  mpz_gcd (t, t, n);
	}
      while (mpz_cmp_ui (t, 1) == 0);

      mpz_divexact (n, n, t);	/* divide by t, before t is overwritten */

      if (!mp_prime_p (t, flag_prove_primality))
	{
	  factor_using_pollard_rho (t, a + 1, factors, flag_prove_primality);
	}
      else
	{
	  factor_insert (factors, t);
	}

      if (mp_prime_p (n, flag_prove_primality))
	{
	  factor_insert (factors, n);
	  break;
	}

      mpz_mod (x, x, n);
      mpz_mod (z, z, n);
      mpz_mod (y, y, n);
    }

  mpz_clears (P, t2, t, z, x, y, NULL);
}

static void
factor (mpz_t t, struct factors *factors, int flag_prove_primality)
{
  factor_init (factors);

  if (mpz_sgn (t) != 0)
    {
      factor_using_division (t, factors);

      if (mpz_cmp_ui (t, 1) != 0)
	{
	  if (mp_prime_p (t, flag_prove_primality))
	    factor_insert (factors, t);
	  else
	    factor_using_pollard_rho (t, 1, factors, flag_prove_primality);
	}
    }
}

static SCM mtfa_mpz_factorize (SCM num, SCM prove_primality)
{
    int flag_prove_primality = scm_to_int(prove_primality);
    
    mpz_t t;
    int i, j, k;
    struct factors factors;

    mpz_init (t);
    scm_to_mpz ( num, t );
    factor (t, &factors, flag_prove_primality);
    mpz_clear(t);

    SCM l = SCM_EOL;
    for (j = 0; j < factors.nfactors; j++)
    {
        for (k = 0; k < factors.e[j]; k++)
        {
            l = scm_cons(scm_from_mpz(factors.p[j]), l);
        }
    }
    factor_clear (&factors);
    return l;
}

static SCM mtfa_mpz_probab_prime_p ( SCM _n, SCM _reps );
SCM mtfa_mpz_probab_prime_p ( SCM _n, SCM _reps )
{
    mpz_t n;
    int reps;

    mpz_init ( n );
    scm_to_mpz(_n, n );
    reps = scm_to_int ( _reps );
    int iRet = mpz_probab_prime_p ( n, reps );

    mpz_clear ( n );

    if ( iRet >= 1 )
        return SCM_BOOL_T;
    else
        return SCM_BOOL_F;
}

static SCM mtfa_mpz_nextprime ( SCM _op );
SCM mtfa_mpz_nextprime ( SCM _op )
{
    mpz_t rop;
    mpz_t op;
    mpz_inits ( rop, op, 0 );
    scm_to_mpz ( _op, op );

    mpz_nextprime ( rop, op );
    SCM ret = scm_from_mpz ( rop );

    mpz_clears ( op, rop, 0 );
    return ret;
}

static SCM mtfa_mpz_gcdext ( SCM _a, SCM _b );
SCM mtfa_mpz_gcdext ( SCM _a, SCM _b )
{
    mpz_t a, b, g, s, t;

    mpz_inits ( a, b, g, s, t, 0 );

    scm_to_mpz ( _a, a );
    scm_to_mpz ( _b, b );

    mpz_gcdext ( g, s, t, a, b );
    SCM l, _g, _s, _t;
    l = SCM_EOL;
    _g = scm_from_mpz ( g );
    _s = scm_from_mpz ( s );
    _t = scm_from_mpz ( t );
    l = scm_cons ( _g, scm_cons ( _s, scm_cons ( _t, l ) ) );

    mpz_clears ( g, s, t, a, b, 0 );
    return l;
}

static SCM mtfa_mpz_invert ( SCM _op1, SCM _op2 );
SCM mtfa_mpz_invert ( SCM _op1, SCM _op2 )
{
    mpz_t rop, op1, op2;

    mpz_inits ( rop, op1, op2, 0 );

    scm_to_mpz ( _op1, op1 );
    scm_to_mpz ( _op2, op2 );

    int iret = mpz_invert ( rop, op1, op2 );

    if ( iret == 0 )
    {
        mpz_clears ( rop, op1, op2, 0 );
        return SCM_EOL;
    }
    else
    {
        SCM ret = scm_from_mpz ( rop );
        mpz_clears ( rop, op1, op2, 0 );
        return ret;
    }
}

static SCM scm_mtfa_compress ( SCM scm_data , SCM scm_level)
{
    unsigned char* p;
    size_t len;

    if (!ScmToPUChar ( scm_data, p, len ))
        return SCM_EOL;

    int level = scm_to_int(scm_level);

    string compressed;
    bool bret = mtfa_compress(string(p, p+len), compressed, level);
    free(p);

    //Compresso bene?
    if (!bret)
        return SCM_EOL;

    //Se era stringa metto 1 in coda, altrimenti metto 2
    if (scm_is_string(scm_data))
        compressed = compressed+"1";
    else
        compressed = compressed+"2";

    //Converto in puntatore
    p = (unsigned char*)malloc(compressed.size());
    memcpy(p, compressed.data(), compressed.size());

    SCM scm_ret = scm_pointer_to_bytevector (
        scm_from_pointer ( ( void* ) p, free ),
        scm_from_int ( compressed.size() ),
        scm_from_int ( 0 ),
        scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM scm_mtfa_uncompress ( SCM scm_data )
{
    unsigned char* p;
    size_t len;

    if ( scm_is_bytevector ( scm_data ) )
    {
        //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
        len = SCM_BYTEVECTOR_LENGTH ( scm_data );
        p = ( unsigned char* ) malloc ( len );
        memcpy ( p, ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_data ), len );
    }
    else
    {
        return SCM_EOL;
    }

    //decomprimo ma prima verifico se era bytevector o stringa
    bool isStringa;
    if (p[len-1]=='1')
        isStringa=true;
    else
        isStringa=false;

    string originale;
    bool bret = mtfa_uncompress(string(p, p+len-1), originale);

    //posso svuotare memoria
    free(p);

    if (!bret)
        return SCM_EOL;

    if (isStringa)
    {
        SCM scmRet = ScmFromStringn(originale.data(), originale.size());
        return scmRet;
    }
    else
    {
        unsigned char * p = (unsigned char*)malloc(originale.size());
        memcpy(p, originale.data(), originale.size());
        SCM scm_ret = scm_pointer_to_bytevector (
            scm_from_pointer ( ( void* ) p, free ),
                                scm_from_int ( originale.size()),
                                scm_from_int ( 0 ),
                                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
}


static SCM mtfa_find_all_positions ( SCM s_stringa, SCM s_pattern,SCM tipo );

static SCM mtfa_find_all_positions ( SCM s_stringa, SCM s_pattern, SCM tipo /*0, 1, 2*/ )
{
    size_t lenData=0;
	
    char* data = ScmToStringn ( s_stringa, &lenData);
    
    if (data == nullptr || lenData==0)
        return SCM_BOOL_F;
    
    size_t lenPattern=0;
	std::list<pair<int, int>>  lret;
	if (scm_is_string(s_pattern))
    {
        char* pattern = ScmToStringn ( s_pattern, &lenPattern);
        if (pattern == nullptr || lenPattern==0)
        {
            free ( data );
            return SCM_BOOL_F;
        }

        lret = mtfa_get_all_submatching_positions(string(data, data+lenData), string(pattern, pattern+lenPattern));
        free ( data );
        free(pattern);
    } else {
        pcre2_code * pattern = (pcre2_code *)scm_to_pointer(s_pattern);
        if (!pattern)
        {
            free ( data );
            return SCM_BOOL_F;
        }
        
        lret = mtfa_get_all_submatching_positions(string(data, lenData), pattern);
        free ( data );
    }

    SCM plist = SCM_EOL;
    
    for (auto it: lret)
    {
        SCM i_start = scm_from_int(it.first);
        SCM i_end = scm_from_int(it.second);
        SCM i_pair = scm_cons(i_start, i_end);
        plist = scm_cons(i_pair, plist);
    }
    return plist; //scm_reverse(plist);
}

static SCM mtfa_find_all_matching_positions( SCM s_stringa, SCM s_pattern, SCM tipo /*0, 1, 2*/ )
{
//     //Provetta
//     {
//         auto pat = "p+";
//         cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", pat).size() << endl;  
//         auto patt = mtfa_compile_pattern("p+", true);
//         cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", patt).size() << endl;
//         auto patf = mtfa_compile_pattern("p+", false);
//         cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", patf).size() << endl;
//     }
    
    if (scm_is_string(s_pattern))
    {
        size_t lenData=0;

        char* data = ScmToStringn ( s_stringa, &lenData );

        if (data == nullptr || lenData==0)
            return SCM_BOOL_F;

        size_t lenPattern=0;

        char* pattern = ScmToStringn ( s_pattern, &lenPattern );
        if (pattern == nullptr || lenPattern==0)
        {
            free ( data );
            return SCM_BOOL_F;
        }

        std::list<pair<int, int>> lret = mtfa_get_all_matching_positions( string(data, data+lenData), string(pattern, pattern+lenPattern));
        free ( data );
        free(pattern);

        SCM plist = SCM_EOL;

        for (auto it: lret)
        {
            SCM i_start = scm_from_int(it.first);
            SCM i_end = scm_from_int(it.second);
            SCM i_pair = scm_cons(i_start, i_end);
            plist = scm_cons(i_pair, plist);
        }
        return scm_reverse(plist);
    }
    else
    {
        size_t len=0;

        char* data = ScmToStringn ( s_stringa, &len );

        if (data == nullptr || len==0)
            return SCM_BOOL_F;

        pcre2_code * pattern = (pcre2_code *)scm_to_pointer(s_pattern);
        std::list<pair<int, int>> lret = mtfa_get_all_matching_positions( string(data, data+len), pattern);
        free ( data );

        SCM plist = SCM_EOL;

        for (auto it: lret)
        {
            SCM i_start = scm_from_int(it.first);
            SCM i_end = scm_from_int(it.second);
            SCM i_pair = scm_cons(i_start, i_end);
            plist = scm_cons(i_pair, plist);
        }
        return scm_reverse(plist);
    }
}

static SCM make_cookie(SCM key, SCM value, SCM expiration, SCM howmanysecs, SCM path, SCM domain, SCM http_only, SCM secure)
{
    //(string key, string value, CookieDurations expiration, int howManySecs, string path, string domain, bool http_only, bool secure)
    char * s_key = ScmToString(key);
    char * s_value = ScmToString(value);
    
    //SessionCookie, NeverExpires, ExpiresInSeconds
    char * s_expiration = ScmToString(expiration);
    int i_howmanysecs = scm_to_int(howmanysecs);
    char * s_path = ScmToString(path);
    char * s_domain = ScmToString(domain);
    bool b_http_only = scm_to_bool(http_only);
    bool b_secure = scm_to_bool(secure);
    CookieDurations duration;
    if (strcmp(s_expiration, "SessionCookie")==0)
        duration=CookieDurations::SessionCookie;
    else if (strcmp(s_expiration, "NeverExpires")==0)
        duration=CookieDurations::NeverExpires;
    else if (strcmp(s_expiration, "ExpiresInSeconds")==0)
        duration=CookieDurations::ExpiresInSeconds;
    else
    {
        free(s_key);
        free(s_value);
        free(s_expiration);
        free(s_path);
        free(s_domain);
        scm_error(SCM_EOL, "Error", "Duration must be one of: SessionCookie NeverExpires ExpiresInSeconds", SCM_EOL, SCM_EOL);
        return SCM_BOOL_F;
    }
    string sret = mtfa_make_cookie(string(s_key), string(s_value), duration, i_howmanysecs, string(s_path), string(s_domain), b_http_only, b_secure);

    free(s_key);
    free(s_value);
    free(s_expiration);
    free(s_path);
    free(s_domain);
    return ScmFromString(sret.c_str());
}


static SCM mtfa_magic_mime (SCM s_filename)
{
	const char *magic_full;
	
	/* MAGIC_MIME tells magic to return a mime of the file, 
	 *       but you can specify different things	*/
	if (!magic_cookie)
	{
		magic_cookie = magic_open(MAGIC_MIME_TYPE);
		if (magic_cookie == NULL) {
			return SCM_BOOL_F;
		}
		
		if (magic_load(magic_cookie, NULL) != 0) {
			magic_close(magic_cookie);
			return SCM_BOOL_F;
		}
	}
	
	if (scm_is_string(s_filename))
	{
		char * filename = scm_to_utf8_string(s_filename);
		magic_full = magic_file(magic_cookie, filename);
		free(filename);
		return ScmFromString(magic_full);
	} else if ( scm_is_bytevector ( s_filename ) )
	{
		int len = SCM_BYTEVECTOR_LENGTH ( s_filename );
		char * p = (char*)SCM_BYTEVECTOR_CONTENTS ( s_filename );
		magic_full = magic_buffer(magic_cookie, p, len);
		return ScmFromString(magic_full);
	}
	else
	{
		return SCM_BOOL_F;
	}
}

static void fill_in_mimes_from_iana(ImplicitDs * s2_extended)
{
	s2_extended->Insert((const char *)".ez", strlen(".ez"), (void*)"application/andrew-inset");
	s2_extended->Insert((const char *)".aw", strlen(".aw"), (void*)"application/applixware");
	s2_extended->Insert((const char *)".atom", strlen(".atom"), (void*)"application/atom+xml");
	s2_extended->Insert((const char *)".atomcat", strlen(".atomcat"), (void*)"application/atomcat+xml");
	s2_extended->Insert((const char *)".atomsvc", strlen(".atomsvc"), (void*)"application/atomsvc+xml");
	s2_extended->Insert((const char *)".ccxml", strlen(".ccxml"), (void*)"application/ccxml+xml");
	s2_extended->Insert((const char *)".cdmia", strlen(".cdmia"), (void*)"application/cdmi-capability");
	s2_extended->Insert((const char *)".cdmic", strlen(".cdmic"), (void*)"application/cdmi-container");
	s2_extended->Insert((const char *)".cdmid", strlen(".cdmid"), (void*)"application/cdmi-domain");
	s2_extended->Insert((const char *)".cdmio", strlen(".cdmio"), (void*)"application/cdmi-object");
	s2_extended->Insert((const char *)".cdmiq", strlen(".cdmiq"), (void*)"application/cdmi-queue");
	s2_extended->Insert((const char *)".cu", strlen(".cu"), (void*)"application/cu-seeme");
	s2_extended->Insert((const char *)".davmount", strlen(".davmount"), (void*)"application/davmount+xml");
	s2_extended->Insert((const char *)".dbk", strlen(".dbk"), (void*)"application/docbook+xml");
	s2_extended->Insert((const char *)".dssc", strlen(".dssc"), (void*)"application/dssc+der");
	s2_extended->Insert((const char *)".xdssc", strlen(".xdssc"), (void*)"application/dssc+xml");
	s2_extended->Insert((const char *)".ecma", strlen(".ecma"), (void*)"application/ecmascript");
	s2_extended->Insert((const char *)".emma", strlen(".emma"), (void*)"application/emma+xml");
	s2_extended->Insert((const char *)".epub", strlen(".epub"), (void*)"application/epub+zip");
	s2_extended->Insert((const char *)".exi", strlen(".exi"), (void*)"application/exi");
	s2_extended->Insert((const char *)".pfr", strlen(".pfr"), (void*)"application/font-tdpfr");
	s2_extended->Insert((const char *)".gml", strlen(".gml"), (void*)"application/gml+xml");
	s2_extended->Insert((const char *)".gpx", strlen(".gpx"), (void*)"application/gpx+xml");
	s2_extended->Insert((const char *)".gxf", strlen(".gxf"), (void*)"application/gxf");
	s2_extended->Insert((const char *)".stk", strlen(".stk"), (void*)"application/hyperstudio");
	s2_extended->Insert((const char *)".ink", strlen(".ink"), (void*)"application/inkml+xml");
	s2_extended->Insert((const char *)".inkml", strlen(".inkml"), (void*)"application/inkml+xml");
	s2_extended->Insert((const char *)".ipfix", strlen(".ipfix"), (void*)"application/ipfix");
	s2_extended->Insert((const char *)".jar", strlen(".jar"), (void*)"application/java-archive");
	s2_extended->Insert((const char *)".ser", strlen(".ser"), (void*)"application/java-serialized-object");
	s2_extended->Insert((const char *)".class", strlen(".class"), (void*)"application/java-vm");
	s2_extended->Insert((const char *)".js", strlen(".js"), (void*)"application/javascript");
	s2_extended->Insert((const char *)".json", strlen(".json"), (void*)"application/json");
	s2_extended->Insert((const char *)".jsonml", strlen(".jsonml"), (void*)"application/jsonml+json");
	s2_extended->Insert((const char *)".lostxml", strlen(".lostxml"), (void*)"application/lost+xml");
	s2_extended->Insert((const char *)".hqx", strlen(".hqx"), (void*)"application/mac-binhex40");
	s2_extended->Insert((const char *)".cpt", strlen(".cpt"), (void*)"application/mac-compactpro");
	s2_extended->Insert((const char *)".mads", strlen(".mads"), (void*)"application/mads+xml");
	s2_extended->Insert((const char *)".mrc", strlen(".mrc"), (void*)"application/marc");
	s2_extended->Insert((const char *)".mrcx", strlen(".mrcx"), (void*)"application/marcxml+xml");
	s2_extended->Insert((const char *)".ma", strlen(".ma"), (void*)"application/mathematica");
	s2_extended->Insert((const char *)".nb", strlen(".nb"), (void*)"application/mathematica");
	s2_extended->Insert((const char *)".mb", strlen(".mb"), (void*)"application/mathematica");
	s2_extended->Insert((const char *)".mathml", strlen(".mathml"), (void*)"application/mathml+xml");
	s2_extended->Insert((const char *)".mbox", strlen(".mbox"), (void*)"application/mbox");
	s2_extended->Insert((const char *)".mscml", strlen(".mscml"), (void*)"application/mediaservercontrol+xml");
	s2_extended->Insert((const char *)".metalink", strlen(".metalink"), (void*)"application/metalink+xml");
	s2_extended->Insert((const char *)".meta4", strlen(".meta4"), (void*)"application/metalink4+xml");
	s2_extended->Insert((const char *)".mets", strlen(".mets"), (void*)"application/mets+xml");
	s2_extended->Insert((const char *)".mods", strlen(".mods"), (void*)"application/mods+xml");
	s2_extended->Insert((const char *)".m21", strlen(".m21"), (void*)"application/mp21");
	s2_extended->Insert((const char *)".mp21", strlen(".mp21"), (void*)"application/mp21");
	s2_extended->Insert((const char *)".mp4s", strlen(".mp4s"), (void*)"application/mp4");
	s2_extended->Insert((const char *)".doc", strlen(".doc"), (void*)"application/msword");
	s2_extended->Insert((const char *)".dot", strlen(".dot"), (void*)"application/msword");
	s2_extended->Insert((const char *)".mxf", strlen(".mxf"), (void*)"application/mxf");
	s2_extended->Insert((const char *)".bin", strlen(".bin"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".dms", strlen(".dms"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".lrf", strlen(".lrf"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".mar", strlen(".mar"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".so", strlen(".so"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".dist", strlen(".dist"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".distz", strlen(".distz"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".pkg", strlen(".pkg"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".bpk", strlen(".bpk"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".dump", strlen(".dump"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".elc", strlen(".elc"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".deploy", strlen(".deploy"), (void*)"application/octet-stream");
	s2_extended->Insert((const char *)".oda", strlen(".oda"), (void*)"application/oda");
	s2_extended->Insert((const char *)".opf", strlen(".opf"), (void*)"application/oebps-package+xml");
	s2_extended->Insert((const char *)".ogx", strlen(".ogx"), (void*)"application/ogg");
	s2_extended->Insert((const char *)".omdoc", strlen(".omdoc"), (void*)"application/omdoc+xml");
	s2_extended->Insert((const char *)".onetoc", strlen(".onetoc"), (void*)"application/onenote");
	s2_extended->Insert((const char *)".onetoc2", strlen(".onetoc2"), (void*)"application/onenote");
	s2_extended->Insert((const char *)".onetmp", strlen(".onetmp"), (void*)"application/onenote");
	s2_extended->Insert((const char *)".onepkg", strlen(".onepkg"), (void*)"application/onenote");
	s2_extended->Insert((const char *)".oxps", strlen(".oxps"), (void*)"application/oxps");
	s2_extended->Insert((const char *)".xer", strlen(".xer"), (void*)"application/patch-ops-error+xml");
	s2_extended->Insert((const char *)".pdf", strlen(".pdf"), (void*)"application/pdf");
	s2_extended->Insert((const char *)".pgp", strlen(".pgp"), (void*)"application/pgp-encrypted");
	s2_extended->Insert((const char *)".asc", strlen(".asc"), (void*)"application/pgp-signature");
	s2_extended->Insert((const char *)".sig", strlen(".sig"), (void*)"application/pgp-signature");
	s2_extended->Insert((const char *)".prf", strlen(".prf"), (void*)"application/pics-rules");
	s2_extended->Insert((const char *)".p10", strlen(".p10"), (void*)"application/pkcs10");
	s2_extended->Insert((const char *)".p7m", strlen(".p7m"), (void*)"application/pkcs7-mime");
	s2_extended->Insert((const char *)".p7c", strlen(".p7c"), (void*)"application/pkcs7-mime");
	s2_extended->Insert((const char *)".p7s", strlen(".p7s"), (void*)"application/pkcs7-signature");
	s2_extended->Insert((const char *)".p8", strlen(".p8"), (void*)"application/pkcs8");
	s2_extended->Insert((const char *)".ac", strlen(".ac"), (void*)"application/pkix-attr-cert");
	s2_extended->Insert((const char *)".cer", strlen(".cer"), (void*)"application/pkix-cert");
	s2_extended->Insert((const char *)".crl", strlen(".crl"), (void*)"application/pkix-crl");
	s2_extended->Insert((const char *)".pkipath", strlen(".pkipath"), (void*)"application/pkix-pkipath");
	s2_extended->Insert((const char *)".pki", strlen(".pki"), (void*)"application/pkixcmp");
	s2_extended->Insert((const char *)".pls", strlen(".pls"), (void*)"application/pls+xml");
	s2_extended->Insert((const char *)".ai", strlen(".ai"), (void*)"application/postscript");
	s2_extended->Insert((const char *)".eps", strlen(".eps"), (void*)"application/postscript");
	s2_extended->Insert((const char *)".ps", strlen(".ps"), (void*)"application/postscript");
	s2_extended->Insert((const char *)".cww", strlen(".cww"), (void*)"application/prs.cww");
	s2_extended->Insert((const char *)".pskcxml", strlen(".pskcxml"), (void*)"application/pskc+xml");
	s2_extended->Insert((const char *)".rdf", strlen(".rdf"), (void*)"application/rdf+xml");
	s2_extended->Insert((const char *)".rif", strlen(".rif"), (void*)"application/reginfo+xml");
	s2_extended->Insert((const char *)".rnc", strlen(".rnc"), (void*)"application/relax-ng-compact-syntax");
	s2_extended->Insert((const char *)".rl", strlen(".rl"), (void*)"application/resource-lists+xml");
	s2_extended->Insert((const char *)".rld", strlen(".rld"), (void*)"application/resource-lists-diff+xml");
	s2_extended->Insert((const char *)".rs", strlen(".rs"), (void*)"application/rls-services+xml");
	s2_extended->Insert((const char *)".gbr", strlen(".gbr"), (void*)"application/rpki-ghostbusters");
	s2_extended->Insert((const char *)".mft", strlen(".mft"), (void*)"application/rpki-manifest");
	s2_extended->Insert((const char *)".roa", strlen(".roa"), (void*)"application/rpki-roa");
	s2_extended->Insert((const char *)".rsd", strlen(".rsd"), (void*)"application/rsd+xml");
	s2_extended->Insert((const char *)".rss", strlen(".rss"), (void*)"application/rss+xml");
	s2_extended->Insert((const char *)".rtf", strlen(".rtf"), (void*)"application/rtf");
	s2_extended->Insert((const char *)".sbml", strlen(".sbml"), (void*)"application/sbml+xml");
	s2_extended->Insert((const char *)".scq", strlen(".scq"), (void*)"application/scvp-cv-request");
	s2_extended->Insert((const char *)".scs", strlen(".scs"), (void*)"application/scvp-cv-response");
	s2_extended->Insert((const char *)".spq", strlen(".spq"), (void*)"application/scvp-vp-request");
	s2_extended->Insert((const char *)".spp", strlen(".spp"), (void*)"application/scvp-vp-response");
	s2_extended->Insert((const char *)".sdp", strlen(".sdp"), (void*)"application/sdp");
	s2_extended->Insert((const char *)".setpay", strlen(".setpay"), (void*)"application/set-payment-initiation");
	s2_extended->Insert((const char *)".setreg", strlen(".setreg"), (void*)"application/set-registration-initiation");
	s2_extended->Insert((const char *)".shf", strlen(".shf"), (void*)"application/shf+xml");
	s2_extended->Insert((const char *)".smi", strlen(".smi"), (void*)"application/smil+xml");
	s2_extended->Insert((const char *)".smil", strlen(".smil"), (void*)"application/smil+xml");
	s2_extended->Insert((const char *)".rq", strlen(".rq"), (void*)"application/sparql-query");
	s2_extended->Insert((const char *)".srx", strlen(".srx"), (void*)"application/sparql-results+xml");
	s2_extended->Insert((const char *)".gram", strlen(".gram"), (void*)"application/srgs");
	s2_extended->Insert((const char *)".grxml", strlen(".grxml"), (void*)"application/srgs+xml");
	s2_extended->Insert((const char *)".sru", strlen(".sru"), (void*)"application/sru+xml");
	s2_extended->Insert((const char *)".ssdl", strlen(".ssdl"), (void*)"application/ssdl+xml");
	s2_extended->Insert((const char *)".ssml", strlen(".ssml"), (void*)"application/ssml+xml");
	s2_extended->Insert((const char *)".tei", strlen(".tei"), (void*)"application/tei+xml");
	s2_extended->Insert((const char *)".teicorpus", strlen(".teicorpus"), (void*)"application/tei+xml");
	s2_extended->Insert((const char *)".tfi", strlen(".tfi"), (void*)"application/thraud+xml");
	s2_extended->Insert((const char *)".tsd", strlen(".tsd"), (void*)"application/timestamped-data");
	s2_extended->Insert((const char *)".plb", strlen(".plb"), (void*)"application/vnd.3gpp.pic-bw-large");
	s2_extended->Insert((const char *)".psb", strlen(".psb"), (void*)"application/vnd.3gpp.pic-bw-small");
	s2_extended->Insert((const char *)".pvb", strlen(".pvb"), (void*)"application/vnd.3gpp.pic-bw-var");
	s2_extended->Insert((const char *)".tcap", strlen(".tcap"), (void*)"application/vnd.3gpp2.tcap");
	s2_extended->Insert((const char *)".pwn", strlen(".pwn"), (void*)"application/vnd.3m.post-it-notes");
	s2_extended->Insert((const char *)".aso", strlen(".aso"), (void*)"application/vnd.accpac.simply.aso");
	s2_extended->Insert((const char *)".imp", strlen(".imp"), (void*)"application/vnd.accpac.simply.imp");
	s2_extended->Insert((const char *)".acu", strlen(".acu"), (void*)"application/vnd.acucobol");
	s2_extended->Insert((const char *)".atc", strlen(".atc"), (void*)"application/vnd.acucorp");
	s2_extended->Insert((const char *)".acutc", strlen(".acutc"), (void*)"application/vnd.acucorp");
	s2_extended->Insert((const char *)".air", strlen(".air"), (void*)"application/vnd.adobe.air-application-installer-package+zip");
	s2_extended->Insert((const char *)".fcdt", strlen(".fcdt"), (void*)"application/vnd.adobe.formscentral.fcdt");
	s2_extended->Insert((const char *)".fxp", strlen(".fxp"), (void*)"application/vnd.adobe.fxp");
	s2_extended->Insert((const char *)".fxpl", strlen(".fxpl"), (void*)"application/vnd.adobe.fxp");
	s2_extended->Insert((const char *)".xdp", strlen(".xdp"), (void*)"application/vnd.adobe.xdp+xml");
	s2_extended->Insert((const char *)".xfdf", strlen(".xfdf"), (void*)"application/vnd.adobe.xfdf");
	s2_extended->Insert((const char *)".ahead", strlen(".ahead"), (void*)"application/vnd.ahead.space");
	s2_extended->Insert((const char *)".azf", strlen(".azf"), (void*)"application/vnd.airzip.filesecure.azf");
	s2_extended->Insert((const char *)".azs", strlen(".azs"), (void*)"application/vnd.airzip.filesecure.azs");
	s2_extended->Insert((const char *)".azw", strlen(".azw"), (void*)"application/vnd.amazon.ebook");
	s2_extended->Insert((const char *)".acc", strlen(".acc"), (void*)"application/vnd.americandynamics.acc");
	s2_extended->Insert((const char *)".ami", strlen(".ami"), (void*)"application/vnd.amiga.ami");
	s2_extended->Insert((const char *)".apk", strlen(".apk"), (void*)"application/vnd.android.package-archive");
	s2_extended->Insert((const char *)".cii", strlen(".cii"), (void*)"application/vnd.anser-web-certificate-issue-initiation");
	s2_extended->Insert((const char *)".fti", strlen(".fti"), (void*)"application/vnd.anser-web-funds-transfer-initiation");
	s2_extended->Insert((const char *)".atx", strlen(".atx"), (void*)"application/vnd.antix.game-component");
	s2_extended->Insert((const char *)".mpkg", strlen(".mpkg"), (void*)"application/vnd.apple.installer+xml");
	s2_extended->Insert((const char *)".m3u8", strlen(".m3u8"), (void*)"application/vnd.apple.mpegurl");
	s2_extended->Insert((const char *)".swi", strlen(".swi"), (void*)"application/vnd.aristanetworks.swi");
	s2_extended->Insert((const char *)".iota", strlen(".iota"), (void*)"application/vnd.astraea-software.iota");
	s2_extended->Insert((const char *)".aep", strlen(".aep"), (void*)"application/vnd.audiograph");
	s2_extended->Insert((const char *)".mpm", strlen(".mpm"), (void*)"application/vnd.blueice.multipass");
	s2_extended->Insert((const char *)".bmi", strlen(".bmi"), (void*)"application/vnd.bmi");
	s2_extended->Insert((const char *)".rep", strlen(".rep"), (void*)"application/vnd.businessobjects");
	s2_extended->Insert((const char *)".cdxml", strlen(".cdxml"), (void*)"application/vnd.chemdraw+xml");
	s2_extended->Insert((const char *)".mmd", strlen(".mmd"), (void*)"application/vnd.chipnuts.karaoke-mmd");
	s2_extended->Insert((const char *)".cdy", strlen(".cdy"), (void*)"application/vnd.cinderella");
	s2_extended->Insert((const char *)".cla", strlen(".cla"), (void*)"application/vnd.claymore");
	s2_extended->Insert((const char *)".rp9", strlen(".rp9"), (void*)"application/vnd.cloanto.rp9");
	s2_extended->Insert((const char *)".c4g", strlen(".c4g"), (void*)"application/vnd.clonk.c4group");
	s2_extended->Insert((const char *)".c4d", strlen(".c4d"), (void*)"application/vnd.clonk.c4group");
	s2_extended->Insert((const char *)".c4f", strlen(".c4f"), (void*)"application/vnd.clonk.c4group");
	s2_extended->Insert((const char *)".c4p", strlen(".c4p"), (void*)"application/vnd.clonk.c4group");
	s2_extended->Insert((const char *)".c4u", strlen(".c4u"), (void*)"application/vnd.clonk.c4group");
	s2_extended->Insert((const char *)".c11amc", strlen(".c11amc"), (void*)"application/vnd.cluetrust.cartomobile-config");
	s2_extended->Insert((const char *)".c11amz", strlen(".c11amz"), (void*)"application/vnd.cluetrust.cartomobile-config-pkg");
	s2_extended->Insert((const char *)".csp", strlen(".csp"), (void*)"application/vnd.commonspace");
	s2_extended->Insert((const char *)".cdbcmsg", strlen(".cdbcmsg"), (void*)"application/vnd.contact.cmsg");
	s2_extended->Insert((const char *)".cmc", strlen(".cmc"), (void*)"application/vnd.cosmocaller");
	s2_extended->Insert((const char *)".clkx", strlen(".clkx"), (void*)"application/vnd.crick.clicker");
	s2_extended->Insert((const char *)".clkk", strlen(".clkk"), (void*)"application/vnd.crick.clicker.keyboard");
	s2_extended->Insert((const char *)".clkp", strlen(".clkp"), (void*)"application/vnd.crick.clicker.palette");
	s2_extended->Insert((const char *)".clkt", strlen(".clkt"), (void*)"application/vnd.crick.clicker.template");
	s2_extended->Insert((const char *)".clkw", strlen(".clkw"), (void*)"application/vnd.crick.clicker.wordbank");
	s2_extended->Insert((const char *)".wbs", strlen(".wbs"), (void*)"application/vnd.criticaltools.wbs+xml");
	s2_extended->Insert((const char *)".pml", strlen(".pml"), (void*)"application/vnd.ctc-posml");
	s2_extended->Insert((const char *)".ppd", strlen(".ppd"), (void*)"application/vnd.cups-ppd");
	s2_extended->Insert((const char *)".car", strlen(".car"), (void*)"application/vnd.curl.car");
	s2_extended->Insert((const char *)".pcurl", strlen(".pcurl"), (void*)"application/vnd.curl.pcurl");
	s2_extended->Insert((const char *)".dart", strlen(".dart"), (void*)"application/vnd.dart");
	s2_extended->Insert((const char *)".rdz", strlen(".rdz"), (void*)"application/vnd.data-vision.rdz");
	s2_extended->Insert((const char *)".uvf", strlen(".uvf"), (void*)"application/vnd.dece.data");
	s2_extended->Insert((const char *)".uvvf", strlen(".uvvf"), (void*)"application/vnd.dece.data");
	s2_extended->Insert((const char *)".uvd", strlen(".uvd"), (void*)"application/vnd.dece.data");
	s2_extended->Insert((const char *)".uvvd", strlen(".uvvd"), (void*)"application/vnd.dece.data");
	s2_extended->Insert((const char *)".uvt", strlen(".uvt"), (void*)"application/vnd.dece.ttml+xml");
	s2_extended->Insert((const char *)".uvvt", strlen(".uvvt"), (void*)"application/vnd.dece.ttml+xml");
	s2_extended->Insert((const char *)".uvx", strlen(".uvx"), (void*)"application/vnd.dece.unspecified");
	s2_extended->Insert((const char *)".uvvx", strlen(".uvvx"), (void*)"application/vnd.dece.unspecified");
	s2_extended->Insert((const char *)".uvz", strlen(".uvz"), (void*)"application/vnd.dece.zip");
	s2_extended->Insert((const char *)".uvvz", strlen(".uvvz"), (void*)"application/vnd.dece.zip");
	s2_extended->Insert((const char *)".fe_launch", strlen(".fe_launch"), (void*)"application/vnd.denovo.fcselayout-link");
	s2_extended->Insert((const char *)".dna", strlen(".dna"), (void*)"application/vnd.dna");
	s2_extended->Insert((const char *)".mlp", strlen(".mlp"), (void*)"application/vnd.dolby.mlp");
	s2_extended->Insert((const char *)".dpg", strlen(".dpg"), (void*)"application/vnd.dpgraph");
	s2_extended->Insert((const char *)".dfac", strlen(".dfac"), (void*)"application/vnd.dreamfactory");
	s2_extended->Insert((const char *)".kpxx", strlen(".kpxx"), (void*)"application/vnd.ds-keypoint");
	s2_extended->Insert((const char *)".ait", strlen(".ait"), (void*)"application/vnd.dvb.ait");
	s2_extended->Insert((const char *)".svc", strlen(".svc"), (void*)"application/vnd.dvb.service");
	s2_extended->Insert((const char *)".geo", strlen(".geo"), (void*)"application/vnd.dynageo");
	s2_extended->Insert((const char *)".mag", strlen(".mag"), (void*)"application/vnd.ecowin.chart");
	s2_extended->Insert((const char *)".nml", strlen(".nml"), (void*)"application/vnd.enliven");
	s2_extended->Insert((const char *)".esf", strlen(".esf"), (void*)"application/vnd.epson.esf");
	s2_extended->Insert((const char *)".msf", strlen(".msf"), (void*)"application/vnd.epson.msf");
	s2_extended->Insert((const char *)".qam", strlen(".qam"), (void*)"application/vnd.epson.quickanime");
	s2_extended->Insert((const char *)".slt", strlen(".slt"), (void*)"application/vnd.epson.salt");
	s2_extended->Insert((const char *)".ssf", strlen(".ssf"), (void*)"application/vnd.epson.ssf");
	s2_extended->Insert((const char *)".es3", strlen(".es3"), (void*)"application/vnd.eszigno3+xml");
	s2_extended->Insert((const char *)".et3", strlen(".et3"), (void*)"application/vnd.eszigno3+xml");
	s2_extended->Insert((const char *)".ez2", strlen(".ez2"), (void*)"application/vnd.ezpix-album");
	s2_extended->Insert((const char *)".ez3", strlen(".ez3"), (void*)"application/vnd.ezpix-package");
	s2_extended->Insert((const char *)".fdf", strlen(".fdf"), (void*)"application/vnd.fdf");
	s2_extended->Insert((const char *)".mseed", strlen(".mseed"), (void*)"application/vnd.fdsn.mseed");
	s2_extended->Insert((const char *)".seed", strlen(".seed"), (void*)"application/vnd.fdsn.seed");
	s2_extended->Insert((const char *)".dataless", strlen(".dataless"), (void*)"application/vnd.fdsn.seed");
	s2_extended->Insert((const char *)".gph", strlen(".gph"), (void*)"application/vnd.flographit");
	s2_extended->Insert((const char *)".ftc", strlen(".ftc"), (void*)"application/vnd.fluxtime.clip");
	s2_extended->Insert((const char *)".fm", strlen(".fm"), (void*)"application/vnd.framemaker");
	s2_extended->Insert((const char *)".frame", strlen(".frame"), (void*)"application/vnd.framemaker");
	s2_extended->Insert((const char *)".maker", strlen(".maker"), (void*)"application/vnd.framemaker");
	s2_extended->Insert((const char *)".book", strlen(".book"), (void*)"application/vnd.framemaker");
	s2_extended->Insert((const char *)".fnc", strlen(".fnc"), (void*)"application/vnd.frogans.fnc");
	s2_extended->Insert((const char *)".ltf", strlen(".ltf"), (void*)"application/vnd.frogans.ltf");
	s2_extended->Insert((const char *)".fsc", strlen(".fsc"), (void*)"application/vnd.fsc.weblaunch");
	s2_extended->Insert((const char *)".oas", strlen(".oas"), (void*)"application/vnd.fujitsu.oasys");
	s2_extended->Insert((const char *)".oa2", strlen(".oa2"), (void*)"application/vnd.fujitsu.oasys2");
	s2_extended->Insert((const char *)".oa3", strlen(".oa3"), (void*)"application/vnd.fujitsu.oasys3");
	s2_extended->Insert((const char *)".fg5", strlen(".fg5"), (void*)"application/vnd.fujitsu.oasysgp");
	s2_extended->Insert((const char *)".bh2", strlen(".bh2"), (void*)"application/vnd.fujitsu.oasysprs");
	s2_extended->Insert((const char *)".ddd", strlen(".ddd"), (void*)"application/vnd.fujixerox.ddd");
	s2_extended->Insert((const char *)".xdw", strlen(".xdw"), (void*)"application/vnd.fujixerox.docuworks");
	s2_extended->Insert((const char *)".xbd", strlen(".xbd"), (void*)"application/vnd.fujixerox.docuworks.binder");
	s2_extended->Insert((const char *)".fzs", strlen(".fzs"), (void*)"application/vnd.fuzzysheet");
	s2_extended->Insert((const char *)".txd", strlen(".txd"), (void*)"application/vnd.genomatix.tuxedo");
	s2_extended->Insert((const char *)".ggb", strlen(".ggb"), (void*)"application/vnd.geogebra.file");
	s2_extended->Insert((const char *)".ggt", strlen(".ggt"), (void*)"application/vnd.geogebra.tool");
	s2_extended->Insert((const char *)".gex", strlen(".gex"), (void*)"application/vnd.geometry-explorer");
	s2_extended->Insert((const char *)".gre", strlen(".gre"), (void*)"application/vnd.geometry-explorer");
	s2_extended->Insert((const char *)".gxt", strlen(".gxt"), (void*)"application/vnd.geonext");
	s2_extended->Insert((const char *)".g2w", strlen(".g2w"), (void*)"application/vnd.geoplan");
	s2_extended->Insert((const char *)".g3w", strlen(".g3w"), (void*)"application/vnd.geospace");
	s2_extended->Insert((const char *)".gmx", strlen(".gmx"), (void*)"application/vnd.gmx");
	s2_extended->Insert((const char *)".kml", strlen(".kml"), (void*)"application/vnd.google-earth.kml+xml");
	s2_extended->Insert((const char *)".kmz", strlen(".kmz"), (void*)"application/vnd.google-earth.kmz");
	s2_extended->Insert((const char *)".gqf", strlen(".gqf"), (void*)"application/vnd.grafeq");
	s2_extended->Insert((const char *)".gqs", strlen(".gqs"), (void*)"application/vnd.grafeq");
	s2_extended->Insert((const char *)".gac", strlen(".gac"), (void*)"application/vnd.groove-account");
	s2_extended->Insert((const char *)".ghf", strlen(".ghf"), (void*)"application/vnd.groove-help");
	s2_extended->Insert((const char *)".gim", strlen(".gim"), (void*)"application/vnd.groove-identity-message");
	s2_extended->Insert((const char *)".grv", strlen(".grv"), (void*)"application/vnd.groove-injector");
	s2_extended->Insert((const char *)".gtm", strlen(".gtm"), (void*)"application/vnd.groove-tool-message");
	s2_extended->Insert((const char *)".tpl", strlen(".tpl"), (void*)"application/vnd.groove-tool-template");
	s2_extended->Insert((const char *)".vcg", strlen(".vcg"), (void*)"application/vnd.groove-vcard");
	s2_extended->Insert((const char *)".hal", strlen(".hal"), (void*)"application/vnd.hal+xml");
	s2_extended->Insert((const char *)".zmm", strlen(".zmm"), (void*)"application/vnd.handheld-entertainment+xml");
	s2_extended->Insert((const char *)".hbci", strlen(".hbci"), (void*)"application/vnd.hbci");
	s2_extended->Insert((const char *)".les", strlen(".les"), (void*)"application/vnd.hhe.lesson-player");
	s2_extended->Insert((const char *)".hpgl", strlen(".hpgl"), (void*)"application/vnd.hp-hpgl");
	s2_extended->Insert((const char *)".hpid", strlen(".hpid"), (void*)"application/vnd.hp-hpid");
	s2_extended->Insert((const char *)".hps", strlen(".hps"), (void*)"application/vnd.hp-hps");
	s2_extended->Insert((const char *)".jlt", strlen(".jlt"), (void*)"application/vnd.hp-jlyt");
	s2_extended->Insert((const char *)".pcl", strlen(".pcl"), (void*)"application/vnd.hp-pcl");
	s2_extended->Insert((const char *)".pclxl", strlen(".pclxl"), (void*)"application/vnd.hp-pclxl");
	s2_extended->Insert((const char *)".sfd-hdstx", strlen(".sfd-hdstx"), (void*)"application/vnd.hydrostatix.sof-data");
	s2_extended->Insert((const char *)".mpy", strlen(".mpy"), (void*)"application/vnd.ibm.minipay");
	s2_extended->Insert((const char *)".afp", strlen(".afp"), (void*)"application/vnd.ibm.modcap");
	s2_extended->Insert((const char *)".listafp", strlen(".listafp"), (void*)"application/vnd.ibm.modcap");
	s2_extended->Insert((const char *)".list3820", strlen(".list3820"), (void*)"application/vnd.ibm.modcap");
	s2_extended->Insert((const char *)".irm", strlen(".irm"), (void*)"application/vnd.ibm.rights-management");
	s2_extended->Insert((const char *)".sc", strlen(".sc"), (void*)"application/vnd.ibm.secure-container");
	s2_extended->Insert((const char *)".icc", strlen(".icc"), (void*)"application/vnd.iccprofile");
	s2_extended->Insert((const char *)".icm", strlen(".icm"), (void*)"application/vnd.iccprofile");
	s2_extended->Insert((const char *)".igl", strlen(".igl"), (void*)"application/vnd.igloader");
	s2_extended->Insert((const char *)".ivp", strlen(".ivp"), (void*)"application/vnd.immervision-ivp");
	s2_extended->Insert((const char *)".ivu", strlen(".ivu"), (void*)"application/vnd.immervision-ivu");
	s2_extended->Insert((const char *)".igm", strlen(".igm"), (void*)"application/vnd.insors.igm");
	s2_extended->Insert((const char *)".xpw", strlen(".xpw"), (void*)"application/vnd.intercon.formnet");
	s2_extended->Insert((const char *)".xpx", strlen(".xpx"), (void*)"application/vnd.intercon.formnet");
	s2_extended->Insert((const char *)".i2g", strlen(".i2g"), (void*)"application/vnd.intergeo");
	s2_extended->Insert((const char *)".qbo", strlen(".qbo"), (void*)"application/vnd.intu.qbo");
	s2_extended->Insert((const char *)".qfx", strlen(".qfx"), (void*)"application/vnd.intu.qfx");
	s2_extended->Insert((const char *)".rcprofile", strlen(".rcprofile"), (void*)"application/vnd.ipunplugged.rcprofile");
	s2_extended->Insert((const char *)".irp", strlen(".irp"), (void*)"application/vnd.irepository.package+xml");
	s2_extended->Insert((const char *)".xpr", strlen(".xpr"), (void*)"application/vnd.is-xpr");
	s2_extended->Insert((const char *)".fcs", strlen(".fcs"), (void*)"application/vnd.isac.fcs");
	s2_extended->Insert((const char *)".jam", strlen(".jam"), (void*)"application/vnd.jam");
	s2_extended->Insert((const char *)".rms", strlen(".rms"), (void*)"application/vnd.jcp.javame.midlet-rms");
	s2_extended->Insert((const char *)".jisp", strlen(".jisp"), (void*)"application/vnd.jisp");
	s2_extended->Insert((const char *)".joda", strlen(".joda"), (void*)"application/vnd.joost.joda-archive");
	s2_extended->Insert((const char *)".ktz", strlen(".ktz"), (void*)"application/vnd.kahootz");
	s2_extended->Insert((const char *)".ktr", strlen(".ktr"), (void*)"application/vnd.kahootz");
	s2_extended->Insert((const char *)".karbon", strlen(".karbon"), (void*)"application/vnd.kde.karbon");
	s2_extended->Insert((const char *)".chrt", strlen(".chrt"), (void*)"application/vnd.kde.kchart");
	s2_extended->Insert((const char *)".kfo", strlen(".kfo"), (void*)"application/vnd.kde.kformula");
	s2_extended->Insert((const char *)".flw", strlen(".flw"), (void*)"application/vnd.kde.kivio");
	s2_extended->Insert((const char *)".kon", strlen(".kon"), (void*)"application/vnd.kde.kontour");
	s2_extended->Insert((const char *)".kpr", strlen(".kpr"), (void*)"application/vnd.kde.kpresenter");
	s2_extended->Insert((const char *)".kpt", strlen(".kpt"), (void*)"application/vnd.kde.kpresenter");
	s2_extended->Insert((const char *)".ksp", strlen(".ksp"), (void*)"application/vnd.kde.kspread");
	s2_extended->Insert((const char *)".kwd", strlen(".kwd"), (void*)"application/vnd.kde.kword");
	s2_extended->Insert((const char *)".kwt", strlen(".kwt"), (void*)"application/vnd.kde.kword");
	s2_extended->Insert((const char *)".htke", strlen(".htke"), (void*)"application/vnd.kenameaapp");
	s2_extended->Insert((const char *)".kia", strlen(".kia"), (void*)"application/vnd.kidspiration");
	s2_extended->Insert((const char *)".kne", strlen(".kne"), (void*)"application/vnd.kinar");
	s2_extended->Insert((const char *)".knp", strlen(".knp"), (void*)"application/vnd.kinar");
	s2_extended->Insert((const char *)".skp", strlen(".skp"), (void*)"application/vnd.koan");
	s2_extended->Insert((const char *)".skd", strlen(".skd"), (void*)"application/vnd.koan");
	s2_extended->Insert((const char *)".skt", strlen(".skt"), (void*)"application/vnd.koan");
	s2_extended->Insert((const char *)".skm", strlen(".skm"), (void*)"application/vnd.koan");
	s2_extended->Insert((const char *)".sse", strlen(".sse"), (void*)"application/vnd.kodak-descriptor");
	s2_extended->Insert((const char *)".lasxml", strlen(".lasxml"), (void*)"application/vnd.las.las+xml");
	s2_extended->Insert((const char *)".lbd", strlen(".lbd"), (void*)"application/vnd.llamagraphics.life-balance.desktop");
	s2_extended->Insert((const char *)".lbe", strlen(".lbe"), (void*)"application/vnd.llamagraphics.life-balance.exchange+xml");
	s2_extended->Insert((const char *)".123", strlen(".123"), (void*)"application/vnd.lotus-1-2-3");
	s2_extended->Insert((const char *)".apr", strlen(".apr"), (void*)"application/vnd.lotus-approach");
	s2_extended->Insert((const char *)".pre", strlen(".pre"), (void*)"application/vnd.lotus-freelance");
	s2_extended->Insert((const char *)".nsf", strlen(".nsf"), (void*)"application/vnd.lotus-notes");
	s2_extended->Insert((const char *)".org", strlen(".org"), (void*)"application/vnd.lotus-organizer");
	s2_extended->Insert((const char *)".scm", strlen(".scm"), (void*)"application/vnd.lotus-screencam");
	s2_extended->Insert((const char *)".lwp", strlen(".lwp"), (void*)"application/vnd.lotus-wordpro");
	s2_extended->Insert((const char *)".portpkg", strlen(".portpkg"), (void*)"application/vnd.macports.portpkg");
	s2_extended->Insert((const char *)".mcd", strlen(".mcd"), (void*)"application/vnd.mcd");
	s2_extended->Insert((const char *)".mc1", strlen(".mc1"), (void*)"application/vnd.medcalcdata");
	s2_extended->Insert((const char *)".cdkey", strlen(".cdkey"), (void*)"application/vnd.mediastation.cdkey");
	s2_extended->Insert((const char *)".mwf", strlen(".mwf"), (void*)"application/vnd.mfer");
	s2_extended->Insert((const char *)".mfm", strlen(".mfm"), (void*)"application/vnd.mfmp");
	s2_extended->Insert((const char *)".flo", strlen(".flo"), (void*)"application/vnd.micrografx.flo");
	s2_extended->Insert((const char *)".igx", strlen(".igx"), (void*)"application/vnd.micrografx.igx");
	s2_extended->Insert((const char *)".mif", strlen(".mif"), (void*)"application/vnd.mif");
	s2_extended->Insert((const char *)".daf", strlen(".daf"), (void*)"application/vnd.mobius.daf");
	s2_extended->Insert((const char *)".dis", strlen(".dis"), (void*)"application/vnd.mobius.dis");
	s2_extended->Insert((const char *)".mbk", strlen(".mbk"), (void*)"application/vnd.mobius.mbk");
	s2_extended->Insert((const char *)".mqy", strlen(".mqy"), (void*)"application/vnd.mobius.mqy");
	s2_extended->Insert((const char *)".msl", strlen(".msl"), (void*)"application/vnd.mobius.msl");
	s2_extended->Insert((const char *)".plc", strlen(".plc"), (void*)"application/vnd.mobius.plc");
	s2_extended->Insert((const char *)".txf", strlen(".txf"), (void*)"application/vnd.mobius.txf");
	s2_extended->Insert((const char *)".mpn", strlen(".mpn"), (void*)"application/vnd.mophun.application");
	s2_extended->Insert((const char *)".mpc", strlen(".mpc"), (void*)"application/vnd.mophun.certificate");
	s2_extended->Insert((const char *)".xul", strlen(".xul"), (void*)"application/vnd.mozilla.xul+xml");
	s2_extended->Insert((const char *)".cil", strlen(".cil"), (void*)"application/vnd.ms-artgalry");
	s2_extended->Insert((const char *)".cab", strlen(".cab"), (void*)"application/vnd.ms-cab-compressed");
	s2_extended->Insert((const char *)".xls", strlen(".xls"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xlm", strlen(".xlm"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xla", strlen(".xla"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xlc", strlen(".xlc"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xlt", strlen(".xlt"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xlw", strlen(".xlw"), (void*)"application/vnd.ms-excel");
	s2_extended->Insert((const char *)".xlam", strlen(".xlam"), (void*)"application/vnd.ms-excel.addin.macroenabled.12");
	s2_extended->Insert((const char *)".xlsb", strlen(".xlsb"), (void*)"application/vnd.ms-excel.sheet.binary.macroenabled.12");
	s2_extended->Insert((const char *)".xlsm", strlen(".xlsm"), (void*)"application/vnd.ms-excel.sheet.macroenabled.12");
	s2_extended->Insert((const char *)".xltm", strlen(".xltm"), (void*)"application/vnd.ms-excel.template.macroenabled.12");
	s2_extended->Insert((const char *)".eot", strlen(".eot"), (void*)"application/vnd.ms-fontobject");
	s2_extended->Insert((const char *)".chm", strlen(".chm"), (void*)"application/vnd.ms-htmlhelp");
	s2_extended->Insert((const char *)".ims", strlen(".ims"), (void*)"application/vnd.ms-ims");
	s2_extended->Insert((const char *)".lrm", strlen(".lrm"), (void*)"application/vnd.ms-lrm");
	s2_extended->Insert((const char *)".thmx", strlen(".thmx"), (void*)"application/vnd.ms-officetheme");
	s2_extended->Insert((const char *)".cat", strlen(".cat"), (void*)"application/vnd.ms-pki.seccat");
	s2_extended->Insert((const char *)".stl", strlen(".stl"), (void*)"application/vnd.ms-pki.stl");
	s2_extended->Insert((const char *)".ppt", strlen(".ppt"), (void*)"application/vnd.ms-powerpoint");
	s2_extended->Insert((const char *)".pps", strlen(".pps"), (void*)"application/vnd.ms-powerpoint");
	s2_extended->Insert((const char *)".pot", strlen(".pot"), (void*)"application/vnd.ms-powerpoint");
	s2_extended->Insert((const char *)".ppam", strlen(".ppam"), (void*)"application/vnd.ms-powerpoint.addin.macroenabled.12");
	s2_extended->Insert((const char *)".pptm", strlen(".pptm"), (void*)"application/vnd.ms-powerpoint.presentation.macroenabled.12");
	s2_extended->Insert((const char *)".sldm", strlen(".sldm"), (void*)"application/vnd.ms-powerpoint.slide.macroenabled.12");
	s2_extended->Insert((const char *)".ppsm", strlen(".ppsm"), (void*)"application/vnd.ms-powerpoint.slideshow.macroenabled.12");
	s2_extended->Insert((const char *)".potm", strlen(".potm"), (void*)"application/vnd.ms-powerpoint.template.macroenabled.12");
	s2_extended->Insert((const char *)".mpp", strlen(".mpp"), (void*)"application/vnd.ms-project");
	s2_extended->Insert((const char *)".mpt", strlen(".mpt"), (void*)"application/vnd.ms-project");
	s2_extended->Insert((const char *)".docm", strlen(".docm"), (void*)"application/vnd.ms-word.document.macroenabled.12");
	s2_extended->Insert((const char *)".dotm", strlen(".dotm"), (void*)"application/vnd.ms-word.template.macroenabled.12");
	s2_extended->Insert((const char *)".wps", strlen(".wps"), (void*)"application/vnd.ms-works");
	s2_extended->Insert((const char *)".wks", strlen(".wks"), (void*)"application/vnd.ms-works");
	s2_extended->Insert((const char *)".wcm", strlen(".wcm"), (void*)"application/vnd.ms-works");
	s2_extended->Insert((const char *)".wdb", strlen(".wdb"), (void*)"application/vnd.ms-works");
	s2_extended->Insert((const char *)".wpl", strlen(".wpl"), (void*)"application/vnd.ms-wpl");
	s2_extended->Insert((const char *)".xps", strlen(".xps"), (void*)"application/vnd.ms-xpsdocument");
	s2_extended->Insert((const char *)".mseq", strlen(".mseq"), (void*)"application/vnd.mseq");
	s2_extended->Insert((const char *)".mus", strlen(".mus"), (void*)"application/vnd.musician");
	s2_extended->Insert((const char *)".msty", strlen(".msty"), (void*)"application/vnd.muvee.style");
	s2_extended->Insert((const char *)".taglet", strlen(".taglet"), (void*)"application/vnd.mynfc");
	s2_extended->Insert((const char *)".nlu", strlen(".nlu"), (void*)"application/vnd.neurolanguage.nlu");
	s2_extended->Insert((const char *)".ntf", strlen(".ntf"), (void*)"application/vnd.nitf");
	s2_extended->Insert((const char *)".nitf", strlen(".nitf"), (void*)"application/vnd.nitf");
	s2_extended->Insert((const char *)".nnd", strlen(".nnd"), (void*)"application/vnd.noblenet-directory");
	s2_extended->Insert((const char *)".nns", strlen(".nns"), (void*)"application/vnd.noblenet-sealer");
	s2_extended->Insert((const char *)".nnw", strlen(".nnw"), (void*)"application/vnd.noblenet-web");
	s2_extended->Insert((const char *)".ngdat", strlen(".ngdat"), (void*)"application/vnd.nokia.n-gage.data");
	s2_extended->Insert((const char *)".n-gage", strlen(".n-gage"), (void*)"application/vnd.nokia.n-gage.symbian.install");
	s2_extended->Insert((const char *)".rpst", strlen(".rpst"), (void*)"application/vnd.nokia.radio-preset");
	s2_extended->Insert((const char *)".rpss", strlen(".rpss"), (void*)"application/vnd.nokia.radio-presets");
	s2_extended->Insert((const char *)".edm", strlen(".edm"), (void*)"application/vnd.novadigm.edm");
	s2_extended->Insert((const char *)".edx", strlen(".edx"), (void*)"application/vnd.novadigm.edx");
	s2_extended->Insert((const char *)".ext", strlen(".ext"), (void*)"application/vnd.novadigm.ext");
	s2_extended->Insert((const char *)".odc", strlen(".odc"), (void*)"application/vnd.oasis.opendocument.chart");
	s2_extended->Insert((const char *)".otc", strlen(".otc"), (void*)"application/vnd.oasis.opendocument.chart-template");
	s2_extended->Insert((const char *)".odb", strlen(".odb"), (void*)"application/vnd.oasis.opendocument.database");
	s2_extended->Insert((const char *)".odf", strlen(".odf"), (void*)"application/vnd.oasis.opendocument.formula");
	s2_extended->Insert((const char *)".odft", strlen(".odft"), (void*)"application/vnd.oasis.opendocument.formula-template");
	s2_extended->Insert((const char *)".odg", strlen(".odg"), (void*)"application/vnd.oasis.opendocument.graphics");
	s2_extended->Insert((const char *)".otg", strlen(".otg"), (void*)"application/vnd.oasis.opendocument.graphics-template");
	s2_extended->Insert((const char *)".odi", strlen(".odi"), (void*)"application/vnd.oasis.opendocument.image");
	s2_extended->Insert((const char *)".oti", strlen(".oti"), (void*)"application/vnd.oasis.opendocument.image-template");
	s2_extended->Insert((const char *)".odp", strlen(".odp"), (void*)"application/vnd.oasis.opendocument.presentation");
	s2_extended->Insert((const char *)".otp", strlen(".otp"), (void*)"application/vnd.oasis.opendocument.presentation-template");
	s2_extended->Insert((const char *)".ods", strlen(".ods"), (void*)"application/vnd.oasis.opendocument.spreadsheet");
	s2_extended->Insert((const char *)".ots", strlen(".ots"), (void*)"application/vnd.oasis.opendocument.spreadsheet-template");
	s2_extended->Insert((const char *)".odt", strlen(".odt"), (void*)"application/vnd.oasis.opendocument.text");
	s2_extended->Insert((const char *)".odm", strlen(".odm"), (void*)"application/vnd.oasis.opendocument.text-master");
	s2_extended->Insert((const char *)".ott", strlen(".ott"), (void*)"application/vnd.oasis.opendocument.text-template");
	s2_extended->Insert((const char *)".oth", strlen(".oth"), (void*)"application/vnd.oasis.opendocument.text-web");
	s2_extended->Insert((const char *)".xo", strlen(".xo"), (void*)"application/vnd.olpc-sugar");
	s2_extended->Insert((const char *)".dd2", strlen(".dd2"), (void*)"application/vnd.oma.dd2+xml");
	s2_extended->Insert((const char *)".oxt", strlen(".oxt"), (void*)"application/vnd.openofficeorg.extension");
	s2_extended->Insert((const char *)".pptx", strlen(".pptx"), (void*)"application/vnd.openxmlformats-officedocument.presentationml.presentation");
	s2_extended->Insert((const char *)".sldx", strlen(".sldx"), (void*)"application/vnd.openxmlformats-officedocument.presentationml.slide");
	s2_extended->Insert((const char *)".ppsx", strlen(".ppsx"), (void*)"application/vnd.openxmlformats-officedocument.presentationml.slideshow");
	s2_extended->Insert((const char *)".potx", strlen(".potx"), (void*)"application/vnd.openxmlformats-officedocument.presentationml.template");
	s2_extended->Insert((const char *)".xlsx", strlen(".xlsx"), (void*)"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
	s2_extended->Insert((const char *)".xltx", strlen(".xltx"), (void*)"application/vnd.openxmlformats-officedocument.spreadsheetml.template");
	s2_extended->Insert((const char *)".docx", strlen(".docx"), (void*)"application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	s2_extended->Insert((const char *)".dotx", strlen(".dotx"), (void*)"application/vnd.openxmlformats-officedocument.wordprocessingml.template");
	s2_extended->Insert((const char *)".mgp", strlen(".mgp"), (void*)"application/vnd.osgeo.mapguide.package");
	s2_extended->Insert((const char *)".dp", strlen(".dp"), (void*)"application/vnd.osgi.dp");
	s2_extended->Insert((const char *)".esa", strlen(".esa"), (void*)"application/vnd.osgi.subsystem");
	s2_extended->Insert((const char *)".pdb", strlen(".pdb"), (void*)"application/vnd.palm");
	s2_extended->Insert((const char *)".pqa", strlen(".pqa"), (void*)"application/vnd.palm");
	s2_extended->Insert((const char *)".oprc", strlen(".oprc"), (void*)"application/vnd.palm");
	s2_extended->Insert((const char *)".paw", strlen(".paw"), (void*)"application/vnd.pawaafile");
	s2_extended->Insert((const char *)".str", strlen(".str"), (void*)"application/vnd.pg.format");
	s2_extended->Insert((const char *)".ei6", strlen(".ei6"), (void*)"application/vnd.pg.osasli");
	s2_extended->Insert((const char *)".efif", strlen(".efif"), (void*)"application/vnd.picsel");
	s2_extended->Insert((const char *)".wg", strlen(".wg"), (void*)"application/vnd.pmi.widget");
	s2_extended->Insert((const char *)".plf", strlen(".plf"), (void*)"application/vnd.pocketlearn");
	s2_extended->Insert((const char *)".pbd", strlen(".pbd"), (void*)"application/vnd.powerbuilder6");
	s2_extended->Insert((const char *)".box", strlen(".box"), (void*)"application/vnd.previewsystems.box");
	s2_extended->Insert((const char *)".mgz", strlen(".mgz"), (void*)"application/vnd.proteus.magazine");
	s2_extended->Insert((const char *)".qps", strlen(".qps"), (void*)"application/vnd.publishare-delta-tree");
	s2_extended->Insert((const char *)".ptid", strlen(".ptid"), (void*)"application/vnd.pvi.ptid1");
	s2_extended->Insert((const char *)".qxd", strlen(".qxd"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".qxt", strlen(".qxt"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".qwd", strlen(".qwd"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".qwt", strlen(".qwt"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".qxl", strlen(".qxl"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".qxb", strlen(".qxb"), (void*)"application/vnd.quark.quarkxpress");
	s2_extended->Insert((const char *)".bed", strlen(".bed"), (void*)"application/vnd.realvnc.bed");
	s2_extended->Insert((const char *)".mxl", strlen(".mxl"), (void*)"application/vnd.recordare.musicxml");
	s2_extended->Insert((const char *)".musicxml", strlen(".musicxml"), (void*)"application/vnd.recordare.musicxml+xml");
	s2_extended->Insert((const char *)".cryptonote", strlen(".cryptonote"), (void*)"application/vnd.rig.cryptonote");
	s2_extended->Insert((const char *)".cod", strlen(".cod"), (void*)"application/vnd.rim.cod");
	s2_extended->Insert((const char *)".rm", strlen(".rm"), (void*)"application/vnd.rn-realmedia");
	s2_extended->Insert((const char *)".rmvb", strlen(".rmvb"), (void*)"application/vnd.rn-realmedia-vbr");
	s2_extended->Insert((const char *)".link66", strlen(".link66"), (void*)"application/vnd.route66.link66+xml");
	s2_extended->Insert((const char *)".st", strlen(".st"), (void*)"application/vnd.sailingtracker.track");
	s2_extended->Insert((const char *)".see", strlen(".see"), (void*)"application/vnd.seemail");
	s2_extended->Insert((const char *)".sema", strlen(".sema"), (void*)"application/vnd.sema");
	s2_extended->Insert((const char *)".semd", strlen(".semd"), (void*)"application/vnd.semd");
	s2_extended->Insert((const char *)".semf", strlen(".semf"), (void*)"application/vnd.semf");
	s2_extended->Insert((const char *)".ifm", strlen(".ifm"), (void*)"application/vnd.shana.informed.formdata");
	s2_extended->Insert((const char *)".itp", strlen(".itp"), (void*)"application/vnd.shana.informed.formtemplate");
	s2_extended->Insert((const char *)".iif", strlen(".iif"), (void*)"application/vnd.shana.informed.interchange");
	s2_extended->Insert((const char *)".ipk", strlen(".ipk"), (void*)"application/vnd.shana.informed.package");
	s2_extended->Insert((const char *)".twd", strlen(".twd"), (void*)"application/vnd.simtech-mindmapper");
	s2_extended->Insert((const char *)".twds", strlen(".twds"), (void*)"application/vnd.simtech-mindmapper");
	s2_extended->Insert((const char *)".mmf", strlen(".mmf"), (void*)"application/vnd.smaf");
	s2_extended->Insert((const char *)".teacher", strlen(".teacher"), (void*)"application/vnd.smart.teacher");
	s2_extended->Insert((const char *)".sdkm", strlen(".sdkm"), (void*)"application/vnd.solent.sdkm+xml");
	s2_extended->Insert((const char *)".sdkd", strlen(".sdkd"), (void*)"application/vnd.solent.sdkm+xml");
	s2_extended->Insert((const char *)".dxp", strlen(".dxp"), (void*)"application/vnd.spotfire.dxp");
	s2_extended->Insert((const char *)".sfs", strlen(".sfs"), (void*)"application/vnd.spotfire.sfs");
	s2_extended->Insert((const char *)".sdc", strlen(".sdc"), (void*)"application/vnd.stardivision.calc");
	s2_extended->Insert((const char *)".sda", strlen(".sda"), (void*)"application/vnd.stardivision.draw");
	s2_extended->Insert((const char *)".sdd", strlen(".sdd"), (void*)"application/vnd.stardivision.impress");
	s2_extended->Insert((const char *)".smf", strlen(".smf"), (void*)"application/vnd.stardivision.math");
	s2_extended->Insert((const char *)".sdw", strlen(".sdw"), (void*)"application/vnd.stardivision.writer");
	s2_extended->Insert((const char *)".vor", strlen(".vor"), (void*)"application/vnd.stardivision.writer");
	s2_extended->Insert((const char *)".sgl", strlen(".sgl"), (void*)"application/vnd.stardivision.writer-global");
	s2_extended->Insert((const char *)".smzip", strlen(".smzip"), (void*)"application/vnd.stepmania.package");
	s2_extended->Insert((const char *)".sm", strlen(".sm"), (void*)"application/vnd.stepmania.stepchart");
	s2_extended->Insert((const char *)".sxc", strlen(".sxc"), (void*)"application/vnd.sun.xml.calc");
	s2_extended->Insert((const char *)".stc", strlen(".stc"), (void*)"application/vnd.sun.xml.calc.template");
	s2_extended->Insert((const char *)".sxd", strlen(".sxd"), (void*)"application/vnd.sun.xml.draw");
	s2_extended->Insert((const char *)".std", strlen(".std"), (void*)"application/vnd.sun.xml.draw.template");
	s2_extended->Insert((const char *)".sxi", strlen(".sxi"), (void*)"application/vnd.sun.xml.impress");
	s2_extended->Insert((const char *)".sti", strlen(".sti"), (void*)"application/vnd.sun.xml.impress.template");
	s2_extended->Insert((const char *)".sxm", strlen(".sxm"), (void*)"application/vnd.sun.xml.math");
	s2_extended->Insert((const char *)".sxw", strlen(".sxw"), (void*)"application/vnd.sun.xml.writer");
	s2_extended->Insert((const char *)".sxg", strlen(".sxg"), (void*)"application/vnd.sun.xml.writer.global");
	s2_extended->Insert((const char *)".stw", strlen(".stw"), (void*)"application/vnd.sun.xml.writer.template");
	s2_extended->Insert((const char *)".sus", strlen(".sus"), (void*)"application/vnd.sus-calendar");
	s2_extended->Insert((const char *)".susp", strlen(".susp"), (void*)"application/vnd.sus-calendar");
	s2_extended->Insert((const char *)".svd", strlen(".svd"), (void*)"application/vnd.svd");
	s2_extended->Insert((const char *)".sis", strlen(".sis"), (void*)"application/vnd.symbian.install");
	s2_extended->Insert((const char *)".sisx", strlen(".sisx"), (void*)"application/vnd.symbian.install");
	s2_extended->Insert((const char *)".xsm", strlen(".xsm"), (void*)"application/vnd.syncml+xml");
	s2_extended->Insert((const char *)".bdm", strlen(".bdm"), (void*)"application/vnd.syncml.dm+wbxml");
	s2_extended->Insert((const char *)".xdm", strlen(".xdm"), (void*)"application/vnd.syncml.dm+xml");
	s2_extended->Insert((const char *)".tao", strlen(".tao"), (void*)"application/vnd.tao.intent-module-archive");
	s2_extended->Insert((const char *)".pcap", strlen(".pcap"), (void*)"application/vnd.tcpdump.pcap");
	s2_extended->Insert((const char *)".cap", strlen(".cap"), (void*)"application/vnd.tcpdump.pcap");
	s2_extended->Insert((const char *)".dmp", strlen(".dmp"), (void*)"application/vnd.tcpdump.pcap");
	s2_extended->Insert((const char *)".tmo", strlen(".tmo"), (void*)"application/vnd.tmobile-livetv");
	s2_extended->Insert((const char *)".tpt", strlen(".tpt"), (void*)"application/vnd.trid.tpt");
	s2_extended->Insert((const char *)".mxs", strlen(".mxs"), (void*)"application/vnd.triscape.mxs");
	s2_extended->Insert((const char *)".tra", strlen(".tra"), (void*)"application/vnd.trueapp");
	s2_extended->Insert((const char *)".ufd", strlen(".ufd"), (void*)"application/vnd.ufdl");
	s2_extended->Insert((const char *)".ufdl", strlen(".ufdl"), (void*)"application/vnd.ufdl");
	s2_extended->Insert((const char *)".utz", strlen(".utz"), (void*)"application/vnd.uiq.theme");
	s2_extended->Insert((const char *)".umj", strlen(".umj"), (void*)"application/vnd.umajin");
	s2_extended->Insert((const char *)".unityweb", strlen(".unityweb"), (void*)"application/vnd.unity");
	s2_extended->Insert((const char *)".uoml", strlen(".uoml"), (void*)"application/vnd.uoml+xml");
	s2_extended->Insert((const char *)".vcx", strlen(".vcx"), (void*)"application/vnd.vcx");
	s2_extended->Insert((const char *)".vsd", strlen(".vsd"), (void*)"application/vnd.visio");
	s2_extended->Insert((const char *)".vst", strlen(".vst"), (void*)"application/vnd.visio");
	s2_extended->Insert((const char *)".vss", strlen(".vss"), (void*)"application/vnd.visio");
	s2_extended->Insert((const char *)".vsw", strlen(".vsw"), (void*)"application/vnd.visio");
	s2_extended->Insert((const char *)".vis", strlen(".vis"), (void*)"application/vnd.visionary");
	s2_extended->Insert((const char *)".vsf", strlen(".vsf"), (void*)"application/vnd.vsf");
	s2_extended->Insert((const char *)".wbxml", strlen(".wbxml"), (void*)"application/vnd.wap.wbxml");
	s2_extended->Insert((const char *)".wmlc", strlen(".wmlc"), (void*)"application/vnd.wap.wmlc");
	s2_extended->Insert((const char *)".wmlsc", strlen(".wmlsc"), (void*)"application/vnd.wap.wmlscriptc");
	s2_extended->Insert((const char *)".wtb", strlen(".wtb"), (void*)"application/vnd.webturbo");
	s2_extended->Insert((const char *)".nbp", strlen(".nbp"), (void*)"application/vnd.wolfram.player");
	s2_extended->Insert((const char *)".wpd", strlen(".wpd"), (void*)"application/vnd.wordperfect");
	s2_extended->Insert((const char *)".wqd", strlen(".wqd"), (void*)"application/vnd.wqd");
	s2_extended->Insert((const char *)".stf", strlen(".stf"), (void*)"application/vnd.wt.stf");
	s2_extended->Insert((const char *)".xar", strlen(".xar"), (void*)"application/vnd.xara");
	s2_extended->Insert((const char *)".xfdl", strlen(".xfdl"), (void*)"application/vnd.xfdl");
	s2_extended->Insert((const char *)".hvd", strlen(".hvd"), (void*)"application/vnd.yamaha.hv-dic");
	s2_extended->Insert((const char *)".hvs", strlen(".hvs"), (void*)"application/vnd.yamaha.hv-script");
	s2_extended->Insert((const char *)".hvp", strlen(".hvp"), (void*)"application/vnd.yamaha.hv-voice");
	s2_extended->Insert((const char *)".osf", strlen(".osf"), (void*)"application/vnd.yamaha.openscoreformat");
	s2_extended->Insert((const char *)".osfpvg", strlen(".osfpvg"), (void*)"application/vnd.yamaha.openscoreformat.osfpvg+xml");
	s2_extended->Insert((const char *)".saf", strlen(".saf"), (void*)"application/vnd.yamaha.smaf-audio");
	s2_extended->Insert((const char *)".spf", strlen(".spf"), (void*)"application/vnd.yamaha.smaf-phrase");
	s2_extended->Insert((const char *)".cmp", strlen(".cmp"), (void*)"application/vnd.yellowriver-custom-menu");
	s2_extended->Insert((const char *)".zir", strlen(".zir"), (void*)"application/vnd.zul");
	s2_extended->Insert((const char *)".zirz", strlen(".zirz"), (void*)"application/vnd.zul");
	s2_extended->Insert((const char *)".zaz", strlen(".zaz"), (void*)"application/vnd.zzazz.deck+xml");
	s2_extended->Insert((const char *)".vxml", strlen(".vxml"), (void*)"application/voicexml+xml");
	s2_extended->Insert((const char *)".wgt", strlen(".wgt"), (void*)"application/widget");
	s2_extended->Insert((const char *)".hlp", strlen(".hlp"), (void*)"application/winhlp");
	s2_extended->Insert((const char *)".wsdl", strlen(".wsdl"), (void*)"application/wsdl+xml");
	s2_extended->Insert((const char *)".wspolicy", strlen(".wspolicy"), (void*)"application/wspolicy+xml");
	s2_extended->Insert((const char *)".7z", strlen(".7z"), (void*)"application/x-7z-compressed");
	s2_extended->Insert((const char *)".abw", strlen(".abw"), (void*)"application/x-abiword");
	s2_extended->Insert((const char *)".ace", strlen(".ace"), (void*)"application/x-ace-compressed");
	s2_extended->Insert((const char *)".dmg", strlen(".dmg"), (void*)"application/x-apple-diskimage");
	s2_extended->Insert((const char *)".aab", strlen(".aab"), (void*)"application/x-authorware-bin");
	s2_extended->Insert((const char *)".x32", strlen(".x32"), (void*)"application/x-authorware-bin");
	s2_extended->Insert((const char *)".u32", strlen(".u32"), (void*)"application/x-authorware-bin");
	s2_extended->Insert((const char *)".vox", strlen(".vox"), (void*)"application/x-authorware-bin");
	s2_extended->Insert((const char *)".aam", strlen(".aam"), (void*)"application/x-authorware-map");
	s2_extended->Insert((const char *)".aas", strlen(".aas"), (void*)"application/x-authorware-seg");
	s2_extended->Insert((const char *)".bcpio", strlen(".bcpio"), (void*)"application/x-bcpio");
	s2_extended->Insert((const char *)".torrent", strlen(".torrent"), (void*)"application/x-bittorrent");
	s2_extended->Insert((const char *)".blb", strlen(".blb"), (void*)"application/x-blorb");
	s2_extended->Insert((const char *)".blorb", strlen(".blorb"), (void*)"application/x-blorb");
	s2_extended->Insert((const char *)".bz", strlen(".bz"), (void*)"application/x-bzip");
	s2_extended->Insert((const char *)".bz2", strlen(".bz2"), (void*)"application/x-bzip2");
	s2_extended->Insert((const char *)".boz", strlen(".boz"), (void*)"application/x-bzip2");
	s2_extended->Insert((const char *)".cbr", strlen(".cbr"), (void*)"application/x-cbr");
	s2_extended->Insert((const char *)".cba", strlen(".cba"), (void*)"application/x-cbr");
	s2_extended->Insert((const char *)".cbt", strlen(".cbt"), (void*)"application/x-cbr");
	s2_extended->Insert((const char *)".cbz", strlen(".cbz"), (void*)"application/x-cbr");
	s2_extended->Insert((const char *)".cb7", strlen(".cb7"), (void*)"application/x-cbr");
	s2_extended->Insert((const char *)".vcd", strlen(".vcd"), (void*)"application/x-cdlink");
	s2_extended->Insert((const char *)".cfs", strlen(".cfs"), (void*)"application/x-cfs-compressed");
	s2_extended->Insert((const char *)".chat", strlen(".chat"), (void*)"application/x-chat");
	s2_extended->Insert((const char *)".pgn", strlen(".pgn"), (void*)"application/x-chess-pgn");
	s2_extended->Insert((const char *)".nsc", strlen(".nsc"), (void*)"application/x-conference");
	s2_extended->Insert((const char *)".cpio", strlen(".cpio"), (void*)"application/x-cpio");
	s2_extended->Insert((const char *)".csh", strlen(".csh"), (void*)"application/x-csh");
	s2_extended->Insert((const char *)".deb", strlen(".deb"), (void*)"application/x-debian-package");
	s2_extended->Insert((const char *)".udeb", strlen(".udeb"), (void*)"application/x-debian-package");
	s2_extended->Insert((const char *)".dgc", strlen(".dgc"), (void*)"application/x-dgc-compressed");
	s2_extended->Insert((const char *)".dir", strlen(".dir"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".dcr", strlen(".dcr"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".dxr", strlen(".dxr"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".cst", strlen(".cst"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".cct", strlen(".cct"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".cxt", strlen(".cxt"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".w3d", strlen(".w3d"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".fgd", strlen(".fgd"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".swa", strlen(".swa"), (void*)"application/x-director");
	s2_extended->Insert((const char *)".wad", strlen(".wad"), (void*)"application/x-doom");
	s2_extended->Insert((const char *)".ncx", strlen(".ncx"), (void*)"application/x-dtbncx+xml");
	s2_extended->Insert((const char *)".dtb", strlen(".dtb"), (void*)"application/x-dtbook+xml");
	s2_extended->Insert((const char *)".res", strlen(".res"), (void*)"application/x-dtbresource+xml");
	s2_extended->Insert((const char *)".dvi", strlen(".dvi"), (void*)"application/x-dvi");
	s2_extended->Insert((const char *)".evy", strlen(".evy"), (void*)"application/x-envoy");
	s2_extended->Insert((const char *)".eva", strlen(".eva"), (void*)"application/x-eva");
	s2_extended->Insert((const char *)".bdf", strlen(".bdf"), (void*)"application/x-font-bdf");
	s2_extended->Insert((const char *)".gsf", strlen(".gsf"), (void*)"application/x-font-ghostscript");
	s2_extended->Insert((const char *)".psf", strlen(".psf"), (void*)"application/x-font-linux-psf");
	s2_extended->Insert((const char *)".pcf", strlen(".pcf"), (void*)"application/x-font-pcf");
	s2_extended->Insert((const char *)".snf", strlen(".snf"), (void*)"application/x-font-snf");
	s2_extended->Insert((const char *)".pfa", strlen(".pfa"), (void*)"application/x-font-type1");
	s2_extended->Insert((const char *)".pfb", strlen(".pfb"), (void*)"application/x-font-type1");
	s2_extended->Insert((const char *)".pfm", strlen(".pfm"), (void*)"application/x-font-type1");
	s2_extended->Insert((const char *)".afm", strlen(".afm"), (void*)"application/x-font-type1");
	s2_extended->Insert((const char *)".arc", strlen(".arc"), (void*)"application/x-freearc");
	s2_extended->Insert((const char *)".spl", strlen(".spl"), (void*)"application/x-futuresplash");
	s2_extended->Insert((const char *)".gca", strlen(".gca"), (void*)"application/x-gca-compressed");
	s2_extended->Insert((const char *)".ulx", strlen(".ulx"), (void*)"application/x-glulx");
	s2_extended->Insert((const char *)".gnumeric", strlen(".gnumeric"), (void*)"application/x-gnumeric");
	s2_extended->Insert((const char *)".gramps", strlen(".gramps"), (void*)"application/x-gramps-xml");
	s2_extended->Insert((const char *)".gtar", strlen(".gtar"), (void*)"application/x-gtar");
	s2_extended->Insert((const char *)".hdf", strlen(".hdf"), (void*)"application/x-hdf");
	s2_extended->Insert((const char *)".install", strlen(".install"), (void*)"application/x-install-instructions");
	s2_extended->Insert((const char *)".iso", strlen(".iso"), (void*)"application/x-iso9660-image");
	s2_extended->Insert((const char *)".jnlp", strlen(".jnlp"), (void*)"application/x-java-jnlp-file");
	s2_extended->Insert((const char *)".latex", strlen(".latex"), (void*)"application/x-latex");
	s2_extended->Insert((const char *)".lzh", strlen(".lzh"), (void*)"application/x-lzh-compressed");
	s2_extended->Insert((const char *)".lha", strlen(".lha"), (void*)"application/x-lzh-compressed");
	s2_extended->Insert((const char *)".mie", strlen(".mie"), (void*)"application/x-mie");
	s2_extended->Insert((const char *)".prc", strlen(".prc"), (void*)"application/x-mobipocket-ebook");
	s2_extended->Insert((const char *)".mobi", strlen(".mobi"), (void*)"application/x-mobipocket-ebook");
	s2_extended->Insert((const char *)".application", strlen(".application"), (void*)"application/x-ms-application");
	s2_extended->Insert((const char *)".lnk", strlen(".lnk"), (void*)"application/x-ms-shortcut");
	s2_extended->Insert((const char *)".wmd", strlen(".wmd"), (void*)"application/x-ms-wmd");
	s2_extended->Insert((const char *)".wmz", strlen(".wmz"), (void*)"application/x-ms-wmz");
	s2_extended->Insert((const char *)".xbap", strlen(".xbap"), (void*)"application/x-ms-xbap");
	s2_extended->Insert((const char *)".mdb", strlen(".mdb"), (void*)"application/x-msaccess");
	s2_extended->Insert((const char *)".obd", strlen(".obd"), (void*)"application/x-msbinder");
	s2_extended->Insert((const char *)".crd", strlen(".crd"), (void*)"application/x-mscardfile");
	s2_extended->Insert((const char *)".clp", strlen(".clp"), (void*)"application/x-msclip");
	s2_extended->Insert((const char *)".exe", strlen(".exe"), (void*)"application/x-msdownload");
	s2_extended->Insert((const char *)".dll", strlen(".dll"), (void*)"application/x-msdownload");
	s2_extended->Insert((const char *)".com", strlen(".com"), (void*)"application/x-msdownload");
	s2_extended->Insert((const char *)".bat", strlen(".bat"), (void*)"application/x-msdownload");
	s2_extended->Insert((const char *)".msi", strlen(".msi"), (void*)"application/x-msdownload");
	s2_extended->Insert((const char *)".mvb", strlen(".mvb"), (void*)"application/x-msmediaview");
	s2_extended->Insert((const char *)".m13", strlen(".m13"), (void*)"application/x-msmediaview");
	s2_extended->Insert((const char *)".m14", strlen(".m14"), (void*)"application/x-msmediaview");
	s2_extended->Insert((const char *)".wmf", strlen(".wmf"), (void*)"application/x-msmetafile");
	s2_extended->Insert((const char *)".wmz", strlen(".wmz"), (void*)"application/x-msmetafile");
	s2_extended->Insert((const char *)".emf", strlen(".emf"), (void*)"application/x-msmetafile");
	s2_extended->Insert((const char *)".emz", strlen(".emz"), (void*)"application/x-msmetafile");
	s2_extended->Insert((const char *)".mny", strlen(".mny"), (void*)"application/x-msmoney");
	s2_extended->Insert((const char *)".pub", strlen(".pub"), (void*)"application/x-mspublisher");
	s2_extended->Insert((const char *)".scd", strlen(".scd"), (void*)"application/x-msschedule");
	s2_extended->Insert((const char *)".trm", strlen(".trm"), (void*)"application/x-msterminal");
	s2_extended->Insert((const char *)".wri", strlen(".wri"), (void*)"application/x-mswrite");
	s2_extended->Insert((const char *)".nc", strlen(".nc"), (void*)"application/x-netcdf");
	s2_extended->Insert((const char *)".cdf", strlen(".cdf"), (void*)"application/x-netcdf");
	s2_extended->Insert((const char *)".nzb", strlen(".nzb"), (void*)"application/x-nzb");
	s2_extended->Insert((const char *)".p12", strlen(".p12"), (void*)"application/x-pkcs12");
	s2_extended->Insert((const char *)".pfx", strlen(".pfx"), (void*)"application/x-pkcs12");
	s2_extended->Insert((const char *)".p7b", strlen(".p7b"), (void*)"application/x-pkcs7-certificates");
	s2_extended->Insert((const char *)".spc", strlen(".spc"), (void*)"application/x-pkcs7-certificates");
	s2_extended->Insert((const char *)".p7r", strlen(".p7r"), (void*)"application/x-pkcs7-certreqresp");
	s2_extended->Insert((const char *)".rar", strlen(".rar"), (void*)"application/x-rar-compressed");
	s2_extended->Insert((const char *)".ris", strlen(".ris"), (void*)"application/x-research-info-systems");
	s2_extended->Insert((const char *)".sh", strlen(".sh"), (void*)"application/x-sh");
	s2_extended->Insert((const char *)".shar", strlen(".shar"), (void*)"application/x-shar");
	s2_extended->Insert((const char *)".swf", strlen(".swf"), (void*)"application/x-shockwave-flash");
	s2_extended->Insert((const char *)".xap", strlen(".xap"), (void*)"application/x-silverlight-app");
	s2_extended->Insert((const char *)".sql", strlen(".sql"), (void*)"application/x-sql");
	s2_extended->Insert((const char *)".sit", strlen(".sit"), (void*)"application/x-stuffit");
	s2_extended->Insert((const char *)".sitx", strlen(".sitx"), (void*)"application/x-stuffitx");
	s2_extended->Insert((const char *)".srt", strlen(".srt"), (void*)"application/x-subrip");
	s2_extended->Insert((const char *)".sv4cpio", strlen(".sv4cpio"), (void*)"application/x-sv4cpio");
	s2_extended->Insert((const char *)".sv4crc", strlen(".sv4crc"), (void*)"application/x-sv4crc");
	s2_extended->Insert((const char *)".t3", strlen(".t3"), (void*)"application/x-t3vm-image");
	s2_extended->Insert((const char *)".gam", strlen(".gam"), (void*)"application/x-tads");
	s2_extended->Insert((const char *)".tar", strlen(".tar"), (void*)"application/x-tar");
	s2_extended->Insert((const char *)".tcl", strlen(".tcl"), (void*)"application/x-tcl");
	s2_extended->Insert((const char *)".tex", strlen(".tex"), (void*)"application/x-tex");
	s2_extended->Insert((const char *)".tfm", strlen(".tfm"), (void*)"application/x-tex-tfm");
	s2_extended->Insert((const char *)".texinfo", strlen(".texinfo"), (void*)"application/x-texinfo");
	s2_extended->Insert((const char *)".texi", strlen(".texi"), (void*)"application/x-texinfo");
	s2_extended->Insert((const char *)".obj", strlen(".obj"), (void*)"application/x-tgif");
	s2_extended->Insert((const char *)".ustar", strlen(".ustar"), (void*)"application/x-ustar");
	s2_extended->Insert((const char *)".src", strlen(".src"), (void*)"application/x-wais-source");
	s2_extended->Insert((const char *)".der", strlen(".der"), (void*)"application/x-x509-ca-cert");
	s2_extended->Insert((const char *)".crt", strlen(".crt"), (void*)"application/x-x509-ca-cert");
	s2_extended->Insert((const char *)".fig", strlen(".fig"), (void*)"application/x-xfig");
	s2_extended->Insert((const char *)".xlf", strlen(".xlf"), (void*)"application/x-xliff+xml");
	s2_extended->Insert((const char *)".xpi", strlen(".xpi"), (void*)"application/x-xpinstall");
	s2_extended->Insert((const char *)".xz", strlen(".xz"), (void*)"application/x-xz");
	s2_extended->Insert((const char *)".z1", strlen(".z1"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z2", strlen(".z2"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z3", strlen(".z3"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z4", strlen(".z4"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z5", strlen(".z5"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z6", strlen(".z6"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z7", strlen(".z7"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".z8", strlen(".z8"), (void*)"application/x-zmachine");
	s2_extended->Insert((const char *)".xaml", strlen(".xaml"), (void*)"application/xaml+xml");
	s2_extended->Insert((const char *)".xdf", strlen(".xdf"), (void*)"application/xcap-diff+xml");
	s2_extended->Insert((const char *)".xenc", strlen(".xenc"), (void*)"application/xenc+xml");
	s2_extended->Insert((const char *)".xhtml", strlen(".xhtml"), (void*)"application/xhtml+xml");
	s2_extended->Insert((const char *)".xht", strlen(".xht"), (void*)"application/xhtml+xml");
	s2_extended->Insert((const char *)".xml", strlen(".xml"), (void*)"application/xml");
	s2_extended->Insert((const char *)".xsl", strlen(".xsl"), (void*)"application/xml");
	s2_extended->Insert((const char *)".dtd", strlen(".dtd"), (void*)"application/xml-dtd");
	s2_extended->Insert((const char *)".xop", strlen(".xop"), (void*)"application/xop+xml");
	s2_extended->Insert((const char *)".xpl", strlen(".xpl"), (void*)"application/xproc+xml");
	s2_extended->Insert((const char *)".xslt", strlen(".xslt"), (void*)"application/xslt+xml");
	s2_extended->Insert((const char *)".xspf", strlen(".xspf"), (void*)"application/xspf+xml");
	s2_extended->Insert((const char *)".mxml", strlen(".mxml"), (void*)"application/xv+xml");
	s2_extended->Insert((const char *)".xhvml", strlen(".xhvml"), (void*)"application/xv+xml");
	s2_extended->Insert((const char *)".xvml", strlen(".xvml"), (void*)"application/xv+xml");
	s2_extended->Insert((const char *)".xvm", strlen(".xvm"), (void*)"application/xv+xml");
	s2_extended->Insert((const char *)".yang", strlen(".yang"), (void*)"application/yang");
	s2_extended->Insert((const char *)".yin", strlen(".yin"), (void*)"application/yin+xml");
	s2_extended->Insert((const char *)".zip", strlen(".zip"), (void*)"application/zip");
	s2_extended->Insert((const char *)".adp", strlen(".adp"), (void*)"audio/adpcm");
	s2_extended->Insert((const char *)".au", strlen(".au"), (void*)"audio/basic");
	s2_extended->Insert((const char *)".snd", strlen(".snd"), (void*)"audio/basic");
	s2_extended->Insert((const char *)".mid", strlen(".mid"), (void*)"audio/midi");
	s2_extended->Insert((const char *)".midi", strlen(".midi"), (void*)"audio/midi");
	s2_extended->Insert((const char *)".kar", strlen(".kar"), (void*)"audio/midi");
	s2_extended->Insert((const char *)".rmi", strlen(".rmi"), (void*)"audio/midi");
	s2_extended->Insert((const char *)".m4a", strlen(".m4a"), (void*)"audio/mp4");
	s2_extended->Insert((const char *)".mp4a", strlen(".mp4a"), (void*)"audio/mp4");
	s2_extended->Insert((const char *)".mpga", strlen(".mpga"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".mp2", strlen(".mp2"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".mp2a", strlen(".mp2a"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".mp3", strlen(".mp3"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".m2a", strlen(".m2a"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".m3a", strlen(".m3a"), (void*)"audio/mpeg");
	s2_extended->Insert((const char *)".oga", strlen(".oga"), (void*)"audio/ogg");
	s2_extended->Insert((const char *)".ogg", strlen(".ogg"), (void*)"audio/ogg");
	s2_extended->Insert((const char *)".spx", strlen(".spx"), (void*)"audio/ogg");
	s2_extended->Insert((const char *)".s3m", strlen(".s3m"), (void*)"audio/s3m");
	s2_extended->Insert((const char *)".sil", strlen(".sil"), (void*)"audio/silk");
	s2_extended->Insert((const char *)".uva", strlen(".uva"), (void*)"audio/vnd.dece.audio");
	s2_extended->Insert((const char *)".uvva", strlen(".uvva"), (void*)"audio/vnd.dece.audio");
	s2_extended->Insert((const char *)".eol", strlen(".eol"), (void*)"audio/vnd.digital-winds");
	s2_extended->Insert((const char *)".dra", strlen(".dra"), (void*)"audio/vnd.dra");
	s2_extended->Insert((const char *)".dts", strlen(".dts"), (void*)"audio/vnd.dts");
	s2_extended->Insert((const char *)".dtshd", strlen(".dtshd"), (void*)"audio/vnd.dts.hd");
	s2_extended->Insert((const char *)".lvp", strlen(".lvp"), (void*)"audio/vnd.lucent.voice");
	s2_extended->Insert((const char *)".pya", strlen(".pya"), (void*)"audio/vnd.ms-playready.media.pya");
	s2_extended->Insert((const char *)".ecelp4800", strlen(".ecelp4800"), (void*)"audio/vnd.nuera.ecelp4800");
	s2_extended->Insert((const char *)".ecelp7470", strlen(".ecelp7470"), (void*)"audio/vnd.nuera.ecelp7470");
	s2_extended->Insert((const char *)".ecelp9600", strlen(".ecelp9600"), (void*)"audio/vnd.nuera.ecelp9600");
	s2_extended->Insert((const char *)".rip", strlen(".rip"), (void*)"audio/vnd.rip");
	s2_extended->Insert((const char *)".weba", strlen(".weba"), (void*)"audio/webm");
	s2_extended->Insert((const char *)".aac", strlen(".aac"), (void*)"audio/x-aac");
	s2_extended->Insert((const char *)".aif", strlen(".aif"), (void*)"audio/x-aiff");
	s2_extended->Insert((const char *)".aiff", strlen(".aiff"), (void*)"audio/x-aiff");
	s2_extended->Insert((const char *)".aifc", strlen(".aifc"), (void*)"audio/x-aiff");
	s2_extended->Insert((const char *)".caf", strlen(".caf"), (void*)"audio/x-caf");
	s2_extended->Insert((const char *)".flac", strlen(".flac"), (void*)"audio/x-flac");
	s2_extended->Insert((const char *)".mka", strlen(".mka"), (void*)"audio/x-matroska");
	s2_extended->Insert((const char *)".m3u", strlen(".m3u"), (void*)"audio/x-mpegurl");
	s2_extended->Insert((const char *)".wax", strlen(".wax"), (void*)"audio/x-ms-wax");
	s2_extended->Insert((const char *)".wma", strlen(".wma"), (void*)"audio/x-ms-wma");
	s2_extended->Insert((const char *)".ram", strlen(".ram"), (void*)"audio/x-pn-realaudio");
	s2_extended->Insert((const char *)".ra", strlen(".ra"), (void*)"audio/x-pn-realaudio");
	s2_extended->Insert((const char *)".rmp", strlen(".rmp"), (void*)"audio/x-pn-realaudio-plugin");
	s2_extended->Insert((const char *)".wav", strlen(".wav"), (void*)"audio/x-wav");
	s2_extended->Insert((const char *)".xm", strlen(".xm"), (void*)"audio/xm");
	s2_extended->Insert((const char *)".cdx", strlen(".cdx"), (void*)"chemical/x-cdx");
	s2_extended->Insert((const char *)".cif", strlen(".cif"), (void*)"chemical/x-cif");
	s2_extended->Insert((const char *)".cmdf", strlen(".cmdf"), (void*)"chemical/x-cmdf");
	s2_extended->Insert((const char *)".cml", strlen(".cml"), (void*)"chemical/x-cml");
	s2_extended->Insert((const char *)".csml", strlen(".csml"), (void*)"chemical/x-csml");
	s2_extended->Insert((const char *)".xyz", strlen(".xyz"), (void*)"chemical/x-xyz");
	s2_extended->Insert((const char *)".ttc", strlen(".ttc"), (void*)"font/collection");
	s2_extended->Insert((const char *)".otf", strlen(".otf"), (void*)"font/otf");
	s2_extended->Insert((const char *)".ttf", strlen(".ttf"), (void*)"font/ttf");
	s2_extended->Insert((const char *)".woff", strlen(".woff"), (void*)"font/woff");
	s2_extended->Insert((const char *)".woff2", strlen(".woff2"), (void*)"font/woff2");
	s2_extended->Insert((const char *)".bmp", strlen(".bmp"), (void*)"image/bmp");
	s2_extended->Insert((const char *)".cgm", strlen(".cgm"), (void*)"image/cgm");
	s2_extended->Insert((const char *)".g3", strlen(".g3"), (void*)"image/g3fax");
	s2_extended->Insert((const char *)".gif", strlen(".gif"), (void*)"image/gif");
	s2_extended->Insert((const char *)".ief", strlen(".ief"), (void*)"image/ief");
	s2_extended->Insert((const char *)".jpeg", strlen(".jpeg"), (void*)"image/jpeg");
	s2_extended->Insert((const char *)".jpg", strlen(".jpg"), (void*)"image/jpeg");
	s2_extended->Insert((const char *)".jpe", strlen(".jpe"), (void*)"image/jpeg");
	s2_extended->Insert((const char *)".ktx", strlen(".ktx"), (void*)"image/ktx");
	s2_extended->Insert((const char *)".png", strlen(".png"), (void*)"image/png");
	s2_extended->Insert((const char *)".btif", strlen(".btif"), (void*)"image/prs.btif");
	s2_extended->Insert((const char *)".sgi", strlen(".sgi"), (void*)"image/sgi");
	s2_extended->Insert((const char *)".svg", strlen(".svg"), (void*)"image/svg+xml");
	s2_extended->Insert((const char *)".svgz", strlen(".svgz"), (void*)"image/svg+xml");
	s2_extended->Insert((const char *)".tiff", strlen(".tiff"), (void*)"image/tiff");
	s2_extended->Insert((const char *)".tif", strlen(".tif"), (void*)"image/tiff");
	s2_extended->Insert((const char *)".psd", strlen(".psd"), (void*)"image/vnd.adobe.photoshop");
	s2_extended->Insert((const char *)".uvi", strlen(".uvi"), (void*)"image/vnd.dece.graphic");
	s2_extended->Insert((const char *)".uvvi", strlen(".uvvi"), (void*)"image/vnd.dece.graphic");
	s2_extended->Insert((const char *)".uvg", strlen(".uvg"), (void*)"image/vnd.dece.graphic");
	s2_extended->Insert((const char *)".uvvg", strlen(".uvvg"), (void*)"image/vnd.dece.graphic");
	s2_extended->Insert((const char *)".djvu", strlen(".djvu"), (void*)"image/vnd.djvu");
	s2_extended->Insert((const char *)".djv", strlen(".djv"), (void*)"image/vnd.djvu");
	s2_extended->Insert((const char *)".sub", strlen(".sub"), (void*)"image/vnd.dvb.subtitle");
	s2_extended->Insert((const char *)".dwg", strlen(".dwg"), (void*)"image/vnd.dwg");
	s2_extended->Insert((const char *)".dxf", strlen(".dxf"), (void*)"image/vnd.dxf");
	s2_extended->Insert((const char *)".fbs", strlen(".fbs"), (void*)"image/vnd.fastbidsheet");
	s2_extended->Insert((const char *)".fpx", strlen(".fpx"), (void*)"image/vnd.fpx");
	s2_extended->Insert((const char *)".fst", strlen(".fst"), (void*)"image/vnd.fst");
	s2_extended->Insert((const char *)".mmr", strlen(".mmr"), (void*)"image/vnd.fujixerox.edmics-mmr");
	s2_extended->Insert((const char *)".rlc", strlen(".rlc"), (void*)"image/vnd.fujixerox.edmics-rlc");
	s2_extended->Insert((const char *)".mdi", strlen(".mdi"), (void*)"image/vnd.ms-modi");
	s2_extended->Insert((const char *)".wdp", strlen(".wdp"), (void*)"image/vnd.ms-photo");
	s2_extended->Insert((const char *)".npx", strlen(".npx"), (void*)"image/vnd.net-fpx");
	s2_extended->Insert((const char *)".wbmp", strlen(".wbmp"), (void*)"image/vnd.wap.wbmp");
	s2_extended->Insert((const char *)".xif", strlen(".xif"), (void*)"image/vnd.xiff");
	s2_extended->Insert((const char *)".webp", strlen(".webp"), (void*)"image/webp");
	s2_extended->Insert((const char *)".3ds", strlen(".3ds"), (void*)"image/x-3ds");
	s2_extended->Insert((const char *)".ras", strlen(".ras"), (void*)"image/x-cmu-raster");
	s2_extended->Insert((const char *)".cmx", strlen(".cmx"), (void*)"image/x-cmx");
	s2_extended->Insert((const char *)".fh", strlen(".fh"), (void*)"image/x-freehand");
	s2_extended->Insert((const char *)".fhc", strlen(".fhc"), (void*)"image/x-freehand");
	s2_extended->Insert((const char *)".fh4", strlen(".fh4"), (void*)"image/x-freehand");
	s2_extended->Insert((const char *)".fh5", strlen(".fh5"), (void*)"image/x-freehand");
	s2_extended->Insert((const char *)".fh7", strlen(".fh7"), (void*)"image/x-freehand");
	s2_extended->Insert((const char *)".ico", strlen(".ico"), (void*)"image/x-icon");
	s2_extended->Insert((const char *)".sid", strlen(".sid"), (void*)"image/x-mrsid-image");
	s2_extended->Insert((const char *)".pcx", strlen(".pcx"), (void*)"image/x-pcx");
	s2_extended->Insert((const char *)".pic", strlen(".pic"), (void*)"image/x-pict");
	s2_extended->Insert((const char *)".pct", strlen(".pct"), (void*)"image/x-pict");
	s2_extended->Insert((const char *)".pnm", strlen(".pnm"), (void*)"image/x-portable-anymap");
	s2_extended->Insert((const char *)".pbm", strlen(".pbm"), (void*)"image/x-portable-bitmap");
	s2_extended->Insert((const char *)".pgm", strlen(".pgm"), (void*)"image/x-portable-graymap");
	s2_extended->Insert((const char *)".ppm", strlen(".ppm"), (void*)"image/x-portable-pixmap");
	s2_extended->Insert((const char *)".rgb", strlen(".rgb"), (void*)"image/x-rgb");
	s2_extended->Insert((const char *)".tga", strlen(".tga"), (void*)"image/x-tga");
	s2_extended->Insert((const char *)".xbm", strlen(".xbm"), (void*)"image/x-xbitmap");
	s2_extended->Insert((const char *)".xpm", strlen(".xpm"), (void*)"image/x-xpixmap");
	s2_extended->Insert((const char *)".xwd", strlen(".xwd"), (void*)"image/x-xwindowdump");
	s2_extended->Insert((const char *)".eml", strlen(".eml"), (void*)"message/rfc822");
	s2_extended->Insert((const char *)".mime", strlen(".mime"), (void*)"message/rfc822");
	s2_extended->Insert((const char *)".igs", strlen(".igs"), (void*)"model/iges");
	s2_extended->Insert((const char *)".iges", strlen(".iges"), (void*)"model/iges");
	s2_extended->Insert((const char *)".msh", strlen(".msh"), (void*)"model/mesh");
	s2_extended->Insert((const char *)".mesh", strlen(".mesh"), (void*)"model/mesh");
	s2_extended->Insert((const char *)".silo", strlen(".silo"), (void*)"model/mesh");
	s2_extended->Insert((const char *)".dae", strlen(".dae"), (void*)"model/vnd.collada+xml");
	s2_extended->Insert((const char *)".dwf", strlen(".dwf"), (void*)"model/vnd.dwf");
	s2_extended->Insert((const char *)".gdl", strlen(".gdl"), (void*)"model/vnd.gdl");
	s2_extended->Insert((const char *)".gtw", strlen(".gtw"), (void*)"model/vnd.gtw");
	s2_extended->Insert((const char *)".mts", strlen(".mts"), (void*)"model/vnd.mts");
	s2_extended->Insert((const char *)".vtu", strlen(".vtu"), (void*)"model/vnd.vtu");
	s2_extended->Insert((const char *)".wrl", strlen(".wrl"), (void*)"model/vrml");
	s2_extended->Insert((const char *)".vrml", strlen(".vrml"), (void*)"model/vrml");
	s2_extended->Insert((const char *)".x3db", strlen(".x3db"), (void*)"model/x3d+binary");
	s2_extended->Insert((const char *)".x3dbz", strlen(".x3dbz"), (void*)"model/x3d+binary");
	s2_extended->Insert((const char *)".x3dv", strlen(".x3dv"), (void*)"model/x3d+vrml");
	s2_extended->Insert((const char *)".x3dvz", strlen(".x3dvz"), (void*)"model/x3d+vrml");
	s2_extended->Insert((const char *)".x3d", strlen(".x3d"), (void*)"model/x3d+xml");
	s2_extended->Insert((const char *)".x3dz", strlen(".x3dz"), (void*)"model/x3d+xml");
	s2_extended->Insert((const char *)".appcache", strlen(".appcache"), (void*)"text/cache-manifest");
	s2_extended->Insert((const char *)".ics", strlen(".ics"), (void*)"text/calendar");
	s2_extended->Insert((const char *)".ifb", strlen(".ifb"), (void*)"text/calendar");
	s2_extended->Insert((const char *)".css", strlen(".css"), (void*)"text/css");
	s2_extended->Insert((const char *)".csv", strlen(".csv"), (void*)"text/csv");
	s2_extended->Insert((const char *)".html", strlen(".html"), (void*)"text/html");
	s2_extended->Insert((const char *)".htm", strlen(".htm"), (void*)"text/html");
	s2_extended->Insert((const char *)".n3", strlen(".n3"), (void*)"text/n3");
	s2_extended->Insert((const char *)".txt", strlen(".txt"), (void*)"text/plain");
	s2_extended->Insert((const char *)".text", strlen(".text"), (void*)"text/plain");
	s2_extended->Insert((const char *)".conf", strlen(".conf"), (void*)"text/plain");
	s2_extended->Insert((const char *)".def", strlen(".def"), (void*)"text/plain");
	s2_extended->Insert((const char *)".list", strlen(".list"), (void*)"text/plain");
	s2_extended->Insert((const char *)".log", strlen(".log"), (void*)"text/plain");
	s2_extended->Insert((const char *)".in", strlen(".in"), (void*)"text/plain");
	s2_extended->Insert((const char *)".dsc", strlen(".dsc"), (void*)"text/prs.lines.tag");
	s2_extended->Insert((const char *)".rtx", strlen(".rtx"), (void*)"text/richtext");
	s2_extended->Insert((const char *)".sgml", strlen(".sgml"), (void*)"text/sgml");
	s2_extended->Insert((const char *)".sgm", strlen(".sgm"), (void*)"text/sgml");
	s2_extended->Insert((const char *)".tsv", strlen(".tsv"), (void*)"text/tab-separated-values");
	s2_extended->Insert((const char *)".t", strlen(".t"), (void*)"text/troff");
	s2_extended->Insert((const char *)".tr", strlen(".tr"), (void*)"text/troff");
	s2_extended->Insert((const char *)".roff", strlen(".roff"), (void*)"text/troff");
	s2_extended->Insert((const char *)".man", strlen(".man"), (void*)"text/troff");
	s2_extended->Insert((const char *)".me", strlen(".me"), (void*)"text/troff");
	s2_extended->Insert((const char *)".ms", strlen(".ms"), (void*)"text/troff");
	s2_extended->Insert((const char *)".ttl", strlen(".ttl"), (void*)"text/turtle");
	s2_extended->Insert((const char *)".uri", strlen(".uri"), (void*)"text/uri-list");
	s2_extended->Insert((const char *)".uris", strlen(".uris"), (void*)"text/uri-list");
	s2_extended->Insert((const char *)".urls", strlen(".urls"), (void*)"text/uri-list");
	s2_extended->Insert((const char *)".vcard", strlen(".vcard"), (void*)"text/vcard");
	s2_extended->Insert((const char *)".curl", strlen(".curl"), (void*)"text/vnd.curl");
	s2_extended->Insert((const char *)".dcurl", strlen(".dcurl"), (void*)"text/vnd.curl.dcurl");
	s2_extended->Insert((const char *)".mcurl", strlen(".mcurl"), (void*)"text/vnd.curl.mcurl");
	s2_extended->Insert((const char *)".scurl", strlen(".scurl"), (void*)"text/vnd.curl.scurl");
	s2_extended->Insert((const char *)".sub", strlen(".sub"), (void*)"text/vnd.dvb.subtitle");
	s2_extended->Insert((const char *)".fly", strlen(".fly"), (void*)"text/vnd.fly");
	s2_extended->Insert((const char *)".flx", strlen(".flx"), (void*)"text/vnd.fmi.flexstor");
	s2_extended->Insert((const char *)".gv", strlen(".gv"), (void*)"text/vnd.graphviz");
	s2_extended->Insert((const char *)".3dml", strlen(".3dml"), (void*)"text/vnd.in3d.3dml");
	s2_extended->Insert((const char *)".spot", strlen(".spot"), (void*)"text/vnd.in3d.spot");
	s2_extended->Insert((const char *)".jad", strlen(".jad"), (void*)"text/vnd.sun.j2me.app-descriptor");
	s2_extended->Insert((const char *)".wml", strlen(".wml"), (void*)"text/vnd.wap.wml");
	s2_extended->Insert((const char *)".wmls", strlen(".wmls"), (void*)"text/vnd.wap.wmlscript");
	s2_extended->Insert((const char *)".s", strlen(".s"), (void*)"text/x-asm");
	s2_extended->Insert((const char *)".asm", strlen(".asm"), (void*)"text/x-asm");
	s2_extended->Insert((const char *)".c", strlen(".c"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".cc", strlen(".cc"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".cxx", strlen(".cxx"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".cpp", strlen(".cpp"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".h", strlen(".h"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".hh", strlen(".hh"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".dic", strlen(".dic"), (void*)"text/x-c");
	s2_extended->Insert((const char *)".f", strlen(".f"), (void*)"text/x-fortran");
	s2_extended->Insert((const char *)".for", strlen(".for"), (void*)"text/x-fortran");
	s2_extended->Insert((const char *)".f77", strlen(".f77"), (void*)"text/x-fortran");
	s2_extended->Insert((const char *)".f90", strlen(".f90"), (void*)"text/x-fortran");
	s2_extended->Insert((const char *)".java", strlen(".java"), (void*)"text/x-java-source");
	s2_extended->Insert((const char *)".nfo", strlen(".nfo"), (void*)"text/x-nfo");
	s2_extended->Insert((const char *)".opml", strlen(".opml"), (void*)"text/x-opml");
	s2_extended->Insert((const char *)".p", strlen(".p"), (void*)"text/x-pascal");
	s2_extended->Insert((const char *)".pas", strlen(".pas"), (void*)"text/x-pascal");
	s2_extended->Insert((const char *)".etx", strlen(".etx"), (void*)"text/x-setext");
	s2_extended->Insert((const char *)".sfv", strlen(".sfv"), (void*)"text/x-sfv");
	s2_extended->Insert((const char *)".uu", strlen(".uu"), (void*)"text/x-uuencode");
	s2_extended->Insert((const char *)".vcs", strlen(".vcs"), (void*)"text/x-vcalendar");
	s2_extended->Insert((const char *)".vcf", strlen(".vcf"), (void*)"text/x-vcard");
	s2_extended->Insert((const char *)".3gp", strlen(".3gp"), (void*)"video/3gpp");
	s2_extended->Insert((const char *)".3g2", strlen(".3g2"), (void*)"video/3gpp2");
	s2_extended->Insert((const char *)".h261", strlen(".h261"), (void*)"video/h261");
	s2_extended->Insert((const char *)".h263", strlen(".h263"), (void*)"video/h263");
	s2_extended->Insert((const char *)".h264", strlen(".h264"), (void*)"video/h264");
	s2_extended->Insert((const char *)".jpgv", strlen(".jpgv"), (void*)"video/jpeg");
	s2_extended->Insert((const char *)".jpm", strlen(".jpm"), (void*)"video/jpm");
	s2_extended->Insert((const char *)".jpgm", strlen(".jpgm"), (void*)"video/jpm");
	s2_extended->Insert((const char *)".mj2", strlen(".mj2"), (void*)"video/mj2");
	s2_extended->Insert((const char *)".mjp2", strlen(".mjp2"), (void*)"video/mj2");
	s2_extended->Insert((const char *)".mp4", strlen(".mp4"), (void*)"video/mp4");
	s2_extended->Insert((const char *)".mp4v", strlen(".mp4v"), (void*)"video/mp4");
	s2_extended->Insert((const char *)".mpg4", strlen(".mpg4"), (void*)"video/mp4");
	s2_extended->Insert((const char *)".mpeg", strlen(".mpeg"), (void*)"video/mpeg");
	s2_extended->Insert((const char *)".mpg", strlen(".mpg"), (void*)"video/mpeg");
	s2_extended->Insert((const char *)".mpe", strlen(".mpe"), (void*)"video/mpeg");
	s2_extended->Insert((const char *)".m1v", strlen(".m1v"), (void*)"video/mpeg");
	s2_extended->Insert((const char *)".m2v", strlen(".m2v"), (void*)"video/mpeg");
	s2_extended->Insert((const char *)".ogv", strlen(".ogv"), (void*)"video/ogg");
	s2_extended->Insert((const char *)".qt", strlen(".qt"), (void*)"video/quicktime");
	s2_extended->Insert((const char *)".mov", strlen(".mov"), (void*)"video/quicktime");
	s2_extended->Insert((const char *)".uvh", strlen(".uvh"), (void*)"video/vnd.dece.hd");
	s2_extended->Insert((const char *)".uvvh", strlen(".uvvh"), (void*)"video/vnd.dece.hd");
	s2_extended->Insert((const char *)".uvm", strlen(".uvm"), (void*)"video/vnd.dece.mobile");
	s2_extended->Insert((const char *)".uvvm", strlen(".uvvm"), (void*)"video/vnd.dece.mobile");
	s2_extended->Insert((const char *)".uvp", strlen(".uvp"), (void*)"video/vnd.dece.pd");
	s2_extended->Insert((const char *)".uvvp", strlen(".uvvp"), (void*)"video/vnd.dece.pd");
	s2_extended->Insert((const char *)".uvs", strlen(".uvs"), (void*)"video/vnd.dece.sd");
	s2_extended->Insert((const char *)".uvvs", strlen(".uvvs"), (void*)"video/vnd.dece.sd");
	s2_extended->Insert((const char *)".uvv", strlen(".uvv"), (void*)"video/vnd.dece.video");
	s2_extended->Insert((const char *)".uvvv", strlen(".uvvv"), (void*)"video/vnd.dece.video");
	s2_extended->Insert((const char *)".dvb", strlen(".dvb"), (void*)"video/vnd.dvb.file");
	s2_extended->Insert((const char *)".fvt", strlen(".fvt"), (void*)"video/vnd.fvt");
	s2_extended->Insert((const char *)".mxu", strlen(".mxu"), (void*)"video/vnd.mpegurl");
	s2_extended->Insert((const char *)".m4u", strlen(".m4u"), (void*)"video/vnd.mpegurl");
	s2_extended->Insert((const char *)".pyv", strlen(".pyv"), (void*)"video/vnd.ms-playready.media.pyv");
	s2_extended->Insert((const char *)".uvu", strlen(".uvu"), (void*)"video/vnd.uvvu.mp4");
	s2_extended->Insert((const char *)".uvvu", strlen(".uvvu"), (void*)"video/vnd.uvvu.mp4");
	s2_extended->Insert((const char *)".viv", strlen(".viv"), (void*)"video/vnd.vivo");
	s2_extended->Insert((const char *)".webm", strlen(".webm"), (void*)"video/webm");
	s2_extended->Insert((const char *)".f4v", strlen(".f4v"), (void*)"video/x-f4v");
	s2_extended->Insert((const char *)".fli", strlen(".fli"), (void*)"video/x-fli");
	s2_extended->Insert((const char *)".flv", strlen(".flv"), (void*)"video/x-flv");
	s2_extended->Insert((const char *)".m4v", strlen(".m4v"), (void*)"video/x-m4v");
	s2_extended->Insert((const char *)".mkv", strlen(".mkv"), (void*)"video/x-matroska");
	s2_extended->Insert((const char *)".mk3d", strlen(".mk3d"), (void*)"video/x-matroska");
	s2_extended->Insert((const char *)".mks", strlen(".mks"), (void*)"video/x-matroska");
	s2_extended->Insert((const char *)".mng", strlen(".mng"), (void*)"video/x-mng");
	s2_extended->Insert((const char *)".asf", strlen(".asf"), (void*)"video/x-ms-asf");
	s2_extended->Insert((const char *)".asx", strlen(".asx"), (void*)"video/x-ms-asf");
	s2_extended->Insert((const char *)".vob", strlen(".vob"), (void*)"video/x-ms-vob");
	s2_extended->Insert((const char *)".wm", strlen(".wm"), (void*)"video/x-ms-wm");
	s2_extended->Insert((const char *)".wmv", strlen(".wmv"), (void*)"video/x-ms-wmv");
	s2_extended->Insert((const char *)".wmx", strlen(".wmx"), (void*)"video/x-ms-wmx");
	s2_extended->Insert((const char *)".wvx", strlen(".wvx"), (void*)"video/x-ms-wvx");
	s2_extended->Insert((const char *)".avi", strlen(".avi"), (void*)"video/x-msvideo");
	s2_extended->Insert((const char *)".movie", strlen(".movie"), (void*)"video/x-sgi-movie");
	s2_extended->Insert((const char *)".smv", strlen(".smv"), (void*)"video/x-smv");
	s2_extended->Insert((const char *)".ice", strlen(".ice"), (void*)"x-conference/x-cooltalk");
}

static SCM mtfa_mime_from_extension (SCM s_filename)
{
	if (!s2_extended)
	{
		s2_extended = new ImplicitDs();
		fill_in_mimes_from_iana(s2_extended);
	}
	
	if (scm_is_string(s_filename))
	{
		char * pc_filename = scm_to_utf8_string(s_filename);
		string filename = pc_filename;
		free(pc_filename);
		std::string::size_type idx;
		idx = filename.rfind('.');
		string extension;
		if(idx != std::string::npos)
		{
			extension = filename.substr(idx);
			if (extension.size()>0)
			{
				char * s;
				auto found = s2_extended->Find(extension.c_str(), extension.size(), (void**)&s);
				if (found)
					return ScmFromString(s);
			}
		}
	}
	return ScmFromString("application/octet-stream");
}

extern "C" {

typedef struct uchardet * uchardet_t;
uchardet_t uchardet_new(void);
void uchardet_delete(uchardet_t ud);
int uchardet_handle_data(uchardet_t ud, const char * data, size_t len);
void uchardet_data_end(uchardet_t ud);
void uchardet_reset(uchardet_t ud);
const char * uchardet_get_charset(uchardet_t ud);
}

static SCM mtfa_detect_charset (SCM s_filename)
{
	const int BUFFER_SIZE = 65536;
	uchardet_t  handle = uchardet_new();
	char       *charset;
	char        buffer[BUFFER_SIZE];
	int         i;
	
	if (scm_is_string(s_filename))
	{
		char* filename = ScmToString(s_filename);
		if (!filename)
			return SCM_BOOL_F;
		
		FILE *fp =fopen(filename, "r");

		free(filename);
		
		if (!fp)
			return SCM_BOOL_F;
		
		while (!feof(fp))
		{
			size_t len = fread(buffer, 1, BUFFER_SIZE, fp);
			int retval = uchardet_handle_data(handle, buffer, len);
			if (retval != 0)
			{
				fclose(fp);
				return SCM_BOOL_F;
			}
		}
		uchardet_data_end(handle);
		fclose(fp);
		
		charset = strdup(uchardet_get_charset(handle));
		for (i = 0; charset[i]; i++)
		{
			charset[i] = tolower(charset[i]);
		}

		SCM result = ScmFromString(charset);
		free(charset);
		
		uchardet_delete(handle);
		return result;
	} else if (scm_is_bytevector(s_filename))
	{
		int plen = SCM_BYTEVECTOR_LENGTH ( s_filename );
		char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( s_filename );
		
		for (int i=0; i<plen; i+=BUFFER_SIZE)
		{
			size_t len;
			if ((plen-i)>BUFFER_SIZE)
				len = BUFFER_SIZE;
			else
				len = plen-i;
			int retval = uchardet_handle_data(handle, &p[i], len);
			if (retval != 0)
			{
				return SCM_BOOL_F;
			}
		}
		uchardet_data_end(handle);
		
		charset = strdup(uchardet_get_charset(handle));
		for (i = 0; charset[i]; i++)
		{
			charset[i] = tolower(charset[i]);
		}
		
		SCM result = ScmFromString(charset);
		free(charset);
		
		uchardet_delete(handle);
		return result;
	}
	else
		return SCM_BOOL_F;
}


static SCM mtfa_fuzzy_find_guile (SCM s_pattern, SCM s_toSearch, SCM s_maxcost)
{
	char * pattern = scm_to_utf8_string(s_pattern);
	char * toSearch = scm_to_utf8_string(s_toSearch);
	int maxcost = scm_to_int(s_maxcost);
	
	int iret = mtfa_fuzzy_find(pattern, toSearch, maxcost);
	free(pattern);
	free(toSearch);
	if (iret < 0)
	{
		return SCM_BOOL_F;
	}
	else
	{
		return scm_from_int(iret);
	}
}

static SCM mtfa_replace_all(SCM s_pattern, SCM s_toSearch, SCM s_toSubstitute, SCM s_all, SCM tipo)
{
    if (scm_is_string(s_pattern))
    {
        size_t patLen=0;
        char* patData = ScmToStringn ( s_pattern, &patLen);

        size_t searchLen=0;
        char* searchData = ScmToStringn ( s_toSearch, &searchLen);

        size_t substLen=0;
        char* substData = ScmToStringn ( s_toSubstitute, &substLen);
        
        string pattern(patData, patData+patLen);
        string search(searchData, searchData+searchLen);
        string substitute(substData, substData+substLen);
        
        free(patData);
        free(searchData);
        free(substData);
        
        string result = mtfa_replace_matching_pattern(search, pattern, substitute, scm_to_bool(s_all));
        return ScmFromString(result.c_str());
    }
    else
    {
        pcre2_code * patData = (pcre2_code *)scm_to_pointer(s_pattern);

        size_t searchLen=0;
        char* searchData = ScmToStringn ( s_toSearch, &searchLen );

        size_t substLen=0;
        char* substData = ScmToStringn ( s_toSubstitute, &substLen );
        
        string search(searchData, searchData+searchLen);
        string substitute(substData, substData+substLen);
        
        free(searchData);
        free(substData);
        
        string result = mtfa_replace_matching_pattern(search, patData, substitute, scm_to_bool(s_all));
        return ScmFromString(result.c_str());
    }
}

static void pcre2_code_finalizer ( void* code)
{
	mtfa_free_pattern((pcre2_code*)code);
}

SCM scm_mtfa_compile_pattern ( SCM s_pattern, SCM bool_use_jitc);

SCM scm_mtfa_compile_pattern ( SCM s_pattern, SCM bool_use_jitc)
{
    bool use_jitc = true;
    if ((SCM_UNDEFINED != bool_use_jitc) && scm_is_false(bool_use_jitc))
        use_jitc=false;

    size_t len=0;
	char* patData = ScmToStringn ( s_pattern, &len );
    
    pcre2_code * pat = mtfa_compile_pattern(string(patData, patData+len), use_jitc);
    
    free(patData);
    
    return scm_from_pointer((void*)pat, pcre2_code_finalizer);
}


// GZip Compression
// @param data - the data to compress (does not have to be string, can be binary data)
// @param compressedData - the resulting gzip compressed data
// @param level - the gzip compress level -1 = default, 0 = no compression, 1= worst/fastest compression, 9 = best/slowest compression
// @return - true on success, false on failure
static bool local_Compress(const std::string& data, std::string& compressedData, int level = -1);

// GZip Decompression
// @param compressedData - the gzip compressed data
// @param data - the resulting uncompressed data (may contain binary data)
// @return - true on success, false on failure
static bool local_Uncompress(const std::string& compressedData, std::string& data);

#define CHUNK 16384
#define windowBits 15
#define GZIP_ENCODING 16

static bool local_Compress(const std::string& data, std::string& compressedData, int level)
{
  unsigned char out[CHUNK];
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  if (deflateInit2(&strm, level, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK)
  {
    return false;
  }
  strm.next_in = (unsigned char*)data.c_str();
  strm.avail_in = data.size();
  do {
    int have;
    strm.avail_out = CHUNK;
    strm.next_out = out;
    if (deflate(&strm, Z_FINISH) == Z_STREAM_ERROR)
    {
      return false;
    }
    have = CHUNK - strm.avail_out;
    compressedData.append((char*)out, have);
  } while (strm.avail_out == 0);
  if (deflateEnd(&strm) != Z_OK)
  {
    return false;
  }
  return true;
}

static bool local_Uncompress(const std::string& compressedData, std::string& data)
{
  int ret;
  unsigned have;
  z_stream strm;
  unsigned char out[CHUNK];

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
  {
    return false;
  }

  strm.avail_in = compressedData.size();
  strm.next_in = (unsigned char*)compressedData.c_str();
  do {
    strm.avail_out = CHUNK;
    strm.next_out = out;
    ret = inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      inflateEnd(&strm);
      return false;
    }
    have = CHUNK - strm.avail_out;
    data.append((char*)out, have);
  } while (strm.avail_out == 0);

  if (inflateEnd(&strm) != Z_OK) {
    return false;
  }

  return true;
}

static SCM mtfa_http_compress (SCM s_data, SCM i_level)
{
    int len;
    char * data;
    int level = scm_to_int(i_level);
    
	if (scm_is_string(s_data))
	{
		data = ScmToString(s_data);
		if (!data)
			return SCM_BOOL_F;
        len = scm_to_int(scm_string_length(s_data));
	} else if (scm_is_bytevector(s_data))
	{
		len = SCM_BYTEVECTOR_LENGTH ( s_data );
		data = ( char* ) SCM_BYTEVECTOR_CONTENTS ( s_data );
		if (!data)
			return SCM_BOOL_F;
    } else
        return SCM_BOOL_F;
    
    //Ora comprimo
    string compressed;
    bool bRet = local_Compress(string(data, data+len), compressed, level);
    if (scm_is_string(s_data))
        free(data);
    if (bRet)
    {
        char * pret = (char*)malloc(compressed.size());
        memcpy(pret, compressed.data(), compressed.size());
        SCM scm_ret = scm_pointer_to_bytevector (
            scm_from_pointer ( ( void* ) pret, free ),
                                scm_from_int ( compressed.size()),
                                scm_from_int ( 0 ),
                                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    } else
        return SCM_BOOL_F;
}

static SCM mtfa_http_uncompress (SCM s_data)
{
    int len;
    char * compressed;
    
	if (scm_is_bytevector(s_data))
	{
		len = SCM_BYTEVECTOR_LENGTH ( s_data );
		compressed = ( char* ) SCM_BYTEVECTOR_CONTENTS ( s_data );
        
        //Ora decomprimo
        string data;
        
        bool bRet = local_Uncompress(string(compressed, compressed+len), data);
        if (bRet)
        {
            char * pret = (char*)malloc(data.size());
            memcpy(pret, data.data(), data.size());
            SCM scm_ret = scm_pointer_to_bytevector (
                scm_from_pointer ( ( void* ) pret, free ),
                                    scm_from_int ( data.size()),
                                    scm_from_int ( 0 ),
                                    scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
            return scm_ret;
        }
        else
            return SCM_BOOL_F;
    }
    else
        return SCM_BOOL_F;
}



void RLPtoJSON(const RLPValue& rval, UniValue& jval);
bool JSONtoRLP(const UniValue& jval, RLPValue& rval);

// static SCM mtfa_scmjson_to_rlp(SCM scm_in)  //è un vettore di vettori contenente interi e stringhe!
// {
//     if (scm_is_vector(scm_in))
//     {
//         //Converte il vettore di vettori in una struttura json => [...[...]...[...]...[...]...] che contiene solo stringhe come termine finale
//         function<string(SCM)> converti = [&] (SCM scm_vect) {
//             string vettore="";
//             SCM vlen = scm_vector_length(scm_vect);
//             int ivlen = scm_to_int(vlen);
//             for (unsigned int i=0; i<ivlen; ++i)
//             {
//                 SCM item = scm_vector_ref(scm_vect, scm_from_int(i));
//                 if (scm_is_exact_integer(item))
//                 {
//                     //cout << "exact integer\n";
//                     //converte il numero in sequenza stringa
//                     unsigned char * p;
//                     int len;
//                     mpz_class rop;
//                     string res;
//                     scm_to_mpz(item, rop.get_mpz_t());
//                     Mpz2UnsignedPChar(&rop, p, len);
//                     for (int j=0; j<len; ++j)
//                     {
//                         char val[256];
//                         sprintf(val, "\\u%04x", (unsigned int)p[j] & 0x000000FF);
//                         //cout << "2-Valore: " << ((unsigned int)p[j] & 0x000000FF) << " => " << val << endl;
//                         res += val;
//                     }
//                     //res.assign(p, p+len);
//                     free(p);
//                     cout << "Aggiunge " << res << "(" << res.size() << ")" << endl;
//                     vettore = vettore + "," + "\"" + res + "\"";
//                 } else if (scm_is_integer(item))
//                 {
//                     //cout << "integer\n";
//                     //converte il numero in sequenza stringa
//                     unsigned char * p;
//                     int len;
//                     mpz_class rop;
//                     string res;
//                    
//                     rop = scm_to_long_long(item);
//                     Mpz2UnsignedPChar(&rop, p, len);
//                     for (int j=0; j<len; ++j)
//                     {
//                         char val[256];
//                         sprintf(val, "\\u%04x", (unsigned int)p[j] & 0x000000FF);
//                         //cout << "1-Valore: " << ((unsigned int)p[j] & 0x000000FF) << " => " << val << endl;
//                         res += val;
//                     }
//                     //res.assign(p, p+len);
//                     free(p);
//                     cout << "Aggiunge " << res << "(" << res.size() << ")" << endl;
//                     vettore = vettore + "," + "\"" + res + "\"";
//                 } 
//                 else if (scm_is_string(item))
//                 {
//                     //lascia la stringa così com'è
//                     char * p;
//                     unsigned long len;
//                     p = scm_to_latin1_stringn(item, &len);
//                     string res(p, p+len);
//                     free(p);
//                     cout << "Aggiunge " << res << "(" << res.size() << ")" << endl;
//                     vettore = vettore + "," + "\"" + res + "\"";
//                 } else if (scm_is_vector(item))
//                 {
//                     //Converte il vettore, ricorsivo
//                     string newvect = converti(item);
//                     //cout << "Aggiunge " << newvect << "(" << newvect.size() << ")" << endl;
//                     vettore = vettore + "," + newvect;
//                 }
//                 else {
//                     scm_wrong_type_arg_msg("mtfa-scmjson-to-rlp", 0, item, "scmjson vector #(...)");
//                 }
//             }
//             if (vettore.size() > 0)
//                 vettore = "["+vettore.substr(1)+"]"; //tolgo la virgola e aggiungo []
//             else
//                 vettore="[]";
//             return vettore;
//         };
//         
//         string ssin = converti(scm_in);
//         
//         //ora converte la stringa in rlp
//         UniValue jval;
//         RLPValue rval;
// //         unsigned long len_psin;
// //         char * psin = scm_to_latin1_stringn(scm_in, &len_psin);
// //         string ssin;
// //         ssin.assign(psin, len_psin);
// //         free(psin);
//         bool rc = jval.read(ssin);
//         //cout << rc << ", " << ssin.size() << ": " << ssin << endl;
//         if (!rc)
//             scm_wrong_type_arg_msg("mtfa-json-to-rlp", 0, scm_in, "scmjson vector");
//         bool bret = JSONtoRLP(jval, rval);
//         if (!bret)
//             scm_wrong_type_arg_msg("mtfa-json-to-rlp", 0, scm_in, "rpl-able scmjson vector");
//             
//         string rlps = rval.write(); //oppure getvalstr???
//         
//         //lo converte in bytevector e esce
//         int len = rlps.size();
//         char * prlp = (char*)malloc(len);
//         memcpy(prlp, rlps.data(), len);
//         SCM scm_ret = scm_pointer_to_bytevector (
//                           scm_from_pointer ( ( void* ) prlp, free ),
//                           scm_from_int ( len ),
//                           scm_from_int ( 0 ),
//                           scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
//         return scm_ret;
//     }
//     else
//     {
// 		scm_wrong_type_arg_msg("mtfa-scmjson-to-rlp", 0, scm_in, "scmjson vector");
//     }
// }
static SCM mtfa_rlp_to_json(SCM scm_bvin)
{
    if (scm_is_bytevector(scm_bvin))
    {
        //converte il bytevector
        UniValue jval;
        RLPValue rval;
        int len = SCM_BYTEVECTOR_LENGTH ( scm_bvin );
        char* rlpdata = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_bvin );
        string body(rlpdata, rlpdata+len);
        //parse rlp input
//         std::vector<unsigned char> buf;
//         if (body.substr(0, 2) == "0x") {
//             string tmp = body.substr(2);
//             buf = ParseHex(tmp);
//         } else
//             buf = ParseHex(body);

//         //Stampo, per verifica
//         for (int i=0; i<len; ++i)
//         {
//             printf("%02X", (unsigned int)rlpdata[i] & 0x00FF);
//         }
//         cout << endl;
        size_t consumed, wanted;
        bool rc = rval.read((unsigned char*)rlpdata, len, consumed, wanted);

        if (!rc)
            scm_wrong_type_arg_msg("mtfa-rlp-to-json", 0, scm_bvin, "rlp bytevector");
        
        RLPtoJSON(rval, jval);
        string sjson = jval.write(); //oppure getvalstr???
        //cout << sjson << endl;
        //Sjson è fatto ["\u0001","w5�\u0000","R\b","?�\u0005�\u001e��)`�p�8,AR�X��","#��o�\u0000\u0000","","&","K\t\\�6�J�\u001ay�@\u0014\u0019@B\f!�\u0017\u000bt,$��xL':�\u001d","*@}\u0011�6e#U�>s\u000f\nK\r�CE\u001e��\u0010��\b\u0004ljB�%"]

        {
            int len = sjson.size();
            char * prlp = (char*)malloc(len);
            memcpy(prlp, sjson.data(), len);
            SCM scm_ret = scm_pointer_to_bytevector (
                            scm_from_pointer ( ( void* ) prlp, free ),
                            scm_from_int ( len ),
                            scm_from_int ( 0 ),
                            scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
            return scm_ret;
        }
        
//         SCM scm_ret = scm_from_latin1_stringn(sjson.data(), sjson.size());
//         return scm_ret;
    }
    else
    {
		scm_wrong_type_arg_msg("mtfa-json-to-rlp", 0, scm_bvin, "bytevector");
    }
}


// // UniValue global_jval;
// // RLPValue global_rval;
// static bool parseJsonInput(const std::string& body)
// {
// 	bool rc = global_jval.read(body);
// 
// 	if (!rc)
// 		fprintf(stderr, "JSON input validation failed\n");
// 
// 	return rc;
// }
// 
// static bool parseRlpInput(const std::string& body)
// {
// 	std::vector<unsigned char> buf;
// 
// 	if (body.substr(0, 2) == "0x") {
// 		string tmp = body.substr(2);
// 		buf = ParseHex(tmp);
// 	} else
// 		buf = ParseHex(body);
// 
// 	size_t consumed, wanted;
// 	bool rc = global_rval.read(&buf[0], buf.size(), consumed, wanted);
// 
// 	if (!rc)
// 		fprintf(stderr, "RLP input validation failed (%zu wanted)\n",
// 			wanted);
// 
// 	return rc;
// }
// 
// static string getJsonOutput()
// {
// 	string body = global_jval.write(2);
// 	return body;
// }
// 
// static string getRlpOutput()
// {
// 	string body = global_rval.write();
// 	string hex = HexStr(body.begin(), body.end());
//     return hex;
// }
// 
// // extern void RLPtoJSON(const RLPValue& rval, UniValue& jval);
// // extern bool JSONtoRLP(const UniValue& jval, RLPValue& rval);
// #endif
// int 
// main ( int argc, char* argv[] ) {
//     unsigned long long start;
//     mtfa_random_generator_init (0); // GetULLTime() );
// 
// #ifdef RLP
// 
// 
//     string body = argv[1];
//     parseJsonInput(body);
//     JSONtoRLP(global_jval, global_rval);
//     
//     string rlp = getRlpOutput();
//     cout << "RLP: " << rlp << endl;
//     
//     parseRlpInput(rlp);
//     RLPtoJSON(global_rval, global_jval);
//     string json = getJsonOutput();
//     cout << "JSON: " << json << endl;
//     return 0;
// #endif
// 

//SHA3-KECCAK
/* 'Words' here refers to uint64_t */
#define SHA3_KECCAK_SPONGE_WORDS \
	(((1600)/8/*bits to byte*/)/sizeof(uint64_t))
typedef struct sha3_context_ {
    uint64_t saved;             /* the portion of the input message that we
                                 * didn't consume yet */
    union {                     /* Keccak's state */
        uint64_t s[SHA3_KECCAK_SPONGE_WORDS];
        uint8_t sb[SHA3_KECCAK_SPONGE_WORDS * 8];
    };
    unsigned byteIndex;         /* 0..7--the next byte after the set one
                                 * (starts from 0; 0--none are buffered) */
    unsigned wordIndex;         /* 0..24--the next word to integrate input
                                 * (starts from 0) */
    unsigned capacityWords;     /* the double size of the hash output in
                                 * words (e.g. 16 for Keccak 512) */
} sha3_context;

enum SHA3_FLAGS {
    SHA3_FLAGS_NONE=0,
    SHA3_FLAGS_KECCAK=1
};

enum SHA3_RETURN {
    SHA3_RETURN_OK=0,
    SHA3_RETURN_BAD_PARAMS=1
};
typedef enum SHA3_RETURN sha3_return_t;

/* For Init or Reset call these: */
static sha3_return_t sha3_Init(void *priv, unsigned bitSize);

static void sha3_Init256(void *priv);
static void sha3_Init384(void *priv);
static void sha3_Init512(void *priv);

enum SHA3_FLAGS sha3_SetFlags(void *priv, enum SHA3_FLAGS);

static void sha3_Update(void *priv, void const *bufIn, size_t len);

static void const *sha3_Finalize(void *priv);

/* Single-call hashing */
static sha3_return_t sha3_HashBuffer( 
    unsigned bitSize,   /* 256, 384, 512 */
    enum SHA3_FLAGS flags, /* SHA3_FLAGS_NONE or SHA3_FLAGS_KECCAK */
    const void *in, unsigned inBytes, 
    void *out, unsigned outBytes );     /* up to bitSize/8; truncation OK */

#define SHA3_ASSERT( x )
#if defined(_MSC_VER)
#define SHA3_TRACE( format, ...)
#define SHA3_TRACE_BUF( format, buf, l, ...)
#else
#define SHA3_TRACE(format, args...)
#define SHA3_TRACE_BUF(format, buf, l, args...)
#endif

/* 
 * This flag is used to configure "pure" Keccak, as opposed to NIST SHA3.
 */
#define SHA3_USE_KECCAK_FLAG 0x80000000
#define SHA3_CW(x) ((x) & (~SHA3_USE_KECCAK_FLAG))


#if defined(_MSC_VER)
#define SHA3_CONST(x) x
#else
#define SHA3_CONST(x) x##L
#endif

#ifndef SHA3_ROTL64
#define SHA3_ROTL64(x, y) \
	(((x) << (y)) | ((x) >> ((sizeof(uint64_t)*8) - (y))))
#endif

static const uint64_t keccakf_rndc[24] = {
    SHA3_CONST(0x0000000000000001UL), SHA3_CONST(0x0000000000008082UL),
    SHA3_CONST(0x800000000000808aUL), SHA3_CONST(0x8000000080008000UL),
    SHA3_CONST(0x000000000000808bUL), SHA3_CONST(0x0000000080000001UL),
    SHA3_CONST(0x8000000080008081UL), SHA3_CONST(0x8000000000008009UL),
    SHA3_CONST(0x000000000000008aUL), SHA3_CONST(0x0000000000000088UL),
    SHA3_CONST(0x0000000080008009UL), SHA3_CONST(0x000000008000000aUL),
    SHA3_CONST(0x000000008000808bUL), SHA3_CONST(0x800000000000008bUL),
    SHA3_CONST(0x8000000000008089UL), SHA3_CONST(0x8000000000008003UL),
    SHA3_CONST(0x8000000000008002UL), SHA3_CONST(0x8000000000000080UL),
    SHA3_CONST(0x000000000000800aUL), SHA3_CONST(0x800000008000000aUL),
    SHA3_CONST(0x8000000080008081UL), SHA3_CONST(0x8000000000008080UL),
    SHA3_CONST(0x0000000080000001UL), SHA3_CONST(0x8000000080008008UL)
};

static const unsigned keccakf_rotc[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62,
    18, 39, 61, 20, 44
};

static const unsigned keccakf_piln[24] = {
    10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20,
    14, 22, 9, 6, 1
};

/* generally called after SHA3_KECCAK_SPONGE_WORDS-ctx->capacityWords words 
 * are XORed into the state s 
 */
static void
keccakf(uint64_t s[25])
{
    int i, j, round;
    uint64_t t, bc[5];
#define KECCAK_ROUNDS 24

    for(round = 0; round < KECCAK_ROUNDS; round++) {

        /* Theta */
        for(i = 0; i < 5; i++)
            bc[i] = s[i] ^ s[i + 5] ^ s[i + 10] ^ s[i + 15] ^ s[i + 20];

        for(i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ SHA3_ROTL64(bc[(i + 1) % 5], 1);
            for(j = 0; j < 25; j += 5)
                s[j + i] ^= t;
        }

        /* Rho Pi */
        t = s[1];
        for(i = 0; i < 24; i++) {
            j = keccakf_piln[i];
            bc[0] = s[j];
            s[j] = SHA3_ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        /* Chi */
        for(j = 0; j < 25; j += 5) {
            for(i = 0; i < 5; i++)
                bc[i] = s[j + i];
            for(i = 0; i < 5; i++)
                s[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        /* Iota */
        s[0] ^= keccakf_rndc[round];
    }
}

/* *************************** Public Inteface ************************ */

/* For Init or Reset call these: */
sha3_return_t
sha3_Init(void *priv, unsigned bitSize) {
    sha3_context *ctx = (sha3_context *) priv;
    if( bitSize != 256 && bitSize != 384 && bitSize != 512 )
        return SHA3_RETURN_BAD_PARAMS;
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacityWords = 2 * bitSize / (8 * sizeof(uint64_t));
    return SHA3_RETURN_OK;
}

void
sha3_Init256(void *priv)
{
    sha3_Init(priv, 256);
}

void
sha3_Init384(void *priv)
{
    sha3_Init(priv, 384);
}

void
sha3_Init512(void *priv)
{
    sha3_Init(priv, 512);
}

enum SHA3_FLAGS
sha3_SetFlags(void *priv, enum SHA3_FLAGS flags)
{
    sha3_context *ctx = (sha3_context *) priv;
    flags &= SHA3_FLAGS_KECCAK;
    ctx->capacityWords |= (flags == SHA3_FLAGS_KECCAK ? SHA3_USE_KECCAK_FLAG : 0);
    return flags;
}


void
sha3_Update(void *priv, void const *bufIn, size_t len)
{
    sha3_context *ctx = (sha3_context *) priv;

    /* 0...7 -- how much is needed to have a word */
    unsigned old_tail = (8 - ctx->byteIndex) & 7;

    size_t words;
    unsigned tail;
    size_t i;

    const uint8_t *buf = bufIn;

    SHA3_TRACE_BUF("called to update with:", buf, len);

    SHA3_ASSERT(ctx->byteIndex < 8);
    SHA3_ASSERT(ctx->wordIndex < sizeof(ctx->s) / sizeof(ctx->s[0]));

    if(len < old_tail) {        /* have no complete word or haven't started 
                                 * the word yet */
        SHA3_TRACE("because %d<%d, store it and return", (unsigned)len,
                (unsigned)old_tail);
        /* endian-independent code follows: */
        while (len--)
            ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);
        SHA3_ASSERT(ctx->byteIndex < 8);
        return;
    }

    if(old_tail) {              /* will have one word to process */
        SHA3_TRACE("completing one word with %d bytes", (unsigned)old_tail);
        /* endian-independent code follows: */
        len -= old_tail;
        while (old_tail--)
            ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);

        /* now ready to add saved to the sponge */
        ctx->s[ctx->wordIndex] ^= ctx->saved;
        SHA3_ASSERT(ctx->byteIndex == 8);
        ctx->byteIndex = 0;
        ctx->saved = 0;
        if(++ctx->wordIndex ==
                (SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords))) {
            keccakf(ctx->s);
            ctx->wordIndex = 0;
        }
    }

    /* now work in full words directly from input */

    SHA3_ASSERT(ctx->byteIndex == 0);

    words = len / sizeof(uint64_t);
    tail = len - words * sizeof(uint64_t);

    SHA3_TRACE("have %d full words to process", (unsigned)words);

    for(i = 0; i < words; i++, buf += sizeof(uint64_t)) {
        const uint64_t t = (uint64_t) (buf[0]) |
                ((uint64_t) (buf[1]) << 8 * 1) |
                ((uint64_t) (buf[2]) << 8 * 2) |
                ((uint64_t) (buf[3]) << 8 * 3) |
                ((uint64_t) (buf[4]) << 8 * 4) |
                ((uint64_t) (buf[5]) << 8 * 5) |
                ((uint64_t) (buf[6]) << 8 * 6) |
                ((uint64_t) (buf[7]) << 8 * 7);
#if defined(__x86_64__ ) || defined(__i386__)
        SHA3_ASSERT(memcmp(&t, buf, 8) == 0);
#endif
        ctx->s[ctx->wordIndex] ^= t;
        if(++ctx->wordIndex ==
                (SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords))) {
            keccakf(ctx->s);
            ctx->wordIndex = 0;
        }
    }

    SHA3_TRACE("have %d bytes left to process, save them", (unsigned)tail);

    /* finally, save the partial word */
    SHA3_ASSERT(ctx->byteIndex == 0 && tail < 8);
    while (tail--) {
        SHA3_TRACE("Store byte %02x '%c'", *buf, *buf);
        ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);
    }
    SHA3_ASSERT(ctx->byteIndex < 8);
    SHA3_TRACE("Have saved=0x%016" PRIx64 " at the end", ctx->saved);
}

/* This is simply the 'update' with the padding block.
 * The padding block is 0x01 || 0x00* || 0x80. First 0x01 and last 0x80 
 * bytes are always present, but they can be the same byte.
 */
void const *
sha3_Finalize(void *priv)
{
    sha3_context *ctx = (sha3_context *) priv;

    SHA3_TRACE("called with %d bytes in the buffer", ctx->byteIndex);

    /* Append 2-bit suffix 01, per SHA-3 spec. Instead of 1 for padding we
     * use 1<<2 below. The 0x02 below corresponds to the suffix 01.
     * Overall, we feed 0, then 1, and finally 1 to start padding. Without
     * M || 01, we would simply use 1 to start padding. */

    uint64_t t;

    if( ctx->capacityWords & SHA3_USE_KECCAK_FLAG ) {
        /* Keccak version */
        t = (uint64_t)(((uint64_t) 1) << (ctx->byteIndex * 8));
    }
    else {
        /* SHA3 version */
        t = (uint64_t)(((uint64_t)(0x02 | (1 << 2))) << ((ctx->byteIndex) * 8));
    }

    ctx->s[ctx->wordIndex] ^= ctx->saved ^ t;

    ctx->s[SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords) - 1] ^=
            SHA3_CONST(0x8000000000000000UL);
    keccakf(ctx->s);

    /* Return first bytes of the ctx->s. This conversion is not needed for
     * little-endian platforms e.g. wrap with #if !defined(__BYTE_ORDER__)
     * || !defined(__ORDER_LITTLE_ENDIAN__) || __BYTE_ORDER__!=__ORDER_LITTLE_ENDIAN__ 
     *    ... the conversion below ...
     * #endif */
    {
        unsigned i;
        for(i = 0; i < SHA3_KECCAK_SPONGE_WORDS; i++) {
            const unsigned t1 = (uint32_t) ctx->s[i];
            const unsigned t2 = (uint32_t) ((ctx->s[i] >> 16) >> 16);
            ctx->sb[i * 8 + 0] = (uint8_t) (t1);
            ctx->sb[i * 8 + 1] = (uint8_t) (t1 >> 8);
            ctx->sb[i * 8 + 2] = (uint8_t) (t1 >> 16);
            ctx->sb[i * 8 + 3] = (uint8_t) (t1 >> 24);
            ctx->sb[i * 8 + 4] = (uint8_t) (t2);
            ctx->sb[i * 8 + 5] = (uint8_t) (t2 >> 8);
            ctx->sb[i * 8 + 6] = (uint8_t) (t2 >> 16);
            ctx->sb[i * 8 + 7] = (uint8_t) (t2 >> 24);
        }
    }

    SHA3_TRACE_BUF("Hash: (first 32 bytes)", ctx->sb, 256 / 8);

    return (ctx->sb);
}

sha3_return_t sha3_HashBuffer( unsigned bitSize, enum SHA3_FLAGS flags, const void *in, unsigned inBytes, void *out, unsigned outBytes ) {
    sha3_return_t err;
    sha3_context c;

    err = sha3_Init(&c, bitSize);
    if( err != SHA3_RETURN_OK )
        return err;
    if( sha3_SetFlags(&c, flags) != flags ) {
        return SHA3_RETURN_BAD_PARAMS;
    }
    sha3_Update(&c, in, inBytes);
    const void *h = sha3_Finalize(&c);

    if(outBytes > bitSize/8)
        outBytes = bitSize/8;
    memcpy(out, h, outBytes);
    return SHA3_RETURN_OK;
}

static SCM mtfa_hash_keccak(SCM bv)
{
    if ( !scm_is_bytevector ( bv ))
    {
		scm_wrong_type_arg_msg("mtfa-hash-keccak", 0, bv, "bytevector");
    }
    else
    {
        //in questo caso la cifra avviene sull'oggetto, senza duplicazioni
        int len = SCM_BYTEVECTOR_LENGTH ( bv );
        char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( bv );

        sha3_context c;
        const uint8_t *hash;
        int image_size=256; //256, 384, 512
        unsigned use_keccak = 1;

        switch(image_size) {
        case 256:
                sha3_Init256(&c);
            break;
        case 384:
                sha3_Init384(&c);
            break;
        case 512:
                sha3_Init512(&c);
            break;
        }

        if( use_keccak ) {
            enum SHA3_FLAGS flags2 = sha3_SetFlags(&c, SHA3_FLAGS_KECCAK);
            if( flags2 != SHA3_FLAGS_KECCAK )  {
                scm_wrong_type_arg_msg("mtfa-hash-keccak", 0, bv, "unable to set Keccak mode");
            }
        }
        sha3_Update(&c, (void*)p, len);
        hash = (uint8_t *)sha3_Finalize(&c);
        
        unsigned char * out = (unsigned char *)malloc(image_size/8);
        memcpy(out, hash, image_size/8);
        
        SCM scm_ret = scm_pointer_to_bytevector (
                    scm_from_pointer ( ( void* ) out, free ),
                    scm_from_int ( image_size/8 ),
                    scm_from_int ( 0 ),
                    scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
}

static SCM mtfa_eth_make_priv_key ()
{
    //genero, forte, una chiave privata
	mpz_class prk;
    unsigned char * uc_prk=nullptr;
    int uc_prk_len;
    secp256k1_context *both;

    both = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    do {
        if (uc_prk) free(uc_prk);
        Random::StrongRandom(256, prk);
        Mpz2UnsignedPChar(&prk, uc_prk, uc_prk_len);
    } while (secp256k1_ec_seckey_verify(both, uc_prk) != 1 || uc_prk_len != 32);
    secp256k1_context_destroy(both);
    
    //ora torniamo indietro il bytevector che rappresenta la chiave privata
    SCM scm_ret = scm_pointer_to_bytevector (
                scm_from_pointer ( ( void* ) uc_prk, free ),
                scm_from_int ( uc_prk_len ),
                scm_from_int ( 0 ),
                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_eth_get_pub_key(SCM bv_prkey)
{
   if ( !scm_is_bytevector ( bv_prkey ))
    {
		scm_wrong_type_arg_msg("mtfa_eth_get_pub_key", 0, bv_prkey, "bytevector");
    }
    else
    {
        int len = SCM_BYTEVECTOR_LENGTH ( bv_prkey );
        unsigned char * uc_prk = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( bv_prkey );

        //Crea il contesto
        secp256k1_context *both;
        both = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

        //Genera la chiave pubblica a partire dalla privata
        secp256k1_pubkey puk;
        memset(&puk.data, 0, sizeof(puk.data));
        int iret = secp256k1_ec_pubkey_create(both, &puk, uc_prk);
        
        //Distrugge il contesto
        secp256k1_context_destroy(both);
        
        if (iret == 1)
        {
            int uc_puk_len = sizeof(puk.data);
            unsigned char * uc_puk = (unsigned char *)malloc(uc_puk_len);
            memcpy(uc_puk, &puk.data[0], uc_puk_len);
            //potrei serializzare la chiave pubblica oppure estrarla direttamente e poi riassegnarla . La estraggo.
            SCM scm_ret = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) uc_puk, free ),
                        scm_from_int ( uc_puk_len ),
                        scm_from_int ( 0 ),
                        scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
            return scm_ret;
        }
        else
        {
            scm_wrong_type_arg_msg("mtfa_eth_get_pub_key", 0, bv_prkey, "pub key not extracted");
        }
    }
}

static SCM mtfa_eth_get_pub_key_serialized(SCM bv_prkey)
{
	if ( !scm_is_bytevector ( bv_prkey ))
    {
		scm_wrong_type_arg_msg("mtfa_eth_get_pub_key", 0, bv_prkey, "bytevector");
    }
    else
    {
        int len = SCM_BYTEVECTOR_LENGTH ( bv_prkey );
        unsigned char * uc_prk = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( bv_prkey );

        //Crea il contesto
        secp256k1_context *both;
        both = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

        //Genera la chiave pubblica a partire dalla privata
        secp256k1_pubkey puk;
        memset(&puk.data, 0, sizeof(puk.data));
        int iret = secp256k1_ec_pubkey_create(both, &puk, uc_prk);
        
        unsigned char serialized[65];
        size_t serialized_len = 65;

        iret = secp256k1_ec_pubkey_serialize(both, serialized, &serialized_len, &puk, SECP256K1_EC_UNCOMPRESSED);
        //cout << "SERIALIZED: " << serialized << "(" << serialized_len << ")" << endl;
        
        //Distrugge il contesto
        secp256k1_context_destroy(both);
        
        if (iret == 1)
        {
            int uc_puk_len = serialized_len; //sizeof(puk.data);
            unsigned char * uc_puk = (unsigned char *)malloc(uc_puk_len);
            memcpy(uc_puk, serialized /*&puk.data[0]*/, uc_puk_len);
            //potrei serializzare la chiave pubblica oppure estrarla direttamente e poi riassegnarla . La estraggo.
            SCM scm_ret = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) uc_puk, free ),
                        scm_from_int ( uc_puk_len ),
                        scm_from_int ( 0 ),
                        scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
            return scm_ret;
        }
        else
        {
            scm_wrong_type_arg_msg("mtfa_eth_get_pub_key", 0, bv_prkey, "pub key not extracted");
        }
    }
}


static SCM mtfa_eth_serialize_signature(SCM signature)
{
    secp256k1_context *sign;
    sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

    int uc_sign_len = SCM_BYTEVECTOR_LENGTH ( signature );
    unsigned char * uc_sign = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( signature );

    //secp256k1_ecdsa_signature firma;
    secp256k1_ecdsa_recoverable_signature firma;
    memcpy(firma.data, uc_sign, uc_sign_len);
    
    unsigned char ret64[64];
    int recid;
    secp256k1_ecdsa_recoverable_signature_serialize_compact(sign, ret64, &recid, &firma);

    //secp256k1_ecdsa_signature_serialize_compact(sign, ret64, &firma );
    secp256k1_context_destroy(sign);

    unsigned char * firmato = (unsigned char *)malloc(65);
    firmato[0] = recid;
    memcpy(&firmato[1], &ret64, 64);
    SCM scm_ret = scm_pointer_to_bytevector (
                scm_from_pointer ( ( void* ) firmato, free ),
                scm_from_int ( sizeof(firma.data) ),
                scm_from_int ( 0 ),
                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM mtfa_eth_sign_msg(SCM prk, SCM puk, SCM msg)
{
    secp256k1_context *sign;
    sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

    secp256k1_context *vrfy;
    vrfy = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);

    int uc_prk_len = SCM_BYTEVECTOR_LENGTH ( prk );
    unsigned char * uc_prk = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( prk );
    
    int uc_puk_len = SCM_BYTEVECTOR_LENGTH ( puk );
    unsigned char * uc_puk = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( puk );

    int uc_msg_len = SCM_BYTEVECTOR_LENGTH ( msg );
    unsigned char * uc_msg = ( unsigned char* ) SCM_BYTEVECTOR_CONTENTS ( msg );
    
    secp256k1_ecdsa_signature nr_firma;
    secp256k1_ecdsa_recoverable_signature firma;
    int iret = secp256k1_ecdsa_sign_recoverable(sign, &firma, uc_msg, uc_prk, nullptr, nullptr);
    secp256k1_ecdsa_recoverable_signature_convert(sign, &nr_firma, &firma);

    //int iret = secp256k1_ecdsa_sign(sign, &firma, uc_msg, uc_prk, nullptr, nullptr);
    secp256k1_context_destroy(sign);
    
    secp256k1_pubkey puk_struct;
    memset(&puk_struct.data[0], 0, sizeof(puk_struct.data));
    memcpy(&puk_struct.data[0], uc_puk, uc_puk_len);

    int ok = secp256k1_ecdsa_verify(vrfy, &nr_firma, uc_msg, &puk_struct);
    secp256k1_context_destroy(vrfy);
    if (ok)
    {
        unsigned char * firmato = (unsigned char *)malloc(sizeof(firma.data));
        memcpy(firmato, firma.data, sizeof(firma.data));
        SCM scm_ret = scm_pointer_to_bytevector (
                    scm_from_pointer ( ( void* ) firmato, free ),
                    scm_from_int ( sizeof(firma.data) ),
                    scm_from_int ( 0 ),
                    scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
    else
        return SCM_BOOL_F;
}

/*
    //Nearly every 256-bit number is a valid ECDSA private key. Specifically, any 256-bit number from 0x1 to 0xFFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFE BAAE DCE6 AF48 A03B BFD2 5E8C D036 4140 is a valid private key.

    cout << "Crea il contesto...\n";
    secp256k1_context *none;
    secp256k1_context *sign;
    secp256k1_context *vrfy;
    secp256k1_context *both;

    none = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    vrfy = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    both = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    
    //genero, forte, una chiave privata
	mpz_class prk;
    unsigned char * uc_prk=nullptr;
    int uc_prk_len;
    do {
        if (uc_prk) free(uc_prk);
        Random::StrongRandom(256, prk);
        Mpz2UnsignedPChar(prk, uc_prk, uc_prk_len);
    } while (secp256k1_ec_seckey_verify(both, uc_prk) != 1 || uc_prk_len != 32);
    
    string s_uc_prk = "c198d8873051ca5e487862a8ea7d7161cc5261cef00346b052222d973535528e";
    for (int i=0; i<32; ++i)
    {
        char v[3];
        v[0] = s_uc_prk[2*i];
        v[1] = s_uc_prk[2*i+1];
        v[2] = 0;
        uc_prk[i] = strtoul(v, nullptr, 16);
    }
    cout << "Chiave privata: ";
    for (int i=0; i<32; ++i)
    {
        printf("%02X", uc_prk[i]);
    }
    cout << endl;

    //Genera la chiave pubblica a partire dalla privata
    secp256k1_pubkey puk;
    memset(&puk.data, 0, sizeof(puk.data));
    int iret = secp256k1_ec_pubkey_create(both, &puk, uc_prk);
    cout << "Generazione chiave pubblica: " << iret << endl;
    
    //Firma!! un hash di un messaggio
    string s_h_msg = "272ce1996a98f7f8e5adbf0703f55eadff04634d037fe07d686bb510783e6726";
    unsigned char * hmessaggio = (unsigned char *)malloc(32);
    for (int i=0; i<32; ++i)
    {
        char v[3];
        v[0] = s_h_msg[2*i];
        v[1] = s_h_msg[2*i+1];
        v[2] = 0;
        hmessaggio[i] = strtoul(v, nullptr, 16);
    }
    for (auto i=0; i<32; ++i)
        printf("%02X", hmessaggio[i]);
    cout << endl;
    
    secp256k1_ecdsa_signature firma;
    secp256k1_ecdsa_sign(sign, &firma, hmessaggio, uc_prk, nullptr, nullptr);

    //dump della firma
    for (auto i=0; i<64; ++i)
        printf("%02X", firma.data[i]);
    cout << endl;
    int ok = secp256k1_ecdsa_verify(vrfy, &firma, hmessaggio, &puk);
    cout << "Firma ok: " << ok << endl;
    
    //distrugge hash messaggio
    free(hmessaggio);
    
    //Distrugge la chiave privata
    free(uc_prk);
    
    secp256k1_context_destroy(none);
    secp256k1_context_destroy(sign);
    secp256k1_context_destroy(vrfy);
    secp256k1_context_destroy(both);
    
    return 0;
*/

SCM mtfa_morton_16_encode(SCM r, SCM c)
{
    unsigned int mn = MortonEncode(scm_to_uint16(r), scm_to_uint16(c));
    return scm_from_uint32(mn);
}

SCM mtfa_morton_16_decode(SCM mn)
{
    unsigned short r,c;
    MortonDecode(scm_to_uint32(mn), r, c);
    return (scm_cons (scm_from_uint16(r), scm_from_uint16(c)));
}

SCM mtfa_morton_32_encode(SCM r, SCM c)
{
    uint64_t mn = libmorton::morton2D_64_encode(scm_to_uint32(c), scm_to_uint32(r));
    return scm_from_uint64(mn);
}

SCM mtfa_morton_32_decode(SCM mn)
{
    unsigned long int r,c;
    libmorton::morton2D_64_decode(scm_to_uint64(mn), c, r);
    return (scm_cons (scm_from_uint32(r), scm_from_uint32(c)));
}

static SCM mtfa_nop()
{
    return SCM_EOL;
}

extern "C" void init_mtfa_lib();
extern "C" void init_mtfa_lib()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-lib" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "ds" ) );
    finalizer = finalize_mtfa_lib;
    mtfa_lib_type = scm_make_foreign_object_type ( name, slots, finalizer );


    //carica la matrice
    loadmatrix();

    scm_c_define_gsubr ( "mtfa-rand", 0, 0, 0, ( void* ) mtfa_rand );
    scm_c_define_gsubr ( "mtfa-rand-d", 0, 0, 0, ( void* ) mtfa_rand_d );
    scm_c_define_gsubr ( "mtfa-rand-ui", 0, 0, 0, ( void* ) mtfa_rand_ui );
    scm_c_define_gsubr ( "mtfa-rand-seed", 1, 0, 0, ( void* ) mtfa_rand_seed );
    scm_c_define_gsubr ( "mtfa-rand-mat", 0, 0, 0, ( void* ) mtfa_rand_mat );
    scm_c_define_gsubr ( "mtfa-rand-mat-ui", 0, 0, 0, ( void* ) mtfa_rand_mat_ui );
    scm_c_define_gsubr ( "mtfa-rand-mat-d", 0, 0, 0, ( void* ) mtfa_rand_mat_d );
    scm_c_define_gsubr ( "mtfa-rand-mat-seed", 1, 0, 0, ( void* ) mtfa_rand_mat_seed );
    scm_c_define_gsubr ( "mtfa-rand-string", 1, 0, 0, ( void* ) local_mtfa_rand_string );
    scm_c_define_gsubr ( "mtfa-rand-alfanum", 2, 0, 0, ( void* ) local_mtfa_rand_alfanum );

    scm_c_define_gsubr ( "mtfa-cypher", 3, 0, 0, ( void* ) mtfa_cypher );
    scm_c_define_gsubr ( "mtfa-http-parse", 1, 0, 0, ( void* ) mtfa_http_parse );

    scm_c_define_gsubr ( "mtfa-b64-encode", 1, 0, 0, ( void* ) mtfa_b64_encode );
    scm_c_define_gsubr ( "mtfa-b64-url-encode", 1, 0, 0, ( void* ) mtfa_b64_url_encode );
    scm_c_define_gsubr ( "mtfa-b64-decode-s", 1, 0, 0, ( void* ) mtfa_b64_decode_s );
    scm_c_define_gsubr ( "mtfa-b64-decode-bv", 1, 0, 0, ( void* ) mtfa_b64_decode_bv );
    scm_c_define_gsubr ( "mtfa-b64-url-decode-s", 1, 0, 0, ( void* ) mtfa_b64_url_decode_s );
    scm_c_define_gsubr ( "mtfa-b64-url-decode-bv", 1, 0, 0, ( void* ) mtfa_b64_url_decode_bv );

    scm_c_define_gsubr ( "mtfa-run-ext-prog", 3, 0, 0, ( void* ) mtfa_run_ext_prog );
    scm_c_define_gsubr ( "mtfa-http-get-page-ex", 10, 0, 0, ( void* ) mtfa_http_get_page );

    scm_c_define_gsubr ( "mtfa-send-mail", 8, 0, 0, ( void* ) ex_mtfa_send_mail );

    scm_c_define_gsubr ( "mtfa-run-generator-string", 4, 0, 0, ( void* ) mtfa_run_generator_string );
    scm_c_define_gsubr ( "mtfa-run-generator-bv", 4, 0, 0, ( void* ) mtfa_run_generator_bv );
    scm_c_define_gsubr ( "mtfa-cypher-generator", 3, 0, 0, ( void* ) mtfa_cypher_generator );
    scm_c_define_gsubr ( "mtfa-base62", 1, 0, 0, ( void* ) local_mtfa_base_62 );
    scm_c_define_gsubr ( "mtfa-debase62", 1, 0, 0, ( void* ) local_mtfa_debase_62 );
    scm_c_define_gsubr ( "mtfa-sqlite3-do", 2, 0, 0, ( void* ) mtfa_sqlite3_do );
    scm_c_define_gsubr ( "mtfa-sqlite3-open", 1, 0, 0, ( void* ) mtfa_sqlite3_open );
    scm_c_define_gsubr ( "mtfa-sqlite3-close", 1, 0, 0, ( void* ) mtfa_sqlite3_close );
    scm_c_define_gsubr ( "mtfa-sqlite3-execute", 2, 0, 0, ( void* ) mtfa_sqlite3_execute );

    scm_c_define_gsubr ( "mtfa-hmac", 3, 0, 0, ( void* ) mtfa_hmac );
    scm_c_define_gsubr ( "mtfa-hash", 2, 0, 0, ( void* ) mtfa_sha );

    scm_c_define_gsubr ( "mtfa-cifra-aes-js", 2, 0, 0, ( void* ) mtfa_cifra_aes_js );
    scm_c_define_gsubr ( "mtfa-decifra-aes-js", 2, 0, 0, ( void* ) mtfa_decifra_aes_js );

    scm_c_define_gsubr ( "mtfa-base32-encode", 1, 0, 0, ( void* ) mtfa_base32_encode );

    scm_c_define_gsubr ( "mtfa-base32-decode", 1, 0, 0, ( void* ) mtfa_base32_decode );

    scm_c_define_gsubr ( "mtfa-eis-get-current-query", 1, 0, 0, ( void* ) mtfa_eis_get_current_query );
    scm_c_define_gsubr ( "mtfa-eis-get-value-current-query", 2, 0, 0, ( void* ) mtfa_eis_get_value_current_query );
    scm_c_define_gsubr ( "mtfa-eis-get-value-current-headers", 2, 0, 0, ( void* ) mtfa_eis_get_value_current_headers );
    scm_c_define_gsubr ( "mtfa-eis-get-value-current-cookies", 2, 0, 0, ( void* ) mtfa_eis_get_value_current_cookies );
	scm_c_define_gsubr ( "mtfa-eis-get-current-headers", 1, 0, 0, ( void* ) mtfa_eis_get_current_headers);
	scm_c_define_gsubr ( "mtfa-eis-get-current-uri", 1, 0, 0, ( void* ) mtfa_eis_get_current_uri);
	scm_c_define_gsubr ( "mtfa-eis-get-current-body", 2, 0, 0, ( void* ) mtfa_eis_get_current_body );
    scm_c_define_gsubr ( "mtfa-eis-get-current-method", 1, 0, 0, ( void* ) mtfa_eis_get_current_method );
    scm_c_define_gsubr ( "mtfa-eis-get-current-protocol", 1, 0, 0, ( void* ) mtfa_eis_get_current_protocol );
    scm_c_define_gsubr ( "mtfa-eis-get-current-host", 1, 0, 0, ( void* ) mtfa_eis_get_current_host );
    scm_c_define_gsubr ( "mtfa-eis-get-current-port", 1, 0, 0, ( void* ) mtfa_eis_get_current_port );
    scm_c_define_gsubr ( "mtfa-eis-get-current-pars", 1, 0, 0, ( void* ) mtfa_eis_get_current_pars );
    scm_c_define_gsubr ( "mtfa-eis-get-current-tag", 1, 0, 0, ( void* ) mtfa_eis_get_current_tag );
    scm_c_define_gsubr ( "mtfa-eis-get-current-url", 1, 0, 0, ( void* ) mtfa_eis_get_current_url );
    scm_c_define_gsubr ( "mtfa-eis-get-current-user-passwd", 1, 0, 0, ( void* ) mtfa_eis_get_current_user_passwd );
    scm_c_define_gsubr ( "mtfa-eis-get-current-mac-src", 1, 0, 0, ( void* ) mtfa_eis_get_current_mac_src );
    scm_c_define_gsubr ( "mtfa-eis-get-current-ip-src", 1, 0, 0, ( void* ) mtfa_eis_get_current_ip_src );
    scm_c_define_gsubr ( "mtfa-eis-get-current-ip-dst", 1, 0, 0, ( void* ) mtfa_eis_get_current_ip_dst );
    scm_c_define_gsubr ( "mtfa-eis-get-current-port-src", 1, 0, 0, ( void* ) mtfa_eis_get_current_port_src );
    scm_c_define_gsubr ( "mtfa-eis-get-current-port-dst", 1, 0, 0, ( void* ) mtfa_eis_get_current_port_dst );
    scm_c_define_gsubr ( "mtfa-eis-get-current-multipart", 1, 0, 0, ( void* ) mtfa_eis_get_current_multipart );
    scm_c_define_gsubr ( "mtfa-eis-get-current-ssl-servername", 1, 0, 0, ( void* ) mtfa_eis_get_current_ssl_servername);

    scm_c_define_gsubr ( "mtfa-eis-get-ip-network-protocol", 1, 0, 0, ( void* ) mtfa_eis_get_current_network_protocol );
    scm_c_define_gsubr ( "mtfa-eis-set-ip-real-data-size", 2, 0, 0, ( void* ) mtfa_eis_set_real_data_size );
    scm_c_define_gsubr ( "mtfa-eis-get-ip-data-len", 1, 0, 0, ( void* ) mtfa_eis_get_tcp_data_len );
    scm_c_define_gsubr ( "mtfa-eis-get-ip-id-probe", 1, 0, 0, ( void* ) mtfa_eis_get_id_sonda );
    
    scm_c_define_gsubr ( "mtfa-eis-enable-pbuf", 3, 0, 0, (void*)mtfa_eis_enable_pbuf);

    scm_c_define_gsubr ( "mtfa-eis-get-raw-data", 1, 0, 0, ( void* ) mtfa_eis_get_raw_data);
    scm_c_define_gsubr ( "mtfa-eis-put-raw-data", 2, 0, 0, ( void* ) mtfa_eis_put_raw_data);

    scm_c_define_gsubr ( "mtfa-get-current-thread", 0, 0, 0, ( void* ) mtfa_get_current_thread );

    //Da NumberTheory
    scm_c_define_gsubr ( "mtfa-strong-random", 1, 0, 0, ( void* ) mtfa_strong_random );
    scm_c_define_gsubr ( "mtfa-make-rsa", 0, 0, 0, ( void* ) mtfa_make_rsa );
    scm_c_define_gsubr ( "mtfa-rsa-pub-cipher", 3, 0, 0, ( void* ) rsa_PubCifra );
    scm_c_define_gsubr ( "mtfa-rsa-pub-decipher", 3, 0, 0, ( void* ) rsa_PubDecifra );
    scm_c_define_gsubr ( "mtfa-rsa-priv-cipher", 3, 0, 0, ( void* ) rsa_PrivCifra );
    scm_c_define_gsubr ( "mtfa-rsa-priv-decipher", 3, 0, 0, ( void* ) rsa_PrivDecifra );
    scm_c_define_gsubr ( "mtfa-rsa-sign", 2, 0, 0, ( void* ) rsa_Sign );
    scm_c_define_gsubr ( "mtfa-rsa-verify", 3, 0, 0, ( void* ) rsa_Verify );
    scm_c_define_gsubr ( "mtfa-rsa-make-key", 2, 0, 0, ( void* ) rsa_MakeKey );
    scm_c_define_gsubr ( "mtfa-rsa-make-key-p-q", 4, 0, 0, ( void* ) rsa_MakeKey_p_q );

    scm_c_define_gsubr ( "mtfa-rsa-get-pub-key", 1, 0, 0, ( void* ) rsa_GetPublic );
    scm_c_define_gsubr ( "mtfa-rsa-get-priv-key", 1, 0, 0, ( void* ) rsa_GetPrivate );
    scm_c_define_gsubr ( "mtfa-rsa-set-pub-key", 2, 0, 0, ( void* ) rsa_SetPubKey );
    scm_c_define_gsubr ( "mtfa-rsa-set-priv-key", 2, 0, 0, ( void* ) rsa_SetPrivKey );

    scm_c_define_gsubr ( "mtfa-nop", 0, 0, 0, ( void* ) mtfa_nop );

    scm_c_define_gsubr ( "mtfa-mpz-probab-prime-p", 2, 0, 0, ( void* ) mtfa_mpz_probab_prime_p );
    scm_c_define_gsubr ( "mtfa-mpz-nextprime", 1, 0, 0, ( void* ) mtfa_mpz_nextprime );
    scm_c_define_gsubr ( "mtfa-mpz-gcdext", 2, 0, 0, ( void* ) mtfa_mpz_gcdext );
    scm_c_define_gsubr ( "mtfa-mpz-invert", 2, 0, 0, ( void* ) mtfa_mpz_invert );
    scm_c_define_gsubr ( "mtfa-mpz-factorize", 2, 0, 0, ( void* ) mtfa_mpz_factorize);

    scm_c_define_gsubr ( "mtfa-compress", 2, 0, 0, ( void* ) scm_mtfa_compress);
    scm_c_define_gsubr ( "mtfa-uncompress", 1, 0, 0, ( void* ) scm_mtfa_uncompress);
    scm_c_define_gsubr ( "mtfa-cookie-make", 8, 0, 0, (void*) make_cookie);

	scm_c_define_gsubr ( "mtfa-find-all-positions", 2, 1, 0, (void*) mtfa_find_all_positions);
	scm_c_define_gsubr ( "mtfa-find-all-matching-positions", 2, 1, 0, (void*) mtfa_find_all_matching_positions);
	
    scm_c_define_gsubr ( "mtfa-blum-make", 1, 0, 0, (void*)mtfa_blum_make);
    scm_c_define_gsubr ( "mtfa-blum-get-values", 1, 0, 0, (void*)mtfa_blum_get_values);
    scm_c_define_gsubr ( "mtfa-blum-get-next", 2, 0, 0, (void*)mtfa_blum_get_next);
    scm_c_define_gsubr ( "mtfa-blum-get-prev", 2, 0, 0, (void*)mtfa_blum_get_prev);
    scm_c_define_gsubr ( "mtfa-blum-get-next-ith", 3, 0, 0, (void*)mtfa_blum_get_next_ith);
    scm_c_define_gsubr ( "mtfa-blum-get-prev-ith", 3, 0, 0, (void*)mtfa_blum_get_prev_ith);
	scm_c_define_gsubr ( "mtfa-blum-get-next-ith-slow", 3, 0, 0, (void*)mtfa_blum_get_next_ith_slow);
	scm_c_define_gsubr ( "mtfa-blum-set-values", 4, 0, 0, (void*)mtfa_blum_set_values);

	scm_c_define_gsubr ( "mtfa-bv-to-num", 1, 0, 0, (void*)mtfa_bv_to_num);
	scm_c_define_gsubr ( "mtfa-bv-to-hex", 1, 0, 0, (void*)mtfa_bv_to_hex);
	scm_c_define_gsubr ( "mtfa-num-to-bv", 1, 0, 0, (void*)mtfa_num_to_bv);
	scm_c_define_gsubr ( "mtfa-blum-make-token", 3, 0, 0, (void*)mtfa_blum_make_token);
	scm_c_define_gsubr ( "mtfa-blum-identify-token", 2, 0, 0, (void*)mtfa_blum_identify_token);

	scm_c_define_gsubr ( "mtfa-fuzzy-find", 3, 0, 0, (void*)mtfa_fuzzy_find_guile);
	scm_c_define_gsubr ( "mtfa-replace-all", 4, 1, 0, (void*)mtfa_replace_all);
	
	scm_c_define_gsubr ( "mtfa-magic-mime", 1, 0, 0, (void*)mtfa_magic_mime);
	scm_c_define_gsubr ( "mtfa-mime-from-extension", 1, 0, 0, (void*)mtfa_mime_from_extension);
	scm_c_define_gsubr ( "mtfa-charset-detect", 1, 0, 0, (void*)mtfa_detect_charset);

    scm_c_define_gsubr ( "mtfa-compile-pattern", 1, 1, 0, (void*)scm_mtfa_compile_pattern);

    scm_c_define_gsubr ( "mtfa-http-compress", 2, 0, 0, (void*)mtfa_http_compress);
    scm_c_define_gsubr ( "mtfa-http-uncompress", 1, 0, 0, (void*)mtfa_http_uncompress);

//    scm_c_define_gsubr ( "mtfa-scmjson-to-rlp::internal", 1, 0, 0, (void*)mtfa_scmjson_to_rlp);
    scm_c_define_gsubr ( "mtfa-rlp-to-json::internal", 1, 0, 0, (void*)mtfa_rlp_to_json);
    scm_c_define_gsubr ( "mtfa-hash-keccak", 1, 0, 0, (void*)mtfa_hash_keccak);
    
    scm_c_define_gsubr ( "mtfa-eth-make-priv-key", 0, 0, 0, (void*)mtfa_eth_make_priv_key);
    scm_c_define_gsubr ( "mtfa-eth-get-pub-key", 1, 0, 0, (void*)mtfa_eth_get_pub_key);
    scm_c_define_gsubr ( "mtfa-eth-get-pub-key-serialized", 1, 0, 0, (void*)mtfa_eth_get_pub_key_serialized);
    scm_c_define_gsubr ( "mtfa-eth-sign-msg", 3, 0, 0, (void*)mtfa_eth_sign_msg);
    scm_c_define_gsubr ( "mtfa-eth-serialize-signature", 1, 0, 0, (void*)mtfa_eth_serialize_signature);
//
    scm_c_define_gsubr ( "mtfa-morton-16-encode", 2, 0, 0, (void*)mtfa_morton_16_encode);
    scm_c_define_gsubr ( "mtfa-morton-16-decode", 1, 0, 0, (void*)mtfa_morton_16_decode);
    scm_c_define_gsubr ( "mtfa-morton-32-encode", 2, 0, 0, (void*)mtfa_morton_32_encode);
    scm_c_define_gsubr ( "mtfa-morton-32-decode", 1, 0, 0, (void*)mtfa_morton_32_decode);

}
