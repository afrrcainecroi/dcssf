#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <list>
#include <iostream>
#include <cstdarg>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <libguile.h>
#include <zlib.h>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//#include "ULL_Rng.h"

#include "mtfa_utils.h"
#include "mtfa_find.h"
#include "uuid.h"

using namespace std;


//Tutte le le funzioni di libreria in c++, le altre sono in mtfa_lib.scm
#ifdef _WIN32

long long Ticks::get()
{
    long long freq;
    long long counter;

    if ( QueryPerformanceFrequency ( ( LARGE_INTEGER* ) & freq ) != 0 )
    {
        if ( QueryPerformanceCounter ( ( LARGE_INTEGER* ) & counter ) != 0 )
        {
            return ( ( counter * 1000 ) / freq );
        }
        else
        {
            goto nonAbbiamo;
        }
    }

nonAbbiamo:
    struct __timeb64 timebuffer;
    _ftime ( &timebuffer );
    return ( timebuffer.millitm + timebuffer.time * 1000 );
}

long long Ticks::get_micro()
{
    long long freq;
    long long counter;

    if ( QueryPerformanceFrequency ( ( LARGE_INTEGER* ) & freq ) != 0 )
    {
        if ( QueryPerformanceCounter ( ( LARGE_INTEGER* ) & counter ) != 0 )
        {
            return ( ( counter * 1000000 ) / freq );
        }
        else
        {
            goto nonAbbiamo;
        }
    }

nonAbbiamo:
    struct __timeb64 timebuffer;
    _ftime ( &timebuffer );
    return ( timebuffer.millitm * 1000 + timebuffer.time * 1000000 );
}
#else

long long Ticks::get()
{
    struct timeval tv;
    gettimeofday ( &tv, NULL );
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

long long Ticks::get_micro()
{
    struct timeval tv;
    gettimeofday ( &tv, NULL );
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
#endif

unsigned long long GetULLTime()
{
    return Ticks::get_micro();
}

string GetStrTime ( )
{
    return GetStrTime ( time ( nullptr ) );
}

string GetStrTime ( time_t t )
{
    char buff[20];
    strftime ( buff, 20, "%Y-%m-%d %H:%M:%S", localtime ( &t ) );
    return string ( buff );
}

int __mtfa_log ( const char* name, int line, const char* fmt... )
{
    va_list args;

    int iret;
    int len = 1024;
    char* buffer = ( char* ) malloc ( len );
    bool ok = false;

    while ( !ok )
    {
        buffer[0] = 0;  //tanto per inizializzare
        va_start ( args, fmt );
        iret = vsnprintf ( buffer, len, fmt, args );
        va_end ( args );

        if ( iret < len )
            ok = true;
        else
        {
            free ( buffer );
            len *= 2;
            buffer = ( char* ) malloc ( len );
        }
    }

    cout << GetStrTime() << ":" << name << ":" << line << " => " << buffer;
    free ( buffer );
    return iret;
}

int WriteAll ( int nfile, unsigned char* buf, int buflen )
{
    int scritti = 0;

    while ( scritti != buflen )
    {
#ifdef _WIN32
        int len = _write ( nfile, &buf[scritti], buflen - scritti );
#else
        int len = write ( nfile, &buf[scritti], buflen - scritti );
#endif

        if ( len <= 0 )
        {
            break;
        }

        scritti += len;
    }

    return scritti;
}

int ReadAll ( int nfile, unsigned char* buf, int buflen )
{
    int letti = 0;

    while ( letti != buflen )
    {
#ifdef _WIN32
        int len = _read ( nfile, &buf[letti], buflen - letti );
#else
        int len = read ( nfile, &buf[letti], buflen - letti );
#endif

        if ( len <= 0 )
        {
            break;
        }

        letti += len;
    }

    return letti;
}

void FileInfo ( const std::string& nomeFile,
                time_t& c_time,
                time_t& a_time,
                time_t& m_time,
                int& drive_number,
                bool& isDirectory,
                long long& size )
{
    int res;

#ifdef _WIN32
    struct _stat st;
    res = _stat ( nomeFile.c_str(), &st );
#else
    struct stat st;
    res = stat ( nomeFile.c_str(), &st );
#endif

    if ( res != 0 )
    {
        c_time = 0;
        a_time = 0;
        m_time = 0;
        drive_number = -1;
        isDirectory = false;
        size = -1;
    }
    else
    {
        c_time = st.st_ctime;
        a_time = st.st_atime;
        m_time = st.st_mtime;
        drive_number = st.st_dev;
        isDirectory = ( ( st.st_mode & S_IFREG ) != 0 );
        size = st.st_size;
    }

    return;
}

inline bool FileExists ( const std::string& name )
{
    return ( access ( name.c_str(), F_OK ) != -1 );
}

void CVettorePChar1::CopyData ( char* p, int len )
{
    //deve avere len dati di spazio, altrimenti ne usa quelli che ha!
    //len è minore di spazio, copio il possibile e poi loop
    int tot = len;

    while ( tot > 0 )
    {
        Reallocate ( righe );
        int vrow = righe >> base_allocazione_bits;
        int vcol = righe & base_allocazione_mask;
        int spazio = righe_attuali - righe;
        int copia = ( ( spazio > tot ) ? tot : spazio );
        memcpy ( &v[vrow][vcol], p, copia );
        righe += copia;
        tot -= copia;
        p += copia;
    }
}

string CVettorePChar1::Linearize ()
{
    int tot = righe;
	char * p = new char[righe];
	int pos = 0;

    for ( int i = 0; i < righe; i += base_allocazione )
    {
        int copia = ( tot < base_allocazione ) ? tot : base_allocazione;
        memcpy ( &p[i], v[pos++], copia );
        tot -= copia;
    }
    string s(p, &p[righe]);
	delete[] p;
	return s;
}

void CVettorePChar1::Reallocate ( unsigned int r )
{
    if ( r >= righe )
    {
        if ( r >= righe_attuali )
        {
            unsigned int newrighe = r + 1; // + base_allocazione;

            if ( v != nullptr )
            {
                int new_vrow = 1 + ( newrighe >> base_allocazione_bits );

                if ( new_vrow > rows )
                {
                    //devo riallocare righe
                    v = ( char** ) realloc ( v, ( new_vrow ) * sizeof ( char* ) );

                    for ( int i = rows; i < new_vrow; ++i )
                    {
                        v[i] = ( char* ) malloc ( base_allocazione * sizeof ( char ) );
                        std::fill ( & ( v[i][0] ), & ( v[i][base_allocazione] ), initializer );
                    }

                    righe_attuali = new_vrow * base_allocazione;
                    rows = new_vrow;
                }
            }
            else
            {
                rows = 1 + ( newrighe >> base_allocazione_bits );
                v = ( char** ) malloc ( rows * sizeof ( char* ) );

                for ( int i = 0; i < rows; ++i )
                {
                    v[i] = ( char* ) malloc ( base_allocazione * sizeof ( char ) );
                    std::fill ( & ( v[i][0] ), & ( v[i][base_allocazione] ), initializer );
                }

                righe_attuali = rows * base_allocazione;
            }
        }

        //righe = r + 1;
    }

    return;
}

void CVettorePChar1::Init ( char in_initializer )
{
    righe = 0;
    initializer = in_initializer;

    righe_attuali = 0; //base_allocazione;
    v = nullptr;
    rows = 0;
}

CVettorePChar1::CVettorePChar1 ( char in_initializer )
{
    Init ( in_initializer );
}

CVettorePChar1::CVettorePChar1()
{
    Init();
}

void CVettorePChar1::Init()
{
    righe = 0;
    initializer = 0; //zerox8000;

    righe_attuali = 0; //base_allocazione;
    v = nullptr;
}

CVettorePChar1::~CVettorePChar1()
{
    if ( v != NULL )
    {
        /*
        for (int i=0; i<righe; ++i)
        {
                int vrow = i >> base_allocazione_bits;
                int vcol = i & base_allocazione_mask;
                if(v[vrow][vcol])
                  free(v[vrow][vcol]);
        }
         */
        for ( int i = 0; i < rows; ++i )
        {
            if ( v[i] )
            {
                free ( v[i] );
                v[i] = nullptr;
            }
        }

        free ( v );
    }

    v = nullptr;
    righe = 0;
    righe_attuali = 0; //base_allocazione;
}

void CVettorePChar1::Clear()
{
    if ( v != NULL )
    {
        /*
        for (int i=0; i<righe; ++i)
        {
                int vrow = i >> base_allocazione_bits;
                int vcol = i & base_allocazione_mask;
                if(v[vrow][vcol])
                  free(v[vrow][vcol]);
        }
         */
        for ( int i = 0; i < rows; ++i )
        {
            if ( v[i] )
            {
                free ( v[i] );
                v[i] = nullptr;
            }
        }

        free ( v );
    }

    v = nullptr;

    righe_attuali = 0;
    righe = 0;
    Init();
}

unsigned long long
CVettorePChar1::Size()
{
    unsigned long long tot;
    tot = sizeof ( base_allocazione );
    tot += sizeof ( righe_attuali );
    tot += sizeof ( righe );
    tot += sizeof ( v );
    tot += sizeof ( char* ) *righe_attuali;
    std::cout << __func__  << "VB. Celle allocate: " << righe << std::endl;

    return tot;
}

//Clas binaryStream

mtfa_binary_stream_save::mtfa_binary_stream_save ( std::string name )
{
    if ( name.size() == 0 )
    {
        file = nullptr;
        filename.clear();
        curpos = 0;
        alloc_size = 1000;
        dim = 0;
        allocated = alloc_size;
    }
    else
    {
        file = fopen ( name.c_str(), "wb" );
        filename = name;
        curpos = 0;
        alloc_size = 1000;
        dim = 0;
        allocated = alloc_size;
    }
}

mtfa_binary_stream_save::~mtfa_binary_stream_save()
{
    if ( file != nullptr )
    {
        string s = stream_buf; //.Linearize ();
        int scritti = WriteAll ( fileno ( file ), ( unsigned char* ) s.data(), s.size());

        if ( file )
        {
            fclose ( file );
        }
    }

    dim = 0;
    curpos = 0;

    file = nullptr;
    alloc_size = 1000;
    dim = 0;
    allocated = 0;
}

std::string mtfa_binary_stream_save::GetStream()
{
    return stream_buf; //.Linearize ();
}

// void mtfa_binary_stream_save::Append(unsigned long long v)
// {
//     unsigned int len = sizeof (unsigned long long);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(long long v)
// {
//     unsigned int len = sizeof (long long);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned short v)
// {
//     unsigned int len = sizeof (unsigned short);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(short v)
// {
//     unsigned int len = sizeof (short);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned int v)
// {
//     unsigned int len = sizeof (unsigned int);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(int v)
// {
//     unsigned int len = sizeof (int);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned char v)
// {
//     unsigned int len = sizeof (unsigned char);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
// void mtfa_binary_stream_save::Append(char v)
// {
//     unsigned int len = sizeof (char);
//     stream_buf.CopyData((char *) &v, len);
//     dim += len;
// }
//
// //Lo stesso per gli array: un array di caratteri
//
// void mtfa_binary_stream_save::Append(unsigned long long * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (unsigned long long)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(long long * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (long long)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned int * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (unsigned int)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(int * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (int)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned short * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (unsigned short)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(short * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (short)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(unsigned char * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (unsigned char)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }
//
// void mtfa_binary_stream_save::Append(char * v, unsigned int item_num)
// {
//     Append(item_num); //Salvo il numero di elementi dell'array
//     unsigned int len = sizeof (char)*item_num;
//     stream_buf.CopyData((char *) v, len);
//     dim += len;
// }

// unsigned char mtfa_binary_stream_load::ReadUC()
// {
//     int len = sizeof (unsigned char);
//     curpos += len;
//     return *(unsigned char*) &stream_buf[curpos - len];
// }
// unsigned char mtfa_binary_stream_load::ReadC()
// {
//     int len = sizeof (char);
//     curpos += len;
//     return *(char*) &stream_buf[curpos - len];
// }
//
// unsigned int mtfa_binary_stream_load::ReadUI()
// {
//     int len = sizeof (unsigned int);
//     curpos += len;
//     return *(unsigned int*) &stream_buf[curpos - len];
// }
// int mtfa_binary_stream_load::ReadI()
// {
//     int len = sizeof (int);
//     curpos += len;
//     return *(int*) &stream_buf[curpos - len];
// }
//
// unsigned short mtfa_binary_stream_load::ReadUS()
// {
//     int len = sizeof (unsigned short);
//     curpos += len;
//     return *(unsigned short*) &stream_buf[curpos - len];
// }
// short mtfa_binary_stream_load::ReadS()
// {
//     int len = sizeof (short);
//     curpos += len;
//     return *(short*) &stream_buf[curpos - len];
// }
//
// unsigned long long mtfa_binary_stream_load::ReadULL()
// {
//     int len = sizeof (unsigned long long);
//     curpos += len;
//     return *(unsigned long long*) &stream_buf[curpos - len];
// }
// long long mtfa_binary_stream_load::ReadLL()
// {
//     int len = sizeof (long long);
//     curpos += len;
//     return *(long long*) &stream_buf[curpos - len];
// }

// unsigned char * mtfa_binary_stream_load::ReadVUC(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//
//     int blen = size * sizeof (unsigned char);
//     unsigned char * p = (unsigned char*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//
//     return p;
// }
// char * mtfa_binary_stream_load::ReadVC(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//
//     int blen = size * sizeof (char);
//     char * p = (char*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//
//     return p;
// }
//
// unsigned int * mtfa_binary_stream_load::ReadVUI(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (unsigned int);
//     unsigned int * p = (unsigned int*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }
// int * mtfa_binary_stream_load::ReadVI(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (int);
//     int * p = (int*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }
//
// unsigned short * mtfa_binary_stream_load::ReadVUS(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (unsigned short);
//     unsigned short* p = (unsigned short*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }
// short * mtfa_binary_stream_load::ReadVS(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (short);
//     short* p = (short*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }
//
// unsigned long long * mtfa_binary_stream_load::ReadVULL(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (unsigned long long);
//     unsigned long long * p = (unsigned long long*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }
// long long * mtfa_binary_stream_load::ReadVLL(unsigned int & size)
// {
//     size = ReadUI();
//     if (size == 0)
//         return NULL;
//     int blen = size * sizeof (long long);
//     long long * p = (long long*) malloc(blen);
//     memcpy(p, &stream_buf[curpos], blen);
//     curpos += blen;
//     return p;
// }

mtfa_binary_stream_load::mtfa_binary_stream_load ( std::string name )
{
    time_t c_time;
    time_t a_time;
    time_t m_time;
    int drive_number;
    bool isDirectory;
    long long size;

    if ( name.compare ( "" ) == 0 )
    {
        file = NULL;
        filename = "";
        curpos = 0;
        alloc_size = 0;
        dim = 0;
        allocated = 0;
        stream_buf = NULL;
    }
    else
    {
        FileInfo ( name, c_time, a_time, m_time, drive_number, isDirectory, size );
        file = fopen ( name.c_str(), "rb" );
        filename = name;
        curpos = 0;
        alloc_size = size;
        dim = 0;
        allocated = alloc_size;
        stream_buf = ( unsigned char* ) malloc ( alloc_size );

        //Lo leggo tutto
        dim = ReadAll ( fileno ( file ), stream_buf, allocated );
    }
}

void mtfa_binary_stream_load::mtfa_binary_stream_set ( const std::string& data )
{
    curpos = 0;
    alloc_size = data.size();
    dim = alloc_size;
    allocated = alloc_size;
    stream_buf = ( unsigned char* ) malloc ( alloc_size );
    memcpy ( stream_buf, data.data(), alloc_size );
}

mtfa_binary_stream_load::~mtfa_binary_stream_load()
{
    if ( file )
    {
        fclose ( file );
    }

    file = nullptr;
    alloc_size = 1000;
    dim = 0;
    allocated = 0;

    if ( stream_buf )
    {
        free ( stream_buf );
    }

    stream_buf = nullptr;
}

// std::string mtfa_binary_stream_load::ReadString()
// {
//     unsigned char * p;
//     unsigned int len;
//     p = Read<unsigned char>(len);
//     std::string s;
//     s.assign(p, &p[len]);
//     free(p);
//     return s;
// }

unsigned long long mtfa_fastrng64::next()
{
    //lock_guard<mutex> lock(*mtx);
    unsigned long long  z = ( x += 0x9E3779B97F4A7C15ULL );
    z = ( z ^ ( z >> 30 ) ) * 0xBF58476D1CE4E5B9ULL;
    z = ( z ^ ( z >> 27 ) ) * 0x94D049BB133111EBULL;
    return z ^ ( z >> 31 );
}

mtfa_fastrng1024::mtfa_fastrng1024()
{
    //mtx = new mutex();
    p = 0;
    mtfa_fastrng64 r ( 0 );

    for ( int i = 0; i < 16; ++i )
        s[i] = r.next();
};
mtfa_fastrng1024::mtfa_fastrng1024 ( unsigned long long seed )
{
    //mtx = new mutex();
    p = 0;
    mtfa_fastrng64 r ( seed );

    for ( int i = 0; i < 16; ++i )
        s[i] = r.next();
};

unsigned long long mtfa_fastrng1024::next ( void )
{
    //lock_guard<mutex> lock(*mtx);
    const unsigned long long s0 = s[p];
    unsigned long long s1 = s[p = ( p + 1 ) & 0x0F];
    s1 ^= s1 << 31; // a
    s[p] = s1 ^ s0 ^ ( s1 >> 11 ) ^ ( s0 >> 30 ); // b,c
    return s[p] * 1181783497276652981ULL;
	
	//11400714819323198483ULL
}

/* This is the jump function for the generator. It is equivalent
 *  to 2^512 calls to next(); it can be used to generate 2^512
 *  non-overlapping subsequences for parallel computations. */

void mtfa_fastrng1024::jump ( void )
{
    //lock_guard<mutex> lock(*mtx);
    static const unsigned long long JUMP[] = { 0x84242f96eca9c41dULL,
                                               0xa3c65b8776f96855ULL, 0x5b34a39f070b5837ULL, 0x4489affce4f31a1eULL,
                                               0x2ffeeb0a48316f40ULL, 0xdc2d9891fe68c022ULL, 0x3659132bb12fea70ULL,
                                               0xaac17d8efa43cab8ULL, 0xc4cb815590989b13ULL, 0x5ee975283d71c93bULL,
                                               0x691548c86c1bd540ULL, 0x7910c41d10a1e6a5ULL, 0x0b5fc64563b3e2a8ULL,
                                               0x047f7684e9fc949dULL, 0xb99181f2d8f685caULL, 0x284600e3f30e38c3ULL };

    unsigned long long t[16] = { 0 };

    for ( int i = 0; i < sizeof JUMP / sizeof * JUMP; i++ )
        for ( int b = 0; b < 64; b++ )
        {
            if ( JUMP[i] & 1ULL << b )
                for ( int j = 0; j < 16; j++ )
                    t[j] ^= s[ ( j + p ) & 15];

            next();
        }

    for ( int j = 0; j < 16; j++ )
        s[ ( j + p ) & 15] = t[j];
}

void mtfa_fastrand_sse::InitFastRand(
    uint16_t a1, uint16_t c1,
    uint16_t b1, uint16_t d1,
    uint16_t a2, uint16_t c2,
    uint16_t b2, uint16_t d2,
    uint16_t a3, uint16_t c3,
    uint16_t b3, uint16_t d3,
    uint16_t a4, uint16_t c4,
    uint16_t b4, uint16_t d4)
{

    //
    // Initialize MWC1616 masks and multipliers
    // Default values of 18000 and 30903 used
    // for multipliers

    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        fmask[i] = 0xFFFF;
        fm1[i] = 0x4650;
        fm2[i] = 0x78B7;
    }

    fa[0] = ((uint32_t)c1 << 16) | a1;
    fa[1] = ((uint32_t)c2 << 16) | a2;
    fa[2] = ((uint32_t)c3 << 16) | a3;
    fa[3] = ((uint32_t)c4 << 16) | a4;
    fb[0] = ((uint32_t)d1 << 16) | b1;
    fb[1] = ((uint32_t)d2 << 16) | b2;
    fb[2] = ((uint32_t)d3 << 16) | b3;
    fb[3] = ((uint32_t)d4 << 16) | b4;
}

mtfa_fastrand_sse::mtfa_fastrand_sse(uint64_t seed)
{

    uint32_t prngSeed[8];
    uint16_t *sptr = (uint16_t *)prngSeed;
    //fastrand = new T_fastrand();
    mtfa_fastrng1024 rng(seed);

    for (uint8_t i = 0; i < 8; i++)
        prngSeed[i] = rng.next();

    InitFastRand(sptr[0], sptr[1], sptr[2], sptr[3], sptr[4], sptr[5], sptr[6],
                 sptr[7], sptr[8], sptr[9], sptr[10], sptr[11], sptr[12], sptr[13],
                 sptr[14], sptr[15]);

    toGenerate = true;
}
mtfa_fastrand_sse::mtfa_fastrand_sse()
{
    mtfa_fastrand_sse((uint64_t)GetULLTime());
}

uint64_t mtfa_fastrand_sse::next()
{
    if (toGenerate)
    {
        __m128i a = _mm_load_si128((const __m128i *)&fa[0]);
        __m128i b = _mm_load_si128((const __m128i *)&fb[0]);

        const __m128i mask = _mm_load_si128((const __m128i *)&fmask[0]);
        const __m128i m1 = _mm_load_si128((const __m128i *)&fm1[0]);
        const __m128i m2 = _mm_load_si128((const __m128i *)&fm2[0]);

        __m128i ashift = _mm_srli_epi32(a, 0x10);
        __m128i amask = _mm_and_si128(a, mask);
        __m128i amullow = _mm_mullo_epi16(amask, m1);
        __m128i amulhigh = _mm_mulhi_epu16(amask, m1);
        __m128i amulhigh_shift = _mm_slli_epi32(amulhigh, 0x10);
        __m128i amul = _mm_or_si128(amullow, amulhigh_shift);
        __m128i anew = _mm_add_epi32(amul, ashift);
        _mm_store_si128((__m128i *)&fa[0], anew);

        __m128i bshift = _mm_srli_epi32(b, 0x10);
        __m128i bmask = _mm_and_si128(b, mask);
        __m128i bmullow = _mm_mullo_epi16(bmask, m2);
        __m128i bmulhigh = _mm_mulhi_epu16(bmask, m2);
        __m128i bmulhigh_shift = _mm_slli_epi32(bmulhigh, 0x10);
        __m128i bmul = _mm_or_si128(bmullow, bmulhigh_shift);
        __m128i bnew = _mm_add_epi32(bmul, bshift);
        _mm_store_si128((__m128i *)&fb[0], bnew);

        __m128i bmasknew = _mm_and_si128(bnew, mask);
        __m128i ashiftnew = _mm_slli_epi32(anew, 0x10);
        __m128i res = _mm_add_epi32(ashiftnew, bmasknew);
        _mm_store_si128((__m128i *)&fres[0], res);

        toGenerate = false;
        return fres[0];
    }
    else
    {
        toGenerate = true;
        return fres[1];
    }
}
uint64_t mtfa_fastrand_sse::next_sse4()
{
    if (toGenerate)
    {
        __m128i a = _mm_load_si128((const __m128i *)fa);
        __m128i b = _mm_load_si128((const __m128i *)fb);

        const __m128i mask = _mm_load_si128((const __m128i *)fmask);
        const __m128i m1 = _mm_load_si128((const __m128i *)fm1);
        const __m128i m2 = _mm_load_si128((const __m128i *)fm2);

        __m128i amask = _mm_and_si128(a, mask);
        __m128i ashift = _mm_srli_epi32(a, 0x10);
        __m128i amul = _mm_mullo_epi32(amask, m1);
        __m128i anew = _mm_add_epi32(amul, ashift);
        _mm_store_si128((__m128i *)fa, anew);

        __m128i bmask = _mm_and_si128(b, mask);
        __m128i bshift = _mm_srli_epi32(b, 0x10);
        __m128i bmul = _mm_mullo_epi32(bmask, m2);
        __m128i bnew = _mm_add_epi32(bmul, bshift);
        _mm_store_si128((__m128i *)fb, bnew);

        __m128i bmasknew = _mm_and_si128(bnew, mask);
        __m128i ashiftnew = _mm_slli_epi32(anew, 0x10);
        __m128i res = _mm_add_epi32(ashiftnew, bmasknew);
        _mm_store_si128((__m128i *)fres, res);

        toGenerate = false;
        return fres[0];
    }
    else
    {
        toGenerate = true;
        return fres[1];
    }
}

#define mat_cypher_ext_dim 0x0000FFFF               //65536
extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];


