#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <sys/time.h>


#include <math.h>
#include <unistd.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <cstdlib>
#include <arpa/inet.h>
//#include <omp.h>

#include "mtfafs.h"
#include "mtfa_ph.h"

#include "mtfa_utils.h"
#include "http_parser.h"
#include "NumberTheory.h"
#include <stdlib.h>

int logn = 28; //Inutile dichiararla fuori!

using namespace std;

#ifdef USE_BOOST_MLP
unsigned int ffs256(const boost::multiprecision::uint256_t &n) {
  if (n.is_zero()) {
    return -1;
  } else {
    return boost::multiprecision::lsb(n);
  }
}
#endif

void*& CVettorePVoid::operator[] ( unsigned int idx )
{
    if (v.size() <= idx)
        v.resize(idx+1, initializer);
    return v[idx];    // return v[0][idx];
}


void CVettorePVoid::Init ( void* in_initializer )
{
    initializer = in_initializer;
    v.clear();
}
void CVettorePVoid::Init()
{
    initializer = nullptr; //zerox8000;
    v.clear();
}

CVettorePVoid::CVettorePVoid ( void* in_initializer )
{
    Init ( in_initializer );
}

CVettorePVoid::CVettorePVoid()
{
    Init();
}


CVettorePVoid::~CVettorePVoid()
{
    v.clear();
}

void CVettorePVoid::Clear()
{
    Init();
}

unsigned long long CVettorePVoid::Size()
{
    return v.size();
}

unsigned int& CVettoreInt::operator[] ( unsigned int idx )
{
    if (v.size() <= idx)
        v.resize(idx+1, initializer);
    return v[idx];    // return v[0][idx];
}

void CVettoreInt::Reallocate( unsigned int idx )
{
    if (v.size() <= idx)
        v.resize(idx+1, initializer);
}

unsigned int CVettoreInt::Get ( unsigned int idx )
{
    if (v.size() <= idx)
        return zerox8000;
    else
        return v[idx];    // return v[0][idx];
}

void CVettoreInt::Init ( unsigned int in_initializer )
{
    v.clear();
    initializer = in_initializer;
}
void CVettoreInt::Init()
{
    v.clear();
    initializer = zerox8000;
}

CVettoreInt::CVettoreInt ( unsigned int in_initializer )
{
    Init ( in_initializer );
}

CVettoreInt::CVettoreInt()
{
    initializer = zerox8000;
    Init();
}


CVettoreInt::~CVettoreInt()
{
    v.clear();
}

void CVettoreInt::Clear()
{
    Init();
}

void CVettoreInt::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) initializer );
    bs->Append ( ( unsigned int ) v.size());

    for ( int i = 0; i < v.size(); ++i )
        bs->Append<unsigned int> ( v[i] );
}

void CVettoreInt::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();
    initializer = bs->Read<unsigned int>();
    unsigned int righe = bs->Read<unsigned int>();
    v.resize(righe, initializer);

    for ( int i = 0; i < righe; ++i )
    {
        unsigned int len = 0;
        v[i] = bs->Read<unsigned int> ( );
    }
}

unsigned long long CVettoreInt::Size()
{
    return v.size();
}

unsigned long long CVettorePString::Size()
{
    unsigned long long tot = 0;
    return tot;
}

unsigned char* CVettorePString::operator[] ( unsigned int idx )
{
    Reallocate ( idx );
    int vrow = idx >> base_allocazione_bits;
    int vcol = idx & base_allocazione_mask;
    //return std::string(vettore[vrow][vcol], vettore[vrow][vcol] + v_len[idx]);
    return vettore[vrow][vcol];
}

int CVettorePString::put ( unsigned int where, unsigned char* what,
                           int what_len )
{
    Reallocate ( where );
    int vrow = where >> base_allocazione_bits;
    int vcol = where & base_allocazione_mask;
    //return std::string(vettore[vrow][vcol], vettore[vrow][vcol] + v_len[idx]);
    vettore[vrow][vcol] = what;
    v_len[where] = what_len;

    return 0;
}

int CVettorePString::put ( unsigned int where, char* what )
{
    Reallocate ( where );
    int vrow = where >> base_allocazione_bits;
    int vcol = where & base_allocazione_mask;
    //return std::string(vettore[vrow][vcol], vettore[vrow][vcol] + v_len[idx]);
    vettore[vrow][vcol] = ( unsigned char* ) what;
    v_len[where] = strlen ( what ) + 1;

    return 0;
}

void CVettorePString::Reallocate ( unsigned int r )
{
    if ( r >= righe )
    {
        if ( r >= righe_attuali )
        {
            unsigned int newrighe = r + 1; // + base_allocazione;

            if ( vettore != nullptr )
            {
                int new_vrow = 1 + ( newrighe >> base_allocazione_bits );

                if ( new_vrow > rows )
                {
                    vettore = ( unsigned char*** ) realloc ( vettore,
                              ( new_vrow ) * sizeof ( unsigned char** ) );

                    for ( int i = rows; i < new_vrow; ++i )
                    {
                        vettore[i] = ( unsigned char** ) malloc (
                                         base_allocazione * sizeof ( unsigned char* ) );
                        std::fill ( & ( vettore[i][0] ),
                                    & ( vettore[i][base_allocazione] ), nullptr );
                    }

                    righe_attuali = new_vrow * base_allocazione;
                    rows = new_vrow;
                }
            }
            else
            {
                rows = 1 + ( newrighe >> base_allocazione_bits );
                vettore = ( unsigned char*** ) malloc (
                              rows * sizeof ( unsigned char** ) );

                for ( int i = 0; i < rows; ++i )
                {
                    vettore[i] = ( unsigned char** ) malloc (
                                     base_allocazione * sizeof ( unsigned char* ) );
                    std::fill ( & ( vettore[i][0] ), & ( vettore[i][base_allocazione] ),
                                nullptr );
                }

                righe_attuali = rows * base_allocazione;
            }
        }

        righe = r + 1;
    }

    return;

}

CVettorePString::CVettorePString()
{
    Init();
    v_len.Init ( 0 );
}

void CVettorePString::Init()
{
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    vettore = nullptr;
    v_len.Clear();
}

CVettorePString::~CVettorePString()
{
    if ( vettore != NULL )
    {
        for ( int i = 0; i < righe; ++i )
        {
            int vrow = i >> base_allocazione_bits;
            int vcol = i & base_allocazione_mask;

            if ( vettore[vrow][vcol] )
            {
                free ( vettore[vrow][vcol] );
            }
        }

        for ( int i = 0; i < rows; ++i )
        {
            if ( vettore[i] )
            {
                free ( vettore[i] );
                vettore[i] = nullptr;
            }
        }

        free ( vettore );
    }

    vettore = nullptr;
    v_len.Clear();
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    rows = 0;
}

void CVettorePString::Clear()
{
    if ( vettore )
    {
        for ( int i = 0; i < righe; ++i )
        {
            int vrow = i >> base_allocazione_bits;
            int vcol = i & base_allocazione_mask;

            if ( vettore[vrow][vcol] )
            {
                free ( vettore[vrow][vcol] );
            }
        }

        for ( int i = 0; i < rows; ++i )
        {
            if ( vettore[i] )
            {
                free ( vettore[i] );
                vettore[i] = nullptr;
            }
        }

        free ( vettore );
    }

    vettore = nullptr;
    v_len.Clear();
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    rows = 0;
    //Init();
}

void CVettorePString::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) righe );

    for ( int i = 0; i < righe; ++i )
    {
        bs->Append ( operator[] ( i ), v_len[i] );
    }

    v_len.Serialize ( bs );
}

void CVettorePString::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();
    unsigned int newrighe = bs->Read<unsigned int>();

    rows = 1 + ( newrighe >> base_allocazione_bits );
    vettore = ( unsigned char*** ) malloc ( rows * sizeof ( unsigned char** ) );

    for ( int i = 0; i < rows; ++i )
    {
        vettore[i] = ( unsigned char** ) malloc (
                         base_allocazione * sizeof ( unsigned char* ) );
        std::fill ( & ( vettore[i][0] ), & ( vettore[i][base_allocazione] ), nullptr );
    }

    righe_attuali = rows * base_allocazione;

    for ( int i = 0; i < newrighe; ++i )
    {
        unsigned int len = 0;
        unsigned char* p = bs->Read<unsigned char> ( len );
        put ( i, p, len );
        //operator[](i) = p;
    }

    v_len.Deserialize ( bs );
}

unsigned char* CVettorePString::Get ( const int& idx, int* plen )
{
    int vrow = idx >> base_allocazione_bits;
    int vcol = idx & base_allocazione_mask;
    *plen = v_len[idx];
    return vettore[vrow][vcol];
}

int CVettorePString::push_back ( std::string s )
{
    int old_righe = righe;
    Reallocate ( old_righe );
    int vrow = old_righe >> base_allocazione_bits;
    int vcol = old_righe & base_allocazione_mask;
    auto dummy = v_len[old_righe];
    v_len[old_righe] = s.size();
    vettore[vrow][vcol] = ( unsigned char* ) malloc (
                              s.size() * sizeof ( unsigned char ) );
    memcpy ( vettore[vrow][vcol], s.data(), sizeof ( char ) * s.size() );
    return old_righe;
}

int CVettorePString::push_back ( unsigned char* p, int len )
{
    int old_righe = righe;
    Reallocate ( old_righe );
    int vrow = old_righe >> base_allocazione_bits;
    int vcol = old_righe & base_allocazione_mask;
    auto dummy=v_len[old_righe];
    v_len[old_righe] = len;
    vettore[vrow][vcol] = ( unsigned char* ) malloc ( len * sizeof ( unsigned char ) );
    memcpy ( vettore[vrow][vcol], p, sizeof ( char ) * len );
    return old_righe;
}

/**********************************Vettore String */
unsigned long long CVettoreString::Size()
{
    unsigned long long tot = 0;
    return tot;
}

std::string& CVettoreString::operator[] ( unsigned int idx )
{
    Reallocate ( idx );
    int vrow = idx >> base_allocazione_bits;
    int vcol = idx & base_allocazione_mask;
    return vettore[vrow][vcol];
}

void CVettoreString::Reallocate ( unsigned int r )
{
    if ( r >= righe )
    {
        if ( r >= righe_attuali )
        {
            unsigned int newrighe = r + 1; // + base_allocazione;

            if ( vettore != nullptr )
            {
                int new_vrow = 1 + ( newrighe >> base_allocazione_bits );

                if ( new_vrow > rows )
                {
                    vettore = ( std::string** ) realloc ( vettore,
                                                          ( new_vrow ) * sizeof ( std::string* ) );

                    for ( int i = rows; i < new_vrow; ++i )
                    {
                        vettore[i] = new std::string[base_allocazione];
                        //vettore[i] = (std::string*)malloc(base_allocazione*sizeof(std::string));
                        //std::fill(&(vettore[i][0]), &(vettore[i][base_allocazione]), std::string());
                    }

                    righe_attuali = new_vrow * base_allocazione;
                    rows = new_vrow;
                }
            }
            else
            {
                rows = 1 + ( newrighe >> base_allocazione_bits );
                vettore = ( std::string** ) malloc ( rows * sizeof ( std::string* ) );

                for ( int i = 0; i < rows; ++i )
                {
                    //std::string empty;
                    vettore[i] = new std::string[base_allocazione];
                    //vettore[i] = (std::string*)malloc(base_allocazione*sizeof(std::string));
                    //std::fill(&(vettore[i][0]), &(vettore[i][base_allocazione]), empty);
                }

                righe_attuali = rows * base_allocazione;
            }
        }

        righe = r + 1;
    }

    return;

}

CVettoreString::CVettoreString()
{
    Init();
}

void CVettoreString::Init()
{
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    vettore = nullptr;
}

CVettoreString::~CVettoreString()
{
    if ( vettore != NULL )
    {
        for ( int i = 0; i < rows; ++i )
        {
            if ( vettore[i] )
            {
                delete[] ( vettore[i] );
                vettore[i] = nullptr;
            }
        }

        free ( vettore );
    }

    vettore = nullptr;
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    rows = 0;
}

void CVettoreString::Clear()
{
    if ( vettore != NULL )
    {
        for ( int i = 0; i < rows; ++i )
        {
            if ( vettore[i] )
            {
                delete[] ( vettore[i] );
                vettore[i] = nullptr;
            }
        }

        free ( vettore );
    }

    vettore = nullptr;
    righe = 0;
    righe_attuali = 0; //base_allocazione;
    rows = 0;
}

void CVettoreString::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) righe );

    for ( int i = 0; i < righe; ++i )
    {
        bs->Append ( operator[] ( i ) );
    }
}

void CVettoreString::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();
    unsigned int new_righe = bs->Read<unsigned int>();
    Reallocate ( new_righe - 1 );

    for ( int i = 0; i < new_righe; ++i )
    {
        //        unsigned int len = 0;
        //        unsigned char * p = bs->ReadVUC(len);
        operator[] ( i ) = bs->Read();
    }
}
/****************************************/

/*
 Vediamo: ho bisogno di verificare se un particolare elemento è effettivamente lui (il confronto con b e il contenuto di m)
 - in m abbiamo sia i valori di b, sia l'indicazione se il campo è vuoto (zerox8000) sia l'indicazione che il campo è terminatore di sequenza (zerox4000)
 - in m1 abbiamo l'enumerazione delle coppie a,b che sono state inserite in questa struttura. è un valore sempre crescente che mi consente di associare un id unico a ogni arco che
 viene istanziato
 - in m2 abbiamo un valore passato come parametro dal programma chiamante. Viene restituito quando si trova il fine sequenza
 - per gestire l'archiviazione parziale delle sequenze, a fronte del fatto che sono uniche e che quindi non è necessario proseguire nella loro archiviazione, dovrei fare qualcos'altro
 che per ora non faccio.
 */

//La matrice compressa e il log grappooo
void CMatriceCompressa_ex_2::ReallocateRows ( unsigned int r )
{
    //Se non supera le righe allocate inutile estendere
    if ( r >= righe )
    {
        if ( r >= righe_allocate )
        {
            unsigned int newrighe = r + 1 + base_allocazione;
            TheMs** tmp_ms = ( TheMs** ) malloc ( sizeof ( TheMs* ) * newrighe );
            std::copy ( & ( ms[0] ), & ( ms[righe_allocate] ), tmp_ms );
            free ( ms );
            ms = tmp_ms;
            //Le nuove righe sono a zerox8000. Solo quando serviranno le utilizzeremo
            std::fill ( &ms[righe_allocate], &ms[newrighe], nullptr );
            //            for (unsigned int i = righe_allocate; i < newrighe; ++i) {
            //                ms[i] = NULL;
            //            }
            righe_allocate = newrighe;
            righe = r + 1;
        }

        righe = r + 1;
    }
}

void CMatriceCompressa_ex_2::ReallocateCols ( unsigned int c )
{
    if ( c >= colonne )
    {
        if ( c >= colonne_allocate )
        {
            //Per ogni riga aggiungo le colonne che servono!
            unsigned int newcol = c + BASE_ALLOCAZIONE_COL + 1; // + colonne_allocate;

            for ( unsigned int i = 0; i < righe_allocate; i++ )
            {
                //In questo caso se la riga puntava a NULL, non la rialloca poiché non ci interessa per ora
                if ( ms[i] != NULL )
                {
                    TheMs* m_i = ( TheMs* ) malloc ( sizeof ( TheMs ) * newcol );
                    std::copy ( & ( ms[i][0] ), & ( ms[i][colonne_allocate] ), m_i );
                    free ( ms[i] );
                    ms[i] = m_i;
                    std::fill ( &ms[i][colonne_allocate], &ms[i][newcol],
                                initializer ); //zerox8000);
                }
            }

            colonne_allocate = newcol;
        }

        colonne = c + 1;
    }
}

void CMatriceCompressa_ex_2::Reallocate ( unsigned int r, unsigned int c )
{
    ReallocateRows ( r );
    ReallocateCols ( c );
}

CMatriceCompressa_ex_2::~CMatriceCompressa_ex_2()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != NULL )
        {
            free ( ms[i] );
        }
    }

    free ( ms );
    vb.Clear();
    vdegb.Clear();
    vb_adj.clear();
    ms = nullptr;
}

CMatriceCompressa_ex_2::CMatriceCompressa_ex_2()
{
    Init();
}

void CMatriceCompressa_ex_2::Init()
{
    righe = 0;
    colonne = 0;
    ms = NULL;
    base_allocazione = BASE_ALLOCAZIONE_CMatriceCompressa_ex_2;
    righe_allocate = 0;
    colonne_allocate = 0;

    //vb = new CVettoreInt();
    //vdegb = new CVettoreInt();

    vb.Init ( zerox8000 );
    vdegb.Init ( 0 );
    vb_adj.clear();

    initializer.val = zerox8000;
    initializer.val1 = 0;
    initializer.val2 = 0;

    righe_allocate = 0; //base_allocazione;
    colonne_allocate = 0; //base_allocazione;
    righe = 0;
    colonne = 0;

    //simplerng.Next(Ticks::get_micro());
}

unsigned int CMatriceCompressa_ex_2::Exists ( unsigned int a, unsigned int b,
        unsigned int* punt, unsigned int** index_pointer,
        unsigned int** data_pointer )
{
    *index_pointer = nullptr;
    *data_pointer = nullptr;

    if ( a >= righe )
    {
        return zerox8000;
    }

    if ( b >= vb.Size())
    {
        return zerox8000;
    }

    //unsigned int vbvb = vb->v[b];
    unsigned int vbvb = vb[b];

    if ( vbvb == zerox8000 )
    {
        return zerox8000;
    }

    TheMs* ma = ms[a];

    if ( ma == NULL )
    {
        return zerox8000;
    }
    else
    {
        if ( ma[vbvb].val == b )
        {
            *index_pointer = & ( ms[a][vbvb].val1 );
            *data_pointer = & ( ms[a][vbvb].val2 );

            *punt = ms[a][vbvb].val2;
            return ms[a][vbvb].val1; //Torno quest'oggetto che deve essere un indice di cui utilizzo senz'altro i tre bytes più significativi.
            //Il bit più alto indica che prosegue, il bit 30 indica che termina qui.
            //non c'è ambiguità con il valore zerox8000 poiché il minimo valore messo il m1[][] vale 1 e non 0.
        }
        else
        {
            return zerox8000;
        }
    }
}

unsigned int CMatriceCompressa_ex_2::FindFirstFreeOrReallocate ( unsigned int a )
{
    if ( colonne > 0 )
    {
        TheMs* msa;

        if ( ! ( msa = ms[a] ) )
        {
            return 0;
        }

        for ( int c = 0; c < colonne; ++c )   //colonne - 20; c < colonne; ++c) {
        {
            if ( ( msa++ )->val == zerox8000 )
            {
                return c;
            }
        }
    }

    ReallocateCols ( colonne );
    return colonne - 1;
    //  if (colonne > 0)
    //  {
    //    long long llcolonne = colonne;
    //    for (long long i = (llcolonne - 1); i >= 0; --i)
    //    {
    //      //E' libero questo posto?
    //
    //      if ((ms[a] == NULL) || (ms[a][i].val & zerox8000))
    //      {
    //        return i;
    //      }
    //    }
    //  }
    //  ReallocateCols(colonne);
    //  return colonne - 1;
}

unsigned int*
CMatriceCompressa_ex_2::EvalAdjacentB ( unsigned int b )
{
    //Calcolo gli adiacenti di b sulla colonna
    if ( vb.Size() <= b )
    {
        return nullptr;
    }

    unsigned int idxb = vb[b];

    if ( idxb == zerox8000 )
    {
        return nullptr;
    }

    auto pstmp = vb_adj.find ( b );

    if ( pstmp == vb_adj.end() )
    {
        unsigned int* result = ( unsigned int* ) malloc (
                                   1 * sizeof ( unsigned int ) );
        result[0] = 0xFFFFFFFF;
        return result;
    }
    else
    {
        int quanti = ( *pstmp ).second.size();
        unsigned int* result = ( unsigned int* ) malloc (
                                   ( quanti + 1 ) * sizeof ( unsigned int ) );
        int i = 0;

        for ( auto it : ( *pstmp ).second )
        {
            result[i++] = it;
        }

        result[quanti] = 0xFFFFFFFF;
        return result;
    }

    //    //Calcolo gli adiacenti di b sulla colonna
    //    if (vb.righe <= b)
    //        return nullptr;
    //    unsigned int idxb = vb[b];
    //    if (idxb == zerox8000)
    //        return nullptr;
    //
    //    int quanti = 0;
    //    unsigned int * result = (unsigned int*) malloc((logn + 2) * sizeof (unsigned int));
    //
    //    auto it = vb_adj.lower_bound(((unsigned long long) b) << 32 | 0);
    //    while (it != vb_adj.end()) {
    //        unsigned long long v = (*it);
    //        ++it;
    //        if ((v >> 32) == b) {
    //            result[quanti++] = v & 0x00000000FFFFFFFF;
    //        } else
    //            break;
    //    }
    //    result[quanti] = 0xFFFFFFFF;
    //    return result;
}

unsigned int CMatriceCompressa_ex_2::FindFreeSpace ( unsigned int a,
        unsigned int b, T_it_vb_adj adiacenti )
{
    TheMs* msa = ms[a];

    if ( !msa )
    {
        return 0; //Va bene tutto!
    }

    //devo cercare la prima colonna che ha liberi gli spazi che in questa sono occupati da b.
    if ( adiacenti == vb_adj.end() )   //nullptr || adiacenti[0] == 0xFFFFFFFF) {
    {
        //        cout << __func__   << "1: " << colonne << endl;
        //        for (int c = (colonne >> 4); c >= 0; --c)
        for ( unsigned int c = 0; c < colonne; ++c )
        {
            int col = c; //simplerng.Next() % colonne;

            //Ma come stiamo messi per l'arco corrente?
            if ( msa[col].val != zerox8000 )
            {
                continue;
            }

            return c;
        }

        ReallocateCols ( colonne );
        return colonne - 1;
    }

    unsigned int vbvb = vb[b];

    for ( int c = ( colonne >> 4 ); c >= 0; --c )
        //for (unsigned int c = 0; c < colonne; ++c)
    {
        int col = rand() % colonne;

        if ( col != vbvb )
        {
            bool found = true;

            //for (auto i = 0; adj[i] != 0xFFFFFFFF; ++i)
            for ( auto adj : adiacenti->second )   // != vb_adj.end()) {
            {
                TheMs* pm;

                if ( ! ( pm = ms[adj] ) )
                {
                    continue;
                }

                if ( pm[col].val != zerox8000 )
                {
                    found = false;
                    break; //Finisce qui questa ricerca, almeno uno già occupato
                }
            }

            if ( !found )
            {
                continue;     //Non l'ho trovata una colonna con tutti gli spazi liberi
            }

            //Ma come stiamo messi per l'arco corrente?
            if ( msa[col].val != zerox8000 )
            {
                continue;
            }

            return col;
        }
    }

    //Se sono qui, allora, purtroppo, non ho trovato una colonna libera, devo aggiungerne una nuova
    //Aggiungo una nuova colonna;
    ReallocateCols ( colonne );
    return colonne - 1;
    //  unsigned int vbvb = vb[b];
    //
    //  for (unsigned int c = 0; c < colonne; ++c)
    //  {
    //    if (c != vbvb)
    //    {
    //      bool found = true;
    //      if (adiacenti != nullptr)
    //      {
    //        for (int i = 0; adiacenti[i] != 0xFFFFFFFF; ++i)
    //        {
    //          TheMs * pm = ms[adiacenti[i]];
    //          if (!pm)
    //            continue;
    //
    //          if (pm[c].val ^ zerox8000)
    //          {
    //            found = false;
    //            break; //Finisce qui questa ricerca, almeno uno già occupato
    //          }
    //        }
    //      }
    //      if (!found)
    //        continue; //Non l'ho trovata una colonna con tutti gli spazi liberi
    //
    //      //Ma come stiamo messi per l'arco corrente?
    //      if (ms[a] && ms[a][c].val != zerox8000)
    //        continue;
    //
    //      return c;
    //
    //    }
    //  }
    //
    //  //Se sono qui, allora, purtroppo, non ho trovato una colonna libera, devo aggiungerne una nuova
    //  //Aggiungo una nuova colonna;
    //  ReallocateCols(colonne);
    //  return colonne - 1;
}

//Nella matrice ci deve stare
// il bit 31 per indicare che è vuota : zerox8000
// il bit 30 per indicare che è un terminatore di stringa archiviata: zerox4000;
// Dato che stiamo utilizzando un long per archiviare un byte, in effetti, potremmo stare più larghi.

/**
 * Inserts.
 *
 * \author  FA
 * \date    05/12/2011
 *
 * \param   a       a.
 * \param   b       The.
 * \param   data    The data.
 *
 * \return  .
 */
unsigned int CMatriceCompressa_ex_2::Insert ( unsigned int a, unsigned int b,
        unsigned int data, unsigned int punt )
{
//     if (a==52582 && b==17355)
//     {
//         cout << "prossimo errore\n";
//         cout << "ADJs[" << b << "] = " << vb_adj[59613].size() << "(Degree: " << vdegb[59613] << ")\n";
//     }
    //In ogni caso, se necessario rialloco!
    auto dummy = vb[b];
    dummy = vdegb[b];
    ReallocateRows ( a );

    //Questo nodo b è stato già assegnato?
    if ( vb[b] & zerox8000 )
    {
        //No, ancora non assegnato!
        //Cerco il primo posto libero nella sequenza associata ad a
        //!!!QUEST'OPERAZIONE PUO' ESSERE VELOCIZZATA CON UN HEAP!!!
        unsigned int firstFree = FindFirstFreeOrReallocate ( a );
        vb[b] = firstFree;

        if ( ms[a] == nullptr )
        {
            ms[a] = ( TheMs* ) malloc ( sizeof ( TheMs ) * colonne_allocate );
            std::fill ( ms[a], &ms[a][colonne_allocate], initializer );
        }

        ms[a][vb[b]].val = b;
        ms[a][vb[b]].val1 = data;

        if ( data & zerox4000 )   //Termina
        {
            ms[a][vb[b]].val2 = punt;
        }

        vdegb[b]++;
        vb_adj[b].insert ( a );
        return data;
    }

    //OK. Il nodo b è stato precedentemente assegnato.
    //Ma non potrebbe essere che l'arco è stato già inserito?
    if ( ms[a] != nullptr )
    {
        if ( ms[a][vb[b]].val == b )
        {
            //avevo ragione.
            //in questo caso non aggiorno il puntatore??? ma si, aggiornalo!
            if ( data & zerox4000 )   //Termina
            {
                ms[a][vb[b]].val2 = punt;
            }

            return ms[a][vb[b]].val1;
        }

        //ma a è libero per questa forestina?
        if ( ms[a][vb[b]].val & zerox8000 )
        {
            //!Libero!, utilizzo questo.
            ms[a][vb[b]].val = b;
            ms[a][vb[b]].val1 = data;

            if ( data & zerox4000 )   //Termina
            {
                ms[a][vb[b]].val2 = punt;
            }

            vdegb[b]++;
            vb_adj[b].insert ( a );
            return data;
        }
    }
    else
    {
        ms[a] = ( TheMs* ) malloc ( sizeof ( TheMs ) * colonne_allocate );
        std::fill ( ms[a], &ms[a][colonne_allocate], initializer );

        if ( ms[a][vb[b]].val & zerox8000 )
        {
            //!Libero!, utilizzo questo.
            ms[a][vb[b]].val = b;
            ms[a][vb[b]].val1 = data;

            if ( data & zerox4000 )   //Termina
            {
                ms[a][vb[b]].val2 = punt;
            }

            vdegb[b]++;
            vb_adj[b].insert ( a );
            return data;
        }
    }

    //OK, ho trovato una foresta anomala, devo ristrutturare.
    /*
     -  Per tutti gli a adiacenti di b, ivi compreso questo che sto cercando di inserire,
     cerco il primo posto libero dove spostare b
     */

    //Calcolo gli adiacenti di b
    //unsigned int * adiacenti = EvalAdjacentB(b);
    /*
     Ricordati gli invarianti:
     1) Esiste sempre un solo nodo b in un insieme di foreste
     2) In una foresta un nodo a ha SEMPRE grado 1.
     Quindi, ripeti con me, cosa stai cercando?
     Sto cercando tra tutti gli a, adiacenti di b, il primo posto libero in comune
     */
    //ora cerco uno special set (una colonna) nella quale ci sia spazio per tutti gli archi uscenti da b, compreso l'arco corrente)
    auto adiacenti = vb_adj.find ( b );
    unsigned int frspace = FindFreeSpace ( a, b, adiacenti );

    //Colonna trovata, la inserisco e elimino la vecchia!
    if ( adiacenti != vb_adj.end() )
    {
        //for (auto i=adiacenti->begin(); i != adiacenti->end(); ++i)
        for ( auto pos : adiacenti->second )
        {
            unsigned int vbvb = vb[b];

            ms[pos][frspace] = ms[pos][vbvb];

            ms[pos][vbvb] = initializer;
        }
    }

    //Aggiungo arco a,b
    ms[a][frspace].val = b;
    ms[a][frspace].val1 = data;

    if ( data & zerox4000 )   //Termina
    {
        ms[a][frspace].val2 = punt;
    }

    vb[b] = frspace; //Aggiorna lo special set in cui si trova ora b
    vdegb[b]++;
    vb_adj[b].insert ( a );
    return data;
}

bool CMatriceCompressa_ex_2::ExistsB ( unsigned int b )
{
    if ( b >= vb.Size() )
    {
        return false;
    }

    return ( vb[b] != zerox8000 );
}

int CMatriceCompressa_ex_2::GetDegreeB ( unsigned int b )
{
    if ( vdegb.Size() > b )
    {
        return vdegb[b];
    }
    else
    {
        return 0;
    }
}

//int
//CMatriceCompressa_ex_2::GetDegreeA(unsigned int a)
//{
//  int deg = 0;
//  for (unsigned int i = 0; i < colonne; i++)
//    if ((ms[a] != NULL) && (ms[a][i].val != zerox8000))
//      deg++;
//
//  return deg;
//}

std::set<unsigned int> CMatriceCompressa_ex_2::GetAllBConnectedToThisA (
    unsigned int a )
{
    std::set<unsigned int> l;

    if ( righe > a )
    {
        for ( unsigned int i = 0; i < colonne; i++ )
        {
            if ( ( ms[a] != NULL ) && ( ms[a][i].val != zerox8000 ) )
            {
                //              if (this->vb[ms[a][i].val & zerox3FFF] == zerox8000)
                //                  cout << __func__   << "QUI";
                l.insert ( ms[a][i].val & zerox3FFF );
            }
        }
    }

    return l;
}

void CMatriceCompressa_ex_2::DeleteEdge ( unsigned int a, unsigned int b )
{
    //  vb->DelItem(a, b);
    unsigned int col = vb[b];
    ms[a][col] = initializer; //zerox8000;
    vdegb[b]--;
    vb_adj[b].erase ( a );
    //vb_adj.erase ( ( ( unsigned long long ) b ) << 32 | a );
}

void CMatriceCompressa_ex_2::Clear()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != NULL )
        {
            free ( ms[i] );
        }
    }

    free ( ms );
    vb.Clear();
    vdegb.Clear();
    vb_adj.clear();
    ms = nullptr;
    righe_allocate = 0;
    colonne_allocate = 0;

    righe = 0;
    colonne = 0;
    Init();

}

