#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#endif

#include <stdio.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <math.h>

#include "mtfa_utils.h"

int WriteAll ( int nfile, unsigned char * buf, int buflen );
int ReadAll ( int nfile, unsigned char * buf, int buflen );


/*
 * 00xx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
 * 10xx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
 * 01xx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
 * 11xx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
 */

// #define MTFAFS_EMPTY                                  0x03FFFFFF
// #define MTFAFS_CONTINUE                                       zerox8000
// #define MTFAFS_FINISHED                                       zerox4000
// #define MTFAFS_CONTINUE_AND_FINISHED  0xC0000000
// SE 0x3FFFFFFF, allora il posto \u00e8 vuoto!
// SE zerox8000, allora la stringa prosegue, ce ne sono altre parti!
// SE zerox4000, allora la stringa si ferma, non c'\u00e8 altro!
// SE 0xC0000000, allora una si ferma e l'altra prosegue!!!
static const unsigned int zerox8000 = 0x80000000;
static const unsigned int zerox4000 = 0x40000000;
static const unsigned int zerox7FFF = 0x7FFFFFFF;
static const unsigned int zerox3FFF = 0x3FFFFFFF;
static const unsigned int zeroxC000 = 0xC0000000;

static const unsigned long long zerox8000ULL = 0x0000000080000000;
static const unsigned long long zerox4000ULL = 0x0000000040000000;
static const unsigned long long zerox7FFFULL = 0x000000007FFFFFFF;
static const unsigned long long zerox3FFFULL = 0x000000003FFFFFFF;
static const unsigned long long zeroxC000ULL = 0x00000000C0000000;
static const unsigned long long zerox0000ULL = 0x0000000000000000;



#define BASE_ALLOCAZIONE_mtfa_base_mat 32
#define BASE_ALLOCAZIONE_CMatriceCompressa_ex_2 32

#define BASE_ALLOCAZIONE_COL    32
#define BASE_ALLOCAZIONE_mtfa_mat 32

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

class CVettorePVoid
{
private:
    vector<void *> v;
public:
//     const int base_allocazione = 8192;
//     const int base_allocazione_bits = 13; //indica quanti shift per fare 8192
//     const unsigned int base_allocazione_mask = 0x00001FFF;
//     unsigned int   righe_attuali;
//     unsigned int   righe;
    void * initializer;
    unsigned long long Size();
//     void Reallocate ( unsigned int r );
    void Init ( void * in_initializer );
    CVettorePVoid ( void * in_initializer );
    CVettorePVoid();
    void Init();
    ~CVettorePVoid();
    void Clear();
    void*       & operator[] ( unsigned int idx );
};

class CVettoreInt
{
private:
    vector<unsigned int> v;
public:
    unsigned int initializer;
    unsigned long long Size();
    void Init ( unsigned int in_initializer );
    CVettoreInt ( unsigned int in_initializer );
    CVettoreInt();
    void Init();
    void Reallocate ( unsigned int r );
    ~CVettoreInt();
    void Clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
    unsigned int       & operator[] ( unsigned int idx );
    unsigned int Get ( unsigned int idx );
};

class CVettorePString
{
private:
    unsigned char *** vettore;
    int rows;
public:
    CVettoreInt v_len;
    const int base_allocazione = 8192;
    const int base_allocazione_bits = 13; //indica quanti shift per fare 8192
    const unsigned int base_allocazione_mask = 0x00001FFF;
    unsigned int   righe_attuali;
    unsigned int   righe;
    unsigned long long Size();
    void Reallocate ( unsigned int r );
    CVettorePString();
    void Init();
    ~CVettorePString();
    void Clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
    unsigned char * operator[] ( unsigned int idx );
    unsigned char * Get ( const int & pos, int *plen );
    int push_back ( std::string s );
    int push_back ( unsigned char * p, int len );
    int put ( unsigned int where, char * what );
    int put ( unsigned int where, unsigned char * what, int what_len );

};