/*
void mtfa_mat_cypher_hybrid::SetMask ( unsigned long long seed )
{
    where = 0;

    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

void mtfa_mat_cypher_hybrid::SetMask ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

mtfa_mat_cypher_hybrid::mtfa_mat_cypher_hybrid ( unsigned long long seed )
{
    where = 0;
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}


mtfa_mat_cypher_hybrid::mtfa_mat_cypher_hybrid ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

void mtfa_mat_cypher_hybrid::NextPosition()
{
    where = 0;
    * ( ( unsigned long long* ) &x[0] ) +=  mtfa_mat_cypher_base_ext[ x[0] & 0x07][* ( unsigned short* ) &x[2]] ^
                                            mtfa_mat_cypher_base_ext[ x[4] & 0x07][* ( unsigned short* ) &x[6]] ^
                                            mask.next();
}

void mtfa_mat_cypher_hybrid::cypher ( char* data, int len )  //Cifra incrementale
{
    if ( len <= 0 )
        return;

    if ( where == 0 ) {
        int conta;
        for ( conta = 0; ( conta + 8 ) <= len; conta += 8 ) {
            * ( ( unsigned long long* ) &data[conta] ) ^= * ( unsigned long long* ) &x[0];
            NextPosition();
        }

        if ( conta < len ) {
            auto pd = &data[conta];
            auto px = &x[0];
            where = len - conta;

            while ( where-- > 0 )
                *pd++ ^= *px++;

            where = len - conta;
        }
    } else {
        if ( len <= ( 8 - where ) ) {
            auto pd = data;
            auto px = &x[where];
            where += len;

            while ( len-- > 0 )
                *pd++ ^= *px++;

            if ( where == 8 )
                NextPosition();
        } else {
            auto pd = data;
            auto px = &x[where];
            int quanti = 8 - where;

            while ( where++ < 8 )
                *pd++ ^= *px++;

            NextPosition();
            cypher ( &data[quanti], len - quanti );
        }
    }
}
*/


/******************Ora la cifra fast*********************/



