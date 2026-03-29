#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <list>
#include <string>
#include <stack>
#include <memory>
#include <functional>
#include <unordered_map>
#include <x86intrin.h>
#include <string.h>
#include <algorithm>

using namespace std;

#include "morton.h"
#include "mtfa_qt.h"


#define EMPTY_REGION 0xFFFFFFFF

unsigned long long roundUpToNextPowerOfTwo ( unsigned int x );
unsigned int int_log2 ( unsigned int v );

/*********************INIZIO LINQT*********************/

std::string int_to_s_bin ( unsigned int i );

typedef std::pair<unsigned int, unsigned int> T_2I;

//namespace std
//{
//
//template <>
//struct hash<T_2I>
//{
//
//    std::size_t operator()(const T_2I & k) const
//    {
//        using std::size_t;
//        using std::hash;
//        return hash<unsigned long long>()((((unsigned long long) k.first) << 32) | k.second);
//    }
//};
//
//}
typedef std::unordered_map<T_2I, double> T_LqtIntersections;

//la storia di una regione è uno stack (deque).
//ogni nuovo insieme di regioni è indirizzato per id-regione e a ogni id-regione c'è uno stack associato

typedef struct __dati_regione__ {
     unsigned int id_regione;
     int area;
     int asx, asy, bdx, bdy;
     int cx, cy;
     unsigned int id_base;
     time_t when;
} T_DatiRegione;
typedef std::unordered_map<unsigned int, T_DatiRegione> T_DatiRegioni;

typedef std::deque<T_DatiRegione> T_StoricoDatiRegione;
typedef std::unordered_map<unsigned int, T_StoricoDatiRegione> T_StoricoDatiRegioni;

void RemoveRegions ( T_Lqt & lqt, std::set<unsigned int> & regions_to_be_removed, int idbase );

int ListLinearQTintersections ( T_Lqt &lqt1, T_Lqt &lqt2, T_LqtIntersections& );

// void GetRC_FromMorton ( unsigned int mor, unsigned int & r, unsigned int & c );
// unsigned int GetMortonFromRC ( unsigned int r, unsigned int c );


//FUNZIONI DI UTILITA' E SUPPORTO AI QT

unsigned long long roundUpToNextPowerOfTwo ( unsigned int i_x )
{
     unsigned long long x = i_x;
     x--;
     x |= x >> 1;
     x |= x >> 2;
     x |= x >> 4;
     x |= x >> 8;
     x |= x >> 16;
     x++;

     return x;
}

unsigned int int_log2 ( unsigned int v )
{
     const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
     const unsigned int S[] = {1, 2, 4, 8, 16};
     int i;

     unsigned int r = 0; // result of log2(v) will go here
     for ( i = 4; i >= 0; i-- ) { // unroll for speed...
          if ( v & b[i] ) {
               v >>= S[i];
               r |= S[i];
          }
     }
     return r;
}

/*********************INIZIO LINQT*********************/

const unsigned short MortonTable256[256] = {
     0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015, 0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055,
     0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115, 0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155,
     0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415, 0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455,
     0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515, 0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555,
     0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015, 0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
     0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115, 0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155,
     0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415, 0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455,
     0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515, 0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555,
     0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015, 0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055,
     0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115, 0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
     0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415, 0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455,
     0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515, 0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555,
     0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015, 0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055,
     0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115, 0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155,
     0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415, 0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
     0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515, 0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555
};

const unsigned char mor_dispari[256] = {
     0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4,
     5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 0, 0, 1, 1,
     0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 4, 4, 5, 5, 4, 4,
     5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 4, 4, 5, 5, 4, 4, 5, 5, 6, 6, 7, 7, 6, 6, 7, 7, 8, 8, 9, 9, 8, 8, 9, 9,
     10, 10, 11, 11, 10, 10, 11, 11, 8, 8, 9, 9, 8, 8, 9, 9, 10, 10, 11, 11, 10, 10, 11, 11, 12, 12, 13, 13, 12,
     12, 13, 13, 14, 14, 15, 15, 14, 14, 15, 15, 12, 12, 13, 13, 12, 12, 13, 13, 14, 14, 15, 15, 14, 14, 15, 15, 8,
     8, 9, 9, 8, 8, 9, 9, 10, 10, 11, 11, 10, 10, 11, 11, 8, 8, 9, 9, 8, 8, 9, 9, 10, 10, 11, 11, 10, 10, 11, 11, 12,
     12, 13, 13, 12, 12, 13, 13, 14, 14, 15, 15, 14, 14, 15, 15, 12, 12, 13, 13, 12, 12, 13, 13, 14, 14, 15, 15, 14,
     14, 15, 15,
};

const unsigned char mor_pari[256] = {
     0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7, 0, 1, 0, 1, 2, 3, 2, 3,
     0, 1, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11,
     10, 11, 12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 15, 8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11,
     10, 11, 12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 15, 0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
     4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 7, 0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7,
     4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12,
     13, 14, 15, 14, 15, 8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14,
     15, 14, 15,
};

#if 0
//Dobbiamo rifare il morton
#include <bitset>
unsigned int our_pdep_u32 ( unsigned int a, unsigned int uimask );
unsigned int our_pdep_u32 ( unsigned int a, unsigned int uimask )
{
     std::bitset<32> tmp ( a );
     std::bitset<32>dst ( 0 );
     std::bitset<32> mask ( uimask );
     unsigned int m = 0;
     unsigned int k = 0;
     while ( m < 32 ) {
          if ( mask[m] ) {
               dst[m]=tmp[k];
               k++;
          }
          m++;
     }
     return dst.to_ulong();
}
unsigned int our_pext_u32 ( unsigned int a, unsigned int uimask );
unsigned int our_pext_u32 ( unsigned int a, unsigned int uimask )
{
     std::bitset<32>tmp ( a );
     std::bitset<32> dst ( 0 );
     std::bitset<32> mask ( uimask );
     unsigned int m = 0;
     unsigned int k = 0;
     while ( m < 32 ) {
          if ( mask[m] ) {
               dst[k] = tmp[m];
               k++;
          }
          m++;
     }
     return dst.to_ulong();
}