class CVettoreString
{
private:
    std::string ** vettore;
    int rows;
public:
    const int base_allocazione = 8192;
    const int base_allocazione_bits = 13; //indica quanti shift per fare 8192
    const unsigned int base_allocazione_mask = 0x00001FFF;
    unsigned int   righe_attuali;
    unsigned int   righe;
    unsigned long long Size();
    void Reallocate ( unsigned int r );
    CVettoreString();
    void Init();
    ~CVettoreString();
    void Clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
    std::string & operator[] ( unsigned int idx );
};


class TheMs
{
public:
    unsigned int val;
    unsigned int val1;
    unsigned int val2;
};


class CMatriceCompressa_ex_2
{
private:
    std::map<unsigned int, unsigned int> freeidx;
    //SimpleRng simplerng;
public:
    int           base_allocazione;
    unsigned int  righe_allocate;
    unsigned int  colonne_allocate;
    unsigned int  righe;
    unsigned int  colonne;
    TheMs **      ms;
    TheMs         initializer;
    CVettoreInt   vb;
    CVettoreInt   vdegb;
    //typedef std::set<unsigned long long> T_vb_adj;
    typedef unordered_map<unsigned int, set<unsigned int>> T_vb_adj; //un modo rapido per calcolare gli adiacenti di b
    typedef unordered_map<unsigned int, set<unsigned int>>::iterator  T_it_vb_adj; //un modo rapido per calcolare gli adiacenti di b
    T_vb_adj vb_adj;

    unsigned long long Size();

    void ReallocateRows ( unsigned int r );

    void ReallocateCols ( unsigned int c );

    void Reallocate ( unsigned int r,
                      unsigned int c );

    ~CMatriceCompressa_ex_2();

    void Init();

    void Compress();

    CMatriceCompressa_ex_2();


    void Clear();

    unsigned int Exists ( unsigned int    a,
                          unsigned int    b,
                          unsigned int *  punt,
                          unsigned int ** index_pointer,
                          unsigned int ** data_pointer );

    unsigned int FindFirstFreeOrReallocate ( unsigned int a );

    unsigned int * EvalAdjacentB ( unsigned int b ); // lungo logn+1

    unsigned int FindFreeSpace ( unsigned int           a,
                                 unsigned int           b,
                                 T_it_vb_adj adiacenti );

    unsigned int Insert ( unsigned int a,
                          unsigned int b,
                          unsigned int data,
                          unsigned int punt );

    bool ExistsB ( unsigned int b );

    int GetDegreeB ( unsigned int b );

    std::set<unsigned int>
    GetAllBConnectedToThisA ( unsigned int a );

    void DeleteEdge ( unsigned int a, unsigned int b );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};


class LogPartition_ex_2
{
public:
    void Init();

    LogPartition_ex_2();

    ~LogPartition_ex_2();


    unsigned long long int Size();

    void Compress();

    CMatriceCompressa_ex_2 ** vm;
    int                       num_m;

    // Per ogni insert il contatore viene incrementato.
    // In tal modo la nuova coppia che si viene a creare, se non c'era prima, assume tale valore.
    // per evitare incoerenze, il valore iniziale del counter \u00e8 messo a 1,
    // in tal modo un valore di ritornodi zerox8000 indica non trovato.
    // invece un valore di 0x80000351 indica trovato 351 ma non \u00e8 terminale e un valore
    // 0x40000465 indica trovato 465 e termina la sua stringa. In questo modo si
    // potrebbero aggiungere anche situazioni in cui
    // potrei trovare un valore di 0xC000091A che potrebbe indicare: 91A pu\u00f2
    // essere un terminatore oppure no. Dipende da cosa stavi cercando.
    unsigned int counter;

    // Il vettore puntatore a ogni matrice
    CVettoreInt va;

    // Torna il contatore globale associato a questo elemento,
    // se presente. zerox8000 se non c'\u00e8
    unsigned int Exists ( unsigned int    a,
                          unsigned int    b,
                          unsigned int *  punt,
                          unsigned int ** index_pointer,
                          unsigned int ** data_pointer );

    bool ExistsA ( unsigned int a );

    std::list<unsigned int> * GetAllAConnectedToThisB ( unsigned int b );

    void DeleteEdge ( unsigned int a,
                      unsigned int b );

    unsigned int Insert ( unsigned int a,
                          unsigned int b,
                          bool         termina,
                          unsigned int punt );

    void Clear();

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

