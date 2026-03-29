#pragma once

#include <string>
#include <list>
#include <set>
#include <tuple>

#include "http_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 0
//# define HTTP_PARSER_STRICT 1 slower but more precise
#endif

class c_http_parser
{
public:
    http_parser_settings settings;
    http_parser *parser;

    char * last_field;
    int len_last_field;
    std::list<std::tuple<char *, int, char *, int> > headers;
    bool headers_complete;
    bool message_complete;
    struct http_parser_url parsed_url;
    char * body_from;
    int body_len;
    char * url_start;

    c_http_parser()
    {
        settings.on_body = my_body_callback;
        settings.on_headers_complete = my_headers_complete_callback;
        settings.on_header_field = my_header_field_callback;
        settings.on_header_value = my_header_value_callback;
        settings.on_message_begin = my_message_begin_callback;
        settings.on_message_complete = my_message_complete_callback;
        settings.on_status = my_status_complete_callback;
        settings.on_url = my_url_callback;

        parser = ( http_parser * ) malloc ( sizeof ( http_parser ) );
        memset ( parser, 0, sizeof ( http_parser ) );
        http_parser_init ( parser, HTTP_BOTH );
        parser->data = this;
        last_field = nullptr;
        len_last_field = 0;
        headers.clear();
        headers_complete = false;
        message_complete = false;
        memset ( &parsed_url, 0, sizeof ( parsed_url ) );

        body_from=nullptr;
        body_len=0;
        url_start=nullptr;
    }

    ~c_http_parser()
    {
        parser->data = nullptr;
        free ( parser );
        parser = nullptr;
        last_field = nullptr;
        len_last_field = 0;
        headers_complete = false;
        message_complete = false;
        if (body_from && body_len)
        {
            free(body_from);
        }
        body_from=nullptr;
        body_len=0;
        url_start=nullptr;
    }

//
//Oggetto: l'accesso a basi dati e relativi sistemi vs del...
//La password pu\u00f2 essere disattivata in quanto \u00e8 stata utilizzata in modo temporaneo su vostra richiesta.class
//le password non sono mai state utilizzate e quindi si possono dismettere.
//le credenziali non sono in uso e quindi si possono dismettere.
//

    int Parse ( char * data, int len )
    {
        return http_parser_execute ( parser, &settings, data, len );
    }

    static int my_url_callback ( http_parser* parser, const char *at, size_t length )
    {
        //	string s;
        //	s.assign(at, at+length);
        ////	printf("Presa url. La parso con il pacchettino!: %s\n", s.c_str());
        //		struct http_parser_url u;
        ( ( c_http_parser* ) parser->data )->url_start = ( char* ) at;
        int iet = http_parser_parse_url ( at, length, 0, & ( ( c_http_parser* ) parser->data )->parsed_url );
        //	for (int i=0; i<UF_MAX; ++i)
        //	{
        //		if (parsed_url.field_set & (1 << i))
        //		{
        //			string s;
        //			s.assign(at+u.field_data[i].off, at+u.field_data[i].off+u.field_data[i].len);
        ////			printf("Parte %2d della url: <%s>\n", i, s.c_str());
        //		}
        //		else
        //			break;
        //	}
        return 0;
    }

    static int my_header_field_callback ( http_parser* parser, const char *at, size_t length )
    {
        ( ( c_http_parser* ) parser->data )->last_field = ( char * ) at;
        ( ( c_http_parser* ) parser->data )->len_last_field = ( int ) length;
        return 0;
    }

    static int my_body_callback ( http_parser* parser, const char *at, size_t length )
    {
        auto &bf = ( ( c_http_parser* ) parser->data )->body_from;
        auto & bl = ( ( c_http_parser* ) parser->data )->body_len;
        if (bf == 0)
        {
            bf = (char*)malloc(length);
            memcpy(bf, at, length);
            bl = length;
        }
        else
        {
            bf = (char *)realloc(bf, bl+length);
            memcpy(&bf[bl], at, length);
            bl += length;
        }
        return 0;
    }

    static int my_headers_complete_callback ( http_parser* parser )
    {
        ( ( c_http_parser* ) parser->data )->headers_complete = true;
        return 0;
    }

    static int my_header_value_callback ( http_parser* parser, const char *at, size_t length )
    {
        if ( ( ( c_http_parser* ) parser->data )->last_field != nullptr ) {
            ( ( c_http_parser* ) parser->data )->headers.push_back ( std::make_tuple (
                        ( ( c_http_parser* ) parser->data )->last_field,
                        ( ( c_http_parser* ) parser->data )->len_last_field,
                        ( char* ) at,
                        length ) );
        }
        ( ( c_http_parser* ) parser->data )->last_field = nullptr;
        ( ( c_http_parser* ) parser->data )->len_last_field = 0;
        return 0;
    }

    static int my_message_begin_callback ( http_parser* parser )
    {
        return 0;
    }

    static int my_message_complete_callback ( http_parser* parser )
    {
        ( ( c_http_parser* ) parser->data )->message_complete = true;
        return 0;
    }

    static int my_status_complete_callback ( http_parser* parser, const char *at, size_t length )
    {
        return 0;
    }

};


#ifdef __cplusplus
}
#endif

