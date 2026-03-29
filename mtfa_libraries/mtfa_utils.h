#pragma once

#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <libguile.h>

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#include <list>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <string>
#include <cstdarg>
#include <vector>
#include <mutex>
#include <thread>
#include <typeinfo>
#include <typeindex>
#include <queue>          // std::queue
#include <gmpxx.h>


#include "ULL_Rng.h"
#include "mtfa_shared_mutex.h"


using namespace std;
using namespace boost::multiprecision;

class CVettorePChar1
{
private:
    char ** v;
    int rows;
public:
    const int base_allocazione = 8192;
    const int base_allocazione_bits = 13; //indica quanti shift per fare 8192
    const unsigned int base_allocazione_mask = 0x00001FFF;
    unsigned int   righe_attuali;
    unsigned int   righe;
    char initializer;
    unsigned long long Size();
    void Reallocate ( unsigned int r );
    void Init ( char in_initializer );
    CVettorePChar1 ( char in_initializer );
    CVettorePChar1();
    void Init();
    ~CVettorePChar1();
    void Clear();
    void CopyData ( char * p, int len );
    string Linearize ();
};

class mtfa_binary_stream_save
{
private:
    //CVettorePChar stream_buf;
	string stream_buf;
    FILE * file;
    std::string filename;
    unsigned int dim;
    unsigned int allocated;
    unsigned int alloc_size;
    unsigned int curpos;
public:
    void Append ( const std::string & v )
    {
        Append<unsigned char> ( ( unsigned char* ) ( v.data() ), ( unsigned int ) v.size() );
    };

    template<typename T> void Append ( const T& v )
    {
        unsigned int len = sizeof ( T );
        //stream_buf.CopyData ( ( char * ) &v, len );
		stream_buf.append(( char * ) &v, len);
        dim += len;
    };

    template<typename T> void Append ( const T* v, const unsigned int item_num )
    {
        Append<unsigned int> ( item_num ); //Salvo il numero di elementi dell'array
        unsigned int len = sizeof ( T ) *item_num;
		//stream_buf.CopyData ( ( char * ) v, len );
		stream_buf.append( ( char * ) v, len );
		dim += len;
    };


    /*
        void Append (int v);
        void Append (unsigned int v);

        void Append (unsigned int * v, unsigned int item_num);
        void Append (int * v, unsigned int item_num);

        void Append (unsigned char v);
        void Append (char v);

        void Append (unsigned char * v, unsigned int item_num);
        void Append (char * v, unsigned int item_num);

        void Append (unsigned long long v);
        void Append (long long v);

        void Append (unsigned long long * v, unsigned int item_num);
        void Append (long long * v, unsigned int item_num);

        void Append (unsigned short v);
        void Append (short v);

        void Append (unsigned short * v, unsigned int item_num);
        void Append (short * v, unsigned int item_num);
    */

    mtfa_binary_stream_save ( std::string name );

    ~mtfa_binary_stream_save ();


    std::string GetStream();
};

class mtfa_binary_stream_load
{
private:
    unsigned char * stream_buf;
    FILE * file;
    std::string filename;
    unsigned int dim;
    unsigned int allocated;
    unsigned int alloc_size;
    unsigned int curpos;
public:
    //queste tre a copertura globale
    template<typename T> T Read()
    {
        int len = sizeof ( T );
        curpos += len;
        return * ( T* ) &stream_buf[curpos - len];
    };

    template<typename T> T * Read ( unsigned int & size )
    {
        size = Read<unsigned int>();
        if ( size == 0 ) {
            return NULL;
        }

        int blen = size * sizeof ( T );
        T* p = ( T* ) malloc ( blen );
        memcpy ( p, &stream_buf[curpos], blen );
        curpos += blen;

        return p;
    };

    string Read()
    {
        unsigned char * p;
        unsigned int len;
        p = Read<unsigned char> ( len );
        std::string s;
        s.assign ( p, &p[len] );
        free ( p );
        return s;
    };

//     unsigned char ReadUC ();
//     unsigned char ReadC ();
//
//     unsigned int ReadUI ();
//     int ReadI ();
//
//     unsigned short ReadUS ();
//     short ReadS ();
//
//     unsigned long long ReadULL ();
//     long long ReadLL ();
//
//     unsigned char * ReadVUC (unsigned int & size);
//     unsigned short * ReadVUS(unsigned int & size);
//     unsigned int * ReadVUI (unsigned int & size);
//     unsigned long long * ReadVULL (unsigned int & size);
//
//     char * ReadVC (unsigned int & size);
//     short * ReadVS(unsigned int & size);
//     int * ReadVI (unsigned int & size);
//     long long * ReadVLL (unsigned int & size);

    mtfa_binary_stream_load ( std::string name );
    void mtfa_binary_stream_set ( const std::string & data );

    ~mtfa_binary_stream_load ();

    //std::string ReadString ();
};

// //Le callback da c++11 a c standard
// namespace {
//     template < typename F, int I,
//                     typename L,
//                     typename R,
//                     typename ... A >
// 	inline F cify(L && l,
// 		      R(*)(A ...)
// 		      noexcept(noexcept
// 			       (::std::declval < F >
// 				()(::std::declval < A > ()...)))) {
// 	// static thread_local L l_(::std::forward<L>(l));
// 	// static thread_local bool full;
// 	static L        l_(::std::forward < L > (l));
// 	static bool     full;
// 
// 	if (full) {
// 	    l_. ~ L();
// 
// 	    new(static_cast < void *>(&l_)) L(::std::forward < L > (l));
// 	} else {
// 	    full = true;
// 	}
// 
// 	struct S {
// 	    static R        f(A ... args)
// 		noexcept(noexcept
// 			 (::std::declval < F >
// 			  ()(::std::forward < A > (args) ...))) {
// 		return l_(::std::forward < A > (args) ...);
// 	}};
// 
// 	return &S::f;
//     }
// 
// }
// 
// template <typename F, int I = 0, typename L>
// F cify ( L&& l )
// {
//     return cify<F, I> ( ::std::forward<L> ( l ), F() );
// }

class Ticks
{
private:
public:

    ///Torna i microsecondi o (in assenza) dai millisecondi della macchina
    static long long get();
    static long long get_micro();
};

unsigned long long GetULLTime();
string GetStrTime ( time_t t );
string GetStrTime ();

int __mtfa_log(const char * name, int line, const char *fmt...);
#define mtfa_log(fmt, ...) __mtfa_log(__func__, __LINE__, fmt, ##__VA_ARGS__)


int WriteAll ( int nfile, unsigned char * buf, int buflen );
int ReadAll ( int nfile, unsigned char * buf, int buflen );
void FileInfo ( const std::string &nomeFile,
                time_t &        c_time,
                time_t &        a_time,
                time_t &        m_time,
                int &           drive_number,
                bool &          isDirectory,
                long long &     size );


