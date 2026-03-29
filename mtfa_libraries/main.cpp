//TODO: verificare il corretto funzionamento di extended_bitset in lisp (mtfa-su)
//OK: 24 5 2018
//TODO: verificare il corretto funzionamento di mtfa_star sia in VA sia in lisp
//OK: 24 5 2018, OK in LISP, sembra OK anche in VA
//TODO: verificare il corretto funzionamento del riconoscimento della richiesta hostname in ssl
//OK: 25 5 2018, OK in LISP, sembra OK anche in VA
//TODO: verificare le configurazioni dei livelli di sicurezza ssl in lb-user per gestire anche stringhe di configurazione tipo apache
//OK: 25 5 2018, OK in LISP, sembra OK anche in VA

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <fstream>
#include <streambuf>

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <set>
#include <bitset>
#include <thread>
#include <boost/dynamic_bitset.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <iostream>
#include <map>
#include <cassert>



//#include <parallel/algorithm>
//#include <parallel/set_operations.h>

#include "mtfafs.h"
#include "mtfa_fs3.h"
#include "mtfa_utils.h"
#include "mtfabrg.h"
#include "mtfa_db.h"
#include "mtfa_qt.h"
#include "morton.h"

//#include "netfilter_queue.h"
#include "mtfa_ph.h"
#include "base64.h"
#include "NumberTheory.h"
#include "hashmap.hpp"
#include "sendmail.h"

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <magic.h>

#include <memory>
#include "pstream.h"

#include <boost/coroutine2/coroutine.hpp>

//La sezione RLP!!
#include <rlpvalue.h>
#include <univalue.h>
#include "InfInt.h"
#include <secp256k1.h>

using namespace std;

//SimpleRng * rng;
mtfa_fastrng1024* rng;

#define BASE_DIM 10000

class MyRNG {
    public:
        typedef size_t result_type;
        static size_t min() {
            return 0;
            }
        static size_t max() {
            return 0xFFFFFFFFFFFFFFFF;
            }
        size_t operator() () {
            // generate a random number in the range [0, 42]
            return rng->next();
            }
    };



template <class T>
void mtfa_shuffle ( T inizio, T fine, unsigned long int ( *rndgen ) () ) {
    rndgen = rndgen;
    typename std::iterator_traits<T>::difference_type i;

    i = fine - inizio;

    while ( i > 1 ) {
        int j = rng->next() % --i;
        std::swap ( inizio[i], inizio[j] );
        }

    return;
    }

//#define CHECK_CIFRA 1

#ifdef CHECK_CIFRA
/**
 * Create an 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init ( unsigned char* key_data, int key_data_len, unsigned char* salt, EVP_CIPHER_CTX* e_ctx,
               EVP_CIPHER_CTX* d_ctx );

int aes_init ( unsigned char* key_data, int key_data_len, unsigned char* salt, EVP_CIPHER_CTX* e_ctx,
               EVP_CIPHER_CTX* d_ctx ) {
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey ( EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv );

    if ( i != 32 ) {
        printf ( "Key size is %d bits - should be 256 bits\n", i );
        return -1;
        }

    EVP_CIPHER_CTX_init ( e_ctx );
    EVP_EncryptInit_ex ( e_ctx, EVP_aes_256_cbc(), NULL, key, iv );
    EVP_CIPHER_CTX_init ( d_ctx );
    EVP_DecryptInit_ex ( d_ctx, EVP_aes_256_cbc(), NULL, key, iv );

    return 0;
    }

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char* aes_encrypt ( EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len );
unsigned char* aes_encrypt ( EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len ) {
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char* ciphertext = ( unsigned char* ) malloc ( c_len );

    /* allows reusing of 'e' for multiple encryption cycles */
    EVP_EncryptInit_ex ( e, NULL, NULL, NULL, NULL );

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
     *len is the size of plaintext in bytes */
    EVP_EncryptUpdate ( e, ciphertext, &c_len, plaintext, *len );

    /* update ciphertext with the final remaining bytes */
    EVP_EncryptFinal_ex ( e, ciphertext + c_len, &f_len );

    *len = c_len + f_len;
    return ciphertext;
    }

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char* aes_decrypt ( EVP_CIPHER_CTX* e, unsigned char* ciphertext, int* len );

unsigned char* aes_decrypt ( EVP_CIPHER_CTX* e, unsigned char* ciphertext, int* len ) {
    /* because we have padding ON, we must allocate an extra cipher block size of memory */
    int p_len = *len, f_len = 0;
    unsigned char* plaintext = ( unsigned char* ) malloc ( p_len + AES_BLOCK_SIZE );

    EVP_DecryptInit_ex ( e, NULL, NULL, NULL, NULL );
    EVP_DecryptUpdate ( e, plaintext, &p_len, ciphertext, *len );
    EVP_DecryptFinal_ex ( e, plaintext + p_len, &f_len );

    *len = p_len + f_len;
    return plaintext;
    }

int aes_main ( int argc, char** argv );

int aes_main ( int argc, char** argv ) {
    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
     status of enc/dec operations */
    EVP_CIPHER_CTX en, de;

    /* 8 bytes to salt the key_data during key generation. This is an example of
     compiled in salt. We just read the bit pattern created by these two 4 byte
     integers on the stack as 64 bits of contigous salt material -
     ofcourse this only works if sizeof(int) >= 4 */
    unsigned int salt[] = {12345, 54321};
    unsigned char* key_data;
    int key_data_len, i;
    char* input[] = {"a", "abcd", "this is a test", "this is a bigger test",
                     "\nWho are you ?\nI am the 'Doctor'.\n'Doctor' who ?\nPrecisely!",
                     "Made with Gibberish\n",
                     NULL
                    };

    /* the key_data is read from the argument list */
    key_data = ( unsigned char* ) argv[1];
    key_data_len = strlen ( argv[1] );

    /* gen key and iv. init the cipher ctx object */
    if ( aes_init ( key_data, key_data_len, ( unsigned char* ) &salt, &en, &de ) ) {
        printf ( "Couldn't initialize AES cipher\n" );
        return -1;
        }

    /* encrypt and decrypt each input string and compare with the original */
    for ( i = 0; input[i]; i++ ) {
        char* plaintext;
        unsigned char* ciphertext;
        int olen, len;

        /* The enc/dec functions deal with binary data and not C strings. strlen() will
         return length of the string without counting the '\0' string marker. We always
         pass in the marker byte to the encrypt/decrypt functions so that after decryption
         we end up with a legal C string */
        olen = len = strlen ( input[i] ) + 1;

        ciphertext = aes_encrypt ( &en, ( unsigned char* ) input[i], &len );
        cout << __func__   << Base64::encode ( len, ciphertext ) << endl;

        string cif = Base64::decode ( "U2FsdGVkX1+21O5RB08bavFTq7Yq/gChmXrO3f00tvJaT55A5pPvqw0zFVnHSW1o" );
        len = cif.size();
        plaintext = ( char* ) aes_decrypt ( &de, ( unsigned char* ) cif.data(), &len );
        //plaintext = (char *)aes_decrypt(&de, ciphertext, &len);

        if ( strncmp ( plaintext, input[i], olen ) ) {
            printf ( "FAIL: enc/dec failed for \"%s\"\n", input[i] );
            }
        else {
            printf ( "OK: enc/dec ok for \"%s\"\n", plaintext );
            }

        free ( ciphertext );
        free ( plaintext );
        }

    EVP_CIPHER_CTX_cleanup ( &en );
    EVP_CIPHER_CTX_cleanup ( &de );

    return 0;
    }
#endif

#include "mtfa_utils.h"
#include "mtfa_find.h"
void DoTime ( const string op, const string where, long long int DIM, unsigned long long start, unsigned long long end );

#if 0
/*LATEST VERSION*/
class Pushing {
    public:
        vector<unsigned char> v;
        unsigned char byte;
        int numbit;
        Pushing() {
            v.clear();
            byte = 0;
            numbit = 0;
            }
        void push ( int bit ) {
            byte = ( byte << 1 ) | bit;
            numbit++;

            if ( numbit >= 8 ) {
                v.push_back ( byte );
                byte = 0;
                numbit = 0;
                }
            }
    };


void PreparePa ( BSetUnion& nva, Pushing& pa );
void PreparePa ( BSetUnion& nva, Pushing& pa ) {
    for ( int i = 0; i < nva.size; ++i ) {
        if ( nva.Occupied ( i ) <= 0 )
            continue;

        bitset<64> bsl ( nva.values_left[i] );
        bitset<64> bsr ( nva.values_right[i] );

        if ( nva.occupied_left[i] > 0 ) {
            auto s = bsl.to_string().substr ( 64 - nva.occupied_left[i] );

            for ( auto c : s )
                pa.push ( ( c == '0' ) ? 0 : 1 );
            }

        if ( nva.occupied_right[i] > 0 ) {
            auto s = bsr.to_string().substr ( 64 - nva.occupied_right[i] );

            for ( auto c : s )
                pa.push ( ( c == '0' ) ? 0 : 1 );
            }
        }
    }

void ScriviPa ( int nfou, Pushing& pa );
void ScriviPa ( int nfou, Pushing& pa ) {
    unsigned char* ppa = &pa.v[0];
    long scritti = 0;

    while ( scritti < pa.v.size() ) {
        long l = write ( nfou, &ppa[scritti], pa.v.size() - scritti );
        scritti += l;
        }
    }

void ScriviString ( int nfou, string& pa );
void ScriviString ( int nfou, string& pa ) {
    long scritti = 0;

    while ( scritti < pa.size() ) {
        long l = write ( nfou, &pa.data() [scritti], pa.size() - scritti );
        scritti += l;
        }
    }

extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];

//Metà chiave
static const int HALF_KEY_SIZE = 1024;

class KGenerator {
    private:
    public:
        KGenerator() {};
        int Log2 ( unsigned long long _v ) {
            auto v = ( unsigned long ) _v;
            static const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
            static const unsigned int S[] = {1, 2, 4, 8, 16};
            int i;

            unsigned int r = 0; // result of log2(v) will go here

            if ( v & b[4] ) {
                v >>= S[4];
                r |= S[4];
                }

            if ( v & b[3] ) {
                v >>= S[3];
                r |= S[3];
                }

            if ( v & b[2] ) {
                v >>= S[2];
                r |= S[2];
                }

            if ( v & b[1] ) {
                v >>= S[1];
                r |= S[1];
                }

            if ( v & b[0] ) {
                v >>= S[0];
                r |= S[0];
                }

            //     for ( i = 4; i >= 0; i-- ) // unroll for speed...
            //     {
            //         if ( v & b[i] )
            //         {
            //             v >>= S[i];
            //             r |= S[i];
            //         }
            //     }

            return r;
            }

        string ToString ( const vector<bool>& v ) {
            string s;

            for ( auto it : v )
                s = s + ( it ? "1" : "0" );

            return s;
            }

        void RotateBit ( bool& a,
                         bool& b,
                         bool& a1,
                         bool& b1,
                         bool direction
                       ) {
            bool tmp = a;

            if ( direction ) {
                //Destra
                a = a1;
                a1 = b1;
                b1 = b;
                b = tmp;
                }
            else {
                //Sinistra
                a = b;
                b = b1;
                b1 = a1;
                a1 = tmp;
                }
            }

        tuple<int, int, int, int> EvalABXY ( BSetUnion& nva,
                                             BSetUnion& nvb,
                                             BSetUnion& nvx,
                                             BSetUnion& nvy,
                                             const unsigned int ia,
                                             const unsigned int ib,
                                             const unsigned int ix,
                                             const unsigned int iy,
                                             const unsigned int ia1,
                                             const unsigned int ib1,
                                             const unsigned int ix1,
                                             const unsigned int iy1 ) {
            static unsigned short mapper[256] = {
                0x0F00, 0x0E02, 0x0120, 0x0022, 0x0B08, 0x0A18, 0x0541, 0x0451, 0x0480, 0x0590, 0x0A05, 0x0B15, 0x0088, 0x0146, 0x0EA8, 0x0F66, 0x0D01, 0x0F03, 0x0321, 0x0123, 0x0928, 0x0B38, 0x0761, 0x0571, 0x06A0, 0x04B0, 0x0825, 0x0A35, 0x0245, 0x008B, 0x0C65, 0x0EAB, 0x0E10, 0x0C12, 0x0030, 0x0232, 0x0A42, 0x0852, 0x040B, 0x061B, 0x0506, 0x0716, 0x0B83, 0x0993, 0x0198, 0x0356, 0x0FB8, 0x0D76, 0x0C11, 0x0D13, 0x0231, 0x0333, 0x0862, 0x0972, 0x062B, 0x073B, 0x0726, 0x0636, 0x09A3, 0x08B3, 0x0355, 0x029B, 0x0D75, 0x0CBB, 0x0704, 0x0A82, 0x0924, 0x04A2, 0x030C, 0x0E1C, 0x0DC1, 0x00D1, 0x0C84, 0x0194, 0x0285, 0x0F95, 0x088C, 0x05C6, 0x06AC, 0x0BE6, 0x0681, 0x0B07, 0x08A1, 0x0527, 0x022C, 0x0F3C, 0x0CE1, 0x01F1, 0x0DA4, 0x00B4, 0x03A5, 0x0EB5, 0x09C5, 0x048F, 0x07E5, 0x0AAF, 0x0514, 0x0892, 0x0B34, 0x06B2, 0x01C2, 0x0CD2, 0x0F0F, 0x021F, 0x0E86, 0x0396, 0x0087, 0x0D97, 0x0A9C, 0x07D6, 0x04BC, 0x09F6, 0x0491, 0x0917, 0x0AB1, 0x0737, 0x00E2, 0x0DF2, 0x0E2F, 0x033F, 0x0FA6, 0x02B6, 0x01A7, 0x0CB7, 0x0BD5, 0x069F, 0x05F5, 0x08BF, 0x0B40, 0x060A, 0x0560, 0x082A, 0x0F48, 0x0258, 0x0149, 0x0C59, 0x00C0, 0x0DD0, 0x0E0D, 0x031D, 0x04C8, 0x094E, 0x0AE8, 0x076E, 0x0909, 0x0443, 0x0729, 0x0A63, 0x0D68, 0x0078, 0x0369, 0x0E79, 0x02E0, 0x0FF0, 0x0C2D, 0x013D, 0x064D, 0x0BCB, 0x086D, 0x05EB, 0x0A50, 0x071A, 0x0470, 0x093A, 0x0E4A, 0x035A, 0x004B, 0x0D5B, 0x010E, 0x0C1E, 0x0FC3, 0x02D3, 0x05D8, 0x085E, 0x0BF8, 0x067E, 0x0819, 0x0553, 0x0639, 0x0B73, 0x0C6A, 0x017A, 0x026B, 0x0F7B, 0x032E, 0x0E3E, 0x0DE3, 0x00F3, 0x075D, 0x0ADB, 0x097D, 0x04FB, 0x0344, 0x028A, 0x0D64, 0x0CAA, 0x074C, 0x065C, 0x09C9, 0x08D9, 0x08C4, 0x09D4, 0x068D, 0x079D, 0x0CCC, 0x0DCE, 0x02EC, 0x03EE, 0x0289, 0x0047, 0x0CA9, 0x0E67, 0x066C, 0x047C, 0x08E9, 0x0AF9, 0x09E4, 0x0BF4, 0x07AD, 0x05BD, 0x0DCD, 0x0FCF, 0x03ED, 0x01EF, 0x0154, 0x039A, 0x0F74, 0x0DBA, 0x05CA, 0x07DA, 0x0B4F, 0x095F, 0x0A8E, 0x089E, 0x04C7, 0x06D7, 0x0EDC, 0x0CDE, 0x00FC, 0x02FE, 0x0099, 0x0157, 0x0EB9, 0x0F77, 0x04EA, 0x05FA, 0x0A6F, 0x0B7F, 0x0BAE, 0x0ABE, 0x05E7, 0x04F7, 0x0FDD, 0x0EDF, 0x01FD, 0x00FF
                };

            bool na, nb, nx, ny;

            bool a = nva.getBitValue ( ia );
            bool b = nvb.getBitValue ( ib );
            bool x = nvx.getBitValue ( ix );
            bool y = nvy.getBitValue ( iy );
            bool a1 = nva.getBitValue ( ia1 );
            bool b1 = nvb.getBitValue ( ib1 );
            bool x1 = nvx.getBitValue ( ix1 );
            bool y1 = nvy.getBitValue ( iy1 );

            unsigned char idx = 0;

            if ( a ) idx |= 0x01;
            else idx &= ~0x01;

            if ( b ) idx |= 0x02;
            else idx &= ~0x02;

            if ( x ) idx |= 0x04;
            else idx &= ~0x04;

            if ( y ) idx |= 0x08;
            else idx &= ~0x08;

            if ( a1 ) idx |= 0x10;
            else idx &= ~0x10;

            if ( b1 ) idx |= 0x20;
            else idx &= ~0x20;

            if ( x1 ) idx |= 0x40;
            else idx &= ~0x40;

            if ( y1 ) idx |= 0x80;
            else idx &= ~0x80;

            unsigned short hres = mapper[idx];

            a = ( hres & 1 ) != 0;
            b = ( hres & 2 ) != 0;
            x = ( hres & 4 ) != 0;
            y = ( hres & 8 ) != 0;
            a1 = ( hres & 16 ) != 0;
            b1 = ( hres & 32 ) != 0;
            x1 = ( hres & 64 ) != 0;
            y1 = ( hres & 128 ) != 0;

            na = ( hres & 0x0100 ) != 0;
            nb = ( hres & 0x0200 ) != 0;
            nx = ( hres & 0x0400 ) != 0;
            ny = ( hres & 0x0800 ) != 0;

            nva.setBitValue ( ia, a );
            nvb.setBitValue ( ib, b );
            nvx.setBitValue ( ix, x );
            nvy.setBitValue ( iy, y );
            nva.setBitValue ( ia, a );
            nvb.setBitValue ( ib, b );
            nvx.setBitValue ( ix, x );
            nvy.setBitValue ( iy, y );
            return make_tuple ( na, nb, nx, ny ); //in questo modo i nuovi valgono 0: 2 e 1: 3
            }

//Questa variabile deve essere reinizializzata per ogni loop in modo da iniziare una nuova valutazione degli indirizzi
//anche se la procedura dovrebbe accorgersi di questo per i valori di ix e iy
//ma potrebbe esserci un caso in cui questo non avviene (con n/2 calcolati questo non avviene mai ma con n calcolati, sì).
        auto GetNewAddressesAB ( BSetUnion& vx,
                                 BSetUnion& vy,
                                 int ix, int iy,
                                 unsigned long long currentSize,
                                 int addressSize
                               ) {
            unsigned int address_x = vx.getNBitsFromIndex ( ix, addressSize );
            unsigned int address_y = vy.getNBitsFromIndex ( iy, addressSize );

            return make_pair ( address_x, address_y );
            }

        bool MakeSpace ( const bitset<HALF_KEY_SIZE>& va, const bitset<HALF_KEY_SIZE>& vx, int ksize, unsigned long long size, BSetUnion& nva ) {
            bool pari = true;
            //calcolo xor di vx
            int xorx = 0;

            for ( int i = 0; i < ksize; ++i ) xorx ^= vx[i];

            int tot = 0;

            //Ne devo mettere ksize in size:= avanzo di size/ksize
            int pos = 0;

            if ( xorx ) {
                //parto con pieno
                pari = true;

                for ( int i = 0; i < size; i += ( size / ksize ) ) {
                    auto d = i; // <<1;
                    int idx = nva.InsertValue ( d, va[pos++], true);

                    if ( ++tot >= HALF_KEY_SIZE )
                        return pari;
                    }
                }
            else {
                //parto con vuoto
                pari = false;

                for ( int i = 0; i < size; i += ( size / ksize ) ) {
                    auto d = i; // <<1;
                    int idx = nva.InsertValue ( ( d + 1 ) % size, va[pos++], true);

                    if ( ++tot >= HALF_KEY_SIZE )
                        return pari;
                    }
                }

            return pari;
            }

        void PiazzaValori ( unsigned char* va, unsigned char a, int address, bool avanti, int size, set<int>& vuoti ) {
            //Piazzo i nuovi valori alle posizioni indicate. Gli spazi DEVONO esserci per costruzione

            //Se avanti, cerco il primo vuoto non minore di address (quindi >=)
            if ( avanti ) {
                auto p_address = vuoti.lower_bound ( address );

                if ( p_address == vuoti.end() )
                    p_address = vuoti.lower_bound ( 0 );

                va[*p_address] = a;
                vuoti.erase ( p_address );
                }
            else {
                //devo cercare indietro, quindi mi serve il primo pari o minore di address (mi serve quindi il <=)
                auto p_address = vuoti.lower_bound ( address );

                if ( p_address == vuoti.end() ) {
                    //non ci sono elementi >= a quello dato, quindi, dato che ci devono essere ancora posti liberi,
                    //mi basta tornare indietro di 1
                    p_address--;
                    }
                else {
                    //potrei essere al primo posto, in questo caso non posso tornare indietro di 1 e quindi passo all'ultimo posto
                    if ( p_address == vuoti.begin() ) {
                        p_address = vuoti.end();
                        p_address--;
                        }
                    else {
                        //qui, sono al primo maggiore di quello richiesto, torno indietro di 1
                        p_address--;
                        }
                    }

                va[*p_address] = a;
                vuoti.erase ( p_address );
                }
            }