unsigned int MortonEncode ( unsigned int y, unsigned int x )
{
     return our_pdep_u32 ( y, 0xAAAAAAAA ) | our_pdep_u32 ( x, 0x55555555 );
}


unsigned int MortonEncode ( unsigned int z, unsigned int y, unsigned int x )
{
     return our_pdep_u32 ( y, 0x24924924 ) | our_pdep_u32 ( y, 0x12492492 ) | our_pdep_u32 ( x, 0x09249249 );
}

void MortonDecode ( unsigned int code, unsigned int &outY, unsigned int &outX )
{
     outX = our_pext_u32 ( code, 0x55555555 );
     outY = our_pext_u32 ( code, 0xAAAAAAAA );
}

void MortonDecode ( unsigned int code, unsigned int &outZ, unsigned int &outY, unsigned int &outX )
{
     outX = our_pext_u32 ( code, 0x09249249 );
     outY = our_pext_u32 ( code, 0x12492492 );
     outZ = our_pext_u32 ( code, 0x24924924 );
}

unsigned int MortonAdd ( unsigned int code, unsigned int r, unsigned int c )
{
     unsigned int r0, c0;
     MortonDecode ( code, r0, c0 );
     r0 += r;
     c0 += c;
     return MortonEncode ( r0, c0 );
}
#endif

unsigned int MortonEncode( unsigned short r, unsigned short c )
{
     return MortonTable256[r >> 8] << 17 |
            MortonTable256[c >> 8] << 16 |
            MortonTable256[r & 0xFF] << 1 |
            MortonTable256[c & 0xFF];
}

void MortonDecode ( unsigned int mor, unsigned short & r, unsigned short & c )
{
     r = mor_dispari[0xFF & ( mor >> 24 )] << 12 |
         mor_dispari[0xFF & ( mor >> 16 )] << 8 |
         mor_dispari[0xFF & ( mor >> 8 )] << 4 |
         mor_dispari[0xFF & ( mor )];
     c = mor_pari[0xFF & ( mor >> 24 )] << 12 |
         mor_pari[0xFF & ( mor >> 16 )] << 8 |
         mor_pari[0xFF & ( mor >> 8 )] << 4 |
         mor_pari[0xFF & ( mor )];
}

unsigned int MortonAdd ( unsigned int code, unsigned short r, unsigned short c )
{
	unsigned short r0, c0;
	MortonDecode ( code, r0, c0 );
	r0 += r;
	c0 += c;
	return MortonEncode ( r0, c0 );
}

unsigned long long MortonEncode64 ( unsigned int x, unsigned int y )
{
	return libmorton::morton2D_64_encode(x, y);
}

void MortonDecode64 ( unsigned long long code, unsigned int &outX, unsigned int &outY )
{
	uint_fast32_t r, c;
	libmorton::morton2D_64_decode(code, r, c);
	r &= 0x00000000FFFFFFFF;
	c &= 0x00000000FFFFFFFF;
}

unsigned long long MortonAdd64 ( unsigned long long code, unsigned int r, unsigned int c )
{
	unsigned int r0, c0;
	MortonDecode64 ( code, r0, c0 );
	r0 += r;
	c0 += c;
	return MortonEncode64 ( r0, c0 );
}

std::string int_to_s_bin ( unsigned int i )
{
     std::string s;
     unsigned int mask = 0x80000000;
     while ( mask ) {
          if ( mask & i ) {
               s += '1';
          } else {
               s += '0';
          }
          mask >>= 1;
     }
     return s;
}

static double negative_power_of_2[32] = {
     1.0 / ( double ) ( 1 << 0 ), 1.0 / ( double ) ( 1 << 1 ), 1.0 / ( double ) ( 1 << 2 ), 1.0 / ( double ) ( 1 << 3 ),
     1.0 / ( double ) ( 1 << 4 ), 1.0 / ( double ) ( 1 << 5 ), 1.0 / ( double ) ( 1 << 6 ), 1.0 / ( double ) ( 1 << 7 ),
     1.0 / ( double ) ( 1 << 8 ), 1.0 / ( double ) ( 1 << 9 ), 1.0 / ( double ) ( 1 << 10 ), 1.0 / ( double ) ( 1 << 11 ),
     1.0 / ( double ) ( 1 << 12 ), 1.0 / ( double ) ( 1 << 13 ), 1.0 / ( double ) ( 1 << 14 ), 1.0 / ( double ) ( 1 << 15 ),
     1.0 / ( double ) ( 1 << 16 ), 1.0 / ( double ) ( 1 << 17 ), 1.0 / ( double ) ( 1 << 18 ), 1.0 / ( double ) ( 1 << 19 ),
     1.0 / ( double ) ( 1 << 20 ), 1.0 / ( double ) ( 1 << 21 ), 1.0 / ( double ) ( 1 << 22 ), 1.0 / ( double ) ( 1 << 23 ),
     1.0 / ( double ) ( 1 << 24 ), 1.0 / ( double ) ( 1 << 25 ), 1.0 / ( double ) ( 1 << 26 ), 1.0 / ( double ) ( 1 << 27 ),
     1.0 / ( double ) ( 1 << 28 ), 1.0 / ( double ) ( 1 << 29 ), 1.0 / ( double ) ( 1 << 30 ), 1.0 / ( double ) ( 1 << 31 ),
};

int CTZ ( unsigned int v )
{
#ifndef _WIN32
     int __builtin_ctz ( unsigned int );
     if ( v == 0 ) {
          return 32;
     } else {
          return __builtin_ctz ( v );
     }
#else
     DWORD trailing_zero = 0;

     if ( _BitScanForward ( &trailing_zero, v ) ) {
          return trailing_zero;
     } else {
          // This is undefined, I better choose 32 than 0
          return 32;
     }
#endif
}

//DecomposeRect lo decompone in quadrati tutti potenza di 2