//Un generatore "veloce"
class SimpleRng
{
private:
    unsigned long long SimpleRngPrev;
//    mutex * pmymutex;

public:
    ~SimpleRng()
    {
//        if (pmymutex)
//            delete pmymutex;
//        pmymutex = nullptr;
    }

    SimpleRng()
    {
        SimpleRngPrev = time ( nullptr );
//        pmymutex = new mutex();
    }
    SimpleRng ( unsigned long long seed )
    {
//        lock_guard<mutex> lock(*pmymutex);
        SimpleRngPrev = seed;
    }
    inline unsigned long long Next()
    {
//        lock_guard<mutex> lock(*pmymutex);
        return SimpleRngPrev = 6364136223846793005 * SimpleRngPrev + 1442695040888963407;
    }
    unsigned long long Next ( unsigned long long seed )
    {
//        lock_guard<mutex> lock(*pmymutex);
        SimpleRngPrev = seed;
        return SimpleRngPrev = 6364136223846793005 * SimpleRngPrev + 1442695040888963407;
    }
};

class mtfa_FileIo
{
public:
    static bool Exists ( const string & nomeFile );

    static bool Rename ( const string &n1,
                         const string &n2 );

    static bool Delete ( const string &nomeFile );

    static bool Create ( const string &nomeFile,
                         bool            isText = false );

    static bool Write ( const string &nomeFile,
                        const string &val,
                        bool            isText = false );

    static bool Write ( const string &nomeFile,
                        unsigned char * pval,
                        int             len,
                        bool            isText = false );

    static bool Append ( const string   &nomeFile,
                         const string & val,
                         bool              isText = false );

    static string ReadAll ( const string & nomeFile,
                            bool      isText = false );

    static std::list<string> ReadAllLines ( const string &nomeFile );

    static void GetFileListing ( std::list<string> & listing,
                                 const string              &directory,
                                 const string              &fileFilter,
                                 bool                   recursively = true );

    static void FileInfo ( const string &nomeFile,
                           time_t &        c_time,
                           time_t &        a_time,
                           time_t &        m_time,
                           int &           drive_number,
                           bool &          isDirectory,
                           long long &     size );

    static string MkTemp();
};

class mtfa_fastrng64
{
private:
    mutex * mtx;
    unsigned long long x; // The state can be seeded with any value.
public:
    ~mtfa_fastrng64() {delete mtx;};
    mtfa_fastrng64() { x=0ULL; mtx = new mutex(); };
    mtfa_fastrng64(unsigned long long seed) {x=seed; mtx = new mutex(); };
    unsigned long long next();
};

class mtfa_fastrng1024
{
private:
    //mutex *mtx;
    unsigned long long s[16]; 
    int p;
    
public:
//     ~mtfa_fastrng1024() {
//         if (mtx) {
//             cout << "Delete!!!" << endl;
//             delete mtx;
//         }
//         mtx = nullptr;
//     }
    mtfa_fastrng1024();
    mtfa_fastrng1024(unsigned long long);
    unsigned long long next(void);
    void jump(void);
};

//Altri due generatori molto ma molto veloci, direttamente INTEL!!
#include <stdint.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

class mtfa_fastrand_sse {
private:
	//
	// MWC1616 data
    uint32_t fa[4];
    uint32_t fb[4];
    uint32_t fmask[4];
    uint32_t fm1[4];
    uint32_t fm2[4];

    uint64_t fres[2];

    bool toGenerate;
    
    void InitFastRand(
        uint16_t a1, uint16_t c1,
        uint16_t b1, uint16_t d1,
        uint16_t a2, uint16_t c2,
        uint16_t b2, uint16_t d2,
        uint16_t a3, uint16_t c3,
        uint16_t b3, uint16_t d3,
        uint16_t a4, uint16_t c4,
        uint16_t b4, uint16_t d4);
public:

//     ~mtfa_fastrand_sse() {
//         if (fastrand)
//             delete fastrand;
//         fastrand=nullptr;
//     };

    mtfa_fastrand_sse();
    mtfa_fastrand_sse(uint64_t seed);
    uint64_t next();
    uint64_t next_sse4();
};

/*
class mtfa_mat_cypher_hybrid
{
private:
    //Sono 8 matrici (vettori da 65536 unsigned long long). Ne utilizziamo due per calcolare la prossima posizione, una per calcolare la formula e sei per il valore.
    mtfa_fastrng1024 mask;
    //posizione attuale nelle 8 matrici
    unsigned char x[8];
    void NextPosition();
    int where;   //la posizione dell'ultimo elemento consumato (0-7)
public:
    void SetMask ( unsigned long long seed );
    void SetMask ( const char * data, int len );
    void cypher ( char * data, int len ); //Cifra incrementale. Modifica direttamente i dati di ingresso
    mtfa_mat_cypher_hybrid ( unsigned long long seed );
    mtfa_mat_cypher_hybrid ( const char * data, int len );
};
*/


//Da utilizzare nelle comunicazioni tra va e am
class mtfa_mat_cypher_fast
{
private:
    //Sono 8 matrici (vettori da 65536 unsigned long long). Ne utilizziamo due per calcolare la prossima posizione, una per calcolare la formula e sei per il valore.
    //SimpleRng * jrpos;
    mtfa_fastrng1024 mask;
    //posizione attuale nelle 8 matrici
    unsigned char x[8];
    void NextPosition();
    int where;   //la posizione dell'ultimo elemento consumato (0-7)
public:
    void SetMask ( unsigned long long seed );
    void SetMask ( const char * data, int len );
    void cypher ( char * data, int len ); //Cifra incrementale. Modifica direttamente i dati di ingresso
    mtfa_mat_cypher_fast ( unsigned long long seed );
    mtfa_mat_cypher_fast ( const char * data, int len );
};

//Mantenuta viva poiché utilizzata con Andrea.
class mtfa_mat_cypher_ext
{
private:
    //Sono 8 matrici (vettori da 65536 unsigned long long). Ne utilizziamo due per calcolare la prossima posizione, una per calcolare la formula e sei per il valore.
    //SimpleRng * jrpos;
    ULL_Rng * mask;
    //posizione attuale nelle 8 matrici
    unsigned short x[8];
    void NextPosition();
public:
    void SetMask ( unsigned long long seed );
    void SetMask ( const char * data, int len );
    unsigned long long DoCrc64 ( const char * data, int len );
    char * cypher ( const char * data, int len ); //Cifra incrementale
    mtfa_mat_cypher_ext ( unsigned long long seed );
    mtfa_mat_cypher_ext ( const char * data, int len );
    mtfa_mat_cypher_ext ();
    ~mtfa_mat_cypher_ext ();
    unsigned long long mtfa_mat_rand();
};