void CMatriceCompressa_ex_2::Compress()
{
    ///Rivede la matrice compressa
    ///1: elimina le colonne e le righe vuote
    ///2: comprime le colonne comprimibili.

    int righe_non_allocate = 0;
    int righe_vuote = 0;
    int colonne_vuote = 0;

    for ( int r = 0; r < righe; ++r )
    {
        if ( !ms[r] )
        {
            righe_non_allocate++;
        }
        else
        {
            bool vuota = true;

            for ( int c = 0; c < colonne; ++c )
            {
                if ( ms[r][c].val != zerox8000 )
                {
                    vuota = false;
                    break;
                }
            }

            if ( vuota )
            {
                free ( ms[r] );
                ms[r] = nullptr;
                righe_vuote++;
            }
        }
    }

    // cout << __func__   << "\n";
    // for (int c=0; c<colonne; ++c)
    // {
    //     bool vuota = true;
    //     for (int r=0; r<righe; ++r)
    //     {
    //         if (ms[r] && ms[r][c].val != zerox8000)
    //         {
    //             vuota = false;
    //             break;
    //         }
    //     }
    //     if (vuota)
    //     {
    //         //devi comprimere la colonna
    //         cout << __func__   << c << ", ";
    //         colonne_vuote++;
    //     }
    // }
    // cout << __func__   << "\n";
    // cout << __func__   << "Righe totali  : " << righe << endl;
    // cout << __func__   << "Righe non all.: " << righe_non_allocate << endl;
    // cout << __func__   << "Righe vuote   : " << righe_vuote << endl;
    // cout << __func__   << "Colonne totali: " << colonne << endl;
    // cout << __func__   << "Colonne vuote: " << colonne_vuote << endl;
    // cout << __func__   << endl;

    if ( colonne > 0 )
    {
        bool modified = true;

        //        bool first_time = true;
        while ( modified )
        {
            //           if (first_time)
            //               first_time = false;
            //           else
            //               cout << __func__   << "Ricomincia\n";
            modified = false;

            for ( int i = 0; i < ( colonne - 1 ); ++i )
            {
                int j = i + 1;
                //for (int j=i+1; j<colonne; ++j)
                {
                    //queste due colonne sono differenti?
                    bool ok = true;
                    bool vuota = true;

                    for ( int r = 0; r < righe; ++r )
                    {
                        if ( ms[r] )
                        {
                            if ( ms[r][j].val != zerox8000 )
                            {
                                vuota = false;
                            }

                            if ( ms[r][i].val != zerox8000
                                    && ms[r][j].val != zerox8000 )
                            {
                                ok = false;
                                break;
                            }
                        }
                    }

                    if ( !vuota && ok )
                    {
                        //                        cout << __func__   << "Merge: " << i << ", " << j << endl;
                        for ( int r = 0; r < righe; ++r )
                        {
                            if ( ms[r] && ms[r][j].val != zerox8000 )
                            {
                                modified = true;
                                ms[r][i] = ms[r][j];
                                vb[ms[r][i].val & zerox3FFF] = i;
                                ms[r][j] = initializer;
                            }
                        }
                    }
                }
            }
        }
    }

}

unsigned long long CMatriceCompressa_ex_2::Size()
{
    //    unsigned long long tot = 0;
    //
    //    tot += sizeof (base_allocazione);
    //    tot += sizeof (righe_allocate);
    //    tot += sizeof (colonne_allocate);
    //    tot += sizeof (righe);
    //    tot += sizeof (colonne);
    //    tot += sizeof (ms);
    //    tot += sizeof (initializer);
    //    tot += sizeof (vb);
    //
    // tot += sizeof (unsigned int*)*righe_allocate;
    //    unsigned long long tot_cols = 0;
    //    for (int i = 0; i < righe_allocate; ++i)
    //    {
    //        if (ms[i] != nullptr)
    //        {
    //            tot_cols += sizeof (TheMs) * colonne_allocate;
    //        }
    //    }
    //    tot += tot_cols;
    //    tot += vb.Size();
    //
    //    printf("CMatriceCompressa_ex_2. Needed: %llu, Total: %llu, FullMatrix: %llu\n", tot - (sizeof (unsigned int*)*righe_allocate - sizeof (unsigned int*)*righe) - (tot_cols - tot_cols / ((colonne_allocate * colonne) == 0 ? 1 : (colonne_allocate * colonne))), tot, (unsigned long long) (righe * colonne * sizeof (TheMs)));
    //
    //    return tot;
    //
    unsigned long long tot = 0;

    tot += sizeof ( base_allocazione );
    tot += sizeof ( righe_allocate );
    tot += sizeof ( colonne_allocate );
    tot += sizeof ( righe );
    tot += sizeof ( colonne );
    tot += sizeof ( ms );
    tot += sizeof ( initializer );
    tot += sizeof ( vb );
    //  tot += sizeof (vdegb);
    tot += sizeof ( unsigned int* ) * righe_allocate;
    unsigned long long tot_cols = 0;
    int righe_reali = 0;
    int celle_piene = 0;

    for ( int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != nullptr )
        {
            righe_reali++;
            tot_cols += sizeof ( TheMs ) * colonne_allocate;

            for ( auto j = 0; j < colonne_allocate; ++j )
                if ( ms[i][j].val != zerox8000 )
                {
                    celle_piene++;
                }
        }
    }

    tot += tot_cols;
    tot += vb.Size();
    tot += vdegb.Size();

    std::cout << __func__ << "ROWS     : " << righe << "\t\tCOLS: " << colonne
              << "\t\tCELLS: " << righe* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS: "
              << colonne << "\t\tCELLS: " << righe_reali* colonne << std::endl;
              std::cout << __func__ << "VIRT-COLS: " << vb.Size() << "\tVIRT CELLS: "
              << vb.Size()* righe << "\tUSED CELLS: " << celle_piene << std::endl
              << std::endl;

    return tot;
}

void CMatriceCompressa_ex_2::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) base_allocazione );
    bs->Append ( ( unsigned int ) righe );
    bs->Append ( ( unsigned int ) colonne );
    vb.Serialize ( bs );
    vdegb.Serialize ( bs );

    if ( ms != NULL )
    {
        bs->Append ( ( unsigned char ) 1 );

        for ( unsigned int i = 0; i < righe; ++i )
        {
            if ( ms[i] != NULL )
            {
                bs->Append ( ( unsigned char ) 1 );

                for ( int j = 0; j < colonne; ++j )
                {
                    bs->Append ( ms[i][j].val );
                    bs->Append ( ms[i][j].val1 );
                    bs->Append ( ms[i][j].val2 );
                }
            }
            else
            {
                bs->Append ( ( unsigned char ) 0 );
            }
        }
    }
    else
    {
        bs->Append ( ( unsigned char ) 0 );
    }

    bs->Append ( ( unsigned int ) freeidx.size() );

    for ( auto it : freeidx )
    {
        bs->Append ( it.first );
        bs->Append ( it.second );
    }

    //    bs->Append((unsigned int) vb_adj.size());
    //    for (auto it : vb_adj) {
    //        bs->Append(it);
    //    }
}

void CMatriceCompressa_ex_2::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();

    base_allocazione = bs->Read<unsigned int>();
    righe = bs->Read<unsigned int>();
    colonne = bs->Read<unsigned int>();
    righe_allocate = righe;
    colonne_allocate = colonne;

    vb.Deserialize ( bs );
    vdegb.Deserialize ( bs );

    unsigned char m_null = bs->Read<unsigned char>();

    if ( m_null != 0 )
    {
        ms = ( TheMs** ) malloc ( righe_allocate * sizeof ( TheMs* ) );

        for ( unsigned int i = 0; i < righe_allocate; ++i )
        {
            //m[i] è null?
            unsigned char m_i_null = bs->Read<unsigned char>();

            if ( m_i_null != 0 )
            {
                ms[i] = ( TheMs* ) malloc ( colonne_allocate * sizeof ( TheMs ) );

                for ( int j = 0; j < colonne_allocate; ++j )
                {
                    ms[i][j].val = bs->Read<unsigned int>();
                    ms[i][j].val1 = bs->Read<unsigned int>();
                    ms[i][j].val2 = bs->Read<unsigned int>();
                    //                 if (!ms[i][j].val)
                    //                     cout << __func__   << "QUI";

                }
            }
            else
            {
                ms[i] = nullptr;
            }
        }
    }
    else
    {
        ms = nullptr;
    }

    freeidx.clear();
    unsigned int len = bs->Read<unsigned int>();

    for ( int i = 0; i < len; ++i )
    {
        unsigned int key = bs->Read<unsigned int>();
        unsigned int val = bs->Read<unsigned int>();
        freeidx[key] = val;
    }

    //    len = bs->Read<unsigned int>();
    //    for (int i = 0; i < len; ++i) {
    //        vb_adj.insert(bs->ReadULL());
    //    }

    //    //Il check lo faccio QUI!
    // for (int r=0; r<righe; ++r)
    //    {
    //        if (ms[r])
    //        {
    //            for (int c=0; c<colonne; ++c)
    //            {
    //                if (ms[r][c].val != zerox8000)
    //                {
    //                    unsigned int b = ms[r][c].val & zerox3FFF;
    //                    if (vb[b] != c)
    //                        cout << __func__   << "QUI";
    //                }
    //
    //            }
    //        }
    //    }
}

unsigned int LogPartition_ex_2::Exists ( unsigned int b, unsigned int a,
        unsigned int* punt, unsigned int** index_pointer,
        unsigned int** data_pointer )
{
    *index_pointer = nullptr;
    *data_pointer = nullptr;

    if ( a >= va.Size() )
    {
        return zerox8000;
    }

    if ( va[a] == zerox8000 )
    {
        return zerox8000;
    }

    return vm[va[a]]->Exists ( a, b, punt, index_pointer, data_pointer );
}

//bool
//LogPartition_ex_2::ExistsA(unsigned int a)
//{
//
//    if (a >= va.righe)
//        return false;
//
//    return va[a] != zerox8000;
//}

//int
//LogPartition_ex_2::GetDegreeA(unsigned int a)
//{
//  //a sta in una sola partizione, se c'è
//
//  if (a >= va.righe)
//    return 0;
//  if (va.v[a] == zerox8000)
//    return 0;
//  return vm[va.v[a]]->GetDegreeA(a);
//}

//unsigned long LogPartition_ex_2::GetNumB(unsigned int a)
//{
//  //quanti b ho qui dentro? MA dove? ovvio, dentro il a corrispondente
//  if (a >= va.righe)
//      return 0;
//  if (va.v[a]== zerox8000)
//      return 0;
//  return vm[va.v[a]]->b_nodes.size();
//}

//std::list<unsigned int> *
//LogPartition_ex_2::GetAllAConnectedToThisB(unsigned int b)
//{
//    std::list<unsigned int> *l = new std::list<unsigned int> ();
//
//    //Il problema è che questo b è condiviso tra tutte le partizioni di quest'insieme.
//    for (int p = 0; p < num_m; ++p)
//    {
//        if (b < vm[p]->vb.righe)
//        {
//            int col = vm[p]->vb[b];
//            if (col != zerox8000)
//            {
//                for (unsigned int r = 0; r < vm[p]->righe; ++r)
//                {
//                    if (vm[p]->ms[r][col].val == b)
//                    {
//                        l->push_back(r);
//                    }
//                }
//            }
//        }
//    }
//    return l;
//}
//
//void
//LogPartition_ex_2::DeleteEdge(unsigned int a, unsigned int b)
//{
//    //Dalla partizione a elimino l'arco a,b
//    vm[va[a]]->DeleteEdge(a, b);
//}

unsigned int LogPartition_ex_2::Insert ( unsigned int b, unsigned int a,
        bool termina, unsigned int punt )
{
    unsigned int vret = 0;
    //Tanto per sicurezza
    auto dummy = va[a];

    //Quanti nodi a abbiamo già in questa matrice?
    int partizione = va[a];

    if ( partizione == zerox8000 )
    {
        va[a] = 0; //parto da qui!!
        partizione = 0;
    }

    //Ma esiste il nodo b in questa partizione?
    if ( !vm[partizione]->ExistsB ( b ) )
    {
        auto vret = vm[partizione]->Insert ( a, b,
                                             counter++ | ( termina ? zerox4000 : zerox8000 ), punt ); //ho inserito una nuova coppia!
        return vret;
    }

    //Il nodo b esiste in questa partizione ne valuto il grado
    int deg = vm[partizione]->GetDegreeB ( b );

    if ( false || ( deg <= logn ) )
    {
        //Lo posso inserire senza problemi!
        auto vret = vm[partizione]->Insert ( a, b, counter++ | ( termina ? zerox4000 : zerox8000 ), punt );
        return vret;
    }

    //Questo arco a,b porta il grado di b sopra il log, devo spostare tutti i b collegati con questo a

    // Costruiamo l'elenco degli elementi b collegati con questo a che mi porto da un'altra parte
    std::set<unsigned int> list_b = vm[partizione]->GetAllBConnectedToThisA ( a );

    // Ma è meglio andare sempre avanti oppure ricominciare a cercare dall'inizio?
    // Ricominciamo a cercare dalla fine.
    for ( int i = ( num_m - 1 ); i >= 0; --i )
    {
        if ( i == partizione )
        {
            continue;
        }

        // Non è un problema di spazio, è un problema di grado.
        // Tutte queste coppie a,b che sto inserendo, aumentano o no il grado di un b qui dentro?
        // Tieni presente che essendo un unico a, non ci possono essere due b uguali altrimenti starei inserendo la stessa coppia.
        // Quindi per ognuno dei b, uno diverso dall'altro, verifico se incrementa il grado dei b esistenti fino alla fine
        for ( auto it : list_b )
        {
            if ( logn <= vm[i]->GetDegreeB ( it ) )
            {
                goto fine_ciclo;
            }
        }

        //Se sono qui, allora posso inserire tutte le coppiette a,b in questa partizione!
        va[a] = i; //a appartiene a questa partizione!

        for ( auto it : list_b )
        {
            unsigned int appo = vm[partizione]->ms[a][vm[partizione]->vb[it]].val1;
            unsigned int vappo = vm[partizione]->ms[a][vm[partizione]->vb[it]].val2;

            vm[i]->Insert ( a, it, appo, vappo ); //ORA ci sto mettendo il valore ORIGINALE!!! counter++ | (termina?zerox8000:zerox4000));

            //Lo elimino dalla precedente
            vm[partizione]->DeleteEdge ( a, it );
        }

        //Inserisco la corrente
        vret = vm[i]->Insert ( a, b, counter++ | ( termina ? zerox4000 : zerox8000 ), punt );

        return vret;

    fine_ciclo:
        unsigned int dummy = 0; //nop operation
    }

    //se sono qui non ho trovato nemmeno una partizione ammissibile, devo crearne una nuova
    num_m++;

    CMatriceCompressa_ex_2** vmtmp = new CMatriceCompressa_ex_2*[num_m];

    for ( int i = 0; i < ( num_m - 1 ); ++i )
    {
        vmtmp[i] = vm[i];
    }

    //le vm[i] le ho spostate. Devo solo cancellare vm
    delete vm;

    vm = vmtmp; //Assegno la nuova struttura ampliata
    vm[num_m - 1] = new CMatriceCompressa_ex_2();
    va[a] = num_m - 1;

    for ( auto it : list_b )
    {
        unsigned int appo = vm[partizione]->ms[a][vm[partizione]->vb[it]].val1;
        unsigned int vappo = vm[partizione]->ms[a][vm[partizione]->vb[it]].val2;
        vm[num_m - 1]->Insert ( a, it, appo, vappo );

        //Lo elimino dalla precedente
        vm[partizione]->DeleteEdge ( a, it );
    }

    //Inserisco la corrente
    vret = vm[num_m - 1]->Insert ( a, b, counter++ | ( termina ? zerox4000 : zerox8000 ), punt );

    return vret;
}

LogPartition_ex_2::LogPartition_ex_2()
{
    Init();
}

void LogPartition_ex_2::Init()
{
    //INIZIALIZZO LE COSTANTI DI INTERESSE

    vm = 0;
    num_m = 0;
    counter = 16;
    //Mi lascio le prime 15 righe come valori speciali.
    //Lo 0 non lo devo contare ma da 1 fino a 15 posso utilizzarli a mio piacimento!

    va.Init ( zerox8000 );

    int parts = 2; //8192; //parts foreste per ogni bipartito!
    int superparts = 2; //4*1024*1024; //max value of "a"
    //Già che ci sono, faccio 24 partizioni di uno spazio di 2^24 valori.
    va.Reallocate ( superparts ); //2^16 è più che sufficiente! Troppo, meglio 1024 (2^10)

    //Quanti nodi a abbiamo già in questa matrice?
    mtfa_fastrng1024 rng ( mtfa_rand_simple_ull() );

    for ( unsigned int i = 0; i < superparts; ++i )
    {
        //0000 0000 0000 0000 0000 0000
        //ma la distribuzione la devo fare il più casuale possibile
        va[i] = i % parts; //Quindi diventano parts partizioni iniziali!
    }

    //Faccio uno shuffle del vettore, anzi due
    //std::random_shuffle(&va.v[0], &va.v[superparts]); //, [&rng] (int i) {return rng.Next()%i;});
    //std::random_shuffle(&va.v[0], &va.v[superparts]); //, [&rng] (int i) {return rng.Next()%i;});

    vm = new CMatriceCompressa_ex_2*[parts];

    for ( int i = 0; i < parts; ++i )
    {
        vm[i] = new CMatriceCompressa_ex_2();
    }

    num_m = parts;
    //#endif
}

LogPartition_ex_2::~LogPartition_ex_2()
{
    //Dovrei fare la delete di m...
    for ( int i = 0; i < num_m; i++ )
        if ( vm[i] )
        {
            delete vm[i];
        }

    delete vm;

    vm = nullptr;
    num_m = 0;
    counter = 0;
}

void LogPartition_ex_2::Clear()
{
    va.Clear();

    if ( vm != nullptr )
    {
        for ( int i = 0; i < num_m; ++i )
        {
            //vm[i]->Clear();
            delete vm[i];
        }

        delete vm;
    }

    num_m = 0;
    vm = nullptr;

    counter = 0;
    Init();
}

void LogPartition_ex_2::Compress()
{
    for ( int i = 0; i < num_m; ++i )
    {
        vm[i]->Compress();
    }
}

unsigned long long LogPartition_ex_2::Size()
{
    unsigned long long tot = 0;
    tot += sizeof ( num_m );
    tot += sizeof ( counter );
    tot += va.Size();
    tot += sizeof ( vm );

    for ( int i = 0; i < num_m; ++i )
    {
        tot += sizeof ( vm[i] );
        tot += vm[i]->Size();
    }

    printf ( "LogPartition_ex_2. Needed: %llu, Total: %llu\n", tot, tot );

    return tot;
}

void LogPartition_ex_2::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) num_m );

    for ( int i = 0; i < num_m; ++i )
    {
        vm[i]->Serialize ( bs );
    }

    bs->Append ( ( unsigned int ) counter );
    va.Serialize ( bs );
}

void LogPartition_ex_2::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();

    if ( vm != nullptr )
    {
        for ( int i = 0; i < num_m; ++i )
        {
            //vm[i]->Clear();
            delete vm[i];
        }

        delete[] vm;
    }

    num_m = bs->Read<unsigned int>();
    vm = new CMatriceCompressa_ex_2*[num_m];

    for ( int i = 0; i < num_m; ++i )
    {
        vm[i] = new CMatriceCompressa_ex_2();
        vm[i]->Deserialize ( bs );
    }

    counter = bs->Read<unsigned int>();
    va.Deserialize ( bs );
}

//La classe mtfafs_s2  la classe per le stringhe di bytes di lunghezza >= 2

mtfafs_s2::mtfafs_s2()
{
    lp_num = 8;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }

    //zerox4000<unsigned int>().val = 0x40000000;
    //zerox8000<unsigned int>().val = 0x80000000;
    //zerox3FFF<unsigned int>().val = 0x3FFFFFFF;

}

//Comprime la matrice

void mtfafs_s2::Compress()
{
    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i]->Compress();
    }
}

//Aggiunge altro spazio per stringhe che sono troppo lunghe per le dimensioni attuali!

void mtfafs_s2::AddSpace()
{
    LogPartition_ex_2** tmp;
    int tmp_num = lp_num + 4;
    tmp = new LogPartition_ex_2*[tmp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        tmp[i] = lp[i];
    }

    for ( int i = lp_num; i < tmp_num; ++i )
    {
        tmp[i] = new LogPartition_ex_2();
    }

    delete[] lp;
    lp = tmp;
    lp_num = tmp_num;
}

//In questo modo decido a priori la lunghezza massima delle stringhe che accetto

mtfafs_s2::mtfafs_s2 ( int num )
{
    lp_num = num;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }
}

mtfafs_s2::~mtfafs_s2()
{
    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
        }
    }

    lp = NULL;
    lp_num = 0;
}

void mtfafs_s2::Clean()
{
    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
        }
    }

    lp = NULL;
    lp_num = 0;
}

void mtfafs_s2::Serialize ( mtfa_binary_stream_save* bs )
{
    //Metto lp_num
    bs->Append ( ( unsigned int ) lp_num );

    for ( int i = 0; i < lp_num; ++i )
    {
        if ( lp[i] != NULL )
        {
            bs->Append ( ( unsigned char ) 1 );
            lp[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }
    }
}

void mtfafs_s2::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clean();
    lp_num = bs->Read<unsigned int>();
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        unsigned char exists = bs->Read<unsigned char>();

        if ( exists == 1 )
        {
            lp[i] = new LogPartition_ex_2();
            lp[i]->Deserialize ( bs );
        }
        else
        {
            lp[i] = new LogPartition_ex_2();
            lp[i] = NULL;
        }
    }
}

bool mtfafs_s2::Exists ( std::string& data, unsigned int* final_value,
                         unsigned int** index_pointer, unsigned int** data_pointer )
{
    //Le stringhe DEVONO essere almeno lunghe 2
    int size = data.size();
    int cur_pos = 0;
    unsigned int val;
    val = ( unsigned char ) data[0];

    bool matched = true;
    // Se non sono uscito per matched, allora potrei
    // 1: essere uscito per next_pos >= (size-1), ovvero ho terminato e sono sull'ultimo carattere della stringa
    // 2: essere uscito per stringa che cerco pi lunga di quelle memorizzate

    while ( matched && cur_pos < ( size - 2 ) && lp_num > cur_pos
            && lp[cur_pos] != NULL )
    {
        val = lp[cur_pos]->Exists ( val & zerox3FFF,
                                    ( unsigned char ) data[cur_pos + 1], final_value, index_pointer,
                                    data_pointer );
        cur_pos++;

        if ( ! ( val & zerox3FFF ) )
        {
            return false; //en_mtfafs_NotFound;
        }

        if ( cur_pos >= lp_num )
        {
            //La lunghezza delle stringhe DEVE SEMPRE essere superiore a cur_pos.
            return false;
        }

        //if (val & zerox3FFF<unsigned int>().val)
        //{
        //    if (val & zerox4000)
        //        return en_mtfafs_ArchivedIsShorter;
        //}
    }

    val = lp[cur_pos]->Exists ( val & zerox3FFF, ( unsigned char ) data[size - 1],
                                final_value, index_pointer, data_pointer );

    //if (val == zerox8000)    //Questo non  possibile, si sarebbe fermata a ArchivedIsShorter
    if ( val & zerox4000 )   //8 significa che prosegue, 4 che si ferma, C che una prosegue e una ferma!
    {
        return true; //en_mtfafs_WholeMatched;
    }
    else
    {
        return false;
    }

    //else if (val == zerox8000)
    //{
    // return en_mtfafs_NotFound;
    //}
    //else
    //{
    // return en_mtfafs_SearchedIsShorter;
    //}
}

bool mtfafs_s2::Insert ( std::string& data, unsigned int* final_value )
{
    unsigned int* index_pointer;
    unsigned int* data_pointer;
    //Per ora posso inserire solo stringhe che non ho gi inserito
    unsigned int tmp;
    bool en_ret = Exists ( data, &tmp, &index_pointer, &data_pointer );

    if ( en_ret )   // != en_mtfafs_NotFound)
    {
        *final_value = tmp;
        return false; //in ogni caso non puoi inserire!!!Gi esiste questa stringa!
    }

    //Ora posso inserire!
    int size = 0;
    unsigned int val;

    size = data.length();

    bool matched = true;

    //Riconfigura se necessario lo spazio a disposizione!
    while ( size > lp_num )
    {
        AddSpace();
    }

    val = ( unsigned char ) data[0];

    for ( int i = 0; i < ( size - 2 ); ++i )
    {
        val = lp[i]->Insert ( val, ( unsigned int ) ( data[i + 1] ), false,
                              *final_value );

        if ( val == zerox8000 )
        {
            return false;
        }
        else
        {
            val &= zerox3FFF;
        }
    }

    val = lp[size - 2]->Insert ( val & zerox3FFF, ( unsigned int ) ( data[size - 1] ),
                                 true, *final_value ); //ho terminato la insert!

    if ( val == zerox8000 )
    {
        return false;
    }
    else
    {
        val &= zerox3FFF;     //zerox3FFFval;
    }

    return true;
}

/*************************************S2-BASE*****************************/

//La classe mtfafs_s2_base  la classe per le stringhe di bytes di lunghezza >= 1 e prevede la cancellazione e altre funzioni di utilit
//Aggiunge altro spazio per stringhe che sono troppo lunghe per le dimensioni attuali!
void mtfafs_s2_base::AddSpace()
{
    LogPartition_ex_2** tmp;
    int tmp_num = lp_num + 4;
    tmp = new LogPartition_ex_2*[tmp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        tmp[i] = lp[i];
    }

    for ( int i = lp_num; i < tmp_num; ++i )
    {
        tmp[i] = new LogPartition_ex_2();
    }

    delete[] lp;
    lp = tmp;
    lp_num = tmp_num;
}

//In questo modo decido a priori la lunghezza massima delle stringhe che accetto

mtfafs_s2_base::mtfafs_s2_base ( int num )
{
    lp_num = num;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }
}

mtfafs_s2_base::mtfafs_s2_base()
{
    //Lunghezza minima = 8
    lp_num = 8;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }
}

mtfafs_s2_base::~mtfafs_s2_base()
{
    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
        }
    }

    lp = NULL;
    lp_num = 0;
}

void mtfafs_s2_base::Clean()
{
    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
        }
    }

    lp_num = 0;
    lp = NULL;
}

void mtfafs_s2_base::Serialize ( mtfa_binary_stream_save* bs )
{
    //Metto lp_num
    bs->Append ( ( unsigned int ) lp_num );

    for ( int i = 0; i < lp_num; ++i )
    {
        if ( lp[i] != NULL )
        {
            bs->Append ( ( unsigned char ) 1 );
            lp[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }
    }
}

void mtfafs_s2_base::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clean();

    lp_num = bs->Read<unsigned int>();
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        unsigned char exists = bs->Read<unsigned char>();

        if ( exists == 1 )
        {
            lp[i] = new LogPartition_ex_2();
            lp[i]->Deserialize ( bs );
        }
        else
        {
            lp[i] = new LogPartition_ex_2();
            lp[i] = NULL;
        }
    }
}

bool mtfafs_s2_base::Exists ( const unsigned char* data, const int len_data,
                              unsigned int* final_value, unsigned int** index_pointer,
                              unsigned int** data_pointer )
{
    *index_pointer = nullptr;
    *data_pointer = nullptr;

    //Tolgo la limitazione sulle stringhe lunghe 2!
    //aggiungo SEMPRE in testa un valore di 1, qualunque sia la stringa!
    //int size = data.size();
    int cur_pos = 0;
    unsigned int val;
    //val=(unsigned char)data[0];
    val = ( unsigned char ) 1;

    //while ((cur_pos < (size-1)) && lp_num > cur_pos)
    while ( ( cur_pos < len_data ) && ( lp_num > cur_pos ) )
    {
        val = lp[cur_pos]->Exists ( val & zerox3FFF,
                                    ( unsigned char ) data[cur_pos], final_value, index_pointer,
                                    data_pointer );
        cur_pos++;

        if ( ( val & zerox3FFF ) == 0 )
        {
            return false; //NOT FOUND!
        }
    }

    //Sono uscito per assenza di dati?
    //if (cur_pos < (size-1))
    if ( cur_pos < len_data )
    {
        return false;
    }

    //Ma ti ho realmente trovato?
    if ( val & zerox4000 )
    {
        return true;
    }

    return false;
}