//Ottimo!
int BuildLinearQT ( T_Lqt &mnodi, int lato, int as_r, int as_c, int bd_r, int bd_c, int idbase )
{
     long long int side = roundUpToNextPowerOfTwo ( lato );
     unsigned int logside = int_log2 ( side );

     //Ritagliamo rispetto al lato in modo da ottenere i quadratini elementari.
     std::list<std::pair<int, int>> decompose_rows;
     while ( as_r <= bd_r ) {
          //quanti zeri a destra
          int qzd = CTZ ( as_r );
          while ( ( ( 1ULL << qzd ) + as_r - 1 ) > bd_r ) {
               --qzd;
          }

          decompose_rows.push_back ( make_pair ( as_r, 1 << qzd ) );
          as_r += ( 1 << qzd );
     }

     //lo stesso faccio sulle colonne
     std::list<std::pair<int, int>> decompose_cols;
     while ( as_c <= bd_c ) {
          //quanti zeri a destra
          int qzd = CTZ ( as_c );
          while ( ( ( 1ULL << qzd ) + as_c - 1 ) > bd_c ) {
               --qzd;
          }

          decompose_cols.push_back ( make_pair ( as_c, 1 << qzd ) );
          as_c += ( 1 << qzd );
     }

     //ora che ho decomposto, posso ottenere i valori
     //int rects = 0;
     for ( auto itr : decompose_rows ) {
          for ( auto itc : decompose_cols ) {
               //Devo normalizzare anche qui!
			  std::function<void ( unsigned short r, unsigned short c, int h, int l ) > riduci = [&] ( int r, int c, int h, int l ) {
                    if ( h == l ) {
                         //rects++;
                         //E se mi portassi r,c e dimensione del lato? che me ne faccio di morton?
                         //morton mi serve poiché mi fornisce l'ordinamento, è l'indice in alto a sinistra
                         //ma comunque non mi serve la stringa
                         int mor = MortonEncode ( r, c ); // GetMortonFromRC(r, c);
                         mnodi.insert ( make_tuple ( mor, h, idbase ) );
                    } else if ( h > l ) {
                         riduci ( r, c, h / 2, l );
                         riduci ( r + h / 2, c, h / 2, l );
                    } else {
                         riduci ( r, c, h, l / 2 );
                         riduci ( r, c + l / 2, h, l / 2 );
                    }
               };
               riduci ( itr.first, itc.first, itr.second, itc.second );
          }
     }
     //cout << __func__   << "Quadratini: " << rects << endl;
     //Devo ordinare rispetto a mor
//    mnodi.sort(
//               []
//               (const std::tuple<std::string, unsigned int, int> & v1,
//               const std::tuple<std::string, unsigned int, int> & v2)
//               {
//                   return get<1>(v1) < get<1>(v2);
//               });
     return 0;
}

//int BuildLinearQT(T_Lqt &mnodi, int ** m, int rows, int cols, unsigned int idbase)
//{
//    int channels = 1;
//    cols /= channels;
//    long long int side = max(rows, cols);
//    side = roundUpToNextPowerOfTwo(side);
//
//    unsigned int logside = int_log2(side);
//
//    for (unsigned int mor = 0; mor < (side * side);)
//    {
//        //Prendo quattro celle
//        unsigned int r0 = mor_dispari[0xFF & (mor >> 24)] << 12 | mor_dispari[0xFF & (mor >> 16)] << 8 | mor_dispari[0xFF & (mor >> 8)] << 4 | mor_dispari[0xFF & (mor)];
//        unsigned int c0 = mor_pari[0xFF & (mor >> 24)] << 12 | mor_pari[0xFF & (mor >> 16)] << 8 | mor_pari[0xFF & (mor >> 8)] << 4 | mor_pari[0xFF & (mor)];
//        mor++;
//        unsigned int r1 = mor_dispari[0xFF & (mor >> 24)] << 12 | mor_dispari[0xFF & (mor >> 16)] << 8 | mor_dispari[0xFF & (mor >> 8)] << 4 | mor_dispari[0xFF & (mor)];
//        unsigned int c1 = mor_pari[0xFF & (mor >> 24)] << 12 | mor_pari[0xFF & (mor >> 16)] << 8 | mor_pari[0xFF & (mor >> 8)] << 4 | mor_pari[0xFF & (mor)];
//        mor++;
//        unsigned int r2 = mor_dispari[0xFF & (mor >> 24)] << 12 | mor_dispari[0xFF & (mor >> 16)] << 8 | mor_dispari[0xFF & (mor >> 8)] << 4 | mor_dispari[0xFF & (mor)];
//        unsigned int c2 = mor_pari[0xFF & (mor >> 24)] << 12 | mor_pari[0xFF & (mor >> 16)] << 8 | mor_pari[0xFF & (mor >> 8)] << 4 | mor_pari[0xFF & (mor)];
//        mor++;
//        unsigned int r3 = mor_dispari[0xFF & (mor >> 24)] << 12 | mor_dispari[0xFF & (mor >> 16)] << 8 | mor_dispari[0xFF & (mor >> 8)] << 4 | mor_dispari[0xFF & (mor)];
//        unsigned int c3 = mor_pari[0xFF & (mor >> 24)] << 12 | mor_pari[0xFF & (mor >> 16)] << 8 | mor_pari[0xFF & (mor >> 8)] << 4 | mor_pari[0xFF & (mor)];
//        mor++;
//
//        //per definizione questi quattro elementi sono appartenenti a un clusterino di 4 pixel che potrei compattare
//
//        //in questo modo io costruisco il QT in modo lineare.
//        unsigned int nw = (r0 < rows && c0 < cols) ? (m[r0][c0]) : 0;
//        unsigned int ne = (r1 < rows && c1 < cols) ? (m[r1][c1]) : 0;
//        unsigned int sw = (r2 < rows && c2 < cols) ? (m[r2][c2]) : 0;
//        unsigned int se = (r3 < rows && c3 < cols) ? (m[r3][c3]) : 0;
//        int side = 1;
//        if (nw == ne && sw == se && nw == sw)
//        {
//            if (nw)
//            {
////                std::string snum = int_to_s_bin(mor - 4);
////                //devo prendere i primi logside-2 bit
////                snum = snum.substr(snum.length() - 2 * logside + 2);
//                mnodi.insert(std::make_tuple(mor - 4, side, idbase + nw));
//
//                unsigned int morappo = mor;
//
//                while (((morappo >> 2) << 2) == morappo)
//                {
//                    //se mor è tale da soddisfare la (((mor>>2)<<2)==mor) ovvero che mor/4 è intero,
//                    //allora provo a vedere gli ultimi quattro elementi e se sono compatibili, li metto insieme
//                    //quello corrente, per definizione, è sempre un sud est
//                    auto p_se = mnodi.end();
//                    p_se--;
//                    auto p_sw = p_se;
//                    p_sw--;
//                    auto p_ne = p_sw;
//                    p_ne--;
//                    auto p_nw = p_ne;
//                    p_nw--;
//
//                    if (p_nw == mnodi.end())
//                        break;
//
//                    //Soddisfano la condizione di pari lunghezza e eguaglianza dei pattern precedenti l'ultimo?
////                    std::string s0 = std::get<0>(*p_nw);
////                    std::string s1 = std::get<0>(*p_ne);
////                    std::string s2 = std::get<0>(*p_sw);
////                    std::string s3 = std::get<0>(*p_se);
//                    int l0 = (int) s0.size();
//                    int l1 = (int) s1.size();
//                    int l2 = (int) s2.size();
//                    int l3 = (int) s3.size();
//                    if (l0 == l1 && l2 == l3 && l0 == l2 &&
//                            (s0.substr(0, l0 - 4).compare(s1.substr(0, l1 - 4)) == 0) &&
//                            (s2.substr(0, l2 - 4).compare(s3.substr(0, l3 - 4)) == 0) &&
//                            (s0.substr(0, l0 - 4).compare(s3.substr(0, l3 - 4)) == 0)
//                            )
//                    {
//                        std::get<0>(*p_nw) = s0.substr(0, l0 - 4) + "00";
//                        mnodi.erase(p_ne);
//                        mnodi.erase(p_sw);
//                        mnodi.erase(p_se);
//                        morappo >>= 2;
//                    }
//                    else
//                        break; //Nulla da fare
//                }
//            }
//        }
//        else
//        {
//            if (nw)
//            {
//                std::string snum = int_to_s_bin(mor - 4);
//                snum = snum.substr(snum.length() - 2 * logside);
//                //mnodi[snum] = std::make_pair(nw, true);
//                mnodi.push_back(std::make_tuple(snum, mor - 4, idbase + nw));
//            }
//            if (ne)
//            {
//                std::string snum = int_to_s_bin(mor - 3);
//                snum = snum.substr(snum.length() - 2 * logside);
//                //mnodi[snum] = std::make_pair(ne, true);
//                mnodi.push_back(std::make_tuple(snum, mor - 3, idbase + ne));
//            }
//            if (sw)
//            {
//                std::string snum = int_to_s_bin(mor - 2);
//                snum = snum.substr(snum.length() - 2 * logside);
//                //mnodi[snum] = std::make_pair(sw, true);
//                mnodi.push_back(std::make_tuple(snum, mor - 2, idbase + sw));
//            }
//            if (se)
//            {
//                std::string snum = int_to_s_bin(mor - 1);
//                snum = snum.substr(snum.length() - 2 * logside);
//                //mnodi[snum] = std::make_pair(se, true);
//                mnodi.push_back(std::make_tuple(snum, mor - 1, idbase + se));
//            }
//        }
//    }
//    return side;
//}