/*
 * Questa classe mi conente di
 * - generare in formato json una matricetta da 8 mbyte, tramite una funzione generatrice
 * - cifrare e decifrare indicando la funzione generatrice della matricetta
 */
class mtfa_mat_cypher_small
{
private:
    //Sono 8 matrici (vettori da 65536 unsigned long long). Ne utilizziamo due per calcolare la prossima posizione, una per calcolare la formula e sei per il valore.
    ULL_Rng * jrpos;
    ULL_Rng * mask;
    int bytes_consumed;
    //posizione attuale nelle 8 matrici
    unsigned short x[8];
public:
    mtfa_mat_cypher_small ( unsigned long long seed );
    ~mtfa_mat_cypher_small ();
};


unsigned long long mtfa_mat_cypher ( unsigned long long magic, unsigned long long disturber, unsigned char * daCifrare, int len );

typedef vector<string> T_V_Split;
T_V_Split mtfa_string_split ( const string& str, const string& delimiters );
T_V_Split mtfa_string_split2 ( const string& str, const string& delimiter );

inline unsigned char to_hex ( unsigned char x );
std::string urlencode ( const std::string &s );
inline unsigned char from_hex ( unsigned char ch );
std::string urldecode ( const std::string &str );


bool mtfa_TestConnection ( char * ip, int port, int timeout=2500 );
bool mtfa_RecvInt ( int s, int & val, int ms=2500 );
bool mtfa_SendInt ( int s, int val, int ms=2500 );
void mtfa_RecvAll ( int s, string & b, int len, int ms=2500 );
int mtfa_SendAll ( int s, char * b, int len, int ms=2500 );
int mtfa_SendAll ( int s, string b, int ms=2500 );
bool mtfa_SendStream ( int s, string str, int ms=2500 );
bool mtfa_RecvStream ( int s, string &str, int ms=2500 );
bool mtfa_RecvBuffer ( int s, string & b, int ms=2500 );
bool mtfa_SendBuffer ( int s, string b, int ms=2500 );

std::string StrTime();

unsigned long long mtfa_reverse_bits ( unsigned long long v );

string mtfa_special_cypher ( string & data );
string mtfa_special_decypher ( string & data );


#define mtfa_min(x,y) (((x)>(y))?(y):(x))
#define mtfa_max(x,y) (((x)>(y))?(x):(y))


unsigned int find_32 ( unsigned int n );
unsigned int find_64 ( unsigned long long n );

class mtfa_base_bitset
{
private:
    static unsigned const int block_size = 32;
    static          std::pair < int, int > GetIdx(const int p);
public:
    unsigned long long v[block_size];
    mtfa_base_bitset();
    mtfa_base_bitset(const mtfa_base_bitset & s1);
    ~mtfa_base_bitset();

    mtfa_base_bitset & set ( const bool val=true );
    mtfa_base_bitset & set ( const unsigned int pos, const bool val = true );
    mtfa_base_bitset & flip();
    mtfa_base_bitset & flip ( const unsigned int pos );
    bool test ( const unsigned int pos );
    bool equal ( const mtfa_base_bitset & s1 );

    bool operator[] ( int pos );
    mtfa_base_bitset & operator= ( const mtfa_base_bitset & s );
    mtfa_base_bitset   operator| ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset & operator|= ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset   operator& ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset & operator&= ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset  operator^ ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset & operator^= ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset  operator- ( const mtfa_base_bitset & s1 );
    mtfa_base_bitset & operator-= ( const mtfa_base_bitset & s1 );

    static int GetBlockSize(){return block_size;};
    int count();
    string toString();

    //Torna array di interi che termina con -1
    list<int> GiveOnes();
    int FirstOne();
    int FirstZero();
    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );
};

class mtfa_extended_bitset
{
private:
    static std::pair < int, int > GetIdx(const int p);
public:
    void clear();
    vector<mtfa_base_bitset*> v;
    mtfa_extended_bitset & resize(int size, bool fill_with_1=false);
    mtfa_extended_bitset();
    mtfa_extended_bitset(const mtfa_extended_bitset & s1);
    ~mtfa_extended_bitset();
    
    mtfa_extended_bitset & set ( const bool val=true );
    mtfa_extended_bitset & set ( const unsigned int pos, const bool val = true );
    mtfa_extended_bitset & flip();
    mtfa_extended_bitset & flip ( const unsigned int pos );
    bool test ( const unsigned int pos );
    bool equal ( const mtfa_extended_bitset & s1 );

    bool operator[] ( int pos );
    mtfa_extended_bitset & operator= ( const mtfa_extended_bitset & s );
    mtfa_extended_bitset   operator| ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset & operator|= ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset   operator& ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset & operator&= ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset  operator^ ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset & operator^= ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset  operator- ( const mtfa_extended_bitset & s1 );
    mtfa_extended_bitset & operator-= ( const mtfa_extended_bitset & s1 );

    int count();
    int size();
    string toString();

    //Torna array di interi che termina con -1
    list<int> GiveOnes();
    int FirstOne();
    int FirstZero();
    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );
};



class mtfa_bitset
{
private:
    unsigned long long *v;
    static const unsigned int allocation_block = 4 << 6;
    unsigned int allocated;
    static bool initialized;
    unsigned int real_size;  //quanti bit ci sono realmente?

    int R64 ( int n );
    void Zero ( int p1, int p2 );
    void Init ( unsigned int n );

public:
    static unsigned long long bits[64];
    static unsigned long long nbits[64];

    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );

    void Initialize();
    mtfa_bitset();
    mtfa_bitset ( mtfa_bitset & );
    ~mtfa_bitset();
    mtfa_bitset ( unsigned int n );
    void reset();
    void resize ( int n );
    unsigned int size ( void );
    void set();
    void set ( unsigned int pos, bool val = true );
    void set ( unordered_set<int> positions, bool val=true );
    mtfa_bitset & flip();
    void flip ( unsigned int pos );
    bool test ( unsigned int pos );
    bool cmp ( mtfa_bitset & s1 );

    mtfa_bitset & operator= ( mtfa_bitset & s );
    bool operator[] ( int pos );
    mtfa_bitset & operator| ( mtfa_bitset & s1 );
    mtfa_bitset & operator|= ( mtfa_bitset & s1 );
    mtfa_bitset operator& ( mtfa_bitset & s1 );
    mtfa_bitset & operator&= ( mtfa_bitset & s1 );
    mtfa_bitset operator^ ( mtfa_bitset & s1 );
    mtfa_bitset & operator^= ( mtfa_bitset & s1 );
    mtfa_bitset operator- ( mtfa_bitset & s1 );
    mtfa_bitset & operator-= ( mtfa_bitset & s1 );