bool mtfafs_s2_base::Exists_substring ( const unsigned char* data,
                                        const int len_data,
                                        std::vector<std::pair<std::string, std::pair<int, int>>>& v_dati,
                                        int pos_attuale, unsigned int* final_value,
                                        unsigned int** index_pointer, unsigned int** data_pointer )
{
    *index_pointer = nullptr;
    *data_pointer = nullptr;

    unsigned int old_final_value;
    unsigned int* old_index_pointer;
    unsigned int* old_data_pointer;
    bool found = false;

    //Tolgo la limitazione sulle stringhe lunghe 2!
    //aggiungo SEMPRE in testa un valore di 1, qualunque sia la stringa!
    //int size = data.size();
    int cur_pos = 0;
    unsigned int val;
    //val=(unsigned char)data[0];
    val = ( unsigned char ) 1;

    //while ((cur_pos < (size-1)) && lp_num > cur_pos)
    while ( ( cur_pos < len_data ) && ( lp_num > cur_pos ) )
    {
        val = lp[cur_pos]->Exists ( val & zerox3FFF,
                                    ( unsigned char ) data[cur_pos], final_value, index_pointer,
                                    data_pointer );
        cur_pos++;

        if ( ( val & zerox3FFF ) == 0 )
        {
            //Ma se avevo trovato qualcos'altro prima, ritorno quello che avevo trovato!
            if ( found )
            {
                *final_value = old_final_value;
                *index_pointer = old_index_pointer;
                *data_pointer = old_data_pointer;
                return true;
            }
            else
            {
                return false;     //NOT FOUND!
            }
        }

        if ( val & zerox4000 )
        {
            //Trovata la sottostringa...a partire dalla posizione iniziale
            //Se ce ne fosse una pi lunga la utilizzerei!
            // Ma prima vedo se matchabno le posizioni di ricerca!
            //Allora, in quale range posso trovare questa stringa?
            int from = v_dati[*final_value].second.first;
            int to = v_dati[*final_value].second.second + from;

            if ( ( pos_attuale >= from )
                    && ( ( pos_attuale + v_dati[*final_value].first.size() ) <= to ) )
            {
                if ( val & zerox8000 )
                {
                    //Termina e prosegue, allora ci provo a proseguire!
                    old_final_value = *final_value;
                    old_index_pointer = *index_pointer;
                    old_data_pointer = *data_pointer;
                    found = true;
                }
                else
                {
                    return true; //FOUND!
                }
            }
            else
            {
                if ( val & zerox8000 )
                {
                    ; //Non faccio nulla poich non ho trovato la stringa! ma posso proseguire!
                }
                else
                {
                    if ( found )
                    {
                        *final_value = old_final_value;
                        *index_pointer = old_index_pointer;
                        *data_pointer = old_data_pointer;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    //Sono uscito per assenza di dati?
    //if (cur_pos < (size-1))
    if ( cur_pos < len_data )
    {
        //Ma se avevo trovato qualcos'altro prima, ritorno quello che avevo trovato!
        if ( found )
        {
            *final_value = old_final_value;
            *index_pointer = old_index_pointer;
            *data_pointer = old_data_pointer;
            return true;
        }
        else
        {
            return false;
        }
    }

    //Ma ti ho realmente trovato?
    if ( val & zerox4000 )
    {
        return true;
    }

    //Ma se avevo trovato qualcos'altro prima, ritorno quello che avevo trovato!
    if ( found )
    {
        *final_value = old_final_value;
        *index_pointer = old_index_pointer;
        *data_pointer = old_data_pointer;
        return true;
    }
    else
    {
        return false;
    }
}

//Sono sicuro che non esiste. La devo solo inserire!!!

bool mtfafs_s2_base::InsertFromHere ( unsigned int val, int pos,
                                      const unsigned char* data, const int len_data,
                                      unsigned int* final_value )
{
    unsigned int* index_pointer = nullptr;
    unsigned int* data_pointer = nullptr;

    //int size = data.size();

    unsigned int result = 0;

    while ( pos >= lp_num )
    {
        AddSpace();
    }

    //for (int i=pos; i<(size-1); ++i)
    for ( int i = pos; i < len_data; ++i )
    {
        unsigned appo = *final_value;
        //result = lp[i]->Exists(val & zerox3FFF<unsigned int>().val, (unsigned char)data[i+1], (void**)&appo, &index_pointer, &data_pointer);
        result = lp[i]->Exists ( val & zerox3FFF, ( unsigned char ) data[i], &appo,
                                 &index_pointer, &data_pointer );
        //Val ora puo valere zerox8000, zerox4000 oppure 0xC0000000, con gli altri bit settati oppure no nel caso 8, altrimenti sempre settati!

        //Se vale anche 0x04000000, allora qualcosa terminava qui ma non quell oche sto inserendo!
        if ( result & zerox4000 )
        {
            //Dato che la stringa che sto inserendo non esiste ancora nella struttura, modifico il valore per indicare fine e prosegue
            ( *index_pointer ) |= zerox8000; //Indico che qualunque cosa ci sia, questa cosa si ferma qui e prosegue anche!
            return InsertFromHere ( result & zerox3FFF, i + 1, data, len_data,
                                    final_value );
        }

        //Ma c' qualcosa qui sotto?
        if ( result != zerox8000 )
        {
            //S, qualcosa c', allora, nel caso, modifico il solo index pointer!
            //Ora, sono sull'ultimo carattere oppure ne mancano ancora?
            //if ((i+2)==data.size())
            if ( ( i + 1 ) == len_data )
            {
                ( *index_pointer ) |= zerox4000; //Si fermano qui i giochi per questa stringa.
                *data_pointer = *final_value;
                return true;
            }
            else
            {
                //Hey Joe, la presente stringa non  terminata e neppure la precedente, proseguo
                return InsertFromHere ( result & zerox3FFF, i + 1, data, len_data,
                                        final_value );
            }
        }
        else
        {
            //Se sono qui non ci sono dati ancora archiviati in questo punto, li inserisco io e poi proseguo, se devo

            //Ora, sono sull'ultimo carattere oppure ne mancano ancora?
            //if ((i+2)==size)
            if ( ( i + 1 ) == len_data )
            {
                //result = lp[i]->Insert(val & zerox3FFF<unsigned int>().val, (unsigned char)data[i+1], true, (void*)*final_value);
                result = lp[i]->Insert ( val & zerox3FFF, ( unsigned int ) data[i],
                                         true, *final_value );
                //*data_pointer = *final_value;
                return true;
            }
            else
            {
                //Hey Joe, la presente stringa non  terminata e neppure la precedente, proseguo
                //result = lp[i]->Insert(val & zerox3FFF<unsigned int>().val, (unsigned char)data[i+1], false, (void*)*final_value);
                result = lp[i]->Insert ( val & zerox3FFF, ( unsigned int ) data[i],
                                         false, *final_value );
                return InsertFromHere ( result & zerox3FFF, i + 1, data, len_data,
                                        final_value );
            }
        }

        val = result;
    }

    return true;
}

void mtfafs_s2_base::Size()
{
    unsigned long long tot_bytes = 0;
    unsigned long long tot_altro = 0;

    unsigned long long usate = 0;
    unsigned long long non_usate = 0;
    unsigned long long non_allocate = 0;
    printf ( "Allocate: %d partizioni.\n", lp_num );

    for ( int i = 0; i < lp_num; ++i )
    {
        printf ( "\tPartizione %3d. Foreste: %d\n", i, lp[i]->num_m );
        tot_bytes += ( lp[i]->va.Size() * 4 );
        tot_bytes += lp[i]->va.Size() * 4;
        tot_bytes += lp[i]->va.Size() * 4;
        //            tot_bytes += lp[i]->va.adiacenti.size()*(4+sizeof(T_SAdiacenti*));

        for ( int j = 0; j < lp[i]->num_m; ++j )
        {
            //tot_altro += (j<lp[i]->vm[j]->a_nodes.size()*8);
            //tot_altro += (j<lp[i]->vm[j]->b_nodes.size()*8);
            //      tot_altro += (j < lp[i]->vm[j]->vdegb->righe_attuali * 4);

            int righe_attive = 0;

            for ( int r = 0; r < lp[i]->vm[j]->righe_allocate; ++r )
            {
                if ( lp[i]->vm[j]->ms[r] != nullptr )
                {
                    //Anche la parte va...
                    tot_bytes += lp[i]->vm[j]->colonne_allocate * 4 * 3;
                    //for (T_Adiacenti::iterator it = lp[i]->va.adiacenti.begin(); it != lp[i]->va.adiacenti.end(); ++it)
                    // tot_bytes += ((*it).second->dim * 4);
                    righe_attive++;

                    for ( int c = 0; c < lp[i]->vm[j]->colonne_allocate; ++c )
                        if ( lp[i]->vm[j]->ms[r][c].val == zerox8000 )
                        {
                            usate++;
                        }
                        else
                        {
                            non_usate++;
                        }
                }
                else
                {
                    non_allocate += lp[i]->vm[j]->colonne_allocate;
                    tot_bytes += 12;
                }
            }

            tot_bytes += righe_attive * lp[i]->vm[j]->colonne_allocate * 12
            + lp[i]->vm[j]->vb.Size() * 4;
            //               printf("\t\tMatrice: %3d. Righe: %d. Items: %d. VB: %3d.\n", j, righe_attive, righe_attive*lp[i]->vm[j]->colonne, lp[i]->vm[j]->vb.righe);
        }
    }

    printf ( "TOTALE GENERALE: %lld bytes\n", tot_bytes + tot_altro );
    printf ( "DI CUI: FORESTE %lld, OTHER %lld\n", tot_bytes, tot_altro );
    printf ( "INDICATORI DI RIEMPIMENTO\n" );
    printf ( "\tTOTALE alberi			: %lld\n", usate + non_usate + non_allocate );
    printf ( "\tTOTALE usati   		: %lld\n", usate );
    printf ( "\tTOTALE NON_USATI		: %lld\n", non_usate );
    printf ( "\tTOTALE NON ALLOCATI	: %lld\n", non_allocate );
    printf ( "Percentuale di compressione matrice sparsa: %lf\n",
             ( double ) ( usate + non_usate )
             / ( double ) ( usate + non_usate + non_allocate ) );
}

bool mtfafs_s2_base::Insert ( const unsigned char* data, const int len_data,
                              unsigned int* final_value )
{
    try
    {
        unsigned int tmp;

        //Questa parte di esistenza non mi dovrebbe interessare, la tolgo!!!
        unsigned int* index_pointer = nullptr;
        unsigned int* data_pointer = nullptr;

        bool en_ret;
        en_ret = Exists ( data, len_data, &tmp, &index_pointer, &data_pointer );

        if ( en_ret )
        {
            *final_value = tmp;
            return false; //in ogni caso non puoi inserire!!!Gi esiste questa stringa!
        }

        int size = 0;
        unsigned int val;

        //size = data.length();

        val = 1; //(unsigned char)(data[0]);

        en_ret = InsertFromHere ( val, 0, data, len_data, final_value );

        return en_ret;

    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
        return false;
    }
}

/*************************************S2-BASE*****************************/

//void
//VStringhe::Initialize()
//{
//    //Sarebbe meglio fare la free, se != nullptr!!!
//    memset(vettore, 0, sizeof (unsigned char*)*allocati);
//    memset(v_len, 0, sizeof (unsigned int)*allocati);
//
//    len = 1; //il posto 0 non esiste!!!
//
//    //    allocati=1024;
//}
//
//VStringhe::VStringhe()
//{
//    vettore = (unsigned char**) malloc(BASE_ALLOCAZIONE * sizeof (unsigned char*)); //new   std::string *[1024];  //Parto da 1024 e faccio incrementi di 1024 per volta
//    v_len = (unsigned int*) malloc(BASE_ALLOCAZIONE * sizeof ( unsigned int)); //new   std::string *[1024];   //Parto da 1024 e faccio incrementi di 1024 per volta
//    memset(vettore, 0, sizeof (unsigned char*)*BASE_ALLOCAZIONE);
//    memset(v_len, 0, sizeof (unsigned int)*BASE_ALLOCAZIONE);
//
//    len = 1; //il posto 0 non esiste!!!
//
//    allocati = BASE_ALLOCAZIONE;
//    Initialize();
//}
//
//
//VStringhe::~VStringhe()
//{
//    if (allocati > 0)
//    {
//        for (int i = 0; i < allocati; ++i)
//        {
//            if (vettore[i] != nullptr)
//            {
//                free(vettore[i]);
//            }
//        }
//        free(vettore);
//        free(v_len);
//        vettore = nullptr;
//        v_len = nullptr;
//        len = 1;
//        allocati = 0;
//    }
//    vettore = nullptr;
//    v_len = nullptr;
//    len = 1;
//    allocati = 0;
//}
//
//void
//VStringhe::Clear()
//{
//    if (allocati > 0)
//    {
//        for (int i = 0; i < allocati; ++i)
//        {
//            if (vettore[i] != nullptr)
//                free(vettore[i]);
//        }
//        free(vettore);
//        free(v_len);
//        vettore = nullptr;
//        v_len = nullptr;
//        len = 0;
//        allocati = 0;
//    }
//    vettore = nullptr;
//    v_len = nullptr;
//    len = 0;
//    allocati = 0;
//    //E poi mi rimetto nelle condizioni iniziali!
//    Initialize();
//}
//
//void
//VStringhe::Rialloca(const int & pos)
//{
//    if (pos < allocati)
//        return;
//
//    int da_allocare = pos + BASE_ALLOCAZIONE; //TOTALE DA ALLOCARE
//    //vettore = (unsigned char **)realloc(vettore, sizeof(unsigned char*)*da_allocare);
//    //v_len   = (unsigned int *)  realloc(v_len,   sizeof(unsigned int)  *da_allocare);
//
//    unsigned char ** tmp_vettore = (unsigned char **) malloc(sizeof (unsigned char*)*da_allocare);
//    unsigned int * tmp_v_len = (unsigned int *) malloc(sizeof (unsigned int)*da_allocare);
//
//    if (vettore != nullptr)
//    {
//        std::copy(&(vettore[0]), &(vettore[allocati]), tmp_vettore);
//        std::copy(&(v_len[0]), &(v_len[allocati]), tmp_v_len);
//
//        free(vettore);
//        free(v_len);
//    }
//
//    vettore = tmp_vettore;
//    v_len = tmp_v_len;
//
//    std::fill(&vettore[allocati], &vettore[da_allocare], nullptr);
//    std::fill(&v_len[allocati], &v_len[da_allocare], 0);
//
//    //memset(&vettore[allocati], 0, sizeof(unsigned char*)*(da_allocare-allocati));
//    //memset(&v_len[allocati], 0, sizeof(unsigned int)*(da_allocare-allocati));
//    allocati = da_allocare;
//}
//
//int
//VStringhe::push_back(unsigned char * p, int plen)
//{
//    if (len >= allocati)
//    {
//        Rialloca(len + 1);
//    }
//    if (vettore[len] != nullptr)
//        free(vettore[len]);
//
//    vettore[len] = (unsigned char *) malloc(plen * sizeof (unsigned char));
//    memcpy(&(vettore[len][0]), p, sizeof (char)*plen);
//    v_len[len] = plen;
//    len++;
//    return len - 1;
//}
//
//int
//VStringhe::push_back(std::string s)
//{
//    return push_back((unsigned char*) s.data(), s.size());
//}
//
//std::string VStringhe::operator[](const int & pos)
//{
//    //if (pos < allocati)
//    //{
//    if (pos < len)
//    {
//        return std::string(&vettore[pos][0], &vettore[pos][v_len[pos]]);
//    }
//    else
//    {
//        return std::string();
//    }
//    //}
//    //else
//    //{
//    //    ////Devo riallocare!!! Ma non lo faccio!
//    //    //Rialloca(pos);
//    //    //len = pos+1;
//    //    return std::string();
//    //}
//}
//
//inline unsigned char *
//VStringhe::Get(const int & pos, int & plen)
//{
//    //if (pos < allocati)
//    //{
//    if (pos < len)
//    {
//        plen = v_len[pos];
//        return vettore[pos];
//    }
//    else
//    {
//        return nullptr;
//    }
//    //}
//    //else
//    //{
//    //    return nullptr;
//    //}
//}
//
//unsigned long long
//VStringhe::Size()
//{
//    unsigned long long tot = 0;
//    tot += sizeof (vettore);
//    tot += sizeof (v_len);
//    tot += sizeof (len);
//    tot += sizeof (allocati);
//    tot += sizeof (unsigned int)*allocati;
//    tot += sizeof (unsigned char *)*allocati;
//    for (int i = 0; i < len; ++i)
//        tot += sizeof (unsigned char)*v_len[i];
//    printf("VStringhe. Needed: %llu, Total: %llu\n", tot - sizeof (unsigned char *)*(allocati - len), tot);
//    return tot;
//}
//
//void
//VStringhe::Serialize(mtfa_binary_stream_save *bs)
//{
//    bs->Append((unsigned int) allocati);
//    bs->Append((unsigned int) len);
//    for (int i = 0; i < len; ++i)
//    {
//        bs->Append((unsigned char*) vettore[i], v_len[i]); //std::string(&vettore[i][0], &vettore[i][v_len[i]]));
//    }
//}
//
//void
//VStringhe::Deserialize(mtfa_binary_stream_load *bs)
//{
//    //Clear();
//    //Leggo quanti sono allocati e quanti ce ne sono attivi
//    allocati = bs->Read<unsigned int>();
//    len = bs->Read<unsigned int>();
//
//    vettore = (unsigned char **) malloc(sizeof (char*)*allocati);
//    v_len = (unsigned int *) malloc(sizeof (unsigned int)*allocati);
//    for (int i = 0; i < len; ++i)
//    {
//        unsigned int len1;
//        *(unsigned char **) &vettore[i] = bs->ReadVUC(len1);
//        v_len[i] = len1;
//    }
//    //    _ASSERTE (_CrtCheckMemory ());
//}
/*****************************************************************************************************************/
//La classe mtfafs_s2_extended  la classe per le stringhe di bytes di lunghezza >= 2 ma che  ottimizzata
//per gestire solo la parte di stringa che corrisponde a quanto gi inserito e che prevede anche la cancellazione!
//Aggiunge altro spazio per stringhe che sono troppo lunghe per le dimensioni attuali!
void mtfafs_s2_extended::AddSpace ( int howmany )
{
    for ( int hm = 0; hm < howmany; ++hm )
    {
        LogPartition_ex_2** tmp;
        int tmp_num = lp_num + 2;
        tmp = new LogPartition_ex_2*[tmp_num];

        for ( int i = 0; i < lp_num; ++i )
        {
            tmp[i] = lp[i];
        }

        for ( int i = lp_num; i < tmp_num; ++i )
        {
            tmp[i] = new LogPartition_ex_2();
        }

        delete[] lp;
        lp = tmp;
        lp_num = tmp_num;
    }
}

//In questo modo decido a priori la lunghezza massima delle stringhe che accetto

mtfafs_s2_extended::mtfafs_s2_extended ( int num )
{
    v_dati = new CVettorePString();
    lp_num = num;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }

    //per le stringhe lunghe 1 e due
    pos_1.resize(256, 0);
    pos_2.resize(256*256, 0);
    unsigned int val = zerox8000;
    std::fill ( pos_1.begin(), pos_1.end(), val );
    std::fill ( pos_2.begin(), pos_2.end(), val );
}

mtfafs_s2_extended::mtfafs_s2_extended()
{
    v_dati = new CVettorePString();
    //Lunghezza minima = 8
    lp_num = 8;
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i] = new LogPartition_ex_2();
    }

    //per le stringhe lunghe 1 e due
    unsigned int val = zerox8000;
    pos_1.resize(256, val);
    pos_2.resize(256 * 256, val);
}

mtfafs_s2_extended::~mtfafs_s2_extended()
{
    v_dati->Clear();

    if ( v_dati != nullptr )
    {
        delete v_dati;
    }

    v_dati = nullptr;

    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
            lp = NULL;
        }

        lp_num = 0;
    }

    pos_1.clear();
    pos_2.clear();

    lp_num = 0;
    lp = NULL;
}

void mtfafs_s2_extended::Clean()
{
    v_dati->Clear();

    if ( lp_num > 0 )
    {
        if ( lp != NULL )
        {
            for ( int i = 0; i < lp_num; ++i )
            {
                if ( lp[i] != NULL )
                {
                    delete lp[i];
                }
            }

            delete lp;
            lp = NULL;
        }

        lp_num = 0;
    }

    pos_1.clear();
    pos_2.clear();

    lp_num = 0;
    lp = NULL;
}

void mtfafs_s2_extended::Serialize ( mtfa_binary_stream_save* bs )
{
    //Prima ci metto il vettore di stringhe...
    bs->Append ( ( unsigned int* ) &pos_1[0], 256 );
    bs->Append ( ( unsigned int* ) &pos_2[0], 256 * 256 );

    //Metto lp_num
    bs->Append ( ( unsigned int ) lp_num );

    for ( int i = 0; i < lp_num; ++i )
    {
        if ( lp[i] != NULL )
        {
            bs->Append ( ( unsigned char ) 1 );
            lp[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }
    }

    v_dati->Serialize ( bs );
}

void mtfafs_s2_extended::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clean();
    v_dati = new CVettorePString();
    unsigned int val = zerox8000;

    unsigned int ll;
    * ( unsigned int** ) &pos_1[0] = bs->Read<unsigned int> ( ll );
    * ( unsigned int** ) &pos_2[0] = bs->Read<unsigned int> ( ll );

    lp_num = bs->Read<unsigned int>();
    lp = new LogPartition_ex_2*[lp_num];

    for ( int i = 0; i < lp_num; ++i )
    {
        unsigned char exists = bs->Read<unsigned char>();

        if ( exists == 1 )
        {
            lp[i] = new LogPartition_ex_2();
            lp[i]->Deserialize ( bs );
        }
        else
        {
            lp[i] = new LogPartition_ex_2();
        }
    }

    v_dati->Deserialize ( bs );
}

bool mtfafs_s2_extended::Exists ( const unsigned char* data, int data_len,
                                  unsigned int* final_value, unsigned int** index_pointer,
                                  unsigned int** data_pointer )
{
    switch ( data_len )
    {
        case 1:
            if ( pos_1[*data] == zerox8000 )
            {
                //posto libero. Not found
                return false;
            }
            else
            {
                //in questo caso il posto è occupato! Torno il valore e la posizione.
                *data_pointer = &pos_1[*data];
                *final_value = pos_1[*data];
                *index_pointer = nullptr;
                return true;
            }

            break;

        case 2:
            if ( pos_2[* ( unsigned short* ) data] == zerox8000 )
            {
                //posto libero. Not found
                return false;
            }
            else
            {
                //in questo caso il posto è occupato! Torno il valore e la posizione.
                *data_pointer = &pos_2[* ( unsigned short* ) data];
                *final_value = pos_2[* ( unsigned short* ) data];
                *index_pointer = nullptr;
                return true;
            }

            break;

        default: //non faccio nulla. Proseguo con next step
            break;
    }

    *index_pointer = nullptr;
    *data_pointer = nullptr;

    unsigned int val = * ( unsigned short* ) data;

    int size = data_len;
    int cur_pos = 2;

    LogPartition_ex_2** lpe2 = lp;

    TheMs* ma;

    while ( cur_pos < size && lp_num > ( ( cur_pos - 1 ) / 2 ) )
    {
        unsigned int b;
        unsigned int a;

        LogPartition_ex_2& pplp = **lpe2;

        b = val & zerox3FFF;

        if ( ( data_len - cur_pos ) == 1 )
        {
            //ultimo valore dispari!
            a = 0x10000 + ( unsigned short ) data[cur_pos];
            ++cur_pos;
        }
        else
        {
            a = * ( unsigned short* ) &data[cur_pos]; //(unsigned char)data[cur_pos];
            cur_pos += 2;
        }

        if ( a >= pplp.va.Size() )
        {
            return false;
        }

        unsigned int lpe2vava;

        if ( ( lpe2vava = pplp.va[a] ) == 0x80000000 )
        {
            return false;
        }

        CMatriceCompressa_ex_2& pplpvm2 = *pplp.vm[lpe2vava];

        if ( a >= pplpvm2.righe )
        {
            return false;
        }

        if ( b >= pplpvm2.vb.Size() )
        {
            return false;
        }

        unsigned int pplpvm2vbvb;

        if ( ( pplpvm2vbvb = pplpvm2.vb[b] ) == 0x80000000 )
        {
            return false;
        }

        if ( ! ( ma = pplpvm2.ms[a] ) )
        {
            return false;
        }

        if ( ma[pplpvm2vbvb].val != b )
        {
            return false;
        }

        TheMs& ama = pplpvm2.ms[a][pplpvm2vbvb];
        ++lpe2;

        if ( ( val = ama.val1 ) & 0x40000000 )   //zerox4000<unsigned int>().val)
        {
            if ( ama.val2 < v_dati->righe )   //->len)
            {
                if ( v_dati->v_len[ama.val2] == data_len
                        && memcmp ( & ( *v_dati ) [ama.val2][cur_pos], &data[cur_pos],
                                    data_len - cur_pos ) == 0
                   )
                {
                    *final_value = ama.val2;
                    *index_pointer = & ( ama.val1 );
                    *data_pointer = ( unsigned int* ) & ( ama.val2 );
                    return true;
                }
            }
        }

        if ( ( val & zerox3FFF ) == 0 )
        {
            return false;
        }
    }

    return false;
}

bool mtfafs_s2_extended::ExistsFast ( const unsigned char* data, int data_len, unsigned int* final_value )
{
    switch ( data_len )
    {
        case 1:
            if ( pos_1[*data] == zerox8000 )
            {
                //posto libero. Not found
                return false;
            }
            else
            {
                //in questo caso il posto è occupato! Torno il valore e la posizione.
                *final_value = pos_1[*data];
                return true;
            }

            break;

        case 2:
            if ( pos_2[* ( unsigned short* ) data] == zerox8000 )
            {
                //posto libero. Not found
                return false;
            }
            else
            {
                //in questo caso il posto è occupato! Torno il valore e la posizione.
                *final_value = pos_2[* ( unsigned short* ) data];
                return true;
            }

            break;

        default: //non faccio nulla. Proseguo con next step
            break;
    }


    unsigned int val = * ( unsigned short* ) data;

    int cur_pos = 2;

    LogPartition_ex_2** lpe2 = lp;

    TheMs* ma;
    int howmany = data_len - cur_pos;

    while ( howmany && lp_num > ( ( cur_pos - 1 ) >> 1 ) )
    {
        unsigned int b;
        unsigned int a;

        LogPartition_ex_2& pplp = **lpe2;

        b = val & zerox3FFF;

        if ( howmany == 1 )
        {
            //ultimo valore dispari!
            a = 0x10000 + ( unsigned short ) data[cur_pos];
            ++cur_pos;
            --howmany;
        }
        else
        {
            a = * ( unsigned short* ) &data[cur_pos]; //(unsigned char)data[cur_pos];
            cur_pos += 2;
            howmany -= 2;
        }

        unsigned int lpe2vava;
        CMatriceCompressa_ex_2* pplpvm2;
        unsigned int pplpvm2vbvb;

        if ( a >= pplp.va.Size() ||
                ( lpe2vava = pplp.va[a] ) == 0x80000000 ||
                ( pplpvm2 = pplp.vm[lpe2vava], a >= pplpvm2->righe ) ||
                b >= pplpvm2->vb.Size() ||
                ( pplpvm2vbvb = pplpvm2->vb[b] ) == 0x80000000 ||
                ! ( ma = pplpvm2->ms[a] ) ||
                ma[pplpvm2vbvb].val != b
           )
            return false;

        TheMs& ama = pplpvm2->ms[a][pplpvm2vbvb];

        if ( ( ( val = ama.val1 ) & 0x40000000 )  &&
                ( *final_value = ama.val2, *final_value < v_dati->righe ) &&
                v_dati->v_len[*final_value] == data_len &&
                memcmp ( & ( *v_dati ) [*final_value][cur_pos], &data[cur_pos], data_len - cur_pos ) == 0
           )
            return true;

        if ( ( val & zerox3FFF ) == 0 )
        {
            return false;
        }

        ++lpe2;
    }

    return false;
}

bool mtfafs_s2_extended::InsertFromHere ( unsigned int val, int pos, unsigned char* data, int datalen, unsigned int* final_value )
{
    unsigned int* index_pointer = nullptr;
    unsigned int* data_pointer = nullptr;

    int size = datalen;

    unsigned int result = 0;

    AddSpace ( ( pos >> 1 ) - lp_num + 1 );

    //Se non esiste questa coppia, allora inserisco e mi fermo
    for ( int i = pos; i < size; i += 2 )
    {
        unsigned int current_data = ( ( datalen - i ) > 1 ) ? * ( unsigned short* ) &data[i] : ( 0x10000 + ( unsigned short ) data[i] );
        unsigned appo = *final_value;

        result = lp[ ( i - 1 ) >> 1]->Exists ( val & zerox3FFF, current_data, &appo, &index_pointer, &data_pointer );

        if ( result & zerox4000 )
        {
            std::string precedente;
            auto precedente_size = v_dati->v_len[*data_pointer];
            precedente.assign ( ( char* ) ( *v_dati ) [*data_pointer], ( char* ) ( *v_dati ) [*data_pointer] + precedente_size ); //v_dati->v_len[*data_pointer] );

            //Se le dimensioni sono le stesse e tutte e due si fermano qui e sono uguali da qui in poi
            //, allora non inserisco e ho terminato
            if ( precedente_size == size && precedente.compare ( string ( data, data + size ) ) == 0 )
            {
                *final_value = *data_pointer;
                return true;
            }
            else if ( precedente_size > ( i + 2 ) )     //ora avanzo di due alla volta!
            {
                //azzero i valori precedenti
                ( *index_pointer ) &= zerox3FFF; //0011 1111 1111 1111 1111 1111 1111 1111
                ( *index_pointer ) |= zerox8000; //Indico che qualunque cosa ci sia, questa cosa prosegue ora!

                unsigned int old_data_pointer = *data_pointer;
                *data_pointer = 0;
                InsertFromHere ( result & zerox3FFF, i + 2, ( unsigned char* ) precedente.data(), precedente_size, &old_data_pointer );

                if ( datalen == ( i + 2 ) )
                {
                    ( *index_pointer ) |= zerox4000; //Si fermano qui i giochi per questa stringa.
                    *data_pointer = *final_value;
                    return true;
                }
                else
                {
                    return InsertFromHere ( result & zerox3FFF, i + 2, data, datalen, final_value );
                }
            }
            else
            {
                //La precedente terminava effettivamente qui, metto il valore a C e proseguo.
                ( *index_pointer ) |= zerox8000; //Ma tanto mi aspetto che questo valore esista già!

                //E ora inserisco l'attuale!
                return InsertFromHere ( result & zerox3FFF, i + 2, data, datalen, final_value );
            }
        }

        if ( result == zerox8000 )
        {
            result = lp[ ( i - 1 ) / 2]->Insert ( val & zerox3FFF, current_data, true, *final_value );
            return true;
        }
        else
        {
            //Ma sono a fine stringa?
            if ( ( i + 1 ) >= ( size - 1 ) )
            {
                ( *index_pointer ) |= zerox4000; //Si fermano qui i giochi per questa stringa.
                *data_pointer = *final_value;
                return true;
            }
        }

        val = result;
    }

    return false;
}

unsigned long long mtfafs_s2_extended::Size()
{
    //  printf("v_dati. allocati: %d, len: %d\n", v_dati->allocati, v_dati->len);
    //  printf("TOT LPNUM: %d\n", this->lp_num);
    //  for (int i = 0; i < this->lp_num; ++i)
    //  {
    //    printf("LPNUM: %d.\n\tCounter: %d. NumM: %d", i, lp[i]->counter, lp[i]->num_m);
    //    printf("\tVA. Adiacenti: %d. Baseall: %d. Righe: %d. RAttuali: %d\n", lp[i]->va.adiacenti->Size(), lp[i]->va.base_allocazione, lp[i]->va.righe, lp[i]->va.righe_attuali);
    //    for (int j = 0; j < lp[i]->num_m; ++j)
    //    {
    //      printf("\tVM. Baseall: %d. Colonne: %d. CAllocate: %d. Righe: %d. Rallocate: %d", lp[i]->vm[j]->base_allocazione, lp[i]->vm[j]->colonne, lp[i]->vm[j]->colonne_allocate,
    //             lp[i]->vm[j]->righe, lp[i]->vm[j]->righe_allocate);
    //      printf("\t\tVMvb. Adiacenti: %d. Baseall: %d. Righe: %d. RAttuali: %d\n", lp[i]->vm[j]->vb->adiacenti->Size(), lp[i]->vm[j]->vb->base_allocazione, lp[i]->vm[j]->vb->righe, lp[i]->vm[j]->vb->righe_attuali);
    //      printf("\t\tVMvdegb. Adiacenti: %d. Baseall: %d. Righe: %d. RAttuali: %d\n", lp[i]->vm[j]->vdegb->adiacenti->Size(), lp[i]->vm[j]->vdegb->base_allocazione, lp[i]->vm[j]->vdegb->righe, lp[i]->vm[j]->vdegb->righe_attuali);
    //    }
    //  }
    //return 0;

    unsigned long long tot = 0;

    //Ci sommo pos1 e pos2

    tot += 256 * sizeof ( unsigned int ) + 256 * 256 * sizeof ( unsigned int );

    //Metto lpnum
    tot += sizeof ( lp_num ); //lpnum
    tot += sizeof ( v_dati );
    tot += v_dati->Size();

    tot += sizeof ( lp );
    tot += sizeof ( *lp ) * lp_num;

    // printf("%d LogPartition_ex_2\n", lp_num);
    for ( int i = 0; i < lp_num; ++i )
    {
        unsigned long long t = lp[i]->Size();
        //        printf("lp[%d]: %llu\n", i, t);
        tot += t;
    }

    printf ( "mtfafs_s2_extended. Needed: %llu, Total: %llu\n", tot, tot );
    return tot;
    //printf("TOTALE GENERALE: %llu bytes\n", tot);
    //printf("DI CUI: MAT %lld, OTHER %lld, DATAVECT %lld\n", tot_bytes, tot_altro, v_dati_size);
    //printf("INDICATORI DI RIEMPIMENTO\n");
    //printf("\tTOTALE CELLE           : %lld\n", usate+non_usate+non_allocate);
    //printf("\tTOTALE USATE           : %lld\n", usate);
    //printf("\tTOTALE NON_USATE       : %lld\n", non_usate);
    //printf("\tTOTALE NON ALLOCATE    : %lld\n", non_allocate);
    //printf("Percentuale di compressione matrice sparsa: %lf\n", (double)(usate+non_usate)/(double)(usate+non_usate+non_allocate));
}

void mtfafs_s2_extended::Compress()
{
    for ( int i = 0; i < lp_num; ++i )
    {
        lp[i]->Compress();
    }
}