////per verificare se la codifica di una regione di un qt è precedente alla codifica dell'altra del secondo qt
//static bool isLess(std::string s1, std::string s2)
//{
//    int i = 0;
//    while (i < s1.size() && i < s2.size())
//    {
//        if (s1[i] < s2[i])
//            return true;
//        else if (s1[i] > s2[i])
//            return false;
//        ++i;
//    }
//    return s1.size() < s2.size();
//}

#if 1

//per confrontare le codifiche di due regioni di due qt
bool SquareIntersect ( unsigned int r1, unsigned int c1, unsigned int l1, unsigned int r2, unsigned int c2, unsigned int l2 )
{
     //si intersecano se hanno qualcosa in comune oppure se non sono disguinti
     //quindi se r1 >= r2+l2 allora r1 viene dopo di r2
     return ! ( r1 >= ( r2+l2 ) || ( r1+l1 ) <= r2 || c1 >= ( c2+l2 ) || ( c1+l1 ) <= c2 );
}

//r1,c1 con lato l1 include propriamente r2,c2 con lato l2?
bool SquareInclude ( unsigned int r1, unsigned int c1, unsigned int l1, unsigned int r2, unsigned int c2, unsigned int l2 )
{
     return
//            (r2 <= r1 && (r2 + l2) >= (r1 + l1) && c2 <= c1 && (c2 + l2) >= (c1 + l1)) ||   //il 2 include il 1
          ( r1 <= r2 && ( r1 + l1 ) >= ( r2 + l2 ) && c1 <= c2 && ( c1 + l1 ) >= ( c2 + l2 ) ); //il 1 include il 2
}
/*
 Valori di result:
 -2: s1 viene prima di s2 e non lo interseca
 -1: s1 viene prima di s2 e lo include
 0: s1 interseca s2
 1: s2 viene prima di s1 e lo include
 2: s2 viene prima di s1 e non lo interseca
 */
int SquareComparison ( unsigned int mor1, unsigned int l1, unsigned int mor2, unsigned int l2 )
{
	unsigned short int r1, c1, r2, c2;
     MortonDecode ( mor1, r1, c1 );
     MortonDecode ( mor2, r2, c2 );

     /*
      * i casi sono
      * q1 prima di q2, nessuna intersezione
      * q1 prima di q2 ma lo interseca o lo contiene
      * q1 insieme a q2 ma lo contiene
      * q1 insieme a q2 ma è contenuto
      * q2 prima di q1 ma lo interseca o lo contiene
      * q2 prima di q1, nessuna intersezione
      * */
     return 0;
}