//     static bool HaveCommonItems ( mtfa_bitset & s1, mtfa_bitset & s2 );
//     static int FirstCommonOneOrZero ( vector<mtfa_bitset*> &bsv, bool findOne );

    int count();
    string toString();

    //Torna array di interi che termina con -1
    int * GiveOnes();
    int FirstOne();
    int FirstZero();
};

class mtfa_bitset_1024
{
private:
    uint1024_t one;
    uint1024_t ones;
    uint1024_t *v;
    static const unsigned int allocation_block = 1024;  //ogni volta alloco un blocco da 1024 bit in più, credo sia sufficiente
    unsigned int allocated;
    static bool initialized;
    unsigned int real_size;  //quanti bit ci sono realmente?

    int R1024 ( int n );
    void Zero ( int p1, int p2 );
    void Init ( unsigned int n );

public:
    static uint1024_t bits[1024];
    static uint1024_t nbits[1024];

    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );

    void Initialize();
    mtfa_bitset_1024();
    mtfa_bitset_1024 ( mtfa_bitset_1024 & );
    ~mtfa_bitset_1024();
    mtfa_bitset_1024 ( unsigned int n );
    void reset();
    void resize ( int n );
    unsigned int size ( void );
    void set();
    void set ( unsigned int pos, bool val = true );
    void set ( unordered_set<int> positions, bool val=true );
    mtfa_bitset_1024 & flip();
    void flip ( unsigned int pos );
    bool test ( unsigned int pos );
    bool cmp ( mtfa_bitset_1024 & s1 );

    mtfa_bitset_1024 & operator= ( mtfa_bitset_1024 & s );
    bool operator[] ( int pos );
    mtfa_bitset_1024 & operator| ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 & operator|= ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 operator& ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 & operator&= ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 operator^ ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 & operator^= ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 operator- ( mtfa_bitset_1024 & s1 );
    mtfa_bitset_1024 & operator-= ( mtfa_bitset_1024 & s1 );

    int count();
    string toString();

    //Torna array di interi che termina con -1
    int * GiveOnes();
    int FirstOne();
    int FirstZero();
};


void GetPageFromUrlProxy ( const string      &hostname,
                           const string	   &port,
                           const string      &protocol, 	//http, https
                           const string      &method,	//get, post
                           const string      &url,	//compresa la query
                           const string      &additional_headers,
                           const string      &data,
                           const string      &proxy_name,
                           const string      &proxy_port,
                           int timeout,
                           string &    page,
                           string &    headers,
                           bool check_ssl_server = false,
                           bool check_name_server = false,
                           const string & ca_cert = "",
                           const string & client_cert = "",
                           const string & client_key = ""
                         );


/*
	Base32 encoding / decoding.
	Encode32 outputs at out bytes with values from 0 to 32 that can be mapped to 32 signs.
	Decode32 input is the output of Encode32. The out parameters should be unsigned char[] of
	length GetDecode32Length(inLen) and GetEncode32Length(inLen) respectively.
    To map the output of Encode32 to an alphabet of 32 characters use Map32.
	To unmap back the output of Map32 to an array understood by Decode32 use Unmap32.
	Both Map32 and Unmap32 do inplace modification of the inout32 array.
	The alpha32 array must be exactly 32 chars long.

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

class Base32
{
public:
    static bool Decode32 ( unsigned char* in, int inLen, unsigned char* out );
    static bool Encode32 ( unsigned char* in, int inLen, unsigned char* out );

    static int  GetDecode32Length ( int bytes );
    static int  GetEncode32Length ( int bytes );

    static bool Map32 ( unsigned char* inout32, int inout32Len, unsigned char* alpha32 );
    static bool Unmap32 ( unsigned char* inout32, int inout32Len, unsigned char* alpha32 );
};



namespace fastcrc64
{
#include <inttypes.h>
#include <stdio.h>

static bool crc64speed_init_done = false;

typedef uint64_t t_table[8][256];

typedef uint64_t ( *crcfn64 ) ( uint64_t, const void *, const uint64_t );

/* CRC-64 */
void crcspeed64little_init ( crcfn64 fn, t_table table );
void crcspeed64big_init ( crcfn64 fn, t_table table );
void crcspeed64native_init ( crcfn64 fn, t_table table );

uint64_t crcspeed64little ( t_table table, uint64_t crc, void *buf, size_t len );
uint64_t crcspeed64big ( t_table table, uint64_t crc, void *buf, size_t len );
uint64_t crcspeed64native ( t_table table, uint64_t crc, void *buf, size_t len );

/* Does not require init */
uint64_t crc64 ( uint64_t crc, const void *data, const uint64_t len );
void crc64speed_cache_table ( void );

/* All other crc functions here require _init() before usage. */
bool crc64speed_init ( void );
uint64_t crc64_lookup ( uint64_t crc, const void *in_data, const uint64_t len );
uint64_t crc64speed ( uint64_t crc, const void *s, const uint64_t l );

bool crc64speed_init_big ( void );
uint64_t crc64speed_big ( uint64_t crc, const void *s, const uint64_t l );

bool crc64speed_init_native ( void );
uint64_t crc64speed_native ( uint64_t crc, const void *s, const uint64_t l );

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
static inline uint_fast64_t crc_reflect ( uint_fast64_t data, size_t data_len );

/**
 *  Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 ******************************************************************************/
uint64_t crc64 ( uint_fast64_t crc, const void *in_data, const uint64_t len );

/* Returns false if CRC64SPEED_SAFE and table already initialized. */
bool crc64speed_init ( void );

/* Returns false if CRC64SPEED_SAFE and table already initialized. */
bool crc64speed_init_big ( void );

uint64_t crc64speed ( uint64_t crc, const void *s, const uint64_t l );

uint64_t crc64speed_big ( uint64_t crc, const void *s, const uint64_t l );

bool crc64speed_init_native ( void );

/* Iterate over table to fully load it into a cache near the CPU. */
void crc64speed_cache_table ( void );

/* If you are on a platform where endianness can change at runtime, this
 * will break unless you compile with CRC64SPEED_DUAL and manually run
 * _init() and _init_big() instead of using _init_native() */
uint64_t crc64speed_native ( uint64_t crc, const void *s, const uint64_t l );
}


string mtfa_base_62 ( const unsigned char * p, int len );
int mtfa_debase_62 ( const string n, unsigned char *&p );

string mtfa_base_62 ( unsigned long long n );
unsigned long long mtfa_debase_62 ( const string b62 );

string HexDump ( string s, bool withAscii=true, int cols=16 );
string HexDump ( const unsigned char * p, int len_p, bool withAscii=true, int cols=16 );