bool mtfafs_s2_extended::Insert ( unsigned char* data_p, int data_len,
                                  unsigned int* final_value )
{
    try
    {
        //Se lunga 1, allora inserisco in pos_1, se 2 in pos_2
        if ( data_len == 1 )
        {
            if ( pos_1[*data_p] == zerox8000 )
            {
                //posto libero. posso inserire.
                *final_value = v_dati->push_back ( data_p, data_len ); //Il valore/posizione che assegno a questa stringa. Questo va aggiornato al termine della insert!
                pos_1[*data_p] = *final_value;
                return true;
            }
            else
            {
                *final_value = pos_1[*data_p];
                return true;
            }
        }
        else if ( data_len == 2 )
        {
            if ( pos_2[* ( unsigned short* ) data_p] == zerox8000 )
            {
                //posto libero. posso inserire.
                *final_value = v_dati->push_back ( data_p, data_len ); //Il valore/posizione che assegno a questa stringa. Questo va aggiornato al termine della insert!
                pos_2[* ( unsigned short* ) data_p] = *final_value;
                return true;
            }
            else
            {
                //in questo caso il posto è occupato! Torno "SEMPRE" false.
                *final_value = pos_2[* ( unsigned short* ) data_p];
                return true;
            }
        }

        unsigned int val;
        val = * ( unsigned short* ) data_p;

        *final_value = v_dati->righe;
        auto old_final_value = *final_value;

        bool bRet = InsertFromHere ( val, 2, data_p, data_len, final_value );

        //Se inserito in ds, appendo il valore
        if ( ( *final_value ) == old_final_value )
            v_dati->push_back ( data_p, data_len ); //Il valore/posizione che assegno a questa stringa. Questo va aggiornato al termine della insert!

        return true;    //in ogni caso la do per inserita, anche quando già presente

    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
        return false;
    }
}

/*****************************************************************************************************/

/*****************************************************************************************************************/
/*
 //La classe mtfafs_s2_extended_v1
 void mtfafs_s2_extended_v1::AddSpace(int howmany)
 {
 for (int hm = 0; hm < howmany; ++hm) {
 LogPartition_ex_2 ** tmp;
 int tmp_num = lp_num + 2;
 tmp = new LogPartition_ex_2*[tmp_num];
 for (int i = 0; i < lp_num; ++i)
 tmp[i] = lp[i];

 for (int i = lp_num; i < tmp_num; ++i)
 tmp[i] = new LogPartition_ex_2();

 delete lp;
 lp = tmp;
 lp_num = tmp_num;
 }
 }


 //In questo modo decido a priori la lunghezza massima delle stringhe che accetto

 mtfafs_s2_extended_v1::mtfafs_s2_extended_v1(int num) {
 v_dati = new CVettorePString();
 lp_num = num;
 lp = new LogPartition_ex_2*[lp_num];
 for (int i = 0; i < lp_num; ++i)
 lp[i] = new LogPartition_ex_2();

 unsigned int val = zerox8000;
 }

 mtfafs_s2_extended_v1::mtfafs_s2_extended_v1() {
 v_dati = new CVettorePString();
 //Lunghezza minima = 8
 lp_num = 8;
 lp = new LogPartition_ex_2*[lp_num];
 for (int i = 0; i < lp_num; ++i)
 lp[i] = new LogPartition_ex_2();

 unsigned int val = zerox8000;
 }

 mtfafs_s2_extended_v1::~mtfafs_s2_extended_v1() {
 v_dati->Clear();
 if (v_dati != nullptr)
 delete v_dati;
 v_dati = nullptr;
 if (lp_num > 0) {
 if (lp != NULL) {
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL)
 delete lp[i];
 }
 delete lp;
 lp = NULL;
 }
 lp_num = 0;
 }
 lp_num = 0;
 lp = NULL;
 }

 void
 mtfafs_s2_extended_v1::Clean() {
 v_dati->Clear();
 if (lp_num > 0) {
 if (lp != NULL) {
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL)
 delete lp[i];
 }
 delete lp;
 lp = NULL;
 }
 lp_num = 0;
 }

 lp_num = 0;
 lp = NULL;
 }

 void
 mtfafs_s2_extended_v1::Serialize(mtfa_binary_stream_save *bs)
 {
 //Metto lp_num
 bs->Append((unsigned int) lp_num);
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL) {
 bs->Append((unsigned char) 1);
 lp[i]->Serialize(bs);
 } else {
 bs->Append((unsigned char) 0);
 }
 }
 v_dati->Serialize(bs);
 }

 void
 mtfafs_s2_extended_v1::Deserialize(mtfa_binary_stream_load *bs) {
 Clean();
 v_dati = new CVettorePString();
 unsigned int val = zerox8000;

 unsigned int ll;

 lp_num = bs->Read<unsigned int>();
 lp = new LogPartition_ex_2*[lp_num];

 for (int i = 0; i < lp_num; ++i) {
 unsigned char exists = bs->Read<unsigned char>();
 if (exists == 1) {
 lp[i] = new LogPartition_ex_2();
 lp[i]->Deserialize(bs);
 } else {
 lp[i] = new LogPartition_ex_2();
 }
 }

 v_dati->Deserialize(bs);
 }


 bool mtfafs_s2_extended_v1::Exists(const unsigned char * data, int data_len, unsigned int *final_value, unsigned int ** index_pointer, unsigned int ** data_pointer)
 {
 *index_pointer = nullptr;
 *data_pointer = nullptr;

 unsigned int val;

 val = 0; // *(unsigned short*) data;

 int size = data_len; //-2;
 int cur_pos = 0;

 LogPartition_ex_2 ** lpe2 = lp;

 TheMs * ma;

 while (cur_pos < size && lp_num > (cur_pos - 1))
 {
 unsigned int b;
 unsigned int a;

 LogPartition_ex_2 & pplp = **lpe2;

 b = val & zerox3FFF;
 a = (unsigned int)data[cur_pos++];

 if (a >= pplp.va.righe) return false;

 unsigned int lpe2vava;

 if ((lpe2vava = pplp.va[a]) == 0x80000000) return false;

 CMatriceCompressa_ex_2 & pplpvm2 = *pplp.vm[lpe2vava];
 if (a >= pplpvm2.righe) return false;

 if (b >= pplpvm2.vb.righe) return false;
 unsigned int pplpvm2vbvb;

 if ((pplpvm2vbvb = pplpvm2.vb[b]) == 0x80000000) return false;

 if (!(ma = pplpvm2.ms[a]))
 return false;

 if (ma[pplpvm2vbvb].val != b) return false;

 TheMs & ama = pplpvm2.ms[a][pplpvm2vbvb];
 ++lpe2;

 if ((val = ama.val1) & 0x40000000) //zerox4000<unsigned int>().val)
 {
 *data_pointer = (unsigned int*) &(ama.val2);

 if (ama.val2 < v_dati->righe)//->len)
 {
 if (v_dati->v_len[ama.val2] == data_len &&
 memcmp(&(*v_dati)[ama.val2][cur_pos], &data[cur_pos], data_len - cur_pos) == 0
 ) {
 *index_pointer = &(ama.val1);
 *final_value = ama.val2;
 return true;
 }
 }
 }

 if ((val & zerox3FFF) == 0)
 return false;
 }

 return false;
 }

 //Sono sicuro che non esiste. La devo solo inserire!!!

 bool mtfafs_s2_extended_v1::InsertFromHere(unsigned int val, int pos, unsigned char * data, int datalen, unsigned int *final_value)
 {
 unsigned int *index_pointer = nullptr;
 unsigned int *data_pointer = nullptr;

 int size = datalen;

 unsigned int result = 0;

 AddSpace(pos - lp_num + 1);

 //Se non esiste questa coppia, allora inserisco e mi fermo
 for (int i = pos; i < size; ++i)
 {
 unsigned int current_data = (unsigned int)data[i];
 unsigned appo = *final_value;

 result = lp[i]->Exists(val & zerox3FFF, current_data, &appo, &index_pointer, &data_pointer);

 if (result & zerox4000)
 {
 std::string precedente;
 precedente.assign((char*) (*v_dati)[*data_pointer], (char*) (*v_dati)[*data_pointer] + v_dati->v_len[*data_pointer]);

 if (precedente.size() >= i)
 {
 //azzero i valori precedenti
 (*index_pointer) &= zerox3FFF; //0011 1111 1111 1111 1111 1111 1111 1111
 (*index_pointer) |= zerox8000; //Indico che qualunque cosa ci sia, questa cosa prosegue ora!

 unsigned int old_data_pointer = *data_pointer;
 *data_pointer = 0;
 InsertFromHere(result & zerox3FFF, i + 1, (unsigned char *) precedente.data(), precedente.size(), &old_data_pointer);

 if (datalen == (i + 1))
 {
 (*index_pointer) |= zerox4000; //Si fermano qui i giochi per questa stringa.
 *data_pointer = *final_value;
 return true;
 } else {
 return InsertFromHere(result & zerox3FFF, i + 1, data, datalen, final_value);
 }
 } else {
 //la precedente terminava effettivamente qui, metto il valore a C e proseguo.
 (*index_pointer) |= zerox8000; //Ma tanto mi aspetto che questo valore esista gi!

 //E ora inserisco l'attuale!
 return InsertFromHere(result & zerox3FFF, i + 1, data, datalen, final_value);
 }
 }

 if (result == zerox8000)
 {
 result = lp[i]->Insert(val & zerox3FFF, current_data, true, *final_value);
 return true;
 } else {
 //Ma sono a fine stringa?
 if ((i + 1) >= (size - 1)) {
 (*index_pointer) |= zerox4000; //Si fermano qui i giochi per questa stringa.
 *data_pointer = *final_value;
 return true;
 }
 }

 val = result;
 }

 return true;
 }

 unsigned long long mtfafs_s2_extended_v1::Size() {
 unsigned long long tot = 0;

 //Metto lpnum
 tot += sizeof (lp_num); //lpnum
 tot += sizeof (v_dati);
 tot += v_dati->Size();

 tot += sizeof (lp);
 tot += sizeof (*lp) * lp_num;
 // printf("%d LogPartition_ex_2\n", lp_num);
 for (int i = 0; i < lp_num; ++i) {
 unsigned long long t = lp[i]->Size();
 //     printf("lp[%d]: %llu\n", i, t);
 tot += t;
 }
 printf("mtfafs_s2_extended. Needed: %llu, Total: %llu\n", tot, tot);
 return tot;
 }

 void mtfafs_s2_extended_v1::Compress() {
 for (int i = 0; i < lp_num; ++i)
 lp[i]->Compress();
 }

 bool
 mtfafs_s2_extended_v1::Insert(unsigned char * data_p, int data_len, unsigned int *final_value) {
 try {
 unsigned int tmp;

 //        //Questa parte di esistenza non mi dovrebbe interessare, la tolgo!!!
 //        unsigned int *index_pointer = nullptr;
 //        unsigned int *data_pointer = nullptr;
 //
 //        bool en_ret = Exists(data_p, data_len, &tmp, &index_pointer, &data_pointer);
 //        if (en_ret) // != en_mtfafs_NotFound)
 //        {
 //            *final_value = tmp;
 //            return false; //in ogni caso non puoi inserire!!!Già esiste questa stringa!
 //        }

 //Appendo in fondo la stringa
 *final_value = v_dati->push_back(data_p, data_len); //Il valore/posizione che assegno a questa stringa. Questo va aggiornato al termine della insert!

 unsigned int val;
 val = 0; // *(unsigned short*) data_p;

 bool bRet = InsertFromHere(val, 0, data_p, data_len, final_value);

 return bRet;

 }    catch (std::exception ex) {
 std::cout << __func__   << ex.what() << std::endl;
 return false;
 }
 }
 */

/*****************************************************************************************************************/
//La classe mtfafs_s2_extended_vint
/*
 typedef std::vector<unsigned int> T_VUINT, *T_PVUINT;

 void mtfafs_s2_extended_vint::AddSpace(int howmany)
 {
 for (int hm = 0; hm < howmany; ++hm) {
 LogPartition_ex_2 ** tmp;
 int tmp_num = lp_num + 2;
 tmp = new LogPartition_ex_2*[tmp_num];
 for (int i = 0; i < lp_num; ++i)
 tmp[i] = lp[i];

 for (int i = lp_num; i < tmp_num; ++i)
 tmp[i] = new LogPartition_ex_2();

 delete lp;
 lp = tmp;
 lp_num = tmp_num;
 }
 }


 mtfafs_s2_extended_vint::mtfafs_s2_extended_vint(int num) {
 v_dati = new CVettorePVoid();
 lp_num = num;
 lp = new LogPartition_ex_2*[lp_num];
 for (int i = 0; i < lp_num; ++i)
 lp[i] = new LogPartition_ex_2();

 unsigned int val = zerox8000;
 }

 mtfafs_s2_extended_vint::mtfafs_s2_extended_vint() {
 v_dati = new CVettorePVoid();
 //Lunghezza minima = 8
 lp_num = 8;
 lp = new LogPartition_ex_2*[lp_num];
 for (int i = 0; i < lp_num; ++i)
 lp[i] = new LogPartition_ex_2();

 unsigned int val = zerox8000;
 }

 mtfafs_s2_extended_vint::~mtfafs_s2_extended_vint() {
 v_dati->Clear();
 if (v_dati != nullptr)
 delete v_dati;
 v_dati = nullptr;
 if (lp_num > 0) {
 if (lp != NULL) {
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL)
 delete lp[i];
 }
 delete lp;
 lp = NULL;
 }
 lp_num = 0;
 }
 lp_num = 0;
 lp = NULL;
 }

 void
 mtfafs_s2_extended_vint::Clean() {
 v_dati->Clear();
 if (lp_num > 0) {
 if (lp != NULL) {
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL)
 delete lp[i];
 }
 delete lp;
 lp = NULL;
 }
 lp_num = 0;
 }

 lp_num = 0;
 lp = NULL;
 }

 void
 mtfafs_s2_extended_vint::Serialize(mtfa_binary_stream_save *bs)
 {
 //Metto lp_num
 bs->Append((unsigned int) lp_num);
 for (int i = 0; i < lp_num; ++i) {
 if (lp[i] != NULL) {
 bs->Append((unsigned char) 1);
 lp[i]->Serialize(bs);
 } else {
 bs->Append((unsigned char) 0);
 }
 }

 bs->Append(v_dati->righe);
 for (int i=0; i<v_dati->righe; ++i)
 {
 vector<unsigned int> * val = (T_PVUINT)(*v_dati)[i];
 bs->Append(val->data(), val->size());
 }
 }

 void
 mtfafs_s2_extended_vint::Deserialize(mtfa_binary_stream_load *bs) {
 Clean();
 v_dati = new CVettorePVoid();
 unsigned int val = zerox8000;

 unsigned int ll;

 lp_num = bs->Read<unsigned int>();
 lp = new LogPartition_ex_2*[lp_num];

 for (int i = 0; i < lp_num; ++i) {
 unsigned char exists = bs->Read<unsigned char>();
 if (exists == 1) {
 lp[i] = new LogPartition_ex_2();
 lp[i]->Deserialize(bs);
 } else {
 lp[i] = new LogPartition_ex_2();
 }
 }

 unsigned int righe = bs->Read<unsigned int>();
 for (int i=0; i<righe; ++i)
 {
 unsigned int len;
 unsigned int * p = bs->Read<unsigned int>(len);

 vector<unsigned int> * val = new vector<unsigned int>();
 val->assign(p, p+len);
 free(p);
 (*v_dati)[i] = val;
 }

 v_dati->righe = righe;  //per essere sicuri che sia corretto
 }

 bool mtfafs_s2_extended_vint::Exists(const unsigned int* data, int data_len, unsigned int *final_value, unsigned int ** index_pointer, unsigned int ** data_pointer)
 {
 *index_pointer = nullptr;
 *data_pointer = nullptr;

 unsigned int val;

 val = 0;

 int size = data_len; //-2;
 int cur_pos = 0;

 LogPartition_ex_2 ** lpe2 = lp;

 TheMs * ma;

 while (cur_pos < size && lp_num > (cur_pos - 1))
 {
 unsigned int b;
 unsigned int a;

 LogPartition_ex_2 & pplp = **lpe2;

 b = val & zerox3FFF;
 a = (unsigned int)data[cur_pos++];

 if (a >= pplp.va.righe) return false;

 unsigned int lpe2vava;

 if ((lpe2vava = pplp.va[a]) == 0x80000000) return false;

 CMatriceCompressa_ex_2 & pplpvm2 = *pplp.vm[lpe2vava];
 if (a >= pplpvm2.righe) return false;

 if (b >= pplpvm2.vb.righe) return false;
 unsigned int pplpvm2vbvb;

 if ((pplpvm2vbvb = pplpvm2.vb[b]) == 0x80000000) return false;

 if (!(ma = pplpvm2.ms[a]))
 return false;

 if (ma[pplpvm2vbvb].val != b) return false;

 TheMs & ama = pplpvm2.ms[a][pplpvm2vbvb];
 ++lpe2;

 if ((val = ama.val1) & 0x40000000) //zerox4000<unsigned int>().val)
 {
 *data_pointer = (unsigned int*) &(ama.val2);

 if (ama.val2 < v_dati->righe)//->len)
 {
 if ((*v_dati)[ama.val2] == data_len &&
 memcmp(&(*v_dati)[ama.val2][cur_pos], &data[cur_pos], data_len - cur_pos) == 0
 ) {
 *index_pointer = &(ama.val1);
 *final_value = ama.val2;
 return true;
 }
 }
 }

 if ((val & zerox3FFF) == 0)
 return false;
 }

 return false;
 }

 //Sono sicuro che non esiste. La devo solo inserire!!!

 bool mtfafs_s2_extended_v1::InsertFromHere(unsigned int val, int pos, unsigned char * data, int datalen, unsigned int *final_value)
 {
 unsigned int *index_pointer = nullptr;
 unsigned int *data_pointer = nullptr;

 int size = datalen;

 unsigned int result = 0;

 AddSpace(pos - lp_num + 1);

 //Se non esiste questa coppia, allora inserisco e mi fermo
 for (int i = pos; i < size; ++i)
 {
 unsigned int current_data = (unsigned int)data[i];
 unsigned appo = *final_value;

 result = lp[i]->Exists(val & zerox3FFF, current_data, &appo, &index_pointer, &data_pointer);

 if (result & zerox4000)
 {
 std::string precedente;
 precedente.assign((char*) (*v_dati)[*data_pointer], (char*) (*v_dati)[*data_pointer] + v_dati->v_len[*data_pointer]);

 if (precedente.size() >= i)
 {
 //azzero i valori precedenti
 (*index_pointer) &= zerox3FFF; //0011 1111 1111 1111 1111 1111 1111 1111
 (*index_pointer) |= zerox8000; //Indico che qualunque cosa ci sia, questa cosa prosegue ora!

 unsigned int old_data_pointer = *data_pointer;
 *data_pointer = 0;
 InsertFromHere(result & zerox3FFF, i + 1, (unsigned char *) precedente.data(), precedente.size(), &old_data_pointer);

 if (datalen == (i + 1))
 {
 (*index_pointer) |= zerox4000; //Si fermano qui i giochi per questa stringa.
 *data_pointer = *final_value;
 return true;
 } else {
 return InsertFromHere(result & zerox3FFF, i + 1, data, datalen, final_value);
 }
 } else {
 //la precedente terminava effettivamente qui, metto il valore a C e proseguo.
 (*index_pointer) |= zerox8000; //Ma tanto mi aspetto che questo valore esista gi!

 //E ora inserisco l'attuale!
 return InsertFromHere(result & zerox3FFF, i + 1, data, datalen, final_value);
 }
 }

 if (result == zerox8000)
 {
 result = lp[i]->Insert(val & zerox3FFF, current_data, true, *final_value);
 return true;
 } else {
 //Ma sono a fine stringa?
 if ((i + 1) >= (size - 1)) {
 (*index_pointer) |= zerox4000; //Si fermano qui i giochi per questa stringa.
 *data_pointer = *final_value;
 return true;
 }
 }

 val = result;
 }

 return true;
 }

 unsigned long long mtfafs_s2_extended_v1::Size() {
 unsigned long long tot = 0;

 //Metto lpnum
 tot += sizeof (lp_num); //lpnum
 tot += sizeof (v_dati);
 tot += v_dati->Size();

 tot += sizeof (lp);
 tot += sizeof (*lp) * lp_num;
 // printf("%d LogPartition_ex_2\n", lp_num);
 for (int i = 0; i < lp_num; ++i) {
 unsigned long long t = lp[i]->Size();
 //     printf("lp[%d]: %llu\n", i, t);
 tot += t;
 }
 printf("mtfafs_s2_extended. Needed: %llu, Total: %llu\n", tot, tot);
 return tot;
 }

 void mtfafs_s2_extended_v1::Compress() {
 for (int i = 0; i < lp_num; ++i)
 lp[i]->Compress();
 }

 bool
 mtfafs_s2_extended_v1::Insert(unsigned char * data_p, int data_len, unsigned int *final_value) {
 try {
 unsigned int tmp;

 //        //Questa parte di esistenza non mi dovrebbe interessare, la tolgo!!!
 //        unsigned int *index_pointer = nullptr;
 //        unsigned int *data_pointer = nullptr;
 //
 //        bool en_ret = Exists(data_p, data_len, &tmp, &index_pointer, &data_pointer);
 //        if (en_ret) // != en_mtfafs_NotFound)
 //        {
 //            *final_value = tmp;
 //            return false; //in ogni caso non puoi inserire!!!Già esiste questa stringa!
 //        }

 //Appendo in fondo la stringa
 *final_value = v_dati->push_back(data_p, data_len); //Il valore/posizione che assegno a questa stringa. Questo va aggiornato al termine della insert!

 unsigned int val;
 val = 0; //  *(unsigned short*) data_p;

 bool bRet = InsertFromHere(val, 0, data_p, data_len, final_value);

 return bRet;

 }    catch (std::exception ex) {
 std::cout << __func__   << ex.what() << std::endl;
 return false;
 }
 }
 */

/*****************************************************************************************************/

//Per coppie di short integers
//Questa  per coppie di short int o di int ma in ogni caso  per coppie di int
mtfafs_ii::mtfafs_ii()
{
    lp = new LogPartition_ex_2();
}

mtfafs_ii::~mtfafs_ii()
{
    if ( lp != NULL )
    {
        delete lp;
    }

    lp = NULL;
}

void mtfafs_ii::Clean()
{
    if ( lp != NULL )
    {
        delete lp;
    }

    lp = NULL;
}

bool mtfafs_ii::Exists ( unsigned int v1, unsigned int v2, unsigned int& val,
                         unsigned int* final_value, unsigned int** index_pointer,
                         unsigned int** data_pointer )
{
    val = lp->Exists ( v1, v2, final_value, index_pointer, data_pointer );

    if ( val & zerox4000 )   //8 significa che prosegue, 4 che si ferma!
    {
        val &= zerox3FFF; //I due bit pi alti sono occupati!
        return true; //en_mtfafs_WholeMatched;
    }

    return false;
}

bool mtfafs_ii::Insert ( unsigned int v1, unsigned int v2, unsigned int& val,
                         unsigned int* final_value )
{
    unsigned int old;
    unsigned int* index_pointer;
    unsigned int* data_pointer;

    val = lp->Exists ( v1, v2, &old, &index_pointer, &data_pointer );

    if ( val & zerox4000 )   //8 significa che prosegue, 4 che si ferma, 2 che una si ferma e l'altra prosegue!
    {
        //Gi inserito, torno il vecchio valore
        val &= zerox3FFF; //I due bit pi alti sono occupati!
        *final_value = old;
        return true; //en_mtfafs_WholeMatched;
    }

    //bool matched = true;

    val = lp->Insert ( v1, v2, true, *final_value );

    if ( val == zerox8000 )
    {
        return false;
    }
    else
    {
        val &= zerox3FFF;
    }

    return true;
}

void mtfafs_ii::Serialize ( mtfa_binary_stream_save* bs )
{
    //Non ci sono variabili locali o altro, solo la classe lp che viene anche creata come vuota.
    if ( lp != NULL )
    {
        lp->Serialize ( bs );
    }
}

void mtfafs_ii::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clean();
    lp = new LogPartition_ex_2();
    lp->Deserialize ( bs );
}

void mtfafs_ii::Size()
{
    printf (
        "Ricorda che sono tre matrici in parallelo e ognuna  di 4 bytes per elemento.\n" );

    unsigned long long tot_bytes = 0;
    unsigned long long tot_altro = 0;

    unsigned long long usate = 0;
    unsigned long long non_usate = 0;
    unsigned long long non_allocate = 0;
    {
        printf ( "\tPartizione. Matrici: %d\n", lp->num_m );
        tot_bytes += ( lp->va.Size() * 4 );
        tot_bytes += lp->va.Size() * 4;
        tot_bytes += lp->va.Size() * 4;
        //tot_bytes += lp->va.adiacenti.size()*(4+sizeof(T_SAdiacenti*));

        for ( int j = 0; j < lp->num_m; ++j )
        {
            //tot_altro += (j<lp->vm[j]->a_nodes.size()*8);
            //tot_altro += (j<lp->vm[j]->b_nodes.size()*8);
            //      tot_altro += (j < lp->vm[j]->vdegb->righe_attuali * 4);

            int righe_attive = 0;

            for ( int r = 0; r < lp->vm[j]->righe_allocate; ++r )
            {
                if ( lp->vm[j]->ms[r] != nullptr )
                {
                    //Anche la parte va...
                    tot_bytes += lp->vm[j]->colonne_allocate * 4 * 3;
                    //for (T_Adiacenti::iterator it = lp->va.adiacenti.begin(); it != lp->va.adiacenti.end(); ++it)
                    // tot_bytes += ((*it).second->dim * 4);
                    righe_attive++;

                    for ( int c = 0; c < lp->vm[j]->colonne_allocate; ++c )
                        if ( lp->vm[j]->ms[r][c].val == zerox8000 )
                        {
                            usate++;
                        }
                        else
                        {
                            non_usate++;
                        }
                }
                else
                {
                    non_allocate += lp->vm[j]->colonne_allocate;
                    tot_bytes += 12;
                }
            }

            tot_bytes += righe_attive * lp->vm[j]->colonne_allocate * 12
            + lp->vm[j]->vb.Size() * 4;
            //               printf("\t\tMatrice: %3d. Righe: %d. Items: %d. VB: %3d.\n", j, righe_attive, righe_attive*lp[i]->vm[j]->colonne, lp[i]->vm[j]->vb.righe);
        }
    }
    printf ( "TOTALE GENERALE: %lld bytes\n", tot_bytes + tot_altro );
    printf ( "DI CUI: MAT %lld, OTHER %lld\n", tot_bytes, tot_altro );
    printf ( "INDICATORI DI RIEMPIMENTO\n" );
    printf ( "\tTOTALE CELLE			: %lld\n", usate + non_usate + non_allocate );
    printf ( "\tTOTALE USATE			: %lld\n", usate );
    printf ( "\tTOTALE NON_USATE		: %lld\n", non_usate );
    printf ( "\tTOTALE NON ALLOCATE	: %lld\n", non_allocate );
    printf ( "Percentuale di compressione matrice sparsa: %lf\n",
             ( double ) ( usate + non_usate )
             / ( double ) ( usate + non_usate + non_allocate ) );
}

mtfafs_ss::mtfafs_ss()
{
    a_s1 = new mtfafs_s2_extended();
    a_s2 = new mtfafs_s2_extended();
    a_s1s2 = new mtfafs_ii();
    p_s1 = 0;
    p_s2 = 0;
}

mtfafs_ss::~mtfafs_ss()
{
    delete a_s1;
    delete a_s2;
    delete a_s1s2;
    a_s1 = nullptr;
    a_s2 = nullptr;
    a_s1s2 = nullptr;
}

void mtfafs_ss::Clean()
{
    delete a_s1;
    delete a_s2;
    delete a_s1s2;
    a_s1 = nullptr;
    a_s2 = nullptr;
    a_s1s2 = nullptr;
}

bool mtfafs_ss::Insert ( std::string v1, std::string v2,
                         unsigned int* final_value )
{
    //inserisce v1 in lp_s1
    unsigned int val_s1 = p_s1;
    bool bRet = a_s1->Insert ( ( unsigned char* ) v1.data(), v1.size(), &val_s1 );

    if ( bRet )
    {
        p_s1++;
    }

    unsigned int val_s2 = p_s2;
    bRet = a_s2->Insert ( ( unsigned char* ) v2.data(), v2.size(), &val_s2 );

    if ( bRet )
    {
        p_s2++;
    }

    //ora posso inserire la coppia!!!
    unsigned int val;
    return a_s1s2->Insert ( val_s1, val_s2, val, final_value ); //se gi presente comunque modifica final_value
}

bool mtfafs_ss::Exists ( std::string v1, std::string v2,
                         unsigned int* final_value, unsigned int** index_pointer,
                         unsigned int** data_pointer )
{
    unsigned int val_s1;
    bool bRet = a_s1->Exists ( ( const unsigned char* ) v1.data(), v1.size(),
                               &val_s1, index_pointer, data_pointer );

    if ( !bRet )
    {
        return false;
    }

    unsigned int val_s2;
    bRet = a_s2->Exists ( ( const unsigned char* ) v2.data(), v2.size(), &val_s2,
                          index_pointer, data_pointer );

    if ( !bRet )
    {
        return false;
    }

    unsigned int val;
    return a_s1s2->Exists ( val_s1, val_s2, val, final_value, index_pointer,
                            data_pointer );
}

void mtfafs_ss::Serialize ( mtfa_binary_stream_save* bs )
{
    //mtfafs_s2 *a_s1;
    //unsigned int p_s1;
    //mtfafs_s2 *a_s2;
    //unsigned int p_s2;
    //mtfafs_ii *a_s1s2;
    //Per me DEVE essere tutto != NULL!
    bs->Append ( p_s1 );
    bs->Append ( p_s2 );
    a_s1->Serialize ( bs );
    a_s2->Serialize ( bs );
    a_s1s2->Serialize ( bs );
}

void mtfafs_ss::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clean();
    p_s1 = bs->Read<unsigned int>();
    p_s2 = bs->Read<unsigned int>();

    a_s1 = new mtfafs_s2_extended();
    a_s2 = new mtfafs_s2_extended();
    a_s1s2 = new mtfafs_ii();

    a_s1->Deserialize ( bs );
    a_s2->Deserialize ( bs );
    a_s1s2->Deserialize ( bs );
}

mtfafs_string_set::mtfafs_string_set ( bool icase )
{
    ignore_case = true;
    xii = new mtfafs_s2_base();
    data_len_max = 0;
    max_data_len = 0;
    v_dati.reserve ( 32 ); //Mi aspetto che ci siano almeno 32 stringhe da cercare, a partire da un certo punto in poi...
}

mtfafs_string_set::~mtfafs_string_set()
{
    if ( xii != nullptr )
    {
        xii->Clean();
        delete xii;
    }

    xii = nullptr;

    v_dati.clear();
}

bool mtfafs_string_set::Find ( const unsigned char* data, int data_len,
                               std::list<std::pair<std::string, std::pair<int, int>>>& result )
{
    unsigned int* index;
    unsigned int* datap;
    unsigned int final;

    //Se. in base alle regole di posizionamento associate alle stringhe da cercare, la lunghezza massima attesa  inferiore a quella del buffer attuale,
    //cerco in meno spazio!
    if ( data_len > max_data_len )
    {
        data_len = max_data_len;
    }

    unsigned char* data_used = ( unsigned char* ) data;   //questi sono i dati che utilizzo

    if ( ignore_case )
    {
        data_used = new unsigned char[data_len];
        memcpy ( data_used, data, data_len );
        std::transform ( data_used, &data_used[data_len], data_used, ::tolower );
    }

    bool found = false;
    //Faccio la ricerca a partire dal primo item della stringa e poi da tutti i successivi
    int i = 0;

    while ( i < data_len )
    {
        if ( xii->Exists_substring ( &data_used[i], data_len - i, v_dati, i,
                                     &final, &index, &datap ) )
        {
            int slen = v_dati[final].first.size();
            result.push_back (
                std::make_pair ( v_dati[final].first,
                                 std::make_pair ( final, i ) ) );
            found = true;
        }

        ++i;
    }

    if ( ignore_case )
    {
        delete data_used;
    }

    return found;
}