void mtfa_mat_cypher_fast::SetMask ( unsigned long long seed )
{
    where = 0;

    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

void mtfa_mat_cypher_fast::SetMask ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

mtfa_mat_cypher_fast::mtfa_mat_cypher_fast ( unsigned long long seed )
{
    where = 0;
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}


mtfa_mat_cypher_fast::mtfa_mat_cypher_fast ( const char* data, int len )
{
    where = 0;
    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = mtfa_fastrng1024 ( seed );

    * ( unsigned long long* ) &x[0] =   mtfa_mat_cypher_base_ext[0][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[1][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[2][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[3][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[4][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[5][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[6][ mask.next() & mat_cypher_ext_dim] ^
                                        mtfa_mat_cypher_base_ext[7][ mask.next() & mat_cypher_ext_dim];
}

void mtfa_mat_cypher_fast::NextPosition()
{
    where = 0;
    * ( ( unsigned long long* ) &x[0] ) +=  mtfa_mat_cypher_base_ext[ x[0] & 0x07][* ( unsigned short* ) &x[2]] ^
                                            mtfa_mat_cypher_base_ext[ x[4] & 0x07][* ( unsigned short* ) &x[6]] ^
                                            mask.next();
}

void mtfa_mat_cypher_fast::cypher ( char* data, int len )  //Cifra incrementale
{
    //Sono 8 caratteri disponibili. where mi dice quale carattere degli 8
    //indirizzati dal vettore x attuale sono stati utilizzati

    if ( len <= 0 )
        return;

    if ( where == 0 )
    {
        //8 caratteri alla volta calcola e fa lo XOR
        int conta;

        for ( conta = 0; ( conta + 8 ) <= len; conta += 8 )
        {
            //cout << "Fa da " << conta << " a " << conta+8 << endl;
            * ( ( unsigned long long* ) &data[conta] ) ^= * ( unsigned long long* ) &x[0];
            NextPosition();
        }

        //Fa i rimanenti
        if ( conta < len )
        {
            //cout << "fa da " << conta << " a " << len << endl;
            auto pd = &data[conta];
            auto px = &x[0];
            where = len - conta;

            while ( where-- > 0 )
                *pd++ ^= *px++;

            where = len - conta;
        }
    }
    else
    {
        if ( len <= ( 8 - where ) )
        {
            //Esaurisco con quelli che ho
            //cout << "fa da " << 0 << " a " << where << endl;
            auto pd = data;
            auto px = &x[where];
            where += len;

            while ( len-- > 0 )
                *pd++ ^= *px++;

            if ( where == 8 )
                NextPosition();
        }
        else
        {
            //len deve consumare più di where. Prima consumo il where che avevo
            //cout << "fa da " << 0 << " a " << 8-where << endl;
            auto pd = data;
            auto px = &x[where];
            int quanti = 8 - where;

            while ( where++ < 8 )
                *pd++ ^= *px++;

            NextPosition();
            //cout << "Riparte da " << quanti << " per " << len  - quanti << " bytes\n";
            cypher ( &data[quanti], len - quanti );
        }
    }
}







void mtfa_mat_cypher_ext::SetMask ( unsigned long long seed )
{
    if ( mask )
    {
        delete mask;
    }

    mask = new ULL_Rng ( seed );

    unsigned long long x0 = mtfa_mat_cypher_base_ext[0][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[1][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[2][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[3][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];
    unsigned long long y0 = mtfa_mat_cypher_base_ext[4][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[5][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[6][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[7][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];

    memcpy ( x, &x0, 8 );
    memcpy ( &x[4], &y0, 8 );
}

void mtfa_mat_cypher_ext::SetMask ( const char* data, int len )
{
    if ( mask )
    {
        delete mask;
    }

    unsigned long long seed = fastcrc64::crc64speed_native ( 0ULL, data, len );
    mask = new ULL_Rng ( seed );

    unsigned long long x0 = mtfa_mat_cypher_base_ext[0][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[1][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[2][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[3][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];
    unsigned long long y0 = mtfa_mat_cypher_base_ext[4][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[5][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[6][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[7][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];

    memcpy ( x, &x0, 8 );
    memcpy ( &x[4], &y0, 8 );
}

unsigned long long mtfa_mat_cypher_ext::DoCrc64 ( const char* data, int len )
{
    return fastcrc64::crc64speed ( 0ULL, data, len );
}

char* mtfa_mat_cypher_ext::cypher ( const char* data, int len )   //Cifra incrementale
{
    unsigned long long tbx;
    char* cif = ( char* ) malloc ( len );
    memcpy ( cif, data, len );
    //8 caratteri alla volta calcola e fa lo XOR
    int conta;

    for ( conta = 0; ( conta + 8 ) <= len; conta += 8 )
    {
        tbx =   mtfa_mat_cypher_base_ext[x[4] & 0x07][x[5]] ^
                mtfa_mat_cypher_base_ext[x[6] & 0x07][x[7]] ^ //mask->Next();
                mtfa_mat_cypher_base_ext[x[0] & 0x07][x[1]] ^
                mtfa_mat_cypher_base_ext[x[2] & 0x07][x[3]];

        NextPosition();
        * ( ( unsigned long long* ) &cif[conta] ) ^= tbx;
    }

    if ( ( len - conta ) > 0 )
    {
        tbx =   mtfa_mat_cypher_base_ext[x[4] & 0x07][x[5]] ^
                mtfa_mat_cypher_base_ext[x[6] & 0x07][x[7]] ^ //mask->Next();
                mtfa_mat_cypher_base_ext[x[0] & 0x07][x[1]] ^
                mtfa_mat_cypher_base_ext[x[2] & 0x07][x[3]];
        NextPosition();
    }

    //ne fa 4, poi 2 e poi 1
    if ( ( len - conta ) >= 4 )
    {
        * ( ( unsigned int* ) &cif[conta] ) ^= * ( unsigned int* ) &tbx;
        conta += 4;
    }

    if ( ( len - conta ) >= 2 )
    {
        * ( ( unsigned short* ) &cif[conta] ) ^= * ( ( unsigned short* ) &tbx + 2 );
        conta += 2;
    }

    if ( ( len - conta ) >= 1 )
    {
        * ( ( unsigned char* ) &cif[conta] ) ^= * ( ( unsigned char* ) &tbx + 6 );
        conta += 1;
    }

    return cif;
}

mtfa_mat_cypher_ext::mtfa_mat_cypher_ext ( unsigned long long seed )
{
    mask = new ULL_Rng ( seed );

    unsigned long long x0 = mtfa_mat_cypher_base_ext[0][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[1][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[2][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[3][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];
    unsigned long long y0 = mtfa_mat_cypher_base_ext[4][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[5][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[6][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[7][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];

    memcpy ( x, &x0, 8 );
    memcpy ( &x[4], &y0, 8 );
}

mtfa_mat_cypher_ext::mtfa_mat_cypher_ext()
{
    mask = nullptr;
}

mtfa_mat_cypher_ext::mtfa_mat_cypher_ext ( const char* data, int len )
{
    unsigned long long seed = DoCrc64 ( data, len );
    mask = new ULL_Rng ( seed );
    unsigned long long x0 = mtfa_mat_cypher_base_ext[0][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[1][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[2][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[3][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];
    unsigned long long y0 = mtfa_mat_cypher_base_ext[4][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[5][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[6][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim] ^
                            mtfa_mat_cypher_base_ext[7][ ( 33577 * ( mask->Next() & mat_cypher_ext_dim ) ) & mat_cypher_ext_dim];

    memcpy ( x, &x0, 8 );
    memcpy ( &x[4], &y0, 8 );
}

mtfa_mat_cypher_ext::~mtfa_mat_cypher_ext()
{
    if ( mask )
    {
        delete mask;
    }

    mask = nullptr;
}

unsigned long long mtfa_mat_cypher_ext::mtfa_mat_rand()
{
    unsigned long long out[6];

    if ( !mask )
    {
        out[0] = mtfa_mat_cypher_base_ext[2][x[2]];
        out[1] = mtfa_mat_cypher_base_ext[3][x[3]];
        out[2] = mtfa_mat_cypher_base_ext[4][x[4]];
        out[3] = mtfa_mat_cypher_base_ext[5][x[5]];
        out[4] = mtfa_mat_cypher_base_ext[6][x[6]];
        out[5] = mtfa_mat_cypher_base_ext[7][x[7]];
    }
    else
    {
        out[0] = mtfa_mat_cypher_base_ext[2][x[2]] ^ mask->Next();
        out[1] = mtfa_mat_cypher_base_ext[3][x[3]] ^ mask->Next();
        out[2] = mtfa_mat_cypher_base_ext[4][x[4]] ^ mask->Next();
        out[3] = mtfa_mat_cypher_base_ext[5][x[5]] ^ mask->Next();
        out[4] = mtfa_mat_cypher_base_ext[6][x[6]] ^ mask->Next();
        out[5] = mtfa_mat_cypher_base_ext[7][x[7]] ^ mask->Next();
    }

    NextPosition();

    if ( !mask )
    {
        return out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5] ^ mask->Next();
    }
    else
    {
        return out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5];
    }
}

void mtfa_mat_cypher_ext::NextPosition()
{
    //le prime due mi danno le posizioni per loro stessi e le altre sei, disturbate dalla distribuzione dello pseudorandom ullrng
//    unsigned long long x0 = mtfa_mat_cypher_base_ext[0][(x[0] * (mask->Next() % mat_cypher_ext_dim)) % mat_cypher_ext_dim];
//    unsigned long long y0 = mtfa_mat_cypher_base_ext[1][(x[1] * (mask->Next() % mat_cypher_ext_dim)) % mat_cypher_ext_dim];
    * ( ( unsigned long long* ) &x[0] ) += mtfa_mat_cypher_base_ext[ ( x[0] ) & 0x07][x[1]];
    * ( ( unsigned long long* ) &x[4] ) += mtfa_mat_cypher_base_ext[ ( x[2] ) & 0x07][x[3]];
}

unsigned long long mtfa_mat_cypher ( unsigned long long magic, unsigned long long disturber, unsigned char* daCifrare, int len )
{
    unsigned long long last_item;

    ULL_Rng jrpos = ULL_Rng ( magic );
    extern unsigned long long mtfa_mat_cypher_base[32768];

    unsigned long long j_steps = 0; //Quanti passi ho fatto?

    int i;

    for ( i = 0; ( i + 32 ) < len; i += 32 )
    {
        //ogni posizione vale 8 bytes
        unsigned long long posto = jrpos.Next() ^ disturber;
        last_item = posto; //compreso il disturber

        unsigned int row1 = ( posto << 10 ) & 0x0FFC00; //1024*1024 >> 1111 1111 1100 0000 0000;
        unsigned int col1 = ( posto >> 8 ) & 0x03FC; //11 1111 1100 1024/4;
        unsigned int row2 = ( posto >> 8 ) & 0x0FFC00; //1024;
        unsigned int col2 = ( posto >> 26 ) & 0x03FC; // 1024/4;
        unsigned int row3 = ( posto >> 26 ) & 0x0FFC00; //1024;
        unsigned int col3 = ( posto >> 44 ) & 0x03FC; // 1024/4;
        unsigned int row4 = ( posto >> 44 ) & 0x0FFC00; //1024;
        unsigned int col4 = ( posto >> 20 ) & 0x03FC; // 1024/4;

        * ( unsigned long long* ) & ( daCifrare[i] ) ^= mtfa_mat_cypher_base[ ( row1 + col1 + 0 ) % 32768];
        * ( unsigned long long* ) & ( daCifrare[i + 8] ) ^= mtfa_mat_cypher_base[ ( row2 + col2 + 1 ) % 32768];
        * ( unsigned long long* ) & ( daCifrare[i + 16] ) ^= mtfa_mat_cypher_base[ ( row3 + col3 + 2 ) % 32768];
        * ( unsigned long long* ) & ( daCifrare[i + 24] ) ^= mtfa_mat_cypher_base[ ( row4 + col4 + 3 ) % 32768];

        j_steps++;
    }

    if ( ( i + 1 ) <= len )
    {
        //Non ho ancora terminato! Ne mancano sicuramente meno di 32
        //ogni posizione vale 8 bytes
        unsigned long long posto = jrpos.Next() ^ disturber;
        last_item = posto; //compreso il disturber
        j_steps++;

        unsigned int row1 = ( posto << 10 ) & 0x0FFC00; //1024;
        unsigned int col1 = ( posto >> 8 ) & 0x003FC; // 1024/4;
        unsigned int row2 = ( posto >> 8 ) & 0x0FFC00; //1024;
        unsigned int col2 = ( posto >> 26 ) & 0x03FC; // 1024/4;
        unsigned int row3 = ( posto >> 26 ) & 0x0FFC00; //1024;
        unsigned int col3 = ( posto >> 44 ) & 0x03FC; // 1024/4;
        unsigned int row4 = ( posto >> 44 ) & 0x0FFC00; //1024;
        unsigned int col4 = ( posto >> 20 ) & 0x03FC; // 1024/4;

        if ( ( len - i ) < 8 )
        {
            for ( int j = i; j < len; j++ )
            {
                daCifrare[j] ^= ( ( unsigned char* ) ( & ( mtfa_mat_cypher_base[ ( row1 + col1 + 0 ) % 32768] ) ) ) [j - i];
            }
        }
        else if ( ( len - i ) < 16 )
        {
            * ( unsigned long long* ) & ( daCifrare[i] ) ^= mtfa_mat_cypher_base[ ( row1 + col1 + 0 ) % 32768];

            for ( int j = ( i + 8 ); j < len; j++ )
            {
                daCifrare[j] ^= ( ( unsigned char* ) ( & ( mtfa_mat_cypher_base[ ( row2 + col2 + 1 ) % 32768] ) ) ) [j - i - 8];
            }
        }
        else if ( ( len - i ) < 24 )
        {
            * ( unsigned long long* ) & ( daCifrare[i] ) ^= mtfa_mat_cypher_base[ ( row1 + col1 + 0 ) % 32768];
            * ( unsigned long long* ) & ( daCifrare[i + 8] ) ^= mtfa_mat_cypher_base[ ( row2 + col2 + 1 ) % 32768];

            for ( int j = ( i + 16 ); j < len; j++ )
            {
                daCifrare[j] ^= ( ( unsigned char* ) ( & ( mtfa_mat_cypher_base[ ( row3 + col3 + 2 ) % 32768] ) ) ) [j - i - 16];
            }
        }
        else
        {
            * ( unsigned long long* ) & ( daCifrare[i] ) ^= mtfa_mat_cypher_base[ ( row1 + col1 + 0 ) % 32768];
            * ( unsigned long long* ) & ( daCifrare[i + 8] ) ^= mtfa_mat_cypher_base[ ( row2 + col2 + 1 ) % 32768];
            * ( unsigned long long* ) & ( daCifrare[i + 16] ) ^= mtfa_mat_cypher_base[ ( row3 + col3 + 2 ) % 32768];

            for ( int j = ( i + 24 ); j < len; j++ )
            {
                daCifrare[j] ^= ( ( unsigned char* ) ( & ( mtfa_mat_cypher_base[ ( row4 + col4 + 3 ) % 32768] ) ) ) [j - i - 24];
            }
        }
    }


    //Ora che ho cifrato, torno indietro: codice (OK), posizione iniziale (steps) e stringa basata e cifrata
    return j_steps;
}

unsigned long long mtfa_reverse_bits ( unsigned long long v )
{
    unsigned long long r = v; // r will be reversed bits of v; first get LSB of v
#ifndef CHAR_BIT
    int s = sizeof ( v ) * 8 - 1; // extra shift needed at end
#else
    int s = sizeof ( v ) * CHAR_BIT - 1; // extra shift needed at end
#endif

    for ( v >>= 1; v; v >>= 1 )
    {
        r <<= 1;
        r |= v & 1;
        s--;
    }

    r <<= s; // shift when v's highest bits are zero
    return r;
}

static mtfa_fastrng1024 random_generator = mtfa_fastrng1024 ( GetULLTime() );

string mtfa_special_cypher ( string& data )
{
    string sRet;

    const int bucket = 64;

    unsigned long long magic = random_generator.next() ^ ( GetULLTime() ^ mtfa_reverse_bits ( GetULLTime() ) );

    //un solo parametro, la stringa da cifrare!
    if ( data.size() == 0 )
    {
        return sRet;
    }

    string to_be_cyphered = data;

    //ora splitto la stringa in parti lunghe piu' o meno bucket caratteri
    std::list<string> ls;
    int i = 0;

    while ( to_be_cyphered.size() > bucket )
    {
        int rnd = random_generator.next() % 10;
        rnd -= 5;

        while ( to_be_cyphered.size() <= ( bucket + rnd ) )
        {
            rnd--;
        }

        ls.push_back ( to_be_cyphered.substr ( 0, bucket + rnd ) );
        to_be_cyphered = to_be_cyphered.substr ( bucket + rnd );
    }

    if ( to_be_cyphered.size() > 0 )
    {
        ls.push_back ( to_be_cyphered );
    }

    //Se mi è avanzato qualcosa....
    unsigned long long dist = random_generator.next() ^ ( GetULLTime() ^ mtfa_reverse_bits ( GetULLTime() ) );

    string result;

    for ( auto it = ls.begin(); it != ls.end(); ++it )
    {
        unsigned char* p = ( unsigned char* ) malloc ( ( *it ).size() );
        memcpy ( p, ( *it ).data(), ( *it ).size() );
        mtfa_mat_cypher ( magic, dist, p, ( *it ).size() );
        unsigned int len = ( *it ).size();
        magic ^= 0xFFFFFFFFFFFFFFFF;
        dist ^= 0xFFFFFFFFFFFFFFFF;
        len ^= 0xFFFFFFFF;
        result += string ( ( unsigned char* ) &magic, & ( ( ( unsigned char* ) &magic ) [8] ) ) +
                  string ( ( unsigned char* ) &dist, & ( ( ( unsigned char* ) &dist ) [8] ) ) +
                  string ( ( unsigned char* ) &len, & ( ( ( unsigned char* ) &len ) [4] ) ) +
                  string ( p, &p[ ( *it ).size()] );
        magic = random_generator.next() ^ dist ^ ( GetULLTime() ^ mtfa_reverse_bits ( GetULLTime() ) );
        free ( p );
        dist = random_generator.next() ^ magic ^ ( GetULLTime() ^ mtfa_reverse_bits ( GetULLTime() ) );
    }

    //ora, nella stringa result ho il pacco cifrato e composto a vari pezzetti. Lo cifro ancora in modo xor seriale
    result[0] ^= 0x57;
    unsigned char inizio = result[0];

    for ( auto it = 1; it < result.size(); ++it )
    {
        result[it] ^= ( inizio * inizio ) & 0xFF;
        inizio = result[it];
    }

    return result;
}

string mtfa_special_decypher ( string& data )
{
    string result;
    string sRet;

    const int bucket = 64;
    string to_be_decyphered = data;

    for ( auto it = to_be_decyphered.size() - 1; it > 0; --it )
    {
        to_be_decyphered[it] ^= ( to_be_decyphered[it - 1] * to_be_decyphered[it - 1] ) & 0xFF;
    }

    to_be_decyphered[0] ^= 0x57;

    //Lavoro su un buffer, piu' pulito!
    unsigned char* p = ( unsigned char* ) malloc ( to_be_decyphered.size() );
    memcpy ( p, to_be_decyphered.data(), to_be_decyphered.size() );

    unsigned char* pos = p;
    unsigned int pos_len = to_be_decyphered.size();

    std::list<string> ls;

    while ( pos_len > 0 )
    {
        unsigned int len;

        if ( pos_len < 20 )
        {
            free ( p );
            return result;
        }

        memcpy ( ( unsigned char* ) &len, &pos[16], 4 );
        len ^= 0xFFFFFFFF;

        if ( pos_len < ( 20 + len ) )
        {
            free ( p );
            return result;
        }

        //ora posso provare a decifrare!
        unsigned long long magic;
        unsigned long long dist;
        memcpy ( ( unsigned char* ) &magic, pos, 8 );
        memcpy ( ( unsigned char* ) &dist, &pos[8], 8 );
        magic ^= 0xFFFFFFFFFFFFFFFF;
        dist ^= 0xFFFFFFFFFFFFFFFF;
        pos += 20;
        mtfa_mat_cypher ( magic, dist, pos, len );
        ls.push_back ( string ( pos, &pos[len] ) );
        pos += len;
        pos_len -= ( 20 + len );
    }

    free ( p );

    //Ora metto tutto nella stessa stringa
    for ( auto it = ls.begin(); it != ls.end(); ++it )
    {
        result += ( *it );
    }

    return result;
}

T_V_Split mtfa_string_split ( const string& str, const string& delimiters )
{
    std::vector<string> result;

    if ( str.size() <= 0 )
    {
        return result;
    }

    int pos;
    int inizio = 0;

    while ( true )
    {
        pos = str.find_first_of ( delimiters, inizio );

        if ( pos == string::npos )
        {
            result.push_back ( str.substr ( inizio ) );
            break;
        }
        else
        {
            result.push_back ( str.substr ( inizio, pos - inizio ) );
            inizio = pos + 1;
        }
    }

    return result;
}

/*
 Questa funzione splitta in base alla sottostringa*/
T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter )
{
    std::vector<string > result;

    if ( str.size() <= 0 )
    {
        return result;
    }

    int pos;
    int inizio = 0;

    while ( true )
    {
        pos = str.find ( delimiter, inizio );

        if ( pos == string::npos )
        {
            result.push_back ( str.substr ( inizio ) );
            break;
        }
        else
        {
            result.push_back ( str.substr ( inizio, pos - inizio ) );
            inizio = pos + delimiter.size();
        }
    }

    return result;
}

inline unsigned char to_hex ( unsigned char x )
{
    return x + ( x > 9 ? ( 'A' - 10 ) : '0' );
}

std::string urlencode ( const std::string& s )
{
    ostringstream os;

    for ( std::string::const_iterator ci = s.begin(); ci != s.end(); ++ci )
    {
        if ( ( *ci >= 'a' && *ci <= 'z' ) ||
                ( *ci >= 'A' && *ci <= 'Z' ) ||
                ( *ci >= '0' && *ci <= '9' ) )
        {
            // allowed
            os << *ci;
        }
        else if ( *ci == ' ' )
        {
            os << "%20";
        }
        else
        {
            os << '%' << to_hex ( *ci >> 4 ) << to_hex ( *ci % 16 );
        }
    }

    return os.str();
}

inline unsigned char from_hex ( unsigned char ch )
{
    if ( ch <= '9' && ch >= '0' )
    {
        ch -= '0';
    }
    else if ( ch <= 'f' && ch >= 'a' )
    {
        ch -= 'a' - 10;
    }
    else if ( ch <= 'F' && ch >= 'A' )
    {
        ch -= 'A' - 10;
    }
    else
    {
        ch = 0;
    }

    return ch;
}


std::string urldecode ( const std::string& str )
{
    using namespace std;

    try
    {
        std::string result;
        string::size_type i;

        for ( i = 0; i < str.size(); ++i )
        {
//             if ( str[i] == '+' )
//             {
//                 result += ' ';
//             }
//             else 
            if ( str[i] == '%' && str.size() > i + 2 )
            {
                const unsigned char ch1 = from_hex ( str[i + 1] );
                const unsigned char ch2 = from_hex ( str[i + 2] );
                const unsigned char ch = ( ch1 << 4 ) | ch2;
                result += ch;
                i += 2;
            }
            else
            {
                result += str[i];
            }
        }

        return result;
    }
    catch ( std::exception& ex )
    {
        mtfa_log("urldecode: %s\n", ex.what());
    }

    return "";
}



//torna il numero di bytes non inviati

int mtfa_SendAll ( int s, string b, int ms )
{
    //Se il timeout \ufffd 0, allora non c'\ufffd
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif
    }

    int len = b.size();
    int sent = 0;

    while ( len > 0 )
    {
        int l = send ( s, b.substr ( sent ).data(), len, 0 );

        if ( l > 0 )
        {
            sent += l;
            len -= l;
        }
        else
        {
            break;
        }
    }

    return len; //se > 0 errore, ne mancano ancora da inviare!
}

int mtfa_SendAll ( int s, char* b, int len, int ms )
{
    //Se il timeout \ufffd 0, allora non c'\ufffd
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif
    }

    int sent = 0;

    while ( len > 0 )
    {
        int l = send ( s, &b[sent], len, 0 );

        if ( l > 0 )
        {
            sent += l;
            len -= l;
        }
        else
        {
            break;
        }
    }

    return len; //se > 0 errore, ne mancano ancora da inviare!
}

void mtfa_RecvAll ( int s, string& b, int len, int ms )
{
    //Se il timeout \ufffd 0, allora non c'\ufffd
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &tv, sizeof ( tv ) );
        printf ( "Messo timeout a %ld secondi e %ld nanosecondi\n", tv.tv_sec, tv.tv_usec );
#endif

    }

    char c;
    int iRet;
    b.clear();

    char* linea = ( char* ) malloc ( len );
    int letti = 0;

    while ( letti != len )
    {
        iRet = recv ( s, &linea[letti], len - letti, 0 );

        if ( iRet <= 0 )
        {
            break;
        }
        else
        {
            letti += iRet;
        }
    }

    if ( letti > 0 )
    {
        b.assign ( linea, &linea[letti] );
    }

    free ( linea );
    return;
}

bool mtfa_SendInt ( int s, int val, int ms )
{
    //Mando sempre in network order
    int theval = htonl ( val );
    int sent = mtfa_SendAll ( s, ( char* ) &theval, sizeof ( theval ), ms );
    return sent == 0;
}

bool mtfa_RecvInt ( int s, int& val, int ms )
{
    //Se il timeout \ufffd 0, allora non c'\ufffd
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif

    }

    char c;
    int iRet;
    int theval = 0;

    int len = sizeof ( int );
    int letti = 0;

    while ( letti != len )
    {
        iRet = recv ( s, & ( ( char* ) &theval ) [letti], len - letti, 0 );

        if ( iRet <= 0 )
        {
            break;
        }
        else
        {
            letti += iRet;
        }
    }

    if ( letti != len )
    {
        return false;
    }
    else
    {
        val = ntohl ( theval );
        return true;
    }
}

bool mtfa_RecvStream ( int s, string& str, int ms )
{
    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif

    }

    char c;
    int iRet;

    while ( ( iRet = recv ( s, &c, 1, 0 ) ) == 1 )
    {
        str += c;

        if ( c == '\n' )
        {
            break;
        }
    }

    if ( iRet == 1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//\ufffd la copia della send all!!!

bool mtfa_SendStream ( int s, string str, int ms )
{
    int iRet = mtfa_SendAll ( s, str, ms );

    if ( iRet == str.size() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool mtfa_TestConnection ( char* ip, int port, int timeout )
{
    int s;
    s = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    int ms = timeout;

    if ( ms > 0 )
    {
#ifdef _WIN32
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &ms, sizeof ( ms ) );
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ( ms % 1000 ) * 1000000;
        setsockopt ( s, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &tv, sizeof ( tv ) );
#endif

    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr ( ip );
    service.sin_port = htons ( port );
    int iRet = connect ( s, ( struct sockaddr* ) & service, sizeof ( service ) );

    if ( iRet == -1 )
    {
        return false;
    }
    else
    {
        close ( s );
        return true;
    }
}

bool mtfa_RecvBuffer ( int s, string& b, int ms )
{
    int len;
    bool bret = mtfa_RecvInt ( s, len, ms );

    if ( bret )
    {
        mtfa_RecvAll ( s, b, len, ms );

        if ( b.size() != len )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool mtfa_SendBuffer ( int s, string b, int ms )
{
    bool bret = mtfa_SendInt ( s, b.size(), ms );

    if ( !bret )
    {
        return false;
    }

    int len = mtfa_SendAll ( s, b, ms );

    if ( len > 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::string StrTime()
{
    struct tm today;
    time_t ltime;
    time ( &ltime );
#ifndef _WIN32
    localtime_r ( &ltime, &today );
#else
    localtime_s ( &today, &ltime );
#endif
    char abstime[128];
#ifndef _WIN32
    ctime_r ( &ltime, abstime );
#else
    ctime_s ( abstime, 127, &ltime );
#endif

    char sout [1024];
    sprintf ( sout, "%04d.%02d.%02d.%02d.%02d.%02d: ", today.tm_year + 1900,
              today.tm_mon + 1,
              today.tm_mday,
              today.tm_hour,
              today.tm_min,
              today.tm_sec );
    return std::string ( sout );
}

string mtfa_FileIo::MkTemp()
{
#ifdef _WIN32

    // lo crea nella cartella temporanea
    char* s = getenv ( "TEMP" );

    // ora creo il nome
    char* basename = "tmpXXXXXXXX";
    char* nome = ( char* ) malloc ( 4096 );

    strcpy ( nome, s );
    strcat ( nome, "/" );
    strcat ( nome, basename );

    char* pcret = _mktemp ( nome );
    string sret = nome;

    free ( nome );

    return sret;
#else

    // lo crea nella cartella temporanea
    string path;

    // ora creo il nome
    char* basename = ( char* ) malloc ( 1024 );

    strcpy ( basename, "/tmp/tmpXXXXXX" );
    close ( mkstemp ( basename ) );

    string name = basename;

    free ( basename );

    path = name;

    return path;
#endif

}

bool mtfa_FileIo::Rename ( const string& n1,
                           const string& n2 )
{
    return rename ( n1.c_str(), n2.c_str() ) == 0;
}

bool mtfa_FileIo::Exists ( const string& nomeFile )
{
    return ( access ( nomeFile.c_str(), 0 ) == 0 );
}

bool mtfa_FileIo::Delete ( const string& nomeFile )
{
    if ( !Exists ( nomeFile ) )
    {
        return true;
    }

    return unlink ( nomeFile.c_str() ) == 0;
}

bool mtfa_FileIo::Create ( const string& nomeFile, bool isText )
{
    try
    {
#ifdef _WIN32
        FILE* fou = fopen ( nomeFile.c_str(), ( isText ? "wt" : "wb" ) );
#else
        FILE* fou = fopen ( nomeFile.c_str(), "w" );
#endif

        if ( fou != NULL )
        {
            fclose ( fou );
            return true;
        }
        else
        {
            return false;
        }
    }
    catch ( std::exception& ex )
    {
        return false;
    }
}

bool mtfa_FileIo::Write ( const string& nomeFile,
                          unsigned char* pval,
                          int len,
                          bool isText )
{
    try
    {
#ifdef _WIN32
        FILE* fou = fopen ( nomeFile.c_str(), ( isText ? "wt" : "wb" ) );
#else
        FILE* fou = fopen ( nomeFile.c_str(), "w" );
#endif

        if ( fou != NULL )
        {
            int scritti;
            scritti = 0;

            while ( scritti < len )
            {
                int l = write ( fileno ( fou ), &pval[scritti], len - scritti ); // questa in genere funziona

                if ( l > 0 )
                {
                    scritti += l;
                }
                else
                {
                    if ( l == 0 )
                    {
                        break;
                    }
                    else
                    {
                        fclose ( fou );
                        return false;
                    }
                }
            }

            fclose ( fou );
            return true;
        }
        else
        {
            return false;
        }
    }
    catch ( std::exception& ex )
    {
        return false;
    }
}

bool mtfa_FileIo::Write ( const string& nomeFile,
                          const string& val,
                          bool isText )
{
    return mtfa_FileIo::Write ( nomeFile, ( unsigned char* ) val.data(), val.size(), isText );
}

bool mtfa_FileIo::Append ( const string& nomeFile,
                           const string& val,
                           bool isText )
{
    try
    {
#ifdef _WIN32
        FILE* fou = fopen ( nomeFile.c_str(), ( isText ? "at" : "ab" ) );
#else
        FILE* fou = fopen ( nomeFile.c_str(), "a" );
#endif

        if ( fou != NULL )
        {
            int scritti;
            unsigned char* pval = ( unsigned char* ) val.c_str();
            int len = val.size();
            scritti = 0;

            while ( scritti < len )
            {
                int l = write ( fileno ( fou ), &pval[scritti], len - scritti ); // questa in genere funziona

                if ( l > 0 )
                {
                    scritti += l;
                }
                else
                {
                    if ( l == 0 )
                    {
                        break;
                    }
                    else
                    {
                        fclose ( fou );

                        return false;
                    }
                }
            }

            fclose ( fou );
            return true;
        }
        else
        {
            return false;
        }
    }
    catch ( std::exception& ex )
    {
        return false;
    }
}

string mtfa_FileIo::ReadAll ( const string& nomeFile, bool isText )
{
    string s_out;

    try
    {
#ifdef _WIN32
        FILE* fin = fopen ( nomeFile.c_str(), ( isText ? "rt" : "rb" ) );
#else
        FILE* fin = fopen ( nomeFile.c_str(), "r" );
#endif

        if ( fin != NULL )
        {
            struct stat st;
            fstat ( fileno ( fin ), &st );
            int dim_file = st.st_size;
            s_out.resize ( dim_file );
            int letti = 0;

            while ( letti < dim_file )
            {
                int quanti = read ( fileno ( fin ), ( char* ) s_out.data(), dim_file - letti );

                if ( quanti > 0 )
                {
                    letti += quanti;
                }
                else
                {
                    break;
                }
            }

            fclose ( fin );
        }
    }
    catch ( std::exception& ex )
    {
    }

    return s_out;
}

std::list<string> mtfa_FileIo::ReadAllLines ( const string& name )
{
    std::string line;
    std::ifstream input ( name );
    std::list<string> ls;

    while ( std::getline ( input, line ) )
    {
        ls.push_back ( line );
    }

    return ls;
}


#ifdef _WIN32

void mtfa_FileIo::GetFileListing ( std::list<string>& listing,
                                   string directory,
                                   string fileFilter,
                                   bool recursively )
{
    // If we are going to recurse over all the subdirectories, first of all
    // get all the files that are in this directory that match the filter
    if ( recursively )
    {
        GetFileListing ( listing, directory, fileFilter, false );
    }

    directory += "\\";

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Setup the filter according to whether we are getting the directories
    // or just the files
    string filter = directory + ( recursively ? "*" : fileFilter );

    // Find the first file in the directory.
    hFind = FindFirstFile ( filter.c_str(), &FindFileData );

    if ( hFind == INVALID_HANDLE_VALUE )
    {
        DWORD dwError = GetLastError();

        if ( dwError != ERROR_FILE_NOT_FOUND )
        {
            // std::cout << __func__   << "Invalid file handle for filter "<<filter<<". Error is " << GetLastError() << std::endl;
        }
    }
    else
    {
        // Add the first file found to the list
        if ( !recursively )
        {
            listing.push_back ( directory + string ( FindFileData.cFileName ) );
        }

        // List all the other files in the directory.
        while ( FindNextFile ( hFind, &FindFileData ) != 0 )
        {
            if ( !recursively )
            {
                listing.push_back ( directory + string ( FindFileData.cFileName ) );
            }
            else
            {
                // If we found a directory then recurse into it
                if ( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) > 0
                        && ( FindFileData.cFileName[0] != '.' ) )
                {
                    GetFileListing ( listing, directory + string ( FindFileData.cFileName ), fileFilter );
                }
            }
        }

        DWORD dwError = GetLastError();

        FindClose ( hFind );

        if ( dwError != ERROR_NO_MORE_FILES )
        {
            // std::cout << __func__   << "FindNextFile error. Error is "<< dwError << std::endl;
        }
    }
}
#else

#include <dirent.h>

void mtfa_FileIo::GetFileListing ( std::list<string>& listing,
                                   const string& directory,
                                   const string& fileFilter,
                                   bool recursively )
{
    listing.clear();
}
#endif

void mtfa_FileIo::FileInfo ( const string& nomeFile,
                             time_t& c_time,
                             time_t& a_time,
                             time_t& m_time,
                             int& drive_number,
                             bool& isDirectory,
                             long long& size )
{
    int res;

    struct stat st;


    res = stat ( nomeFile.c_str(), &st );

    if ( res != 0 )
    {
        c_time = 0;
        a_time = 0;
        m_time = 0;
        drive_number = -1;
        isDirectory = false;
        size = -1;
    }
    else
    {
        c_time = st.st_ctime;
        a_time = st.st_atime;
        m_time = st.st_mtime;
        drive_number = st.st_dev;
        isDirectory = ( ( st.st_mode & S_IFREG ) != 0 );
        size = st.st_size;
    }

    return;
}

/********************************************************************/
/********************* mtfa_base_bitset *****************************/
/********************************************************************/
pair<int, int> mtfa_base_bitset::GetIdx ( const int p )
{
    unsigned int block, idx;
    block = p / ( 8 * sizeof ( unsigned long long ) );
    idx = p % ( 8 * sizeof ( unsigned long long ) );
//     cout << "IDX: " << p << " blocco: " << block << " indice: " << idx << endl;
    return make_pair ( block, idx );
}

mtfa_base_bitset::mtfa_base_bitset()
{
    std::fill ( &v[0], &v[block_size], 0ull );
}

mtfa_base_bitset::mtfa_base_bitset ( const mtfa_base_bitset& s1 )
{
    std::copy ( &s1.v[0], &s1.v[block_size], &v[0] );
}

mtfa_base_bitset::~mtfa_base_bitset()
{
    std::fill ( &v[0], &v[block_size], 0ull );
}

mtfa_base_bitset& mtfa_base_bitset::set ( const bool val )
{
    if ( val )
        std::fill ( &v[0], &v[block_size], 0xffffffffffffffffull );
    else
        std::fill ( &v[0], &v[block_size], 0x0000000000000000ull );

    return *this;
}

mtfa_base_bitset& mtfa_base_bitset::set ( const unsigned int pos, const bool val )
{
    pair<int, int> bi = GetIdx ( pos );

    if ( val )
        v[bi.first] |= 1ULL << bi.second;
    else
        v[bi.first] &= ~ ( 1ULL << bi.second );

    return *this;
}

mtfa_base_bitset& mtfa_base_bitset::flip()
{
    for ( int i = 0; i < block_size; ++i )
    {
        v[i] ^= 0xFFFFFFFFFFFFFFFFULL;
    }

    return *this;
}

mtfa_base_bitset& mtfa_base_bitset::flip ( const unsigned int pos )
{
    pair<int, int> bi = GetIdx ( pos );
    v[bi.first] ^= 1ULL << bi.second;
    return *this;
}

bool mtfa_base_bitset::test ( const unsigned int pos )
{
    pair<int, int> bi = GetIdx ( pos );
    return (v[bi.first] & (1ULL << bi.second)) != 0;
}

bool mtfa_base_bitset::equal ( const mtfa_base_bitset& s1 )
{
    return std::equal ( &v[0], &v[block_size], &s1.v[0], &s1.v[block_size] );
}

bool mtfa_base_bitset::operator[] ( const int pos )
{
    pair<int, int> bi = GetIdx ( pos );
	//cout << "check for: " << bi.first << ", " << bi.second << " on: " << v[bi.first] << endl;
    return (v[bi.first] & (1ULL << bi.second)) != 0;
}

mtfa_base_bitset& mtfa_base_bitset::operator= ( const mtfa_base_bitset& s )
{
    std::copy ( &s.v[0], &s.v[block_size], &v[0] );
    return *this;
}

mtfa_base_bitset mtfa_base_bitset::operator| ( const mtfa_base_bitset& s1 )
{
    mtfa_base_bitset r ( s1 );

    for ( int i = 0; i < block_size; ++i )
    {
        r.v[i] |= v[i];
    }

    return r;
}

mtfa_base_bitset& mtfa_base_bitset::operator|= ( const mtfa_base_bitset& s1 )
{
    for ( int i = 0; i < block_size; ++i )
    {
        this->v[i] |= s1.v[i];
    }

    return *this;
}

mtfa_base_bitset mtfa_base_bitset::operator& ( const mtfa_base_bitset& s1 )
{
    mtfa_base_bitset r ( s1 );

    for ( int i = 0; i < block_size; ++i )
    {
        r.v[i] &= v[i];
    }

    return r;
}

mtfa_base_bitset& mtfa_base_bitset::operator&= ( const mtfa_base_bitset& s1 )
{
    for ( int i = 0; i < block_size; ++i )
    {
        this->v[i] &= s1.v[i];
    }

    return *this;
}

mtfa_base_bitset mtfa_base_bitset::operator^ ( const mtfa_base_bitset& s1 )
{
    mtfa_base_bitset r ( s1 );

    for ( int i = 0; i < block_size; ++i )
    {
        r.v[i] ^= v[i];
    }

    return r;
}

mtfa_base_bitset& mtfa_base_bitset::operator^= ( const mtfa_base_bitset& s1 )
{
    for ( int i = 0; i < block_size; ++i )
    {
        this->v[i] ^= s1.v[i];
    }

    return *this;
}

mtfa_base_bitset mtfa_base_bitset::operator- ( const mtfa_base_bitset& s1 )
{
    //A-B all items in a and not in b => A & ^B
    mtfa_base_bitset r ( s1 );
    r.flip();               //^B
    r &= *this;             //A &
    return r;
}

mtfa_base_bitset& mtfa_base_bitset::operator-= ( const mtfa_base_bitset& s1 )
{
    mtfa_base_bitset r ( s1 );
    *this &= r.flip();
    return *this;
}


static unsigned long long local_swap64 ( unsigned long long val )
{
#define ZZZZ(x,s,m) (((x) >>(s)) & (m)) | (((x) & (m))<<(s));
    /* val = (((val) >>16) & 0xFFFF0000FFFF) | (((val) & 0xFFFF0000FFFF)<<16); */

    val = ZZZZ ( val, 32, 0x00000000FFFFFFFFull );
    val = ZZZZ ( val, 16, 0x0000FFFF0000FFFFull );
    val = ZZZZ ( val, 8, 0x00FF00FF00FF00FFull );
    val = ZZZZ ( val, 4, 0x0F0F0F0F0F0F0F0Full );
    val = ZZZZ ( val, 2, 0x3333333333333333ull );
    val = ZZZZ ( val, 1, 0x5555555555555555ull );

    return val;
#undef ZZZZ
}

static int local_First1 ( unsigned long long v )
{
    unsigned int r; // Input: bit's desired rank [1-64].
    unsigned int s; // Output: Resulting position of bit with rank r [1-64]
    unsigned long long a, b, c, d; // Intermediate temporaries for bit count.
    unsigned int t; // Bit count temporary.

    if ( v == 0 )
    {
        return 64; //non trovato
    }

    v = local_swap64 ( v );
    r = 1;

    // Do a normal parallel bit count for a 64-bit integer,
    // but store all intermediate steps.
    // a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
    a = v - ( ( v >> 1 ) & ~0UL / 3 );
    // b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
    b = ( a & ~0UL / 5 ) + ( ( a >> 2 ) & ~0UL / 5 );
    // c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
    c = ( b + ( b >> 4 ) ) & ~0UL / 0x11;
    // d = (c & 0x00ff...) + ((c >> 8) & 0x00ff...);
    d = ( c + ( c >> 8 ) ) & ~0UL / 0x101;
    t = ( d >> 32 ) + ( d >> 48 );
    // Now do branchless select!
    s = 64;

    if ( r > t )
    {
        s -= 32;
        r -= t;
    }

    //  s -= ( ( t - r ) & 256 ) >> 3;
    //  r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( d >> ( s - 16 ) ) & 0xff;

    if ( r > t )
    {
        s -= 16;
        r -= t;
    }

    //  s -= ( ( t - r ) & 256 ) >> 4;
    //  r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( c >> ( s - 8 ) ) & 0xf;

    if ( r > t )
    {
        s -= 8;
        r -= t;
    }

    //  s -= ( ( t - r ) & 256 ) >> 5;
    //  r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( b >> ( s - 4 ) ) & 0x7;

    if ( r > t )
    {
        s -= 4;
        r -= t;
    }

    //  s -= ( ( t - r ) & 256 ) >> 6;
    //  r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( a >> ( s - 2 ) ) & 0x3;

    if ( r > t )
    {
        s -= 2;
        r -= t;
    }

    //  s -= ( ( t - r ) & 256 ) >> 7;
    //  r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( v >> ( s - 1 ) ) & 0x1;

    if ( r > t ) s--;

    //  s -= ( ( t - r ) & 256 ) >> 8;
    s = 65 - s - 1;

    return s;
}

// #pragma GCC push_options
// #pragma GCC optimize ("O")
static list<int> GetPosOfBitsSet ( unsigned long long p, int baseidx )
{
    list<int> conta;
    int pos = 0;

    //while ((pos = __builtin_ctzll(p)) != 64)
    while ( ( pos = local_First1 ( p ) ) != 64 )
    {
        conta.push_back ( pos + baseidx );
        p &= ( p - 1 );
    }

    return conta;
}
// #pragma GCC pop_options

// #pragma GCC push_options
// #pragma GCC optimize ("O")
static int GetPosOfFirstBitSet ( unsigned long long p, int baseidx )
{
    //int pos=__builtin_ctzll(p);
    int pos = local_First1 ( p );

    if ( pos == 64 )
        return -1;
    else
        return pos + baseidx;
}
// #pragma GCC pop_options

// #pragma GCC push_options
// #pragma GCC optimize ("O")
static int GetPosOfFirstBitZero ( unsigned long long p, int baseidx )
{
    //int pos=__builtin_ctzll(p ^ 0xFFFFFFFFFFFFFFFFULL);
    int pos = local_First1 ( p ^ 0xFFFFFFFFFFFFFFFFULL );

    if ( pos == 64 )
        return -1;
    else
        return pos + baseidx;
}
// #pragma GCC pop_options

static int local_count ( unsigned long long v )
{
    v = v - ( ( v >> 1 ) & ( unsigned long long ) ~ ( unsigned long long ) 0 / 3 );              // temp
    v = ( v & ( unsigned long long ) ~ ( unsigned long long ) 0 / 15 * 3 ) + ( ( v >> 2 ) & ( unsigned long long ) ~ ( unsigned long long ) 0 / 15 * 3 ); // temp
    v = ( v + ( v >> 4 ) ) & ( unsigned long long ) ~ ( unsigned long long ) 0 / 255 * 15;       // temp
    int c = ( unsigned long long ) ( v * ( ( unsigned long long ) ~ ( unsigned long long ) 0 / 255 ) ) >> ( sizeof ( unsigned long long ) - 1 ) * CHAR_BIT; // count
    return c;
};

int mtfa_base_bitset::count()
{
    int tot = 0;

    for ( int i = 0; i < block_size; ++i )
    {
        //tot+=__builtin_popcountll(v[i]);
        tot += local_count ( v[i] );
    }

    return tot;
}

string mtfa_base_bitset::toString()
{
    char p[block_size * sizeof ( unsigned long long ) * 8];
    memset ( p, '0', sizeof ( p ) );

    for ( int block = 0; block < block_size; ++block )
    {
        list<int> lst = GetPosOfBitsSet ( v[block], 0 );

        for ( auto idx : lst )
        {
            p[block * sizeof ( unsigned long long ) * 8 + idx] = '1';
        }
    }

    string s ( p, &p[block_size * sizeof ( unsigned long long ) * 8] );
    return s;
}

list<int> mtfa_base_bitset::GiveOnes()
{
    list<int> elenco_1;

    for ( int block = 0; block < block_size; ++block )
    {
        list<int> lst = GetPosOfBitsSet ( v[block], block * sizeof ( unsigned long long ) * 8 );
        elenco_1.splice ( elenco_1.end(), lst );
    }

    return elenco_1;
}

int mtfa_base_bitset::FirstOne()
{
    int pos = -1;

    for ( int block = 0; block < block_size; ++block )
    {
        pos = GetPosOfFirstBitSet ( v[block], block * sizeof ( unsigned long long ) * 8 );

        if ( pos != -1 )
            break;
    }

    return pos;
}

int mtfa_base_bitset::FirstZero()
{
    int pos = -1;

    for ( int block = 0; block < block_size; ++block )
    {
        pos = GetPosOfFirstBitZero ( v[block], block * sizeof ( unsigned long long ) * 8 );

        if ( pos != -1 )
            break;
    }

    return pos;
}

void mtfa_base_bitset::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( v, block_size );
}

void mtfa_base_bitset::Deserialize ( mtfa_binary_stream_load* bs )
{
    unsigned int bsize;
    unsigned long long* vnew = bs->Read<unsigned long long> ( bsize );
	//std::copy ( vnew, vnew + bsize * sizeof ( unsigned long long ), &v[0] );
	std::copy ( vnew, vnew + bsize, &v[0] );
	free ( vnew );
}


/********************************************************************/
/************************** mtfa_extended_bitset *****************************/
/********************************************************************/
pair<int, int> mtfa_extended_bitset::GetIdx ( const int p )
{
    unsigned int block, idx;
    block = p / ( mtfa_base_bitset::GetBlockSize() * 64 );
    idx = p % ( mtfa_base_bitset::GetBlockSize() * 64 );
    //     cout << "IDX: " << p << " blocco: " << block << " indice: " << idx << endl;
    return make_pair ( block, idx );
}

void mtfa_extended_bitset::clear()
{
    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            delete v[i];
            v[i] = nullptr;
        }
    }

    v.clear();
    v.resize ( 0 );
}

mtfa_extended_bitset::mtfa_extended_bitset()
{
    clear();
}

mtfa_extended_bitset::mtfa_extended_bitset ( const mtfa_extended_bitset& s1 )
{
    clear();
    v = s1.v;   //prima copio a poi riassegno

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            v[i] = new mtfa_base_bitset ( *v[i] );
        }
    }
}

mtfa_extended_bitset::~mtfa_extended_bitset()
{
    clear();
}

mtfa_extended_bitset& mtfa_extended_bitset::resize ( int size, bool fill_with_1 )
{
    if ( size < v.size() )
    {
        for ( int i = size; i < v.size(); ++i )
        {
            if ( v[i] )
            {
                delete v[i];
                v[i] = nullptr;
            }
        }

        v.resize ( size );
    }
    else
    {
        int oldsize = v.size();
        v.resize ( size );

        for ( int i = oldsize; i < size; ++i )
        {
            if ( fill_with_1 )
            {
                v[i] = new mtfa_base_bitset();
                v[i]->set ( true );
            }
            else
            {
                v[i] = nullptr;
            }
        }
    }

    return *this;
}

mtfa_extended_bitset& mtfa_extended_bitset::set ( const bool val )
{
    //metto tutti gli esistenti a 1 o a 0, fino all'indice finale
    for ( int i = 0; i < v.size(); ++i )
    {
        if ( val )
        {
            if ( v[i] )
            {
                v[i]->set ( true );
            }
            else
            {
                v[i] = new mtfa_base_bitset();
                v[i]->set ( true );
            }
        }
    }

    return *this;
}

mtfa_extended_bitset& mtfa_extended_bitset::set ( const unsigned int pos, const bool val )
{
    pair<int, int> bi = GetIdx ( pos );

    if ( v.size() <= bi.first )
        resize ( bi.first + 1 );

    if ( !v[bi.first] )
        v[bi.first] = new mtfa_base_bitset();

    v[bi.first]->set ( bi.second, val );
    return *this;
}

mtfa_extended_bitset& mtfa_extended_bitset::flip()
{
    for ( int i = 0; i < v.size(); ++i )
    {
        if ( !v[i] )
            v[i] = new mtfa_base_bitset();

        v[i]->flip();
    }

    return *this;
}

mtfa_extended_bitset& mtfa_extended_bitset::flip ( const unsigned int pos )
{
    pair<int, int> bi = GetIdx ( pos );

    if ( v.size() <= bi.first )
        resize ( bi.first + 1 );

    if ( !v[bi.first] )
        v[bi.first] = new mtfa_base_bitset();

    v[bi.first]->flip ( bi.second );
    return *this;
}

bool mtfa_extended_bitset::test ( const unsigned int pos )
{
    pair<int, int> bi = GetIdx ( pos );

    if ( v.size() <= bi.first )
        resize ( bi.first + 1 );

    if ( !v[bi.first] )
        return false;
    else
        return v[bi.first]->test ( bi.second );
}

bool mtfa_extended_bitset::equal ( const mtfa_extended_bitset& s1 )
{
    if ( v.size() != s1.v.size() )
        return false;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            if ( s1.v[i] )
            {
                if ( ! ( v[i]->equal ( *s1.v[i] ) ) )
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if ( s1.v[i] )
                return false;
        }
    }

    return true;
}

bool mtfa_extended_bitset::operator[] ( const int pos )
{
    pair<int, int> bi = GetIdx ( pos );

    if ( v[bi.first] )
        return ( *v[bi.first] ) [bi.second];
    else
        return false;
}

mtfa_extended_bitset& mtfa_extended_bitset::operator= ( const mtfa_extended_bitset& s1 )
{
    clear();
    v = s1.v;   //prima copio a poi riassegno

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            v[i] = new mtfa_base_bitset ( *v[i] );
        }
    }

    return *this;
}

mtfa_extended_bitset mtfa_extended_bitset::operator| ( const mtfa_extended_bitset& s1 )
{
    //per prima cosa, chi è il maggiore?
    mtfa_extended_bitset r;
    const mtfa_extended_bitset* other;

    if ( v.size() > s1.v.size() )
    {
        r = s1;
        r.resize ( v.size(), false );
        other = this;
    }
    else
    {
        r = *this;
        r.resize ( s1.v.size(), false );
        other = &s1;
    }

    //ora hanno la stessa dimensione
    for ( int i = 0; i < r.v.size(); ++i )
    {
        if ( r.v[i] )
        {
            if ( other->v[i] )
            {
                *r.v[i] |= *other->v[i];
            }
            else
            {
                //nulla, other->v[i] sta a 0 e quindi vince r.v[i]
            }
        }
        else
        {
            if ( other->v[i] )
            {
                r.v[i] = new mtfa_base_bitset ( *other->v[i] );
            }
            else
            {
                //nulla, sono tutti e due a 0
            }
        }
    }

    return r;
}

mtfa_extended_bitset& mtfa_extended_bitset::operator|= ( const mtfa_extended_bitset& s1 )
{
    mtfa_extended_bitset r = *this | s1;
    clear();
    v = move ( r.v );
    return *this;
}

mtfa_extended_bitset mtfa_extended_bitset::operator& ( const mtfa_extended_bitset& s1 )
{
    //per prima cosa, chi è il maggiore?
    mtfa_extended_bitset r;
    const mtfa_extended_bitset* other;

    if ( v.size() > s1.v.size() )
    {
        r = s1;
        r.resize ( v.size(), false );
        other = this;
    }
    else
    {
        r = *this;
        r.resize ( s1.v.size(), false );
        other = &s1;
    }

    //ora hanno la stessa dimensione. Qui faccio AND
    for ( int i = 0; i < r.v.size(); ++i )
    {
        if ( r.v[i] )
        {
            if ( other->v[i] )
            {
                *r.v[i] &= *other->v[i];
            }
            else
            {
                //Vince chi sta a 0
                delete r.v[i];
                r.v[i] = nullptr;
            }
        }
        else
        {
            //se r sta a 0, vince
        }
    }

    return r;
}

mtfa_extended_bitset& mtfa_extended_bitset::operator&= ( const mtfa_extended_bitset& s1 )
{
    mtfa_extended_bitset r = *this & s1;
    clear();
    v = move ( r.v );
    return *this;
}

mtfa_extended_bitset mtfa_extended_bitset::operator^ ( const mtfa_extended_bitset& s1 )
{
    //per prima cosa, chi è il maggiore?
    mtfa_extended_bitset r;
    const mtfa_extended_bitset* other;

    if ( v.size() > s1.v.size() )
    {
        r = s1;
        r.resize ( v.size(), false );
        other = this;
    }
    else
    {
        r = *this;
        r.resize ( s1.v.size(), false );
        other = &s1;
    }

    //ora hanno la stessa dimensione. ora lo XOR
    for ( int i = 0; i < r.v.size(); ++i )
    {
        if ( r.v[i] )
        {
            if ( other->v[i] )
            {
                *r.v[i] ^= *other->v[i];
            }
            else
            {
                //nulla, other->v[i] sta a 0 e quindi vince r.v[i]
            }
        }
        else
        {
            if ( other->v[i] )
            {
                r.v[i] = new mtfa_base_bitset ( *other->v[i] );
            }
            else
            {
                //nulla, sono tutti e due a 0
            }
        }
    }

    return r;
}

mtfa_extended_bitset& mtfa_extended_bitset::operator^= ( const mtfa_extended_bitset& s1 )
{
    mtfa_extended_bitset r = *this ^ s1;
    clear();
    v = move ( r.v );
    return *this;
}

mtfa_extended_bitset mtfa_extended_bitset::operator- ( const mtfa_extended_bitset& s1 )
{
    //A-B all items in a and not in b => A & ^B
    mtfa_extended_bitset r ( s1 );
    r.flip();               //^B
    r &= *this;             //A &
    return r;
}

mtfa_extended_bitset& mtfa_extended_bitset::operator-= ( const mtfa_extended_bitset& s1 )
{
    mtfa_extended_bitset r ( s1 );
    *this &= r.flip();
    return *this;
}
int mtfa_extended_bitset::count()
{
    int tot = 0;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
            tot += v[i]->count();
    }

    return tot;
}

string mtfa_extended_bitset::toString()
{
    string s;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            s += v[i]->toString();
        }
        else
        {
            s += string ( mtfa_base_bitset::GetBlockSize() * 64, '0' );
        }
    }

    return s;
}

int mtfa_extended_bitset::size()
{
    return mtfa_base_bitset::GetBlockSize() * 64 * v.size();
}

list<int> mtfa_extended_bitset::GiveOnes()
{
    int bsize = mtfa_base_bitset::GetBlockSize() * 64;
    list<int> elenco_1;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            auto res = v[i]->GiveOnes();
            for_each ( res.begin(), res.end(), [i, bsize] ( int& val )
            {
                val += i * bsize;
            } );
            elenco_1.splice ( elenco_1.end(), res );
        }
    }

    return elenco_1;
}

int mtfa_extended_bitset::FirstOne()
{
    int pos = -1;
    int bsize = mtfa_base_bitset::GetBlockSize() * 64;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( v[i] )
        {
            pos = v[i]->FirstOne();

            if ( pos >= 0 )
                return pos + i * bsize;
        }
    }

    return -1;
}

int mtfa_extended_bitset::FirstZero()
{
    int pos = -1;
    int bsize = mtfa_base_bitset::GetBlockSize() * 64;

    for ( int i = 0; i < v.size(); ++i )
    {
        if ( !v[i] )
        {
            return i * bsize;
        }
        else
        {
            pos = v[i]->FirstZero();

            if ( pos >= 0 )
                return pos + i * bsize;
        }
    }

    return -1;
}

void mtfa_extended_bitset::Serialize ( mtfa_binary_stream_save* bs )
{
    int size = v.size();
    bs->Append ( size );

    for ( int i = 0; i < size; ++i )
    {
        if ( v[i] )
        {
            unsigned char presente = 1;
            bs->Append ( presente );
            v[i]->Serialize ( bs );
        }
        else
        {
            unsigned char presente = 0;
            bs->Append ( presente );
        }
    }
}

void mtfa_extended_bitset::Deserialize ( mtfa_binary_stream_load* bs )
{
    clear();
    int size;
    size = bs->Read<int>();
    resize ( size );

    for ( int i = 0; i < size; ++i )
    {
        unsigned char presente = bs->Read<unsigned char>();

        if ( presente )
        {
            v[i] = new mtfa_base_bitset();
            v[i]->Deserialize ( bs );
        }
        else
            v[i] = nullptr;
    }
}


/********************************************************************/
/************************** mtfa_bitset *****************************/
/********************************************************************/

int mtfa_bitset::R64 ( int n )
{
    if ( ( n % 64 ) != 0 )
    {
        return ( n / 64 + 1 ) * 64;
    }
    else
    {
        return n;
    }
}

void mtfa_bitset::Zero ( int p1, int p2 )
{
    unsigned long long* p = &v[p1];
    unsigned long long* pf = &v[p2];

    while ( p != pf )
    {
        *p++ = 0ULL;
    }
}

bool mtfa_bitset::initialized = false;
unsigned long long mtfa_bitset::bits[64];
unsigned long long mtfa_bitset::nbits[64];

void mtfa_bitset::Initialize()
{
    if ( !mtfa_bitset::initialized )
    {
        mtfa_bitset::initialized = true;
        unsigned long long bit = 1;

        for ( int i = 0; i < 64; ++i )
        {
            mtfa_bitset::bits[i] = bit;
            mtfa_bitset::nbits[i] = bit ^ 0xFFFFFFFFFFFFFFFF;
            bit <<= 1;
        }
    }
}

void mtfa_bitset::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( real_size );
    unsigned int ll = allocated;
    bs->Append ( ( unsigned long long* ) v, ll );
}

//#include <bitset>

void mtfa_bitset::Deserialize ( mtfa_binary_stream_load* bs )
{
    real_size = bs->Read<unsigned int>();
    unsigned int ll;
    * ( unsigned long long** ) &v = bs->Read<unsigned long long> ( ll );
    allocated = ll;

    for ( int pos = real_size; pos < ( allocated << 6 ); ++pos )
    {
        int where = pos >> 6; /// 64;
        int which = pos & 0x0000003F; //% 64;
        v[where] &= nbits[which];
    }
}

///Ricorda che n sono bit!!

void mtfa_bitset::Init ( unsigned int n )
{
    real_size = 0;
    n = R64 ( n );

    allocated = n >> 6;
    v = ( unsigned long long* ) malloc ( allocated * sizeof ( unsigned long long ) );
    Zero ( 0, allocated );
    mtfa_bitset::Initialize();
}

mtfa_bitset::mtfa_bitset()
{
    real_size = 0;
    Init ( allocation_block );
}

mtfa_bitset::mtfa_bitset ( mtfa_bitset& sin )
{
    Init ( allocation_block );
    reset();
    resize ( sin.size() );
    real_size = sin.real_size;

    unsigned long long* d = &v[0];
    unsigned long long* p = &sin.v[0];
    unsigned long long* pf = &sin.v[allocated];

    while ( p != pf )
    {
        *d++ = *p++;
    }
}

mtfa_bitset::~mtfa_bitset()
{
    if ( allocated > 0 && v != nullptr )
    {
        free ( v );
    }

    v = nullptr;
    allocated = 0;
    real_size = 0;
}

//Ricorda che n è espresso in bit!

mtfa_bitset::mtfa_bitset ( unsigned int n )
{
    Init ( n );
}

void mtfa_bitset::reset()
{
    Zero ( 0, allocated );
}

//Ricorda che n e' in bit

void mtfa_bitset::resize ( int n )
{
    //Rialloco!
    real_size = n;
    n = R64 ( n );

    v = ( unsigned long long* ) realloc ( v, ( n >> 6 ) * sizeof ( unsigned long long ) );

    if ( ( n >> 6 ) > allocated )
    {
        Zero ( allocated, n >> 6 );
    }

    allocated = n >> 6;
}

unsigned int mtfa_bitset::size()
{
    return real_size;
}

void mtfa_bitset::set ( unordered_set<int> positions, bool val )
{
    if ( positions.empty() )
    {
        return;
    }

    int maxset = * ( positions.begin()++ );

    for ( auto it : positions )
        if ( it > maxset )
        {
            maxset = it;
        }

    set ( maxset, val ); //in questo modo ne faccio la resize

    //ora, elemento per elemento, calcolo cosa mettere a 1
    for ( auto pos : positions )
    {
        if ( pos != maxset )
        {
            int where = pos >> 6; /// 64;
            int which = pos & 0x0000003F; //% 64;

            if ( val )
            {
                v[where] |= bits[which];
            }
            else
            {
                v[where] &= ~bits[which];
            }
        }
    }
}

void mtfa_bitset::set()
{
    //Mette tutto a 1
    unsigned long long* p = &v[0];
    unsigned long long* pf = &v[allocated];

    while ( p != pf )
    {
        *p++ = 0xFFFFFFFFFFFFFFFFULL;
    }
}

void mtfa_bitset::set ( unsigned int pos, bool val )
{
    int bit = 0;

    if ( val )
    {
        bit = 1;
    }

    if ( pos >= ( allocated << 6 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 6; /// 64;
    int which = pos & 0x0000003F; //% 64;

    if ( val )
    {
        v[where] |= bits[which];
    }
    else
    {
        v[where] &= ~bits[which];
    }
}

mtfa_bitset& mtfa_bitset::flip()
{
    //Flippa tutto!
    unsigned long long* p = &v[0];
    unsigned long long* pf = &v[allocated];

    while ( p != pf )
    {
        *p++ ^= 0xFFFFFFFFFFFFFFFFULL;
    }

    for ( int pos = real_size; pos < ( allocated << 6 ); ++pos )
    {
        int where = pos >> 6; /// 64;
        int which = pos & 0x0000003F; //% 64;
        v[where] &= nbits[which];
    }

    return *this;
}

void mtfa_bitset::flip ( unsigned int pos )
{
    if ( pos >= ( allocated << 6 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 6; /// 64;
    int which = pos & 0x0000003F; //% 64;
    v[where] ^= bits[which];
}

bool mtfa_bitset::test ( unsigned int pos )
{
    if ( pos >= ( allocated << 6 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 6; /// 64;
    int which = pos & 0x0000003F; //% 64;
    return ( v[where] & bits[which] ) != 0;
}

bool mtfa_bitset::cmp ( mtfa_bitset& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    unsigned long long* d = &v[0];
    unsigned long long* p = &s1.v[0];
    unsigned long long* pf = &s1.v[allocated];

    while ( p != pf )
    {
        if ( *d++ != *p++ )
        {
            return false;
        }
    }

    return true;
}

mtfa_bitset& mtfa_bitset::operator= ( mtfa_bitset& s )
{
    this->reset();
    this->resize ( s.size() );

    unsigned long long* d = &this->v[0];
    unsigned long long* p = &s.v[0];
    unsigned long long* pf = &s.v[allocated];

    while ( p != pf )
    {
        *d++ = *p++;
    }

    return *this;
}

bool mtfa_bitset::operator[] ( int pos )
{
    return ( test ( pos ) );
}

mtfa_bitset& mtfa_bitset::operator| ( mtfa_bitset& s1 )
{
    mtfa_bitset* op = new mtfa_bitset();
    *op = *this;
    return *op |= s1;
}

mtfa_bitset& mtfa_bitset::operator|= ( mtfa_bitset& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    unsigned long long* d = &v[0];
    unsigned long long* p = &s1.v[0];
    unsigned long long* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ |= *p++;
    }

    return *this;
}

// int mtfa_bitset::FirstCommonOneOrZero ( vector<mtfa_bitset*> &bsv, bool findOne )
// {
//      int maxlen = bsv[0]->allocated;
//      bool different = false;
//      for ( auto it : bsv ) {
//           if ( maxlen < it->allocated ) {
//                different = true;
//                maxlen = it->allocated;
//           }
//      }
//
//      int bsvlen = bsv.size();
//      if ( findOne ) {
//           //cerco il primo 1 comune a tutte
//           for ( int pos = 0; pos < maxlen; ++pos ) {
//                unsigned long long ull = 0xFFFFFFFFFFFFFFFF;
//                for ( int i = 0; i < bsvlen; ++i ) {
//                     if ( bsv[i]->allocated <= pos ) {
//                          return -1;     //non l'ho trovato
//                     }
//                     ull &= bsv[i]->v[pos];
//                }
//                if ( ull ) {
//                     return __builtin_ctzll ( ull ) - 1 + 64 * pos;     //torna 64 se 0, torna 1 se 1, torna 2 se 2 .... fino a 63
//                }
//           }
//           return -1;
//      } else {
//           //cerco il primo 0 comune a tutte
//           for ( int pos = 0; pos < maxlen; ++pos ) {
//                const unsigned long long mask = 0xFFFFFFFFFFFFFFFF;
//                unsigned long long ull = 0;
//                for ( int i = 0; i < bsvlen; ++i ) {
//                     if ( bsv[i]->allocated <= pos ) {
//                          /*NOP*/; //qui tutto bene
//                     } else {
//                          ull |= bsv[i]->v[pos];
//                     }
//                }
//                if ( ull != mask ) {
//                     return __builtin_ctzll ( ull^mask ) - 1 + 64 * pos;     //torna 64 se 0, torna 1 se 1, torna 2 se 2 .... fino a 63
//                }
//           }
//           return -1;
//      }
// }

mtfa_bitset mtfa_bitset::operator& ( mtfa_bitset& s1 )
{
    mtfa_bitset* op = new mtfa_bitset();
    *op = *this;
    *op &= s1;
    return *op;
}

mtfa_bitset& mtfa_bitset::operator&= ( mtfa_bitset& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    unsigned long long* d = &v[0];
    unsigned long long* p = &s1.v[0];
    unsigned long long* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ &= *p++;
    }

    return *this;
}

// bool mtfa_bitset::HaveCommonItems ( mtfa_bitset & s1, mtfa_bitset & s2 )
// {
//      int minsize = ( s1.allocated < s2.allocated ) ? s1.allocated : s2.allocated;
//      for ( int i = 0; i < minsize; ++i ) {
//           if ( s1.v[i] & s2.v[i] ) {
//                return true;
//           }
//      }
//      return false;
// }

mtfa_bitset mtfa_bitset::operator^ ( mtfa_bitset& s1 )
{
    mtfa_bitset* op = new mtfa_bitset();
    *op = *this;
    return *op ^= s1;
}

mtfa_bitset& mtfa_bitset::operator^= ( mtfa_bitset& s1 )
{
    int size;
    unsigned int dsize = s1.size();
    unsigned int ssize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        this->resize ( size );
    }
    else
    {
        size = ssize;
        s1.resize ( size );
    }

    unsigned long long* d = &v[0];
    unsigned long long* p = &s1.v[0];
    unsigned long long* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ ^= *p++;
    }

    return *this;
}

mtfa_bitset mtfa_bitset::operator- ( mtfa_bitset& s1 )
{
    mtfa_bitset* op = new mtfa_bitset();
    *op = *this;
    return *op -= s1;
}

mtfa_bitset& mtfa_bitset::operator-= ( mtfa_bitset& s1 )
{
    int size;
    unsigned int dsize = s1.size();
    unsigned int ssize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        this->resize ( size );
    }
    else
    {
        size = ssize;
        s1.resize ( size );
    }

    unsigned long long* d = &this->v[0];
    unsigned long long* p = &s1.v[0];
    unsigned long long* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ &= ~ ( *p++ );
    }

    for ( int pos = real_size; pos < ( allocated << 6 ); ++pos )
    {
        int where = pos >> 6; /// 64;
        int which = pos & 0x0000003F; //% 64;
        v[where] &= nbits[which];
    }

    return *this;
}

int mtfa_bitset::count()
{
    int tot = 0;

    for ( int i = 0; i < allocated; ++i )
    {
        unsigned long long p = v[i];

        while ( p != 0 )
        {
            p &= ( p - 1 );
            tot++;
        }
    }

    return tot;
}

unsigned int find_32 ( unsigned int n )
{
    unsigned long idx;
#ifdef _WIN32
    unsigned char iRet = _BitScanForward ( &idx, n );

    if ( iRet )
    {
        return idx + 1;
    }
    else
    {
        return 0;
    }

#else

    if ( n == 0 )
    {
        return 0;
    }

    idx = __builtin_ctz ( n );
    return idx + 1;
#endif
}

unsigned int find_64 ( unsigned long long n )
{
    if ( n == 0 )
    {
        return 0;
    }

#ifdef _DEBUG
    unsigned int c = find_32 ( ( unsigned int ) ( n & 0x00000000FFFFFFFF ) ); //(unsigned int)(n & 0x00000000FFFFFFFF)); //the lower 32 bits
#else
    unsigned int c = find_32 ( n ); //(unsigned int)(n & 0x00000000FFFFFFFF)); //the lower 32 bits
#endif

    if ( c )
    {
        return c;
    }

    return find_32 ( n >> 32 ) + 32; //the higher 32 bits
}

///Torna un array di interi che termina con -1;

int* mtfa_bitset::GiveOnes()
{
    std::list<int> li;

    for ( int i = 0; i < allocated; ++i )
    {
        li.splice ( li.end(), GetPosOfBitsSet ( v[i], i << 6 ) );
    }

#if 0

    for ( int i = 0; i < allocated; ++i )
    {
        unsigned int pos = 0;
        unsigned long long p = v[i] & 0x00000000FFFFFFFF;

        while ( p )
        {
            unsigned long idx;
#ifdef _WIN32
            unsigned char iRet = _BitScanForward ( &idx, p );

            if ( iRet )
            {
                p ^= ( 1ULL << ( idx + 0 ) );
                li.push_back ( idx + i * sizeof ( unsigned long long ) * 8 );
            }
            else
            {
                break;
            }

#else
            idx = __builtin_ctz ( p );
            p ^= ( 1ULL << ( idx + 0 ) );
            li.push_back ( idx + i * sizeof ( unsigned long long ) * 8 );
#endif
        }

        p = v[i] >> 32;

        while ( p )
        {
            unsigned long idx;
#ifdef _WIN32
            unsigned char iRet = _BitScanForward ( &idx, p );

            if ( iRet )
            {
                p ^= ( 1ULL << ( idx + 0 ) );
                li.push_back ( idx + i * sizeof ( unsigned long long ) * 8 + 32 );
            }
            else
            {
                break;
            }

#else
            idx = __builtin_ctz ( p );
            p ^= ( 1ULL << ( idx + 0 ) );
            li.push_back ( idx + i * sizeof ( unsigned long long ) * 8 + 32 );
#endif
        }
    }

#endif

    int* vint = ( int* ) malloc ( ( li.size() + 1 ) * sizeof ( int ) );
    std::copy ( li.begin(), li.end(), vint );
    vint[li.size()] = -1;
    return vint;
}

static unsigned long long swap64 ( unsigned long long val )
{
#define ZZZZ(x,s,m) (((x) >>(s)) & (m)) | (((x) & (m))<<(s));
    /* val = (((val) >>16) & 0xFFFF0000FFFF) | (((val) & 0xFFFF0000FFFF)<<16); */

    val = ZZZZ ( val, 32, 0x00000000FFFFFFFFull );
    val = ZZZZ ( val, 16, 0x0000FFFF0000FFFFull );
    val = ZZZZ ( val, 8, 0x00FF00FF00FF00FFull );
    val = ZZZZ ( val, 4, 0x0F0F0F0F0F0F0F0Full );
    val = ZZZZ ( val, 2, 0x3333333333333333ull );
    val = ZZZZ ( val, 1, 0x5555555555555555ull );

    return val;
#undef ZZZZ
}

static int First1 ( unsigned long long v )
{
    unsigned int r; // Input: bit's desired rank [1-64].
    unsigned int s; // Output: Resulting position of bit with rank r [1-64]
    unsigned long long a, b, c, d; // Intermediate temporaries for bit count.
    unsigned int t; // Bit count temporary.

    if ( v == 0 )
    {
        return 0; //non trovato
    }

    v = swap64 ( v );
    r = 1;

    // Do a normal parallel bit count for a 64-bit integer,
    // but store all intermediate steps.
    // a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
    a = v - ( ( v >> 1 ) & ~0UL / 3 );
    // b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
    b = ( a & ~0UL / 5 ) + ( ( a >> 2 ) & ~0UL / 5 );
    // c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
    c = ( b + ( b >> 4 ) ) & ~0UL / 0x11;
    // d = (c & 0x00ff...) + ((c >> 8) & 0x00ff...);
    d = ( c + ( c >> 8 ) ) & ~0UL / 0x101;
    t = ( d >> 32 ) + ( d >> 48 );
    // Now do branchless select!
    s = 64;
    //if (r > t) {s -= 32; r -= t;}
    s -= ( ( t - r ) & 256 ) >> 3;
    r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( d >> ( s - 16 ) ) & 0xff;
    //if (r > t) {s -= 16; r -= t;}
    s -= ( ( t - r ) & 256 ) >> 4;
    r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( c >> ( s - 8 ) ) & 0xf;
    //if (r > t) {s -= 8; r -= t;}
    s -= ( ( t - r ) & 256 ) >> 5;
    r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( b >> ( s - 4 ) ) & 0x7;
    //if (r > t) {s -= 4; r -= t;}
    s -= ( ( t - r ) & 256 ) >> 6;
    r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( a >> ( s - 2 ) ) & 0x3;
    //if (r > t) {s -= 2; r -= t;}
    s -= ( ( t - r ) & 256 ) >> 7;
    r -= ( t & ( ( t - r ) >> 8 ) );
    t = ( v >> ( s - 1 ) ) & 0x1;
    //if (r > t) s--;
    s -= ( ( t - r ) & 256 ) >> 8;
    s = 65 - s;

    return s;
}

// void CheckBits(unsigned long long v1, unsigned long long howmany)
// {
//     unsigned long long start;
//     unsigned long long tot = 0;
//
//     unsigned long long v;
//     v = v1;
//     start = GetULLTime();
//     for (int i=0; i<howmany; ++i)
//     {
//         tot+= First1(v++);
//     }
//     cout << "First1             : " << howmany << " cicli: " << GetULLTime() - start << " usec. " << tot << endl;
//
//     v = v1;
//     start = GetULLTime();
//     for (int i=0; i<howmany; ++i)
//     {
//         tot+= GetPosOfFirstBitSet(v++, 0);
//     }
//     cout << "GetPosOfFirstBitSet: " << howmany << " cicli: " << GetULLTime() - start << " usec. " << tot << endl;
//
//     v = v1;
//     start = GetULLTime();
//     for (int i=0; i<howmany; ++i)
//     {
//         unsigned long long nv = ~v++;
//         tot+= First1(nv);
//     }
//     cout << "First0             : " << howmany << " cicli: " << GetULLTime() - start << " usec. " << tot << endl;
//
//     v = v1;
//     start = GetULLTime();
//     for (int i=0; i<howmany; ++i)
//     {
//         tot+= GetPosOfFirstBitZero(v++, 0);
//     }
//     cout << "GetPosOfFirstBitZero: " << howmany << " cicli: " << GetULLTime() - start << " usec. " << tot << endl;
//
//     cout << "First 0              di 8: " << First1(~8) << endl;
//     cout << "First 0              di 7: " << First1(~7) << endl;
//     cout << "GetPosOfFirstBitZero di 8: " << GetPosOfFirstBitZero(8, 0) << endl;
//     cout << "GetPosOfFirstBitZero di 7: " << GetPosOfFirstBitZero(7, 0) << endl;
//
//     cout << "First 1             di 8: " << First1(8) << endl;
//     cout << "First 1             di 7: " << First1(7) << endl;
//     cout << "GetPosOfFirstBitSet di 8: " << GetPosOfFirstBitSet(8, 0) << endl;
//     cout << "GetPosOfFirstBitSet di 7: " << GetPosOfFirstBitSet(7, 0) << endl;
//
//     cout << "First 0              di FF: " << First1(~0xFFFFFFFFFFFFFFFF) << endl;
//     cout << "First 0              di EF: " << First1(~0xEFFFFFFFFFFFFFFF) << endl;
//     cout << "GetPosOfFirstBitZero di FF: " << GetPosOfFirstBitZero(0xFFFFFFFFFFFFFFFF, 0) << endl;
//     cout << "GetPosOfFirstBitZero di EF: " << GetPosOfFirstBitZero(0xEFFFFFFFFFFFFFFF, 0) << endl;
//
//     cout << "First 1             di 0: " << First1(0) << endl;
//     cout << "First 1             di 80: " << First1(0x8000000000000000) << endl;
//     cout << "GetPosOfFirstBitSet di 0: " << GetPosOfFirstBitSet(0, 0) << endl;
//     cout << "GetPosOfFirstBitSet di 80: " << GetPosOfFirstBitSet(0x8000000000000000, 0) << endl;
// }

int mtfa_bitset::FirstOne()
{
    unsigned long long* ptmp = v;
    unsigned long long p;
    int pos;

    for ( int i = 0; i < allocated; ++i )
    {
        //p = v[i];
        p = ( *ptmp++ );

        if ( p )
        {
            pos = GetPosOfFirstBitSet ( p, 0 ); // First1 ( p );
            return ( i << 6 ) + pos; // - 1; //da 0 in poi
        }
    }

    return -1; //not found
}

int mtfa_bitset::FirstZero()
{
    unsigned long long* ptmp = v;
    unsigned long long p;
    int pos;

    for ( int i = 0; i < allocated; ++i )
    {
        p = ~ ( *ptmp++ );

        if ( p )
        {
            pos = GetPosOfFirstBitSet ( p, 0 ); //First1 ( p );
            return ( i << 6 ) + pos; // - 1; //da 0 in poi
        }
    }

    return -1; //not found
}

string mtfa_bitset::toString()
{
    int l = size();
    char* p = ( char* ) malloc ( l );
    memset ( p, '0', l );
    int i = 0;

    for ( i = 0; i < l; ++i )
        if ( test ( i ) )
        {
            p[i] = '1';
        }

    string s ( p, &p[l] );
    free ( p );
    return s;
}

/***********************************************************************/
/***********************************************************************/
/************************** mtfa_bitset_1024 ***************************/
/***********************************************************************/
/***********************************************************************/

int mtfa_bitset_1024::R1024 ( int n )
{
    if ( ( n % 1024 ) != 0 )
    {
        return ( n / 1024 + 1 ) * 1024;
    }
    else
    {
        return n;
    }
}

void mtfa_bitset_1024::Zero ( int p1, int p2 )
{
    uint1024_t * p = &v[p1];
    uint1024_t * pf = &v[p2];

    while ( p != pf )
    {
        *p++ = uint1024_t(0);
    }
}

bool mtfa_bitset_1024::initialized = false;
uint1024_t mtfa_bitset_1024::bits[1024];
uint1024_t mtfa_bitset_1024::nbits[1024];

void mtfa_bitset_1024::Initialize()
{
    if ( !mtfa_bitset_1024::initialized )
    {
        mtfa_bitset_1024::initialized = true;

        for ( int i = 0; i < 1024; ++i )
        {
            mtfa_bitset_1024::bits[i] = one << i;
            mtfa_bitset_1024::nbits[i] = ones ^ (one << i);
        }
    }
}

void mtfa_bitset_1024::Serialize ( mtfa_binary_stream_save* bs )
{
//     bs->Append ( real_size );
//     unsigned int ll = allocated;
//     bs->Append ( ( unsigned long long* ) v, ll );
}

void mtfa_bitset_1024::Deserialize ( mtfa_binary_stream_load* bs )
{
//     real_size = bs->Read<unsigned int>();
//     unsigned int ll;
//     * ( unsigned long long** ) &v = bs->Read<unsigned long long> ( ll );
//     allocated = ll;
// 
//     for ( int pos = real_size; pos < ( allocated << 6 ); ++pos )
//     {
//         int where = pos >> 6; /// 64;
//         int which = pos & 0x0000003F; //% 64;
//         v[where] &= nbits[which];
//     }
}

void mtfa_bitset_1024::Init ( unsigned int n )
{
    one = 1;
    ones = (one << 1024) - 1;
    real_size = 0;
    n = R1024 ( n ); //nel senso che le dimensioni in bit sono multiple di 1024

    allocated = n >> 10;  //quanti da 1024 devo allocare?
    v = ( uint1024_t* ) malloc ( allocated * sizeof ( uint1024_t ) );
    Zero ( 0, allocated );
    mtfa_bitset_1024::Initialize();
}

mtfa_bitset_1024::mtfa_bitset_1024()
{
    real_size = 0;
    Init ( allocation_block );
}

mtfa_bitset_1024::mtfa_bitset_1024 ( mtfa_bitset_1024& sin )
{
    Init ( allocation_block );
    reset();
    resize ( sin.size() );
    real_size = sin.real_size;

    uint1024_t* d = &v[0];
    uint1024_t* p = &sin.v[0];
    uint1024_t* pf = &sin.v[allocated];

    while ( p != pf )
    {
        *d++ = *p++;
    }
}

mtfa_bitset_1024::~mtfa_bitset_1024()
{
    if ( allocated > 0 && v != nullptr )
    {
        free ( v );
    }

    v = nullptr;
    allocated = 0;
    real_size = 0;
}

mtfa_bitset_1024::mtfa_bitset_1024 ( unsigned int n )
{
    Init ( n );
}

void mtfa_bitset_1024::reset()
{
    Zero ( 0, allocated );
}

void mtfa_bitset_1024::resize ( int n )
{
    //Rialloco!
    real_size = n;
    n = R1024 ( n );

    v = ( uint1024_t * ) realloc ( v, ( n >> 10 ) * sizeof ( uint1024_t ) );

    if ( ( n >> 10 ) > allocated )
    {
        Zero ( allocated, n >> 10 );
    }

    allocated = n >> 10;
}

unsigned int mtfa_bitset_1024::size()
{
    return real_size;
}

void mtfa_bitset_1024::set ( unordered_set<int> positions, bool val )
{
    if ( positions.empty() )
    {
        return;
    }

    int maxset = * ( positions.begin()++ );

    for ( auto it : positions )
        if ( it > maxset )
        {
            maxset = it;
        }

    set ( maxset, val ); //in questo modo ne faccio la resize

    //ora, elemento per elemento, calcolo cosa mettere a 1
    for ( auto pos : positions )
    {
        if ( pos != maxset )
        {
            int where = pos >> 10;
            int which = pos & 0x000003FF;

            if ( val )
            {
                v[where] |= bits[which];
            }
            else
            {
                v[where] &= ~bits[which];
            }
        }
    }
}

void mtfa_bitset_1024::set()
{
    //Mette tutto a 1
    uint1024_t* p = &v[0];
    uint1024_t* pf = &v[allocated];

    while ( p != pf )
    {
        *p++ = ones;
    }
}

void mtfa_bitset_1024::set ( unsigned int pos, bool val )
{
    if ( pos >= ( allocated << 10 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 10;
    int which = pos & 0x000003FF;

    if ( val )
    {
        bit_set(v[where], which);
        //v[where] |= bits[which];
    }
    else
    {
        bit_unset(v[where], which);
        //v[where] &= ~bits[which];
    }
}

mtfa_bitset_1024& mtfa_bitset_1024::flip()
{
    //Flippa tutto!
    uint1024_t* p = &v[0];
    uint1024_t* pf = &v[allocated];

    while ( p != pf )
    {
        *p++ ^= ones;
    }

//     for ( int pos = real_size; pos < ( allocated << 10 ); ++pos )
//     {
//         int where = pos >> 10;
//         int which = pos & 0x000003FF;
//         v[where] &= nbits[which];
//     }

    return *this;
}

void mtfa_bitset_1024::flip ( unsigned int pos )
{
    if ( pos >= ( allocated << 10 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 10;
    int which = pos & 0x000003FF;
    //v[where] ^= bits[which];
    bit_flip(v[where], which);
}

bool mtfa_bitset_1024::test ( unsigned int pos )
{
    if ( pos >= ( allocated << 10 ) )
    {
        resize ( pos + allocation_block );
    }

    if ( pos >= real_size )
    {
        real_size = pos + 1;
    }

    int where = pos >> 10;
    int which = pos & 0x000003FF;
    //return ( v[where] & bits[which] ) != 0;
    return bit_test(v[where], which);
}

bool mtfa_bitset_1024::cmp ( mtfa_bitset_1024& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    uint1024_t* d = &v[0];
    uint1024_t* p = &s1.v[0];
    uint1024_t* pf = &s1.v[allocated];

    while ( p != pf )
    {
        if ( *d++ != *p++ )
        {
            return false;
        }
    }

    return true;
}

mtfa_bitset_1024& mtfa_bitset_1024::operator= ( mtfa_bitset_1024& s )
{
    this->reset();
    this->resize ( s.size() );

    uint1024_t* d = &this->v[0];
    uint1024_t* p = &s.v[0];
    uint1024_t* pf = &s.v[allocated];

    while ( p != pf )
    {
        *d++ = *p++;
    }

    return *this;
}

bool mtfa_bitset_1024::operator[] ( int pos )
{
    return ( test ( pos ) );
}

mtfa_bitset_1024& mtfa_bitset_1024::operator| ( mtfa_bitset_1024& s1 )
{
    mtfa_bitset_1024* op = new mtfa_bitset_1024();
    *op = *this;
    return *op |= s1;
}

mtfa_bitset_1024& mtfa_bitset_1024::operator|= ( mtfa_bitset_1024& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    uint1024_t* d = &v[0];
    uint1024_t* p = &s1.v[0];
    uint1024_t* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ |= *p++;
    }

    return *this;
}

mtfa_bitset_1024 mtfa_bitset_1024::operator& ( mtfa_bitset_1024& s1 )
{
    mtfa_bitset_1024* op = new mtfa_bitset_1024();
    *op = *this;
    *op &= s1;
    return *op;
}

mtfa_bitset_1024& mtfa_bitset_1024::operator&= ( mtfa_bitset_1024& s1 )
{
    int size;
    unsigned int ssize = s1.size();
    unsigned int dsize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        s1.resize ( size );
    }
    else
    {
        size = ssize;
        this->resize ( size );
    }

    uint1024_t* d = &v[0];
    uint1024_t* p = &s1.v[0];
    uint1024_t* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ &= *p++;
    }

    return *this;
}

mtfa_bitset_1024 mtfa_bitset_1024::operator^ ( mtfa_bitset_1024& s1 )
{
    mtfa_bitset_1024* op = new mtfa_bitset_1024();
    *op = *this;
    return *op ^= s1;
}

mtfa_bitset_1024& mtfa_bitset_1024::operator^= ( mtfa_bitset_1024& s1 )
{
    int size;
    unsigned int dsize = s1.size();
    unsigned int ssize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        this->resize ( size );
    }
    else
    {
        size = ssize;
        s1.resize ( size );
    }

    uint1024_t* d = &v[0];
    uint1024_t* p = &s1.v[0];
    uint1024_t* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ ^= *p++;
    }

    return *this;
}

mtfa_bitset_1024 mtfa_bitset_1024::operator- ( mtfa_bitset_1024& s1 )
{
    mtfa_bitset_1024* op = new mtfa_bitset_1024();
    *op = *this;
    return *op -= s1;
}

mtfa_bitset_1024& mtfa_bitset_1024::operator-= ( mtfa_bitset_1024& s1 )
{
    int size;
    unsigned int dsize = s1.size();
    unsigned int ssize = this->size();

    if ( dsize > ssize )
    {
        size = dsize;
        this->resize ( size );
    }
    else
    {
        size = ssize;
        s1.resize ( size );
    }

    uint1024_t* d = &this->v[0];
    uint1024_t* p = &s1.v[0];
    uint1024_t* pf = &s1.v[allocated];

    while ( p != pf )
    {
        *d++ &= ~ ( *p++ );
    }

//     for ( int pos = real_size; pos < ( allocated << 6 ); ++pos )
//     {
//         int where = pos >> 10;
//         int which = pos & 0x000003FF;
//         v[where] &= nbits[which];
//     }

    return *this;
}

int mtfa_bitset_1024::count()
{
    int tot = 0;

    for ( int i = 0; i < allocated; ++i )
    {
        uint1024_t p = v[i];

        while ( p != 0 )
        {
            p &= ( p - 1 );
            tot++;
        }
    }

    return tot;
}

int* mtfa_bitset_1024::GiveOnes()
{
    std::list<int> li;

    for ( int i = 0; i < allocated; ++i )
    {
        auto p = v[i];
        while (p!=0) {
            auto pos = lsb(p);
            li.push_back(i*1024+pos);
            bit_unset(p, pos);
        }
    }

    int* vint = ( int* ) malloc ( ( li.size() + 1 ) * sizeof ( int ) );
    std::copy ( li.begin(), li.end(), vint );
    vint[li.size()] = -1;
    return vint;
}


int mtfa_bitset_1024::FirstOne()
{
    for ( int i = 0; i < allocated; ++i )
    {
        if ( v[i] )
        {
            auto pos = lsb(v[i]);
            return 1024*i+ pos;
        }
    }

    return -1; //not found
}

int mtfa_bitset_1024::FirstZero()
{
    for ( int i = 0; i < allocated; ++i )
    {
        if ( v[i] != ones )
        {
            auto pos = lsb(ones ^ v[i]);
            return 1024*i + pos;
        }
    }

    return -1;
}

string mtfa_bitset_1024::toString()
{
    int l = size();
    char* p = ( char* ) malloc ( l );
    memset ( p, '0', l );
    int i = 0;

    for ( i = 0; i < l; ++i )
        if ( test ( i ) )
        {
            p[i] = '1';
        }

    string s ( p, &p[l] );
    free ( p );
    return s;
}


/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


//accesso web tramite curl!
#include <curl/curl.h>

#include <curl/curl.h>

// Define our struct for accepting LCs output

struct BufferStruct
{
    char* buffer;
    size_t size;
};


// This is the function we pass to LC, which writes the output to a BufferStruct

static size_t WriteMemoryCallback ( void* ptr,
                                    size_t size,
                                    size_t nmemb,
                                    void* data )
{
    size_t realsize = size * nmemb;

    struct BufferStruct* mem = ( struct BufferStruct* ) data;


    mem -> buffer = ( char* ) realloc ( mem -> buffer, mem -> size + realsize + 1 );

    if ( mem -> buffer )
    {
        memcpy ( & ( mem -> buffer[mem -> size] ), ptr, realsize );

        mem -> size += realsize;
        mem -> buffer[mem -> size] = 0;
    }

    return realsize;
}

static size_t header_callback ( char* buffer, size_t size, size_t nitems, void* userdata )
{
    /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
    /* 'userdata' is set with CURLOPT_HEADERDATA */

    string* ps = ( string* ) userdata;
    ( *ps ).append ( buffer, buffer + nitems * size );

    return nitems * size;
}

void GetPageFromUrlProxy ( const string& hostname,
                           const string& port,
                           const string& protocol, //http, https
                           const string& method, //get, post
                           const string& url, //compresa la query
                           const string& additional_headers,
                           const string& data,
                           const string& proxy_name,
                           const string& proxy_port,
                           int timeout,
                           string& page,
                           string& headers,
                           bool check_ssl_server, //false by default
                           bool check_name_server, //false by default
                           const string& ca_cert,   //empty by default
                           const string& client_cert,   //empty by default
                           const string& client_key    //empty by default
                         )
{
    page.clear();

    CURL* curl;
    CURLcode res;

    //Per la gestione dell'host name e del mapping con dominio
    struct curl_slist *hostmap = nullptr;
    string realhostName = hostname;
    
    auto pos = hostname.find(":");
    if (pos != string::npos)
    {
        //C'è la coppia nome:ip 
        T_V_Split vs = mtfa_string_split(hostname, ":");
        hostmap = curl_slist_append(nullptr, hostname.c_str());
        realhostName = vs[0];
    }


    curl = curl_easy_init();

    if ( curl )
    {
        string theurl;
        string up_protocol = protocol;
        string up_method = method;

        transform ( method.begin(), method.end(), up_method.begin(), [] ( unsigned char c )
        {
            return std::toupper ( c );
        } );
        transform ( protocol.begin(), protocol.end(), up_protocol.begin(), [] ( unsigned char c )
        {
            return std::toupper ( c );
        } );

        if (
            ( port.compare ( "80" ) == 0 && up_protocol.compare ( "HTTP" ) == 0 ) ||
            ( port.compare ( "443" ) == 0 && up_protocol.compare ( "HTTPS" ) == 0 )
           )
        {
            theurl = protocol + "://" + realhostName + url;
        }
        else
        {
            theurl = protocol + "://" + realhostName + ":" + port + url;
        }

        if (hostmap)
        {
            cout << "RESOLVE: " << hostname << endl;
            auto iRet = curl_easy_setopt(curl, CURLOPT_RESOLVE, hostmap);
            if (iRet != CURLE_OK)
            {
                cout << "CURLOPT_RESOLVE not supported" << endl;
            }
        }
        
        curl_easy_setopt ( curl, CURLOPT_URL, theurl.c_str() );

        if ( check_ssl_server )
        {
            /*
            * If you want to connect to a site who isn't using a certificate that is
            * signed by one of the certs in the CA bundle you have, you can skip the
            * verification of the server's certificate. This makes the connection
            * A LOT LESS SECURE.
            *
            * If you have a CA cert for the server stored someplace else than in the
            * default bundle, then the CURLOPT_CAPATH option might come handy for
            * you.
            */
            curl_easy_setopt ( curl, CURLOPT_CAINFO, ca_cert.c_str() );
            curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYPEER, 1L );
        }
        else
        {
            curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYPEER, 0L );
        }

        if ( check_name_server )
        {
            /*
             * If the site you're connecting to uses a different host name that what
             * they have mentioned in their server certificate's commonName (or
             * subjectAltName) fields, libcurl will refuse to connect. You can skip
             * this check, but this will make the connection less secure.
             */
            curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYHOST, 1L );
        }
        else
        {
            curl_easy_setopt ( curl, CURLOPT_SSL_VERIFYHOST, 0L );
        }

        if ( client_cert.size() > 0 )
        {
            curl_easy_setopt ( curl, CURLOPT_SSLCERT, client_cert.c_str() );
            curl_easy_setopt ( curl, CURLOPT_SSLKEY,  client_key.c_str() );
        }

        curl_easy_setopt ( curl, CURLOPT_FOLLOWLOCATION, 1L );
        curl_easy_setopt ( curl, CURLOPT_NOPROGRESS, 1L );

        if ( proxy_name.size() > 0 )
        {
            curl_easy_setopt ( curl, CURLOPT_PROXY, proxy_name.c_str() );

            if ( proxy_port.size() > 0 )
            {
                curl_easy_setopt ( curl, CURLOPT_PROXYPORT, strtoul ( proxy_name.c_str(), nullptr, 10 ) );
            }
        }

        struct curl_slist* slist = nullptr;

        // Se ci sono headers, allora li metto a posto
        if ( additional_headers.size() > 0 )
        {
            string s = additional_headers;

            while ( true )
            {
                int pos = s.find ( "\r\n" );

                if ( pos == string::npos )
                {
                    slist = curl_slist_append ( slist, s.c_str() );
                    break;
                }
                else
                {
                    slist = curl_slist_append ( slist, s.substr ( 0, pos ).c_str() );
                    s = s.substr ( pos + 2 );
                }
            }
        }

        if ( slist != nullptr )
        {
            curl_easy_setopt ( curl, CURLOPT_HTTPHEADER, slist );
        }

        //if (FindPattern("method", "[pP][oO][sS][tT]"))
        //cout << __func__   << "METODO: " << up_method << endl;
        if ( up_method.compare ( "POST" ) == 0 )
        {
            // POST
            curl_easy_setopt ( curl, CURLOPT_POST, 1L );
            curl_easy_setopt ( curl, CURLOPT_POSTFIELDS, data.data() );
            curl_easy_setopt ( curl, CURLOPT_POSTFIELDSIZE, data.size() );
        }
        else
        {
            // GET
            curl_easy_setopt ( curl, CURLOPT_HTTPGET, 1L );
        }

        struct BufferStruct output;

        std::string mem_headers;

        memset ( &output, 0, sizeof ( output ) );

        curl_easy_setopt ( curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );

        curl_easy_setopt ( curl, CURLOPT_WRITEDATA, ( void* ) &output );

        //size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *userdata);
        curl_easy_setopt ( curl, CURLOPT_HEADERFUNCTION, header_callback );

        curl_easy_setopt ( curl, CURLOPT_HEADERDATA, ( void* ) &headers );

        if ( timeout > 0 )
        {
            curl_easy_setopt ( curl, CURLOPT_TIMEOUT, timeout );
        }

        res = curl_easy_perform ( curl );

        if ( res != CURLE_OK )
        {
            if ( output.buffer )
            {
                free ( output.buffer );

                output.buffer = 0;
                output.size = 0;
            }
        }

        if ( output.buffer )
        {
            page.assign ( output.buffer, output.buffer + output.size );
            free ( output.buffer );

            output.buffer = 0;
            output.size = 0;
        }

        //Provo a stampare gli headers
        //cout << __func__   << "Header: " << mem_headers << endl;

        if ( slist != nullptr )
        {
            curl_slist_free_all ( slist ); /* free the list again */
        }
        
        if (hostmap)
            curl_slist_free_all(hostmap);

        curl_easy_cleanup ( curl );
    }
}



//string BuildAnswer (
//    string document_root,
//    string the_url,
//    map<string, string> & current_query,
//    map<string, string> & current_headers,
//    map<string, string> & current_cookies
//)
//{
//
//
//
//}
//
//

int Base32::GetEncode32Length ( int bytes )
{
    int bits = bytes * 8;
    int length = bits / 5;

    if ( ( bits % 5 ) > 0 )
    {
        length++;
    }

    return length;
}

int Base32::GetDecode32Length ( int bytes )
{
    int bits = bytes * 5;
    int length = bits / 8;
    return length;
}

static bool Encode32Block ( unsigned char* in5, unsigned char* out8 )
{
    // pack 5 bytes
    unsigned long long buffer = 0;

    for ( int i = 0; i < 5; i++ )
    {
        if ( i != 0 )
        {
            buffer = ( buffer << 8 );
        }

        buffer = buffer | in5[i];
    }

    // output 8 bytes
    for ( int j = 7; j >= 0; j-- )
    {
        buffer = buffer << ( 24 + ( 7 - j ) * 5 );
        buffer = buffer >> ( 24 + ( 7 - j ) * 5 );
        unsigned char c = ( unsigned char ) ( buffer >> ( j * 5 ) );

        // self check
        if ( c >= 32 )
        {
            return false;
        }

        out8[7 - j] = c;
    }

    return true;
}

bool Base32::Encode32 ( unsigned char* in, int inLen, unsigned char* out )
{
    if ( ( in == 0 ) || ( inLen <= 0 ) || ( out == 0 ) )
    {
        return false;
    }

    int d = inLen / 5;
    int r = inLen % 5;

    unsigned char outBuff[8];

    for ( int j = 0; j < d; j++ )
    {
        if ( !Encode32Block ( &in[j * 5], &outBuff[0] ) )
        {
            return false;
        }

        memmove ( &out[j * 8], &outBuff[0], sizeof ( unsigned char ) * 8 );
    }

    unsigned char padd[5];
    memset ( padd, 0, sizeof ( unsigned char ) * 5 );

    for ( int i = 0; i < r; i++ )
    {
        padd[i] = in[inLen - r + i];
    }

    if ( !Encode32Block ( &padd[0], &outBuff[0] ) )
    {
        return false;
    }

    memmove ( &out[d * 8], &outBuff[0], sizeof ( unsigned char ) * GetEncode32Length ( r ) );

    return true;
}

static bool Decode32Block ( unsigned char* in8, unsigned char* out5 )
{
    // pack 8 bytes
    unsigned long long buffer = 0;

    for ( int i = 0; i < 8; i++ )
    {
        // input check
        if ( in8[i] >= 32 )
        {
            return false;
        }

        if ( i != 0 )
        {
            buffer = ( buffer << 5 );
        }

        buffer = buffer | in8[i];
    }

    // output 5 bytes
    for ( int j = 4; j >= 0; j-- )
    {
        out5[4 - j] = ( unsigned char ) ( buffer >> ( j * 8 ) );
    }

    return true;
}

bool Base32::Decode32 ( unsigned char* in, int inLen, unsigned char* out )
{
    if ( ( in == 0 ) || ( inLen <= 0 ) || ( out == 0 ) )
    {
        return false;
    }

    int d = inLen / 8;
    int r = inLen % 8;

    unsigned char outBuff[5];

    for ( int j = 0; j < d; j++ )
    {
        if ( !Decode32Block ( &in[j * 8], &outBuff[0] ) )
        {
            return false;
        }

        memmove ( &out[j * 5], &outBuff[0], sizeof ( unsigned char ) * 5 );
    }

    unsigned char padd[8];
    memset ( padd, 0, sizeof ( unsigned char ) * 8 );

    for ( int i = 0; i < r; i++ )
    {
        padd[i] = in[inLen - r + i];
    }

    if ( !Decode32Block ( &padd[0], &outBuff[0] ) )
    {
        return false;
    }

    memmove ( &out[d * 5], &outBuff[0], sizeof ( unsigned char ) * GetDecode32Length ( r ) );

    return true;
}

bool Base32::Map32 ( unsigned char* inout32, int inout32Len, unsigned char* alpha32 )
{
    if ( ( inout32 == 0 ) || ( inout32Len <= 0 ) || ( alpha32 == 0 ) )
    {
        return false;
    }

    for ( int i = 0; i < inout32Len; i++ )
    {
        if ( inout32[i] >= 32 )
        {
            return false;
        }

        inout32[i] = alpha32[inout32[i]];
    }

    return true;
}

static void ReverseMap ( unsigned char* inAlpha32, unsigned char* outMap )
{
    memset ( outMap, 0, sizeof ( unsigned char ) * 256 );

    for ( int i = 0; i < 32; i++ )
    {
        outMap[ ( int ) inAlpha32[i]] = i;
    }
}

bool Base32::Unmap32 ( unsigned char* inout32, int inout32Len, unsigned char* alpha32 )
{
    if ( ( inout32 == 0 ) || ( inout32Len <= 0 ) || ( alpha32 == 0 ) )
    {
        return false;
    }

    unsigned char rmap[256];
    ReverseMap ( alpha32, rmap );

    for ( int i = 0; i < inout32Len; i++ )
    {
        inout32[i] = rmap[ ( int ) inout32[i]];
    }

    return true;
}


namespace fastcrc64
{
#include <inttypes.h>
#include <stdio.h>

//static bool crc64speed_init_done = false;

    static t_table crc64_table_little =
    {
        {0}
    };
    static t_table crc64_table_big =
    {
        {0}
    };
    static void* crc64_table = NULL;
    static const bool dual = true;

    /* value of crc64_table[0][1], architecture dependent. */
#define LITTLE1 UINT64_C(0x7ad870c830358979)
#define BIG1 UINT64_C(0x79893530c870d87a)

    /* Define CRC64SPEED_SAFE if you want runtime checks to stop
     * CRCs from being calculated by uninitialized tables (and also stop tables
     * from being initialized more than once). */
#ifdef CRC64SPEED_SAFE
#define should_init(table, val)                                                \
    do {                                                                       \
        if ((table)[0][1] == (val))                                            \
            return false;                                                      \
    } while (0)
#define check_init(table, val)                                                 \
    do {                                                                       \
        if ((table)[0][1] != (val))                                            \
            return false;                                                      \
    } while (0)
#else
#define should_init(a, b)
#define check_init(a, b)
#endif

#define POLY UINT64_C(0xad93d23594c935a9)

    /**
     * Reflect all bits of a \a data word of \a data_len bytes.
     *
     * \param data         The data word to be reflected.
     * \param data_len     The width of \a data expressed in number of bits.
     * \return             The reflected data.
     *****************************************************************************/
    static inline uint_fast64_t crc_reflect ( uint_fast64_t data, size_t data_len )
    {
        uint_fast64_t ret = data & 0x01;

        for ( size_t i = 1; i < data_len; i++ )
        {
            data >>= 1;
            ret = ( ret << 1 ) | ( data & 0x01 );
        }

        return ret;
    }

    /**
     *  Update the crc value with new data.
     *
     * \param crc      The current crc value.
     * \param data     Pointer to a buffer of \a data_len bytes.
     * \param data_len Number of bytes in the \a data buffer.
     * \return         The updated crc value.
     ******************************************************************************/
    uint64_t crc64 ( uint_fast64_t crc, const void* in_data, const uint64_t len )
    {
        const uint8_t* data = ( const uint8_t* ) in_data;
        bool bit;

        for ( uint64_t offset = 0; offset < len; offset++ )
        {
            uint8_t c = data[offset];

            for ( uint_fast8_t i = 0x01; i & 0xff; i <<= 1 )
            {
                bit = crc & 0x8000000000000000;

                if ( c & i )
                {
                    bit = !bit;
                }

                crc <<= 1;

                if ( bit )
                {
                    crc ^= POLY;
                }
            }

            crc &= 0xffffffffffffffff;
        }

        crc = crc & 0xffffffffffffffff;
        return crc_reflect ( crc, 64 ) ^ 0x0000000000000000;
    }

    /******************** END GENERATED PYCRC FUNCTIONS ********************/

    void crcspeed64little_init ( crcfn64 crcfn, uint64_t table[8][256] )
    {
        uint64_t crc;

        /* generate CRCs for all single byte sequences */
        for ( int n = 0; n < 256; n++ )
        {
            table[0][n] = crcfn ( 0, &n, 1 );
        }

        /* generate nested CRC table for future slice-by-8 lookup */
        for ( int n = 0; n < 256; n++ )
        {
            crc = table[0][n];

            for ( int k = 1; k < 8; k++ )
            {
                crc = table[0][crc & 0xff] ^ ( crc >> 8 );
                table[k][n] = crc;
            }
        }
    }

    /* Reverse the bytes in a 64-bit word. */
    static inline uint64_t rev8 ( uint64_t a )
    {
#if defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap64 ( a );
#else
        uint64_t m;

        m = UINT64_C ( 0xff00ff00ff00ff );
        a = ( ( a >> 8 ) & m ) | ( a & m ) << 8;
        m = UINT64_C ( 0xffff0000ffff );
        a = ( ( a >> 16 ) & m ) | ( a & m ) << 16;
        return a >> 32 | a << 32;
#endif
    }

    /* This function is called once to initialize the CRC table for use on a
       big-endian architecture. */
    void crcspeed64big_init ( crcfn64 fn, t_table big_table )
    {
        /* Create the little endian table then reverse all the entires. */
        crcspeed64little_init ( fn, big_table );

        for ( int k = 0; k < 8; k++ )
        {
            for ( int n = 0; n < 256; n++ )
            {
                big_table[k][n] = rev8 ( big_table[k][n] );
            }
        }
    }

    /* Calculate a non-inverted CRC multiple bytes at a time on a little-endian
     * architecture. If you need inverted CRC, invert *before* calling and invert
     * *after* calling.
     * 64 bit crc = process 8 bytes at once;
     */
    uint64_t crcspeed64little ( t_table little_table, uint64_t crc, void* buf, size_t len )
    {
        unsigned char* next = ( unsigned char* ) buf;

        /* process individual bytes until we reach an 8-byte aligned pointer */
        while ( len && ( ( uintptr_t ) next & 7 ) != 0 )
        {
            crc = little_table[0][ ( crc ^ *next++ ) & 0xff] ^ ( crc >> 8 );
            len--;
        }

        /* fast middle processing, 8 bytes (aligned!) per loop */
        while ( len >= 8 )
        {
            crc ^= * ( uint64_t* ) next;
            crc = little_table[7][crc & 0xff] ^
                  little_table[6][ ( crc >> 8 ) & 0xff] ^
                  little_table[5][ ( crc >> 16 ) & 0xff] ^
                  little_table[4][ ( crc >> 24 ) & 0xff] ^
                  little_table[3][ ( crc >> 32 ) & 0xff] ^
                  little_table[2][ ( crc >> 40 ) & 0xff] ^
                  little_table[1][ ( crc >> 48 ) & 0xff] ^
                  little_table[0][crc >> 56];
            next += 8;
            len -= 8;
        }

        /* process remaining bytes (can't be larger than 8) */
        while ( len )
        {
            crc = little_table[0][ ( crc ^ *next++ ) & 0xff] ^ ( crc >> 8 );
            len--;
        }

        return crc;
    }

    /* Calculate a non-inverted CRC eight bytes at a time on a big-endian
     * architecture.
     */
    uint64_t crcspeed64big ( t_table big_table, uint64_t crc, void* buf, size_t len )
    {
        unsigned char* next = ( unsigned char* ) buf;

        crc = rev8 ( crc );

        while ( len && ( ( uintptr_t ) next & 7 ) != 0 )
        {
            crc = big_table[0][ ( crc >> 56 ) ^ *next++] ^ ( crc << 8 );
            len--;
        }

        while ( len >= 8 )
        {
            crc ^= * ( uint64_t* ) next;
            crc = big_table[0][crc & 0xff] ^
                  big_table[1][ ( crc >> 8 ) & 0xff] ^
                  big_table[2][ ( crc >> 16 ) & 0xff] ^
                  big_table[3][ ( crc >> 24 ) & 0xff] ^
                  big_table[4][ ( crc >> 32 ) & 0xff] ^
                  big_table[5][ ( crc >> 40 ) & 0xff] ^
                  big_table[6][ ( crc >> 48 ) & 0xff] ^
                  big_table[7][crc >> 56];
            next += 8;
            len -= 8;
        }

        while ( len )
        {
            crc = big_table[0][ ( crc >> 56 ) ^ *next++] ^ ( crc << 8 );
            len--;
        }

        return rev8 ( crc );
    }

    uint64_t crcspeed64native ( t_table table, uint64_t crc, void* buf, size_t len )
    {
        uint64_t n = 1;

        return * ( char* ) &n ? crcspeed64little ( table, crc, buf, len )
               : crcspeed64big ( table, crc, buf, len );
    }

    /* Initialize CRC lookup table in architecture-dependent manner. */
    void crcspeed64native_init ( crcfn64 fn, t_table table )
    {
        uint64_t n = 1;

        * ( char* ) &n ? crcspeed64little_init ( fn, table )
        : crcspeed64big_init ( fn, table );
    }

    /* Returns false if CRC64SPEED_SAFE and table already initialized. */
    bool crc64speed_init ( void )
    {
        should_init ( crc64_table_little, LITTLE1 );
        crcspeed64little_init ( crc64, dual ? crc64_table_little : * ( ( t_table* ) crc64_table ) );
        return true;
    }

    /* Returns false if CRC64SPEED_SAFE and table already initialized. */
    bool crc64speed_init_big ( void )
    {
        should_init ( crc64_table_big, BIG1 );
        crcspeed64big_init ( crc64, dual ? crc64_table_big : * ( ( t_table* ) crc64_table ) );
        return true;
    }

    uint64_t crc64speed ( uint64_t crc, const void* s, const uint64_t l )
    {
        /* Quickly check if CRC table is initialized to little endian correctly. */
        check_init ( crc64_table_little, LITTLE1 );
        return crcspeed64little ( dual ? crc64_table_little : * ( ( t_table* ) crc64_table ), crc, ( void* ) s, l );
    }

    uint64_t crc64speed_big ( uint64_t crc, const void* s, const uint64_t l )
    {
        /* Quickly check if CRC table is initialized to big endian correctly. */
        check_init ( crc64_table_big, BIG1 );
        return crcspeed64big ( dual ? crc64_table_big : * ( ( t_table* ) crc64_table ), crc, ( void* ) s,
                               l );
    }

    bool crc64speed_init_native ( void )
    {
        const uint64_t n = 1;
        return * ( char* ) &n ? crc64speed_init() : crc64speed_init_big();
    }

    /* Iterate over table to fully load it into a cache near the CPU. */
    void crc64speed_cache_table ( void )
    {
        uint64_t m;

        for ( int i = 0; i < 8; ++i )
        {
            for ( int j = 0; j < 256; ++j )
            {
                m = crc64_table_little[i][j];
                m += crc64_table_big[i][j];
                ++m;
            }
        }
    }

    /* If you are on a platform where endianness can change at runtime, this
     * will break unless you compile with CRC64SPEED_DUAL and manually run
     * _init() and _init_big() instead of using _init_native() */
    uint64_t crc64speed_native ( uint64_t crc, const void* s, const uint64_t l )
    {
        const uint64_t n = 1;

        if ( !crc64speed_init_done )
        {
            crc64speed_cache_table();
            crc64speed_init_native();
            crc64speed_init_done = true;
        }

        return * ( char* ) &n ? crc64speed ( crc, s, l ) : crc64speed_big ( crc, s, l );
    }

}

static mpz_class UnsignedPChar2Mpz ( const unsigned char* p, unsigned int len )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), len, 1, 1, 1, 0, p );
    return value;
}

static void Mpz2UnsignedPChar ( mpz_class value, unsigned char*& p, int& len )
{
    unsigned char* vbytes;

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

string mtfa_base_62 ( const unsigned char* p, int len )
{
    mpz_class cvalue;
    //    static int s2a2 = 62*62;
    //    static char table[62*62][3];
    //    static bool initialized = false;
    //    if (!initialized)
    //    {
    //        initialized = true;
    //        for (int i=0; i<s2a2; ++i)
    //        {
    //            mpz_class v = i;
    //            string v62 = v.get_str(62);
    //            if (v62.size() == 1)
    //            {
    //                table[i][0] = '0';
    //                table[i][1] = v62[0];
    //                table[i][2] = 0;
    //            }
    //            else
    //            {
    //                table[i][0] = v62[0];
    //                table[i][1] = v62[1];
    //                table[i][2] = 0;
    //            }
    //        }
    //    }
    //    if (len == 8)
    //    {
    //        //è un unsigned long long
    //        unsigned long long pull = *(unsigned long long*)p;
    //        string res;
    //        while (pull > 0)
    //        {
    //            res = table[pull % s2a2];
    //            pull /= s2a2;
    //        }
    //        return res;
    //    }
    //    else
    {
        cvalue = UnsignedPChar2Mpz ( p, len );
        return cvalue.get_str ( 62 );
    }
}

int mtfa_debase_62 ( const string n, unsigned char*& res )
{
    mpz_class cvalue;
    cvalue.set_str ( n.c_str(), 62 );

    res = NULL;
    int ilen;
    Mpz2UnsignedPChar ( cvalue, res, ilen );
    return ilen;
}

string mtfa_base_62 ( unsigned long long n )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), 1, -1, sizeof ( n ), 0, 0, &n );
    return value.get_str ( 62 );
}

unsigned long long mtfa_debase_62 ( const string b62 )
{
    mpz_class value;
    value.set_str ( b62, 62 );

    unsigned long long result = 0;
    mpz_export ( &result, 0, -1, sizeof ( result ), 0, 0, value.get_mpz_t() );
    return result;
}

string HexDump ( const unsigned char* p, int len_p, bool withAscii, int cols )
{
    ostringstream s_out;

    for ( int i = 0; i < len_p; i += cols )
    {
        for ( int j = 0; j < cols; j++ )
        {
            if ( ( i + j ) >= len_p )
            {
                s_out << "   ";
            }
            else
            {
                char l[32];
                sprintf ( l, "%02X ", p[i + j] );
                s_out << l;
            }
        }

        if ( withAscii )
        {
            s_out << "  ";

            for ( int j = 0; j < cols; j++ )
            {
                if ( ( i + j ) >= len_p )
                {
                    s_out << " ";
                }
                else
                {
                    if ( isprint ( p[i + j] ) )
                    {
                        s_out << p[i + j];
                    }
                    else
                    {
                        s_out << ".";
                    }
                }
            }
        }

        s_out << std::endl;
    }

    s_out << std::endl;
    return s_out.str();
}

string HexDump ( string s, bool withAscii, int cols )
{
    return HexDump ( ( const unsigned char* ) s.c_str(), s.size(), withAscii, cols );
}

//A faster atoi in base 10
unsigned long long mtfa_atoull ( const string& p )
{
    return mtfa_atoull ( p.c_str() );
}
unsigned long long mtfa_atoull ( const char* p )
{
    unsigned long long x = 0;

    while ( p && *p != 0 )
    {
        x = ( x * 10 ) + ( *p - '0' );
        ++p;
    }

    return x;
};

string mtfa_lltoa ( long long value, int radix )
{
    if ( value > 0 )
        return mtfa_ulltoa ( ( unsigned long long ) value, radix );
    else
        return "-" + mtfa_ulltoa ( ( unsigned long long ) ( -value ), radix );
}

string mtfa_ulltoa ( unsigned long long value, int radix )
{
    string sret;
    char tmp[65];
    char* tp = &tmp[0];
    int i;
    unsigned long long v;

    if ( radix > 62 || radix <= 1 )
    {
        return sret;
    }

    v = ( unsigned long long ) value;

    while ( v || tp == &tmp[0] )
    {
        i = v % ( long long ) radix;
        v = v / ( long long ) radix;

        if ( i < 10 )
            *tp++ = i + '0';
        else if ( i < 36 )
            *tp++ = i - 10 + 'A';
        else
            *tp++ = i - 36 + 'a';
    }

    *tp = 0; //fine stringa

    sret.assign ( tmp );
    return sret;
}

unsigned long long mtfa_rand_simple_ull ()
{
    return random_generator.next();
}

void mtfa_random_generator_init ( unsigned long long seed )
{
    random_generator = mtfa_fastrng1024 ( seed );
}

string mtfa_rand_string ( int len, int base )
{
    string sret = "";

    while ( sret.size() < len )
    {
        unsigned long long val = random_generator.next();
        string stmp =  mtfa_ulltoa ( val, base );
        sret += stmp;
    }

    return sret.substr ( 0, len );
}

string mtfa_rand_alfanum ( int len, string alphabet )
{
    const char* data = alphabet.data();
    int dim = alphabet.size();
    char* result = ( char* ) malloc ( len + 1 );
    result[len] = 0;

    for ( int i = 0; i < len; ++i )
    {
        result[i] = data[random_generator.next() % dim];
    }

    string sret;
    sret.assign ( result, result + len );
    free ( result );
    return sret;
}

string mtfa_make_cookie ( string key, string value, CookieDurations expiration, int howManySecs, string path, string domain, bool http_only, bool secure )
{
    /*
        Set-Cookie: <cookie-name>=<cookie-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Expires=<date>
        Set-Cookie: <cookie-name>=<cookie-value>; Max-Age=<non-zero-digit>
        Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Path=<path-value>
        Set-Cookie: <cookie-name>=<cookie-value>; Secure
        Set-Cookie: <cookie-name>=<cookie-value>; HttpOnly

        Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Strict
        Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Lax

        Multiple directives are also possible, for example:
        Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>; Secure; HttpOnly

        Sun, 17-Jan-2038 19:14:07 GMT
        "Set-Cookie: " + k + "=" + v + "; expires=" + date + "; path=" + path + "; domain=" + domain
     * */

    //Costruiamo il cookie
    std::ostringstream ss;
    ss << "Set-Cookie: " << key << "=" << value;

    if ( path.size() > 0 )
        ss << "; " << "Path=" << path;

    switch ( expiration )
    {
        case CookieDurations::NeverExpires:
            ss << "; " << "Expires=" << "Fri, 31-Dec-9999 23:59:59 GMT";
            break;

        case CookieDurations::ExpiresInSeconds:
        {
            auto now = chrono::system_clock::now();
            auto next = chrono::system_clock::to_time_t ( now + chrono::seconds ( howManySecs ) );
            struct tm tm_buf;
            char date[128];
            gmtime_r ( &next, &tm_buf );
            strftime ( date, sizeof date, "%a, %e-%b-%Y %H:%M:%S %Z", &tm_buf );
            ss << "; " << "Expires=" << date;
        }
        break;

        case CookieDurations::SessionCookie:
            //in questo caso non si mette l'expiration
            break;
    }

    if ( domain.size() > 0 )
        ss << "; " << "Domain=" << domain;

    if ( http_only )
        ss << "; " << "HttpOnly";

    if ( secure )
        ss << "; " << "Secure";

    return ss.str();
}

string mtfa_delete_cookie ( string key, string path, string domain, bool http_only, bool secure )
{
    //Costruiamo il cookie
    std::ostringstream ss;
    ss << "Set-Cookie: " << key << "=" << "; expires=Thu, 01 Jan 1970 00:00:00 GMT";

    if ( path.size() > 0 )
        ss << "; " << "Path=" << path;

    if ( domain.size() > 0 )
        ss << "; " << "Domain=" << domain;

    if ( http_only )
        ss << "; " << "HttpOnly";

    if ( secure )
        ss << "; " << "Secure";

    return ss.str();
}


/********************/
//
//mtfa_conc_umap_ss::mtfa_conc_umap_ss()
//{
//    m = new mutex();
//};
//mtfa_conc_umap_ss::~mtfa_conc_umap_ss()
//{
//    delete m;
//};
//string mtfa_conc_umap_ss::find(const string& k)
//{
//    lock_guard<mutex> lock(*m);
//    auto p = um.find(k);
//    if (p == um.end())
//        return "";
//    else
//        return p->second;
//};
//int mtfa_conc_umap_ss::size()
//{
//    lock_guard<mutex> lock(*m);
//    return um.size();
//};
//void mtfa_conc_umap_ss::clear()
//{
//    lock_guard<mutex> lock(*m);
//    um.clear();
//};
//void mtfa_conc_umap_ss::insert(const string&k, const string&v)
//{
//    lock_guard<mutex> lock(*m);
//    um.insert(make_pair(k, v));
//};
//
///***/
//
//mtfa_conc_umap_is::mtfa_conc_umap_is()
//{
//    m = new mutex();
//};
//mtfa_conc_umap_is::~mtfa_conc_umap_is()
//{
//    delete m;
//};
//string mtfa_conc_umap_is::find(const int& k)
//{
//    lock_guard<mutex> lock(*m);
//    auto p = um.find(k);
//    if (p == um.end())
//        return "";
//    else
//        return p->second;
//};
//int mtfa_conc_umap_is::size()
//{
//    lock_guard<mutex> lock(*m);
//    return um.size();
//};
//void mtfa_conc_umap_is::insert(const int&k, const string&v)
//{
//    lock_guard<mutex> lock(*m);
//    um.insert(make_pair(k, v));
//};
//void mtfa_conc_umap_is::clear()
//{
//    lock_guard<mutex> lock(*m);
//    um.clear();
//};
//
///***************************/
//
//mtfa_conc_uset_i::mtfa_conc_uset_i()
//{
//    m = new mutex();
//};
//mtfa_conc_uset_i::~mtfa_conc_uset_i()
//{
//    delete m;
//};
//bool mtfa_conc_uset_i::find(const int& k)
//{
//    lock_guard<mutex> lock(*m);
//    auto p = um.find(k);
//    if (p == um.end())
//        return false;
//    else
//        return true;
//};
//int mtfa_conc_uset_i::size()
//{
//    lock_guard<mutex> lock(*m);
//    return um.size();
//};
//void mtfa_conc_uset_i::insert(const int&k)
//{
//    lock_guard<mutex> lock(*m);
//    um.insert(k);
//};
//void mtfa_conc_uset_i::clear()
//{
//    lock_guard<mutex> lock(*m);
//    um.clear();
//};
//
///***/
//
//mtfa_conc_uset_s::mtfa_conc_uset_s()
//{
//    m = new mutex();
//};
//mtfa_conc_uset_s::~mtfa_conc_uset_s()
//{
//    delete m;
//};
//bool mtfa_conc_uset_s::find(const string& k)
//{
//    lock_guard<mutex> lock(*m);
//    auto p = um.find(k);
//    if (p == um.end())
//        return false;
//    else
//        return true;
//};
//int mtfa_conc_uset_s::size()
//{
//    lock_guard<mutex> lock(*m);
//    return um.size();
//};
//void mtfa_conc_uset_s::insert(const string&k)
//{
//    lock_guard<mutex> lock(*m);
//    um.insert(k);
//};
//void mtfa_conc_uset_s::clear()
//{
//    lock_guard<mutex> lock(*m);
//    um.clear();
//};
//

#include <libguile.h>

#if 0
static SCM my_handler_proc ( void* handler_data, SCM key, SCM parameters )
{
    //     //Put the code which you want to handle an error here.
    // #ifdef _DEBUG
    //     fprintf(stderr, "Errore in internal. to_latin=%d. to_stringn=%d\n",
    //             ((T_body_data_to*)handler_data)->to_latin,
    //             ((T_body_data_to*)handler_data)->to_stringn);
    // #endif
    //     ((T_body_data_to*)handler_data)->bret = 0;
    return SCM_BOOL_F;
}

typedef struct __body_data_to__
{
    SCM sin;
    char* p;
    size_t len;
    int to_latin;
    int to_stringn;
} T_body_data_to;

static SCM internal_scm_to_string_proc ( void* p )
{
    T_body_data_to* bd = ( T_body_data_to* ) p;

    if ( bd->to_latin == 1 )
    {
        if ( bd->to_stringn == 1 )
            bd->p = scm_to_latin1_stringn ( bd->sin, &bd->len );
        else
            bd->p = scm_to_latin1_string ( bd->sin );
    }
    else if ( bd->to_latin == 0 )
    {
        if ( bd->to_stringn == 1 )
            bd->p = scm_to_locale_stringn ( bd->sin, &bd->len );
        else
            bd->p = scm_to_locale_string ( bd->sin );
    }
    else
	{
		if ( bd->to_stringn == 1 )
			bd->p = scm_to_utf8_stringn ( bd->sin, &bd->len );
		else
			bd->p = scm_to_utf8_string ( bd->sin );
	}
	
    return SCM_BOOL_T;
}

char* ScmToString ( SCM s, size_t* len, int tipo /*1 latin, 0 locale, 2 utf8*/ )
{
	T_body_data_to bd;
	bd.len = 0;
	bd.p = nullptr;
	bd.sin = s;
	bd.to_latin = (tipo % 3);
	bd.to_stringn = (len==nullptr)?0:1;
	void* handler_data = &bd;
	SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_to_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );
	
	if ( bret == SCM_BOOL_T )
	{
		*len = bd.len;
		return bd.p;
	}
	else
		return nullptr;
}
	

//Conversione senza errori. La utilizzo in tutti i codici
char* ScmToStringn ( SCM s, size_t* len )
{
     T_body_data_to bd;
     bd.len = 0;
     bd.p = nullptr;
     bd.sin = s;
     bd.to_latin = 0;
     bd.to_stringn = 1;
     void* handler_data = &bd;
     SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_to_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );
 
     if ( bret == SCM_BOOL_T )
     {
         *len = bd.len;
         return bd.p;
     }
 
     bd.len = 0;
     bd.p = nullptr;
     bd.sin = s;
     bd.to_latin = 1;
     bd.to_stringn = 1;
     handler_data = &bd;
     bret = scm_c_catch ( SCM_BOOL_T, internal_scm_to_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );
 
     if ( bret == SCM_BOOL_T )
     {
         *len = bd.len;
         return bd.p;
     }
 
     return nullptr;
}

char* ScmToString ( SCM s )
{
    T_body_data_to bd;
    bd.len = 0;
    bd.p = nullptr;
    bd.sin = s;
    bd.to_latin = 0;
    bd.to_stringn = 0;
    void* handler_data = &bd;
    SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_to_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.p;
    }

    bd.len = 0;
    bd.p = nullptr;
    bd.sin = s;
    bd.to_latin = 1;
    bd.to_stringn = 0;
    handler_data = &bd;
    bret = scm_c_catch ( SCM_BOOL_T, internal_scm_to_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.p;
    }

    return nullptr;
}

typedef struct __body_data_from__
{
    SCM sout;
    char* p;
    size_t len;
    int from_latin;
    int from_stringn;
} T_body_data_from;

static SCM internal_scm_from_string_proc ( void* p )
{
    T_body_data_from* bd = ( T_body_data_from* ) p;

    if ( bd->from_latin == 1 )
    {
        if ( bd->from_stringn == 1 )
            bd->sout = scm_from_latin1_stringn ( bd->p, bd->len );
        else
            bd->sout = scm_from_latin1_string ( bd->p );
    }
    else if ( bd->from_latin == 0 )
    {
        if ( bd->from_stringn == 1 )
            bd->sout = scm_from_locale_stringn ( bd->p, bd->len );
        else
            bd->sout = scm_from_locale_string ( bd->p );
    }
    else
	{
		if ( bd->from_stringn == 1 )
			bd->sout = scm_from_utf8_stringn ( bd->p, bd->len );
		else
			bd->sout = scm_from_utf8_string ( bd->p );
	}
	
    return SCM_BOOL_T;
}

SCM ScmFromString ( const char* s, const int len, int tipo /*2:utf8, 1: latin, 0: locale*/)
{
	T_body_data_from bd;
	bd.len = len;
	bd.p = ( char* ) s;
	bd.from_latin = (tipo % 3);
	bd.from_stringn = (len == 0)?0:1;
	void* handler_data = &bd;
	SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_from_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );
	
	if ( bret == SCM_BOOL_T )
	{
		return bd.sout;
	}
	else
		return SCM_EOL;
}