unsigned long long mtfa_atoull ( const char * p );
unsigned long long mtfa_atoull ( const string &p );
string mtfa_ulltoa(unsigned long long value, int radix);
string mtfa_lltoa(long long value, int radix);

unsigned long long mtfa_rand_simple_ull ();
string mtfa_rand_string ( int len, int base );
string mtfa_rand_alfanum ( int len, string alphabet );
void mtfa_random_generator_init(unsigned long long seed);


////Classes unordered_map e unordered_set concorrente stringhe, stringhe oppure int, stringhe
//class mtfa_conc_umap_ss
//{
//private:
//    unordered_map<string, string> um;
//    mutex * m;
//public:
//    mtfa_conc_umap_ss();
//    ~mtfa_conc_umap_ss();
//    string find(const string& k);
//    int size();
//    void insert(const string&k, const string&v);
//    void clear();
//};
//class mtfa_conc_umap_is
//{
//private:
//    unordered_map<int, string> um;
//    mutex * m;
//public:
//    mtfa_conc_umap_is();
//    ~mtfa_conc_umap_is();
//    string find(const int& k);
//    int size();
//    void insert(const int&k, const string&v);
//    void clear();
//};
//
//class mtfa_conc_uset_s
//{
//private:
//    unordered_set<string> um;
//    mutex * m;
//public:
//    mtfa_conc_uset_s();
//    ~mtfa_conc_uset_s();
//    bool find(const string& k);
//    int size();
//    void insert(const string&k);
//    void clear();
//};
//class mtfa_conc_uset_i
//{
//private:
//    unordered_set<int> um;
//    mutex * m;
//public:
//    mtfa_conc_uset_i();
//    ~mtfa_conc_uset_i();
//    bool find(const int& k);
//    int size();
//    void insert(const int&k);
//    void clear();
//};
//

/*********************/
#if 0
template <class KEY, class VALUE>
class mtfa_conc_umap
{
public:
    unordered_map<KEY, VALUE> um;
    mutex *m;
public:
    mtfa_conc_umap()
    {
        m = new mutex();
    };
    ~mtfa_conc_umap()
    {
        if ( m ) {
            delete m;
        }
        m = nullptr;
    };

//    // Move initialization
//    mtfa_conc_umap(mtfa_conc_umap&& other) {
//        lock_guard<mutex> lock1(*other.m);
//        lock_guard<mutex> lock2(*m);
//        um = std::move(other.um);
//        m = other.m;
//        other.m = nullptr;
//      }

    // Copy initialization
    mtfa_conc_umap ( const mtfa_conc_umap& other )
    {
        lock_guard<mutex> lock1 ( *other.m );
        lock_guard<mutex> lock2 ( *m );
        um = other.um;
        m = new mutex();
    }

//    // Move assignment
//    mtfa_conc_umap& operator = (mtfa_conc_umap&& other) {
//        lock_guard<mutex> lock1(*other.m);
//        lock_guard<mutex> lock2(*m);
//        um = std::move(other.um);
//        return *this;
//    }

    // Copy assignment
    mtfa_conc_umap& operator = ( const mtfa_conc_umap& other )
    {
        lock_guard<mutex> lock1 ( *other.m );
        lock_guard<mutex> lock2 ( *m );
        um = other.um;
        return *this;
    }

    bool exists ( const KEY& k )
    {
        lock_guard<mutex> lock ( *m );
        auto p = um.find ( k );
        if ( p == um.end() ) {
            return false;
        } else {
            return true;
        }
    }
    VALUE find ( const KEY& k )
    {
        lock_guard<mutex> lock ( *m );
        auto p = um.find ( k );
        if ( p == um.end() ) {
            return VALUE();
        } else {
            return p->second;
        }
    }
    int size()
    {
        lock_guard<mutex> lock ( *m );
        return um.size();
    };
    void insert ( const KEY&k, const VALUE&v )
    {
        lock_guard<mutex> lock ( *m );
        um[k]=v;
    };
    void clear()
    {
        lock_guard<mutex> lock ( *m );
        um.clear();
    }
};

template <class KEY>
class mtfa_conc_uset
{
public:
    unordered_set<KEY> um;
    mutex * m;
public:
    mtfa_conc_uset()
    {
        m = new mutex();
    };
    ~mtfa_conc_uset()
    {
        delete m;
    };
    bool find ( const KEY& k )
    {
        lock_guard<mutex> lock ( *m );
        auto p = um.find ( k );
        if ( p == um.end() ) {
            return false;
        } else {
            return true;
        }
    }
    int size()
    {
        lock_guard<mutex> lock ( *m );
        return um.size();
    };
    void insert ( const KEY&k )
    {
        lock_guard<mutex> lock ( *m );
        um.insert ( k );
    };
    void clear()
    {
        lock_guard<mutex> lock ( *m );
        um.clear();
    };

    void Serialize ( mtfa_binary_stream_save * bs )
    {
        lock_guard<mutex> lock ( *m );
        bs->Append<int> ( ( int ) um.size() );
        for ( auto &it: um ) {
            bs->Append<KEY> ( it );
        }
    };
    void Deserialize ( mtfa_binary_stream_load * bs )
    {
        lock_guard<mutex> lock ( *m );
        um.clear();
        int len = bs->Read<int>();
        for ( int i=0; i<len; ++i ) {
            um.insert ( bs->Read<KEY>() );
        }
    };

};
#endif

//Per la gestione della conversione di enum a interi e viceversa. c++11 e enum classes
template< typename E >
using mtfa_enable_enum_t = typename std::enable_if< std::is_enum<E>::value, typename std::underlying_type<E>::type >::type;


template< typename E >
constexpr inline mtfa_enable_enum_t<E> mtfa_enum_to_int ( E e ) noexcept {
    return static_cast< typename std::underlying_type<E>::type > ( e );
}


template< typename E , typename T>
constexpr inline typename std::enable_if< std::is_enum<E>::value && std::is_integral<T>::value, E >::type
mtfa_int_to_enum ( T value ) noexcept {
    return static_cast<E> ( value );
}

// #include <boost/thread/locks.hpp>
// #include <boost/thread/shared_mutex.hpp>

typedef mtfa::shared_mutex mtfa_type_lock;
typedef mtfa::mtfa_unique_lock mtfa_write_lock;
typedef mtfa::mtfa_shared_lock mtfa_read_lock;

