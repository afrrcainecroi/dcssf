#pragma once

#include <stdlib.h>
#include <string>

using namespace std;

// THE ALPHABET -> ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
// You can use as terminator the other characters . , -
class Base64
{
public:
    static unsigned int decode ( unsigned char ** dest,
                                 string        src,
                                 char             terminator = '=' );

    static string decode ( const string &src,
                           char            terminator = '=' );

    static string encode ( unsigned int    size,
                           unsigned char * src,
                           char            terminator = '=' );

    static string encode ( const string &src,
                           char            terminator = '=' );

    static int s_decode ( char *  src,
                          int     len_in,
                          char ** dest,
                          int *   len_out );

    static int s_encode ( char *  src,
                          int     len_in,
                          char ** dest,
                          int *   len_out );

    static int s_decode_len ( const string &src,
                              int *           len );

    static int s_encode_len ( const string &src,
                              int *           len );

    static unsigned int GetDecodedLength ( unsigned int size );

    static unsigned int GetEncodedLength ( unsigned int size );
};