SCM ScmFromStringn ( const char* s, const int len )
{
    T_body_data_from bd;
    bd.len = len;
    bd.p = ( char* ) s;
    bd.from_latin = 0;
    bd.from_stringn = 1;
    void* handler_data = &bd;
    SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_from_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.sout;
    }

    bd.len = len;
    bd.p = ( char* ) s;
    bd.from_latin = 1;
    bd.from_stringn = 1;
    handler_data = &bd;
    bret = scm_c_catch ( SCM_BOOL_T, internal_scm_from_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.sout;
    }

    return SCM_EOL;
}

SCM ScmFromString ( const char* s )
{
    T_body_data_from bd;
    bd.p = ( char* ) s;
    bd.from_latin = 0;
    bd.from_stringn = 0;
    void* handler_data = &bd;
    SCM bret = scm_c_catch ( SCM_BOOL_T, internal_scm_from_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.sout;
    }

    bd.p = ( char* ) s;
    bd.from_latin = 1;
    bd.from_stringn = 0;
    handler_data = &bd;
    bret = scm_c_catch ( SCM_BOOL_T, internal_scm_from_string_proc, &bd, my_handler_proc, handler_data, NULL, NULL );

    if ( bret == SCM_BOOL_T )
    {
        return bd.sout;
    }

    return SCM_EOL;
}
#else