        unsigned char b2B ( unsigned char* p ) {
            //p è un array di 0 e 1. Converto i prossimi 8 in un numero di 8 bit
            unsigned char a;
            a = 0;

            if ( *p++ )
                a = 1;
            else
                a = 0;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            if ( *p++ )
                a = ( a << 1 ) | 1;
            else
                a <<= 1;

            return a;
            }

            void mfa_generator ( unsigned long long finalSize, int mask, int start, string* sRet, unsigned long long* time_spent, int cicli, const vector<vector<unsigned char>> & aba2b2, const vector<vector<unsigned char>> & xyx2y2) {
            unsigned long long start_time = GetULLTime();
            bitset<HALF_KEY_SIZE> va;
            bitset<HALF_KEY_SIZE> vb;
            bitset<HALF_KEY_SIZE> vx;
            bitset<HALF_KEY_SIZE> vy;
            bitset<HALF_KEY_SIZE> va_2;
            bitset<HALF_KEY_SIZE> vb_2;
            bitset<HALF_KEY_SIZE> vx_2;
            bitset<HALF_KEY_SIZE> vy_2;

			//prima li metto casualmente e poi, se c'è input, li metto in base a input
			for ( int i = start; i < ( start + HALF_KEY_SIZE ); ++i ) {
				va[i-start] = ( mtfa_mat_cypher_base_ext[0][i%65536] & mask ) ? true : false; //==1;
				vb[i-start] = ( mtfa_mat_cypher_base_ext[1][i%65536] & mask ) ? true : false; //==1;
				vx[i-start] = ( mtfa_mat_cypher_base_ext[2][i%65536] & mask ) ? true : false; //==1;
				vy[i-start] = ( mtfa_mat_cypher_base_ext[3][i%65536] & mask ) ? true : false; //==1;
				va_2[i-start] = ( mtfa_mat_cypher_base_ext[4][i%65536] & mask ) ? true : false; //==1;
				vb_2[i-start] = ( mtfa_mat_cypher_base_ext[5][i%65536] & mask ) ? true : false; //==1;
				vx_2[i-start] = ( mtfa_mat_cypher_base_ext[6][i%65536] & mask ) ? true : false; //==1;
				vy_2[i-start] = ( mtfa_mat_cypher_base_ext[7][i%65536] & mask ) ? true : false; //==1;
			}
			
			//ora metto a posto, con i valori corretti, va, vb, va2 e vb_2
			if (aba2b2.size()>0)
			{
				for (int i=0; i<va.size(); ++i)
				{
					for (int j=0; j<8; ++j) {
						if ((i*8+j)<HALF_KEY_SIZE) {
							va[i*8+j]  =(aba2b2[0][i]&(1<<j))?true:false;
							vb[i*8+j]  =(aba2b2[1][i]&(1<<j))?true:false;
							va_2[i*8+j]=(aba2b2[2][i]&(1<<j))?true:false;
							vb_2[i*8+j]=(aba2b2[3][i]&(1<<j))?true:false;
						}
					}
				}
			}

			//ora metto a posto, con i valori corretti, vx, vy, vx_2 e vy_2
			if (xyx2y2.size()>0)
			{
				for (int i=0; i<vx.size(); ++i)
				{
					for (int j=0; j<8; ++j) {
						if ((i*8+j)<HALF_KEY_SIZE) {
							vx[i*8+j]  =(xyx2y2[0][i]&(1<<j))?true:false;
							vy[i*8+j]  =(xyx2y2[1][i]&(1<<j))?true:false;
							vx_2[i*8+j]=(xyx2y2[2][i]&(1<<j))?true:false;
							vy_2[i*8+j]=(xyx2y2[3][i]&(1<<j))?true:false;
						}
					}
				}
			}
			
			
			unsigned long long current_size; // = HALF_KEY_SIZE;
            finalSize /= 4;
            current_size = finalSize; // + finalSize/2; //*3/4;  //rimane divisibile per 64 se la dimensione è almeno 256
            //cout << "Key size: " << HALF_KEY_SIZE << ". Current size: " << current_size << ". Final size: " << finalSize << endl;

//   *
//     *   Ora l'incrocio è anche a livello di chiavi, per il calcolo degli indirizzi
//     *   a  b   x  y
//     *
//     *   a2 b2  x2 y2
//     *   indirizzo di   ab da x2y2
//     *   indirizzo di   xy da a2b2
//     *   indirizzo di a2b2 da xy
//     *   indirizzo di x2y2 da ab
//     *

            //Con la set union
            BSetUnion nva ( current_size );
            BSetUnion nvb ( current_size );
            BSetUnion nvx ( current_size );
            BSetUnion nvy ( current_size );

            BSetUnion nva_2 ( current_size );
            BSetUnion nvb_2 ( current_size );
            BSetUnion nvx_2 ( current_size );
            BSetUnion nvy_2 ( current_size );

            MakeSpace ( va, vx, HALF_KEY_SIZE, current_size, nva );
            MakeSpace ( vb, vy, HALF_KEY_SIZE, current_size, nvb );
            MakeSpace ( vx, va, HALF_KEY_SIZE, current_size, nvx );
            MakeSpace ( vy, vb, HALF_KEY_SIZE, current_size, nvy );

            MakeSpace ( va_2, vx_2, HALF_KEY_SIZE, current_size, nva_2 );
            MakeSpace ( vb_2, vy_2, HALF_KEY_SIZE, current_size, nvb_2 );
            MakeSpace ( vx_2, va_2, HALF_KEY_SIZE, current_size, nvx_2 );
            MakeSpace ( vy_2, vb_2, HALF_KEY_SIZE, current_size, nvy_2 );

            //Riempio gli spazi vuoti
            int pos_a = 0;
            int pos_b = 0;
            int pos_x = 0;
            int pos_y = 0;

            int pos_a_2 = 0;
            int pos_b_2 = 0;
            int pos_x_2 = 0;
            int pos_y_2 = 0;

            unsigned long long limit = finalSize - HALF_KEY_SIZE; //finalSize-HALF_KEY_SIZE;

            for ( int fatti = 0; fatti < limit; ++fatti ) { //qui posso gestire la creazione di 1/2 in più per volta
                //Calcolo indirizzi di a b e di x y
                auto addresses_ab   = GetNewAddressesAB ( nvx_2, nvy_2, pos_x_2, pos_y_2, current_size, Log2 ( current_size ) );
                auto addresses_xy   = GetNewAddressesAB ( nva_2, nvb_2, pos_a_2, pos_b_2, current_size, Log2 ( current_size ) );
                auto addresses_ab_2 = GetNewAddressesAB ( nvx, nvy, pos_x, pos_y, current_size, Log2 ( current_size ) );
                auto addresses_xy_2 = GetNewAddressesAB ( nva, nvb, pos_a, pos_b, current_size, Log2 ( current_size ) );

                if ( ( fatti % 10000000 ) == 0 )
                    cout << fatti + HALF_KEY_SIZE << endl;

                //Calcolo i nuovi a, b, x e y e ruoto i precedenti
                //Per farlo devo cercare i primi occupati a partire dalla posizione corrente, che appartengano alla chiave attuale
                unsigned int fa  = nva.getAddressOfNextBit ( pos_a, true, true );
                unsigned int fb  = nvb.getAddressOfNextBit ( pos_b, true, true );
                unsigned int fx  = nvx.getAddressOfNextBit ( pos_x, true, true );
                unsigned int fy  = nvy.getAddressOfNextBit ( pos_y, true, true );
                unsigned int fa1 = nva.getAddressOfNextBit ( fa + 1, true, true );
                unsigned int fb1 = nvb.getAddressOfNextBit ( fb + 1, true, true );
                unsigned int fx1 = nvx.getAddressOfNextBit ( fx + 1, true, true );
                unsigned int fy1 = nvy.getAddressOfNextBit ( fy + 1, true, true );

                unsigned int fa_2  = nva_2.getAddressOfNextBit ( pos_a_2, true, true );
                unsigned int fb_2  = nvb_2.getAddressOfNextBit ( pos_b_2, true, true );
                unsigned int fx_2  = nvx_2.getAddressOfNextBit ( pos_x_2, true, true );
                unsigned int fy_2  = nvy_2.getAddressOfNextBit ( pos_y_2, true, true );
                unsigned int fa1_2 = nva_2.getAddressOfNextBit ( fa_2 + 1, true, true );
                unsigned int fb1_2 = nvb_2.getAddressOfNextBit ( fb_2 + 1, true, true );
                unsigned int fx1_2 = nvx_2.getAddressOfNextBit ( fx_2 + 1, true, true );
                unsigned int fy1_2 = nvy_2.getAddressOfNextBit ( fy_2 + 1, true, true );


                //Ora devo calcolare se, in caso di collisione, devo andare avanti o indietro. Questo lo faccio utilizzando tutti e 4 i valori
                bool avanti_a = nvy.getBitValue ( fy ) == nvy.getBitValue ( fy1 );
                bool avanti_b = nvx.getBitValue ( fx ) == nvx.getBitValue ( fx1 );
                bool avanti_x = nvb.getBitValue ( fb ) == nvb.getBitValue ( fb1 );
                bool avanti_y = nva.getBitValue ( fa ) == nva.getBitValue ( fa1 );

                bool avanti_a_2 = nvy_2.getBitValue ( fy_2 ) == nvy_2.getBitValue ( fy1_2 );
                bool avanti_b_2 = nvx_2.getBitValue ( fx_2 ) == nvx_2.getBitValue ( fx1_2 );
                bool avanti_x_2 = nvb_2.getBitValue ( fb_2 ) == nvb_2.getBitValue ( fb1_2 );
                bool avanti_y_2 = nva_2.getBitValue ( fa_2 ) == nva_2.getBitValue ( fa1_2 );

                //Calcolo i nuovi valori della quaterna
                auto nuovi_abxy   = EvalABXY ( nva, nvb, nvx, nvy, fa, fb, fx, fy, fa1, fb1, fx1, fy1 );
                auto nuovi_abxy_2 = EvalABXY ( nva_2, nvb_2, nvx_2, nvy_2, fa_2, fb_2, fx_2, fy_2, fa1_2, fb1_2, fx1_2, fy1_2 );

                //cout << get<0>(nuovi_abxy) << " " << get<1>(nuovi_abxy) << " " << get<2>(nuovi_abxy) << " " << get<3>(nuovi_abxy) << endl;

                //in modo deterministico, avanti e indietro cambiano semantica ogni operazione: una volta true è avanti, la volta successiva true è indietro...
                nva.InsertValue ( addresses_ab.first, get<0> ( nuovi_abxy ), avanti_a);
                nvb.InsertValue ( addresses_ab.second, get<1> ( nuovi_abxy ), avanti_b);
                nvx.InsertValue ( addresses_xy.first, get<2> ( nuovi_abxy ), avanti_x);
                nvy.InsertValue ( addresses_xy.second, get<3> ( nuovi_abxy ), avanti_y);

                nva_2.InsertValue ( addresses_ab_2.first, get<0> ( nuovi_abxy_2 ), avanti_a_2);
                nvb_2.InsertValue ( addresses_ab_2.second, get<1> ( nuovi_abxy_2 ), avanti_b_2);
                nvx_2.InsertValue ( addresses_xy_2.first, get<2> ( nuovi_abxy_2 ), avanti_x_2);
                nvy_2.InsertValue ( addresses_xy_2.second, get<3> ( nuovi_abxy_2 ), avanti_y_2);

                pos_a = fa1;
                pos_b = fb1;
                pos_x = fx1;
                pos_y = fy1;

                pos_a_2 = fa1_2;
                pos_b_2 = fb1_2;
                pos_x_2 = fx1_2;
                pos_y_2 = fy1_2;
                }
#if 0
            for ( int i=0; i<cicli; ++i ) {
                //Riscrivo, se necessario, quanto già scritto
                pos_a = 0;
                pos_b = 0;
                pos_x = 0;
                pos_y = 0;

                pos_a_2 = 0;
                pos_b_2 = 0;
                pos_x_2 = 0;
                pos_y_2 = 0;

                for ( int fatti = 0; fatti < limit; ++fatti ) { //qui posso gestire la creazione di 1/2 in più per volta
                    //Calcolo indirizzi di a b e di x y
                    auto addresses_ab   = GetNewAddressesAB ( nvx_2, nvy_2, pos_x_2, pos_y_2, current_size, Log2 ( current_size ) );
                    auto addresses_xy   = GetNewAddressesAB ( nva_2, nvb_2, pos_a_2, pos_b_2, current_size, Log2 ( current_size ) );
                    auto addresses_ab_2 = GetNewAddressesAB ( nvx, nvy, pos_x, pos_y, current_size, Log2 ( current_size ) );
                    auto addresses_xy_2 = GetNewAddressesAB ( nva, nvb, pos_a, pos_b, current_size, Log2 ( current_size ) );

                    if ( ( fatti % 10000000 ) == 0 )
                        cout << fatti + HALF_KEY_SIZE << endl;

                    //Calcolo i nuovi a, b, x e y e ruoto i precedenti
                    //Per farlo devo cercare i primi occupati a partire dalla posizione corrente, che appartengano alla chiave attuale
                    unsigned int fa  = nva.getAddressOfNextBit ( pos_a, true, true );
                    unsigned int fb  = nvb.getAddressOfNextBit ( pos_b, true, true );
                    unsigned int fx  = nvx.getAddressOfNextBit ( pos_x, true, true );
                    unsigned int fy  = nvy.getAddressOfNextBit ( pos_y, true, true );
                    unsigned int fa1 = nva.getAddressOfNextBit ( fa + 1, true, true );
                    unsigned int fb1 = nvb.getAddressOfNextBit ( fb + 1, true, true );
                    unsigned int fx1 = nvx.getAddressOfNextBit ( fx + 1, true, true );
                    unsigned int fy1 = nvy.getAddressOfNextBit ( fy + 1, true, true );

                    unsigned int fa_2  = nva_2.getAddressOfNextBit ( pos_a_2, true, true );
                    unsigned int fb_2  = nvb_2.getAddressOfNextBit ( pos_b_2, true, true );
                    unsigned int fx_2  = nvx_2.getAddressOfNextBit ( pos_x_2, true, true );
                    unsigned int fy_2  = nvy_2.getAddressOfNextBit ( pos_y_2, true, true );
                    unsigned int fa1_2 = nva_2.getAddressOfNextBit ( fa_2 + 1, true, true );
                    unsigned int fb1_2 = nvb_2.getAddressOfNextBit ( fb_2 + 1, true, true );
                    unsigned int fx1_2 = nvx_2.getAddressOfNextBit ( fx_2 + 1, true, true );
                    unsigned int fy1_2 = nvy_2.getAddressOfNextBit ( fy_2 + 1, true, true );


                    //Ora devo calcolare se, in caso di collisione, devo andare avanti o indietro. Questo lo faccio utilizzando tutti e 4 i valori
                    bool avanti_a = nvy.getBitValue ( fy ) == nvy.getBitValue ( fy1 );
                    bool avanti_b = nvx.getBitValue ( fx ) == nvx.getBitValue ( fx1 );
                    bool avanti_x = nvb.getBitValue ( fb ) == nvb.getBitValue ( fb1 );
                    bool avanti_y = nva.getBitValue ( fa ) == nva.getBitValue ( fa1 );

                    bool avanti_a_2 = nvy_2.getBitValue ( fy_2 ) == nvy_2.getBitValue ( fy1_2 );
                    bool avanti_b_2 = nvx_2.getBitValue ( fx_2 ) == nvx_2.getBitValue ( fx1_2 );
                    bool avanti_x_2 = nvb_2.getBitValue ( fb_2 ) == nvb_2.getBitValue ( fb1_2 );
                    bool avanti_y_2 = nva_2.getBitValue ( fa_2 ) == nva_2.getBitValue ( fa1_2 );

                    //Calcolo i nuovi valori della quaterna
                    auto nuovi_abxy   = EvalABXY ( nva, nvb, nvx, nvy, fa, fb, fx, fy, fa1, fb1, fx1, fy1 );
                    auto nuovi_abxy_2 = EvalABXY ( nva_2, nvb_2, nvx_2, nvy_2, fa_2, fb_2, fx_2, fy_2, fa1_2, fb1_2, fx1_2, fy1_2 );

                    //cout << get<0>(nuovi_abxy) << " " << get<1>(nuovi_abxy) << " " << get<2>(nuovi_abxy) << " " << get<3>(nuovi_abxy) << endl;

                    //in modo deterministico, avanti e indietro cambiano semantica ogni operazione: una volta true è avanti, la volta successiva true è indietro...
                    nva.setBitValue ( addresses_ab.first, get<0> ( nuovi_abxy ) );
                    nvb.setBitValue ( addresses_ab.second, get<1> ( nuovi_abxy ) );
                    nvx.setBitValue ( addresses_xy.first, get<2> ( nuovi_abxy ) );
                    nvy.setBitValue ( addresses_xy.second, get<3> ( nuovi_abxy ) );

                    nva_2.setBitValue ( addresses_ab_2.first, get<0> ( nuovi_abxy_2 ) );
                    nvb_2.setBitValue ( addresses_ab_2.second, get<1> ( nuovi_abxy_2 ) );
                    nvx_2.setBitValue ( addresses_xy_2.first, get<2> ( nuovi_abxy_2 ) );
                    nvy_2.setBitValue ( addresses_xy_2.second, get<3> ( nuovi_abxy_2 ) );

                    pos_a = fa1;
                    pos_b = fb1;
                    pos_x = fx1;
                    pos_y = fy1;

                    pos_a_2 = fa1_2;
                    pos_b_2 = fb1_2;
                    pos_x_2 = fx1_2;
                    pos_y_2 = fy1_2;
                    }
                }

#endif
            unsigned long long end_time = GetULLTime();
            cout << "Preparing output data\n";

            //Preparo un file di uscita
            Pushing pa, pb, pa_2, pb_2, px, py, px_2, py_2;
            PreparePa ( nva, pa );
            PreparePa ( nvb, pb );
            PreparePa ( nva_2, pa_2 );
            PreparePa ( nvb_2, pb_2 );

			sRet->assign ( pa.v.begin(), pa.v.end() );
            sRet->append ( pb.v.begin(),   pb.v.end() );
            sRet->append ( pa_2.v.begin(), pa_2.v.end() );
            sRet->append ( pb_2.v.begin(), pb_2.v.end() );
			
			
			//in questo caso devo tornare tutte le chiavi, per riattivare alla prossima generazione!
			if (aba2b2.size() > 0)
			{
				PreparePa ( nvx, px );
				PreparePa ( nvy, py );
				PreparePa ( nvx_2, py_2 );
				PreparePa ( nvy_2, py_2 );
				
				sRet->append ( px.v.begin(),   px.v.end() );
				sRet->append ( py.v.begin(),   py.v.end() );
				sRet->append ( px_2.v.begin(), px_2.v.end() );
				sRet->append ( py_2.v.begin(), py_2.v.end() );
			}
			
            *time_spent = end_time - start_time;
            return;
            }
    };

	void MyThread ( unsigned long long finalSize, int mask, int start, string* sRet, unsigned long long* time_spent, int cicli, const vector<vector<unsigned char>> & aba2b2, const vector<vector<unsigned char>> & xyx2y2) {
    KGenerator kg;
    kg.mfa_generator ( finalSize, mask, start, sRet, time_spent, cicli, aba2b2, xyx2y2 );
    }
#endif

// //Il vantaggio è che le dimensioni sono sempre multiplo di 64
// vector<unsigned long long> ConvertBitSetToVUint64 ( const boost::dynamic_bitset<> & bs ) {
//     vector<unsigned long long> vull ( bs.size() /64, 0 );
//     for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
//         auto pos = i >> 6;
//         auto bit = i & 0x0000003F;
//         vull[pos] |= 1ULL << bit;
//         }
//     return vull;
//     }
// 
// 
//     vector<unsigned char> ConvertBitSetToVUint8 ( const boost::dynamic_bitset<> & bs ) {
//     vector<unsigned char> vuc ( bs.size() /8, 0 );
// 
//     for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
//         auto pos = i >> 3;
//         auto bit = i & 0x00000007;
//         vuc[pos] |= 1 << bit;
//         }
// 
//     return vuc;
//     }
// extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];


const char* mtfa_magic_mime (char * filename)
{
	const char *magic_full;
	magic_t magic_cookie;
	
	/* MAGIC_MIME tells magic to return a mime of the file, 
	 *       but you can specify different things	*/
	if (!magic_cookie)
	{
		magic_cookie = magic_open(MAGIC_MIME_TYPE);
		if (magic_cookie == NULL) {
			return "NO\n";
		}
		
		if (magic_load(magic_cookie, NULL) != 0) {
			magic_close(magic_cookie);
			return "NO\n";
		}
	}
	
	magic_full = magic_file(magic_cookie, filename);
	cout << magic_full << endl;
	return "";
}

#if 0
/**
* @brief La classe implementa la rappresentazione fast dei set.
* tutto ok, sia la insert che la delete. Ora manca la navigazione sui soli elementi attivi!
* Questa potrei farla facendo un set di vuoti e poi a mano a mano tolgo elementi
* Altra possibilità è una lista doppiamente puntata dove la root punta al primo elemento e poi prosegue in questo modo
* Il problema è che non sono in grado di assicurare l'ordine della lista e quindi non saprei come arrivare all'elemento
* successivo all'i-esimo
* L'unica cosa che potrei fare è un heap in cui inserisco gli insiemi man mano che si creano
* ma questo mi riporterebbe immediatamente al logaritmo
*/
class SetUnion
{
private:
	int32_t Add(const int32_t a1, const int32_t a2) { return (a1+a2) % size; }
	int32_t Sub(const int32_t s1, const int32_t s2) { auto s = s1 - s2; if (s < 0) return size+s; else return s; }
	typedef struct __pair__ {
		int32_t ssize;   //indice del primo elemento del Set
		int32_t right;  //indice del primo elemento libero a destra del set (oppure dimensione del set)
	} TPair;
	int32_t size;
	vector<TPair*> ranges; //Uso i puntatori poiché si semplifica il modello. All'inizio tutti i puntatori puntano a loro stessi
    SetUnion ( const int32_t _size );
    ~SetUnion();
public:
    bool Insert( const int32_t index );
    bool Delete( const int32_t index );
    bool Find(const int32_t index);
    void Print();
};

