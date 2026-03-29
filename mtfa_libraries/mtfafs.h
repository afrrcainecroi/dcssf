#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#endif

#include <map>

#include <unordered_map>
#include <unordered_set>

#include <boost/multiprecision/cpp_int.hpp>
using namespace boost::multiprecision;


//#include "mtfa_Utilities.h"
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <cstdint>
#include <math.h>
#include "mtfafs_base.h"
#include "mtfa_ph.h"
#include "mtfa_fs3.h"


using namespace std;
// Questa è la classe che posso istanziare sulle stringhe (binarie) più lunghe di due.
// Ma ora ho la classe base per le stringhe lunghe anche 1
// mtfafs_s2_base!!!

class mtfafs_s2
{
public:

    int lp_num;
    LogPartition_ex_2 ** lp;
    void AddSpace();
    void Compress();
    void Clean();
    mtfafs_s2();
    mtfafs_s2 ( int num );
    ~mtfafs_s2();

    bool Exists ( std::string & data,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    bool Insert ( std::string & data,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

/*
 * Mtfafs 2 extended. Usefull as classifier.
 * Minumum length: 2
 * Maximum length: any.
 * Really fast. Does not store all the std::string but only the first, identifiyng, part.
 * You can add a final_value associated to the std::string. it is a pointer to unsigned int
 * but you can cast it to anything!
 * \author      FA
 * \date        02/09/2012
 */
class mtfafs_s2_extended
{
private:

    /*
     * Se lunga 1, uso un array di 256 chars.
     * Se lunga 2, uso un array di 65536 chars
     * Se lunga >=3, uso struttura dati.
     * Se lunghezza è dispari, l'ultimo valore vale 2^16+valore, altrimenti prendo i valori a coppie.
     */

    vector<unsigned int>pos_1;
    vector<unsigned int>pos_2;
    int lp_num;
    LogPartition_ex_2 ** lp;

    void AddSpace ( int howmany = 1 );

    bool InsertFromHere ( unsigned int val,
                          int pos,
                          unsigned char * data,
                          int datalen,
                          unsigned int * final_value );

public:

    CVettorePString * v_dati;

    void Clean();

    mtfafs_s2_extended ( int num );

    mtfafs_s2_extended();

    ~mtfafs_s2_extended();

    unsigned long long Size();

    void Compress();

    bool Exists ( const unsigned char * data,
                  int datalen,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );
    
    bool ExistsFast ( const unsigned char * data,
                  int datalen,
                  unsigned int * final_value);
    
    bool Insert ( unsigned char * data_p,
                  int datalen,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfafs_s2_extended_v1
{
private:

    int lp_num;
    LogPartition_ex_2 ** lp;

    void AddSpace ( int howmany = 1 );

    bool InsertFromHere ( unsigned int val,
                          int pos,
                          unsigned char * data,
                          int datalen,
                          unsigned int * final_value );

public:

    CVettorePString * v_dati;

    void Clean();

    mtfafs_s2_extended_v1 ( int num );

    mtfafs_s2_extended_v1();

    ~mtfafs_s2_extended_v1();

    unsigned long long Size();

    void Compress();

    bool Exists ( const unsigned char * data,
                  int datalen,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    bool Insert ( unsigned char * data_p,
                  int datalen,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfafs_s2_extended_vint
{
private:

    int lp_num;
    LogPartition_ex_2 ** lp;

    void AddSpace ( int howmany = 1 );

    bool InsertFromHere ( unsigned int val,
                          int pos,
                          unsigned int * data,
                          int datalen,
                          unsigned int * final_value );

public:

    CVettorePVoid * v_dati;

    void Clean();

    mtfafs_s2_extended_vint ( int num );

    mtfafs_s2_extended_vint();

    ~mtfafs_s2_extended_vint();

    unsigned long long Size();

    void Compress();

    bool Exists ( const unsigned int * data,
                  int datalen,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    bool Insert ( unsigned int * data_p,
                  int datalen,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

/*
 * Mtfafs 2 base.
 * Useful for inserting a complete std::string (buffer) into the Data structure.
 * The data stored is a binary buffer of any length.
 * \author      FA
 * \date        02/09/2012
 */
class mtfafs_s2_base
{
private:
    int lp_num;
    LogPartition_ex_2 ** lp;

    void AddSpace();

public:
    void Clean();

    mtfafs_s2_base ( bool icase,
                     int num );

    mtfafs_s2_base ( bool icase );

    mtfafs_s2_base ( int num );

    mtfafs_s2_base();

    ~mtfafs_s2_base();

    void Size();

    bool InsertFromHere ( unsigned int val,
                          int pos,
                          const unsigned char * data,
                          const int len_data,
                          unsigned int * final_value );

    bool Exists ( const unsigned char * data,
                  const int len_data,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    // Esiste una stringa di quelle archiviate nei dati di ingresso?
    // Funziona al contrario della esist. La Exists torna se tutto data  tra le archiviate mentre
    // Exists_substring torna se la prima parte di data contiene una di quelle archiviate.
    bool Exists_substring ( const unsigned char * data,
                            const int len_data,
                            std::vector<std::pair<std::string, std::pair<int, int> > > & v_dati,
                            int pos_attuale,
                            unsigned int * final_value,
                            unsigned int ** index_pointer,
                            unsigned int ** data_pointer );

    bool Insert ( const unsigned char * data,
                  const int len_data,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};


// Questa  per coppie di int

class mtfafs_ii
{
private:

public:

    // Qui ci metto la classe generica (basata su stringhe di lunghezza variabile da 1 a ...)
    LogPartition_ex_2 * lp; // solo un grafo da 2 interi! Poi me la vedo io !!!

public:
    void Clean();

    mtfafs_ii();

    ~mtfafs_ii();

    bool Exists ( unsigned int v1,
                  unsigned int v2,
                  unsigned int & val,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    bool Insert ( unsigned int v1,
                  unsigned int v2,
                  unsigned int & val,
                  unsigned int * final_value );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );

    void Size();
};


// Questa coppie di stringhe binarie
// Associo a ogni stringa un numero e poi faccio l'associazione tra i due numeri

class mtfafs_ss
{
private:
    mtfafs_s2_extended * a_s1;
    unsigned int p_s1;
    mtfafs_s2_extended * a_s2;
    unsigned int p_s2;
    mtfafs_ii * a_s1s2;

public:
    void Clean();

    mtfafs_ss();

    ~mtfafs_ss();

    bool Insert ( std::string v1,
                  std::string v2,
                  unsigned int * final_value );

    bool Exists ( std::string v1,
                  std::string v2,
                  unsigned int * final_value,
                  unsigned int ** index_pointer,
                  unsigned int ** data_pointer );

    void Serialize ( mtfa_binary_stream_save * bs );

    void Deserialize ( mtfa_binary_stream_load * bs );
};

/*
 * Mtfafs std::string set.
 * Questa class  importante poich gestisce la ricerca di un insieme di stringhe su un buffer di ingresso di lunghezza data
 * Ogni stringa da cercare ha associato un limite di ricerca
 * Per ogni stringa trovata nel buffer di input, viene costruita
 * una lista di coppie formate da stringa, posizione iniziale nel buffer di ingresso.
 * \author      FA
 * \date        03/09/2012
 */
class mtfafs_string_set
{
private:
    mtfafs_s2_base * xii;
    std::vector<std::pair<std::string, std::pair<int, int> > > v_dati;
    int data_len_max; // per ricordare la massima lunghezza delle sue stringhe
    bool ignore_case;
    int max_data_len; // per ricordare la massima lugnhezza attesa per il buffer su cui cercare

public:
    bool AddToSet ( std::string val,
                    int from,
                    int howmany,
                    unsigned int & position );

    mtfafs_string_set ( bool ignore_case = false );

    ~mtfafs_string_set();

    // ogni stringa che ho trovato, la stringa, la sua chiave (posizione unica nel vettore) e dove inizia in data
    bool Find ( const unsigned char * data,
                int data_len,
                std::list<std::pair<std::string, std::pair<int, int> > > & result );
};

class mtfa_base_mat
{
private:
public:

    typedef struct __TheMs__ {
        unsigned int val;
        unsigned int val1;
    } TheMs;

    //std::map<unsigned int, unsigned int> freeidx;
    mtfa_fastrng1024 simplerng;

public:
    const int base_allocazione = BASE_ALLOCAZIONE_mtfa_base_mat;
    unsigned int righe_allocate;
    unsigned int colonne_allocate;
    unsigned int righe;
    unsigned int colonne;
    TheMs ** ms;
    TheMs initializer;
    CVettoreInt vb;
    CVettoreInt vdegb; // in questo modo non devo calcolare ogni volta il grado di b ma lo conosco a priori
    typedef unordered_map<unsigned int, set<unsigned int>> T_vb_adj; //un modo rapido per calcolare gli adiacenti di b
    typedef unordered_map<unsigned int, set<unsigned int>>::iterator T_it_vb_adj;
    T_vb_adj vb_adj;


    // private:
    unsigned long long Size();

    void ReallocateRows ( unsigned int r );

    void ReallocateCols ( unsigned int c );

    void Reallocate ( unsigned int r,
                      unsigned int c );

    // public:
    ~mtfa_base_mat();

    void Init();

    mtfa_base_mat();

    void Clear();

    // void Conta();

    unsigned int Exists ( unsigned int a, unsigned int b );

    unsigned int FindFirstFreeOrReallocate ( unsigned int a );

    unsigned int * EvalAdjacentB ( unsigned int b ); // lungo logn+1

    unsigned int FindFreeSpace ( unsigned int a,
                                 unsigned int b,
                                 T_it_vb_adj adiacenti );

    // public:
    unsigned int Insert ( unsigned int a, unsigned int b, unsigned int idx );

    bool ExistsB ( unsigned int b );

    int GetDegreeB ( unsigned int b );

    //int GetDegreeA(unsigned int a);

    // unsigned int GetNumA();
    std::list<unsigned int> * GetAllBConnectedToThisA ( unsigned int a );

    void DeleteEdge ( unsigned int a,
                      unsigned int b );

    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfa_mat
{
private:
    int a_size;
public:
    mtfa_fastrng1024 simplerng;
    vector<mtfa_bitset*> occupati;

public:
    unsigned int righe_allocate;
    unsigned int colonne_allocate;
    unsigned int righe;
    unsigned int colonne;

    vector<unsigned int> vb;
    vector<unsigned int> vdegb; // in questo modo non devo calcolare ogni volta il grado di b ma lo conosco a priori

    unsigned int ** ms_b; //questa contiene bit alto per indicare vuota e 31 bit per indicare valore di b
    unsigned long long ** ms_v; //Questa contiene un valore che associo alla coppia (es: indice della coppia)
    //L'ho fatta come long long poiché può contenere sia l'indice dell'arco nella matrice, sia un valore utente

    unsigned long long Size();

    void ReallocateCols ( unsigned int c );

    ~mtfa_mat();

    void Init();

    mtfa_mat ( int a_size );

    void Clear();

    //unsigned int Exists(unsigned int a, unsigned int b);
    bool Exists ( unsigned int a, unsigned int b, unsigned long long** pindex );

    unsigned int FindFirstFreeOrReallocate ( unsigned int a );

    unsigned int FindFreeSpace ( unsigned int a,
                                 unsigned int b,
                                 int len_adiacenti, unsigned int * adiacenti );

    // public:
    unsigned long long * Insert ( unsigned int a, unsigned int b, unsigned int idx );
    //unsigned int Insert(T_range range, unsigned int idx);

    bool ExistsB ( unsigned int b );

    int GetDegreeB ( unsigned int b );
    unsigned int * GetAllBConnectedToThisA ( unsigned int a, int & len );
    set<int> GetAllBConnectedToThisA ( unsigned int a );

    void DeleteEdge ( unsigned int a,
                      unsigned int b );

    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class log_mat
{
private:
    int a_size;
public:
    void Init();

    log_mat ( int a_size = 256 );

    ~log_mat();

    unsigned long long int Size();

    // L'insieme delle matrici compresse
    mtfa_mat ** vm;
    int num_m;

    // Il vettore puntatore a ogni matrice
    //CVettoreInt vna;
    vector<unsigned int> vna;

    //unsigned int Exists(unsigned int a, unsigned int b);    // Torna il contatore globale associato a questo elemento, se presente. zerox8000 se non c'è
    bool Exists ( unsigned int a, unsigned int b, unsigned long long ** pindex ); // Torna il contatore globale associato a questo elemento, se presente. zerox8000 se non c'è

    //    bool ExistsA(unsigned char a);

    //    std::list<unsigned char> * GetAllAConnectedToThisB(unsigned int b);

    //    void DeleteEdge(unsigned char a, unsigned int b);

    unsigned long long * Insert ( unsigned int a, unsigned int b, unsigned int idx );

    void Clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfads_pair
{
private:
    int a_size;
public:
    log_mat * lmat;

public:

    unsigned long long Size()
    {
        return lmat->Size();
    };

    void Clean();

    mtfads_pair ( int a_size = 256 );

    ~mtfads_pair();

    bool Exists ( const unsigned int v1, const unsigned int v2, unsigned long long** pindex );

    unsigned long long * Insert ( const unsigned int v1, const unsigned int v2, unsigned int index );
    list<tuple<unsigned int, unsigned int, unsigned int>> GetAll();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};


class mtfa_mat_star
{
private:
    int a_size;
public:
    mtfa_fastrng1024 simplerng;
    vector<mtfa_bitset*> occupati;  //Occupati è un array di 256 elementi di cui ognuno rappresenta un set di bit (che sono i valori di b)

public:
    unsigned int righe_allocate;
    unsigned int colonne_allocate;
    unsigned int righe;
    unsigned int colonne;

    vector<unsigned int> vb;
    vector<unsigned int> vdegb; // in questo modo non devo calcolare ogni volta il grado di b ma lo conosco a priori

    unsigned int ** ms_b; //questa contiene bit alto per indicare vuota e 31 bit per indicare valore di b
    unsigned long long ** ms_v; //Questa contiene un valore che associo alla coppia (es: indice della coppia)
    //L'ho fatta come long long poiché può contenere sia l'indice dell'arco nella matrice, sia un valore utente

    unsigned long long Size();

    void ReallocateCols ( unsigned int c );

    ~mtfa_mat_star();

    void Init();

    mtfa_mat_star ( int a_size );

    void Clear();

    //unsigned int Exists(unsigned int a, unsigned int b);
    bool Exists ( unsigned int a, unsigned int b, unsigned long long** pindex );

    unsigned int FindFirstFreeOrReallocate ( unsigned int a );

    unsigned int FindFreeSpace ( unsigned int a,
                                 unsigned int b,
                                 int len_adiacenti, unsigned int * adiacenti );

    // public:
    unsigned long long * Insert ( unsigned int a, unsigned int b, unsigned int idx );
    //unsigned int Insert(T_range range, unsigned int idx);

    bool ExistsB ( unsigned int b );

    int GetDegreeB ( unsigned int b );
    unsigned int * GetAllBConnectedToThisA ( unsigned int a, int & len );

    void DeleteEdge ( unsigned int a,
                      unsigned int b );

    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class log_mat_star
{
private:
    int a_size;
public:
    void Init();

    log_mat_star ( int a_size = 65536 );

    ~log_mat_star();

    unsigned long long int Size();

    // L'insieme delle matrici compresse
    mtfa_mat_star ** vm;
    int num_m;

    // Il vettore puntatore a ogni matrice
    //CVettoreInt vna;
    vector<unsigned int> vna;

    //unsigned int Exists(unsigned int a, unsigned int b);    // Torna il contatore globale associato a questo elemento, se presente. zerox8000 se non c'è
    bool Exists ( unsigned int a, unsigned int b, unsigned long long ** pindex ); // Torna il contatore globale associato a questo elemento, se presente. zerox8000 se non c'è

    //    bool ExistsA(unsigned char a);

    //    std::list<unsigned char> * GetAllAConnectedToThisB(unsigned int b);

    //    void DeleteEdge(unsigned char a, unsigned int b);

    unsigned long long * Insert ( unsigned int a, unsigned int b, unsigned int idx );

    void Clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfads_pair_star
{
private:
    int a_size;
public:
    log_mat_star * lmat;
    //    unsigned int indice_archi_inseriti;

public:

    unsigned long long Size()
    {
        return lmat->Size();
    };

    void Clean();

    mtfads_pair_star ( int a_size = 65536 ); //jolly e null value

    ~mtfads_pair_star();

    //int Exists(const unsigned int v1, const unsigned int v2);
    bool Exists ( const unsigned int v1, const unsigned int v2, unsigned long long** pindex );

    //    int Insert(const unsigned int v1, const unsigned int v2);
    unsigned long long * Insert ( const unsigned int v1, const unsigned int v2, unsigned int index );
    list<tuple<unsigned int, unsigned int, unsigned int>> GetAll();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

class mtfa_sequence_star
{
private:
    unsigned int InsertFromHere ( const unsigned short *s, int slen, unsigned int _prec_idx, unsigned long long ** last_cell );
    void InternalInsert();
    int internal_memcmp ( const unsigned short * s1, const unsigned char * s2, int len );
    int internal_memcmp ( const unsigned short * s1, const unsigned short * s2, int len );
    int internal_memcmp1 ( const unsigned short * s1, const unsigned short * s2, int len );
    int internal_memcmp_noj ( const unsigned short * s1, const unsigned short * s2, int len );
    unsigned long long _size; //quanti elementi ho inserito

public:
    static const unsigned short jollyvalue;
    static const unsigned short emptyvalue;
    map<vector<unsigned short>, set<int>> db;
    vector<pair<unsigned int, mtfads_pair_star*>> vpairs; //idx partono da 1
    vector<set<int>> rule_set;  //gli insiemi delle regole di ingresso.
    ~mtfa_sequence_star();
    mtfa_sequence_star();
    void Insert ( string, unsigned int, int );
    void Insert ( vector<unsigned short>, unsigned int );
    unsigned int Build();
    bool Search ( const unsigned char * s, int slen, unsigned int & ident);
    bool Search ( const vector<unsigned short> s, unsigned int & ident);
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
    set<int> GetRules ( int );
};

/********************************************************************************************************************/
/********************************************************************************************************************/
/*********************************************** mtfa_sequences ******************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
inline vector<uint8_t> mtfa_StringToVui8(const string & s);
inline string mtfa_Vui8ToString(const vector<uint8_t> &v);

class mtfa_umph {
private:
    unordered_map<string, int> umsi;
    unordered_map<int, unordered_map<string, int>::iterator> umis;
    int uidx;  //da 1 in poi
public:
    mtfa_umph();
    ~mtfa_umph();
    int Insert(const string & s);
    int Search(const string & s);
    int Insert(const vector<uint8_t> & v);
    int Search(const vector<uint8_t> & v);
    string ReverseSearch(const int & i);
    void clear();
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

//Assegna "sempre" 3 (da 1 a 4) byte per stringa
vector<uint8_t> mtfa_LsToVui8(mtfa_umph &ph, const list<string> & ls, const int numOfBytesPerString=3);

//la sequence base non archivia ma costruisce esclusivamente struttura a partire dall'input. Consente la ricerca, lavora su vettori di unsigned int8
class mtfa_sequence_base
{
private:
    long uidx;
    vector<pair<mtfads_pair*, int>> vpairs;
public:
    ~mtfa_sequence_base();
    mtfa_sequence_base();
    long Insert ( const vector<uint8_t> & vui8);  //-1 error
    long Search ( const vector<uint8_t> & vui8);  //-1 not found
    long Search1 ( const vector<uint8_t> & vui8);  //-1 not found
    //bool GetFastPath ( const vector<uint8_t> & vui8, vector<uint8_t> & path);
    unordered_set<long> GetAll ( const vector<uint8_t> & vui8 );  //-1 not found
    bool Unlink ( const vector<uint8_t> & vui8 ); //elimina solo se punta a una foglia

    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};

//La gestione di sequenze di stringhe!
class mtfa_string_sequences {
public:
    mtfa_umph ph;
    mtfa_sequence_base *seq;
    unordered_map<int, vector<uint8_t>> itovui8;

public:
    mtfa_string_sequences();
    ~mtfa_string_sequences();

    int insert(const list<string> & ls, const int len=3);  //len va da 1 a 4 i base alla numerosità delle stringhe di input differenti
    int insert(const vector <uint8_t> vui8);
    int search(const list<string> & ls, const int len=3);
    int search(const vector <uint8_t> vui8);
    bool unlink(const list<string> & ls, const int len=3);
    bool unlink(const vector <uint8_t> vui8);
    list<string> getval(int idx, const int len=3);
    vector<uint8_t> getvalvui8(int idx, const int len=3);
    void Serialize ( mtfa_binary_stream_save * bs );
    void Deserialize ( mtfa_binary_stream_load * bs );
};