//Conversione senza errori. La utilizzo in tutti i codici
char* ScmToStringn ( SCM s, size_t* len )
{
    if (!scm_is_string(s))
    {
        char * p = (char *)malloc(1);
        p[0]=0;
        *len=0;
        return p;
    }
    char * p = nullptr;
    try {
        p = scm_to_locale_stringn(s, len);
    } catch (exception ex)
    {
        mtfa_log("Eccezione: %s\n", ex.what());
    }
    return p;
}

char* ScmToString ( SCM s )
{
    if (!scm_is_string(s))
    {
        char * p = (char *)malloc(1);
        p[0]=0;
        return p;
    }
    char * p = nullptr;
    try {
        p = scm_to_locale_stringn(s, nullptr);
    } catch (exception ex)
    {
        mtfa_log("Eccezione: %s\n", ex.what());
    }
    return p;
}


SCM ScmFromString ( const char* s)
{
    SCM ret = SCM_EOL;
    try {
        ret = scm_from_locale_string(s);
    } catch (exception ex)
    {
        mtfa_log("Eccezione: %s\n", ex.what());
    }
    return ret;
}


SCM ScmFromStringn ( const char* s, const int len )
{
    SCM ret = SCM_EOL;
    try {
        ret = scm_from_locale_stringn(s, len);
    } catch (exception ex)
    {
        mtfa_log("Eccezione: %s\n", ex.what());
    }
    return ret;
}