bool mtfafs_string_set::AddToSet ( std::string val, int from, int howmany,
                                   unsigned int& final )
{
    //Archivio la stringa indicando la posizione massima alla quale me la aspetto!
    final = v_dati.size();

    if ( data_len_max < val.size() )
    {
        data_len_max = val.size();
    }

    std::string val1 = val;

    if ( ignore_case )
    {
        std::transform ( val.begin(), val.end(), val.begin(), ::tolower );
    }

    if ( max_data_len < ( from + howmany ) )
    {
        max_data_len = from + howmany;
    }

    bool bRet = xii->Insert ( ( unsigned char* ) val.data(), val.size(), &final );

    if ( bRet )
    {
        v_dati.push_back ( std::make_pair ( val1, std::make_pair ( from, howmany ) ) );
    }

    return bRet;
}

/*****************************************************************************************************************/

/********************************************LA NUOVA MATRICE COMPRESSA*******************************************/

void mtfa_base_mat::ReallocateRows ( unsigned int r )
{
    //Se non supera le righe allocate inutile estendere
    if ( r >= righe )
    {
        if ( r >= righe_allocate )
        {
            unsigned int newrighe = r + 1 + base_allocazione;
            TheMs** tmp_ms = ( TheMs** ) malloc ( sizeof ( TheMs* ) * newrighe );
            std::copy ( & ( ms[0] ), & ( ms[righe_allocate] ), tmp_ms );
            free ( ms );
            ms = tmp_ms;

            std::fill ( &ms[righe_allocate], &ms[newrighe], nullptr );
            //            for (unsigned int i = righe_allocate; i < newrighe; ++i) {
            //                ms[i] = NULL;
            //            }
            righe_allocate = newrighe;
            righe = r + 1;
            //cerr << "Ora ra: " << righe_allocate << endl;
        }

        righe = r + 1;
    }
}

void mtfa_base_mat::ReallocateCols ( unsigned int c )
{
    if ( c >= colonne )
    {
        if ( c >= colonne_allocate )
        {
            //Per ogni riga aggiungo le colonne che servono!
            unsigned int newcol = c + 1 + BASE_ALLOCAZIONE_COL; // + colonne_allocate;

            for ( unsigned int i = 0; i < righe_allocate; i++ )
            {
                //In questo caso se la riga puntava a NULL, non la rialloca poiché non ci interessa per ora
                if ( ms[i] != NULL )
                {
                    TheMs* m_i = ( TheMs* ) malloc ( sizeof ( TheMs ) * newcol );
                    std::copy ( & ( ms[i][0] ), & ( ms[i][colonne_allocate] ), m_i );
                    free ( ms[i] );
                    ms[i] = m_i;
                    std::fill ( &ms[i][colonne_allocate], &ms[i][newcol],
                                initializer ); //zerox8000);
                }
            }

            colonne_allocate = newcol; //cerr << "COLALL: " << colonne_allocate << ". COL: " << c+1 << endl;
        }

        colonne = c + 1;
    }
}

void mtfa_base_mat::Reallocate ( unsigned int r, unsigned int c )
{
    ReallocateRows ( r );
    ReallocateCols ( c );
}

mtfa_base_mat::~mtfa_base_mat()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != NULL )
        {
            free ( ms[i] );
        }
    }

    free ( ms );
    vb.Clear();
    vdegb.Clear();
    vb_adj.clear();

    ms = nullptr;
    //vb = nullptr;
    //vdegb = nullptr;
}

mtfa_base_mat::mtfa_base_mat()
{
    Init();
}

void mtfa_base_mat::Init()
{
    righe = 0;
    colonne = 0;
    ms = NULL;
    //base_allocazione = BASE_ALLOCAZIONE;
    righe_allocate = 0;
    colonne_allocate = 0;

    //vb = new  CVettoreInt();
    //vdegb = new  CVettoreInt();

    vb.Init ( zerox8000 );
    vdegb.Init ( 0 );
    vb_adj.clear();

    initializer.val = zerox8000;
    initializer.val1 = 0;

    righe_allocate = 0; //base_allocazione;
    colonne_allocate = 0; //base_allocazione;
    righe = 0;
    colonne = 0;

    simplerng = mtfa_fastrng1024 ( mtfa_rand_simple_ull() );

}

unsigned int mtfa_base_mat::Exists ( unsigned int a, unsigned int b )
{
    if ( a >= righe )
    {
        return zerox8000;
    }

    if ( b >= vb.Size() )
    {
        return zerox8000;
    }

    unsigned int vbvb = vb[b];

    if ( vbvb == zerox8000 )
    {
        return zerox8000;
    }

    TheMs* ma = ms[a];

    if ( ma == NULL )
    {
        return zerox8000;
    }
    else
    {
        if ( ma[vbvb].val == b )
        {
            return ms[a][vbvb].val1;
        }
        else
        {
            return zerox8000;
        }
    }
}

unsigned int mtfa_base_mat::FindFirstFreeOrReallocate ( unsigned int a )
{
    if ( colonne > 0 )
    {
        TheMs* msa;

        if ( ! ( msa = ms[a] ) )
        {
            return 0;
        }

        //se non sta nelle prossime 16, allora non c'è

        for ( int c = 0; c < colonne; ++c )
        {
            if ( ( msa++ )->val == zerox8000 )
            {
                return c;
            }
        }
    }

    ReallocateCols ( colonne );
    return colonne - 1;
}

unsigned int* mtfa_base_mat::EvalAdjacentB ( unsigned int b )
{
    //Calcolo gli adiacenti di b sulla colonna
    if ( vb.Size() <= b )
    {
        return nullptr;
    }

    unsigned int idxb = vb[b];

    if ( idxb == zerox8000 )
    {
        return nullptr;
    }

    auto pstmp = vb_adj.find ( b );

    if ( pstmp == vb_adj.end() )
    {
        unsigned int* result = ( unsigned int* ) malloc (
                                   1 * sizeof ( unsigned int ) );
        result[0] = 0xFFFFFFFF;
        return result;
    }
    else
    {
        int quanti = ( *pstmp ).second.size();
        unsigned int* result = ( unsigned int* ) malloc (
                                   ( quanti + 1 ) * sizeof ( unsigned int ) );
        int i = 0;

        for ( auto it : ( *pstmp ).second )
        {
            result[i++] = it;
        }

        result[quanti] = 0xFFFFFFFF;
        return result;
    }

    //    int quanti = 0;
    //    unsigned int * result = (unsigned int*) malloc((logn + 2) * sizeof (unsigned int));
    //
    //    auto it = vb_adj.lower_bound(((unsigned long long) b) << 32 | 0);
    //    while (it != vb_adj.end()) {
    //        unsigned long long v = (*it);
    //        ++it;
    //        if ((v >> 32) == b) {
    //            result[quanti++] = v & 0x00000000FFFFFFFF;
    //        } else
    //            break;
    //    }
    //    result[quanti] = 0xFFFFFFFF;
    //    return result;

    //    //Calcolo gli adiacenti di b sulla colonna
    //    if (vb.righe <= b)
    //        return nullptr;
    //    unsigned int idxb = vb[b];
    //    if (idxb == zerox8000)
    //        return nullptr;
    //
    //    int quanti = 0;
    //    unsigned int * result = (unsigned int*) malloc((logn + 2) * sizeof (unsigned int));
    //    for (int r = 0; r < righe; ++r) {
    //        TheMs * msr = ms[r];
    //        if (msr) {
    //            if ((msr + idxb)->val == b)
    //                result[quanti++] = r;
    //        }
    //    }
    //    result[quanti] = 0xFFFFFFFF;
    //    return result;
}

class local_rng
{
    public:
        typedef size_t result_type;

        static constexpr size_t min()
        {
            return 0;
        }

        static constexpr size_t max()
        {
            return 0xFFFFFFFFFFFFFFFF;
        }
        static mtfa_fastrng1024* rng;

        size_t operator() ()
        {
            // generate a random number in the range [0, 42]
            if ( !rng )
            {
                rng = new mtfa_fastrng1024 ( mtfa_rand_simple_ull() );
            }

            return rng->next();
        }
};

mtfa_fastrng1024* local_rng::rng = nullptr;

unsigned int mtfa_base_mat::FindFreeSpace ( unsigned int a, unsigned int b,
        T_it_vb_adj adiacenti )
{
    TheMs* msa = ms[a];

    if ( !msa )
    {
        return 0; //Va bene tutto!
    }

#if 1
    unsigned int vbvb = vb[b];

    //faccio un vettore per assicurarmi che le posizioni 0..colonne
    //siano visitate in modo casuale
    unsigned int* vcol = new unsigned int[colonne];
    std::iota ( vcol, vcol + colonne, 0 ); //i numeri da 0 a n-1
    std::shuffle ( vcol, vcol + colonne, local_rng() );

    //#define PARALLEL 1
#ifdef PARALLEL
    //faccio un vettore che indicizza per 16-esimi di colonne
    vector<unsigned int> vindex = {0,
                                   colonne / 16, colonne / 16 * 2, colonne / 16 * 3, colonne / 16 * 4, colonne / 16 * 5, colonne / 16 * 6, colonne / 16 * 7, colonne / 16 * 8,
                                   colonne / 16 * 9, colonne / 16 * 10, colonne / 16 * 11, colonne / 16 * 12, colonne / 16 * 13, colonne / 16 * 14, colonne / 16 * 15, colonne
                                  };
    //    vector<unsigned int> vindex = {0,
    //        colonne/32, colonne/32*2, colonne/32*3, colonne/32*4, colonne/32*5, colonne/32*6, colonne/32*7, colonne/32*8,
    //        colonne/32*9, colonne/32*10, colonne/32*11, colonne/32*12, colonne/32*13, colonne/32*14, colonne/32*15, colonne/32*16,
    //        colonne/32*17, colonne/32*18, colonne/32*19, colonne/32*20, colonne/32*21, colonne/32*22, colonne/32*23, colonne/32*24,
    //        colonne/32*25, colonne/32*26, colonne/32*27, colonne/32*28, colonne/32*29, colonne/32*30, colonne/32*31, colonne
    //    };

    //    vector<unsigned int> vindex = {0,
    //        colonne/4, colonne/4*2, colonne/4*3, colonne
    //    };
    bool bfoundcol = false;
    unsigned int ifoundcol = 0;

    #pragma omp parallel for shared(bfoundcol, ifoundcol)

    for ( int index = 0; index < 16; index++ )
    {
        #pragma omp flush(bfoundcol)

        for ( unsigned int c = vindex[index]; c < vindex[index + 1]; ++c )
#else
    for ( unsigned int c = 0; c < colonne; ++c )
#endif
        {
#ifdef PARALLEL
            //mprintf("%d-%d-%d ", index, omp_get_thread_num(), omp_get_num_threads());
            #pragma omp flush(bfoundcol)

            if ( bfoundcol )
            {
                break;
            }

#endif
            int col = vcol[c];

            if ( msa[col].val != zerox8000 )
            {
                continue;
            }

            bool found = true;

            for ( auto adj : adiacenti->second )
            {
#ifdef PARALLEL

                if ( bfoundcol )
                {
                    cout << __func__ << "2";
                    break;
                }

#endif

                TheMs* pm;

                if ( ! ( pm = ms[adj] ) )
                {
                    //ok, colonna vuota.
                    continue;
                }

                if ( pm[col].val != zerox8000 )
                {
                    found = false;
                    break; //Finisce qui questa ricerca, almeno uno già occupato
                }
            }

#ifdef PARALLEL

            if ( !bfoundcol && ( !found || msa[col].val != zerox8000 ) )
            {
                continue;     //Non l'ho trovata una colonna con tutti gli spazi liberi
            }

            ifoundcol = col;
            bfoundcol = true;
            #pragma omp flush(bfoundcol)
            break;
        }

        //cout << __func__   << GetULLTime() << " ";
    }

//cout << __func__   << endl;
#else

            if ( !found )
            {
                continue;     //Non l'ho trovata una colonna con tutti gli spazi liberi
            }

            delete [] vcol;
            return col;
        }
#endif
    delete [] vcol;

#ifdef PARALLEL
    //printf("\n");
    if ( bfoundcol )
    {
        return ifoundcol;
    }

#endif

    ReallocateCols ( colonne );
    return colonne - 1;

#else
    /*
     * devo trovare quella colonna che ha spazi vuoti allo stesso posto di questi.
     * cioè di quelli che sono in adiacenti!
     * tieni presente che posso avere max 65536 righe e centinaia di colonne
     * */

    unsigned int vbvb = vb[b];

    auto cadiacenti = adiacenti->second; //per non buttare gli adiacenti

    //ma ci sono ora adiacenti?
    auto adj = adiacenti->second.begin();

    if ( adj == adiacenti->second.end() )
    {
        for ( unsigned int c = 0; c < colonne; ++c )
        {
            //Come stiamo messi per l'arco corrente?
            if ( msa[c].val != zerox8000 )
            {
                continue;
            }

            return c;
        }

        ReallocateCols ( colonne );
        return colonne - 1;
    }

    //Se sono qui ho almeno un adiacente a b.
    //mi faccio una prima passata per vedere quali colonne sono vuote rispetto
    //alla coppia b con il primo degli adiacenti di b
    //elemento degli adiacenti e poi proseguo solo con quelli trovati vuoti

    //faccio un vettore per assicurarmi che le posizioni 0..colonne
    //siano visitate in modo casuale
    unsigned int* vcol = new unsigned int[colonne];
    std::iota ( vcol, vcol + colonne, 0 ); //i numeri da 0 a n-1
    std::shuffle ( vcol, vcol + colonne, local_rng() );

    std::list<unsigned int> elenco;

    for ( unsigned int c1 = 0; c1 < colonne; ++c1 )
    {
        int c = vcol[c1];

        if ( !ms[ ( *adj )] ||
                ( msa[c].val == zerox8000 && ms[ ( *adj )][c].val == zerox8000 ) )
        {
            elenco.push_back ( c );
            continue;
        }
    }

    delete vcol;

    //Ora ho una lista di colonne per le quali il primo degli adiacenti era vuoto
    //proseguo e intanto elimino colonne
    while ( elenco.size() > 0 )
    {
        //        auto it = elenco.begin();
        //        list<unsigned int> subelenco;
        //        for (int i = 0; i < 32 && elenco.size() > 0; ++i) {
        //            subelenco.push_back(*it);
        //            it = elenco.erase(it);
        //        }
        adj = adiacenti->second.begin();
        adj++;

        while ( subelenco.size() > 0 && adj != adiacenti->second.end() )
        {
            auto msadj = ms[ ( *adj )];

            if ( !msadj )
            {
                //per questo adiacente, tutte le colonne sono libere
                adj++;
                continue;
            }

            auto it = subelenco.begin();

            while ( it != subelenco.end() )
            {
                if ( msadj[ ( *it )].val != zerox8000 || msa[*it].val != zerox8000 )
                {
                    it = subelenco.erase ( it );
                }
                else
                {
                    it++;
                }
            }

            adj++;
        }

        if ( subelenco.size() > 0 )
        {
            return subelenco.front();
        }
    }

    //cout << __func__   << "Adiacenti: " << cadiacenti.size() << "(" << adiacenti->second.size() << "). Elenco: " << elenco.size() << ". Colonne: " << colonne << endl;
    if ( elenco.size() == 0 )
    {
        ReallocateCols ( colonne );
        return colonne - 1;
    }
    else
    {
        return elenco.front();
    }

#endif
}

unsigned int mtfa_base_mat::Insert ( unsigned int a, unsigned int b,
                                     unsigned int idx )
{
    //in ogni caso, se necessario rialloco!
    vb.Reallocate ( b );
    vdegb.Reallocate ( b );
    ReallocateRows ( a );

    //Questo nodo b è stato già assegnato?
    if ( vb[b] & zerox8000 )
    {
        //No, ancora non assegnato!
        //Cerco il primo posto libero nella sequenza associata ad a
        unsigned int firstFree = FindFirstFreeOrReallocate ( a );
        vb[b] = firstFree;

        if ( ms[a] == nullptr )
        {
            ms[a] = ( TheMs* ) malloc ( sizeof ( TheMs ) * colonne_allocate );
            std::fill ( ms[a], &ms[a][colonne_allocate], initializer );
        }

        ms[a][vb[b]].val = b;
        ms[a][vb[b]].val1 = idx;
        vdegb[b]++;
        //vb_adj.insert(((unsigned long long) b) << 32 | a);
        vb_adj[b].insert ( a );

        //vb->AddItem(a, b);
        return idx;
    }

    //OK. Il nodo b è stato precedentemente assegnato.
    //Ma non potrebbe essere che l'arco è stato già inserito?
    if ( ms[a] != nullptr )
    {
        if ( ms[a][vb[b]].val == b )
        {
            //avevo ragione.
            return ms[a][vb[b]].val1;
        }

        //ma a è libero per questa forestina?
        if ( ms[a][vb[b]].val & zerox8000 )
        {
            //!Libero!, utilizzo questo.
            ms[a][vb[b]].val = b;
            ms[a][vb[b]].val1 = idx;
            //vb->AddItem(a, b);
            vdegb[b]++;
            //vb_adj.insert(((unsigned long long) b) << 32 | a);
            vb_adj[b].insert ( a );

            return idx;
        }
    }
    else
    {
        ms[a] = ( TheMs* ) malloc ( sizeof ( TheMs ) * colonne_allocate );
        std::fill ( ms[a], &ms[a][colonne_allocate], initializer );

        if ( ms[a][vb[b]].val & zerox8000 )
        {
            //!Libero!, utilizzo questo.
            ms[a][vb[b]].val = b;
            ms[a][vb[b]].val1 = idx;
            //vb->AddItem(a, b);
            vdegb[b]++;
            //vb_adj.insert(((unsigned long long) b) << 32 | a);
            vb_adj[b].insert ( a );

            return idx;
        }
    }

    //OK, ho trovato una foresta anomala, devo ristrutturare.
    /*
     -  Per tutti gli a adiacenti di b, ivi compreso questo che sto cercando di inserire,
     cerco il primo posto libero dove spostare b
     */

    //Calcolo gli adiacenti di b
    //unsigned int * adiacenti = EvalAdjacentB(b);
    /*
     Ricordati gli invarianti:
     1) Esiste sempre un solo nodo b in un insieme di foreste
     2) In una foresta un nodo a ha SEMPRE grado 1.
     Quindi, ripeti con me, cosa stai cercando?
     Sto cercando tra tutti gli a, adiacenti di b, il primo posto libero in comune
     */
    //ora cerco uno special set (una colonna) nella quale ci sia spazio per tutti gli archi uscenti da b, compreso l'arco corrente)
    auto adiacenti = vb_adj.find ( b );
    unsigned int frspace = FindFreeSpace ( a, b, adiacenti );

    //Colonna trovata, la inserisco e elimino la vecchia!
    if ( adiacenti != vb_adj.end() )   //Questa è SEMPRE vera
    {
        //for (auto i=adiacenti->begin(); i != adiacenti->end(); ++i)
        for ( auto pos : adiacenti->second )
        {
            unsigned int vbvb = vb[b];

            ms[pos][frspace] = ms[pos][vbvb];

            ms[pos][vbvb] = initializer;
        }
    }
    else
    {
        //cerr << "Non capita mai!\n";
    }

    //Aggiungo arco a,b
    ms[a][frspace].val = b;
    ms[a][frspace].val1 = idx;
    vb[b] = frspace; //Aggiorna lo special set in cui si trova ora b
    //vb->AddItem(a, b);
    vdegb[b]++;
    //vb_adj.insert(((unsigned long long) b) << 32 | a);
    vb_adj[b].insert ( a );

    return idx;
}

bool mtfa_base_mat::ExistsB ( unsigned int b )
{
    if ( b >= vb.Size() )
    {
        return false;
    }

    return ( vb[b] != zerox8000 );
}

int mtfa_base_mat::GetDegreeB ( unsigned int b )
{
    //Dovrebbe corrispondere anche a quanto scritto in adiacenti!!!
    //Calcolo gli adiacenti di b sulla colonna
    if ( vb.Size() <= b )
    {
        return 0;
    }

    return vdegb[b];
    //    unsigned int idxb = vb[b];
    //    if (idxb == zerox8000)
    //        return 0;
    //
    //    int quanti = 0;
    //    for (int r = 0; r < righe; ++r) {
    //        TheMs * msr = ms[r];
    //        if (msr) {
    //            if ((msr + idxb)->val == b)
    //                quanti++;
    //        }
    //    }
    //    return quanti;
}

//int
//mtfa_base_mat::GetDegreeA(unsigned int a)
//{
//  int deg = 0;
//  for (unsigned int i = 0; i < colonne; i++)
//    if ((ms[a] != NULL) && (ms[a][i].val != zerox8000))
//      deg++;
//
//  return deg;
//}

std::list<unsigned int>*
mtfa_base_mat::GetAllBConnectedToThisA ( unsigned int a )
{
    std::list<unsigned int>* l = new std::list<unsigned int>();

    if ( righe > a )
    {
        for ( unsigned int i = 0; i < colonne; i++ )
        {
            TheMs* msa = ms[a];

            if ( msa && ( msa[i].val != zerox8000 ) )
            {
                l->push_back ( msa[i].val ); //.val & zerox7FFF);
            }
        }
    }

    return l;
}

void mtfa_base_mat::DeleteEdge ( unsigned int a, unsigned int b )
{
    //vb->DelItem(a, b);
    unsigned int col = vb[b];
    ms[a][col] = initializer; //zerox8000;
    vdegb[b]--;

    //vb_adj.erase(((unsigned long long) b) << 32 | a);
    vb_adj[b].erase ( a );

}

void mtfa_base_mat::Clear()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != NULL )
        {
            free ( ms[i] );
        }
    }

    free ( ms );
    vdegb.Clear();
    vb.Clear();
    vb_adj.clear();

    ms = nullptr;
    righe_allocate = 0;
    colonne_allocate = 0;
    //vdegb = nullptr;
    //vb = nullptr;
    righe = 0;
    colonne = 0;
    Init();

}

unsigned long long mtfa_base_mat::Size()
{
    //        int           base_allocazione;
    //        unsigned int  righe_allocate;
    //        unsigned int  colonne_allocate;
    //        unsigned int  righe;
    //        unsigned int  colonne;
    //        TheMs **      ms;
    //        TheMs         initializer;
    //        CVettoreInt * vb;
    //        CVettoreInt * vdegb;    // in questo modo non devo calcolare ogni volta il grado di b ma lo conosco a priori
    unsigned long long tot = 0;

    tot += sizeof ( base_allocazione );
    tot += sizeof ( righe_allocate );
    tot += sizeof ( colonne_allocate );
    tot += sizeof ( righe );
    tot += sizeof ( colonne );
    tot += sizeof ( ms );
    tot += sizeof ( initializer );
    tot += sizeof ( vb );

    //cerr << "basic variables: " << tot << endl;
    //  tot += sizeof (vdegb);

    unsigned long long row_all = sizeof ( unsigned int* ) * righe_allocate;
    cerr << "Rows allocated: " << row_all << endl;
    tot += row_all;
    unsigned long long tot_cols = 0;
    int righe_reali = 0;
    int celle_piene = 0;
    int celle_allocate = 0;

    for ( int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != nullptr )
        {
            //            printf("%3d: ", i);
            righe_reali++;
            tot_cols += sizeof ( TheMs ) * colonne_allocate;

            for ( auto j = 0; j < colonne_allocate; ++j )
            {
                if ( ms[i][j].val != zerox8000 )
                {
                    celle_piene++;
                    //                    printf("%3d ", ms[i][j].val);
                }
                else
                {
                    //                    printf("--- ");
                }

                celle_allocate++;
            }

            //            printf("\n");
        }
    }

    //    printf("\n\n");

#if 0
    //Ora stampo la lista di adiacenza
    printf ( "(define bipartito '(\n" );

    for ( int i = 0; i < righe_allocate; ++i )
    {
        if ( ms[i] != nullptr )
        {
            printf ( "(%d (", i );

            for ( auto j = 0; j < colonne_allocate; ++j )
            {
                if ( ms[i][j].val != zerox8000 )
                {
                    printf ( "%d ", ms[i][j].val );
                }
            }

            printf ( "))\n" );
        }
    }

    printf ( "))\n\n" );
#endif

    tot += tot_cols;
    unsigned long long vbsize = vb.Size();
    cerr << "vb-size: " << vbsize << endl;
    tot += vbsize;
    //  tot += vdegb->Size();

    //Pura curiosità, ci sono colonne vuote?
    //Ci sono colonne che potrei unire?
    //Spazzolo tutto per colonna in modo quadratico, solo per provare
    //    int vuote = 0;
    //    for (int col0 = 0; col0 < (colonne - 1); ++col0) {
    //        for (int col1 = col0 + 1; col1 < colonne; ++col1) {
    //            bool mergeables = true;
    //            bool vuota = true;
    //            for (int row = 0; row < righe; row++) {
    //                if (ms[row]) {
    //                    if (ms[row][col0].val != zerox8000) {
    //                        vuota = false;
    //                        if (ms[row][col1].val != zerox8000) {
    //                            mergeables = false;
    //                            break;
    //                        }
    //                    }
    //                }
    //            }
    //            if (vuota) {
    //                cout << __func__   << "Vuota: " << col0 << endl;
    //                break;
    //            } else if (mergeables) {
    //                cout << __func__   << "Merge " << col0 << ", " << col1 << endl;
    //                break;
    //            }
    //        }
    //    }

    std::cout << __func__ << "ROWS     : " << righe << "\t\tCOLS: " << colonne
              << "\t\tCELLS: " << righe* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS: "
              << colonne << "\t\tCELLS: " << righe_reali* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS_alloc: "
              << colonne_allocate << "\t\tCELLS_used: "
              << righe_reali* colonne_allocate << std::endl;
              std::cout << __func__ << "VIRT-COLS: " << vb.Size() << "\tVIRT CELLS: "
    << vb.Size()* righe << "\tREAL-CELLS: " << celle_piene << std::endl;

    //  printf("mtfa_base_mat. Needed: %llu, Total: %llu, FullMatrix: %llu\n",
    //         tot - (sizeof (unsigned int*)*righe_allocate - sizeof (unsigned int*)*righe) - (tot_cols - tot_cols / ((colonne_allocate * colonne) == 0 ? 1 : (colonne_allocate * colonne))),
    //         tot,
    //         (unsigned long long) (righe * colonne * sizeof (TheMs)));
    //printf("Space used: %llu\n", tot);

    return tot;
}

void mtfa_base_mat::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) righe );
    bs->Append ( ( unsigned int ) colonne );
    vb.Serialize ( bs );
    vdegb.Serialize ( bs );

    if ( ms != NULL )
    {
        bs->Append ( ( unsigned char ) 1 );

        for ( unsigned int i = 0; i < righe; ++i )
        {
            if ( ms[i] != NULL )
            {
                bs->Append ( ( unsigned char ) 1 );

                for ( int j = 0; j < colonne; ++j )
                {
                    bs->Append ( ms[i][j].val );
                    bs->Append ( ms[i][j].val1 );
                }
            }
            else
            {
                bs->Append ( ( unsigned char ) 0 );
            }
        }
    }
    else
    {
        bs->Append ( ( unsigned char ) 0 );
    }

    //    bs->Append((unsigned int) freeidx.size());
    //    for (auto it : freeidx) {
    //        bs->Append(it.first);
    //        bs->Append(it.second);
    //    }

}

void mtfa_base_mat::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();
    righe = bs->Read<unsigned int>();
    colonne = bs->Read<unsigned int>();
    righe_allocate = righe;
    colonne_allocate = colonne;

    vb.Deserialize ( bs );
    vdegb.Deserialize ( bs );

    unsigned char m_null = bs->Read<unsigned char>();

    if ( m_null != 0 )
    {
        ms = ( TheMs** ) malloc ( righe_allocate * sizeof ( TheMs* ) );

        for ( unsigned int i = 0; i < righe_allocate; ++i )
        {
            //m[i] è null?
            unsigned char m_i_null = bs->Read<unsigned char>();

            if ( m_i_null != 0 )
            {
                ms[i] = ( TheMs* ) malloc ( colonne_allocate * sizeof ( TheMs ) );

                for ( int j = 0; j < colonne_allocate; ++j )
                {
                    ms[i][j].val = bs->Read<unsigned int>();
                    ms[i][j].val1 = bs->Read<unsigned int>();
                }
            }
            else
            {
                ms[i] = nullptr;
            }
        }
    }
    else
    {
        ms = nullptr;
    }

    //freeidx.clear();
    //    unsigned int len = bs->Read<unsigned int>();
    //    for (int i = 0; i < len; ++i) {
    //        unsigned int key = bs->Read<unsigned int>();
    //        unsigned int val = bs->Read<unsigned int>();
    //        freeidx[key] = val;
    //    }

}

/********************************************LA MATRICE COMPRESSA DEFINITIVA*******************************************/

/*
 * Ricorda che qui abbiamo
 *  unsigned int * ms_b;
 *  unsigned int * ms_v;
 * e tutti e due sono trattati come matrici piene, quindi non ci sono rows da riallocare (queste sono sempre 256)
 * ma, da riallocare abbiamo le colonne. quelle iniziali sono pari a 0 e crescono (poco per volta).
 */

void mtfa_mat::ReallocateCols ( unsigned int c )
{
    if ( c >= colonne )
    {
        if ( c >= colonne_allocate )
        {
            //Per ogni riga aggiungo le colonne che servono!
            unsigned int newcol = c + 1 + BASE_ALLOCAZIONE_COL; // + colonne_allocate;

            for ( unsigned int i = 0; i < righe_allocate; i++ )
            {
                if ( ms_b[i] )
                {
                    ms_b[i] = ( unsigned int* ) realloc ( ms_b[i],
                                                          sizeof ( unsigned int ) * newcol );
                    std::fill ( & ( ms_b[i][colonne_allocate] ), & ( ms_b[i][newcol] ),
                                zerox8000 );

                    ms_v[i] = ( unsigned long long* ) realloc ( ms_v[i],
                              sizeof ( unsigned long long ) * newcol );
                    std::fill ( & ( ms_v[i][colonne_allocate] ), & ( ms_v[i][newcol] ),
                                zerox8000ULL ); //zerox8000);
                }
            }

            colonne_allocate = newcol; //cerr << "COLALL: " << colonne_allocate << ". COL: " << c+1 << endl;
        }

        colonne = c + 1;

        for ( int i = 0; i < a_size; ++i )
        {
            if ( ms_b[i] )
            {
                occupati[i]->resize ( colonne );
            }
        }
    }
}

mtfa_mat::~mtfa_mat()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            free ( ms_b[i] );
            free ( ms_v[i] );
        }
    }

    free ( ms_b );
    free ( ms_v );

    vb.clear();
    vdegb.clear();

    //    vb_adj.clear();

    ms_b = nullptr;
    ms_v = nullptr;

    for ( int i = 0; i < a_size; ++i )
    {
        if ( occupati[i] )
        {
            delete occupati[i];
        }
    }
}

mtfa_mat::mtfa_mat ( int _in_a_size )
{
    a_size = _in_a_size;
    Init();
}