SetUnion::SetUnion ( const int32_t _size )
{
    size = _size;
    ranges.resize(size);

    //All'inizio tutti i posti sono vuoti e quindi ognuno punta a sè stesso
    for ( int32_t i = 0; i < size; ++i )
    {
        ranges[i] = nullptr;
    }
}

SetUnion::~SetUnion()
{
    //tipo la print
}

bool SetUnion::Find( const int32_t index )
{
    if (ranges[index])
        return true;
    else
        return false;
}
//Inserisce l'elemento index nella struttura.
//Ricorda che ogni elemento punta al primo libero a destra
//e all'ultimo occupato a sinistra
//c'è un puntatore che punta all'indice occupato più basso (a partire da 0)
bool SetUnion::Insert( const int32_t index )
{
    //Se già presente, esce
    if (ranges[index])
        return true;
    
    //E' libero e quindi vuoto! Inserisco!

    //il primo libero a destra è il primo libero a destra dell'elemento in posizione index+1
    //l'altra informazione che mi serve è la dimensione del set che si sta formando
    //inoltre se il set a destra coincide con il set a sinistra, allora sto inserendo l'ultimo elemento!!
    auto setOnRight = ranges[Add(index,1)];
    auto setOnLeft  = ranges[Sub(index, 1)];
    if (setOnLeft && setOnLeft == setOnRight)
    {
        //è tutto completato tranne l'elemento corrente. Aggiungo anche questo.
        ranges[index]=setOnLeft;
        setOnLeft->ssize++;
        
        //I seguenti valori per gestire il set completamente riempito!
        setOnLeft->right = -1;
        return true;
    }

    //il set a destra non è uguale al set a sinistra, posso fare la merge tra i tre puntatori (sinistra, centro, destra)

    //in base alla presenza o assenza a destra e a sinistra
    if (setOnRight)
    {
        if (setOnLeft)
        {
            //A destra e a sinistra c'è qualcosa
            //Ora qui è anche palese che setonleft e setonright saranno fusi insieme e quindi il gioco dei puntatori di root
            //va aggiornato ma questo lo faccio nei punti giusti del codice
            if (setOnLeft->ssize <= setOnRight->ssize)
            {
                //left is smaller, aggiorno lo smaller con il puntatore puntato dal right
                //e aggiorno anche il corrente che prima non puntava nulla
                ranges[index] = setOnRight;

                //Aggiorno la dimensione del set finale
                setOnRight->ssize += 1 + setOnLeft->ssize;

                //ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
                auto pos = Sub(setOnLeft->right, setOnLeft->ssize);

                for (int i=0; i<setOnLeft->ssize; ++i)
                {
                    ranges[Add(pos, i)] = setOnRight;
                }
                delete(setOnLeft);
                return true;
            }
            else
            {
                //right is smaller, aggiorno lo smaller con il puntatore puntato dal left
                //e aggiorno anche il corrente che prima non puntava nulla
                ranges[index] = setOnLeft;

                //Aggiorno la dimensione del set finale
                setOnLeft->ssize += 1 + setOnRight->ssize;
                setOnLeft->right = setOnRight->right;

                //ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
                auto pos = Sub(setOnRight->right, setOnRight->ssize);

                for (int i=0; i<setOnRight->ssize; ++i)
                {
                    ranges[Add(pos,i)] = setOnLeft;
                }
                delete(setOnRight);
                return true;
            }
        }
        else
        {
            //a sinistra il nulla e a destra qualcosa
            //mi estendo a destra mentre a sinistra nulla cambia
            ranges[index] = setOnRight;

            //Aggiorno la dimensione del set finale
            setOnRight->ssize += 1;
            return true;
        }
    }
    else
    {
        if (setOnLeft)
        {
            //a destra il nulla
            //a sinistra puntava a me, quindi devo aggiornare il sinistra
            ranges[index] = setOnLeft;

            //Aggiorno la dimensione del set finale
            setOnLeft->ssize += 1;
            setOnLeft->right = Add(index, 1);
            return true;
        }
        else
        {
            //a destra e a sinistra il nulla
            //modifico solo me stesso
            ranges[index] = new TPair();
            ranges[index]->ssize = 1;
            ranges[index]->right = Add(index, 1);
            return true;
        }
    }
    
    return true;
}


bool SetUnion::Delete( const int32_t index )
{
    //Se non presente, esce
    if (!ranges[index])
        return true;
    
    //Se fully allora splitto in due
    if (ranges[index]->right < 0)
    {
        //Ti spiezzo in due! cioè ho ancora un solo set che va da index+1 a index-1
        ranges[index]->right=index;
        ranges[index]->ssize--;
        ranges[index]=nullptr;
        return true;
    }
    
    //Presente, allora che devo fare? se sono solo, è sufficiente autoeliminarmi
    auto setOnRight = ranges[Add(index, 1)];
    auto setOnLeft  = ranges[Sub(index, 1)];
    if (!setOnLeft && !setOnRight)
    {
        //nulla intorno a me! mi autoelimino
        delete ranges[index];
        ranges[index]=nullptr;
        return true;
    }
    if (setOnLeft == setOnRight)
    {
        //Appartengo a un insieme che si estende da destra a sinistra.
        //devo aggiornare l'insieme più corto
        auto leftStart = Sub(setOnLeft->right, setOnLeft->ssize);

        auto rightEnd  =  setOnLeft->right;
        auto sizeOnLeft = Sub(index, leftStart);
        auto sizeOnRight = Sub(rightEnd, index)-1;
        if (sizeOnLeft <= sizeOnRight)
        {
            //creo un nuovo ptr e lo assegno a sinistra
            //a destra cambia il size ma non l'estremo
            if (sizeOnLeft)
            {
                auto ptr = new TPair();
                ptr->right=index;   //primo libero a destra
                ptr->ssize = sizeOnLeft;
                for (int i=0; i<sizeOnLeft; ++i)
                    ranges[Add(i, leftStart)] = ptr;
            }
            //aggiorno il ptr sulla destra
            setOnRight->ssize -= (sizeOnLeft+1);
            
            //Rimuovo l'elemento che devo rimuovere
            ranges[index] = nullptr;
            return true;
        }
        else
        {
            //creo un nuovo ptr e lo assegno a destra
            //a sinistra cambia il size e l'estremo libero che diventa index
            if (sizeOnRight)
            {
                auto ptr = new TPair();
                ptr->right=setOnRight->right;   //primo libero a destra
                ptr->ssize = sizeOnRight;
                for (int i=0; i<sizeOnRight; ++i)
                    ranges[Add(i+1, index)] = ptr;
            }            
            //aggiorno il ptr sulla sinistra
            setOnLeft->ssize -= (sizeOnRight+1);
            
            //Rimuovo l'elemento che devo rimuovere
            ranges[index] = nullptr;
            return true;
        }
    }

    //Se sono qui, allora il nostro index è estremo di un insieme (posizionato alla sua destra o alla sua sinistra)
    if (setOnLeft)
    {
        setOnLeft->ssize--;
        setOnLeft->right = index;
        ranges[index]=nullptr;
        return true;
    }
    else
    {
        //setOnRight
        setOnRight->ssize--;
        ranges[index]=nullptr;
        return true;
    }
}

void SetUnion::Print()
{
    int i=0;
    int tot = 0;
    while (tot < size)
    {
        if (ranges[i] && ranges[i]->ssize)
        {
            cout << "Set: " << i << " size: " << ranges[i]->ssize << " right: " << ranges[i]->right << endl;
            tot += ranges[i]->ssize;
            i = ranges[i]->right;
        }
        else
        {
            tot++;
            i++;
        }
    }
}


/************************************************************/
/************************************************************/
class SetUnion64
{
private:
public:
	int32_t Add(const int32_t a1, const int32_t a2) { return (a1+a2) % size; }
	int32_t Sub(const int32_t s1, const int32_t s2) { auto s = s1 - s2; if (s < 0) return size+s; else return s; }
	typedef struct __pair__ {
		int32_t ssize;   //indice del primo elemento del Set
		int32_t right;  //indice del primo elemento libero a destra del set (oppure dimensione del set)
	} TPair;
	int32_t size;
	vector<TPair*> ranges; //Uso i puntatori poiché si semplifica il modello. All'inizio tutti i puntatori puntano a loro stessi
	vector<unsigned long long> vbits;
    SetUnion64 ( const int32_t _size );
    ~SetUnion64();
    bool Insert( const int32_t index );
    bool Delete( const int32_t index );
    bool Find(const int32_t index);
    void Print();
};

SetUnion64::SetUnion64 ( const int32_t _size )
{
    size = _size >> 6;
    ranges.resize(size, nullptr);
    vbits.resize(size, 0);
}

SetUnion64::~SetUnion64()
{
    //tipo la print
}

bool SetUnion64::Find( const int32_t _index )
{
    int index = _index >> 6;
    int bit = 1ULL<<(_index&0x0000003F);
    
    if (ranges[index] && (vbits[index] & bit))
        return true;
    else
        return false;
}
//Inserisce l'elemento index nella struttura.
//Ricorda che ogni elemento punta al primo libero a destra
//e all'ultimo occupato a sinistra
//c'è un puntatore che punta all'indice occupato più basso (a partire da 0)
bool SetUnion64::Insert( const int32_t _index )
{
    int index = _index >> 6;
    int bit = 1ULL<<(_index&0x0000003F);

    //Se già presente, esce
    if (ranges[index])
    {
        vbits[index] |= bit; //in ogni caso inserisco!
        return true;
    }
    
    //E' libero e quindi vuoto! Inserisco!
    vbits[index] |= bit; //Questo per ricordare il bit, il resto per il contenitore del bit

    //il primo libero a destra è il primo libero a destra dell'elemento in posizione index+1
    //l'altra informazione che mi serve è la dimensione del set che si sta formando
    //inoltre se il set a destra coincide con il set a sinistra, allora sto inserendo l'ultimo elemento!!
    auto setOnRight = ranges[Add(index,1)];
    auto setOnLeft  = ranges[Sub(index, 1)];
    if (setOnLeft && setOnLeft == setOnRight)
    {
        //è tutto completato tranne l'elemento corrente. Aggiungo anche questo.
        ranges[index]=setOnLeft;
        setOnLeft->ssize++;
        
        //I seguenti valori per gestire il set completamente riempito!
        setOnLeft->right = -1;
        return true;
    }

    //il set a destra non è uguale al set a sinistra, posso fare la merge tra i tre puntatori (sinistra, centro, destra)

    //in base alla presenza o assenza a destra e a sinistra
    if (setOnRight)
    {
        if (setOnLeft)
        {
            //A destra e a sinistra c'è qualcosa
            //Ora qui è anche palese che setonleft e setonright saranno fusi insieme e quindi il gioco dei puntatori di root
            //va aggiornato ma questo lo faccio nei punti giusti del codice
            if (setOnLeft->ssize <= setOnRight->ssize)
            {
                //left is smaller, aggiorno lo smaller con il puntatore puntato dal right
                //e aggiorno anche il corrente che prima non puntava nulla
                ranges[index] = setOnRight;

                //Aggiorno la dimensione del set finale
                setOnRight->ssize += 1 + setOnLeft->ssize;

                //ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
                auto pos = Sub(setOnLeft->right, setOnLeft->ssize);

                for (int i=0; i<setOnLeft->ssize; ++i)
                {
                    ranges[Add(pos, i)] = setOnRight;
                }
                delete(setOnLeft);
                return true;
            }
            else
            {
                //right is smaller, aggiorno lo smaller con il puntatore puntato dal left
                //e aggiorno anche il corrente che prima non puntava nulla
                ranges[index] = setOnLeft;

                //Aggiorno la dimensione del set finale
                setOnLeft->ssize += 1 + setOnRight->ssize;
                setOnLeft->right = setOnRight->right;

                //ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
                auto pos = Sub(setOnRight->right, setOnRight->ssize);

                for (int i=0; i<setOnRight->ssize; ++i)
                {
                    ranges[Add(pos,i)] = setOnLeft;
                }
                delete(setOnRight);
                return true;
            }
        }
        else
        {
            //a sinistra il nulla e a destra qualcosa
            //mi estendo a destra mentre a sinistra nulla cambia
            ranges[index] = setOnRight;

            //Aggiorno la dimensione del set finale
            setOnRight->ssize += 1;
            return true;
        }
    }
    else
    {
        if (setOnLeft)
        {
            //a destra il nulla
            //a sinistra puntava a me, quindi devo aggiornare il sinistra
            ranges[index] = setOnLeft;

            //Aggiorno la dimensione del set finale
            setOnLeft->ssize += 1;
            setOnLeft->right = Add(index, 1);
            return true;
        }
        else
        {
            //a destra e a sinistra il nulla
            //modifico solo me stesso
            ranges[index] = new TPair();
            ranges[index]->ssize = 1;
            ranges[index]->right = Add(index, 1);
            return true;
        }
    }
    
    return true;
}

bool SetUnion64::Delete( const int32_t _index )
{
    int index = _index >> 6;
    int bit = 1ULL<<(_index&0x0000003F);

    //Se non presente, esce
    if (!ranges[index])
        return true;
    
    //presente come cluster, vediamo come bit
    if (vbits[index] & bit)
        vbits[index] ^= bit;
    
    //ora, se non ci sono più elementi, tolgo anche il cluster, altrimenti lo lascio
    if (vbits[index])
        return true;
    
    //Se fully allora splitto in due
    if (ranges[index]->right < 0)
    {
        //Ti spiezzo in due! cioè ho ancora un solo set che va da index+1 a index-1
        ranges[index]->right=index;
        ranges[index]->ssize--;
        ranges[index]=nullptr;
        return true;
    }
    
    //Presente, allora che devo fare? se sono solo, è sufficiente autoeliminarmi
    auto setOnRight = ranges[Add(index, 1)];
    auto setOnLeft  = ranges[Sub(index, 1)];
    if (!setOnLeft && !setOnRight)
    {
        //nulla intorno a me! mi autoelimino
        delete ranges[index];
        ranges[index]=nullptr;
        return true;
    }
    if (setOnLeft == setOnRight)
    {
        //Appartengo a un insieme che si estende da destra a sinistra.
        //devo aggiornare l'insieme più corto
        auto leftStart = Sub(setOnLeft->right, setOnLeft->ssize);

        auto rightEnd  =  setOnLeft->right;
        auto sizeOnLeft = Sub(index, leftStart);
        auto sizeOnRight = Sub(rightEnd, index)-1;
        if (sizeOnLeft <= sizeOnRight)
        {
            //creo un nuovo ptr e lo assegno a sinistra
            //a destra cambia il size ma non l'estremo
            if (sizeOnLeft)
            {
                auto ptr = new TPair();
                ptr->right=index;   //primo libero a destra
                ptr->ssize = sizeOnLeft;
                for (int i=0; i<sizeOnLeft; ++i)
                    ranges[Add(i, leftStart)] = ptr;
            }
            //aggiorno il ptr sulla destra
            setOnRight->ssize -= (sizeOnLeft+1);
            
            //Rimuovo l'elemento che devo rimuovere
            ranges[index] = nullptr;
            return true;
        }
        else
        {
            //creo un nuovo ptr e lo assegno a destra
            //a sinistra cambia il size e l'estremo libero che diventa index
            if (sizeOnRight)
            {
                auto ptr = new TPair();
                ptr->right=setOnRight->right;   //primo libero a destra
                ptr->ssize = sizeOnRight;
                for (int i=0; i<sizeOnRight; ++i)
                    ranges[Add(i+1, index)] = ptr;
            }            
            //aggiorno il ptr sulla sinistra
            setOnLeft->ssize -= (sizeOnRight+1);
            
            //Rimuovo l'elemento che devo rimuovere
            ranges[index] = nullptr;
            return true;
        }
    }

    //Se sono qui, allora il nostro index è estremo di un insieme (posizionato alla sua destra o alla sua sinistra)
    if (setOnLeft)
    {
        setOnLeft->ssize--;
        setOnLeft->right = index;
        ranges[index]=nullptr;
        return true;
    }
    else
    {
        //setOnRight
        setOnRight->ssize--;
        ranges[index]=nullptr;
        return true;
    }
}

void SetUnion64::Print()
{
    int i=0;
    int tot = 0;
    while (tot < size)
    {
        if (ranges[i] && ranges[i]->ssize)
        {
            cout << "Set: " << i << " size: " << ranges[i]->ssize << " right: " << ranges[i]->right << endl;
            tot += ranges[i]->ssize;
            i = ranges[i]->right;
        }
        else
        {
            tot++;
            i++;
        }
    }
}
#endif

/**************************************************/
#if 0
//Le tavole di bit non servono a nulla, lo shift è più veloce
    static unsigned long long table64[]={
        0x0000000000000001ULL, 0x0000000000000002ULL, 0x0000000000000004ULL, 0x0000000000000008ULL, 0x0000000000000010ULL, 0x0000000000000020ULL, 0x0000000000000040ULL, 0x0000000000000080ULL, 
        0x0000000000000100ULL, 0x0000000000000200ULL, 0x0000000000000400ULL, 0x0000000000000800ULL, 0x0000000000001000ULL, 0x0000000000002000ULL, 0x0000000000004000ULL, 0x0000000000008000ULL, 
        0x0000000000010000ULL, 0x0000000000020000ULL, 0x0000000000040000ULL, 0x0000000000080000ULL, 0x0000000000100000ULL, 0x0000000000200000ULL, 0x0000000000400000ULL, 0x0000000000800000ULL, 
        0x0000000001000000ULL, 0x0000000002000000ULL, 0x0000000004000000ULL, 0x0000000008000000ULL, 0x0000000010000000ULL, 0x0000000020000000ULL, 0x0000000040000000ULL, 0x0000000080000000ULL, 
        0x0000000100000000ULL, 0x0000000200000000ULL, 0x0000000400000000ULL, 0x0000000800000000ULL, 0x0000001000000000ULL, 0x0000002000000000ULL, 0x0000004000000000ULL, 0x0000008000000000ULL, 
        0x0000010000000000ULL, 0x0000020000000000ULL, 0x0000040000000000ULL, 0x0000080000000000ULL, 0x0000100000000000ULL, 0x0000200000000000ULL, 0x0000400000000000ULL, 0x0000800000000000ULL, 
        0x0001000000000000ULL, 0x0002000000000000ULL, 0x0004000000000000ULL, 0x0008000000000000ULL, 0x0010000000000000ULL, 0x0020000000000000ULL, 0x0040000000000000ULL, 0x0080000000000000ULL, 
        0x0100000000000000ULL, 0x0200000000000000ULL, 0x0400000000000000ULL, 0x0800000000000000ULL, 0x1000000000000000ULL, 0x2000000000000000ULL, 0x4000000000000000ULL, 0x8000000000000000ULL, 
    };
    {
        uint64_t start;
        start = GetULLTime();
        uint64_t pt = 1;
        for (int i=0; i<atoi(argv[1]); ++i)
            pt ^= table64[i % 64];
        cout << "Tempo per calcolare " << pt << ": " << GetULLTime()-start << endl;
        
        start = GetULLTime();
        pt = 1;
        for (int i=0; i<atoi(argv[1]); ++i)
            pt ^= 1ULL<<(i % 64);
        cout << "Tempo per calcolare " << pt << ": " << GetULLTime()-start << endl;

//         start = GetULLTime();
//         for (int i=0; i<atoi(argv[1]); ++i)
//             pt ^= table64[mtfa_rand_simple_ull()&0x000000000000003F];
//         cout << "Tempo per calcolare " << pt << ": " << GetULLTime()-start << endl;
//         
//         start = GetULLTime();
//         for (int i=0; i<atoi(argv[1]); ++i)
//             pt ^= 1ULL<<(mtfa_rand_simple_ull()&0x000000000000003F);
//         cout << "Tempo per calcolare " << pt << ": " << GetULLTime()-start << endl;
        exit(0);
    }
#endif
/**************************************************/

// template <typename Rank, typename Parent>
// void algo(Rank& r, Parent& p, std::vector<Element>& elements)
// {
//  boost::disjoint_sets<Rank,Parent> dsets(r, p);
//  for (std::vector<Element>::iterator e = elements.begin();
//       e != elements.end(); e++)
//   dsets.make_set(*e);
// }
#if 0   //Base64 usato in guile-development
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

int base64_encode(const unsigned char *in,  unsigned long inlen,
                                 char *out, unsigned long *outlen)
{
    return _base64_encode_internal(in, inlen, out, outlen, codes_base64, 1);
}

int base64url_encode(const unsigned char *in,  unsigned long inlen,
                                    char *out, unsigned long *outlen)
{
    return _base64_encode_internal(in, inlen, out, outlen, codes_base64url, 0);
}

int base64url_strict_encode(const unsigned char *in,  unsigned long inlen,
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
int base64_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64, insane);
}
int base64_strict_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
   return _base64_decode_internal(in, inlen, out, outlen, map_base64, strict);
}
int base64_sane_decode(const char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen)
{
   return _base64_decode_internal(in, inlen, out, outlen, map_base64, relaxed);
}