void OverlayLqt ( T_Lqt lqt0, T_Lqt lqt1, T_Lqt_Overlay & result )
{
     //stringa MD, MD, id area
     auto it0 = lqt0.begin();
     auto it1 = lqt1.begin();

     while ( it0 != lqt0.end() && it1 != lqt1.end() ) {
          unsigned int md0 = std::get<0> ( *it0 );
          unsigned int l0 = std::get<1> ( *it0 );
          unsigned int id0 = std::get<2> ( *it0 );

          unsigned int md1 = std::get<0> ( *it1 );
          unsigned int l1 = std::get<1> ( *it1 );
          unsigned int id1 = std::get<2> ( *it1 );

		  unsigned short int r0, c0, r1, c1;
          MortonDecode ( md0, r0, c0 );
          MortonDecode ( md1, r1, c1 );

          if ( md0 < md1 ) {
               if ( SquareIntersect ( r0, c0, l0, r1, c1, l1 ) ) {
                    //si intersecano, quindi devo splittare. In che modo?
                    //Don't worry, splitto in quattro it0 e ricomincio
                    //nota che splitto it0 poiché viene prima e interseca, quindi il suo lato è più grande!
                    auto old0 = ( *it0 );
                    lqt0.erase ( it0 );
                    unsigned int newl0 = l0>>1;
                    auto where_00 = lqt0.insert ( make_tuple ( md0, newl0, id0 ) );
                    auto where_01 = lqt0.insert ( make_tuple ( MortonAdd ( md0, 0, newl0 ), newl0, id0 ) );
                    auto where_10 = lqt0.insert ( make_tuple ( MortonAdd ( md0, newl0, 0 ), newl0, id0 ) );
                    auto where_11 = lqt0.insert ( make_tuple ( MortonAdd ( md0, newl0, newl0 ), newl0, id0 ) );
                    it0 = where_00.first;
                    continue;
               } else {
                    //nessuna intersezione, il primo è fuori
                    result.insert ( make_tuple ( md0, l0, id0, EMPTY_REGION ) );
                    ++it0;
                    continue;
               }
          }

          //Else inutile poiché l'if interno fa continue
          if ( md1 < md0 ) {
               if ( SquareIntersect ( r1, c1, l1, r0, c0, l0 ) ) {
                    //si intersecano, quindi devo splittare. In che modo?
                    //Don't worry, splitto in quattro it1 e ricomincio
                    //nota che splitto it1 poiché viene prima e interseca, quindi il suo lato è più grande!
                    auto old1 = ( *it1 );
                    lqt1.erase ( it1 );
                    unsigned int newl1 = l1>>1;
                    auto where_00 = lqt1.insert ( make_tuple ( md1, newl1, id1 ) );
                    auto where_01 = lqt1.insert ( make_tuple ( MortonAdd ( md1, 0, newl1 ), newl1, id1 ) );
                    auto where_10 = lqt1.insert ( make_tuple ( MortonAdd ( md1, newl1, 0 ), newl1, id1 ) );
                    auto where_11 = lqt1.insert ( make_tuple ( MortonAdd ( md1, newl1, newl1 ), newl1, id1 ) );
                    it1 = where_00.first;
                    continue;
               } else {
                    //nessuna intersezione, il primo è fuori
                    result.insert ( make_tuple ( md1, l1, EMPTY_REGION, id1 ) );
                    ++it1;
                    continue;
               }
          }

          //se sono qui, allora sono uguali. Quindi, se sono uguali verifico se hanno lo stesso lato
          if ( l0 == l1 ) {
               //Sono proprio uguali, costruisco uscita
               result.insert ( make_tuple ( md0, l0, id0, id1 ) );
               ++it0;
               ++it1;
          } else {
               //non sono uguali, splitto il più grande!
               if ( l0 > l1 ) {
                    auto old0 = ( *it0 );
                    lqt0.erase ( it0 );
                    unsigned int newl0 = l0>>1;
                    auto where_00 = lqt0.insert ( make_tuple ( md0, newl0, id0 ) );
                    auto where_01 = lqt0.insert ( make_tuple ( MortonAdd ( md0, 0, newl0 ), newl0, id0 ) );
                    auto where_10 = lqt0.insert ( make_tuple ( MortonAdd ( md0, newl0, 0 ), newl0, id0 ) );
                    auto where_11 = lqt0.insert ( make_tuple ( MortonAdd ( md0, newl0, newl0 ), newl0, id0 ) );
                    it0 = where_00.first;
                    continue;
               } else {
                    auto old1 = ( *it1 );
                    lqt1.erase ( it1 );
                    unsigned int newl1 = l1>>1;
                    auto where_00 = lqt1.insert ( make_tuple ( md1, newl1, id1 ) );
                    auto where_01 = lqt1.insert ( make_tuple ( MortonAdd ( md1, 0, newl1 ), newl1, id1 ) );
                    auto where_10 = lqt1.insert ( make_tuple ( MortonAdd ( md1, newl1, 0 ), newl1, id1 ) );
                    auto where_11 = lqt1.insert ( make_tuple ( MortonAdd ( md1, newl1, newl1 ), newl1, id1 ) );
                    it1 = where_00.first;
                    continue;
               }
          }
     }

     //Se sono qui, allora ho terminato uno dei due lqt oppure tutti e due
     while ( it0 != lqt0.end() ) {
          unsigned int md0 = std::get<0> ( *it0 );
          unsigned int l0 = std::get<1> ( *it0 );
          unsigned int id0 = std::get<2> ( *it0 );
          result.insert ( make_tuple ( md0, l0, id0, EMPTY_REGION ) );
          ++it0;
     }
     while ( it1 != lqt1.end() ) {

          unsigned int md1 = std::get<0> ( *it1 );
          unsigned int l1 = std::get<1> ( *it1 );
          unsigned int id1 = std::get<2> ( *it1 );
          result.insert ( make_tuple ( md1, l1, EMPTY_REGION, id1 ) );
          ++it1;
     }
}
//
//void MergeLqt(T_Lqt & lqt0, T_Lqt & lqt1, std::set<int> inOldAndNotInNew, T_Lqt & result)
//{
//	//stringa MD, MD, id area
//	std::list<std::tuple<std::string, unsigned int, unsigned int>> regionsinold;
//
//	for (auto it: lqtold)
//	{
//		if (inOldAndNotInNew.find(std::get<2>(it)) != inOldAndNotInNew.end())
//		{
//			regionsinold.push_back(it);
//		}
//	}
//
//	std::cout << __func__   <<inOldAndNotInNew.size() << "\n";
//	std::cout << __func__   <<regionsinold.size() << "\n";
//	std::cout << __func__   <<lqtold.size() << "\n";
//
//	auto it2 = lqtnew.begin();
//	auto it1 = regionsinold.begin();
//
//	/*
//	  for each region of the qt regions in old
//		insert the region in the correct place of the qtnew
//	 */
//
//	while (it2 != lqtnew.end() && it1 != regionsinold.end())
//	{
//		std::string & s1 = std::get<0>(*it1);
//		std::string & s2 = std::get<0>(*it2);
//		unsigned int md1 = std::get<1>(*it1);
//		unsigned int md2 = std::get<1>(*it2);
//		unsigned int id1 = std::get<2>(*it1);
//		unsigned int id2 = std::get<2>(*it2);
//
//		if (isLess (s2, s1))
//		{
//			result.push_back(*it1);
//			++it1;
//		}
//		else
//		{
//			result.push_back(*it2);
//			++it2;
//		}
//	}
//	while (it2 != lqtnew.end())
//	{
//		result.push_back(*it2);
//		++it2;
//	}
//	while (it1 != regionsinold.end())
//	{
//		result.push_back(*it1);
//		++it1;
//	}
//}
//