void mtfa_mat::Init()
{
    //to be sure
    righe = 0;
    colonne = 0;
    ms_b = ( unsigned int** ) malloc ( sizeof ( unsigned int* ) * a_size );
    ms_v = ( unsigned long long** ) malloc ( sizeof ( unsigned long long* ) * a_size );

    righe_allocate = a_size;
    righe = a_size;
    colonne_allocate = 0; //BASE_ALLOCAZIONE_COL;

    for ( unsigned int i = 0; i < righe_allocate; i++ )
    {
        ms_b[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
        ms_v[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
        //        std::fill(&(ms_b[i][0]), &(ms_b[i][colonne_allocate]), zerox8000);
        //        std::fill(&(ms_v[i][0]), &(ms_v[i][colonne_allocate]), 0);
    }

    simplerng = mtfa_fastrng1024 ( mtfa_rand_simple_ull() );
    occupati.resize ( a_size, nullptr );
}

//Parto dal presupposto che a è SEMPRE <= 255

//bool mtfa_mat::Exists(unsigned int a, unsigned int b, unsigned int ** pindex)
//{
//    unsigned int vbvb;
//
//    if (b >= vb.size())
//        return zerox8000;
//
//    if ((vbvb = vb[b]) == zerox8000)
//        return zerox8000;
//
//    if (ms_b[a] && ms_b[a][vbvb] == b) {
//        return ms_v[a][vbvb];
//    }
//    else {
//        return zerox8000;
//    }
//}
//

bool mtfa_mat::Exists ( unsigned int a, unsigned int b,
                        unsigned long long** pindex )
{
    unsigned int vbvb;

    if ( b >= vb.size() )
    {
        return false;
    }

    if ( ( vbvb = vb[b] ) == zerox8000 )
    {
        return false;
    }

    if ( ms_b[a] && ms_b[a][vbvb] == b )
    {
        *pindex = &ms_v[a][vbvb];
        return true;
    }
    else
    {
        return false;
    }
}

/*Se lo facessi come lo sto pensando ora, avrei
 * che ogni colonna è rappresentata da un intero a 256 bit
 * Quando cerco una colonna libera in certe posizioni allora, per ogni colonna dovrei fare and con un intero che ha 1 solo in quello posizioni e mi dovrei fermare al primo che risulta 0
 * */


unsigned int mtfa_mat::FindFirstFreeOrReallocate ( unsigned int a )
{
    //Cerco il primo bit a 0 nel bitset occupati[a]
    int pos=-1;

    if ( ( ( pos = occupati[a]->FirstZero() ) < 0 ) || ( pos >= colonne ) )
    {
        ReallocateCols ( colonne );
        return colonne - 1;
    }
    return pos;
}

/*
 * Cerca la prima colonna, se disponibile, in cui tutti gli adiacenti di a stanno a 0
 * Cioè: occupati[i]  mi dice quali colonne di una specifica riga sono occupate oppure no.
 * Quindi al termine degli or ottengo in p l'insieme aggregato di tutte le colonne in cui c'è un adiacente di b.
 * E se lo facessi al contrario? Vediamo:
 * */
unsigned int mtfa_mat::FindFreeSpace ( unsigned int a, unsigned int b, int len_adiacenti, unsigned int* adiacenti )
{
    mtfa_bitset p;

    for ( int i = 0; i < len_adiacenti; ++i )   //->second)
    {
        p |= *occupati[*adiacenti++];
    }

    p |= *occupati[a];

    int colonnaok = p.FirstZero();

    if ( colonnaok < 0 || colonnaok >= colonne )   // || ms_range[vb[b]])
    {
        ReallocateCols ( colonne );
        return colonne - 1;
    }
    else
    {
        return colonnaok;
    }
}

unsigned long long* mtfa_mat::Insert ( unsigned int a, unsigned int b, unsigned int idx )
{
    //Insert di un valore singolo, occorre verificare che non esista
    //già come range, in tal caso va duplicata l'insert
    //in ogni caso, se necessario rialloco!
    if ( vb.size() <= b )
    {
        vb.resize ( b + BASE_ALLOCAZIONE_mtfa_mat, zerox8000 );
        vdegb.resize ( b + BASE_ALLOCAZIONE_mtfa_mat, 0 );
    }

//     if (a==1 && b == 99)
//     {
//         cout << "Sono qui" << endl;
//         b = 99;
//     }

    if ( !ms_b[a] )
    {
        //Devo allocare una riga ma nulla per ms_range poiché qui stiamo parlando di valori atomici
        if ( colonne_allocate == 0 )
        {
            ms_b[a] = ( unsigned int* ) malloc ( sizeof ( unsigned int ) * BASE_ALLOCAZIONE_COL );
            ms_v[a] = ( unsigned long long* ) malloc ( sizeof ( unsigned long long ) * BASE_ALLOCAZIONE_COL );
            std::fill ( & ( ms_b[a][0] ), & ( ms_b[a][BASE_ALLOCAZIONE_COL] ), zerox8000 );
            std::fill ( & ( ms_v[a][0] ), & ( ms_v[a][BASE_ALLOCAZIONE_COL] ), zerox8000ULL );
            occupati[a] = new mtfa_bitset();
            colonne_allocate = BASE_ALLOCAZIONE_COL;
            colonne = 0;
        }
        else
        {
            ms_b[a] = ( unsigned int* ) malloc ( sizeof ( unsigned int ) * colonne_allocate );
            ms_v[a] = ( unsigned long long* ) malloc ( sizeof ( unsigned long long ) * colonne_allocate );
            std::fill ( & ( ms_b[a][0] ), & ( ms_b[a][colonne_allocate] ), zerox8000 );
            std::fill ( & ( ms_v[a][0] ), & ( ms_v[a][colonne_allocate] ), zerox8000ULL );
            occupati[a] = new mtfa_bitset();
        }
    }

    //Questo nodo b è stato già assegnato?
    auto vb_b = vb[b];

    if ( vb_b & zerox8000 )
    {
        //No, ancora non assegnato!
        //Cerco il primo posto libero nella sequenza associata ad a
        unsigned int firstFree = FindFirstFreeOrReallocate ( a );
        vb[b] = firstFree;
        ms_b[a][firstFree] = b;
        ms_v[a][firstFree] |= idx; //va nella parte bassa dei 64 bit
        occupati[a]->set ( firstFree, true );
        vdegb[b]++;
        return &ms_v[a][firstFree];
    }

    //OK. Il nodo b è stato precedentemente assegnato.
    //Ma non potrebbe essere che l'arco è stato già inserito?
    if ( ms_b[a][vb_b] == b )
    {
        //avevo ragione.
        return &ms_v[a][vb_b];
    }

    //ma a è libero per questa forestina?
    if ( ms_b[a][vb_b] & zerox8000 )
    {
        //!Libero!, utilizzo questo.
        ms_b[a][vb_b] = b;
        ms_v[a][vb_b] |= idx; //sempre nella parte bassa. In quella alta altre info
        occupati[a]->set ( vb_b, true );
        vdegb[b]++;
        return &ms_v[a][vb_b];
    }

    //OK, ho trovato una foresta anomala, devo ristrutturare.

    /*
     Ricordati gli invarianti:
     1) Esiste sempre un solo nodo b in un insieme di foreste
     2) In una foresta un nodo a ha SEMPRE grado 1.
     Quindi, ripeti con me, cosa stai cercando?
     Sto cercando tra tutti gli a, adiacenti di b, il primo posto libero in comune
     */
    //ora cerco uno special set (una colonna) nella quale ci sia spazio per tutti gli archi uscenti da b, compreso l'arco corrente)
    //cerco tutti gli a adiacenti a b, questi so con certezza che non possono superare i log (es: 28 o 32)
    //e quindi ho una cosa del tipo: per ogni b elenco degli a che gli sono adiacenti.
    //oppure li cerco poiché sono al massimo, dato b, 256, quindi non è poi così tanto.
    //list<unsigned int> adiacenti;
    unsigned int* adiacenti = ( unsigned int* ) malloc ( a_size * sizeof ( unsigned int ) );
    int len_adiacenti = 0;
    int dim_adiacenti = a_size;

    int iconta = 0;

    for ( auto it = &ms_b[0]; it != &ms_b[a_size]; ++it )
    {
        if ( *it && ( ( *it ) [vb_b] == b ) )  //se la riga esiste e se al posto vb_b ha b come valore, metto anche questa riga nella lista degli adiacenti
        {
            if ( len_adiacenti == dim_adiacenti )
            {
                mtfa_log("Errore!!!\n");
                adiacenti = ( unsigned int* ) realloc ( adiacenti, ( dim_adiacenti + 256 ) * sizeof ( unsigned int ) );
                dim_adiacenti += 256;
            }
            //mtfa_log("Add adiacente to %d: %d\n", b, iconta);
            adiacenti[len_adiacenti++] = iconta;
        }

        iconta++;
    }
    unsigned int frspace = FindFreeSpace ( a, b, len_adiacenti, adiacenti );

    //Colonna trovata, la inserisco e elimino la vecchia!
    {
        for ( auto pos = &adiacenti[0]; pos != &adiacenti[len_adiacenti]; ++pos )
        {
            //int pos = adiacenti[i];
            unsigned int vbvb = vb[b];

            ms_b[*pos][frspace] = ms_b[*pos][vbvb];
            ms_b[*pos][vbvb] = zerox8000;
            occupati[*pos]->set ( vbvb, false );
            occupati[*pos]->set ( frspace, true );

            ms_v[*pos][frspace] = ms_v[*pos][vbvb];
            ms_v[*pos][vbvb] = zerox8000ULL;
        }
    }

    free ( adiacenti );
    //    else {
    //        cerr << "Non capita mai!\n";
    //    }
    //Aggiungo arco a,b
    ms_b[a][frspace] = b;
    ms_v[a][frspace] |= idx; //sempre nella parte bassa!
    occupati[a]->set ( frspace, true );
    vb[b] = frspace; //Aggiorna lo special set in cui si trova ora b
    vdegb[b]++;
    return &ms_v[a][frspace];
}

bool mtfa_mat::ExistsB ( unsigned int b )
{
    if ( b >= vb.size() )
    {
        return false;
    }

    return ( vb[b] != zerox8000 );
}

int mtfa_mat::GetDegreeB ( unsigned int b )
{
    if ( vb.size() <= b )
    {
        return 0;
    }

    return vdegb[b];
}

unsigned int* mtfa_mat::GetAllBConnectedToThisA ( unsigned int a, int& len )
{
    unsigned int* uipret = ( unsigned int* ) malloc (colonne * sizeof ( unsigned int ) );
    len = 0;
    unsigned int* msa;

    if ( unsigned int* msa = ms_b[a] )
    {
        for ( unsigned int i = 0; i < colonne; i++ )
        {
            if ( msa[i] != zerox8000 )
            {
                uipret[len++] = msa[i]; //.val & zerox7FFF);
            }
        }
    }

    return uipret;
}

set<int> mtfa_mat::GetAllBConnectedToThisA ( unsigned int a )
{
    set<int> si;
    unsigned int* msa;

    if ( unsigned int* msa = ms_b[a] )
    {
        for ( unsigned int i = 0; i < colonne; i++ )
        {
            if ( msa[i] != zerox8000 )
            {
                si.insert(msa[i]);
            }
        }
    }

    return si;
}

void mtfa_mat::DeleteEdge ( unsigned int a, unsigned int b )
{
    unsigned int col = vb[b];
    ms_b[a][col] = zerox8000;
    ms_v[a][col] = zerox8000ULL;
    vdegb[b]--;
    occupati[a]->set ( col, false );
}

void mtfa_mat::Clear()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            free ( ms_b[i] );
            free ( ms_v[i] );
        }
    }

    free ( ms_b );
    free ( ms_v );
    //    vb.Clear();
    //    vdegb.Clear();
    vb.clear();
    vdegb.clear();

    //    vb_adj.clear();

    ms_b = nullptr;
    ms_v = nullptr;

    for ( int i = 0; i < a_size; ++i )
    {
        if ( occupati[i] )
        {
            delete occupati[i];
        }
    }

    Init();
}

unsigned long long mtfa_mat::Size()
{
    unsigned long long tot = 0;

    //    tot += sizeof (base_allocazione);
    tot += sizeof ( righe_allocate );
    tot += sizeof ( colonne_allocate );
    tot += sizeof ( righe );
    tot += sizeof ( colonne );
    tot += sizeof ( ms_b );
    tot += sizeof ( ms_v );
    tot += sizeof ( vb );

    unsigned long long row_all = sizeof ( unsigned int* ) * righe_allocate;
    cerr << "Rows allocated: " << row_all << endl;
    tot += row_all;
    unsigned long long tot_cols = 0;
    int righe_reali = 0;
    int celle_piene = 0;
    int celle_allocate = 0;

    for ( int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            righe_reali++;
            tot_cols += sizeof ( unsigned int ) * colonne_allocate; //ms_b
            tot_cols += sizeof ( unsigned int ) * colonne_allocate; //ms_v

            for ( auto j = 0; j < colonne_allocate; ++j )
            {
                if ( ms_b[i][j] != zerox8000 )
                {
                    celle_piene++;
                }
                else
                {
                }

                celle_allocate++;
            }
        }
    }

    tot += tot_cols;
    //unsigned long long vbsize = vb.Size();
    unsigned long long vbsize = vb.size();
    cerr << "vb-size: " << vbsize << endl;
    tot += vbsize;

    std::cout << __func__ << "ROWS     : " << righe << "\t\tCOLS: " << colonne
              << "\t\tCELLS: " << righe* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS: "
              << colonne << "\t\tCELLS: " << righe_reali* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS_alloc: "
              << colonne_allocate << "\t\tCELLS_used: "
              << righe_reali* colonne_allocate << std::endl;
    //std::cout << __func__  << "VIRT-COLS: " << vb.righe << "\tVIRT CELLS: " << vb.righe * righe << "\tREAL-CELLS: " << celle_piene << std::endl;
    std::cout << __func__ << "VIRT-COLS: " << vb.size() << "\tVIRT CELLS: "
              << vb.size() * righe << "\tREAL-CELLS: " << celle_piene
              << std::endl;

    return tot;
}

void mtfa_mat::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) a_size );

    bs->Append ( ( unsigned int ) righe );
    bs->Append ( ( unsigned int ) colonne );

    bs->Append ( ( unsigned int ) vb.size() );

    if ( vb.size() > 0 )
    {
        bs->Append ( vb.data(), vb.size() );
        bs->Append ( vdegb.data(), vdegb.size() );
    }

    for ( int i = 0; i < a_size; ++i )
    {
        if ( ms_b[i] )
        {
            bs->Append ( ( unsigned char ) 1 );
            bs->Append ( ms_b[i], colonne );
            bs->Append ( ms_v[i], colonne );

            occupati[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }
    }
}

void mtfa_mat::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();
    a_size = bs->Read<unsigned int>();

    righe = bs->Read<unsigned int>();
    colonne = bs->Read<unsigned int>();
    righe_allocate = righe;
    colonne_allocate = colonne;

    unsigned int vb_size = bs->Read<unsigned int>();

    if ( vb_size > 0 )
    {
        unsigned int* tmp_vb = bs->Read<unsigned int> ( vb_size );
        unsigned int* tmp_vdegb = bs->Read<unsigned int> ( vb_size );
        vb.assign ( tmp_vb, &tmp_vb[vb_size] );
        vdegb.assign ( tmp_vdegb, &tmp_vdegb[vb_size] );
        free ( tmp_vb );
        free ( tmp_vdegb );
    }
    else
    {
        vb.resize ( 0 );
        vdegb.resize ( 0 );
    }

    for ( int i = 0; i < a_size; ++i )
    {
        unsigned char p = bs->Read<unsigned char>();

        if ( p == 0 )
        {
            //non ci sono colonne
        }
        else
        {
            ms_b[i] = bs->Read<unsigned int> ( colonne );
            ms_v[i] = bs->Read<unsigned long long> ( colonne );
            occupati[i] = new mtfa_bitset();
            occupati[i]->Deserialize ( bs );
        }
    }
}

/*******************************************************LOG MAT***************************************************/
//unsigned int log_mat::Exists(unsigned int a, unsigned int b)
//{
//    //    if (a >= vna.size()) {
//    //        return zerox8000;
//    //    }
//
//    unsigned int vna_a = vna[a];
//    if (vna_a == zerox8000)
//        return zerox8000;
//
//    return vm[vna_a]->Exists(a, b);
//}
bool log_mat::Exists ( unsigned int a, unsigned int b,
                       unsigned long long** pindex )
{
    //    if (a >= vna.size()) {
    //        return zerox8000;
    //    }

    unsigned int vna_a = vna[a];

    if ( vna_a == zerox8000 )
    {
        return false;
    }

    return vm[vna_a]->Exists ( a, b, pindex );
}

//bool log_mat::ExistsA(unsigned char a) {
//
//    if (a >= vna.size())
//        return false;
//
//    return vna[a] != zerox8000;
//}

//std::list<unsigned char> *
//log_mat::GetAllAConnectedToThisB(unsigned int b) {
//    std::list<unsigned char> *l = new std::list<unsigned char> ();
//
//    //Il problema è che questo b è condiviso tra tutte le partizioni di quest'insieme.
//    for (int p = 0; p < num_m; ++p) {
//        //if (b < vm[p]->vb.righe) {
//        if (b < vm[p]->vb.size()) {
//            int col = vm[p]->vb[b];
//            if (col != zerox8000) {
//                for (unsigned int r = 0; r < vm[p]->righe; ++r) {
//                    if (vm[p]->ms_b[r][col] == b) {
//                        l->push_back(r);
//                    }
//                }
//            }
//        }
//    }
//    return l;
//}

//void log_mat::DeleteEdge(unsigned char a, unsigned int b) {
//    //Dalla partizione a elimino l'arco a,b
//    vm[vna[a]]->DeleteEdge(a, b);
//}

unsigned long long* log_mat::Insert ( unsigned int a, unsigned int b,
                                      unsigned int idx )
{
    unsigned long long* vret;  // = nullptr;

    //Quanti nodi a abbiamo già in questa matrice?
    unsigned int partizione;

    if ( ( partizione = vna[a] ) == zerox8000 )
    {
        vna[a] = 0; //parto da qui!!
        partizione = 0;
    }

    //Ma esiste il nodo b in questa partizione?
    if ( !vm[partizione]->ExistsB ( b ) )
    {
        return vm[partizione]->Insert ( a, b, idx ); //ho inserito una nuova coppia!
    }

    //Il nodo b esiste in questa partizione ne valuto il grado
    int deg = vm[partizione]->GetDegreeB ( b );

    if ( deg <= logn )
    {
        //Lo posso inserire senza problemi!
        unsigned long long* ret = vm[partizione]->Insert ( a, b, idx );
        return ret;
    }

    //Questo arco a,b porta il grado di b sopra il log, devo spostare tutti i b collegati con questo a

    // Costruiamo l'elenco degli elementi b collegati con questo a che mi porto da un'altra parte
    set<int> si = vm[partizione]->GetAllBConnectedToThisA ( a );

//     int listb_len;
//     unsigned int* list_b = vm[partizione]->GetAllBConnectedToThisA ( a, listb_len );

    // Ma è meglio andare sempre avanti oppure ricominciare a cercare dall'inizio?
    // Ricominciamo a cercare dalla fine
    for ( int i = ( num_m - 1 ); i >= 0; --i )
    {
        if ( i == partizione )
        {
            continue;
        }

        // Non è un problema di spazio, è un problema di grado.
        // Tutte queste coppie a,b che sto inserendo, aumentano o no il grado di un b qui dentro?
        // Tieni presente che essendo un unico a, non ci possono essere due b uguali altrimenti starei inserendo la stessa coppia.
        // Quindi per ognuno dei b, uno diverso dall'altro, verifico se incrementa il grado dei b esistenti fino alla fine
        bool must_continue = false;

        for ( auto it: si )
        {
            if ( logn <= ( 1 + vm[i]->GetDegreeB ( it ) ) )
            {
                must_continue = true;
                break;
            }
        }

        if ( must_continue )
        {
            continue;
        }

        //Se sono qui, allora posso inserire tutte le coppiette a,b in questa partizione!
        vna[a] = i; //a appartiene a questa partizione!

        auto vm_partizione_ms_v_a = vm[partizione]->ms_v[a];
        auto vm_partizione_vb = vm[partizione]->vb;

        for ( auto it: si )
        {
            unsigned int v = it;
            unsigned long long appo = vm_partizione_ms_v_a[vm_partizione_vb[v]];
            unsigned long long * ptr = vm[i]->Insert ( a, v, appo );
            *ptr = appo;
            //Lo elimino dalla precedente
            vm[partizione]->DeleteEdge ( a, v );
        }

        //Inserisco la coppia corrente
        vret = vm[i]->Insert ( a, b, idx );
        //free ( list_b );
        return vret;

//fine_ciclo:
//        ;
    }

    //se sono qui non ho trovato nemmeno una partizione ammissibile, devo crearne una nuova
    num_m++;

    mtfa_mat** vmtmp = new mtfa_mat*[num_m];

    for ( int i = 0; i < ( num_m - 1 ); ++i )
    {
        vmtmp[i] = vm[i];
    }

    delete vm;

    vm = vmtmp; //Assegno la nuova struttura ampliata
    vm[num_m - 1] = new mtfa_mat ( a_size );
    vna[a] = num_m - 1;

    for ( auto it: si) //int it = 0; it < listb_len; ++it )
    {
        unsigned int v = it; //list_b[it];
        unsigned int appo = vm[partizione]->ms_v[a][vm[partizione]->vb[v]];
        unsigned long long * ptr = vm[num_m - 1]->Insert ( a, v, appo );
        *ptr=appo;

        //Lo elimino dalla precedente
        vm[partizione]->DeleteEdge ( a, v );
    }

    //Inserisco la corrente
    vret = vm[num_m - 1]->Insert ( a, b, idx );

    //free ( list_b );
    return vret;
}

log_mat::log_mat ( int in_a_size )
{
    a_size = in_a_size;
    Init();
}

void log_mat::Init()
{
    vm = 0;
    num_m = 0;

    vna.resize ( a_size, zerox8000 );

    vm = new mtfa_mat*[1];
    vm[0] = new mtfa_mat ( a_size );
    num_m = 1;
}

log_mat::~log_mat()
{
    //Dovrei fare la delete di m...
    for ( int i = 0; i < num_m; i++ )
        if ( vm[i] )
        {
            delete vm[i];
        }

    delete[] vm;

    vm = nullptr;
    num_m = 0;
}

void log_mat::Clear()
{
    vna.clear();

    if ( vm != nullptr )
    {
        for ( int i = 0; i < num_m; ++i )
        {
            vm[i]->Clear();
            delete vm[i];
        }

        delete vm;
    }

    num_m = 0;
    vm = nullptr;

    Init();
}

unsigned long long log_mat::Size()
{
    //        // L'insieme delle matrici compresse
    //        mtfa_base_mat ** vm;
    //        int num_m;
    //        // Il vettore puntatore a ogni matrice
    //        CVettoreInt va;
    unsigned long long tot = 0;
    tot += sizeof ( num_m );
    tot += vna.size();
    tot += sizeof ( vm );

    cout << __func__ << "Num of logmat: " << num_m << endl;

    for ( int i = 0; i < num_m; ++i )
    {
        cerr << "\nMAT-" << i << " start" << endl;
        unsigned long long v1 = sizeof ( vm[i] );
        unsigned long long v2 = vm[i]->Size();
        cerr << "MAT-" << i << " total size: " << v1 + v2 << endl;
        tot += v1;
        tot += v2;
    }

    //printf("log_mat. Total: %llu\n", tot);

    return tot;
}

void log_mat::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) a_size );
    bs->Append ( ( unsigned int ) num_m );

    for ( int i = 0; i < num_m; ++i )
    {
        if ( vm[i] != NULL )
        {
            bs->Append ( ( unsigned char ) 1 );
            vm[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }

    }

    bs->Append ( vna.data(), a_size );
}

void log_mat::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();

    //La clear chiama init che crea una vm
    for ( int i = 0; i < num_m; ++i )
    {
        delete vm[i];
    }

    delete[] vm;
    vm = 0;

    a_size = bs->Read<unsigned int>();

    num_m = bs->Read<unsigned int>();
    vm = new mtfa_mat*[num_m];

    for ( int i = 0; i < num_m; ++i )
    {
        unsigned char exists = bs->Read<unsigned char>();

        if ( exists == 1 )
        {
            vm[i] = new mtfa_mat ( a_size );
            vm[i]->Deserialize ( bs );
        }
        else
        {
            vm[i] = new mtfa_mat ( a_size );
        }

    }

    unsigned int vna_dim = a_size;
    unsigned int* p = bs->Read<unsigned int> ( vna_dim );
    vna.assign ( p, &p[a_size] );
    free ( p );
}

/*****************************************************************************************************************/
//La classe mtfads_pair
mtfads_pair::mtfads_pair ( int _a_size )
{
    a_size = _a_size;
    lmat = new log_mat ( a_size );
    //    indice_archi_inseriti = 1; //Si parte sempre da 1
}

mtfads_pair::~mtfads_pair()
{
    if ( lmat )
    {
        delete lmat;
    }

    lmat = nullptr;
}

void mtfads_pair::Clean()
{
    lmat->Clear();
}

//int mtfads_pair::Exists(const unsigned int v1, const unsigned int v2)
//{
//    unsigned int idx = lmat->Exists(v1, v2);
//    if (idx == zerox8000) {
//        return -1;
//    }
//    else
//        return idx;
//}

bool mtfads_pair::Exists ( const unsigned int v1, const unsigned int v2,
                           unsigned long long** pindex )
{
    return lmat->Exists ( v1, v2, pindex );
}

//int mtfads_pair::Insert(const unsigned int v1, const unsigned int v2)
//{
//    int idx = lmat->Insert(v1, v2, indice_archi_inseriti);
//
//    //L'ho inserito oppure era già presente?
//    if (idx == indice_archi_inseriti)
//        ++indice_archi_inseriti;
//
//    return idx;
//}

unsigned long long* mtfads_pair::Insert ( const unsigned int v1,
        const unsigned int v2, unsigned int index )
{
    unsigned long long* ret = lmat->Insert ( v1, v2, index );
    return ret;
}

std::list<std::tuple<unsigned int, unsigned int, unsigned int>> mtfads_pair::GetAll()
{
    std::list<std::tuple<unsigned int, unsigned int, unsigned int>> all;

    if ( !lmat )
    {
        return all;
    }

    std::set<unsigned int> righe;

    for ( int r = 0; r < lmat->vna.size(); ++r )
    {
        righe.insert ( lmat->vna[r] );
    }

    for ( auto val : righe )
    {
        //unsigned int val = lmat->vna[r];
        if ( val != zerox8000 )
        {
            //Questa matrice esiste.
            for ( unsigned int a = 0; a < lmat->vm[val]->righe; ++a )
            {
                for ( unsigned int c = 0; c < lmat->vm[val]->colonne; ++c )
                {
                    if ( lmat->vm[val]->ms_b[a][c] != zerox8000 )
                        all.push_back (
                            std::make_tuple ( a, lmat->vm[val]->ms_b[a][c],
                                              lmat->vm[val]->ms_v[a][c] ) );
                }
            }
        }
    }

    return all;
}

void mtfads_pair::Serialize ( mtfa_binary_stream_save* bs )
{
    //    bs->Append(indice_archi_inseriti);
    lmat->Serialize ( bs );
}

void mtfads_pair::Deserialize ( mtfa_binary_stream_load* bs )
{
    //    indice_archi_inseriti = bs->Read<unsigned int>();
    lmat->Deserialize ( bs );
}


/********************************************LA MATRICE COMPRESSA STAR DEFINITIVA*******************************************/
void mtfa_mat_star::ReallocateCols ( unsigned int c )
{
    if ( c >= colonne )
    {
        if ( c >= colonne_allocate )
        {
            //Per ogni riga aggiungo le colonne che servono!
            unsigned int newcol = c + 1 + BASE_ALLOCAZIONE_COL; // + colonne_allocate;

            for ( unsigned int i = 0; i < righe_allocate; i++ )
            {
                if ( ms_b[i] )
                {
                    ms_b[i] = ( unsigned int* ) realloc ( ms_b[i],
                                                          sizeof ( unsigned int ) * newcol );
                    std::fill ( & ( ms_b[i][colonne_allocate] ), & ( ms_b[i][newcol] ),
                                zerox8000 );

                    ms_v[i] = ( unsigned long long* ) realloc ( ms_v[i],
                              sizeof ( unsigned long long ) * newcol );
                    std::fill ( & ( ms_v[i][colonne_allocate] ), & ( ms_v[i][newcol] ),
                                zerox8000ULL ); //zerox8000);
                }
            }

            colonne_allocate = newcol; //cerr << "COLALL: " << colonne_allocate << ". COL: " << c+1 << endl;
        }

        colonne = c + 1;

        for ( int i = 0; i < a_size; ++i )
        {
            if ( ms_b[i] )
            {
                occupati[i]->resize ( colonne );
            }
        }
    }
}

mtfa_mat_star::~mtfa_mat_star()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            free ( ms_b[i] );
            free ( ms_v[i] );
        }
    }

    free ( ms_b );
    free ( ms_v );

    vb.clear();
    vdegb.clear();

    ms_b = nullptr;
    ms_v = nullptr;

    for ( int i = 0; i < a_size; ++i )
    {
        if ( occupati[i] )
        {
            delete occupati[i];
        }
    }
}

mtfa_mat_star::mtfa_mat_star ( int _in_a_size )
{
    a_size = _in_a_size;
    Init();
}