#endif


string mtfa_gen_uuid ( string p1, string p2 )
{
    uuids::uuid ret = uuids::system_uuid();
    return ret.to_string();
}

bool mtfa_compress ( const string& sin, string& sout, int level )
{
    sout.clear();

    //Alloco spazio per il compressore
	ulong destLen = compressBound ( sin.size() );

	auto dest = new unsigned char[destLen+12]; //4 length, 8 crc64

    int cok = compress2 ( &dest[12], &destLen, ( unsigned char* ) sin.data(), sin.size(), level );

    //compresso bene?
    if ( cok != Z_OK )
    {
        delete[] dest;
        return false;
    }

    //metto size originale aggiungo il crc64
    * ( unsigned int* ) ( &dest[8] ) = sin.size();

    //calcolo crc64
    unsigned long long crc64 = fastcrc64::crc64 ( 0, &dest[8], destLen + 4 );

    //aggiungo crc64
    * ( unsigned long long* ) ( &dest[0] ) = crc64;

    //Occupo solo lo spazio del compresso, rilascio memoria non utilizzata
    sout.assign ( dest, dest + destLen + 12 );
	delete [] dest;
	
    return true;
}

bool mtfa_uncompress ( const string& sin, string& sout )
{
    //Verifico che i dati compressi siano corretti

    sout.clear();

    if ( sin.size() <= 12 )
    {
        return false;
    }

    //verifico il crc64
    unsigned long long newcrc64 = fastcrc64::crc64 ( 0, &sin.data() [8], sin.size() - 8 );

    if ( newcrc64 != * ( unsigned long long* ) sin.data() )
    {
        return false;
    }

    //possiamo decomprimere
    ulong origLen = ( ulong ) * ( unsigned int* ) &sin.data() [8];
    unsigned char* orig = new unsigned char[origLen];

    int dok = uncompress ( orig, &origLen, ( unsigned char* ) &sin.data() [12], sin.size() - 12 );

    if ( dok != Z_OK )
    {
        delete[] orig;
        return false;
    }

    sout.assign ( orig, orig + origLen );
	delete[] orig;
	return true;
}