//Esamina un QT e se ci sono elementi raggruppabili, li raggruppa!
bool RebuildLqt ( T_Lqt & lqt )
{
     std::function<bool ( T_Lqt & lqt ) > comprimi = [] ( T_Lqt & lqt ) -> bool {
          bool modified = false;
          while ( modified )
          {
               auto it = lqt.begin();
               while ( true ) {
                    //prendo i prossimi 4 elementi
                    unsigned int md0 = std::get<0> ( *it );
                    unsigned int l0 = std::get<1> ( *it );
                    unsigned int id0 = std::get<2> ( *it );
                    ++it;
                    if ( it == lqt.end() ) {
                         break;
                    }

                    unsigned int md1 = std::get<0> ( *it );
                    unsigned int l1 = std::get<1> ( *it );
                    unsigned int id1 = std::get<2> ( *it );
                    ++it;
                    if ( it == lqt.end() ) {
                         break;
                    }

                    unsigned int md2 = std::get<0> ( *it );
                    unsigned int l2 = std::get<1> ( *it );
                    unsigned int id2 = std::get<2> ( *it );
                    ++it;
                    if ( it == lqt.end() ) {
                         break;
                    }

                    unsigned int md3 = std::get<0> ( *it );
                    unsigned int l3 = std::get<1> ( *it );
                    unsigned int id3 = std::get<2> ( *it );

                    //se sono di stesse dimensioni, valore e se sono quadranti, li shrinko
                    if ( l0==l1 && l0==l2 && l0==l3 && MortonAdd ( md0, l0, 0 ) == md1 && MortonAdd ( md0, 0, l0 ) == md2 && MortonAdd ( md0, l0, l0 ) == md3 && id0==id1 && id1==id2 && id2==id3 ) {
                         //sono strettamente in sequenza, li faccio diventare uno solo
                         it--;
                         it--;
                         it--;
                         auto old = ( *it );
                         it = lqt.erase ( it );
                         it = lqt.erase ( it );
                         it = lqt.erase ( it );
                         it = lqt.erase ( it );
                         get<1> ( old ) = l0<<1;
                         lqt.insert ( old );
                         modified = true;
                    } else {
                         it--;
                         it--;
                    }
               }
          }
          return modified;
     };

     comprimi ( lqt );
     return false;
}

char ** MakeMatrix ( T_Lqt_Overlay qt, unsigned int &lato )
{
     //per prima cosa calcolo il lato della matrice
     lato=0;
     for ( auto it:qt ) {
		 unsigned int m,l,id;
		 unsigned short int r,c;
		 m = get<0> ( it );
          l = get<1> ( it );
          MortonDecode ( m, r, c );
          if ( ( r+l ) > lato ) {
               lato = r+l;
          }
          if ( ( c+l ) > lato ) {
               lato = c+l;
          }
     }
     //ok, ora facciamo la matrice
     char ** mat = new char*[lato]();
     for ( int i=0; i<lato; ++i ) {
          mat[i] = new char [lato]();
          memset ( mat[i], ( char ) 0, lato );
     }

     //ora faccio la matrice
     for ( auto it:qt ) {
		 unsigned int m,l,id0,id1;
		 unsigned short r,c;
		 m = get<0> ( it );
          l = get<1> ( it );
          id0 = get<2> ( it );
          id1 = get<3> ( it );
          MortonDecode ( m, r, c );
          for ( int r0=0; r0<l; ++r0 ) {
               for ( int c0=0; c0<l; ++c0 ) {
                    if ( id0!=EMPTY_REGION && id1 != EMPTY_REGION ) {
                         mat[r0+r][c0+c] = 'W';
                    } else {
                         if ( id0!=EMPTY_REGION ) {
                              mat [r0+r][c0+c] = '1';
                         } else {
                              mat [r0+r][c0+c] = '2';
                         }
                    }
               }
          }
     }
     return mat;
}

char ** MakeMatrix ( T_Lqt qt, unsigned int &lato )
{
     //per prima cosa calcolo il lato della matrice
     lato=0;
     for ( auto it:qt ) {
		 unsigned int m,l,id;
		 unsigned short int r,c;
		 m = get<0> ( it );
          l = get<1> ( it );
          MortonDecode ( m, r, c );
          if ( ( r+l ) > lato ) {
               lato = r+l;
          }
          if ( ( c+l ) > lato ) {
               lato = c+l;
          }
     }
     //ok, ora facciamo la matrice
     char ** mat = new char*[lato]();
     for ( int i=0; i<lato; ++i ) {
          mat[i] = new char [lato]();
          memset ( mat[i], ( char ) 0, lato );
     }

     //ora faccio la matrice
     for ( auto it:qt ) {
		 unsigned int m,l,id;
		 unsigned short int r,c;
		 m = get<0> ( it );
          l = get<1> ( it );
          id = get<2> ( it );
          MortonDecode ( m, r, c );
          for ( int r0=0; r0<l; ++r0 ) {
               for ( int c0=0; c0<l; ++c0 ) {
                    mat [r0+r][c0+c] = 'X';
               }
          }
     }
     return mat;
}