void mtfa_mat_star::Init()
{
    //to be sure
    righe = 0;
    colonne = 0;
    ms_b = ( unsigned int** ) malloc ( sizeof ( unsigned int* ) * a_size );
    ms_v = ( unsigned long long** ) malloc ( sizeof ( unsigned long long* ) * a_size );

    righe_allocate = a_size;
    righe = a_size;
    colonne_allocate = 0; //BASE_ALLOCAZIONE_COL;

    for ( unsigned int i = 0; i < righe_allocate; i++ )
    {
        ms_b[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
        ms_v[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
    }

    simplerng = mtfa_fastrng1024 ( mtfa_rand_simple_ull() );
    occupati.resize ( a_size, nullptr );
}

bool mtfa_mat_star::Exists ( unsigned int a, unsigned int b,
                             unsigned long long** pindex )
{
    unsigned int vbvb;

    if ( b >= vb.size() )
    {
        return false;
    }

    if ( ( vbvb = vb[b] ) == zerox8000 )
    {
        return false;
    }

    if ( ms_b[a] && ms_b[a][vbvb] == b )
    {
        *pindex = &ms_v[a][vbvb];
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int mtfa_mat_star::FindFirstFreeOrReallocate ( unsigned int a )
{
    //questa va fatta anche per ms_range ma è un'altra funzione!
    //Cerco il primo bit a 0 nel bitset occupati[a]
    int pos; // = occupati[a]->FirstZero();

    if ( ( ( pos = occupati[a]->FirstZero() ) < 0 ) || ( pos >= colonne ) )
    {
        ReallocateCols ( colonne );
        return colonne - 1;
    }

    return pos;
}

unsigned int mtfa_mat_star::FindFreeSpace ( unsigned int a, unsigned int b,
        int len_adiacenti, unsigned int* adiacenti )
{
    int eval_col;
    mtfa_bitset p;

    for ( int i = 0; i < len_adiacenti; ++i )   //->second)
    {
        p |= *occupati[*adiacenti++];
    }

    p |= *occupati[a];
    //std::cout << __func__   << p.toString() << endl;
    int colonnaok = p.FirstZero();

    if ( colonnaok < 0 || colonnaok >= colonne )   // || ms_range[vb[b]])
    {
        ReallocateCols ( colonne );
        return colonne - 1;
    }
    else
    {
        return colonnaok;
    }
}

unsigned long long* mtfa_mat_star::Insert ( unsigned int a, unsigned int b, unsigned int idx )
{
    //Insert di un valore singolo, occorre verificare che non esista
    //già come range, in tal caso va duplicata l'insert
    //in ogni caso, se necessario rialloco!
    if ( vb.size() <= b )
    {
        vb.resize ( b + BASE_ALLOCAZIONE_mtfa_mat, zerox8000 );
        vdegb.resize ( b + BASE_ALLOCAZIONE_mtfa_mat, 0 );
    }

    if ( !ms_b[a] )
    {
        //Devo allocare una riga ma nulla per ms_range poiché qui stiamo parlando di valori atomici
        if ( colonne_allocate == 0 )
        {
            ms_b[a] = ( unsigned int* ) malloc ( sizeof ( unsigned int ) * BASE_ALLOCAZIONE_COL );
            ms_v[a] = ( unsigned long long* ) malloc ( sizeof ( unsigned long long ) * BASE_ALLOCAZIONE_COL );
            std::fill ( & ( ms_b[a][0] ), & ( ms_b[a][BASE_ALLOCAZIONE_COL] ), zerox8000 );
            std::fill ( & ( ms_v[a][0] ), & ( ms_v[a][BASE_ALLOCAZIONE_COL] ), zerox8000ULL );
            occupati[a] = new mtfa_bitset();
            colonne_allocate = BASE_ALLOCAZIONE_COL;
            colonne = 0;
        }
        else
        {
            ms_b[a] = ( unsigned int* ) malloc ( sizeof ( unsigned int ) * colonne_allocate );
            ms_v[a] = ( unsigned long long* ) malloc ( sizeof ( unsigned long long ) * colonne_allocate );
            std::fill ( & ( ms_b[a][0] ), & ( ms_b[a][colonne_allocate] ), zerox8000 );
            std::fill ( & ( ms_v[a][0] ), & ( ms_v[a][colonne_allocate] ), zerox8000ULL );
            occupati[a] = new mtfa_bitset();
        }
    }

    //Questo nodo b è stato già assegnato?
    auto vb_b = vb[b];

    if ( vb_b & zerox8000 )
    {
        //No, ancora non assegnato!
        //Cerco il primo posto libero nella sequenza associata ad a
        unsigned int firstFree = FindFirstFreeOrReallocate ( a );
        vb[b] = firstFree;
        ms_b[a][firstFree] = b;
        ms_v[a][firstFree] = idx; //va nella parte bassa dei 64 bit
        occupati[a]->set ( firstFree, true );
        vdegb[b]++;

        return &ms_v[a][firstFree];
    }

    //OK. Il nodo b è stato precedentemente assegnato.
    //Ma non potrebbe essere che l'arco è stato già inserito?
    if ( ms_b[a][vb_b] == b )
    {
        //avevo ragione.
        return &ms_v[a][vb_b];
    }

    //Ma a è libero per questa forestina?
    if ( ms_b[a][vb_b] & zerox8000 )
    {
        //!Libero!, utilizzo questo.
        ms_b[a][vb_b] = b;
        ms_v[a][vb_b] = idx; //sempre nella parte bassa. In quella alta altre info
        occupati[a]->set ( vb_b, true );
        vdegb[b]++;
        return &ms_v[a][vb_b];
    }

    unsigned int* adiacenti = ( unsigned int* ) malloc ( a_size * sizeof ( unsigned int ) );
    int len_adiacenti = 0;
    int dim_adiacenti = a_size;

    int iconta = 0;

    for ( auto it = &ms_b[0]; it != &ms_b[a_size]; ++it )
    {
        if ( *it && ( ( *it ) [vb_b] == b ) )
        {
            if ( len_adiacenti == dim_adiacenti )
            {
                adiacenti = ( unsigned int* ) realloc ( adiacenti, ( dim_adiacenti + 256 ) * sizeof ( unsigned int ) );
                dim_adiacenti += 256;
            }

            adiacenti[len_adiacenti++] = iconta;
        }

        iconta++;
    }

    unsigned int frspace = FindFreeSpace ( a, b, len_adiacenti, adiacenti );

    //Colonna trovata, la inserisco e elimino la vecchia!
    //   if (adiacenti != vb_adj.end()) //Questa è SEMPRE vera
    {
        //for (auto i=adiacenti->begin(); i != adiacenti->end(); ++i)
        //for (int i=0; i<len_adiacenti; ++i) // pos : adiacenti) //->second)
        for ( auto pos = &adiacenti[0]; pos != &adiacenti[len_adiacenti];
                ++pos )
        {
            //int pos = adiacenti[i];
            unsigned int vbvb = vb[b];

            ms_b[*pos][frspace] = ms_b[*pos][vbvb];
            ms_b[*pos][vbvb] = zerox8000;
            occupati[*pos]->set ( vbvb, false );
            occupati[*pos]->set ( frspace, true );
            ms_v[*pos][frspace] = ms_v[*pos][vbvb];
            ms_v[*pos][vbvb] = zerox8000ULL;
        }
    }

    free ( adiacenti );
    //    else {
    //        cerr << "Non capita mai!\n";
    //    }
    //Aggiungo arco a,b
    ms_b[a][frspace] = b;
    ms_v[a][frspace] = idx; //sempre nella parte bassa!
    occupati[a]->set ( frspace, true );
    vb[b] = frspace; //Aggiorna lo special set in cui si trova ora b
    vdegb[b]++;
    return &ms_v[a][frspace];
}

bool mtfa_mat_star::ExistsB ( unsigned int b )
{
    //if (b >= vb.righe)
    if ( b >= vb.size() )
    {
        return false;
    }

    return ( vb[b] != zerox8000 );
}

int mtfa_mat_star::GetDegreeB ( unsigned int b )
{
    //if (vb.righe <= b)
    if ( vb.size() <= b )
    {
        return 0;
    }

    return vdegb[b];
}

unsigned int* mtfa_mat_star::GetAllBConnectedToThisA ( unsigned int a,
        int& len )
{
    unsigned int* uipret = ( unsigned int* ) malloc (
                               colonne * sizeof ( unsigned int ) );
    len = 0;
    unsigned int* msa;

    if ( unsigned int* msa = ms_b[a] )
    {
        for ( unsigned int i = 0; i < colonne; i++ )
        {
            if ( msa[i] != zerox8000 )
            {
                uipret[len++] = msa[i]; //.val & zerox7FFF);
            }
        }
    }

    return uipret;
}

void mtfa_mat_star::DeleteEdge ( unsigned int a, unsigned int b )
{
    unsigned int col = vb[b];
    ms_b[a][col] = zerox8000;
    ms_v[a][col] = zerox8000ULL;
    vdegb[b]--;
    occupati[a]->set ( col, false );
}

void mtfa_mat_star::Clear()
{
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            free ( ms_b[i] );
            free ( ms_v[i] );
        }
    }

    free ( ms_b );
    free ( ms_v );
    vb.clear();
    vdegb.clear();
    ms_b = nullptr;
    ms_v = nullptr;

    for ( int i = 0; i < a_size; ++i )
    {
        if ( occupati[i] )
        {
            delete occupati[i];
        }
    }

    Init();
}

unsigned long long mtfa_mat_star::Size()
{
    unsigned long long tot = 0;

    tot += sizeof ( righe_allocate );
    tot += sizeof ( colonne_allocate );
    tot += sizeof ( righe );
    tot += sizeof ( colonne );
    tot += sizeof ( ms_b );
    tot += sizeof ( ms_v );
    tot += sizeof ( vb );

    unsigned long long row_all = sizeof ( unsigned int* ) * righe_allocate;
    cerr << "Rows allocated: " << row_all << endl;
    tot += row_all;
    unsigned long long tot_cols = 0;
    int righe_reali = 0;
    int celle_piene = 0;
    int celle_allocate = 0;

    for ( int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            righe_reali++;
            tot_cols += sizeof ( unsigned int ) * colonne_allocate; //ms_b
            tot_cols += sizeof ( unsigned int ) * colonne_allocate; //ms_v

            for ( auto j = 0; j < colonne_allocate; ++j )
            {
                if ( ms_b[i][j] != zerox8000 )
                {
                    celle_piene++;
                }
                else
                {
                }

                celle_allocate++;
            }
        }
    }

    tot += tot_cols;
    //unsigned long long vbsize = vb.Size();
    unsigned long long vbsize = vb.size();
    cerr << "vb-size: " << vbsize << endl;
    tot += vbsize;

    std::cout << __func__ << "ROWS     : " << righe << "\t\tCOLS: " << colonne
              << "\t\tCELLS: " << righe* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS: "
              << colonne << "\t\tCELLS: " << righe_reali* colonne << std::endl;
    std::cout << __func__ << "REAL ROWS: " << righe_reali << "\t\tCOLS_alloc: "
              << colonne_allocate << "\t\tCELLS_used: "
              << righe_reali* colonne_allocate << std::endl;
    //std::cout << __func__  << "VIRT-COLS: " << vb.righe << "\tVIRT CELLS: " << vb.righe * righe << "\tREAL-CELLS: " << celle_piene << std::endl;
    std::cout << __func__ << "VIRT-COLS: " << vb.size() << "\tVIRT CELLS: "
              << vb.size() * righe << "\tREAL-CELLS: " << celle_piene
              << std::endl;

    return tot;
}

void mtfa_mat_star::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) a_size );

    bs->Append ( ( unsigned int ) righe );
    bs->Append ( ( unsigned int ) colonne );

    bs->Append ( ( unsigned int ) vb.size() );

    if ( vb.size() > 0 )
    {
        bs->Append ( vb.data(), vb.size() );
        bs->Append ( vdegb.data(), vdegb.size() );
    }

    for ( int i = 0; i < a_size; ++i )
    {
        if ( ms_b[i] )
        {
            bs->Append ( ( unsigned char ) 1 );
            bs->Append ( ms_b[i], colonne );
            bs->Append ( ms_v[i], colonne );
            occupati[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }
    }
}