int base64url_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, insane);
}

int base64url_strict_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, strict);
}

int base64url_sane_decode(const char *in,  unsigned long inlen,
                           unsigned char *out, unsigned long *outlen)
{
    return _base64_decode_internal(in, inlen, out, outlen, map_base64url, relaxed);
}
#endif

void DoTime ( const string op, const string where, long long int DIM, unsigned long long start, unsigned long long end ) {
    unsigned long long tempo = end - start;
    cout << op <<  " " << DIM << " items in " << where << ": " << tempo << " us. (" << ( ( long double ) DIM ) / ( ( long double ) tempo ) << " " << op << "/us)" << endl;
    }
    
    

#if 0   //era una prova la butterei vvvvvia

class mtfa_sequence
{
private:
    PerfectHash * ph;

public:
     vector<pair<mtfads_pair*, int>> vpairs;
    ~mtfa_sequence();
    mtfa_sequence();
    list<uint8_t> ConvertLoS2LoB3( const list<string> & ls);
    vector<uint8_t> ConvertLoS2VoB3( const list<string> & ls);
    vector<uint8_t> ConvertLoB3VoB3( const list<uint8_t> & ls);
    void Insert ( const list<string> & ls);
    void Insert ( const list<uint8_t> & ls);
    void Insert ( const vector<uint8_t> & vs);
    bool Search ( const list<string> & ls);
    bool Search ( const list<uint8_t> & ls);
    bool Search ( const vector<uint8_t> & vs);


//     vector<uint32_t> ConvertLoS2VoL( const list<string> & ls);
//     void Insert ( const vector<uint32_t> & vs);
//     bool Search ( const vector<uint32_t> & vs);
    
    //ora alcune ricerche "intelligenti"
    //1) tutto quello che c'è sotto un prefisso dato, linearizzato
    list<list<uint8_t>> Children(const list<uint8_t> & lui8);
    
//     void Serialize ( mtfa_binary_stream_save * bs );
//     void Deserialize ( mtfa_binary_stream_load * bs );
};

mtfa_sequence::~mtfa_sequence()
{
    if (ph)
        delete ph;
    ph = nullptr;
     for ( auto it : vpairs )
          if ( it.first) {
               delete it.first;
               it.first = nullptr;
          }
     vpairs.clear();
//      for ( auto it : whole_strings )
//           if ( it.first ) {
//                free ( it.first );
//           }
//      whole_strings.clear();
}

mtfa_sequence::mtfa_sequence()
{
    ph = new PerfectHash();
}

list<uint8_t> mtfa_sequence::ConvertLoS2LoB3( const list<string> & ls)
{
    //per prima cosa converte la lista di stringhe in lista di interi
    list<uint8_t> li8;
    for (auto &s: ls)
    {
        //ogni indice è al più tre bytes e lo converto in tre indici da 1 byte ognuno
        auto idx3B = ph->Insert(s.data(), s.size());
        unsigned char idx0, idx1, idx2;
        idx0 = (idx3B & 0x00FF0000) >> 16;
        idx1 = (idx3B & 0x0000FF00) >>  8;
        idx2 = (idx3B & 0x000000FF) >>  0;
        li8.push_back(idx0);
        li8.push_back(idx1);
        li8.push_back(idx2);
    }
    return li8;
}
vector<uint8_t> mtfa_sequence::ConvertLoS2VoB3( const list<string> & ls)
{
//     auto lui8 = ConvertLoS2LoB3(ls);
//     std::vector<uint8_t> v{ std::make_move_iterator(std::begin(lui8)), std::make_move_iterator(std::end(lui8)) };
//     return v;

    //per prima cosa converte la lista di stringhe in lista di interi
    vector<uint8_t> v8;
    v8.reserve(3*ls.size());
    auto pv8 = v8.begin();
    for (auto &s: ls)
    {
        //ogni indice è al più tre bytes e lo converto in tre indici da 1 byte ognuno
        auto idx3B = ph->Insert(s.data(), s.size());
//         *pv8++ = (idx3B & 0x00FF0000) >> 16;
//         *pv8++ = (idx3B & 0x0000FF00) >>  8;
//         *pv8++ = (idx3B & 0x000000FF) >>  0;
        auto puc = (unsigned char *)&idx3B;
        *pv8++ = *puc++;
        *pv8++ = *puc++;
        *pv8++ = *puc;
    }
    return v8;
}
vector<uint8_t> mtfa_sequence::ConvertLoB3VoB3( const list<uint8_t> & ls)
{
    std::vector<uint8_t> v;
    v.reserve(ls.size());
    std::copy(std::begin(ls), std::end(ls), std::back_inserter(v));
    return v;
}

// vector<uint32_t> mtfa_sequence::ConvertLoS2VoL( const list<string> & ls)
// {
//     //per prima cosa converte la lista di stringhe in lista di interi
//     vector<uint32_t> li;
//     for (auto &s: ls)
//     {
//         //ogni indice è al più tre bytes e lo converto in tre indici da 1 byte ognuno
//         auto idx3B = ph->Insert(s.data(), s.size());
//         li.push_back(idx3B);
//     }
//     return li;
// }

// void mtfa_sequence::Insert ( const vector<uint32_t> & li)
// {
//     //ora che ho una lista di bytes
//     //inserisco nella struttura dati
//     //Ricorda che la convenzione è la seguente:
//     //SE zerox8000, allora posto vuoto
//     //SE zerox4000, allora termine sequenza e quindi il codice indicato mi potrà servire in seguito
//     //SE zeroxC000, allora una sequenza di ferma qui e un'altra prosegue con il codice indicato
//     if (vpairs.size() < li.size())
//     {
//         auto olds = vpairs.size();
//         vpairs.resize(li.size(), make_pair(nullptr, 0));
//         for (int i=olds; i<vpairs.size(); ++i)
//             vpairs[i].first = new mtfads_pair(0x01000000);
//     } 
//         
//     int vpos = 0;
//     int last_idx = 0;
//     for (auto idx: li)
//     {
//         //inserisco last_idx e idx e ottengo il prossimo indice
//         unsigned long long * pindex;
//         bool bret = vpairs[vpos].first->Exists(idx, last_idx, &pindex);
//         if (bret)
//         {
//             //already exists, continue
//             last_idx = (unsigned long long)*pindex  & zerox3FFFULL; //i due bit alti servono per ricordare che il sito è vuoto e altre info
//         }
//         else
//         {
//             //insert, please
//             unsigned long long * pret = vpairs[vpos].first->Insert(idx, last_idx, vpairs[vpos].second);
//             last_idx = vpairs[vpos].second++;
//         }
//         vpos++;
//     }
// }
// 
// bool mtfa_sequence::Search ( const vector<uint32_t> & vs)
// {
//     if (vs.size() > vpairs.size())
//         return false;
// 
//     int vpos = 0;
//     int last_idx = 0;
//     for (auto idx: vs)
//     {
//         //cerco last_idx e idx e ottengo il prossimo indice
//         unsigned long long * pindex;
//         bool bret = vpairs[vpos].first->Exists(idx, last_idx, &pindex);
//         if (bret)
//         {
//             //already exists, continue
//             last_idx = (unsigned long long)*pindex & zerox3FFFULL; //i due bit alti servono per ricordare che il sito è vuoto e altre info
//         }
//         else
//         {
//             return false;
//         }
//         vpos++;
//     }
//     return true;
// }

    
void mtfa_sequence::Insert ( const vector<uint8_t> & li8)
{
    //ora che ho una lista di bytes
    //inserisco nella struttura dati
    //Ricorda che la convenzione è la seguente:
    //SE zerox8000, allora posto vuoto
    //SE zerox4000, allora termine sequenza e quindi il codice indicato mi potrà servire in seguito
    //SE zeroxC000, allora una sequenza di ferma qui e un'altra prosegue con il codice indicato
    if (vpairs.size() < li8.size())
    {
        auto olds = vpairs.size();
        vpairs.resize(li8.size(), make_pair(nullptr, 0));
        for (int i=olds; i<vpairs.size(); ++i)
            vpairs[i].first = new mtfads_pair(256);
    } 
        
    int vpos = 0;
    int last_idx = 0;
    for (auto idx8: li8)
    {
        //inserisco last_idx e idx e ottengo il prossimo indice
        unsigned long long * pindex;
        bool bret = vpairs[vpos].first->Exists(idx8, last_idx, &pindex);
        if (bret)
        {
            //already exists, continue
            last_idx = (unsigned long long)*pindex  & zerox3FFFULL; //i due bit alti servono per ricordare che il sito è vuoto e altre info
        }
        else
        {
            //insert, please
            unsigned long long * pret = vpairs[vpos].first->Insert(idx8, last_idx, vpairs[vpos].second);
            //ora, la coppia idx8,last_idx ha in idx pari a vpairs[vpos].second e incremento il valore di indice
            last_idx = vpairs[vpos].second++;
        }
        vpos++;
    }
}

void mtfa_sequence::Insert ( const list<uint8_t> & li8)
{
    return Insert(ConvertLoB3VoB3(li8));
}


void mtfa_sequence::Insert ( const list<string> & ls)
{
    //per prima cosa converte la lista di stringhe in lista di interi
    return Insert(ConvertLoS2VoB3(ls));
}

bool mtfa_sequence::Search ( const vector<uint8_t> & li8)
{
    if (li8.size() > vpairs.size())
        return false;

    int vpos = 0;
    int last_idx = 0;
    for (auto idx8: li8)
    {
        //cerco last_idx e idx e ottengo il prossimo indice
        unsigned long long * pindex;
        bool bret = vpairs[vpos].first->Exists(idx8, last_idx, &pindex);
        if (bret)
        {
            //already exists, continue
            last_idx = (unsigned long long)*pindex & zerox3FFFULL; //i due bit alti servono per ricordare che il sito è vuoto e altre info
        }
        else
        {
            return false;
        }
        vpos++;
    }
    return true;
}

bool mtfa_sequence::Search ( const list<string> & ls )
{
    //per prima cosa converte la lista di stringhe in lista di interi
    auto li8 = ConvertLoS2VoB3(ls);
    return Search ( li8 );
}

bool mtfa_sequence::Search ( const list<uint8_t> & ls )
{
    //per prima cosa converte la lista di stringhe in lista di interi
    auto li8 = ConvertLoB3VoB3(ls);
    return Search ( li8 );
}

list<list<uint8_t>> mtfa_sequence::Children(const list<uint8_t> & lui8)
{
    list<list<uint8_t>> llui8;
    if (lui8.size() >= vpairs.size())
        return llui8;

    //vado fino al nodo  terminale della stringa di ricerca
    int vpos = 0;
    int last_idx = 0;
    for (auto idx8: lui8)
    {
        //cerco last_idx e idx e ottengo il prossimo indice
        unsigned long long * pindex;
        bool bret = vpairs[vpos].first->Exists(idx8, last_idx, &pindex);
        if (bret)
        {
            //already exists, continue
            last_idx = (unsigned long long)*pindex & zerox3FFFULL; //i due bit alti servono per ricordare che il sito è vuoto e altre info
        }
        else
        {
            return llui8;
        }
        vpos++;
    }
    
    //ora last_idx punta ad una <b> i cui corrispondenti valori di <a> mi danno tutti i figli
    auto b = last_idx;
    while (vpos < vpairs.size())
    {
        //il valore di a va da 0 a 255
        for (int a=0; a<256; ++a)
        {
            auto idxa = vpairs[vpos].first->lmat->vna[a];
            if (idxa == zerox8000)
                continue;
            auto idxb = vpairs[vpos].first->lmat->vm[idxa]->vb[b];
            if ( idxb == zerox8000)
                continue;
            
            auto abmat = vpairs[vpos].first->lmat->vm[idxa]->ms_b[a][idxb];
            if (abmat == zerox8000 || abmat != b)
                continue;
            
            //Il valore di indice è vmat
            auto vmat = vpairs[vpos].first->lmat->vm[idxa]->ms_v[a][idxb];
            if (vmat == zerox8000ULL)
                continue;
            
            //il valore del prossimo indice l'ho ottenuto!
        }
    }
    return llui8;
}

// void mtfa_sequence::Serialize ( mtfa_binary_stream_save * bs )
// {
//      //    vector<pair<unsigned int, mtfads_pair*>> vpairs;  //idx partono da 1
//      //    vector<pair<char*, int>> whole_strings;  //Contiene le stringhe che sono state archiviate, anche parzialmente, nella struttura
//      unsigned int len = vpairs.size();
//      bs->Append ( len );
//      for ( auto it : vpairs ) {
//           bs->Append ( it.first );
//           it.second->Serialize ( bs );
//      }
// 
//      len = whole_strings.size();
//      bs->Append ( len );
//      for ( auto it : whole_strings ) {
//           bs->Append ( ( unsigned char* ) it.first, it.second );
//      }
// }
// 
// void mtfa_sequence::Deserialize ( mtfa_binary_stream_load * bs )
// {
//      //    vector<pair<unsigned int, mtfads_pair*>> vpairs;  //idx partono da 1
//      //    vector<pair<char*, int>> whole_strings;  //Contiene le stringhe che sono state archiviate, anche parzialmente, nella struttura
//      for ( auto &it : vpairs ) {
//           if ( it.second ) {
//                delete it.second;
//           }
//      }
//      vpairs.clear();
// 
//      for ( auto it : whole_strings ) {
//           if ( it.first ) {
//                delete it.first;
//           }
//      }
//      whole_strings.clear();
// 
//      unsigned int len = bs->Read<unsigned int>();
//      vpairs.resize ( len );
//      for ( int i = 0; i < len; ++i ) {
//           int pos = bs->Read<unsigned int>();
//           mtfads_pair* p = new mtfads_pair();
//           p->Deserialize ( bs );
//           vpairs[i] = make_pair ( pos, p );
//      }
// 
//      len = bs->Read<unsigned int>();
//      whole_strings.resize ( len );
//      for ( int i = 0; i < len; ++i ) {
//           unsigned int lens;
//           unsigned char * s = ( unsigned char* ) bs->Read<unsigned char> ( lens );
//           whole_strings[i] = make_pair ( s, lens );
//      }
// }

/********************************************************************************************************************/
/********************************************************************************************************************/
/*********************************************** mtfa_sequence ******************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
#endif

#if 0  //coroutines
struct node{
    typedef std::shared_ptr<node> ptr_t;

    // Each tree node has an optional left subtree,
    // an optional right subtree and a value of its own.
    // The value is considered to be between the left
    // subtree and the right.
    ptr_t       left,right;
    std::string value;

    // construct leaf
    node(const std::string& v):
        left(),right(),value(v)
    {}
    // construct nonleaf
    node(ptr_t l,const std::string& v,ptr_t r):
        left(l),right(r),value(v)
    {}

    static ptr_t create(const std::string& v){
        return ptr_t(new node(v));
    }

    static ptr_t create(ptr_t l,const std::string& v,ptr_t r){
        return ptr_t(new node(l,v,r));
    }
};

node::ptr_t create_left_tree_from(const std::string& root){
    /* --------
         root
         / \
        b   e
       / \
      a   c
     -------- */
    return node::create(
            node::create(
                node::create("a"),
                "b",
                node::create("c")),
            root,
            node::create("e"));
}

node::ptr_t create_right_tree_from(const std::string& root){
    /* --------
         root
         / \
        a   d
           / \
          c   e
       -------- */
    return node::create(
            node::create("a"),
            root,
            node::create(
                node::create("c"),
                "d",
                node::create("e")));
}

typedef boost::coroutines2::coroutine<std::string>   coro_t;

// recursively walk the tree, delivering values in order
void traverse(node::ptr_t n, coro_t::push_type& out) 
{
    if(n->left) traverse(n->left,out);
    out(n->value);
    if(n->right) traverse(n->right,out);
}

void pippo()
// evaluation
{
	{
    	node::ptr_t left_d(create_left_tree_from("d"));
    	coro_t::pull_type left_d_reader([&](coro_t::push_type & out){
                                        	traverse(left_d,out);
                                    	});
	
    	node::ptr_t right_b(create_right_tree_from("b"));
    	coro_t::pull_type right_b_reader([&](coro_t::push_type & out){
                                        	traverse(right_b,out);
                                     	});
	
    	std::cout << "left tree from d == right tree from b? "
              	<< std::boolalpha
              	<< std::equal(begin(left_d_reader),
                            	end(left_d_reader),
                            	begin(right_b_reader))
              	<< std::endl;
	}
	{
    	node::ptr_t left_d(create_left_tree_from("d"));
    	coro_t::pull_type left_d_reader([&](coro_t::push_type & out){
                                        	traverse(left_d,out);
                                    	});
	
    	node::ptr_t right_x(create_right_tree_from("x"));
    	coro_t::pull_type right_x_reader([&](coro_t::push_type & out){
                                         	traverse(right_x,out);
                                     	});
	
    	std::cout << "left tree from d == right tree from x? "
              	<< std::boolalpha
              	<< std::equal(begin(left_d_reader),
                            	end(left_d_reader),
                            	begin(right_x_reader))
              	<< std::endl;
	}
	std::cout << "Done" << std::endl;
}

#endif

//#define theRLP 1
#ifdef theRLP
UniValue global_jval;
RLPValue global_rval;
static bool parseJsonInput(const std::string& body)
{
	bool rc = global_jval.read(body);

	if (!rc)
		fprintf(stderr, "JSON input validation failed\n");

	return rc;
}

static bool parseRlpInput(const std::string& body)
{
	std::vector<unsigned char> buf;

	if (body.substr(0, 2) == "0x") {
		string tmp = body.substr(2);
		buf = ParseHex(tmp);
	} else
		buf = ParseHex(body);

	size_t consumed, wanted;
	bool rc = global_rval.read(&buf[0], buf.size(), consumed, wanted);

	if (!rc)
		fprintf(stderr, "RLP input validation failed (%zu wanted)\n",
			wanted);

	return rc;
}

static string getJsonOutput()
{
	string body = global_jval.write();
	return body;
}

static string getRlpOutput()
{
	string body = global_rval.write();
	string hex = HexStr(body.begin(), body.end());
    return hex;
}

extern void RLPtoJSON(const RLPValue& rval, UniValue& jval);
extern bool JSONtoRLP(const UniValue& jval, RLPValue& rval);
#endif

#if 1
static mpz_class UnsignedPChar2Mpz ( const unsigned char* p, unsigned int len )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), len, 1, 1, 1, 0, p );
    return value;
}

static void Mpz2UnsignedPChar ( mpz_class value, unsigned char*& p, int& len )
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
        int count = ( mpz_sizeinbase ( value.get_mpz_t(), 2 ) + numb - 1 ) / numb;
        count += 16; //per ogni evenienza!
        p = ( unsigned char* ) malloc ( count * size );

        int countp = 0;

        mpz_export ( p, ( size_t* ) & countp, 1, size, 1, 0, value.get_mpz_t() );
        len = countp;
    }
}

#endif

// unsigned long long * myptr=0;
// bool myflag=false;
// bool myflag1=false;

#if 1
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/dynamic_bitset.hpp>
using namespace boost::multiprecision;

static unsigned int ffs256(const boost::multiprecision::uint256_t &n) {
  if (n.is_zero()) {
    return -1;
  } else {
    return boost::multiprecision::lsb(n);
  }
}
#endif


unsigned long long mpz2ull(mpz_t z)
{
    unsigned long long result = 0;
    mpz_export(&result, 0, -1, sizeof result, 0, 0, z);
    return result;
}

void ull2mpz(mpz_t z, unsigned long long ull)
{
    mpz_import(z, 1, -1, sizeof ull, 0, 0, &ull);
}

#if 0
#define BIGMAT_ROWS 256
#define BIGMAT_COLS 65536 //2097152
#define BIGMAT_BIT_COLS 0x0000FFFF //0x001FFFFF
unsigned long long bigmat[BIGMAT_ROWS][BIGMAT_COLS];

class mtfa_mat_cypher_hybrid
{
private:
    //Sono 8 matrici (vettori da 65536 unsigned long long). Ne utilizziamo due per calcolare la prossima posizione, una per calcolare la formula e sei per il valore.
    mtfa_fastrng1024 mask;
    
    //posizione e valore attuale nelle 8 matrici
    unsigned char xpos[8];
    unsigned char vpos[8];
    void NextPosition();
    int where;   //la posizione dell'ultimo elemento consumato (0-7)
public:
    void SetMask ( unsigned long long seed );
    void SetMask ( const char * data, int len );
    void cypher ( char * data, int len ); //Cifra incrementale. Modifica direttamente i dati di ingresso
    mtfa_mat_cypher_hybrid ( unsigned long long seed );
    mtfa_mat_cypher_hybrid ( const char * data, int len );
};

void mtfa_mat_cypher_hybrid::SetMask ( unsigned long long seed )
{
    where = 0;

    mask = mtfa_fastrng1024 ( seed );
/*
    * ( unsigned long long* ) &x[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[7][ mask.next() & BIGMAT_BIT_COLS];*/
}

void mtfa_mat_cypher_hybrid::SetMask ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

//     * ( unsigned long long* ) &x[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
//                                         bigmat[7][ mask.next() & BIGMAT_BIT_COLS];
}