//Linear range
T_LinearRange BuildLinearRange ( unsigned long long inizio, unsigned long long fine, unsigned long long id )
{
     T_LinearRange lr;
     lr.push_back ( make_tuple ( inizio, fine, id ) );
     return lr;
}

T_LinearRange JoinLinearRange ( T_LinearRange lr1, T_LinearRange lr2, unsigned long long &new_id, map<unsigned long long int, mtfa_bitset*> & mapping )
{
     T_LinearRange lr_out;

     if ( lr1.size() ==0 ) {
          return lr2;
     }
     if ( lr2.size() == 0 ) {
          return lr1;
     }

     //potremmo usare i lower bound per rendere più efficiente la ricerca ma tanto poi devo sempre inserire in out
     auto it1 = lr1.begin();
     auto it2 = lr2.begin();

     while ( it1 != lr1.end() && it2 != lr2.end() ) {
          unsigned long long i1  = get<0> ( *it1 );
          unsigned long long f1  = get<1> ( *it1 );
          unsigned long long id1 = get<2> ( *it1 );

          unsigned long long i2  = get<0> ( *it2 );
          unsigned long long f2  = get<1> ( *it2 );
          unsigned long long id2 = get<2> ( *it2 );

          if ( f1 < i2 ) {
               //sono due range consecutivi, senza intersezioni. metto it1 e proseguo
               lr_out.push_back ( *it1 );
               ++it1;
               continue;
          }
          if ( f2 < i1 ) {
               //sono due range consecutivi, senza intersezioni. metto it2 e proseguo
               lr_out.push_back ( *it2 );
               ++it2;
               continue;
          }
          //a questo punto o sono uno nell'altro oppure si intersecano
          if ( i2 >= i1 ) {
               if ( f2 <= f1 ) {
                    //il secondo coincide oppure sta tutto nel primo
                    if ( i1 < i2 ) {
                         lr_out.push_back ( make_tuple ( i1, i2-1, id1 ) );
                    }
                    //ora la parte condivisa

                    //se id1 o id2 sono già in mapping, uso il loro mapping
                    auto found_id1 = mapping.find ( id1 );
                    auto found_id2 = mapping.find ( id2 );

                    mapping[new_id] = new mtfa_bitset();
                    if ( found_id1 == mapping.end() ) {
                         mapping[new_id]->set ( id1, true );
                    } else {
                         * ( mapping[new_id] ) |= * ( found_id1->second );
                    }
                    if ( found_id2 == mapping.end() ) {
                         mapping[new_id]->set ( id2, true );
                    } else {
                         * ( mapping[new_id] ) = * ( found_id2->second );
                    }
                    lr_out.push_back ( make_tuple ( i2, f2, new_id++ ) );

                    //ora la parte finale, se presente
                    if ( f2 < f1 ) {
                         get<0> ( *it1 ) = f2+1;
                         ++it2;
                         continue;
                    } else {
                         ++it1;
                         ++it2;
                         continue;
                    }
               } else {
                    //il primo inizia prima del secondo ma termina anche prima (f2 > f1))

                    //tratto, se presente dall'inizio del primo all'inizio del secondo
                    if ( i1 < i2 ) {
                         lr_out.push_back ( make_tuple ( i1, i2-1, id1 ) );
                    }

                    //ora la parte condivisa, ma stavolta fino alla fine del primo
                    //se id1 o id2 sono già in mapping, uso il loro mapping
                    auto found_id1 = mapping.find ( id1 );
                    auto found_id2 = mapping.find ( id2 );

                    mapping[new_id] = new mtfa_bitset();
                    if ( found_id1 == mapping.end() ) {
                         mapping[new_id]->set ( id1, true );
                    } else {
                         * ( mapping[new_id] ) |= * ( found_id1->second );
                    }
                    if ( found_id2 == mapping.end() ) {
                         mapping[new_id]->set ( id2, true );
                    } else {
                         * ( mapping[new_id] ) = * ( found_id2->second );
                    }
                    lr_out.push_back ( make_tuple ( i2, f1, new_id++ ) );

                    // f1 è terminato. f2 prosegue, quindi devo riconsiderarlo
                    get<0> ( *it2 ) = f1+1;
                    ++it1;
                    continue;
               }
          }
          if ( i1 >= i2 ) {
               if ( f1 <= f2 ) {
                    //il primo nel secondo
                    if ( i2 < i1 ) {
                         lr_out.push_back ( make_tuple ( i2, i1-1, id2 ) );
                    }
                    //ora la parte condivisa
                    //se id1 o id2 sono già in mapping, uso il loro mapping
                    auto found_id1 = mapping.find ( id1 );
                    auto found_id2 = mapping.find ( id2 );

                    mapping[new_id] = new mtfa_bitset();
                    if ( found_id1 == mapping.end() ) {
                         mapping[new_id]->set ( id1, true );
                    } else {
                         * ( mapping[new_id] ) |= * ( found_id1->second );
                    }
                    if ( found_id2 == mapping.end() ) {
                         mapping[new_id]->set ( id2, true );
                    } else {
                         * ( mapping[new_id] ) = * ( found_id2->second );
                    }
                    lr_out.push_back ( make_tuple ( i1, f1, new_id++ ) );

                    //ora la parte finale, se presente
                    if ( f1 < f2 ) {
                         get<0> ( *it2 ) = f1+1;
                         ++it1;
                         continue;
                    } else {
                         ++it1;
                         ++it2;
                         continue;
                    }
               } else {
                    //il secondo inizia prima del primo ma termina anche prima (f1 > f2))

                    //tratto, se presente dall'inizio del secondo all'inizio del primo
                    if ( i2 < i1 ) {
                         lr_out.push_back ( make_tuple ( i2, i1-1, id2 ) );
                    }

                    //ora la parte condivisa, ma stavolta fino alla fine del primo
                    //se id1 o id2 sono già in mapping, uso il loro mapping
                    auto found_id1 = mapping.find ( id1 );
                    auto found_id2 = mapping.find ( id2 );

                    mapping[new_id] = new mtfa_bitset();
                    if ( found_id1 == mapping.end() ) {
                         mapping[new_id]->set ( id1, true );
                    } else {
                         * ( mapping[new_id] ) |= * ( found_id1->second );
                    }
                    if ( found_id2 == mapping.end() ) {
                         mapping[new_id]->set ( id2, true );
                    } else {
                         * ( mapping[new_id] ) = * ( found_id2->second );
                    }
                    lr_out.push_back ( make_tuple ( i1, f2, new_id++ ) );

                    // f2 è terminato. f1 prosegue, quindi devo riconsiderarlo
                    get<0> ( *it1 ) = f2+1;
                    ++it2;
                    continue;
               }
          }
     }
     if ( it1 != lr1.end() ) {
          lr_out.insert ( lr_out.end(), it1, lr1.end() );
     } else {
          lr_out.insert ( lr_out.end(), it2, lr2.end() );
     }
     return lr_out;
}