//Due classi per la gestione della concorrenza
template <class T_KEY>
class mtfa_unordered_set {
private:
public:
    mtfa_type_lock m;
public:
    unordered_set<T_KEY> ds;
    void Insert(const T_KEY k)
    {
        mtfa_write_lock l(m);
        ds.insert(k);
    }
    void Clear()
    {
        mtfa_write_lock l(m);
        ds.clear();
    }
    void Delete(const T_KEY k)
    {
        mtfa_write_lock l(m);
        ds.erase(k);
    }
    bool Exists(const T_KEY k)
    {
        mtfa_read_lock l(m);
        return ds.find(k) != ds.end();
    }
    void ForEach_R(std::function<void(const T_KEY&, void*)> f, void * parameter)
    {
        mtfa_read_lock l(m);
        for (auto & it: ds)
            f(it, parameter);
    }
    void ForEach_W(function<void(const T_KEY&, void*)> f, void * parameter)
    {
        mtfa_write_lock l(m);
        for (auto & it: ds)
            f(it, parameter);
    }
    void DeleteIf(function<bool(const T_KEY&, void*)> f, void * parameter)
    {
        mtfa_write_lock l(m);
        auto it = ds.begin();
        if (f(*it, parameter))
            it = ds.erase(it);
        else
            ++it;
    }
    void WriteLock()
    {
        m.lock();
    }
    void WriteUnlock()
    {
        m.unlock();
    }
    void ReadLock()
    {
        m.lock_shared();
    }
    void ReadUnlock()
    {
        m.unlock_shared();
    }
};

template <class T_KEY, class T_VAL>
class mtfa_unordered_map {
private:
    mtfa_type_lock m;
public:
    unordered_map<T_KEY, T_VAL> ds;
    void Insert(const pair<T_KEY, T_VAL> & kv)
    {
        mtfa_write_lock l(m);
        ds.insert(kv);
    }
    void Insert(const T_KEY & k, const T_VAL & v)
    {
        mtfa_write_lock l(m);
        ds.insert(make_pair(k,v));
    }
    void Clear()
    {
        mtfa_write_lock l(m);
        ds.clear();
    }
    bool Exists(const T_KEY k)
    {
        mtfa_read_lock l(m);
        return ds.find(k) != ds.end();
    }
    void ForEach_R(function<void(const T_KEY&, const T_VAL&, void*)> f, void * parameter)
    {
        mtfa_read_lock l(m);
        for (auto & it: ds)
            f(it.first, it.second, parameter);
    }
    void ForEach_W(function<void(const T_KEY&, const T_VAL&, void*)> f, void * parameter)
    {
        mtfa_write_lock l(m);
        for (auto & it: ds)
            f(it.first, it.second, parameter);
    }
    void Delete(const T_KEY k)
    {
        mtfa_write_lock l(m);
        ds.erase(k);
    }
    bool GetValue(const T_KEY & k, T_VAL & v)
    {
        mtfa_read_lock l(m);
        auto found = ds.find(k);
        if (found == ds.end())
            return false;
        v = found->second;
        return true;
    }
    void DeleteIf(function<bool(const T_KEY&, const T_VAL&, void*)> f, void * parameter)
    {
        //cout << __func__ << ". WriteLock" << endl;
        mtfa_write_lock l(m);
        auto it = ds.begin();
        while (it != ds.end())
        {
            if (f(it->first, it->second, parameter))
                it = ds.erase(it);
            else
                ++it;
        }
    }
    void WriteLock()
    {
        //cout << __func__ << ". WriteLock" << endl;
        m.lock();
    }
    void WriteUnlock()
    {
        //cout << __func__ << ". WriteUnlock" << endl;
        m.unlock();
    }
    void ReadLock()
    {
        //cout << __func__ << ". ReadLock" << endl;
        m.lock_shared();
    }
    void ReadUnlock()
    {
        //cout << __func__ << ". ReadUnlock" << endl;
        m.unlock_shared();
    }
};

enum class CookieDurations: int {SessionCookie, NeverExpires, ExpiresInSeconds};
string mtfa_make_cookie(string key, string value, CookieDurations expiration, int howManySecs=0, string path="/", string domain="", bool http_only=false, bool secure=false);
string mtfa_delete_cookie(string key, string path="/", string domain="", bool http_only=false, bool secure=false);


// 
// //Semplice concorrenza
// template <class E>
// class c_unordered_set {
// public:
//     unordered_set<E> ds;
//     mtfa_type_lock lock;
// };
// 
// template <class E, class F>
// class c_unordered_map {
// public:
//     unordered_set<E, F> ds;
//     mtfa_type_lock lock;
// };
// 
// template <class E>
// class c_list {
// public:
//     list<E> ds;
//     mtfa_type_lock lock;
// };
// 
// template <class E>
// class c_vector {
// public:
//     vector<E> ds;
//     mtfa_type_lock lock;
// };

//Alcune semplici funzioni SCM
SCM ScmFromString ( const char* s );
SCM ScmFromStringn ( const char* s, const int len );

char* ScmToStringn ( SCM s, size_t* len );
char* ScmToString ( SCM s );


string mtfa_gen_uuid(string p1, string p2);

bool mtfa_compress ( const string & sin, string & sout, int level);
bool mtfa_uncompress ( const string & sin, string & sout);

/*
 * BSetUnion e generatore kolmogorof
 */
class BSetUnion
{
    //in questa versione utilizziamo int64 per memorizzare i posti liberi e occupati
    //quindi 2 livelli: vettore e uint64. Inoltre aggiungiamo una funzione di accumulazione dei bit in un cluster da 64,
    //avendo cura di utilizzare solo il risultato finale dopo 64 insert
private:
    int bitAfterLastBitLastCall;
    unsigned int oldAddress;
public:

    //Da 0 a 63. Seleziona tutti i bit più significativi di quello indicato. 0 indica prendi dallo 0 al 63
    static constexpr unsigned long long /*__int128*/ BitMaskLH[] =
    {
        0XFFFFFFFFFFFFFFFFULL, 0XFFFFFFFFFFFFFFFEULL, 0XFFFFFFFFFFFFFFFCULL, 0XFFFFFFFFFFFFFFF8ULL, 0XFFFFFFFFFFFFFFF0ULL, 
        0XFFFFFFFFFFFFFFE0ULL, 0XFFFFFFFFFFFFFFC0ULL, 0XFFFFFFFFFFFFFF80ULL, 0XFFFFFFFFFFFFFF00ULL, 0XFFFFFFFFFFFFFE00ULL, 
        0XFFFFFFFFFFFFFC00ULL, 0XFFFFFFFFFFFFF800ULL, 0XFFFFFFFFFFFFF000ULL, 0XFFFFFFFFFFFFE000ULL, 0XFFFFFFFFFFFFC000ULL, 
        0XFFFFFFFFFFFF8000ULL, 0XFFFFFFFFFFFF0000ULL, 0XFFFFFFFFFFFE0000ULL, 0XFFFFFFFFFFFC0000ULL, 0XFFFFFFFFFFF80000ULL, 
        0XFFFFFFFFFFF00000ULL, 0XFFFFFFFFFFE00000ULL, 0XFFFFFFFFFFC00000ULL, 0XFFFFFFFFFF800000ULL, 0XFFFFFFFFFF000000ULL, 
        0XFFFFFFFFFE000000ULL, 0XFFFFFFFFFC000000ULL, 0XFFFFFFFFF8000000ULL, 0XFFFFFFFFF0000000ULL, 0XFFFFFFFFE0000000ULL, 
        0XFFFFFFFFC0000000ULL, 0XFFFFFFFF80000000ULL, 0XFFFFFFFF00000000ULL, 0XFFFFFFFE00000000ULL, 0XFFFFFFFC00000000ULL, 
        0XFFFFFFF800000000ULL, 0XFFFFFFF000000000ULL, 0XFFFFFFE000000000ULL, 0XFFFFFFC000000000ULL, 0XFFFFFF8000000000ULL, 
        0XFFFFFF0000000000ULL, 0XFFFFFE0000000000ULL, 0XFFFFFC0000000000ULL, 0XFFFFF80000000000ULL, 0XFFFFF00000000000ULL, 
        0XFFFFE00000000000ULL, 0XFFFFC00000000000ULL, 0XFFFF800000000000ULL, 0XFFFF000000000000ULL, 0XFFFE000000000000ULL, 
        0XFFFC000000000000ULL, 0XFFF8000000000000ULL, 0XFFF0000000000000ULL, 0XFFE0000000000000ULL, 0XFFC0000000000000ULL, 
        0XFF80000000000000ULL, 0XFF00000000000000ULL, 0XFE00000000000000ULL, 0XFC00000000000000ULL, 0XF800000000000000ULL, 
        0XF000000000000000ULL, 0XE000000000000000ULL, 0XC000000000000000ULL, 0X8000000000000000ULL
    };
    
    //Da 0 a 63. Seleziona tutti i bit meno significativi di quello dato. 0: seleziona il meno significativo
    //63 prendili dal bit 63 al bit 0
    static constexpr unsigned long long BitMaskHL[] =
    {
        0X0000000000000001ULL, 0X0000000000000003ULL, 0X0000000000000007ULL, 0X000000000000000FULL, 0X000000000000001FULL,
        0X000000000000003FULL, 0X000000000000007FULL, 0X00000000000000FFULL, 0X00000000000001FFULL, 0X00000000000003FFULL,
        0X00000000000007FFULL, 0X0000000000000FFFULL, 0X0000000000001FFFULL, 0X0000000000003FFFULL, 0X0000000000007FFFULL,
        0X000000000000FFFFULL, 0X000000000001FFFFULL, 0X000000000003FFFFULL, 0X000000000007FFFFULL, 0X00000000000FFFFFULL,
        0X00000000001FFFFFULL, 0X00000000003FFFFFULL, 0X00000000007FFFFFULL, 0X0000000000FFFFFFULL, 0X0000000001FFFFFFULL,
        0X0000000003FFFFFFULL, 0X0000000007FFFFFFULL, 0X000000000FFFFFFFULL, 0X000000001FFFFFFFULL, 0X000000003FFFFFFFULL,
        0X000000007FFFFFFFULL, 0X00000000FFFFFFFFULL, 0X00000001FFFFFFFFULL, 0X00000003FFFFFFFFULL, 0X00000007FFFFFFFFULL,
        0X0000000FFFFFFFFFULL, 0X0000001FFFFFFFFFULL, 0X0000003FFFFFFFFFULL, 0X0000007FFFFFFFFFULL, 0X000000FFFFFFFFFFULL,
        0X000001FFFFFFFFFFULL, 0X000003FFFFFFFFFFULL, 0X000007FFFFFFFFFFULL, 0X00000FFFFFFFFFFFULL, 0X00001FFFFFFFFFFFULL,
        0X00003FFFFFFFFFFFULL, 0X00007FFFFFFFFFFFULL, 0X0000FFFFFFFFFFFFULL, 0X0001FFFFFFFFFFFFULL, 0X0003FFFFFFFFFFFFULL,
        0X0007FFFFFFFFFFFFULL, 0X000FFFFFFFFFFFFFULL, 0X001FFFFFFFFFFFFFULL, 0X003FFFFFFFFFFFFFULL, 0X007FFFFFFFFFFFFFULL,
        0X00FFFFFFFFFFFFFFULL, 0X01FFFFFFFFFFFFFFULL, 0X03FFFFFFFFFFFFFFULL, 0X07FFFFFFFFFFFFFFULL, 0X0FFFFFFFFFFFFFFFULL,
        0X1FFFFFFFFFFFFFFFULL, 0X3FFFFFFFFFFFFFFFULL, 0X7FFFFFFFFFFFFFFFULL, 0XFFFFFFFFFFFFFFFFULL
    };
    
    static constexpr unsigned long long SetBitMask[] =
    {
        0X0000000000000001ULL, 0X0000000000000002ULL, 0X0000000000000004ULL, 0X0000000000000008ULL, 0X0000000000000010ULL,
        0X0000000000000020ULL, 0X0000000000000040ULL, 0X0000000000000080ULL, 0X0000000000000100ULL, 0X0000000000000200ULL,
        0X0000000000000400ULL, 0X0000000000000800ULL, 0X0000000000001000ULL, 0X0000000000002000ULL, 0X0000000000004000ULL,
        0X0000000000008000ULL, 0X0000000000010000ULL, 0X0000000000020000ULL, 0X0000000000040000ULL, 0X0000000000080000ULL,
        0X0000000000100000ULL, 0X0000000000200000ULL, 0X0000000000400000ULL, 0X0000000000800000ULL, 0X0000000001000000ULL,
        0X0000000002000000ULL, 0X0000000004000000ULL, 0X0000000008000000ULL, 0X0000000010000000ULL, 0X0000000020000000ULL, 0X0000000040000000ULL,
        0X0000000080000000ULL, 0X0000000100000000ULL, 0X0000000200000000ULL, 0X0000000400000000ULL, 0X0000000800000000ULL, 0X0000001000000000ULL,
        0X0000002000000000ULL, 0X0000004000000000ULL, 0X0000008000000000ULL, 0X0000010000000000ULL, 0X0000020000000000ULL, 0X0000040000000000ULL,
        0X0000080000000000ULL, 0X0000100000000000ULL, 0X0000200000000000ULL, 0X0000400000000000ULL, 0X0000800000000000ULL, 0X0001000000000000ULL,
        0X0002000000000000ULL, 0X0004000000000000ULL, 0X0008000000000000ULL, 0X0010000000000000ULL, 0X0020000000000000ULL, 0X0040000000000000ULL,
        0X0080000000000000ULL, 0X0100000000000000ULL, 0X0200000000000000ULL, 0X0400000000000000ULL, 0X0800000000000000ULL, 0X1000000000000000ULL,
        0X2000000000000000ULL, 0X4000000000000000ULL, 0X8000000000000000ULL
    };
    