mtfa_mat_cypher_hybrid::mtfa_mat_cypher_hybrid ( unsigned long long seed )
{
    where = 0;
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &xpos[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[7][ mask.next() & BIGMAT_BIT_COLS];
    * ( unsigned long long* ) &vpos[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[7][ mask.next() & BIGMAT_BIT_COLS];
    NextPosition();
}


mtfa_mat_cypher_hybrid::mtfa_mat_cypher_hybrid ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &xpos[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[7][ mask.next() & BIGMAT_BIT_COLS];
    * ( unsigned long long* ) &vpos[0] =   bigmat[0][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[1][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[2][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[3][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[4][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[5][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[6][ mask.next() & BIGMAT_BIT_COLS] ^
                                        bigmat[7][ mask.next() & BIGMAT_BIT_COLS];
    NextPosition();
}

void mtfa_mat_cypher_hybrid::NextPosition()
{
    where = 0;
    * ( ( unsigned long long* ) &xpos[0] ) +=   bigmat[xpos[0] & 0x03 ][*(uint16_t *)&xpos[1]] ^
                                                mask.next();

    * ( ( unsigned long long* ) &vpos[0] ) +=   bigmat[xpos[4] & 0x03 ][*(uint16_t *)&xpos[5]] ^
                                                mask.next();
}

void mtfa_mat_cypher_hybrid::cypher ( char* data, int len )  //Cifra incrementale
{
    if ( len <= 0 )
        return;

    if ( where == 0 ) {
        int conta;
        for ( conta = 0; ( conta + 8 ) <= len; conta += 8 ) {
            * ( ( unsigned long long* ) &data[conta] ) ^= * ( unsigned long long* ) &vpos[0];
            NextPosition();
        }

        if ( conta < len ) {
            auto pd = &data[conta];
            auto px = &vpos[0];
            where = len - conta;

            while ( where-- > 0 )
                *pd++ ^= *px++;

            where = len - conta;
        }
    } else {
        if ( len <= ( 8 - where ) ) {
            auto pd = data;
            auto px = &vpos[where];
            where += len;

            while ( len-- > 0 )
                *pd++ ^= *px++;

            if ( where == 8 )
                NextPosition();
        } else {
            auto pd = data;
            auto px = &vpos[where];
            int quanti = 8 - where;

            while ( where++ < 8 )
                *pd++ ^= *px++;

            NextPosition();
            cypher ( &data[quanti], len - quanti );
        }
    }
}

#endif

#include <iostream>     // std::cout
#include <algorithm>    // std::reverse
#include <vector>       // std::vector

void tohex(char* s, int len) {
    for (int i=0; i<len; ++i) {
        printf("%02x", s[i]);
    }
}

int main ( int argc, char* argv[] )
{
    unsigned long long start;
    mtfa_random_generator_init (GetULLTime());
    
#if 0    //Ti mostra l'uso corretto dei web socket
    
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
  
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8889);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 6) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    auto cb = [](const string & data, bool & data_type) -> pair<string, bool>  {
        //cout << "Type: " << (data_type?"STRING":"BINARY") << "\nCB data: " << data << endl;
        auto newdata=data;
        reverse(newdata.begin(),newdata.end());
        return make_pair(newdata.c_str(), true); //data_type);
    };

    auto runme = [&cb](int new_socket) {
        int RCVBUFSIZE = 163840000;
        char *bin = new char[RCVBUFSIZE]; //[1024] = { 0 };
        char *bout = new char[RCVBUFSIZE]; //[1024] = { 0 };

        MyWebSocket * mws = new MyWebSocket(cb, nullptr);

        bool stop = false;
        thread threcv([&] () {
            int recvMsgSize;
            while ((recvMsgSize = read(new_socket, bin, RCVBUFSIZE)) > 0) { // Zero means end of transmission
                cout << "Impila in coda di ingresso: " << recvMsgSize << endl;
                auto ms = HexDump(bin, bin+recvMsgSize);
                //tohex(bin, recvMsgSize);
                cout << ms << endl;
                mws->q_in.push(string(bin, recvMsgSize));
                cout << "Impilato!\n";
                memset(bin, 0, RCVBUFSIZE);
            }
            stop = true;
            cout << "Chiuso!!!\n";
        });
        threcv.detach();
        
        // thread thsend([&] () {
        //     while (!stop) {
        //         if ((rand() % 100) < 10) {
        //             mws->prepare_to_send(make_pair("Esempio di invio " + to_string(rand()), true));
        //             cout << "INVIA: " << "Esempio di invio " << endl;
        //         }
        //         if ((rand() % 100) < 10) {
        //             mws->prepare_to_send(make_pair("Basta", true));
        //             cout << "INVIA: " << "Basta" << endl;
        //         }
        //         sleep(1);
        //     }
        //     cout << "Me ne sono accorto!!!!!!\n";
        // });
/*        
        thsend.detach();*/

        while (!stop) {
            auto sout = mws->q_out.front();
            cout << "Deve inviare: ";
            auto ms = HexDump(sout);
            cout << ms << endl;
            //tohex(sout.data(), sout.size());
            //cout << endl;
            int scritti=0;
            while (scritti != sout.size()) {
                int iret = send(new_socket, &sout.data()[scritti], sout.size()-scritti, 0);
                if (iret < 0) {
                    cout << "Errore" << endl;
                    exit(0);
                } else
                    scritti += iret;
            }
            cout << "Inviato\n";
        }
        cout << "Anche io!!!!!!\n";
        mws->q_in.stop();
        mws->q_out.stop();
        mws->close();
        cout << "Terminated" << endl;
        sleep (5);
        delete mws;
        delete[] bin;
        delete[] bout;
    };
    
    while ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) > 0) {
        thread a(runme, new_socket);
        a.detach();
    }    
/*
    memset(bin, 0, 16384);
    valread = read(new_socket, bin, 16284);
    printf("Ricevuto: %s\n", bin);
    

    auto ses = new MyWebSocket(cb);
    ses->q_in.push(bin);    //Qui gestisco la richiesta di connessione!!!
    while (true) {
        if (ses->q_out.empty()) {
            usleep(100);
            continue;
        }
        while (!ses->q_out.empty()) {
            strcpy(bout, ses->q_out.front().c_str());
            //cout << "Sending: " << bout << endl;
            int scritti=0;
            int lenbout= strlen(bout);
            while (scritti != lenbout) {
                int iret = send(new_socket, &bout[scritti], lenbout-scritti, 0);
                if (iret < 0) {
                    cout << "Errore" << endl;
                    exit(0);
                } else
                    scritti += iret;
            }
        }
        //printf("bout message sent\n");
        //Qui provo a inviare senza aver prima ricevuto una richiesta
        sleep(5);
        cout << "Invia" << endl;
        string s="Ciao";
        ses->prepare_to_send(make_pair(s, true));
        
        memset(bin, 0, 16384);
        
        valread = read(new_socket, bin, 16284);
        //printf("Ricevuto: %s\n", bin);
        ses->q_in.push(bin);
    }
    
  // closing the connected socket
    close(new_socket);*/
  // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
    
#endif
    

#if 0
    //leggo la nuova matrice grande e poi con questa cifro!!
    FILE *fin= fopen("bigmat.dat", "r");
    int letti = ReadAll(fileno(fin), (unsigned char*)&bigmat, 8*2097152*8);
    fclose(fin);
    cout << "Letti: " << letti << endl;
    
    if (atoi(argv[1])==0) {
        mtfa_mat_cypher_hybrid cif(1020304050607);
        mtfa_mat_cypher_hybrid decif(1020304050607);
        
        cif.SetMask(7060504030201);
        decif.SetMask(7060504030201);

        const int DATA_SIZE=atoi(argv[2]);
        unsigned char *data = new unsigned char[DATA_SIZE];
        for (int i=0; i<DATA_SIZE; ++i)
            data[i] = i & 0x00FF;

        start = GetULLTime();
        cif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_hybrid", "Cifra", DATA_SIZE, start, GetULLTime());

        start = GetULLTime();
        decif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_hybrid", "Decifra", DATA_SIZE, start, GetULLTime());

        //check!!
        for (int i=0; i<DATA_SIZE; ++i)
            if (data[i] != (i & 0x00FF)) {
                cout << "Errore cifra/decifra!!!" << endl;
                break;
            }
            
        delete[] data;
    } else {
        mtfa_mat_cypher_fast cif(100);
        mtfa_mat_cypher_fast decif(100);
        
        cif.SetMask(1111111111);
        decif.SetMask(1111111111);

        const int DATA_SIZE=atoi(argv[2]);
        unsigned char *data = new unsigned char[DATA_SIZE];
        for (int i=0; i<DATA_SIZE; ++i)
            data[i] = i & 0x00FF;

        start = GetULLTime();
        cif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_fast", "Cifra", DATA_SIZE, start, GetULLTime());

        start = GetULLTime();
        decif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_fast", "Decifra", DATA_SIZE, start, GetULLTime());

        //check!!
        for (int i=0; i<DATA_SIZE; ++i)
            if (data[i] != (i & 0x00FF)) {
                cout << "Errore cifra/decifra!!!" << endl;
                break;
            }
            
        delete[] data;
    }
    
    return 0;
#endif
    
    
#if 0
    //conto quanti numeri ci sono in bignumbers.dat
    FILE *fin = fopen("bignumbers.dat", "r");
    FILE *fou = fopen("bigmat.dat", "w");
    int linea = 0;
    for (int i=0; i<16777216; i++) {
        unsigned long long n;
        n=0;
        fscanf(fin, "%llu", &n);
        if (n <= 0)
        {
            cout << "Errore: " << n << " alla linea: " << linea << endl;
            break;
        }
        //cout << n << " ";
        write(fileno(fou), &n, 8);
        linea++;
    }
    cout << "Sono " << linea << " linee" << endl;
    fclose(fin);
    fclose(fou);
    return 0;
#endif
    
#if 0
    {
        mtfa_mat_cypher_hybrid cif(100);
        mtfa_mat_cypher_hybrid decif(100);
        
        cif.SetMask(1111111111);
        decif.SetMask(1111111111);

        const int DATA_SIZE=atoi(argv[1]);
        unsigned char *data = new unsigned char[DATA_SIZE];
        for (int i=0; i<DATA_SIZE; ++i)
            data[i] = i & 0x00FF;

        start = GetULLTime();
        cif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_hybrid", "Cifra", DATA_SIZE, start, GetULLTime());

        start = GetULLTime();
        decif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_hybrid", "Decifra", DATA_SIZE, start, GetULLTime());

        //check!!
        for (int i=0; i<DATA_SIZE; ++i)
            if (data[i] != (i & 0x00FF)) {
                cout << "Errore cifra/decifra!!!" << endl;
                break;
            }
            
        delete[] data;
    }
    
    {
        mtfa_mat_cypher_fast cif(100);
        mtfa_mat_cypher_fast decif(100);
        
        cif.SetMask(1111111111);
        decif.SetMask(1111111111);

        const int DATA_SIZE=atoi(argv[1]);
        unsigned char *data = new unsigned char[DATA_SIZE];
        for (int i=0; i<DATA_SIZE; ++i)
            data[i] = i & 0x00FF;

        start = GetULLTime();
        cif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_fast", "Cifra", DATA_SIZE, start, GetULLTime());

        start = GetULLTime();
        decif.cypher((char*)data, DATA_SIZE);
        DoTime("mtfa_mat_cypher_fast", "Decifra", DATA_SIZE, start, GetULLTime());

        //check!!
        for (int i=0; i<DATA_SIZE; ++i)
            if (data[i] != (i & 0x00FF)) {
                cout << "Errore cifra/decifra!!!" << endl;
                break;
            }
            
        delete[] data;
    }
    
    return 0;
    
#endif
    
    
#if 0
    BlumInteger *blum = nullptr;

    start = GetULLTime();
    for (int i=0; i<atoi(argv[2]); ++i) {
        blum = new BlumInteger(atoi(argv[1]));
    	cout << blum->getM() << endl;
    	cout << blum->getP() << endl;
    	cout << blum->getQ() << endl;
        delete blum;
    }
    DoTime("Blumint", "generazione", atoi(argv[1]), start, GetULLTime());
    return 0;
#endif
    
#if 0
    mpz_class num;
    Random::StrongRandom(64, num);
    mtfa_fastrng1024 rng(GetULLTime() ^ mpz2ull(num.get_mpz_t()));
    mtfa_fastrng64 rng64(GetULLTime() ^ mpz2ull(num.get_mpz_t()));
    SimpleRng rngs(GetULLTime() ^ mpz2ull(num.get_mpz_t()));

    start = GetULLTime();    
    unsigned long long tot=0;
    for (int i=0; i<atoi(argv[1]); ++i)
        tot+=rng.next();
    DoTime("generati", "fast1024", atoi(argv[1]), start, GetULLTime());

    start = GetULLTime();
    tot=0;
    for (int i=0; i<atoi(argv[1]); ++i)
        tot+=rng64.next();
    DoTime("generati", "fast64", atoi(argv[1]), start, GetULLTime());

#if 0
    start = GetULLTime();
    tot=0;
    for (int i=0; i<atoi(argv[1]); ++i)
        tot+=InternalRng();
    DoTime("generati", "internal", atoi(argv[1]), start, GetULLTime());
#endif

    start = GetULLTime();    
    tot=0;
    for (int i=0; i<atoi(argv[1]); ++i)
        tot+=rngs.Next();
    DoTime("generati", "simple", atoi(argv[1]), start, GetULLTime());
    
    fastrand fr;
	
	uint32_t prngSeed[8];
	uint16_t *sptr = (uint16_t *)prngSeed;

	//
	// Randomize the seed values

	for(uint8_t i=0; i<8; i++)
	{
		prngSeed[i] = rand();
	}

	//
	// Initialize the PRNG

	InitFastRand(	sptr[0], sptr[1], 
		sptr[2], sptr[3],
		sptr[4], sptr[5],
		sptr[6], sptr[7],
		sptr[8], sptr[9],
		sptr[10], sptr[11],
		sptr[12], sptr[13],
		sptr[14], sptr[15],
		&fr);

    start = GetULLTime();    
    tot = 0;
    for(uint32_t i=0; i<(atoi(argv[1])/2); i++) {
		FastRand_SSE(&fr);
        uint64_t ull = fr.res[1];
        ull <<= 8;
        ull |= fr.res[0];
        tot += ull;

        ull = fr.res[3];
        ull <<= 8;
        ull |= fr.res[2];
        tot += ull;
	}
    DoTime("generati", "FastRand", atoi(argv[1]), start, GetULLTime());
	
    start = GetULLTime();    
    tot = 0;
//    unsigned char * data = (unsigned char *)malloc(
    for(uint32_t i=0; i<(atoi(argv[1])/2); i++) {
		FastRand_SSE4(&fr);
        uint64_t ull = fr.res[1];
        ull <<= 8;
        ull |= fr.res[0];
        tot += ull;

        ull = fr.res[3];
        ull <<= 8;
        ull |= fr.res[2];
        tot += ull;
	}
    DoTime("generati", "FastRand4", atoi(argv[1]), start, GetULLTime());
//     FILE * out = fopen("pippo.dat", "w");
//     write(fileno(out), &(fr.res[0]), 16);
//     fclose(out);

    return 0;

    
#endif    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
#if 0
    auto pat = "p+";
    cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", pat).size() << endl;  
    auto patt = mtfa_compile_pattern("p+", true);
    cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", patt).size() << endl;
    auto patf = mtfa_compile_pattern("p+", false);
    cout << mtfa_get_all_matching_positions("pipp2ppp3pppp4ppppp5p6p7p8", patf).size() << endl;
    
    exit(0);
#endif
    
    
#if 0
    list<int> li;
#define MAX 10000000
    for (int i=0; i<50000000; ++i)
        li.push_back(mtfa_rand_simple_ull()% MAX);

    mtfa_bitset bs(MAX);
    mtfa_bitset_1024 bs1024(MAX);

    start = GetULLTime();
    auto it = li.begin();
    for (int i=0; i<MAX; ++i)
        bs.set(*it++);
    DoTime("insert", "bitset", MAX, start, GetULLTime());
    
    start = GetULLTime();
    it = li.begin();
    for (int i=0; i<MAX; ++i)
        bs1024.set(*it++);
    DoTime("insert", "bitset_1024", MAX, start, GetULLTime());
    
    
    mpz_class mpv = 0;
    start = GetULLTime();
    it = li.begin();
    auto ptr = mpv.get_mpz_t();
    for (int i=0; i<MAX; ++i)
        mpz_setbit(ptr, *it++);
    DoTime("insert", "mpz_class", MAX, start, GetULLTime());
    

//     cout << bs.toString() << endl;
//     cout << bs1024.toString() << endl;
    
    exit(0);
#endif
    
#if 0
    list<int> li;
    for (int i=0; i<10000000; ++i)
        li.push_back(mtfa_rand_simple_ull()% 1000000);

    vector<mtfa_bitset *> vbs;
    vector<cpp_int> vui;
    vector<boost::dynamic_bitset<>> vdb;
    
    cpp_int tall=1; tall <<= 1000000; tall--;
    
    for (int i=0; i<100; ++i) {
        vbs.push_back(new mtfa_bitset(1000000));
        vbs[i]->reset();
        vui.push_back(cpp_int(0));
        vdb.push_back(boost::dynamic_bitset<>(1000000));
    }
    
    //inserisco un pochino di dati
    start = GetULLTime();
    auto it = li.begin();
    for (int i=0; i<10; ++i) {
        for (int j=0; j<100000; ++j) {
            vbs[i]->set(*it);
        }
    }
    DoTime("A", "VBS", 10*100000, start, GetULLTime());

    start = GetULLTime();
    it = li.begin();
    for (int i=0; i<10; ++i) {
        for (int j=0; j<100000; ++j) {
            bit_set(vui[i], *it++);
        }
    }
    DoTime("A", "VUI", 10*100000, start, GetULLTime());

    start = GetULLTime();
    it = li.begin();
    for (int i=0; i<10; ++i) {
        for (int j=0; j<100000; ++j) {
            vdb[i].set(*it++);
        }
    }
    DoTime("A", "VDB", 10*100000, start, GetULLTime());

    //     //Stampo tutto
//     for (int i=0; i<100; ++i) {
//         cout << vbs[i]->toString() << endl;
//         cout << vui[i] << endl;
//     }
    
//     //per ognuno verifico la corrispondenza dei bit
//     for (int i=0; i<100; ++i) {
//         for (int j=0; j<256; ++j) {
//             auto vbs1 = vbs[i]->test(j);
//             auto vui1 = bit_test(vui[i], j);
//             if (vbs1 != vui1)
//                 cout << "0 => (" << i << ", " << j << "): " << vbs1 << ", " << vui1 << endl;
//         }
//     }
//     
//     for (int i=0; i<50; ++i) {
//         mtfa_bitset vbs1 = (*vbs[i]) | *(vbs[i+50]);
//         auto vui1 = vui[i] | vui[i+50];
//     }
//     for (int i=0; i<50; ++i) {
//         for (int j=0; j<256; ++j) {
//             auto vbs1 = vbs[i]->test(j);
//             auto vui1 = bit_test(vui[i], j);
//             if (vbs1 != vui1)
//                 cout << "1 => (" << i << ", " << j << "): " << vbs1 << ", " << vui1 << endl;
//         }
//     }
// 
//     for (int i=0; i<100; ++i) {
//         auto pos1 = vbs[i]->FirstZero();
//         auto pos2 = ffs256(vui[i]^tall);
//         if (pos1!= pos2)
//             cout << "2 => (" << i << "): " << pos1<< ", " << pos2<< endl;
//         
//     }

    exit(0);
#endif
#if 0
    list<list<string>> lls;
    int len = atoi(argv[1]);
    for (int i=0; i<len; ++i)
        lls.push_back(std::list<string>{mtfa_rand_string(10, 10), mtfa_rand_string(10, 10), mtfa_rand_string(10, 10)});

//     list<list<string>> lls1;
//     for (int i=0; i<100000; ++i)
//         lls1.push_back(std::list<string>{mtfa_rand_string(10, 10), mtfa_rand_string(10, 10), mtfa_rand_string(10, 10)});
//     
//     list<list<string>> lls2;
//     for (int i=0; i<100000; ++i)
//         lls2.push_back(std::list<string>{mtfa_rand_string(10, 10), mtfa_rand_string(10, 10), mtfa_rand_string(10, 10)});
    
    mtfa_string_sequences * sss = new mtfa_string_sequences();
    
    start=GetULLTime();
    for (auto & ls: lls)
        sss->insert(ls);
    DoTime("insert", "sss", len, start, GetULLTime());
//     start=GetULLTime();
//     for (auto & ls: lls1)
//         sss->insert(ls);
//     DoTime("insert", "sss", 100000, start, GetULLTime());
//     start=GetULLTime();
//     for (auto & ls: lls2)
//         sss->insert(ls);
//     DoTime("insert", "sss", 100000, start, GetULLTime());
    
    delete sss;
    exit(0);
#endif    
    
#if 0    
    int colonnaok;
    
#define set_size 1000000
#define num_add 2000000
#define cicli    1
    
    list<int> li;
    for (int i=0; i<256; ++i) {
        for (int j=0; j<num_add; ++j)
            li.push_back(mtfa_rand_simple_ull()%set_size);
    }

    {
        vector<mtfa_bitset*> occupati;
        list<int>::iterator it = li.begin();
        for (int i=0; i<256; ++i) {
            occupati.push_back(new mtfa_bitset(set_size));
            for (int j=0; j<num_add; ++j)
                occupati[i]->set(*it++);
        }
        
        //l'operazione determinante è: or di tutto e poi cerco il primo zero libero
        start=GetULLTime();

        for (int k=0; k<cicli; ++k) {
            mtfa_bitset p;
            for ( int i = 0; i < 256; ++i )   //->second)
            {
                p |= *occupati[i];
            }
            
            colonnaok = p.FirstZero();
        }
        DoTime("or", "firstZero", 256, start, GetULLTime());
    }
    
    cout << colonnaok << endl;
    
    {
        vector<std::bitset<set_size>> occupati(256);
        list<int>::iterator it = li.begin();
        for (int i=0; i<256; ++i) {
            occupati[i].set();
            for (int j=0; j<num_add; ++j)
                occupati[i].reset(*it++);
        }
        
        //l'operazione determinante è: or di tutto e poi cerco il primo zero libero
        start=GetULLTime();

        for (int k=0; k<cicli; ++k) {
            bitset<set_size> p;
            p.set();
            for ( int i = 0; i < 256; ++i )   //->second)
            {
                p &= occupati[i];
            }

            colonnaok = p._Find_first();
        }
        DoTime("or", "firstZero", 256, start, GetULLTime());
    }
    cout << colonnaok << endl;
  

    {
        vector<mpz_class> occupati(256);
        list<int>::iterator it = li.begin();
        for (int i=0; i<256; ++i) {
            occupati[i]=0;
            for (int j=0; j<num_add; ++j)
                mpz_setbit(occupati[i].get_mpz_t(), *it++);
        }
        
        //l'operazione determinante è: or di tutto e poi cerco il primo zero libero
        start=GetULLTime();

        for (int k=0; k<cicli; ++k) {
            mpz_class p=0;
            for ( int i = 0; i < 256; ++i )   //->second)
            {
                p |= occupati[i];
            }

            colonnaok = mpz_scan0(p.get_mpz_t(), 0);
        }
        DoTime("or", "firstZero", 256, start, GetULLTime());
    }
    cout << colonnaok << endl;
    
    {
        vector<uint256_t> occupati(256);
        list<int>::iterator it = li.begin();
        for (int i=0; i<256; ++i) {
            occupati[i]=0;
            for (int j=0; j<num_add; ++j) {
                cpp_int r(1);
                bit_set(occupati[i], *it++);
            }
        }
        
        //l'operazione determinante è: or di tutto e poi cerco il primo zero libero
        start=GetULLTime();

        for (int k=0; k<cicli; ++k) {
            uint256_t p=0;
            for ( int i = 0; i < 256; ++i )   //->second)
            {
                p |= occupati[i];
            }
            
            uint256_t t=1; t <<= 256; t--;
            colonnaok = ffs256(p^t);
        }
        DoTime("or", "firstZero", 256, start, GetULLTime());
    }
    cout << colonnaok << endl;
#endif

    
#if 0
    //Prova di brg
    BRG * brg;
	int len = 2048; //bytes
	if (argc > 1)
		len = atoi(argv[1]);


	int cicli = 128;
	if (argc > 2)
		cicli = atoi(argv[2]);
    
    unsigned long long seed = 123123123123;
    if (argc > 3)
		seed = strtoull(argv[3], nullptr, 10);
    
	auto seeded_random = [&](unsigned long long seed, int len) -> vector<char> {
		mtfa_fastrng1024 rg(seed);
		vector<char> data(len);
		for (int i = 0; i < len; ++i)
			data[i] = rg.next();
        return data;
	};

	start = micros();
	//brg = new BRG("ciccio", StrongRandom(len));
	brg = new BRG("ciccio", seeded_random(seed, len));
	auto lennew = micros() - start;
	long long totnext = 0;
	long long tot = 0;
	for (int i = 0; i < cicli; ++i)
	{
		auto start = micros();
		brg->Next();
		totnext += (micros() - start);
		ofstream fout;
		tot += brg->bufOriginal.size();
		cout << "Cicle: " << i << ". Writes a buf of size: " << brg->bufOriginal.size() << ". Written so far: " << tot << " bytes." << endl;
		fout.open("out.dat", ios::out | ios::app | ios::binary);
		fout.write((char*)brg->bufOriginal.data(), brg->bufOriginal.size());

		fout.close();
	}
	cout << "New: " << lennew << ". Run: " << totnext << endl;

	return 0;

    
    
#endif    
    
#if 0
    extern void mygo(vector<vector<uint8_t>> & vvui8);
    vector<vector<uint8_t>> vvui8;
    mygo(vvui8);
    mtfa_sequence_base * sb = new mtfa_sequence_base();
    
    
    
    int found = -1;
    for (auto vui8: vvui8)
    {
        if (vui8[0] == 0 && vui8[1] == 0 && vui8[2] == 26 && vui8[3] == 0 && vui8[4] == 0 && vui8[5] == 250)
        {
            cout << "OK: " << sb->Search(vector<uint8_t>{0, 14, 8, 0, 0, 250}) << endl;
        }
        if (vui8[0] == 0 && vui8[1] == 14 && vui8[2] == 8 && vui8[3] == 0 && vui8[4] == 0 && vui8[5] == 250)
        {
            cout << "KO: " << sb->Search(vector<uint8_t>{0, 14, 8, 0, 0, 250}) << endl;
//             myflag = true;
        }
        sb->Insert(vui8);
//         if (myflag)
//         {
//             printf("Assegnato myptr: %llX => %llX\n", myptr, *myptr);
//             myflag = false;
//             myflag1=true;
//         }
//         if (myflag1)
//         {
//             printf("Valore    myptr: %llX => %llX\n", myptr, *myptr);
//         }
        int l = sb->Search(vector<uint8_t>{0, 14, 8, 0, 0, 250});
        if (l > 0)
        {
            found = l;
        }
        if (found > 0 && l == 0)
        {
            for (auto i: vui8)
                cout << (int)i << ", ";
            cout << endl;
            cout << sb->Search(vector<uint8_t>{0, 14, 8, 0, 0, 250}) << endl;
//            cout << sb->Search1(vector<uint8_t>{0, 14, 8, 0, 0, 250}) << endl;
            exit(0);
        }
    }

    exit(0);
#endif
    
    
    
#if 0
    list<string> ls;
    start = GetULLTime();
    for (int i=0; i<10000; ++i)
    {
        int curlens = 8192+mtfa_rand_simple_ull()%8192;
        ls.push_back(mtfa_rand_alfanum(curlens, "abcdefghijklmnopqrstuvwxyz"));
    }
    DoTime("Generate", "LoS", ls.size(), start, GetULLTime());
    start = GetULLTime();
    for (auto &it: ls)
    {
	string b64 = Base64::encode(it);
	string d64 = Base64::decode(b64);
	if (d64.compare(it) != 0)
	    cout << "Errore" << endl;
    }
    DoTime("Encode-Decode", "b64-old", ls.size(), start, GetULLTime());
    start = GetULLTime();
    for (auto &it: ls)
    {
	string b64 = EncodeBase64(it);
	string d64 = DecodeBase64(b64);
	if (d64.compare(it) != 0)
	    cout << "Errore" << endl;
    }
    DoTime("Encode-Decode", "b64-new", ls.size(), start, GetULLTime());

    return 0;
	
#endif

#if 0
//#ifdef theRLP
    string body = argv[1];
    parseJsonInput(body);
    JSONtoRLP(global_jval, global_rval);
    
    string rlp = getRlpOutput();
    cout << "RLP: " << rlp << endl;
    
    parseRlpInput(rlp);
    RLPtoJSON(global_rval, global_jval);
    string json = getJsonOutput();
    cout << "JSON: " << json << endl;
    return 0;
#endif
#if 0 //signature secp256k1
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
    
    /*Firma con openssl 
     * OpenSSL_add_all_algorithms();

        const unsigned char* privKey = // Private Key 32 bytes
        const char* rlpTx = // nonce, gas limit etc encoded to RLP

        const uint8_t c_chainID = 1; // Mainnet

        BIGNUM* bn  = BN_bin2bn(privKey, 32, nullptr);
        EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);

        EC_KEY_set_private_key(key, bn);

        unsigned char* hash = nullptr;
        int size = 0;

        Keccak256::calcHash(rlpTx, hash, &size);

        ECDSA_SIG* sign = ECDSA_do_sign(hash, key);

        unsigned char* r = new unsigned char[BN_num_bytes(sign->r)];
        unsigned char* s = new unsigned char[BN_num_bytes(sign->s)];
        uint8_t        v = c_chainID * 2 + 35;

        BN_bn2bin(sign->r, r);
        BN_bn2bin(sign->s, s);

        ECDSA_SIG_free(sign);
        BN_free(bn); 
        EC_KEY_free(privKey);

        delete[] s;
        delete[] r;
        */

    
    /*
     *  Verifica della firma
     *  The signature must consist of a 32-byte big endian R value, followed by a
     *  32-byte big endian S value. If R or S fall outside of [0..order-1], the
     *  encoding is invalid. R and S with value 0 are allowed in the encoding.
     * 
     * 
     * (V with chain ID, 0x266e = 9838 = 4901 * 2 + 36,!!!!!!!!!!!
    
     *  Il valore V come è calcolato?
     *  FORK_BLKNUM: 2,675,000
     *  CHAIN_ID: 1 (main net)
     *  Specification
     *  If block.number >= FORK_BLKNUM and v = CHAIN_ID * 2 + 35 or v = CHAIN_ID * 2 + 36, 
     *  then when computing the hash of a transaction for purposes of signing or recovering, 
     *  instead of hashing only the first six elements (i.e. nonce, gasprice, startgas, to, value, data), 
     *  hash nine elements, with v replaced by CHAIN_ID, r = 0 and s = 0. The currently existing 
     *  signature scheme using v = 27 and v = 28 remains valid and continues to operate under the same rules as it does now.
     *  
     *  Example
     *  Consider a transaction with nonce = 9, gasprice = 20 * 10**9, startgas = 21000, to = 0x3535353535353535353535353535353535353535, value = 10**18, data='' (empty).
     *  
     *  The "signing data" becomes:
     *  
     *  0xec098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a764000080018080
        che tradotta da noi fornisce:   #(9 4c2a817c38800      5208    3535353535353535353535353535353535353535 dc3a0c2b6c2b3c2a7640000 0 1 0 0)
                                            1339927115827200   21000
     *  The "signing hash" becomes:
     *  
     *  0xdaf5a779ae972f972197303d7b574746c7ef83eadac0f2791ad23db92e4c8e53 =>
        (mtfa-bv-to-hex-string (mtfa-hash-keccak (mtfa-hex-string-to-bv "ec098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a764000080018080")))

     *  If the transaction is signed with the private key 0x4646464646464646464646464646464646464646464646464646464646464646, then the v,r,s values become:
     *  
     *  (37, 18515461264373351373200002665853028612451056578545711640558177340181847433846, 46948507304638947509940763649030358759909902576025900602547168820602576006531)
             28ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276               67cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83
             28ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276               67cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83

            mia firma
            766263AA200659E163FF713C5DA1E1036086677553FE9521BC39D90B3461EF28836D3B6A96B17F294B2164DCF3C955F5CC00384B3003B7EC1A767F99D8E9CB67
     *  Notice the use of 37 instead of 27. The signed tx would become:
     *  
     *  0xf86c098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a76400008025a028ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276a067cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83
     *  Rationale
     *  This would provide a way to send transactions that work on Ethereum without working on 
     *  ETC or the Morden testnet. ETC is encouraged to adopt this EIP but replacing CHAIN_ID 
     *  with a different value, and all future testnets, consortium chains and alt-etherea are 
     *  encouraged to adopt this EIP replacing CHAIN_ID with a unique value.
     *  
     *  List of Chain ID's:
     *  CHAIN_ID	Chain(s)
     *  1	Ethereum mainnet
     *  2	Morden (disused), Expanse mainnet
     *  3	Ropsten
     *  4	Rinkeby
     *  5	Goerli
     *  42	Kovan
     *  1337	Geth private chains (default)
    */

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
//    secp256k1_ec_pubkey_create()
#endif

#if 0
//     pippo();  //coroutines
//     exit(0);
//     
// 
// (load "test.scm")
// (define sss (mtfa-sss-make))
// (mtfa-sss-insert sss '("uno" "due" "tre"))
// (mtfa-sss-insert sss '("uno" "due" "tre1"))
// (mtfa-sss-getval sss 2)
// (mtfa-sss-getall sss '("uno"))
// (mtfa-sss-ls2bv sss '("uno" "tre"))
// 
// cdbcaptbdqacd
// a -> b -> c -> d
// 
// //algoritmo di ricerca (in un albero)
// classificare le strutture in modo da indicare un certo valore in che posizione si trova
// ES: in trj_1 a sta in 3, 7, 9, 123
//     in trj_2 a sta in 4, 7, 11, 20, 40
//     in trj_1 b sta in 7,9,100
//     in trj_2 b sta in 1,2
// 
// 
// oppure:
// Soluzione A)
// a3 trj_1
// a7 trj1, trj_2
// ...
// io ho il vantaggio che le traiettorie sono ordinate per definizione, poiché mentre le inserisco incremento il loro id.
// Quindi, se associo a ogni possibile valore le traiettorie che lo contengono
// avrei fatto un passo avanti
// - tutte le traiettorie che hanno a in {3..5} e b in {7..9}
// è l'intersezione di due insieme di id-s ottenuti come or => AND (OR(a{3-5}, b{87-9}))
// cosa che non è affatto complicata!
// 
// Altra possibilità è di mantenere l'albero completo in forma di struttura dati ad accesso diretto: questa però mi serve per poter affermare che un certo cammino (contiguo) è presente.
// ora, se volessi mantenere tutti i possibili cammini, che costituisce un secondo caso da esaminare, allora dovrei mantenere una struttura quadratica (se volessi mantenere solo le coppie) altrimenti molto ma molto più complessa, se volessi mantenere anche le terne, le quaterne ecc.
// 
// Ma, quanto mi costa un algoritmo di ricerca che estrae tutte le traiettorie che soddisfano una specifica condizione (a primadi b primadi c)? una navigazione completa dell'albero delle traiettorie 
// Sembra che quindi la soluzione più efficiente sia la A)
// in termini di spazio: ogni traiettoria è lunga 120
// ogni elemento compare in diversi posti di una traiettoria, in valore assoluto quanti elementi (variabili) diverse ho? e quanti valori ammissibili?
// Supponi 100 variabili e, per ogni variabile, 10 valori ammissibili
// in totale abbiamo
// 100(variabili)*10(valori)*120(posizioni) = 120000 item
// per ogni item abbiamo un array che mi dice in quali traiettorie si trova
// e quindi, avessimo 1000000 traiettorie con ogni variabile che tocca almeno il 10% delle traiettorie, avremmo
// 120000*100000=12.000.000.000 => 12 miliardi di elementi da memorizzare (nulla rispetto a macchine con 1T ram)
// ma per fare questo a cosa mi serve la parte C++ del linguaggio?
// - per ogni prefisso corrispondente
// 	- per ogni elemento corrispondente a primo elemento
// 		- per ogni elemento corrispondente a secondo elemento
// 			- per ogni elemento corrispondente a terzo elemento
// 				.......
// 					!!!trovata sequenza
// e le sequenze le devo trovare tutte
// 
#endif
   
#if 0

    int size = 10000;
    if (argc > 1)
        size = atoi(argv[1]);

    int len = 5;
    if (argc > 2)
        len= atoi(argv[2]);

    int lens = 10;
    if (argc > 3)
        lens= atoi(argv[3]);

    int maxlen = 7;
    if (argc > 4)
        maxlen= atoi(argv[4]);

    int maxlens = 15;
    if (argc > 5)
        maxlens= atoi(argv[5]);
    
    cout << "SIZE: " << size << ". LEN: " << len << ", " << maxlen << ". LENS: " << lens << ", " << maxlens << endl;

    //Generiamo qualche sequenza di stringhe
    list<list<string>> lls;
    start = GetULLTime();
    for (int i=0; i<size; ++i)
    {
        list<string> ls;
        int curlen = len+mtfa_rand_simple_ull()%(maxlen-len+1);
        for (int j=0; j<curlen; ++j)
        {
            int curlens = lens+mtfa_rand_simple_ull()%(maxlens-lens+1);
            ls.push_back(mtfa_rand_alfanum(curlens, "abcdefghijklmnopqrstuvwxyz"));
        }
        lls.push_back(ls);
    }
    DoTime("Generate", "LoS", lls.size(), start, GetULLTime());

    //vado con la mtfa_sequence_base
    //Questa struttura lavora esclusivamente su vettori di byte. é indipendente dalla loro lunghezza ecc, ecc, ecc.
//     mtfa_sequence_base *sb = new mtfa_sequence_base();
//     cout << sb->Insert(vector<uint8_t>{0,0,1,0,0,2}) << endl;
//     cout << sb->Insert(vector<uint8_t>{0,0,1,0,0,2}) << endl;
//     cout << sb->Insert(vector<uint8_t>{0,0,1,0,0,3}) << endl;
//     cout << sb->Insert(vector<uint8_t>{0,1,1,0,0,3}) << endl;
//     cout << sb->Insert(vector<uint8_t>{0,2,1,1,0,3}) << endl;
//     
//     cout << sb->Search(vector<uint8_t>{0,1,1,0,0,1}) << endl;
//     cout << sb->Search(vector<uint8_t>{0,0,1,0,0,1}) << endl;
//     cout << sb->Search(vector<uint8_t>{0,0,2,0,0,3}) << endl;
//     cout << sb->Search(vector<uint8_t>{0,1,1,0,0,2}) << endl;
//     cout << sb->Search(vector<uint8_t>{0,2,1,1,0,3}) << endl;
//     
//     cout << "\nStart from {}: ";
//     for (auto it: sb->GetAll(vector<uint8_t>()))
//         cout << it << " ";
//     cout << "\nStart from 0: ";
//     for (auto it: sb->GetAll(vector<uint8_t>({0})))
//         cout << it << " ";
//     cout << "\nStart from 0,0: ";
//     for (auto it: sb->GetAll(vector<uint8_t>({0,0})))
//         cout << it << " ";
//     cout << "\nStart from 0,0,0: ";
//     for (auto it: sb->GetAll(vector<uint8_t>({0,2,1,1})))
//         cout << it << " ";
//     cout << endl;
//     exit(0);
    
//     mtfa_string_sequences *mss = new mtfa_string_sequences();
//     cout << mss->insert(list<string>{"a1", "b23", "c345"}) << endl;
//     cout << mss->insert(list<string>{"a2", "b23", "c345"}) << endl;
//     cout << mss->insert(list<string>{"a1", "b232", "c345"}) << endl;
//     cout << mss->insert(list<string>{"a1", "b23", "c3451"}) << endl;
//     
//     cout << mss->search(list<string>{"a1", "b23", "c345"}) << endl;
//     cout << mss->search(list<string>{"a2", "b23", "c345"}) << endl;
//     cout << mss->search(list<string>{"a1", "b232", "c345"}) << endl;
//     cout << mss->search(list<string>{"a1", "b23", "c3451"}) << endl;
//     
//     cout << mss->search(list<string>{"a1", "b23", "1c345"}) << endl;
//     cout << mss->search(list<string>{"a2", "1b23", "c345"}) << endl;
//     cout << mss->search(list<string>{"1a1", "b232", "c345"}) << endl;
// 
//     for (auto s: mss->getval(0))
//         cout << s << " ";
//     cout << endl;
//     for (auto s: mss->getval(1))
//         cout << s << " ";
//     cout << endl;
//     for (auto s: mss->getval(2))
//         cout << s << " ";
//     cout << endl;
//     exit(0);

    mtfa_string_sequences ds;
    start = GetULLTime();
    int num = 0;
    for (auto & ls: lls)
    {
        ds.insert(ls);
        num+=ls.size();
        for (auto s: ls)
            num+=s.size();
    }
    DoTime("Insert", "DS", num, start, GetULLTime());

    start = GetULLTime();
    for (auto & ls: lls)
    {
        int iret= ds.search(ls);
        if (iret <= 0)
            cout << "Errore" << endl;
    }
    DoTime("Search", "DS", num, start, GetULLTime());

    cout << "1: " ;
    for (auto s: lls.front())
        cout << s << " ";
    cout << endl;
    
    
    cout << "0: " ;
    for (auto s: ds.getval(0))
        cout << s << " ";
    cout << endl;
    cout << "1: " ;
    for (auto s: ds.getval(1))
        cout << s << " ";
    cout << endl;
    cout << "2: " ;
    for (auto s: ds.getval(2))
        cout << s << " ";
    cout << endl;
    cout << "1: " ;
    for (auto s: ds.getvalvui8(1))
        cout << (int)s << " ";
    cout << endl;
    
    exit(0);

    getchar();
    
    //l'idea è una struttura dati "gerarchica" vista come sequenza di stringhe
    //a1.a2.a3.a4.a5.a6... e, in base al prefisso, ottengo tutte le sequenze interne
    //le dimensioni non sono limitate: mi aspetto 24 bit (16,777,216) per gli indici delle informazioni
    //la radice di tutto è 0 e poi si prosegue
    //quindi per cercare a1.a2.a3 => cercherò: 0:idx(a1):idx(a2):idx(a3):...
    
    exit(0);
    
#endif    
    
#if 0
    list<int> li;
    atomic<int> liLen;
    mutex mut;
    
    function<void()> finsert = [&] () {
        while (true)
        {
            if (liLen<5)
            {
                mut.lock();
                li.push_back(mtfa_rand_simple_ull());
                liLen++;
                mut.unlock();
            }
        }
    };
    function<void()> fremove = [&] () {
        while (true)
        {
            if (liLen>0)
            {
                mut.lock();
                cout << li.front()  << ", ";
                li.pop_front();
                liLen--;
                mut.unlock();
            }
        }
    };
    
    thread th1(finsert);
    thread th2(fremove);
    th1.detach();
    th2.detach();
    while (true)
    {
        sleep(1);
    }
    return 0;
#endif
    
#if 0
    cout << "Usage: " << argv[0] << " <quante conversioni> <lunghezza delle stringhe>" << endl;
    const int N = atoi(argv[1]);
    const int LEN = atoi(argv[2]);
    
    //genero le N stringhe da convertire
    list<string> ls;
    int disturber = -5;
    for (int i=0; i<N; ++i)
    {
        string s = mtfa_rand_alfanum(LEN+disturber, string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'?^+*-_.:,;<>!\"%/()=\\|~[]{}")); //\r\n"));
        ls.push_back(s);
        //cout << "<" << s << ">" << endl;
        disturber++;
        if (disturber>5)
            disturber = -5;
    }

    list<string> lsb64;
    start = GetULLTime();
    for (auto it: ls)
    {
        static char out[1024];
        unsigned long outlen = 1024;
        base64url_encode((const unsigned char*)it.data(), (unsigned long)it.size(), (char*)out, &outlen);
        lsb64.push_back(string(out, out+outlen));
    }
    cout << "Codificati\t" << N << " elementi in " << GetULLTime()-start << " us\t";
    cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";
    
    list<string> lsd;
    start = GetULLTime();
    for (auto it: lsb64)
    {
        static unsigned char out[1024];
        unsigned long outlen= 1024;
        base64url_decode((const char*)it.data(), (unsigned long)it.size(), out, &outlen);
        lsd.push_back(string(out, out+outlen));
    }
    cout << "Decodificati\t" << N << " elementi in " << GetULLTime()-start << " us\t";
    cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";

    cout << "Inizio confronti...\n";
    auto orig = ls.begin();
    auto b64  = lsb64.begin();
    auto deco = lsd.begin();
    while (orig != ls.end() && deco != lsd.end())
    {
        cout << "B64 : " << *b64++<< endl;
        cout << "ORIG: " << *orig << endl;
        cout << "DECO: " << *deco << endl;
        if ((*orig++).compare(*deco++)!=0)
        {
            cout << "Errore di confronto\n";
            exit(0);
        }
    }
    if (orig != ls.end() || deco != lsd.end())
        cout << "Errore di lunghezza\n";
    lsb64.clear();
    lsd.clear();
    cout << "Termine confronti...\n";
        

    start = GetULLTime();
    for (auto it: ls)
    {
        lsb64.push_back(Base64::encode(it));
    }
    cout << "1-Codificati\t" << N << " elementi in " << GetULLTime()-start << " us\t";
    cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";
    
    start = GetULLTime();
    for (auto it: lsb64)
    {
        lsd.push_back(Base64::decode(it));
    }
    cout << "1-Decodificati\t" << N << " elementi in " << GetULLTime()-start << " us\t";
    cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";
    
    cout << "Inizio confronti...\n";
    orig = ls.begin();
    b64  = lsb64.begin();
    deco = lsd.begin();
    while (orig != ls.end() && deco != lsd.end())
    {
        cout << "B64 : " << *b64++<< endl;
        cout << "ORIG: " << *orig << endl;
        cout << "DECO: " << *deco << endl;
        if ((*orig++).compare(*deco++)!=0)
        {
            cout << "Errore di confronto\n";
            exit(0);
        }
    }
    if (orig != ls.end() || deco != lsd.end())
        cout << "Errore di lunghezza\n";
    cout << "Termine confronti...\n";


    exit(0);
    
#endif
#if 0
    {
        list<int> li;
#define MAX 10
        for (int i=0; i<50000000; ++i)
            li.push_back(mtfa_rand_simple_ull()% MAX);

        typedef std::vector<int> VecInt;
        typedef unordered_set<int> SetInt;

        VecInt rank (MAX);
        VecInt parent (MAX);
        boost::disjoint_sets<int*,int*> ds(&rank[0], &parent[0]);
        SetInt elements;

        start = GetULLTime();
        for (int i=0; i<MAX; ++i) {
            ds.make_set(i);
            elements.insert(i);
        }

        DoTime("insert", "disjoint", MAX, start, GetULLTime());

        ds.union_set(0,9);
        ds.union_set(1,8);
        ds.union_set(2,7);

        printf("Elements: ");
        for (auto it: elements)
            cout << it << " ";
        cout << endl;
        
        printf("Number of sets:\n\t%d\n", (int)ds.count_sets(elements.begin(), elements.end()));

        for (SetInt::const_iterator i = elements.begin(); i != elements.end(); ++i) {
            printf("%d %d\n", *i, ds.find_set(*i));
        }
        
        cout << endl;

        // normalize set so that parent is always the smallest number
        ds.normalize_sets(elements.begin(), elements.end());
        for (SetInt::const_iterator i = elements.begin(); i != elements.end(); ++i) {
            printf("%d %d\n", *i, ds.find_set(*i));
        }
        exit(0);
    }
#endif
#if 0
    {
        typedef string Element;
        //typedef int Element;
        list<Element> elements;
        
        int N = 100000;
        int M = 1000;
        if (argc > 1)
            N = atoi(argv[1]);
        if (argc > 2)
            M = atoi(argv[2]);

        for (int i=0; i<N; ++i)
            elements.push_back(mtfa_rand_string(16, 32));

        typedef unordered_map<Element,int> rank_t;
        typedef unordered_map<Element,Element> parent_t;

        rank_t rank_map;
        parent_t parent_map;
        
        typedef boost::associative_property_map<rank_t>   T_rank_pmap;
        typedef boost::associative_property_map<parent_t> T_parent_pmap;

        T_rank_pmap rank_pmap(rank_map);
        T_parent_pmap parent_pmap(parent_map);
        
        boost::disjoint_sets<T_rank_pmap, T_parent_pmap> ds(rank_pmap, parent_pmap);

        start = GetULLTime();
        for (auto it: elements)
            ds.make_set(it);
        cout << "Inseriti " << N << " elementi in " << GetULLTime()-start << " us\n";
        cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";
        
        //Faccio N set di 1000000/N elementi ognuno
        string old="";
        int num=0;
        start = GetULLTime();
        for (auto it: elements)
        {
            if (old.size()==0)
            {
                old = it;
                continue;
            }
            if (num < (N/M))
            {
                ds.union_set(old, it);
                old = it;
                num++;
                continue;
            }
            else
            {
                //ds.union_set(old, it);
                old = "";
                num=0;
                continue;
            }
        }
        cout << "Uniti " << N << " elementi in " << GetULLTime()-start << " us\n";
        cout << 1000000.0*(double)N/(double)(GetULLTime()-start) << " op/sec\n";

//     //elements.push_back(1);
//     elements.push_back(10);
//     elements.push_back(11);
//     elements.push_back(12);
//     for (auto it: elements)
//         ds.make_set(it);
// 
//     ds.union_set(100, 110);
//     ds.union_set(100, 111);
//     ds.union_set(101, 112);
//     ds.union_set(102, 113);
//     //ds.union_set(2, 11);
//     ds.union_set(102, 114);
//         cout << "Count: " << ds.count_sets(elements.begin(), elements.end()) << endl;
//         for (auto el: parent_map)
//         {
//             cout << "it: " << el.first << ". Parent: " << el.second << ". Rank: " << rank_map[el.first] << endl;
//         }
// 
//         cout << endl;
//         ds.compress_sets(elements.begin(), elements.end());
//         
//         for (auto el: parent_map)
//         {
//             cout << "it: " << el.first << ". Parent: " << el.second << ". Rank: " << rank_map[el.first] << endl;
//         }
//         
//         //ora noi gestiamo sottoinsiemi disgiunti di un insieme
//         //e su questi possiamo interrogare sull'appartenenza a un insieme dato
//         //cioè noi diciamo che, dati N elementi a1..an, questi sono raggruppati in sottoinsiemi
//         //12345, 6789, 10-11-12-13, ... e ogni sottoinsieme ha un suo rappresentante
//         //come posso trasformare questo modello in unione e intersezione? 
//         //esempio: ho un f1 che contiene p1, p2 e p3. f2 che contiene p2, p3 e p4.
//         //Quanti fx contengono p2?
        
    }
    exit(0);
#endif


#if 0
    //cout << mtfa_magic_mime ( argv[1] );

    int size = strtol(argv[1], nullptr, 10);
    int num = strtol(argv[2], nullptr, 10);
    cout << "Size is: " << size << endl;
    list<int> li1;
    for (int i=0; i<num; ++i)
        li1.push_back(mtfa_rand_simple_ull()%size);
    list<int> li2;
    for (int i=0; i<num; ++i)
        li2.push_back(mtfa_rand_simple_ull()%size);

    set<int> si1;
 	set<int> si2;
    SetUnion su1(size);
    SetUnion su2(size);

//     start = GetULLTime();
//     for (auto v: li1)
//     {
//         si1.insert(v);
//     }
//     cout << "SI1. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
//     
//     start = GetULLTime();
//     for (auto v: li2)
//     {
//         si2.insert(v);
//     }
//     cout << "SI2. Inserted " << num << " items in us: " << GetULLTime()-start << endl;

    start = GetULLTime();
    for (auto v: li1)
    {
        su1.Insert(v);
    }
    cout << "SU1. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
 
    start = GetULLTime();
    for (auto v: li2)
    {
        su2.Insert(v);
    }
    cout << "SU2. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
    
    su1.Print();
    exit(0);
    
//     start = GetULLTime();
//     mtfa_bitset * bs1 = new mtfa_bitset(size);
//     bs1->set(size, true);
//     for (auto v: li1)
//     {
//         bs1->set(v, true);
//     }
//     cout << "BS1. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
//     
//     start = GetULLTime();
//     mtfa_bitset * bs2 = new mtfa_bitset(size);
//     bs2->set(size, true);
//     for (auto v: li1)
//     {
//         bs2->set(v, true);
//     }
//     cout << "BS2. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
//     
//     start = GetULLTime();
//     mtfa_extended_bitset * ebs1 = new mtfa_extended_bitset();
//     ebs1->set(size, true);
//     for (auto v: li1)
//     {
//         ebs1->set(v, true);
//     }
//     cout << "EBS1. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
//     
//     start = GetULLTime();
//     mtfa_extended_bitset * ebs2 = new mtfa_extended_bitset();
//     ebs2->set(size, true);
//     for (auto v: li1)
//     {
//         ebs2->set(v, true);
//     }
//     cout << "EBS2. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
    
    SetUnion64 su64_1(size);
    start = GetULLTime();
    for (auto v: li1)
    {
        su64_1.Insert(v);
    }
    cout << "Su64_1. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
    
    SetUnion64 su64_2(size);
    start = GetULLTime();
    for (auto v: li2)
    {
        su64_2.Insert(v);
    }
    cout << "Su64_2. Inserted " << num << " items in us: " << GetULLTime()-start << endl;
    
//     //Ora faccio unione con i vettori e con i set
//     start = GetULLTime();
//     vector<int32_t> sires;
//     set_union(si1.begin(), si1.end(), si2.begin(), si2.end(), back_inserter(sires));
//     set<int32_t> sires1(sires.begin(), sires.end());
//     cout << "Si Union 1: " << sires1.size() << " items in us: " << GetULLTime()-start << endl;
// 
//     //ora provo con i vettori
//     SetUnion sures(size);
//     start = GetULLTime();
//     for (int i=0; i<size; ++i)
//     {
//         if (su2.ranges[i])
//             sures.Insert(i);
//         else if (su1.ranges[i])
//             sures.Insert(i);
//     }
//     cout << "SU Union: " << num << " items in us: " << GetULLTime()-start << endl;
//     
// 
//     //ora la stessa cosa ma con i gruppi!
//     list<pair<int, int>> gi1;
//     int last = -1;
//     int width = 0;
//     for (int i=0; i<size; ++i)
//     {
//         if (su1.ranges[i])
//         {
//             if (last == -1)
//             {
//                 last = i;
//                 width = 1;
//             }
//             else
//             {
//                 width++;
//             }
//         }
//         else
//         {
//             if (last != -1)
//             {
//                 gi1.push_back(make_pair(last, width));
//                 last = -1;
//                 width = 0;
//             }
//         }
//     }
//     if (last != -1)
//     {
//         gi1.push_back(make_pair(last, width));
//     }
// 
//     list<pair<int, int>> gi2;
//     last = -1;
//     width = 0;
//     for (int i=0; i<size; ++i)
//     {
//         if (su2.ranges[i])
//         {
//             if (last == -1)
//             {
//                 last = i;
//                 width = 1;
//             }
//             else
//             {
//                 width++;
//             }
//         }
//         else
//         {
//             if (last != -1)
//             {
//                 gi2.push_back(make_pair(last, width));
//                 last = -1;
//                 width = 0;
//             }
//         }
//     }
//     if (last != -1)
//     {
//         gi2.push_back(make_pair(last, width));
//     }
// 
//     SetUnion sures1(size);
//     start = GetULLTime();
//     for (auto it: gi1)
//     {
//         for (int i=0; i<it.second; ++i)
//         {
//             sures1.Insert(it.first+i);
//         }
//     }
//     for (auto it: gi2)
//     {
//         for (int i=0; i<it.second; ++i)
//         {
//             sures1.Insert(it.first+i);
//         }
//     }
//     cout << "SU2 Union: " << num << " items in us: " << GetULLTime()-start << endl;
// 
//     start = GetULLTime();
//     mtfa_bitset * bsres = new mtfa_bitset(size);
//     bsres->set(size, true);
//     *bsres = *bs1 | *bs2;
//     cout << "BS Union: " << bsres->count() << " items in us: " << GetULLTime()-start << endl;
// 
//     start = GetULLTime();
//     mtfa_extended_bitset * ebsres = new mtfa_extended_bitset();
//     ebsres->set(size, true);
//     *ebsres = *ebs1 | *ebs2;
//     cout << "EBS Union: " << ebsres->count() << " items in us: " << GetULLTime()-start << endl;
// 
    
    
    //     sures.Print(); cout << endl;
//     sures1.Print(); cout << endl;
    
//     //ed ora per prova
//     for (auto it: gi1)
//         cout << "(" << it.first << ", " << it.second << "), ";
//     cout << endl;
//     su1.Print(); cout << endl;
//     for (auto it: gi2)
//         cout << "(" << it.first << ", " << it.second << "), ";
//     cout << endl;
//     su2.Print(); cout << endl;
    
	exit(0);
#endif

#if 0
	boost::dynamic_bitset<> Va;
	Va.resize(16, 0);
	Va[0]=true;
	Va[2]=true;
	Va[4]=true;
	Va[6]=true;
	cout << Va.count() << endl;

	exit(0);
#endif
#if 0
	
// 	string sin, sout, desout;
// 	sin = mtfa_rand_string(1000000, 16);
// 	
// 	mtfa_compress(sin, sout, 6);
// 	
// 	mtfa_uncompress(sout, desout);
// 	
// 	cout << desout.compare(sin) << endl;
// 	exit(0);
	
	
	mtfa_extended_bitset* pt = new mtfa_extended_bitset();
	pt->set(11234, true);
	pt->set(1234, true);
	pt->set(111234, true);
	pt->set(21234, true);
	pt->set(211234, true);
	pt->set(221234, true);

	cout << "vecchio: ";
	auto all = pt->GiveOnes();
	for (auto it: all)
		cout << it << " ";
	cout << endl;
	
	mtfa_binary_stream_save* bs = new mtfa_binary_stream_save ( "" );
	pt->Serialize ( bs );
	std::string ser = bs->GetStream();
	delete bs;
	
 	string cmpser;
 	bool bret = mtfa_compress(ser, cmpser, 6);
 	
 	string decomp;
 	bret = mtfa_uncompress(cmpser, decomp);
	
	decomp.assign(ser);
 	
 	cout << "dopo la uncompress: " << ser.compare(decomp) << endl;
	
	pt->clear();
	pt->set(100, true);
	
	all = pt->GiveOnes();
	for (auto it: all)
		cout << it << " ";
	cout << endl;
	
	cout << "carivca l vecchio\n";
	mtfa_binary_stream_load* bsl = new mtfa_binary_stream_load ( "" );
	bsl->mtfa_binary_stream_set ( decomp );
	pt->Deserialize ( bsl );
	delete bsl;
	
	all = pt->GiveOnes();
	for (auto it: all)
		cout << it << " ";
	cout << endl;
	
	
	exit(0);
	
#endif

#if 0
// 	auto rng = mtfa_fastrng64(1);
// 	//cout << rng.next() << ", " << rng.next() << endl;
// 	
// 	auto rng1 = mtfa_fastrng1024();
// 	cout << rng1.next() << ", " << rng1.next() << endl;

	//exit(0);
	vector<char> vc(atoi(argv[1]));
    for (int i=0; i<vc.size(); ++i) {
        vc[i] = i & 0x00FF; //mtfa_rand_simple_ull() % 0x00FF;
    }
    start = GetULLTime();
	BRG brg("uno", vc);
    DoTime("Build brg", "bits: ", atoi(argv[1]), start, GetULLTime());

    auto fullstart = GetULLTime();
	for (int i=0; i<atoi(argv[2]); ++i) {
        start = GetULLTime();
        brg.Next();
        DoTime("Next brg", "bits: ", atoi(argv[1]), start, GetULLTime());
        auto v = brg.GetBufAsVByte();
        cout << "Block size: " << v.size() << endl;
//         int howmany = 16;
//         for (auto it: v)
//         {
//             printf("%u ", it);
//             howmany--;
//             if (howmany <= 0)
//                 break;
//        }
//        cout << endl;
    }
    DoTime("Full end", "bits: ", atoi(argv[1]), fullstart, GetULLTime());
	exit ( 0 );
#endif

#if 0

    list<pair<unsigned long long, unsigned long long>> uul;
    for ( int i=0; i<atoi ( argv[1] ); ++i ) {
        uul.push_back ( make_pair ( mtfa_rand_simple_ull(), mtfa_rand_simple_ull() ) );
    }

    {

        unsigned short r, c, r1, c1;
        unsigned int mn;
        start = GetULLTime();
        for ( auto & it: uul ) {
            r = it.first % ( 1 << 16 );
            c = it.second % ( 1 << 16 );
            mn = MortonEncode ( r, c );
            MortonDecode ( mn, r1, c1 );

            if ( r1 != r || c1 != c )
                cout << "R: " << r << ". C: " << c << ". MN: " << mn << ". R': " << r1 << ". C': " << c1 << endl;
            }
            DoTime("Encode/decode 16   ", "", uul.size(), start, GetULLTime());
    }

        {
        uint_fast16_t r, c, r1, c1;
        start = GetULLTime();
        for ( auto it: uul ) {
            r = it.first % ( 1 << 16 );
            c = it.second % ( 1 << 16 );
            const uint_fast32_t mn = libmorton::morton2D_32_encode ( r,c );
            libmorton::morton2D_32_decode ( mn, r1, c1 );

            if ( r1 != r || c1 != c )
                cout << "R: " << r << ". C: " << c << ". MN: " << mn << ". R': " << r1 << ". C': " << c1 << endl;
            }
            DoTime("Encode/decode 16 2D", "", uul.size(), start, GetULLTime());
        }

        {
        uint_fast32_t r, c, r1, c1;
        start = GetULLTime();
        for ( auto it: uul ) {
            r = it.first & 0x00000000FFFFFFFF;
            c = it.second & 0x00000000FFFFFFFF;
            const uint_fast64_t mn = libmorton::morton2D_64_encode ( r,c );
            libmorton::morton2D_64_decode ( mn, r1, c1 );

            r1 = r1  & 0x00000000FFFFFFFF;
            c1 = c1  & 0x00000000FFFFFFFF;

            if ( r1 != r || c1 != c )
                cout << "R: " << r << ". C: " << c << ". MN: " << mn << ". R': " << r1 << ". C': " << c1 << endl;
            }
            DoTime("Encode/decode 32 2D", "", uul.size(), start, GetULLTime());
        }

    return 0;
#endif

#if 0
    auto res = mtfa_get_all_submatching_positions ( argv[1], argv[2] );
    cout << "First\n";
    for ( auto it: res ) {
        cout << "From: " << it.first << ", " << it.second << endl;
        }

    cout << "Second\n";
    auto r1 = mtfa_get_all_submatching_patterns ( argv[1], argv[2] );
    for ( auto it: r1 ) {
        cout << "From: " << it << endl;
        }
    exit ( 0 );
#endif

#if 0
    unsigned long long finalSize = 4 * 8192; //65536; //4*1048576;  //8192 268435456 67108864

    if ( argc >= 2 )
        finalSize = atoi ( argv[1] );

    //Per prendere altri bit dai blum originali
    unsigned int mask = 1;

    if ( argc > 2 )
        mask = atoi ( argv[2] );

    int numThreads = 4;

    if ( argc > 3 )
        numThreads = atoi ( argv[3] );

    int cicli=0;
	
//#define BRGGEN 1
#ifdef BRGGEN
    if ( argc > 4 )
        cicli = atoi ( argv[4] );

    list<tuple<thread*, string*, unsigned long long*>> lt;

    for ( int i = 0; i < numThreads; ++i ) {
        //Ricorda sempre che hai una costante che si chiama HALF_KEY_SIZE e devi fare in modo che HALF_KEY_SIZE*i < 65536
        string* sret = new string();
        unsigned long long* ull = new unsigned long long();
		vector<vector<unsigned long long>> aba2b2;
        thread* th = new thread ( MyThread, finalSize / numThreads, mask, HALF_KEY_SIZE * i, sret, ull, cicli, aba2b2 );
        lt.push_back ( make_tuple ( th, sret, ull ) );
	}

    //Faccio le join
    while ( true ) {
        int num = 0;

        for ( auto& it : lt ) {
            if ( get<0> ( it )->joinable() )
                get<0> ( it )->join();
            else
                num++;
            }

        if ( num == numThreads )
            break;
        }

//     bool ok=false;
//     while (!ok)
//     {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         ok = true;
//         for (auto & it: lt)
//             if (it.second->size() == 0)
//             {
//                 ok=false;
//                 break;
//             }
//     }

    unsigned long long tempo_impiegato = 0;

    for ( auto& it : lt ) {
        tempo_impiegato += *get<2> ( it );
        delete get<2> ( it );
        delete get<0> ( it );
        }

    cout << "Tempo impiegato in totale per generare " << finalSize / 4 << " bytes: " << tempo_impiegato / 1000000 / numThreads << " secs" << endl;

    //Preparo un file di uscita
    FILE* fou;
    string mod = to_string ( time ( nullptr ) );
    mod = "out." + mod;

    cout << "Writing output file\n";
    fou = fopen ( mod.c_str(), "w" );

    for ( auto& it : lt ) {
        ScriviString ( fileno ( fou ), *get<1> ( it ) );
        delete get<1> ( it );
        }

    fclose ( fou );
    cout << "Operazione terminata\n";
#else
	cout << "ora cifro" << endl;
	{
		//Chiave da 1024 bit * 4 (a, b, a2, b2) che faccio diventare da 8192 e prendo gli ultimi 1024*4 bit con i quali cifro
		//512 bytes
		KGenerator kg;
		string result;
		unsigned long long timeSpent;
		
		string testo1 = "                          Il visconte dimezzato\n"
		"C'era una guerra contro i turchi."
		"Il visconte Medardo di Terralba, mio zio, cavalcava per la pianura di Boemia diretto all'accampamento dei cristiani. Lo seguiva uno scudiero a nome Curzio."
		"Le cicogne volavano basse, in bianchi stormi traversando l'aria "
		"opaca e ferma."
		"- Perche' tante cicogne? - chiese Medardo a Curzio, - dove volano? "
		"Mio zio era nuovo arrivato, essendosi arruolato appena allora, per compiacere certi duchi nostri vicini impegnati in quella guerra.";
		
		cout << testo1.size() << endl;
		
		
		string testo2 = R"END(                Capitolo II
		Tirato via il lenzuolo, il corpo del visconte apparve orrendamente mutilato. ... tutto quel che c'era di torace e d'addome tra quel braccio e quella gamba era stato portato via, polverizzato da quella cannonata presa in pieno.
		Del capo restavano un occhio, un orecchio, una guancia, mezzo naso, mezza bocca, mezzo mento e mezza fronte: dell'altra metà del capo c'era più solo una pappetta. A farla breve, se n’era salvato solo metà, la parte destra, perfettamente conservata.)END";
		
		cout << testo2.size() << endl;
		
#if 0
		//per mostrare come la ricostrzione funzioni
		char pippo[512];
		for (int i=0; i<128; ++i) {
			pippo[i*4]=(char)aba2b2[0][i];
		}
		for (int i=0; i<128; ++i) {
			pippo[i*4+1]=(char)aba2b2[1][i];
		}
		for (int i=0; i<128; ++i) {
			pippo[i*4+2]=(char)aba2b2[2][i];
		}
		for (int i=0; i<128; ++i) {
			pippo[i*4+3]=(char)aba2b2[3][i];
		}

		cout << string(pippo, &pippo[512]) << endl;
		exit(0);
#endif

		/*in result sono in sequenza a, b, a2, b2 e poi x y x2 e y2
		quindi mi conviene operare nel seguente modo: se devo cifrare 512 bytes, allora devo riuscire a ottenere
		512 byte per cifrare e 1024bit*4 = 4096/8=512 bytes per inizializzare la prossima cifra
		dato che io genero ogni volta fino a 16384 bits, pari a 2048 bytes, allora dei 2048 cifro con i primi 1024 e genero prossima chiave con i secondi 1024
			Ricapitolo: 
				- le mie chiavi sono da 1024 bit*4 =  4096 parte shared e 1024 bit*4 = 4096 parte hidden
				  pari a 512 bytes parte shared e 512 bytes parte hidden
				- se io genero 2048 bytes = 2048*8 = 16384 bit come risultato, allora
				  potrò utilizzare 4096 bit come nuova parte shared e 16384-4096 = 12288/8 = 1536 byte per cifrare
		*/
		
		//la prima volta, xyx2y2 la prendo direttamente nel sistema mentre aba2b2 è la stringa da cifrare
		
		/*Allora, devo cifrare 512bytes=4096 bits. 
		 * 1) fill a, b, a2 e b2 con questi 4 da 1024 bit (la nostra chiave deve essere 1024 bit*4)
		 *    e fill x, y, x2 e y2 con i dati della matriciona (ma potrei metterci qualunque altra cosa)
		 * 2) chiedo di generare 8192 bit => 2 da 4096.
		 * 	  i primi   4096 li uso come chiave di cifra per i 512 byte
		 *    i secondi 4096 li uso come prossima chiave shared
		 * 
		 * Inoltre, avendo anche le x, y, x1 e y2, 
		 * 	  faccio lo xor dei primi 4096 con i secondi 4096 della chiave hidden e uso i 54096 risultanti come seme
			  per la prossima hidden...
		*/

		vector<vector<unsigned char>> aba2b2;
		vector<vector<unsigned char>> xyx2y2;
		kg.mfa_generator ( 16384, 1<<24, 0, &result, &timeSpent, 0, aba2b2, xyx2y2 );
		
		
		cout << "Result size: " << result.size() << endl;

		string primi4096aba2b2   = result.substr(0, 512);
		string secondi4096a1a2b2 = result.substr(512, 512);
		string primi4096xyx2y2   = result.substr(1024, 512);
		string secondi4096xyx2y2 = result.substr(1536, 512);

		//Faccio lo xor con la stringa test1
		string testo1_cifrato = testo1;
		//per test, prendo il dato di uscita e decifra
		FILE * fin = fopen("testo1cifrato.dat", "r");
		char data1[512];
		read(fileno(fin), data1, 512);
		fclose(fin);
		for (int i=0; i<512; ++i)
		{
			testo1_cifrato[i] ^= primi4096aba2b2[i];
			data1[i] ^= primi4096aba2b2[i];
		}
		
		string xorxy = primi4096xyx2y2;
		for (int i=0; i<512; ++i)
		{
			xorxy[i] ^= secondi4096xyx2y2[i];
		}
		
		//Stampo in esadecimale il primo testo
		cout << "Testo 1 cifrato: " << endl;
		for (int i=0; i<512; ++i)
		{
			printf("%02X", (unsigned char)testo1_cifrato[i]);
		}
		printf("\n");

		aba2b2 = vector<vector<unsigned char>>(4, vector<unsigned char>(128));
		for (int i=0; i<128; ++i) {
			aba2b2[0][i]=secondi4096a1a2b2[i*4];
		}
		for (int i=0; i<128; ++i) {
			aba2b2[1][i]=secondi4096a1a2b2[i*4+1];
		}
		for (int i=0; i<128; ++i) {
			aba2b2[2][i]=secondi4096a1a2b2[i*4+2];
		}
		for (int i=0; i<128; ++i) {
			aba2b2[3][i]=secondi4096a1a2b2[i*4+3];
		}

		xyx2y2 = vector<vector<unsigned char>>(4, vector<unsigned char>(128));
		for (int i=0; i<128; ++i) {
			xyx2y2[0][i]=xorxy[i*4];
		}
		for (int i=0; i<128; ++i) {
			xyx2y2[1][i]=xorxy[i*4+1];
		}
		for (int i=0; i<128; ++i) {
			xyx2y2[2][i]=xorxy[i*4+2];
		}
		for (int i=0; i<128; ++i) {
			xyx2y2[3][i]=xorxy[i*4+3];
		}

		result.clear();
		kg.mfa_generator ( 8192, 1<<24, 0, &result, &timeSpent, 0, aba2b2, xyx2y2 );
		
		primi4096aba2b2   = result.substr(0, 512);
		secondi4096a1a2b2 = result.substr(512, 512);
		primi4096xyx2y2   = result.substr(1024, 512);
		secondi4096xyx2y2 = result.substr(1536, 512);
		
		//Faccio lo xor con la stringa test1
		string testo2_cifrato = testo2;
		fin = fopen("testo2cifrato.dat", "r");
		char data2[512];
		read(fileno(fin), data2, 512);
		fclose(fin);
		
		for (int i=0; i<512; ++i)
		{
			testo2_cifrato[i] ^= primi4096aba2b2[i];
			data2[i] ^= primi4096aba2b2[i];
		}
		
		cout << "Data1: " << string(data1, data1+512) << endl;
		cout << "Data2: " << string(data2, data2+512) << endl;
		
		//Stampo in esadecimale il secondo testo
		for (int i=0; i<512; ++i)
		{
			printf("%02X", (unsigned char)testo2_cifrato[i]);
		}
		printf("\n");
		
// 		FILE * fou = fopen("testo1cifrato.dat", "w");
// 		cout << "Scritti su file: " << write(fileno(fou), testo1_cifrato.data(), 512) << endl;
// 		fclose(fou);
// 		fou = fopen("testo2cifrato.dat", "w");
// 		cout << "Scritti su file: " << write(fileno(fou), testo2_cifrato.data(), 512) << endl;
// 		fclose(fou);
	}
#endif
    exit ( 0 );
#endif
#if 0
    unsigned long long finalSize = 4 * 1048576; //65536; //4*1048576;  //8192 268435456 67108864

    if ( argc >= 2 )
        finalSize = atoi ( argv[1] );

    //Per prendere altri bit dai blum originali
    unsigned int mask = 1;

    if ( argc > 2 )
        mask = atoi ( argv[2] );

    bitset<HALF_KEY_SIZE> va;
    bitset<HALF_KEY_SIZE> vb;
    bitset<HALF_KEY_SIZE> vx;
    bitset<HALF_KEY_SIZE> vy;
    bitset<HALF_KEY_SIZE> va_2;
    bitset<HALF_KEY_SIZE> vb_2;
    bitset<HALF_KEY_SIZE> vx_2;
    bitset<HALF_KEY_SIZE> vy_2;

    mtfa_random_generator_init ( 1 );

    for ( int i = 0; i < HALF_KEY_SIZE; ++i ) {
        va[i] = ( mtfa_mat_cypher_base_ext[0][i] & mask ) ? true : false; //==1;
        vb[i] = ( mtfa_mat_cypher_base_ext[1][i] & mask ) ? true : false; //==1;
        vx[i] = ( mtfa_mat_cypher_base_ext[2][i] & mask ) ? true : false; //==1;
        vy[i] = ( mtfa_mat_cypher_base_ext[3][i] & mask ) ? true : false; //==1;
        va_2[i] = ( mtfa_mat_cypher_base_ext[4][i] & mask ) ? true : false; //==1;
        vb_2[i] = ( mtfa_mat_cypher_base_ext[5][i] & mask ) ? true : false; //==1;
        vx_2[i] = ( mtfa_mat_cypher_base_ext[6][i] & mask ) ? true : false; //==1;
        vy_2[i] = ( mtfa_mat_cypher_base_ext[7][i] & mask ) ? true : false; //==1;

//         va[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vb[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vx[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vy[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         va_2[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vb_2[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vx_2[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;
//         vy_2[i] = ( mtfa_rand_simple_ull() &mask ) ? true : false;

        }

    unsigned long long current_size; // = HALF_KEY_SIZE;
    finalSize /= 2;
    current_size = finalSize; // + finalSize/2; //*3/4;  //rimane divisibile per 64 se la dimensione è almeno 256
    cout << "Key size: " << HALF_KEY_SIZE << ". Current size: " << current_size << ". Final size: " << finalSize << endl;

    /*
     *   Ora l'incrocio è anche a livello di chiavi, per il calcolo degli indirizzi
     *   a  b   x  y
     *
     *   a2 b2  x2 y2
     *   indirizzo di   ab da x2y2
     *   indirizzo di   xy da a2b2
     *   indirizzo di a2b2 da xy
     *   indirizzo di x2y2 da ab
     */

    //Con la set union
    BSetUnion nva ( current_size );
    BSetUnion nvb ( current_size );
    BSetUnion nvx ( current_size );
    BSetUnion nvy ( current_size );

    BSetUnion nva_2 ( current_size );
    BSetUnion nvb_2 ( current_size );
    BSetUnion nvx_2 ( current_size );
    BSetUnion nvy_2 ( current_size );

    MakeSpace ( va, vx, HALF_KEY_SIZE, current_size, nva );
    MakeSpace ( vb, vy, HALF_KEY_SIZE, current_size, nvb );
    MakeSpace ( vx, va, HALF_KEY_SIZE, current_size, nvx );
    MakeSpace ( vy, vb, HALF_KEY_SIZE, current_size, nvy );

    MakeSpace ( va_2, vx_2, HALF_KEY_SIZE, current_size, nva_2 );
    MakeSpace ( vb_2, vy_2, HALF_KEY_SIZE, current_size, nvb_2 );
    MakeSpace ( vx_2, va_2, HALF_KEY_SIZE, current_size, nvx_2 );
    MakeSpace ( vy_2, vb_2, HALF_KEY_SIZE, current_size, nvy_2 );

    //Riempio gli spazi vuoti
    int pos_a = 0;
    int pos_b = 0;
    int pos_x = 0;
    int pos_y = 0;

    int pos_a_2 = 0;
    int pos_b_2 = 0;
    int pos_x_2 = 0;
    int pos_y_2 = 0;

    unsigned long long limit = finalSize - HALF_KEY_SIZE; //finalSize-HALF_KEY_SIZE;

    for ( int fatti = 0; fatti < limit; ++fatti ) { //qui posso gestire la creazione di 1/2 in più per volta
        //Calcolo indirizzi di a b e di x y
        auto addresses_ab   = GetNewAddressesAB ( nvx_2, nvy_2, pos_x_2, pos_y_2, current_size, Log2 ( current_size ) );
        auto addresses_xy   = GetNewAddressesAB ( nva_2, nvb_2, pos_a_2, pos_b_2, current_size, Log2 ( current_size ) );
        auto addresses_ab_2 = GetNewAddressesAB ( nvx, nvy, pos_x, pos_y, current_size, Log2 ( current_size ) );
        auto addresses_xy_2 = GetNewAddressesAB ( nva, nvb, pos_a, pos_b, current_size, Log2 ( current_size ) );

        if ( ( fatti % 100000 ) == 0 )
            cout << fatti + HALF_KEY_SIZE << endl;

        //Calcolo i nuovi a, b, x e y e ruoto i precedenti
        //Per farlo devo cercare i primi occupati a partire dalla posizione corrente, che appartengano alla chiave attuale
        unsigned int fa  = nva.getAddressOfNextBit ( pos_a, true, true );
        unsigned int fb  = nvb.getAddressOfNextBit ( pos_b, true, true );
        unsigned int fx  = nvx.getAddressOfNextBit ( pos_x, true, true );
        unsigned int fy  = nvy.getAddressOfNextBit ( pos_y, true, true );
        unsigned int fa1 = nva.getAddressOfNextBit ( fa + 1, true, true );
        unsigned int fb1 = nvb.getAddressOfNextBit ( fb + 1, true, true );
        unsigned int fx1 = nvx.getAddressOfNextBit ( fx + 1, true, true );
        unsigned int fy1 = nvy.getAddressOfNextBit ( fy + 1, true, true );

        unsigned int fa_2  = nva_2.getAddressOfNextBit ( pos_a_2, true, true );
        unsigned int fb_2  = nvb_2.getAddressOfNextBit ( pos_b_2, true, true );
        unsigned int fx_2  = nvx_2.getAddressOfNextBit ( pos_x_2, true, true );
        unsigned int fy_2  = nvy_2.getAddressOfNextBit ( pos_y_2, true, true );
        unsigned int fa1_2 = nva_2.getAddressOfNextBit ( fa_2 + 1, true, true );
        unsigned int fb1_2 = nvb_2.getAddressOfNextBit ( fb_2 + 1, true, true );
        unsigned int fx1_2 = nvx_2.getAddressOfNextBit ( fx_2 + 1, true, true );
        unsigned int fy1_2 = nvy_2.getAddressOfNextBit ( fy_2 + 1, true, true );


        //Ora devo calcolare se, in caso di collisione, devo andare avanti o indietro. Questo lo faccio utilizzando tutti e 4 i valori
        bool avanti_a = nvy.getBitValue ( fy ) == nvy.getBitValue ( fy1 );
        bool avanti_b = nvx.getBitValue ( fx ) == nvx.getBitValue ( fx1 );
        bool avanti_x = nvb.getBitValue ( fb ) == nvb.getBitValue ( fb1 );
        bool avanti_y = nva.getBitValue ( fa ) == nva.getBitValue ( fa1 );

        bool avanti_a_2 = nvy_2.getBitValue ( fy_2 ) == nvy_2.getBitValue ( fy1_2 );
        bool avanti_b_2 = nvx_2.getBitValue ( fx_2 ) == nvx_2.getBitValue ( fx1_2 );
        bool avanti_x_2 = nvb_2.getBitValue ( fb_2 ) == nvb_2.getBitValue ( fb1_2 );
        bool avanti_y_2 = nva_2.getBitValue ( fa_2 ) == nva_2.getBitValue ( fa1_2 );

        //Calcolo i nuovi valori della quaterna
        auto nuovi_abxy   = EvalABXY ( nva, nvb, nvx, nvy, fa, fb, fx, fy, fa1, fb1, fx1, fy1 );
        auto nuovi_abxy_2 = EvalABXY ( nva_2, nvb_2, nvx_2, nvy_2, fa_2, fb_2, fx_2, fy_2, fa1_2, fb1_2, fx1_2, fy1_2 );

        //cout << get<0>(nuovi_abxy) << " " << get<1>(nuovi_abxy) << " " << get<2>(nuovi_abxy) << " " << get<3>(nuovi_abxy) << endl;

        //in modo deterministico, avanti e indietro cambiano semantica ogni operazione: una volta true è avanti, la volta successiva true è indietro...
        nva.InsertValue ( addresses_ab.first, get<0> ( nuovi_abxy ), avanti_a, false );
        nvb.InsertValue ( addresses_ab.second, get<1> ( nuovi_abxy ), avanti_b, false );
        nvx.InsertValue ( addresses_xy.first, get<2> ( nuovi_abxy ), avanti_x, false );
        nvy.InsertValue ( addresses_xy.second, get<3> ( nuovi_abxy ), avanti_y, false );

        nva_2.InsertValue ( addresses_ab_2.first, get<0> ( nuovi_abxy_2 ), avanti_a_2, false );
        nvb_2.InsertValue ( addresses_ab_2.second, get<1> ( nuovi_abxy_2 ), avanti_b_2, false );
        nvx_2.InsertValue ( addresses_xy_2.first, get<2> ( nuovi_abxy_2 ), avanti_x_2, false );
        nvy_2.InsertValue ( addresses_xy_2.second, get<3> ( nuovi_abxy_2 ), avanti_y_2, false );

        pos_a = fa1;
        pos_b = fb1;
        pos_x = fx1;
        pos_y = fy1;

        pos_a_2 = fa1_2;
        pos_b_2 = fb1_2;
        pos_x_2 = fx1_2;
        pos_y_2 = fy1_2;
        }

    cout << "Preparing output file\n";
//    exit(0);

    //Preparo un file di uscita
    FILE* fou;
    string mod = to_string ( time ( nullptr ) );
    mod = "out." + mod;

    Pushing pa, pb, pa_2, pb_2;
    PreparePa ( nva, pa );
    PreparePa ( nvb, pb );
    PreparePa ( nva_2, pa_2 );
    PreparePa ( nvb_2, pb_2 );

    cout << "Writing output file\n";
    fou = fopen ( mod.c_str(), "w" );
    ScriviPa ( fileno ( fou ), pa );
    ScriviPa ( fileno ( fou ), pb );
    ScriviPa ( fileno ( fou ), pa_2 );
    ScriviPa ( fileno ( fou ), pb_2 );
    fclose ( fou );

    exit ( 0 );

#endif

#if 0
    string sc = mtfa_compress ( "Ciao", 6 );
    string uc = mtfa_uncompress ( sc );
    cout << uc << endl;
    exit ( 0 );
#endif
#if 0

    for ( int i = 0; i < 100; ++i ) {
        list<string> ls;

        for ( int i = 0; i < 10000; ++i )
            ls.push_back ( mtfa_rand_string ( 16, 32 ) );

        PerfectHash* ds = new PerfectHash();

        for ( auto it : ls )
            ds->Insert ( it.c_str(), it.size() );

        mtfa_binary_stream_save* bs = new mtfa_binary_stream_save ( "" );
        ds->Serialize ( bs );

        auto ser = bs->GetStream();
        delete bs;
        delete ds;

        PerfectHash* ds1 = new PerfectHash();
        mtfa_binary_stream_load* bs1 = new mtfa_binary_stream_load ( "" );
        bs1->mtfa_binary_stream_set ( ser );
        ds1->Deserialize ( bs1 );
        delete bs1;

        for ( auto it : ls ) {
            auto res = ds1->Find ( it.c_str(), it.size() );

            if ( res < 0 )
                cout << it << ": " << "not found" << endl;
            }

        delete ds1;
        }

    exit ( 0 );
#endif


    return 0;
    }