#endif


#if 0
//
//	common_type<<#class _Tp#>>
//
//setq RebuildQt 'lambda(lqt)
//let (RB lqt)
//[let d pp lqt 1]
//if nilp lqt
//[Then]
//cons false cons nil nil
//[Else]
//if < length lqt 4
//[Then]
//cons false lqt
//[Else]
//let s0 car nth-item lqt 0
//let s1 car nth-item lqt 1
//let s2 car nth-item lqt 2
//let s3 car nth-item lqt 3
//let m0 cadr nth-item lqt 0
//let m1 cadr nth-item lqt 1
//let m2 cadr nth-item lqt 2
//let m3 cadr nth-item lqt 3
//[
// Se la distanza tra gli m_i è una potenza di 4
// 4^0, 4^1, ..., allora li posso unire insieme
// ]
//if  lor
//lor
//lor <> - m1 m0 - m2 m1
//<> - m1 m0 - m3 m2
//lor <> string-length s0 string-length s1
//<> string-length s2 string-length s3
//<> string-length s0 string-length s3
//[Then]
//let res (RB cdr lqt)
//[let d pp list (&Res0: & res) 1]
//if car res [modificato]
//cons true cdr (RB cons car lqt cdr res)
//cons false cons car lqt cdr res
//[Else]
//[Stanno alla stessa distanza, vediamo se potenza di 4]
//let dist - m1 m0
//let it car lqt
//if = dist 1
//[Then: Posso fare la merge]
//let res0 list (string-sub car it 0 * 2 - QT::slen dist cadr it caddr it)
//cons true cdr (RB cons res0 nth-cdr lqt 4)
//[Else: verifico se potenze di 4]
//let potenza nt-remove-factor dist 4
//if = cadr potenza dist
//[Then]
//let res (RB cdr lqt)
//[let d pp list (&Res1: & res) 1]
//if car res
//cons true cdr (RB cons car lqt cdr res)
//cons false cons car lqt cdr res
//[Else]
//let res0 list (string-sub car it 0 * 2 - QT::slen car potenza cadr it caddr it)
//cons true cdr (RB cons res0 nth-cdr lqt 4)
//let res (RB lqt)
//[if car res
// let d pp &ECCOLO!\n& 1
// (RB cdr res)
// ]
//cdr res
//
//setq MergeQt 'lambda(qt0 qt1)
//let (f it)
//let v0 nth-item it 2
//let v1 nth-item it 3
//if = v0 v1
//list (car it cadr it v0)
//if = v0 QTS::EMPTY-REGION
//list (car it cadr it v1)
//if = v1 QTS::EMPTY-REGION
//list (car it cadr it v0)
//[Se sono diversi e non vuoti, c'è un errore]
// let d pp &Error in Merge& 1
// nil
// qt-overlay QT::side qt0 qt1 'f



//void RemoveRegions(T_Lqt & lqt, std::set<unsigned long long> & regions_to_be_removed, unsigned long long idbase)
//{
//	auto it = lqt.begin();
//	while (it != lqt.end())
//	{
//		unsigned long long id = idbase + std::get<2>(*it);
//		if (regions_to_be_removed.find(id) != regions_to_be_removed.end())
//		{
//			auto rem = it;
//			++it;
//			lqt.erase(rem);
//		}
//		else
//		{
//			++it;
//		}
//	}
//}

//lo stesso per operazioni di intersect e di union



//int GetMaxId(std::list<std::tuple<std::string, unsigned int, unsigned int>>  & mnodi)
//{
//	int maxval = -1;
//	for (auto it: mnodi)
//	{
//		int val = std::get<2>(it);
//		if (maxval < val)
//			maxval = val;
//	}
//	return maxval;
//}
//
//void ListAreas(T_Lqt & mnodi, std::set<int> & areas)
//{
//	for (auto it: mnodi)
//		areas.insert(std::get<2>(it));
//}
//
//void RegionAreas(T_Lqt lqt, T_Regione  & area)
//{
//	for (auto & it: lqt)
//	{
//		std::get<0>(area[std::get<2>(it)]) += negative_power_of_2[std::get<0>(it).size()];
//	}
//}
//
//void ChangeIds(T_Lqt & mnodi, std::unordered_map<int, int> &mapper)
//{
//	for (auto & it: mnodi)
//	{
//		auto it1 = mapper.find(std::get<2>(it));
//		if (it1 != mapper.end())
//			std::get<2>(it) = (*it1).second; //mapper[std::get<2>(it)];
//		else{
//			std::cout << __func__   << "la regione " << std::get<2>(it) << " non è mappata\n";
//			//exit(0);
//		}
//	}
//}
//

#endif