    static const unsigned long long fullCluster = 0xFFFFFFFFFFFFFFFF;
    int size;   //quanti ce ne sono in totale
    unsigned long long total;
    typedef struct __range__
    {
        int size;
        int firstFreeOnRight;
        int lastBusyOnLeft;
        __range__()
        {
            size = 0;
            firstFreeOnRight = 0;
            lastBusyOnLeft = 0;
        }
    } TRange;
    TRange** ranges;
    TRange* the_ranges;
    unsigned long long* values_left;
    unsigned char* occupied_left;
    unsigned long long* values_right;
    unsigned char* occupied_right;
    unsigned char* occupied_lr;
    pair<int, int> Index ( int idx );

    void Dump();
    //Il size è in bit!
    int Occupied ( const int i );
    void setBitValue( const unsigned int index, const bool value );
    BSetUnion ( int _size );
    ~BSetUnion();
    //Inserisce a un indirizzo di bit, all'interno lo converto in indirizzo di cluster (64bit)
    bool InsertValue ( int _index, bool value, bool right); //right means: grow to right otherwise grow to left
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
    int getAddressOfNextBit ( const unsigned int index, const bool occupied, const bool avanti );

    //presuppongo che il bit sia occupato!!!
    bool getBitValue ( const unsigned int index );
    unsigned int getNBitsFromIndex ( const int idx, int numBits );

    //da 0 a 63
    void SetBit ( unsigned long long& w, //the word to modify:  if (f) w |= m; else w &= ~m;
                         const unsigned int _m, // the bit mask
                         const bool f           // conditional flag
    );
    pair<int, int> GetIndex ( int i );
    
    list<pair<int, int>> EnumerateSets();
	
	boost::dynamic_bitset<> ToBitSet();
};


/********************************************************/
/********************************************************/
//      La sezione RLP per ethereum
/********************************************************/
/********************************************************/

std::vector<unsigned char> ParseHex(const char* psz);
std::vector<unsigned char> ParseHex(const std::string& str);
signed char HexDigit(char c);
/* Returns true if each character in str is a hex character, and has an even
 * number of hex digits.*/
bool IsHex(const std::string& str);
/**
* Return true if the string is a hex number, optionally prefixed with "0x"
*/
bool IsHexNumber(const std::string& str);
std::vector<unsigned char> DecodeBase64(const char* p, bool* pfInvalid = nullptr);
std::string DecodeBase64(const std::string& str);
std::string EncodeBase64(const unsigned char* pch, size_t len);
std::string EncodeBase64(const std::string& str);

template<typename T>
std::string HexStr(const T itbegin, const T itend, bool fSpaces=false)
{
    std::string rv;
    static const char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    rv.reserve((itend-itbegin)*3);
    for(T it = itbegin; it < itend; ++it)
    {
        unsigned char val = (unsigned char)(*it);
        if(fSpaces && it != itbegin)
            rv.push_back(' ');
        rv.push_back(hexmap[val>>4]);
        rv.push_back(hexmap[val&15]);
    }

    return rv;
}

template<typename T>
inline std::string HexStr(const T& vch, bool fSpaces=false)
{
    return HexStr(vch.begin(), vch.end(), fSpaces);
}




/*********************I WebSockets....*******************/
#if 0
/****La classe dei web socket */
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>


/*****************Per la gestione dei Web Socket *****************/
/*
 * Nel mondo esterno ho un produttore di dati (che provengono dalla rete)
 * un consumatore di dati (che scrive sulla rete), 
 * e un server applicativo che riceve dati applicativi e restituisce dati applicativi in modo sincronico
 * Il produttore di dati chiama la manageio dei websocket.
 * La manageio estrae la componente di payload e chiama il server applicativo
 * I dati in uscita al server applicativo sono incapsulati nel protocollo websocket e sono forniti al consumatore dei dati (la rete)
 * */

typedef websocketpp::server<websocketpp::config::core> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

#include <condition_variable>
#include <unistd.h>

class MyWebSocketQueue {
private:
    queue<string> q;
    mutex m;
    std::condition_variable cv;
    bool terminate;
public:
    bool data_ready;
    MyWebSocketQueue () {
        data_ready = false;
        terminate=false;
    }
    void stop() {
        terminate=true;
        cv.notify_all();
    }
    string front() {
//        std::lock_guard<std::mutex> lck ( m ); //per evitare collisioni sullo stesso websocket
       std::unique_lock<std::mutex> lk(m);
        while (q.empty()) {
            cv.wait(lk);
            if (terminate)
                return("");
        }
        auto f = q.front();
        q.pop();
        lk.unlock();
        return f;
    }
    void push(const string & v) {
        std::lock_guard<std::mutex> lck ( m ); //per evitare collisioni sullo stesso websocket
        q.push(v);
        cv.notify_all();
    }
//     void pop() {
//         std::lock_guard<std::mutex> lck ( m ); //per evitare collisioni sullo stesso websocket
//         q.pop();
//     }
    bool empty() {
        std::lock_guard<std::mutex> lck ( m ); //per evitare collisioni sullo stesso websocket
        return q.empty();
    }
};

class MyWebSocket{
private:
    void ioManager();
public:
    //pull out the type of messages sent by our config
    typedef websocketpp::server<websocketpp::config::core> server;
    typedef server::message_ptr message_ptr;

    bool terminate;
    server the_server;
    websocketpp::connection_hdl con_hdl;
    
    void * user_data;
    
    MyWebSocketQueue q_out;  //Il mondo esterno si sincronizza su q_out per ottenere i dati da inviare!!!
    MyWebSocketQueue q_in;   //Il mondo esterno si sincronizza su q_in quando riceve nuovi dati!!!

    server::connection_ptr con;

    string oldData;
    
    bool closed;
    bool catchedAnError;

    //la callback da registrare
    pair<string, bool> (*cb)(const string & data, bool & data_type, void * user_data);
    
    void prepare_to_send(const pair<string, bool> & data);
    
    mutex mymutex;

    //A questa passo la cb per elaborare i dati applicativi
    MyWebSocket(pair<string, bool> (*_cb)(const string & data, bool & data_type, void * user_data), void * user_data=nullptr);

    ~MyWebSocket() {
        terminate=true;
        q_out.stop();
        q_in.stop();
    }

    bool isClosed();
    bool gotAnError();
    void resetError();
    void close();
};
#endif

