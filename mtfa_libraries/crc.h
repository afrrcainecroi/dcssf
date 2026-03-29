#pragma once

#include <iostream>
#include <string>

using namespace std;

// Per rendere il dato un pochino piu' interessante, tutti gli input diventano grandi almeno 8 bytes
// CRC CLASSE
class CrcClass
{
#define QUOTIENT 0x04c11db7
private:
    unsigned int wombat ( unsigned char * data,
                          int             len );

    unsigned int crctab[256];

    // bool crc_class_initialized = false;

public:
    CrcClass();

    unsigned int crc ( unsigned char data );

    unsigned int crc ( unsigned short data );

    unsigned int crc ( unsigned int data );

    unsigned int crc ( unsigned char * data,
                       int             len );

    unsigned int crc ( char data );

    unsigned int crc ( short data );

    unsigned int crc ( int data );

    unsigned int crc ( char * data,
                       int    len );
};


class av_crc
{
private:

    // static unsigned int ctx_b[];
    static unsigned int ctx[];

public:
    static unsigned int crc_full ( const string & buffer );

    static unsigned int crc_full ( const unsigned char * buffer,
                                   int                   length );

    static unsigned int crc_continue ( unsigned int          crc,
                                       const unsigned char * buffer,
                                       int                   length );
};


void mhash_clear_crc32 ( unsigned int * crc );

void mhash_get_crc32 ( const unsigned int * crc,
                       void *               ret );

void mhash_crc32 ( unsigned int * crc,
                   const void *   given_buf,
                   unsigned int   len );

void mhash_crc32b ( unsigned int * crc,
                    const void *   given_buf,
                    unsigned int   len );

unsigned short ip_checksum ( unsigned char * addr,
                             int             len );

unsigned short close_incremental_ip_checksum ( int & old_cksum );

void incremental_ip_checksum ( int &           old_cksum,
                               unsigned char * addr,
                               int             len );

// ****************************
// UNA FUNZIONE HASH MOLTO MOLTO VELOCE
// ****************************
typedef enum { OK = 0, XXH_ERROR } XXH_errorcode;

unsigned int XXH32 ( const void * input,
                     int          len,
                     unsigned int seed );

void * XXH32_init ( unsigned int seed );

XXH_errorcode XXH32_update ( void *       state,
                             const void * input,
                             int          len );

unsigned int XXH32_digest ( void * state );

int XXH32_sizeofState();

XXH_errorcode XXH32_resetState ( void *       state_in,
                                 unsigned int seed );

unsigned int XXH32_intermediateDigest ( void * state );