/*
 * SetUnion e generatore kolmogorof
 */
pair<int, int> BSetUnion::Index ( int idx )
{
    return make_pair ( idx >> 6, idx & 63 );
}

void BSetUnion::setBitValue ( const unsigned int index, const bool value )
{
    auto pos = GetIndex ( index );
    
    if ( occupied_left[pos.first] > pos.second )
    {
        if ( value )
        {
            values_left[pos.first] |= SetBitMask[pos.second];
        }
        else
        {
            values_left[pos.first] &= ~SetBitMask[pos.second];
        }
        
        return;
    }
    
    if ( occupied_right[pos.first] > ( pos.second - occupied_left[pos.first] ) )
    {
        if ( value )
        {
            values_right[pos.first] |= SetBitMask[pos.second - occupied_left[pos.first]];
        }
        else
        {
            values_right[pos.first] &= ~SetBitMask[pos.second - occupied_left[pos.first]];
        }
    }
    else
        throw "Errore il bit non esiste";
}

void BSetUnion::Dump()
{
    if ( ranges[0] != nullptr )
    {
        printf ( "Ranges (size, lastBusyOnLeft, FirstFreeOnRight, Value)\n" );

        for ( int i = 0; i < size; ++i )
        {
            printf ( "%4d) %4d \t%4d \t%4d \t%016llX(%d)\t%016llX(%d)\n",
                     i,
                     ranges[i]->size,
                     ranges[i]->lastBusyOnLeft,
                     ranges[i]->firstFreeOnRight,
                     values_left[i], occupied_left[i],
                     values_right[i], occupied_right[i]
                   );
        }
    }

    cout << endl;
}

//Il size è in bit!
BSetUnion::BSetUnion ( int _size )
{
    bitAfterLastBitLastCall = -1;

    size = _size >> 6;
    total = 0;

    ranges = new TRange*[size](); //vector<TRange*>(size, nullptr);
    the_ranges = new TRange[size](); //vector<TRange>(size);
    values_left = new unsigned long long[size];
    values_right = new unsigned long long[size];
    occupied_left = new unsigned char[size];
    occupied_right = new unsigned char[size];
    occupied_lr = new unsigned char[size];
    fill ( occupied_left, occupied_left + size, 0 );
    fill ( occupied_right, occupied_right + size, 0 );
    fill ( occupied_lr, occupied_lr + size, 0 );
    fill ( values_left, values_left + size, 0ULL );
    fill ( values_right, values_right + size, 0ULL );

    //All'inizio tutti i posti sono vuoti
    for ( int i = 0; i < size; ++i )
    {
        ranges[i] = &the_ranges[i];
        ranges[i]->size = 0; //tieni presente che finchè non raggiunge i 64, no problems, inserisco nel ull
        ranges[i]->firstFreeOnRight = i;
        ranges[i]->lastBusyOnLeft = i;
    }
}

BSetUnion::~BSetUnion()
{
    delete[] the_ranges;
    delete[] ranges;
    delete[] values_left;
    delete[] values_right;
    delete[] occupied_left;
    delete[] occupied_right;
    delete[] occupied_lr;
}

int BSetUnion::Occupied ( const int i )
{
    return occupied_lr[i];
    //return occupied_right[i] + occupied_left[i];
}

//Inserisce a un indirizzo di bit, all'interno lo converto in indirizzo di cluster (64bit)
bool BSetUnion::InsertValue ( int _index, bool value, bool right) //right means: grow to right otherwise grow to left
{
    unsigned int index = _index >> 6; /// 64;

    //Quando è piena la struttura? Quando è rimasto un solo puntatore a null
    if ( ranges[index] == nullptr || total >= ( size << 6 ) )
    {
        cout << "Terminata!\n" << endl;
        throw "Errore, spazio terminato!";
    }

    int firstNotFull;

    //Da quale lato devo inserire? Per costruzione ho sempre spazio per inserire
    if ( right )
    {
        firstNotFull = ranges[index]->firstFreeOnRight;
        values_left[firstNotFull] = values_left[firstNotFull] << 1 | ( value ? 1 : 0 );
        occupied_left[firstNotFull]++;
        occupied_lr[firstNotFull]++;
    }
    else
    {
        //devo inserire a sinistra
        int idx_sinistra = ranges[index]->lastBusyOnLeft - 1;

        if ( idx_sinistra < 0 )
            idx_sinistra = size - 1;

        firstNotFull = ranges[idx_sinistra]->firstFreeOnRight;
        values_right[firstNotFull] = values_right[firstNotFull] << 1 | ( value ? 1 : 0 );
        occupied_right[firstNotFull]++;
        occupied_lr[firstNotFull]++;
    }

    //ne ho aggiunto 1 a firstNotFull, ora devo fare la union?
    total++;

    if ( Occupied ( firstNotFull ) < 64 )
        return true;

    //Devo fare la union sempre a destra di firstNotNull
    int idx_sinistra, idx_destra;

    idx_sinistra = firstNotFull;
    idx_destra = ( ranges[firstNotFull]->firstFreeOnRight + 1 ) % size;

    ranges[firstNotFull]->size++;

    if ( ranges[firstNotFull]->size >= size )
        return true; //non devo fare union, non c'è più spazio

    auto num_sinistra = ranges[idx_sinistra]->size;
    auto num_destra   = ranges[idx_destra]->size;

    if ( ( num_destra + num_sinistra ) >= size )
        return true; //non devo fare union, non c'è più spazio

    if ( num_sinistra >= num_destra )
    {
        ranges[idx_sinistra]->size += num_destra;
        //cout << ranges[idx_sinistra]->firstFreeOnRight << ", " << ranges[idx_destra]->firstFreeOnRight << endl;
        ranges[idx_sinistra]->firstFreeOnRight = ranges[idx_destra]->firstFreeOnRight;

        int start, howmany;
        start = ranges[idx_destra]->lastBusyOnLeft;
        howmany = ranges[idx_destra]->size;

        for ( int i = 0; i <= howmany; ++i )
            ranges[ ( i + start ) % size] = ranges[idx_sinistra];

        return true;
    }
    else
    {
        ranges[idx_destra]->size += num_sinistra;
        ranges[idx_destra]->lastBusyOnLeft = ranges[idx_sinistra]->lastBusyOnLeft;

        int start, howmany;
        start = ranges[idx_sinistra]->lastBusyOnLeft;
        howmany = ranges[idx_sinistra]->size;

        for ( int i = 0; i <= howmany; ++i )
            ranges[ ( i + start ) % size] = ranges[idx_destra];

        return true;
    }

}

/**
 * @brief Cerca il prossimo bit "occupato" (inserito nella struttura) a partire dalla posizione index
 *
 * @param index p_index: posizione da cui partire
 * @param occupied p_occupied: indica se cercare occupato oppure libero
 * @param avanti p_avanti: indica la direzione di ricerca (avanti/indietro)
 * @return int: torna la posizione del bit "occupato"
 *
 * Considerare che i bit messi nella parola i 64 bit sono aggiunti a "sinistra" oppure a "destra"
 * e sono considerati crescenti da sinistra a destra. Quindi i bit di destra ???
 */