void mtfa_mat_star::Deserialize ( mtfa_binary_stream_load* bs )
{
    //Clear!!
    for ( unsigned int i = 0; i < righe_allocate; ++i )
    {
        if ( ms_b[i] )
        {
            free ( ms_b[i] );
            free ( ms_v[i] );
        }
    }

    free ( ms_b );
    free ( ms_v );
    vb.clear();
    vdegb.clear();
    ms_b = nullptr;
    ms_v = nullptr;

    for ( int i = 0; i < a_size; ++i )
    {
        if ( occupati[i] )
        {
            delete occupati[i];
        }
    }

    a_size = bs->Read<unsigned int>();

    righe = bs->Read<unsigned int>();
    colonne = bs->Read<unsigned int>();
    righe_allocate = righe;
    colonne_allocate = colonne;

    ms_b = ( unsigned int** ) malloc ( sizeof ( unsigned int* ) * a_size );
    ms_v = ( unsigned long long** ) malloc ( sizeof ( unsigned long long* ) * a_size );

    righe_allocate = a_size;
    righe = a_size;

    for ( unsigned int i = 0; i < righe_allocate; i++ )
    {
        ms_b[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
        ms_v[i] = nullptr; //(unsigned int*)malloc(sizeof(unsigned int)*colonne_allocate);
    }

    simplerng = mtfa_fastrng1024 ( mtfa_rand_simple_ull() );
    occupati.resize ( a_size, nullptr );

    unsigned int vb_size = bs->Read<unsigned int>();

    if ( vb_size > 0 )
    {
        unsigned int* tmp_vb = bs->Read<unsigned int> ( vb_size );
        unsigned int* tmp_vdegb = bs->Read<unsigned int> ( vb_size );
        vb.assign ( tmp_vb, &tmp_vb[vb_size] );
        vdegb.assign ( tmp_vdegb, &tmp_vdegb[vb_size] );
        free ( tmp_vb );
        free ( tmp_vdegb );
    }
    else
    {
        vb.resize ( 0 );
        vdegb.resize ( 0 );
    }

    for ( int i = 0; i < a_size; ++i )
    {
        unsigned char p = bs->Read<unsigned char>();

        if ( p == 0 )
        {
            //non ci sono colonne
        }
        else
        {
            ms_b[i] = bs->Read<unsigned int> ( colonne );
            ms_v[i] = bs->Read<unsigned long long> ( colonne );
            occupati[i] = new mtfa_bitset();
            occupati[i]->Deserialize ( bs );
        }
    }
}

/*******************************************************LOG MAT_STAR***************************************************/
bool log_mat_star::Exists ( unsigned int a, unsigned int b,
                            unsigned long long** pindex )
{
    //    if (a >= vna.size()) {
    //        return zerox8000;
    //    }

    unsigned int vna_a = vna[a];

    if ( vna_a == zerox8000 )
    {
        return false;
    }

    return vm[vna_a]->Exists ( a, b, pindex );
}

unsigned long long* log_mat_star::Insert ( unsigned int a, unsigned int b, unsigned int idx )
{
    unsigned long long* vret;  // = nullptr;

    //    //a è un byte oppure una *?
    //    if (a <= 255)
    //    {
    //        /*
    //         * Esempietto: a è un asterisco, quindi va preso come tutti i possibili valori. Qui ci sono arrivato da un
    //         * cammino preciso e questo cammino che porta a un asterisco, potrebbe essere uguale a altri cammini che portano
    //         * a valori singoli, ad esempio 1*2 133 che va letto come 1[*-2-3]2 e poi come 132 e 133.
    //         * quindi, supponiamo di essere all'inizio. Entra "*", lo mappo come tutti i possibili valori: [0-255]
    //         * poi entra 1. Per questa matrice 1 e * sono la stessa cosa ma 1 porta a un indice diverso da *
    //         * Ma quindi non sarebbe meglio fare le cose dall'esterno? cioè riscrivo l'input e lo inserisco senza dover modificare
    //         * a tempo di insert?
    //         * Esempio ulteriore:
    //         * 1*2*31
    //         * 12*132
    //         * 122333
    //         *
    //         * 1*
    //         * 12
    //         * diventa  1* => 1
    //         *          12 => 2
    //         */
    //    }
    //    else
    //    {
    //        //Se è un * allora devo spazzolarmi tutti i valori!
    //    }

    //Quanti nodi a abbiamo già in questa matrice?
    unsigned int partizione;

    if ( ( partizione = vna[a] ) == zerox8000 )
    {
        vna[a] = 0; //parto da qui!!
        partizione = 0;
    }

    //Ma esiste il nodo b in questa partizione?
    if ( !vm[partizione]->ExistsB ( b ) )
    {
        return vm[partizione]->Insert ( a, b, idx ); //ho inserito una nuova coppia!
    }

    //Il nodo b esiste in questa partizione ne valuto il grado
    int deg = vm[partizione]->GetDegreeB ( b );

    if ( deg <= logn )
    {
        //Lo posso inserire senza problemi!
        unsigned long long* ret = vm[partizione]->Insert ( a, b, idx );
        return ret;
    }

    //Questo arco a,b porta il grado di b sopra il log, devo spostare tutti i b collegati con questo a

    // Costruiamo l'elenco degli elementi b collegati con questo a che mi porto da un'altra parte
    int listb_len;
    unsigned int* list_b = vm[partizione]->GetAllBConnectedToThisA ( a, listb_len );

    // Ma è meglio andare sempre avanti oppure ricominciare a cercare dall'inizio?
    // Ricominciamo a cercare dalla fine
    for ( int i = ( num_m - 1 ); i >= 0; --i )
    {
        if ( i == partizione )
            continue;

        // Non è un problema di spazio, è un problema di grado.
        // Tutte queste coppie a,b che sto inserendo, aumentano o no il grado di un b qui dentro?
        // Tieni presente che essendo un unico a, non ci possono essere due b uguali altrimenti starei inserendo la stessa coppia.
        // Quindi per ognuno dei b, uno diverso dall'altro, verifico se incrementa il grado dei b esistenti fino alla fine
        set<unsigned int> lsui;
        bool must_continue = false;

        for ( int it = 0; it < listb_len; ++it )
        {
            if ( logn <= ( 1 + vm[i]->GetDegreeB ( list_b[it] ) ) )
            {
                must_continue = true;
                break;
            }
        }

        if ( must_continue )
            continue;

        //se sono qui, allora posso inserire tutte le coppiette a,b in questa partizione!
        vna[a] = i; //a appartiene a questa partizione!

        auto vm_partizione_ms_v_a = vm[partizione]->ms_v[a];
        auto vm_partizione_vb = vm[partizione]->vb;

        for ( int it = 0; it < listb_len; ++it )
        {
            unsigned int v = list_b[it];
            unsigned int appo = vm_partizione_ms_v_a[vm_partizione_vb[v]];
            vm[i]->Insert ( a, v, appo );
            //Lo elimino dalla precedente
            vm[partizione]->DeleteEdge ( a, v );
        }

        //Inserisco la coppia corrente
        vret = vm[i]->Insert ( a, b, idx );
        free ( list_b );
        return vret;
    }

    //se sono qui non ho trovato nemmeno una partizione ammissibile, devo crearne una nuova
    num_m++;

    mtfa_mat_star** vmtmp = new mtfa_mat_star*[num_m];

    for ( int i = 0; i < ( num_m - 1 ); ++i )
        vmtmp[i] = vm[i];

    delete vm;

    vm = vmtmp; //Assegno la nuova struttura ampliata
    vm[num_m - 1] = new mtfa_mat_star ( a_size );
    vna[a] = num_m - 1;

    for ( int it = 0; it < listb_len; ++it )
    {
        unsigned int v = list_b[it];
        unsigned int appo = vm[partizione]->ms_v[a][vm[partizione]->vb[v]]; // & zerox3FFF;
        vm[num_m - 1]->Insert ( a, v, appo );

        //Lo elimino dalla precedente
        vm[partizione]->DeleteEdge ( a, v );
    }

    //Inserisco la corrente
    vret = vm[num_m - 1]->Insert ( a, b, idx );

    free ( list_b );
    return vret;
}

log_mat_star::log_mat_star ( int in_a_size )
{
    a_size = in_a_size;
    Init();
}

void log_mat_star::Init()
{
    vm = 0;
    num_m = 0;

    vna.resize ( a_size, zerox8000 );

    vm = new mtfa_mat_star*[1];
    vm[0] = new mtfa_mat_star ( a_size );
    num_m = 1;
}

log_mat_star::~log_mat_star()
{
    //Dovrei fare la delete di m...
    for ( int i = 0; i < num_m; i++ )
        if ( vm[i] )
        {
            delete vm[i];
        }

    delete[] vm;

    vm = nullptr;
    num_m = 0;
}

void log_mat_star::Clear()
{
    vna.clear();

    if ( vm != nullptr )
    {
        for ( int i = 0; i < num_m; ++i )
        {
            vm[i]->Clear();
            delete vm[i];
        }

        delete vm;
    }

    num_m = 0;
    vm = nullptr;

    Init();
}

unsigned long long log_mat_star::Size()
{
    unsigned long long tot = 0;
    tot += sizeof ( num_m );
    tot += vna.size();
    tot += sizeof ( vm );

    cout << __func__ << "Num of logmat: " << num_m << endl;

    for ( int i = 0; i < num_m; ++i )
    {
        cerr << "\nMAT-" << i << " start" << endl;
        unsigned long long v1 = sizeof ( vm[i] );
        unsigned long long v2 = vm[i]->Size();
        cerr << "MAT-" << i << " total size: " << v1 + v2 << endl;
        tot += v1;
        tot += v2;
    }

    //printf("log_mat. Total: %llu\n", tot);

    return tot;
}

void log_mat_star::Serialize ( mtfa_binary_stream_save* bs )
{
    bs->Append ( ( unsigned int ) a_size );
    bs->Append ( ( unsigned int ) num_m );

    for ( int i = 0; i < num_m; ++i )
    {
        if ( vm[i] != nullptr )
        {
            bs->Append ( ( unsigned char ) 1 );
            vm[i]->Serialize ( bs );
        }
        else
        {
            bs->Append ( ( unsigned char ) 0 );
        }

    }

    bs->Append ( vna.data(), a_size );
}

void log_mat_star::Deserialize ( mtfa_binary_stream_load* bs )
{
    Clear();

    //La clear chiama init che crea una vm
    for ( int i = 0; i < num_m; ++i )
    {
        delete vm[i];
    }

    delete[] vm;
    vm = 0;

    a_size = bs->Read<unsigned int>();

    num_m = bs->Read<unsigned int>();
    vm = new mtfa_mat_star*[num_m];

    for ( int i = 0; i < num_m; ++i )
    {
        unsigned char exists = bs->Read<unsigned char>();

        if ( exists == 1 )
        {
            vm[i] = new mtfa_mat_star ( a_size );
            vm[i]->Deserialize ( bs );
        }
        else
        {
            vm[i] = nullptr; //new mtfa_mat_star(a_size);
        }
    }

    unsigned int vna_dim = a_size;
    unsigned int* p = bs->Read<unsigned int> ( vna_dim );
    vna.assign ( p, p + a_size );
    free ( p );
}

/*****************************************************************************************************************/
//La classe mtfads_pair_star
mtfads_pair_star::mtfads_pair_star ( int _a_size )
{
    a_size = _a_size;
    lmat = new log_mat_star ( a_size );
    //    indice_archi_inseriti = 1; //Si parte sempre da 1
}

mtfads_pair_star::~mtfads_pair_star()
{
    if ( lmat )
    {
        delete lmat;
    }

    lmat = nullptr;
}

void mtfads_pair_star::Clean()
{
    lmat->Clear();
}

//int mtfads_pair::Exists(const unsigned int v1, const unsigned int v2)
//{
//    unsigned int idx = lmat->Exists(v1, v2);
//    if (idx == zerox8000) {
//        return -1;
//    }
//    else
//        return idx;
//}

bool mtfads_pair_star::Exists ( const unsigned int v1, const unsigned int v2,
                                unsigned long long** pindex )
{
    return lmat->Exists ( v1, v2, pindex );
}

//int mtfads_pair::Insert(const unsigned int v1, const unsigned int v2)
//{
//    int idx = lmat->Insert(v1, v2, indice_archi_inseriti);
//
//    //L'ho inserito oppure era già presente?
//    if (idx == indice_archi_inseriti)
//        ++indice_archi_inseriti;
//
//    return idx;
//}

unsigned long long* mtfads_pair_star::Insert ( const unsigned int v1,
        const unsigned int v2, unsigned int index )
{
    unsigned long long* ret = lmat->Insert ( v1, v2, index );
    return ret;
}

std::list<std::tuple<unsigned int, unsigned int, unsigned int>> mtfads_pair_star::GetAll()
{
    std::list<std::tuple<unsigned int, unsigned int, unsigned int>> all;

    if ( !lmat )
    {
        return all;
    }

    std::set<unsigned int> righe;

    for ( int r = 0; r < lmat->vna.size(); ++r )
    {
        righe.insert ( lmat->vna[r] );
    }

    for ( auto val : righe )
    {
        //unsigned int val = lmat->vna[r];
        if ( val != zerox8000 )
        {
            //Questa matrice esiste.
            for ( unsigned int a = 0; a < lmat->vm[val]->righe; ++a )
            {
                for ( unsigned int c = 0; c < lmat->vm[val]->colonne; ++c )
                {
                    if ( lmat->vm[val]->ms_b[a][c] != zerox8000 )
                        all.push_back (
                            std::make_tuple ( a, lmat->vm[val]->ms_b[a][c],
                                              lmat->vm[val]->ms_v[a][c] ) );
                }
            }
        }
    }

    return all;
}

void mtfads_pair_star::Serialize ( mtfa_binary_stream_save* bs )
{
    //    bs->Append(indice_archi_inseriti);
    lmat->Serialize ( bs );
}

void mtfads_pair_star::Deserialize ( mtfa_binary_stream_load* bs )
{
    //    indice_archi_inseriti = bs->Read<unsigned int>();
    lmat->Deserialize ( bs );
}

/*********************************************** mtfa_sequence_star ******************************************************/

const unsigned short mtfa_sequence_star::jollyvalue = ( unsigned short ) 0x0100;
const unsigned short mtfa_sequence_star::emptyvalue = ( unsigned short ) 0x0101;

mtfa_sequence_star::~mtfa_sequence_star()
{
    for ( auto it : vpairs )
        if ( it.second )
        {
            delete it.second;
        }

    vpairs.clear();

}

mtfa_sequence_star::mtfa_sequence_star()
{
    _size = 0;
}

unsigned int mtfa_sequence_star::InsertFromHere ( const unsigned short* s, int slen, unsigned int _prec_idx, unsigned long long** last_cell )
{
    unsigned long long prec_idx = _prec_idx;
    unsigned long long* pprec_idx = &prec_idx;
    unsigned long long pos;

    //cout << __func__   << "Deve inserire <" << string(s, s+slen) << " a partire da " << vpos << " con indice: " << _prec_idx << endl;

    for ( int i = 0; i < slen; ++i )
    {
        auto curr_idx = vpairs[i].first;    //nuovo indice che posso assegnare
        pprec_idx = vpairs[i].second->Insert ( s[i], ( unsigned int ) ( ( *pprec_idx ) & zerox3FFFULL ), curr_idx );
        *last_cell = pprec_idx;

        //Il valore di ritorno è l'indirizzo della cella di matrice in cui ha messo l'indice attuale oppure in cui è presente l'indice
        //di un elemento già inserito in precedenza

        //Se l'indice nuovo è pari a curr_idx, ha inserito la coppia come nuovo elemento
        switch ( ( *pprec_idx ) & zeroxC000ULL )
        {
            case zerox0000ULL:
            {
                //Può essere nuova oppure può dipendere dal fatto che già esisteva.
                if ( ( ( *pprec_idx ) & zerox3FFF ) == curr_idx )
                {
                    //ho aggiunto una nuova coppia
                    vpairs[i].first++; //aggiorno l'indice di livello (è una nuova coppia=)

                    //ho terminato la insert della sequenza completa?
                    if ( ( slen - 1 - i ) == 0 )
                    {
                        //scrivo tre cose: quanti elementi ho inserito finora (size), l'informazione che qui termina una sequenza, l'indice della sequenza
                        pos = _size++;
                        ( *pprec_idx ) = ( pos << 32 ) | zerox4000 | curr_idx;  //0x4000 => qui una sequenza termina
                        return pos; //ho inserito finora pos elementi
                    }
                    else
                    {
                        //Non termina qui, qui non ci sono passate altre sequenze. Proseguo! //0x0000 ci sono sequenze passate qui ma nessuna termina
                        ( *pprec_idx ) = curr_idx;
                        continue;
                    }
                }
                else
                {
                    //non è stata inserita una coppia, già esisteva.

                    //ho terminato la insert della sequenza completa?
                    if ( ( slen - 1 - i ) == 0 )
                    {
                        //scrivo tre cose: quanti elementi ho inserito finora (size), l'informazione che qui termina una sequenza, l'indice della sequenza
                        pos = _size++;
                        ( *pprec_idx ) |= ( pos << 32 ) | zerox4000;  //0x4000 => qui una sequenza termina (è possibile che un'altra prosegua)
                        return pos; //ho inserito finora pos elementi
                    }
                    else
                    {
                        //Non termina qui, qui ci sono passate altre sequenze. Proseguo!
                        continue;
                    }
                }
            }
            break;

            case zerox4000ULL:   //in questo posto una terminava
            {
                //Se termina anche questa, nulla cambia e anche se non termina nulla cambia. Ne termina almeno una e una prosegue
                if ( ( slen - 1 - i ) == 0 )
                {
                    //Prendo la posizione della precedente poiché anche questa termina e quindi è uguale a questa
                    return ( *pprec_idx ) >> 32;
                }
                else
                {
                    //Non termina, non faccio nulla, proseguo e lascio 4
                    continue;
                }
            }
            break;

        defaut:
            mtfa_log ( "Errore in switch case. Case non previsto." );
            break;
        }
    }

    //Non accade mai!
    return 0;
}

//La insert che accumula
void mtfa_sequence_star::Insert ( string rule, unsigned int id, int jolly )
{
    //cout << __func__   << rule << " " << id << endl;
    vector<unsigned short> vs ( rule.size() );
    vs.assign ( rule.begin(), rule.end() );

    for ( int i = 0; i < vs.size(); ++i )
        if ( vs[i] == jolly )
        {
            vs[i] = jollyvalue;
        }

    db[vs].insert ( id );
    return;
}

/**Ricorda che hai sempre due valori, jollivalue e emptyvalue, che sono speciali. Quindi anche in questo caso, la insert vale per stringhe di byte.
 * Il fatto che sia un vettore di short è dovuto alla necessità di mettere un valore jolly e un valore vuoto.
 */
void mtfa_sequence_star::Insert ( vector<unsigned short> vs, unsigned int id )
{
    db[vs].insert ( id );
    return;
}

struct scomp_umsi_it
{
    bool operator() ( const unordered_map<string, int>::iterator lhs,
                      const unordered_map<string, int>::iterator rhs ) const
    {
        return lhs->first.compare ( rhs->first );
    }
};

struct hash_umsi_it
{
    std::size_t operator() ( const map<string, int>::iterator& p ) const
    {
        return hash<string>() ( p->first ) ^ hash<int>() ( p->second );
    }
};

struct scomp_msi_it
{
    bool operator() ( const map<string, int>::iterator lhs,
                      const map<string, int>::iterator rhs ) const
    {
        return lhs->first < rhs->first;
    }
};


static string SV ( vector<unsigned short> vs )
{
    std::ostringstream bout;

    for ( int i = 0; i < vs.size(); ++i )
        bout << ( ( vs[i] < 256 ) ? ( char ) vs[i] : '*' );

    return bout.str();
}
static string SV ( set<int> vs )
{
    std::ostringstream bout;

    for ( auto i : vs )
        bout << i << " ";

    return bout.str();
}

void mtfa_sequence_star::InternalInsert()
{
    map<vector<unsigned short>, set<int>> newdb;
    map<vector<unsigned short>, set<int>> olddb;

    int maxpos = 0;
    map<int, set<pair<vector<unsigned short>, set<int>>> > posstar;

#if 0

    //Stampo il db
    for ( auto& it : db )
    {
        for ( auto jt : it.first )
            cout << ( ( jt < 256 ) ? ( char ) jt : '*' ) << " ";

        cout << ": ";

        for ( auto jt : it.second )
            cout << jt << " ";

        cout << endl;
    }

    cout << endl;
#endif

    for ( auto it : db )
    {
        auto pos = it.first.begin();
        pos = std::find ( pos, end ( it.first ), mtfa_sequence_star::jollyvalue ); //cerco il primo asterisco

        while ( pos != it.first.end() )
        {
            int ipos = std::distance ( it.first.begin(), pos );

            if ( ipos > maxpos )
            {
                maxpos = ipos;
            }

            posstar[ipos].insert ( it );
            pos++;
            pos = std::find ( pos, it.first.end(), mtfa_sequence_star::jollyvalue ); //cerco il successivo asterisco
        }
    }

#if 0

    //Stampo posstar
    for ( auto& it : posstar )
    {
        cout << it.first << endl;

        for ( auto& jt : it.second )
        {
            cout << "\t";

            for ( auto kt : jt.first )
                cout << ( ( kt < 256 ) ? ( char ) kt : '*' ) << " ";

            cout << ": ";

            for ( auto kt : jt.second )
                cout << kt << " ";

            cout << endl;
        }

        cout << endl;
    }

#endif


    olddb.insert ( db.begin(), db.end() );

    for ( int pos = 0; pos <= maxpos; ++pos )
    {
        //per ogni elemento che ha un asterisco in posizione pos,
        //cout << "Ci sono " << posstar[pos].size() << " posstar[" << pos << "]" << endl;
        for ( auto it : posstar[pos] )
        {
            //cout << "Ci sono " << olddb.size() << " items in olddb" << endl;
            for ( auto jt : olddb )
            {
                if ( jt.first.size() > pos &&
                        jt.first[pos] != mtfa_sequence_star::jollyvalue &&
                        internal_memcmp1 ( &it.first[0], &jt.first[0], pos ) == 0 )
                {
#if 0
                    cout << "Uguali: (" << SV ( it.first ) << ") (" << SV ( jt.first ) << ")" << endl;
#endif
                    vector<unsigned short> news;

                    //Fino a pos+1 prende di preferenza jt ma se jt è *, allora prende it1
                    for ( int i = 0; i < ( pos + 1 ); ++i )
                    {
                        if ( jt.first[i] == mtfa_sequence_star::jollyvalue )
                            news.push_back ( it.first[i] );
                        else
                            news.push_back ( jt.first[i] );
                    }

                    //poi mette il rimanente
                    //news.insert ( news.end(), &jt.first[0], &jt.first[pos+1] );
                    news.insert ( news.end(), next ( it.first.begin(), pos + 1 ), it.first.end() );
                    //cout << "Inserisce: " << SV(news) << endl;
                    //auto news = jt.first.substr(0, pos+1)+it.first.substr(pos+1);
                    auto p = newdb.find ( news );

                    if ( p == newdb.end() )
                    {
                        newdb[news] = it.second;
                        //se ci sono altri asterischi da qui al termine, allora inserisco in posstar
                        auto newpos = news.begin();
                        newpos = find ( newpos, news.end(), mtfa_sequence_star::jollyvalue );

                        //news.find(jollyvalue, pos+1);    //cerco il primo asterisco
                        while ( newpos != news.end() )
                        {
                            posstar[std::distance ( news.begin(), newpos )].insert ( make_pair ( news, it.second ) );
                            newpos++;
                            newpos = find ( newpos, news.end(), mtfa_sequence_star::jollyvalue );
                            //newpos = news.find(jollyvalue, newpos + 1);  //Cerco i successivi asterischi
                        }
                    }
                }
            }
        }

        olddb.insert ( newdb.begin(), newdb.end() );
        newdb.clear();
    }

    db.clear();
    db.insert ( olddb.begin(), olddb.end() );

    return;
}

unsigned int mtfa_sequence_star::Build()
{
    InternalInsert();
    //cout << __func__   << "Modified rules: " << db.size() << endl;

#if 1

    /* Ora, per ogni elemento senza asterischi, se match con gli altri, aggiunge il set */
    map<vector<unsigned short>, set<int>> db_senzajolly;
    map<vector<unsigned short>, set<int>> db_conjolly;

    for ( auto& it : db )
    {
        auto found = find ( it.first.begin(), it.first.end(), jollyvalue );

        if ( found != it.first.end() )
        {
            //Trovato elemento con jolly!
            db_conjolly.insert ( it );
        }
        else
        {
            //Trovato elemento senza jolly!
            db_senzajolly.insert ( it );
        }
    }

    for ( auto& sj : db_senzajolly )
    {
        for ( auto& cj : db_conjolly )
        {
            bool match = true;

            if ( sj.first.size() != cj.first.size() )
            {
                continue;
            }

            for ( int i = 0; i < sj.first.size(); ++i )
            {
                if ( sj.first[i] != cj.first[i] && cj.first[i] != jollyvalue )
                {
                    //cout << __func__   << "Confronta: " << (char)sj.first[i] << "-" << (char)cj.first[i] << endl;;
                    match = false;
                    break;
                }
            }

            if ( match )
            {
                //cout << __func__   << "Inserisce: " << endl;
                sj.second.insert ( cj.second.begin(), cj.second.end() );
            }
        }
    }

    db = db_senzajolly;
    db.insert ( db_conjolly.begin(), db_conjolly.end() );

#endif

#if 0
    cout << __func__ << "Archivio delle regole\n";

    for ( auto it : db )
    {
        for ( int i = 0; i < it.first.size(); ++i )
        {
            if ( it.first[i] == jollyvalue )
            {
                cout << "*";
            }
            else
            {
                cout << it.first[i];
            }
        }

        cout << " => ";

        for ( auto j : it.second )
        {
            cout << j << " ";
        }

        cout << endl;
    }

#endif

    //Ora che ho in db tutto, allora posso inserire nella mtfa_sequence
    for ( auto it : db )
    {
        int vsize = vpairs.size();

        if ( vsize < it.first.size() )
        {
            vpairs.resize ( it.first.size() );

            for ( int j = vsize; j < it.first.size(); ++j )
            {
                vpairs[j].first = 1;
                vpairs[j].second = new mtfads_pair_star ( 258 );
            }
        }

        unsigned long long* last_cell;

        //Inserisce it.second regole!
        unsigned int pos = InsertFromHere ( &it.first[0], it.first.size(), 0, &last_cell );

        if ( pos >= rule_set.size() )
        {
            rule_set.resize ( pos + 1 );
        }

        rule_set[pos].insert ( it.second.begin(), it.second.end() );
    }

    return 0;
}

int mtfa_sequence_star::internal_memcmp ( const unsigned short* s1,
        const unsigned char* s2, int len )
{
    while ( ( len > 0 ) && ( *s1 == ( unsigned short ) *s2++ || *s1 == jollyvalue ) )
    {
        s1++, len--;
    }

    return len;
}

//per gli array di short
int mtfa_sequence_star::internal_memcmp ( const unsigned short* s1,
        const unsigned short* s2, int len )
{
    while ( ( len > 0 ) && ( *s1 == *s2++ || *s1 == jollyvalue ) )
    {
        s1++, len--;
    }

    return len;
}

int mtfa_sequence_star::internal_memcmp1 ( const unsigned short* s1,
        const unsigned short* s2, int len )
{
    while ( ( len > 0 ) && ( *s1 == *s2 || *s1 == jollyvalue || *s2 == jollyvalue ) )
    {
        s1++, s2++, len--;
    }

    return len;
}

int mtfa_sequence_star::internal_memcmp_noj ( const unsigned short* s1,
        const unsigned short* s2, int len )
{
    while ( ( len > 0 ) && ( *s1 == *s2 ) )
    {
        s1++, s2++, len--;
    }

    return len;
}

set<int> mtfa_sequence_star::GetRules ( int pos )
{
    if ( pos >= rule_set.size() )
    {
        return set<int>();
    }
    else
    {
        return rule_set[pos];
    }
}

bool mtfa_sequence_star::Search ( const vector<unsigned short> s, unsigned int& ident )
{
    //Torna la posizione di questo elemento
    int vsize = vpairs.size();
    int slen = s.size();

    if ( vsize < slen )
        return false;

    unsigned long long idx = 0;
    unsigned long long* pidx = nullptr;

    for ( int i = 0; i < vsize; ++i )
    {
        /*I casi possono essere:
         * indico con S la stringa da cercare e con D quella nella DS
         *  - matchano sempre
         *      1) S[i] = 0-255; D[i] = 0-255
         *      2) S[i] = empty; D[i] = empty
         *      3) S[i] = 0-255; D[i] = *
         *      4) S[i] = empty; D[i] = *
         *
         *  - non matchano mai
         *      5) S[i] = 0-255; D[i] = empty
         *      6) S[i] = empty; D[i] = 0-255
         * */

        //questa prima ricerca copre: 1 e 2
        bool bret = vpairs[i].second->Exists ( s[i], idx, &pidx );

        if ( !bret )
        {
            //questa seconda ricerca copre 3 e 4
            bret = vpairs[i].second->Exists ( jollyvalue, idx, &pidx );

            if ( !bret )
                return false;
        }

        idx = *pidx & zerox3FFFULL;

        switch ( *pidx & zeroxC000ULL )
        {
            case zerox8000ULL:
            {
                //Vuoto
                return false;
            }
            break;

            case zerox4000ULL:
            {
                //Almeno una che si ferma.
                if ( ( slen - i - 1 ) == 0 )
                {
                    ident = ( *pidx ) >> 32;
                    return true;
                }
            }
            break;

            case 0x0000000000000000ULL: //nessuna si ferma
            {
            }
            break;
        }
    }

    return false;
}


bool mtfa_sequence_star::Search ( const unsigned char* s, int slen, unsigned int& ident )
{
    //Torna la posizione di questo elemento
    int vsize = vpairs.size();

    if ( vsize < slen )
        return false;

    unsigned long long idx = 0;
    unsigned long long* pidx = nullptr;

    for ( int i = 0; i < slen; ++i )
    {
        bool bret = vpairs[i].second->Exists ( s[i], idx, &pidx );

        if ( !bret )
        {
            bret = vpairs[i].second->Exists ( jollyvalue, idx, &pidx );

            if ( !bret )
                return false;
        }

        idx = *pidx & zerox3FFFULL;

        switch ( *pidx & zeroxC000ULL )
        {
            //La nuova versione
            case zerox8000ULL:
            {
                return false;
            }
            break;

            case zerox4000ULL:   //Almeno una che si ferma.
            {
                //Almeno una che si ferma.
                if ( ( slen - i - 1 ) == 0 )
                {
                    ident = ( *pidx ) >> 32;
                    return true;
                }
            }
            break;

            case 0x0000000000000000ULL: //nessuna si ferma
            {
            }
            break;
        }
    }

    return false;
}

void mtfa_sequence_star::Serialize ( mtfa_binary_stream_save* bs )
{
    //vector<pair<unsigned int, mtfads_pair_star*>> vpairs; //idx partono da 1
    unsigned int len = vpairs.size();
    bs->Append ( len );

    for ( int i = 0; i < len; ++i )
    {
        bs->Append ( vpairs[i].first );
        vpairs[i].second->Serialize ( bs );
    }

    //vector<pair<short*, int>> whole_strings; //Contiene le stringhe che sono state archiviate, anche parzialmente, nella struttura
    //map<vector<short>, set<int>> db;
    //Archivio anche il db di input, non si sa mai, dovessi rifare il tutto
    len = db.size();
    bs->Append ( len );

    for ( auto it : db )
    {
        len = it.first.size();
        bs->Append ( len );

        for ( int i = 0; i < len; ++i )
        {
            bs->Append ( it.first[i] );
        }

        len = it.second.size();
        bs->Append ( len );

        for ( auto jt : it.second )
        {
            bs->Append ( ( unsigned int ) jt );
        }
    }

    //vector<set<int>> rule_set;  //gli insiemi delle regole di ingresso.
    len = rule_set.size();
    bs->Append ( len );

    for ( auto it : rule_set )
    {
        len = it.size();
        bs->Append ( len );

        for ( auto jt : it )
        {
            bs->Append ( ( unsigned int ) jt );
        }
    }
}

void mtfa_sequence_star::Deserialize ( mtfa_binary_stream_load* bs )
{
    for ( auto& it : vpairs )
    {
        if ( it.second )
        {
            delete it.second;
        }
    }

    vpairs.clear();

    db.clear();
    rule_set.clear();

    unsigned int len = bs->Read<unsigned int>();
    vpairs.resize ( len );

    for ( int i = 0; i < len; ++i )
    {
        int pos = bs->Read<unsigned int>();
        mtfads_pair_star* p = new mtfads_pair_star ( 258 );
        p->Deserialize ( bs );
        vpairs[i] = make_pair ( pos, p );
    }

    //map<vector<short>, set<int>> db;
    //Leggo anche il db di input, non si sa mai, dovessi rifare il tutto
    len = bs->Read<unsigned int>();

    for ( int i = 0; i < len; ++i )   //gli <len> elementi della map
    {
        int ilen = bs->Read<unsigned int>();    //vettore key lungo ilen
        vector<unsigned short> vs;
        vs.resize ( ilen );

        for ( int j = 0; j < ilen; ++j )
        {
            vs[j] = bs->Read<unsigned short>();
        }

        //ora leggo il set di int
        set<int> si;
        ilen = bs->Read<unsigned int>();    //set<int>, dati lungo ilen

        for ( int j = 0; j < ilen; ++j )
        {
            si.insert ( bs->Read<unsigned int>() );
        }

        //e ora inserisco nella map
        db[vs] = si;
    }

    //vector<set<int>> rule_set;  //gli insiemi delle regole di ingresso.
    len = bs->Read<unsigned int>();
    rule_set.resize ( len );

    for ( int i = 0; i < len; ++i )   //leggo il vettore rule_set
    {
        //leggo il set di interi
        int ilen = bs->Read<unsigned int>();    //vettore key lungo ilen
        set<int> si;

        for ( int j = 0; j < ilen; ++j )
        {
            si.insert ( bs->Read<unsigned int>() );
        }

        rule_set[i] = si;
    }
}


/********************************************************************************************************************/
/********************************************************************************************************************/
/*********************************************** mtfa_sequences ******************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
vector<uint8_t> mtfa_StringToVui8(const string & s)
{
    vector<uint8_t> v;
    v.assign(s.begin(), s.end());
    return v;
};
string mtfa_Vui8ToString(const vector<uint8_t> &v)
{
    //converto vettore in stringa
    string s;
    s.assign((char*)&v.data()[0], v.size());
    return s;
};

//Assegna "sempre" 3 (da 1 a 4) byte per stringa
vector<uint8_t> mtfa_LsToVui8(mtfa_umph &ph, const list<string> & ls, const int numOfBytesPerString)
{
    vector<uint8_t> vui8;
    for (auto s: ls)
    {
        int it = ph.Insert(s);
        uint8_t vll=0, vlh=0, vhl=0, vhh=0;
        switch (numOfBytesPerString)
        {
            case 4:
                vhh = (it >> 24) & 0x000000FF;
                vui8.push_back(vhh);
            case 3:
                vhl = (it >> 16) & 0x000000FF;
                vui8.push_back(vhl);
            case 2:
                vlh = (it >> 8) & 0x000000FF;
                vui8.push_back(vlh);
            case 1:
                vll = (it >> 0) & 0x000000FF;
                vui8.push_back(vll);
        }
    }
    return vui8;
}

void mtfa_umph::clear()
{
    umsi.clear();;
    umis.clear();
}
mtfa_umph::mtfa_umph()
{
    uidx = 0;       //il primo indice valido è 1
}
mtfa_umph::~mtfa_umph()
{
    //nothing to do
}
int mtfa_umph::Insert(const string & s)
{
    auto result = umsi.insert(make_pair(s, ++uidx));
    if (!result.second) 
    {
        --uidx; //non ho inserito
        return result.first->second;
    }
    else
    {
        umis[uidx] = result.first;
        return uidx;
    }
}
int mtfa_umph::Search(const string & s)
{
    auto it = umsi.find(s);
    if (it == umsi.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}
int mtfa_umph::Insert(const vector<uint8_t> & v)
{
    return Insert(mtfa_Vui8ToString(v));
}
int mtfa_umph::Search(const vector<uint8_t> & v)
{
    return Search(mtfa_Vui8ToString(v));
}
string mtfa_umph::ReverseSearch(const int & i)
{
    auto result = umis.find(i);
    if (result == umis.end())
        return string();
    else
        return result->second->first;
}

void mtfa_umph::Serialize ( mtfa_binary_stream_save * bs )
{
//     unordered_map<string, int> umsi;
//     unordered_map<int, unordered_map<string, int>::iterator> umis;
//     int uidx;  //da 1 in poi
    int lenumsi = umsi.size();
    bs->Append(lenumsi);
    for (auto it: umsi)
    {
        string s = it.first;
        int i = it.second;
        bs->Append(s);
        bs->Append(i);
    }
    
    bs->Append(uidx);
}

void mtfa_umph::Deserialize ( mtfa_binary_stream_load * bs )
{
//     unordered_map<string, int> umsi;
//     unordered_map<int, unordered_map<string, int>::iterator> umis;
//     int uidx;  //da 1 in poi
    umsi.clear();
    umis.clear();
    int lenumsi = bs->Read<int>();
    for (int i=0; i<lenumsi; ++i)
    {
        string s = bs->Read();
        int idx = bs->Read<int>();
        auto ptr = umsi.insert(make_pair(s, idx));
        if (ptr.second)
            umis[idx] = ptr.first;
    }
    uidx = bs->Read<int>();
}



//SE zerox8000, allora posto vuoto
//SE zerox4000, allora termine sequenza e quindi il codice indicato mi potrà servire in seguito
//SE zeroxC000, allora una sequenza di ferma qui e un'altra prosegue con il codice indicato

//la sequence base non archivia ma costruisce esclusivamente struttura a partire dall'input. Consente la ricerca, lavora su vettori di unsigned int8
mtfa_sequence_base::~mtfa_sequence_base()
{
    for ( auto it : vpairs )
        if ( it.first) {
            delete it.first;
            it.first = nullptr;
        }
     vpairs.clear();
}

mtfa_sequence_base::mtfa_sequence_base()
{
    uidx=0; //La struttura è vuota
}

long mtfa_sequence_base::Insert ( const vector<uint8_t> & vui8)
{
    //ora che ho una lista di bytes
    //inserisco nella struttura dati
    //Ricorda che la convenzione è la seguente:
    //SE zerox8000, allora posto vuoto
    //SE zerox4000, allora termine sequenza e quindi il codice indicato mi potrà servire in seguito
    //SE zeroxC000, allora una sequenza di ferma qui e un'altra prosegue con il codice indicato
    
    //Per la matrice dei valori, invece valgono le seguenti attribuzioni
    //zerox8000ULL = 0x0000000080000000;  //utlizzata a tempo di allocazione della matrice dei valori (unsigned long long)
    //zerox4000ULL = 0x0000000040000000;
    //zerox7FFFULL = 0x000000007FFFFFFF;
    //zerox3FFFULL = 0x000000003FFFFFFF;
    //zeroxC000ULL = 0x00000000C0000000;
    //zerox0000ULL = 0x0000000000000000;

    //se non ho spazio per tutto, lo creo a priori
    if (vpairs.size() < vui8.size())
    {
        auto olds = vpairs.size();
        vpairs.resize(vui8.size(), make_pair(nullptr, 0));
        for (int i=olds; i<vpairs.size(); ++i)
            vpairs[i].first = new mtfads_pair(256);
    } 
        
    auto vpairs_it = vpairs.begin();
    int valb = 0;
    unsigned long long *puidx;  //la parte alta mi fornisce l'indice assoluto. La parte bassa l'indice nella prossima struttura, se necessario.
    bool inserted = false;
    for (auto vala: vui8)
    {
        bool bret = vpairs_it->first->Exists(vala, valb, &puidx);  //puidx è l'indirizzo di quanto scritto in mval ed è stato inizializzato a zerox8000ULL
//         {
//             extern bool myflag1;
//             if (myflag1)
//             {
//                 extern unsigned long long * myptr;
//                 printf("1-Valore    myptr: %llX => %llX\n", myptr, *myptr);
//             }
//         }
        if (bret)
        {
            valb = *puidx & zerox3FFFULL; //il bit più significativo c'è sempre. il successivo c'è solo se termina qui una sequenza.
        }
        else
        {
//             {
//                 extern bool myflag1;
//                 if (myflag1)
//                 {
//                     extern unsigned long long * myptr;
//                     printf("2.0-Valore    myptr: %llX => %llX\n", myptr, *myptr);
//                 }
//             }
            puidx = vpairs_it->first->Insert(vala, valb, vpairs_it->second++ );
//             {
//                 extern bool myflag1;
//                 if (myflag1)
//                 {
//                     extern unsigned long long * myptr;
//                     printf("2.1-Valore    myptr: %llX => %llX\n", myptr, *myptr);
//                 }
//             }
            valb = *puidx & zerox3FFFULL;
            inserted = true;
        }
        vpairs_it++;
    }
    //puidx punta alla parte valore dell'ultimo elemento! è nella parte alta di questo che metto l'indice assoluto!
    if (inserted)
    {  
//         extern unsigned long long * myptr;
//         extern bool myflag;
//         
//         if (myflag)
//         {   
//             myptr = puidx;
//             printf("Assegno myptr: %llX\n", *myptr);
//         }

        unsigned long long add = ++uidx;
        //printf("Inserisce in %llX il valore %llX ottenendo %llX\n", *puidx, add, *puidx | (add << 32));
        *puidx |= (add << 32);
//         {
//             extern bool myflag1;
//             if (myflag1)
//             {
//                 extern unsigned long long * myptr;
//                 printf("3-Valore    myptr: %llX => %llX\n", myptr, *myptr);
//             }
//         }
        return uidx;
    }
    else
    {
        //possiamo avere il caso di presente ma con puidx parte alta a 0, significa che non c'è e quindi significa che c'era
        //una sequenza più lunga di questa e ora sto inserendo questa
        if (*puidx >> 32)
        {
            //printf("Trovata sequenza più lunga: %llX\n", *puidx);
            return *puidx >> 32;
        }
        else
        {
            //devo inserire
            unsigned long long add = ++uidx;
            //printf("Sequenza più lunga ma inserisce in %llX il valore %llX ottenendo %llX\n", *puidx, add, *puidx | (add << 32));
            *puidx |= (add << 32);
            return uidx;
        }
    }
}

long mtfa_sequence_base::Search ( const vector<uint8_t> & vui8)
{
    if (vui8.size() > vpairs.size())
        return -1;

    auto vpairs_it = vpairs.begin();
    int valb = 0;
    unsigned long long *puidx;  //la parte alta mi fornisce l'indice assoluto. La parte bassa l'indice nella prossima struttura, se necessario.
    for (auto vala: vui8)
    {
        bool bret = vpairs_it->first->Exists(vala, valb, &puidx);
        if (bret)
        {
            //already exists, continue
            valb = *puidx & zerox3FFFULL;
        }
        else
        {
            return -1;
        }
        vpairs_it++;
    }
    unsigned int idx = *puidx >> 32;
    return idx;
}

long mtfa_sequence_base::Search1 ( const vector<uint8_t> & vui8)
{
    if (vui8.size() > vpairs.size())
        return -1;

    auto vpairs_it = vpairs.begin();
    int valb = 0;
    unsigned long long *puidx;  //la parte alta mi fornisce l'indice assoluto. La parte bassa l'indice nella prossima struttura, se necessario.
    for (auto vala: vui8)
    {
        if (vpairs_it->first->lmat->vna.size() <= vala)
            return -1;
        auto posa = vpairs_it->first->lmat->vna[vala];
        if (posa == zerox8000)
            return -1;
        if (vpairs_it->first->lmat->vm[posa]->vb.size() <= valb)
            return -1;
        
        auto colb = vpairs_it->first->lmat->vm[posa]->vb[valb];
        if (vpairs_it->first->lmat->vm[posa]->ms_b[vala] == nullptr)
            return 1;
        if (vpairs_it->first->lmat->vm[posa]->ms_b[vala][colb] != valb)
            return -1;
        puidx = &vpairs_it->first->lmat->vm[posa]->ms_v[vala][colb];
        valb = *puidx & zerox3FFFULL;
        vpairs_it++;
    }
    unsigned int idx = *puidx >> 32;
    return idx;
}

// bool mtfa_sequence_base::GetFastPath ( const vector<uint8_t> & vui8, vector<uint8_t> & path)
// {
//     path.clear();
//     if (vui8.size() > vpairs.size())
//         return false;
// 
//     auto vpairs_it = vpairs.begin();
//     int valb = 0;
//     unsigned long long *puidx;  //la parte alta mi fornisce l'indice assoluto. La parte bassa l'indice nella prossima struttura, se necessario.
//     for (auto vala: vui8)
//     {
//         bool bret = vpairs_it->first->Exists(vala, valb, &puidx);
//         if (bret)
//         {
//             //already exists, continue
//             path.push_back(valb);
//             path.push_back(vala);
//             valb = *puidx & zerox3FFFULL;
//         }
//         else
//         {
//             return false;
//         }
//         vpairs_it++;
//     }
//     return true;
// }

unordered_set<long> mtfa_sequence_base::GetAll ( const vector<uint8_t> & vui8 )
{
    unordered_set<long> usl;
    if (vui8.size() > vpairs.size())
    {
        return usl;
    }

    auto vpairs_it = vpairs.begin();
    int valb = 0;
    unsigned long long zero=0;
    unsigned long long *puidx=&zero;
    for (auto vala: vui8)
    {
        bool bret = vpairs_it->first->Exists(vala, valb, &puidx);
        if (bret)
        {
            //already exists, continue
            valb = *puidx & zerox3FFFULL;
        }
        else
        {
            return usl;
        }
        vpairs_it++;
    }
    
    //Da questo punto in poi, prendo tutto quello che ha *puidx >> 32 > 0
    unsigned int idx = *puidx >> 32;
    if (idx > 0)
    {
        usl.insert(idx);
    }
    list<int> lvalb;
    list<int> tmp_lvalb;
    tmp_lvalb.push_back(valb);
    while (vpairs_it != vpairs.end())
    {
        //valb lo conosco, devo prendere tutti i possibili valori di a
        lvalb = tmp_lvalb;
        tmp_lvalb.clear();

        for (int valb: lvalb)
        {
            for (int im = 0; im<vpairs_it->first->lmat->num_m; ++im)
            {
                int colb;
                if (vpairs_it->first->lmat->vm[im]->vb.size() > valb &&
                    (colb = vpairs_it->first->lmat->vm[im]->vb[valb]) != zerox8000)
                {
                    for (int ia=0; ia<vpairs_it->first->lmat->vm[im]->righe; ++ia)
                    {
                        if (vpairs_it->first->lmat->vm[im]->ms_b[ia] && vpairs_it->first->lmat->vm[im]->ms_b[ia][colb] == valb)
                        {
                            //posso utilizzare il valore nell'altra matrice
                            auto pv = vpairs_it->first->lmat->vm[im]->ms_v[ia][colb];
                            tmp_lvalb.push_back(pv & zerox3FFFULL);
                            idx = pv >> 32;
                            if (idx > 0)
                            {
                                usl.insert(idx);
                            }
                        }                        
                    }
                }
            }
        }
        vpairs_it++;
    }

    return usl;
}

//Elimina solo se vui8 punta a una foglia
bool mtfa_sequence_base::Unlink ( const vector<uint8_t> & vui8 )
{
    if (vui8.size() > vpairs.size())
        return false;

    auto vpairs_it = vpairs.begin();
    int valb = 0;
    unsigned long long zero=0;
    unsigned long long *puidx=&zero;
    int scanned = 0;
    for (auto vala: vui8)
    {
        bool bret = vpairs_it->first->Exists(vala, valb, &puidx);
        if (bret)
        {
            //already exists, continue
            valb = *puidx & zerox3FFFULL;
            scanned++;
        }
        else
        {
            break; //??ho terminato vui8???
        }
        vpairs_it++;
    }
    //se non ho terminato la scansione di vui8, esco
    if (scanned != vui8.size())
        return false; //non ho terminato neppure vui8

    //ora metto a zero l'elemento puntato da puidx
    unsigned int idx = *puidx >> 32;
    if (idx != 0)
    {
        *puidx &= zerox3FFFULL; //ho eliminato questo index
        return true;
    }
    else
        return false;
}

void mtfa_sequence_base::Serialize ( mtfa_binary_stream_save * bs )
{
//     long uidx;
//     vector<pair<mtfads_pair*, int>> vpairs;

    bs->Append ( uidx );
    int vlen = vpairs.size();
    
    bs->Append(vlen);
    
    for (int i=0; i<vlen; ++i)
    {
        auto pair = vpairs[i].first;
        int idx = vpairs[i].second;
        pair->Serialize(bs);
        bs->Append(idx);
    }
}

void mtfa_sequence_base::Deserialize ( mtfa_binary_stream_load * bs )
{
    //free del precedente
    for ( auto it : vpairs )
    if ( it.first) {
        delete it.first;
        it.first = nullptr;
    }
    vpairs.clear();

    uidx = bs->Read<long>();
    int vlen = bs->Read<int>();
    
    vpairs.resize ( vlen );

    for ( int i = 0; i < vlen; ++i )
    {
        mtfads_pair* p = new mtfads_pair ( );
        p->Deserialize(bs);
        int idx = bs->Read<int>();
        vpairs[i] = make_pair ( p, idx );
    }
}

//La gestione di sequenze di stringhe!
mtfa_string_sequences::mtfa_string_sequences()
{
    seq = new mtfa_sequence_base();
}
mtfa_string_sequences::~mtfa_string_sequences()
{
    if (seq)
        delete seq;
    seq = nullptr;
    ph.clear();
    itovui8.clear();
}
    
//Ora la insert
int mtfa_string_sequences::insert(const list<string> & ls, const int len)  //len va da 1 a 4 i base alla numerosità delle stringhe di input differenti
{
    //converte la lista di stringhe in lista di uint8_t
    vector <uint8_t> vui8 = mtfa_LsToVui8(ph, ls, len);
//     for (int i=0; i<vui8.size(); ++i)
//         cout << (int)vui8[i] << ", ";
//     cout << endl;
    auto idx = seq->Insert(vui8);
    itovui8[idx] = vui8;
    return idx;
}
int mtfa_string_sequences::insert(const vector <uint8_t> vui8)
{
    auto idx = seq->Insert(vui8);
    itovui8[idx] = vui8;
    return idx;
}

//Ora la search
int mtfa_string_sequences::search(const list<string> & ls, const int len)
{
    //converte la lista di stringhe in lista di uint8_t
    vector <uint8_t> vui8 = mtfa_LsToVui8(ph, ls, len);
    return seq->Search(vui8);
}
int mtfa_string_sequences::search(const vector <uint8_t> vui8)
{
    return seq->Search(vui8);
}

//Ora la searchunlink
bool mtfa_string_sequences::unlink(const list<string> & ls, const int len)
{
    //converte la lista di stringhe in lista di uint8_t
    vector <uint8_t> vui8 = mtfa_LsToVui8(ph, ls, len);
    return seq->Unlink(vui8);
}
bool mtfa_string_sequences::unlink(const vector <uint8_t> vui8)
{
    return seq->Unlink(vui8);
}

//Ora la GetVal
list<string> mtfa_string_sequences::getval(int idx, const int len)
{
    auto pvui8 = itovui8.find(idx);
    if (pvui8 == itovui8.end())
    {
        return list<string>();
    }
    else
    {
        list<string> ls;
        //li prendo len per volta
        for (int i=0; i<pvui8->second.size(); i+=len)
        {
            unsigned int idx=0;
            int pos=len;
            for (int j=0; j<len; ++j)
                ((uint8_t*)&idx)[--pos] = pvui8->second[i+j];
            //cout << "IDX: " << idx << ". IDX[0]...: " << (int)((uint8_t*)&idx)[0]<< ", " << (int)((uint8_t*)&idx)[1] << ", " << (int)((uint8_t*)&idx)[2] << ", " << (int)((uint8_t*)&idx)[3] << endl;
            string s = ph.ReverseSearch(idx);
            ls.push_back(s);
        }
        ls.reverse();
        return ls;
    }
}
//Ora la GetVal
vector<uint8_t> mtfa_string_sequences::getvalvui8(int idx, const int len)
{
    auto pvui8 = itovui8.find(idx);
    return pvui8->second;
}

void mtfa_string_sequences::Serialize ( mtfa_binary_stream_save * bs )
{
//     mtfa_umph ph;
//     mtfa_sequence_base *seq;
//     unordered_map<int, vector<uint8_t>> itovui8;
    ph.Serialize(bs);
    seq->Serialize(bs);
    
    int lenito = itovui8.size();
    bs->Append(lenito);
    for (auto it: itovui8)
    {
        int i = it.first;
        auto vui = it.second;
        bs->Append(i);
        
        int lenvui = vui.size();
        bs->Append(vui.data(), lenvui);
    }
}

void mtfa_string_sequences::Deserialize ( mtfa_binary_stream_load * bs )
{
    ph.clear();
    ph.Deserialize(bs);
    delete seq;
    seq = new mtfa_sequence_base();
    seq->Deserialize(bs);
    
    itovui8.clear();
    int lenito = bs->Read<int>();
    for (int i=0; i<lenito; ++i)
    {
        int key = bs->Read<int>();
        unsigned int size;
        
        uint8_t * p = bs->Read<uint8_t>(size);
        vector<uint8_t> v(p, p+size);
        free(p);
        itovui8[key] = v;
    }
}