int BSetUnion::getAddressOfNextBit ( const unsigned int index, const bool occupied, const bool avanti )
{
    //a partire da index (indirizzo lungo) cerca la posizione del prossimo bit occupato o libero
    auto pos = GetIndex ( index );
    pos.first %= size;

    //consideriamo come se gli occupati a sinistra proseguano negli occupati a destra
    if ( avanti )
    {
        if ( occupied )
        {
            while ( true )
            {
                //cerco il primo occupato a partire dall'indirizzo dato
                if ( Occupied ( pos.first ) > pos.second )
                {
                    //Ci sono più occupati di quanto richiesto
                    //                             if (Opos.first != pos.first || Opos.second != pos.second)
                    //                             {
                    //                                 cout << "C1(" << Opos.first << ", " << Opos.second << "): " << (Opos.first << 6) + Opos.second << endl;
                    //                                 cout << "C2(" << pos.first << ", " << pos.second << "): " << (pos.first << 6) + pos.second << endl;
                    //                             }
                    return ( pos.first << 6 ) + pos.second;
                }
                else
                {
                    //Devo andare al prossimo cluster
                    pos.second = 0;
                    pos.first = ( pos.first + 1 ) % size;
                    continue;
                }
            }
        }
        else
        {
            //cerco il primo indirizzo libero a partire da quello dato
            while ( true )
            {
                //se non sono tutti occupati allora ce nè uno libero
                if ( Occupied ( pos.first ) < pos.second )
                    return ( pos.first << 6 ) + pos.second;

                if ( Occupied ( pos.first ) < 64 )
                    return ( pos.first << 6 ) + Occupied ( pos.first );

                pos.second = 0;
                pos.first = ( pos.first + 1 ) % size;
            }
        }
    }
    else
    {
        //cerca indietro
        if ( occupied )
        {
            while ( true )
            {
                //cerco il primo occupato a partire dalla posizione data andando verso sinistra
                if ( Occupied ( pos.first ) > pos.second )
                    return ( pos.first << 6 ) + pos.second;

                //ma ce ne sono di occupati?
                if ( Occupied ( pos.first ) > 0 )
                    return ( pos.first << 6 ) + Occupied ( pos.first );

                //ho esaurito gli occupati a sinistra, passo al cluster precedente
                pos.second = 63;
                pos.first = pos.first - 1;

                if ( pos.first < 0 )
                    pos.first = size - 1;

                continue;
            }
        }
        else
        {
            //cerco il primo indirizzo libero a partire da quallo dato verso sinistra !
            while ( true )
            {
                if ( Occupied ( pos.first ) < pos.second )
                    return ( pos.first << 6 ) + pos.second;

                //ho esaurito i liberi a sinistra, passo al cluster precedente
                pos.second = 63;
                pos.first = pos.first - 1;

                if ( pos.first < 0 )
                    pos.first = size - 1;

                continue;
            }
        }
    }
}

//presuppongo che il bit sia occupato!!!
bool BSetUnion::getBitValue ( const unsigned int index )
{
    auto pos = GetIndex ( index );

    if ( occupied_left[pos.first] > pos.second )
    {
        bool val = ( SetBitMask[pos.second] & values_left[pos.first] ) ? true : false;
        return val;
    }

    if ( occupied_right[pos.first] > ( pos.second - occupied_left[pos.first] ) )
    {
        bool val = ( SetBitMask[pos.second - occupied_left[pos.first]] & values_right[pos.first] ) ? true : false;
        return val;
    }
    else
        throw "Errore il bit non esiste";
}

unsigned int BSetUnion::getNBitsFromIndex ( const int idx, int numBits )
{
    //ricordo l'ultimo bit della chiamata precedente (quello dopo l'ultimo)
	//cout << "Chiede " << numBits << " a partire da " << idx << endl;
	bitAfterLastBitLastCall = -1;
    if ( bitAfterLastBitLastCall < 0 )
    {
        unsigned int address = 0;
        int pos = idx;

        while ( numBits-- > 0 )
        {
            pos = getAddressOfNextBit ( pos, true, true );
            bool value = getBitValue ( pos );
            address = ( address << 1 ) | ( value ? 1 : 0 );
            pos = ( pos + 1 ) % ( size << 6 );
        }

        bitAfterLastBitLastCall = pos;
        oldAddress = address;
        return address;
    }
    else
    {
        // mi servono due bit a partire da idx e 1 bit da bitAfter..
        bool bithh, bith, bitl;
        int pos = idx;
        pos = getAddressOfNextBit ( pos, true, true );
        bithh = getBitValue ( pos );
        pos = ( pos + 1 ) % ( size << 6 );
        pos = getAddressOfNextBit ( pos, true, true );
        bith = getBitValue ( pos );
        pos = ( pos + 1 ) % ( size << 6 );

        //ora mi serve l'ultimo bit
        pos = getAddressOfNextBit ( bitAfterLastBitLastCall, true, true );
        bitl = getBitValue ( pos );
        pos = ( pos + 1 ) % ( size << 6 );
        bitAfterLastBitLastCall = pos;

        //ora devo modificare il vecchio address
        if ( bithh )
            oldAddress |= SetBitMask[numBits - 1];
        else
            oldAddress &= ~SetBitMask[numBits - 1];

        if ( bith )
            oldAddress |= SetBitMask[numBits - 2];
        else
            oldAddress &= ~SetBitMask[numBits - 2];

        if ( bitl )
            oldAddress |= 1;
        else
            oldAddress &= ~SetBitMask[0];

        return oldAddress;
    }
}

//da 0 a 63
void BSetUnion::SetBit ( unsigned long long& w, //the word to modify:  if (f) w |= m; else w &= ~m;
                         const unsigned int _m, // the bit mask
                         const bool f           // conditional flag
                       )
{
    unsigned long long m = SetBitMask[_m];
    w = ( w & ~m ) | ( -f & m );
}

pair<int, int> BSetUnion::GetIndex ( int i )
{
    return make_pair ( i >> 6, i & 63 );
}

//posizione, dimensione dell'insieme
list<pair<int, int>> BSetUnion::EnumerateSets()
{
    list<pair<int, int>> li;

    //parto dalla posizione 0 e poi avanti fino alla fine
    //ovvio che non è detto che in posizione 0 ci sia qualcosa e quindi inizio a cercare,
    //al primo occupato, lo utilizzo e poi proseguo di conserva
    int idx = 0;

    while ( true )      //Quando gira ho terminato, poichè sono partito da 0
    {
        if ( Occupied ( idx ) == 0 )
        {
            ++idx;
            if (idx >= size)
                break;
            else
                continue;
        }

        pair<int, int> currentPair;
        //se sono qui, c'è qualcosa.
        //devo valutare il range di occupazione
        int left = the_ranges[idx].lastBusyOnLeft;      //se == idx, allora inizia qui
        int right = the_ranges[idx].firstFreeOnRight;   //se == idx, allora termina qui

        if ( left <= right )
        {
            currentPair = make_pair ( left << 6, ( ( right - left ) << 6 ) + Occupied ( right ) );
            li.push_back ( currentPair );
            idx = right + 1;
        }
        else
        {
            //ho girato, devo terminare
            //ho pieni da left a size
            //e poi da 0 a right, incluso
            currentPair = make_pair ( left << 6, ( ( size - left ) << 6 ) + ( right << 6 ) + Occupied ( right ) );
            li.push_back ( currentPair );
            break;
        }
    }

    return li;
}


boost::dynamic_bitset<> concatLoopDyn( const boost::dynamic_bitset<>& bs1,const boost::dynamic_bitset<>& bs2)
{
	boost::dynamic_bitset<> res(bs1);
	res.resize(bs1.size()+bs2.size());
	size_t bs1Size=bs1.size();
	size_t bs2Size=bs2.size();
	
	for(size_t i=0;i<bs2Size;i++)
		res.set(i+bs1Size, bs2[i]);
	return res;
}


boost::dynamic_bitset<> BSetUnion::ToBitSet() {

	boost::dynamic_bitset<> btot;
	
	int totale = 0;
	for (int i = 0; i < size; ++i) {
		if (Occupied ( i ) <= 0) {
			continue;
		}
		
		if (occupied_left[i] > 0) {
			btot.resize(totale+occupied_left[i]);
			unsigned long long val=1;
			for (int k=0; k<occupied_left[i]; ++k)
			{
				btot.set(totale+occupied_left[i]-k-1, values_left[i] & val);
				val <<= 1;
			}
			totale += occupied_left[i];
		}
		if (occupied_right[i] > 0) {
			btot.resize(totale+occupied_right[i]);
			unsigned long long val=1;
			for (int k=0; k<occupied_right[i]; ++k)
			{
				btot.set(totale+occupied_right[i]-k-1, values_right[i] & val);
				val <<= 1;
			}
			totale += occupied_right[i];
		}
	}
	
	return btot;
	
}


#if 0
/***********************************SET UNION CLASSICA******************************/

// bool SetUnion::Delete( int index )
// {
//     if (ranges[index]->firstFreeOnRight == ranges[index]->lastBusyOnLeft)
//     {
//         //Vuoto!
//         return false;
//     }
// 
//     //non è vuolo, lo devo splittare
//     int firstFreeOnRight, lastBusyOnLeft;
//     firstFreeOnRight = ranges[index]->firstFreeOnRight;
//     lastBusyOnLeft = ranges[index]->lastBusyOnLeft;
//     
//     //ne ho tolto 1
//     total--;
//     ranges[firstFreeOnRight]->size--;
// 
//     //ora, l'elemento in posizione index splitta la struttura in due
//     //devo aggiornare la parte più corta
//     //Parto da lastbusyonleft fino a index e poi da index a firstFreeOnRight
// 
//     //TODO: ricordati che devi gestire il caso in cui stai togliendo l'ultimo 
//     int idx_sinistra, idx_center, idx_destra;
// 
//     idx_sinistra = lastBusyOnLeft;
//     idx_center = index;
//     idx_destra = firstFreeOnRight;
//     
//     if (index == idx_sinistra)
//     {
//         //Sto togliendo da uno dei bordi! é sufficiente aggiornare il bordo a sinistra 
//         //che dovrà puntare di nuovo a sè stesso
//     }
// 
//     int num_sinistra_centro;
//     if (index < idx_sinistra)
//         num_sinistra_centro = index-idx_sinistra-1;
//     else
//         num_sinistra_centro = size-idx_sinistra+index
//     
//     auto num_centro_destra   = firstFreeOnRight-index;
//     
//     if ( num_sinistra >= num_destra )
//     {
//         ranges[idx_sinistra]->size += num_destra;
//         ranges[idx_sinistra]->firstFreeOnRight = ranges[idx_destra]->firstFreeOnRight;
//         
//         int start, howmany;
//         start = ranges[idx_destra]->lastBusyOnLeft;
//         howmany = ranges[idx_destra]->size;
//         
//         for ( int i = 0; i <= howmany; ++i )
//             ranges[ ( i + start ) % size] = ranges[idx_sinistra];
//         
//         return true;
//     }
//     else
//     {
//         ranges[idx_destra]->size += num_sinistra;
//         ranges[idx_destra]->lastBusyOnLeft = ranges[idx_sinistra]->lastBusyOnLeft;
//         int start, howmany;
//         start = ranges[idx_sinistra]->lastBusyOnLeft;
//         howmany = ranges[idx_sinistra]->size;
//         
//         for ( int i = 0; i <= howmany; ++i )
//             ranges[ ( i + start ) % size] = ranges[idx_destra];
//         
//         return true;
//     }
// }
#endif


/**********************************************/
/**********************************************/
//          La sezione RLP per ethereum
/**********************************************/
/**********************************************/

#include <rlpvalue.h>
#include <univalue.h>
#include <InfInt.h>

#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))
#define UBEGIN(a)           ((unsigned char*)&(a))
#define UEND(a)             ((unsigned char*)&((&(a))[1]))
#define ARRAYLEN(array)     (sizeof(array)/sizeof((array)[0]))



static const std::string CHARS_ALPHA_NUM = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

const signed char p_util_hexdigit[256] =
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };

signed char HexDigit(char c)
{
    return p_util_hexdigit[(unsigned char)c];
}

bool IsHex(const std::string& str)
{
    for(std::string::const_iterator it(str.begin()); it != str.end(); ++it)
    {
        if (HexDigit(*it) < 0)
            return false;
    }
    return (str.size() > 0) && (str.size()%2 == 0);
}

bool IsHexNumber(const std::string& str)
{
    size_t starting_location = 0;
    if (str.size() > 2 && *str.begin() == '0' && *(str.begin()+1) == 'x') {
        starting_location = 2;
    }
    for (auto c : str.substr(starting_location)) {
        if (HexDigit(c) < 0) return false;
    }
    // Return false for empty string or "0x".
    return (str.size() > starting_location);
}

std::vector<unsigned char> ParseHex(const char* psz)
{
    // convert hex dump to vector
    std::vector<unsigned char> vch;
    while (true)
    {
        while (isspace(*psz))
            psz++;
        signed char c = HexDigit(*psz++);
        if (c == (signed char)-1)
            break;
        unsigned char n = (c << 4);
        c = HexDigit(*psz++);
        if (c == (signed char)-1)
            break;
        n |= c;
        vch.push_back(n);
    }
    return vch;
}

std::vector<unsigned char> ParseHex(const std::string& str)
{
    return ParseHex(str.c_str());
}

std::string EncodeBase64(const unsigned char* pch, size_t len)
{
    static const char *pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string strRet = "";
    strRet.reserve((len+2)/3*4);

    int mode=0, left=0;
    const unsigned char *pchEnd = pch+len;

    while (pch<pchEnd)
    {
        int enc = *(pch++);
        switch (mode)
        {
            case 0: // we have no bits
                strRet += pbase64[enc >> 2];
                left = (enc & 3) << 4;
                mode = 1;
                break;

            case 1: // we have two bits
                strRet += pbase64[left | (enc >> 4)];
                left = (enc & 15) << 2;
                mode = 2;
                break;

            case 2: // we have four bits
                strRet += pbase64[left | (enc >> 6)];
                strRet += pbase64[enc & 63];
                mode = 0;
                break;
        }
    }

    if (mode)
    {
        strRet += pbase64[left];
        strRet += '=';
        if (mode == 1)
            strRet += '=';
    }

    return strRet;
}

std::string EncodeBase64(const std::string& str)
{
    return EncodeBase64((const unsigned char*)str.c_str(), str.size());
}

std::vector<unsigned char> DecodeBase64(const char* p, bool* pfInvalid)
{
    static const int decode64_table[256] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
        -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
        29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    if (pfInvalid)
        *pfInvalid = false;

    std::vector<unsigned char> vchRet;
    vchRet.reserve(strlen(p)*3/4);

    int mode = 0;
    int left = 0;

    while (1)
    {
         int dec = decode64_table[(unsigned char)*p];
         if (dec == -1) break;
         p++;
         switch (mode)
         {
             case 0: // we have no bits and get 6
                 left = dec;
                 mode = 1;
                 break;

              case 1: // we have 6 bits and keep 4
                  vchRet.push_back((left<<2) | (dec>>4));
                  left = dec & 15;
                  mode = 2;
                  break;

             case 2: // we have 4 bits and get 6, we keep 2
                 vchRet.push_back((left<<4) | (dec>>2));
                 left = dec & 3;
                 mode = 3;
                 break;

             case 3: // we have 2 bits and get 6
                 vchRet.push_back((left<<6) | dec);
                 mode = 0;
                 break;
         }
    }

    if (pfInvalid)
        switch (mode)
        {
            case 0: // 4n base64 characters processed: ok
                break;

            case 1: // 4n+1 base64 character processed: impossible
                *pfInvalid = true;
                break;

            case 2: // 4n+2 base64 characters processed: require '=='
                if (left || p[0] != '=' || p[1] != '=' || decode64_table[(unsigned char)p[2]] != -1)
                    *pfInvalid = true;
                break;

            case 3: // 4n+3 base64 characters processed: require '='
                if (left || p[0] != '=' || decode64_table[(unsigned char)p[1]] != -1)
                    *pfInvalid = true;
                break;
        }

    return vchRet;
}

std::string DecodeBase64(const std::string& str)
{
    std::vector<unsigned char> vchRet = DecodeBase64(str.c_str());
    return std::string((const char*)vchRet.data(), vchRet.size());
}



void RLPtoJSON(const RLPValue& rval, UniValue& jval);
std::string encodeBinary(uint64_t n);

static void assignJsonArray(UniValue& jval, const RLPValue& rval)
{
	jval.setArray();

	const std::vector<RLPValue>& values = rval.getValues();
	for (auto it = values.begin(); it != values.end(); it++) {
		const RLPValue& childVal = *it;
		UniValue childJval;

		RLPtoJSON(childVal, childJval);

		jval.push_back(childJval);
	}
}

static void assignJsonBuffer(UniValue& jval, const RLPValue& rval)
{
	jval.setStr(rval.getValStr());
}

void RLPtoJSON(const RLPValue& rval, UniValue& jval)
{
	if (rval.isBuffer())
		assignJsonBuffer(jval, rval);
	else
		assignJsonArray(jval, rval);
}

static bool isBigNumStr(const std::string& s)
{
	// first char must be #
	if (s.empty() || s[0] != '#')
		return false;

	// remaining chars must be digits
	for (unsigned int i = 1; i < s.size(); i++)
		if (!isdigit(s[i]))
			return false;

	return true;
}

static std::string encodeBigNum(const InfInt& n)
{
	std::string rs;

	if (n == 0) {
		// do nothing; return empty string
	} else {
		rs.assign(encodeBigNum(n / 256));

		InfInt iich = n % 256;
		unsigned char ch = iich.toUnsignedLong();
		rs.append((const char *) &ch, 1);
	}

	return rs;
}

static std::string encodeBigNumStr ( const std::string& s )
{
    InfInt n ( s );
    return encodeBigNum ( n );
}

bool JSONtoRLP ( const UniValue& jval, RLPValue& rval )
{
    if ( jval.isStr() ) {
        std::string ins = jval.getValStr();
        if ( isBigNumStr ( ins ) )
            ins = encodeBigNumStr ( ins.substr ( 1 ) );

        rval.assign ( ins );
        return true;
    }

    if ( jval.isNum() ) {
        uint64_t val = jval.get_int64();
        std::string val_enc = encodeBinary ( val );
        rval.assign ( val_enc );
        return true;
    }

    if ( jval.isArray() ) {
        rval.setArray();
        const std::vector<UniValue>& arrVals = jval.getValues();
        for ( auto it = arrVals.begin(); it != arrVals.end(); it++ ) {
            const UniValue& childJval = *it;
            RLPValue tmp;
            if ( !JSONtoRLP ( childJval, tmp ) )
                return false;
            rval.push_back ( tmp );
        }
        return true;
    }

    return false;
}



#if 0
/****La classe dei web socket */
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>

/*****************Per la gestione dei Web Socket *****************/
typedef websocketpp::server<websocketpp::config::core> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

//Arrivano dati da fuori, li devo elaborare e li metto nella coda di uscita
//è un while, prende i dati dalla coda di ingresso, li elabora, e li mette nella coda di uscita
void MyWebSocket::ioManager ()
{
    thread th([this]() {
        while (!terminate) {
            //se ci sono dati in ingresso, arricchisco oldData
            try {
                oldData += q_in.front();
                if (terminate)
                    return;
                do {
                    int got = con->read_some ( oldData.data(), oldData.size() );
                    oldData = oldData.substr ( got );
                } while (oldData.size() > 0);
            } catch ( websocketpp::exception const & e ) {
                //std::cerr << "WSS Failed-1: " << e.what() << std::endl;
            }
        }
    });
    th.detach();
}

MyWebSocket::MyWebSocket(pair<string, bool> (*_cb)(const string & data, bool & data_type, void * user_data), void * _user_data) {
    closed = false;
    terminate = false;
    catchedAnError=false;
    the_server.clear_access_channels ( websocketpp::log::alevel::all );
    cb = _cb;
    user_data = _user_data;

    //La write handler scrive sulla coda di uscita
    auto write_handler = [this] (websocketpp::connection_hdl hdl, char const * data, size_t len) -> websocketpp::lib::error_code {
        //cout << "write_handler: " << len << endl;
        q_out.push(string(data, len));
        return std::error_code ( 0, std::generic_category() );
    };
    the_server.set_write_handler(write_handler);

    oldData = ""; //per la stringona di input gestita da manageio

    // Register our close handler: lib::error_code(connection_hdl)
    the_server.set_shutdown_handler ( [this] ( websocketpp::connection_hdl hdl ) -> std::error_code {
        this->closed = true;
        this->terminate= true;
        return std::error_code ( 0, std::generic_category() ); //per ora torna sempre allo stesso modo
    } );

    // Register our message handler. Questo è per i messaggi applicativi! In sostanza è chiamato da manageio
    the_server.set_message_handler ( bind ( [this] ( server* s, websocketpp::connection_hdl hdl, message_ptr msg ) -> void {
        this->con_hdl = hdl;
        
        pair<string, bool> recvd_data;
        recvd_data.first = msg->get_payload();
        if ( msg->get_opcode() == websocketpp::frame::opcode::text )
            recvd_data.second = true; //text
        else
            recvd_data.second = false; //binary

        auto sret = this->cb(recvd_data.first, recvd_data.second, user_data);
        try
        {
            if ( sret.first != "" ) {
                if ( sret.second ) {
                    s->send ( hdl, sret.first, websocketpp::frame::opcode::text );
                } else {
                    s->send ( hdl, sret.first, websocketpp::frame::opcode::binary );
                }
            }
        } catch ( websocketpp::exception const & e )
        {
            this->catchedAnError=true;
            cerr << std::string ( "WSS Failed: " )+e.what() << endl;
        }
    }, &the_server,::_1,::_2 ) );

    con = the_server.get_connection();
    con->start();
    ioManager();
}

void MyWebSocket::prepare_to_send(const pair<string, bool> & data) {
    if (data.second)
        the_server.send ( con_hdl, data.first, websocketpp::frame::opcode::text );
    else
        the_server.send ( con_hdl, data.first, websocketpp::frame::opcode::binary );
}

bool MyWebSocket::isClosed() {
    return closed;
}

bool MyWebSocket::gotAnError()
{
    return catchedAnError;
}

void MyWebSocket::resetError()
{
    catchedAnError=false;
}

void MyWebSocket::close()
{
    std::lock_guard<std::mutex> lck ( mymutex ); //per evitare collisioni sullo stesso websocket
    if ( !closed ) {
        terminate = true;
        closed=true;
        try {
            //ho ancora dati in oldData?
            string sret;
            while ( oldData.size() > 0 && !terminate) {
                int got = con->read_some ( oldData.data(), oldData.size() );
                oldData = oldData.substr ( got );
            }
            con->close ( websocketpp::close::status::normal, "Session terminated" );
        } catch ( websocketpp::exception const & e ) {
            catchedAnError=true;
            std::cerr << "WSS Failed-0: " << e.what() << std::endl;
        }
    }
}

/* COME SI USA???*/
//     MyWebSocket * mws = new MyWebSocket(cb);
// 
//     bool stop = false;
//     thread threcv([&] () {
//         while ((recvMsgSize = sock->recv(bufin, RCVBUFSIZE)) > 0) { // Zero means end of transmission
//             //cout << "Impila in coda di ingresso: " << recvMsgSize << endl;
//             mws->q_in.push(string(bufin, recvMsgSize));
//             //cout << "Impilato!\n";
//             memset(bufin, 0, RCVBUFSIZE);
//         }
//         stop = true;
//         cout << "Chiuso!!!\n";
//     });
//     
//     threcv.detach();
//     
//     thread thsend([&] () {
//         while (!stop) {
//             if ((rand() % 100) < 10)
//                 mws->prepare_to_send(make_pair("Esempio di invio " + to_string(rand()), true));
//             if ((rand() % 100) < 10)
//                 mws->prepare_to_send(make_pair("Basta", true));
//             usleep(10000);
//         }
//         cout << "Me ne sono accorto!!!!!!\n";
//         mws->q_in.stop();
//         mws->q_out.stop();
//     });
//     
//     thsend.detach();
// 
//     while (!stop) {
//         auto sout = mws->q_out.front();
//         int sent = send(sock->sockDesc, sout.data(), sout.size(), 0);
//         //cout << "Sent: " << sout.size() << endl;
//     }
//     cout << "Anche io!!!!!!\n";
//     mws->close();
//     cout << "Terminated" << endl;
//     sleep (5);
//     delete mws;

#endif

