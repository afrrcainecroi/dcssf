#include <iostream>

#include <string>
#include <list>
#include <set>

#include "mtfa_utils.h"
#include "mtfafs.h"
#include "mtfa_fs3.h"

ImplicitDs::ImplicitDs()
{
     p = new mtfafs_s2_extended();
}

ImplicitDs::~ImplicitDs()
{
     if ( p != nullptr ) {
          delete p;
     }
     p = nullptr;

     vs.Clear();
}

//Da definire come si serializzano elementi del lisp!
void ImplicitDs::Serialize ( mtfa_binary_stream_save *bs )
{
     p->Serialize ( bs );
     //vs.Serialize(bs);
}

void ImplicitDs::Deserialize ( mtfa_binary_stream_load *bs )
{
     if ( p != nullptr ) {
          delete p;
     }
     p = nullptr;

     //vs.Clear();
     p = new mtfafs_s2_extended();

     p->Deserialize ( bs );
     //vs.Deserialize (bs);
}

bool ImplicitDs::Insert ( const char* k, int len, void * s )
{
     unsigned int pos = 0;
     bool bRet = p->Insert ( ( unsigned char* ) k, len, &pos );
     if ( bRet ) {
          vs[pos] = s;
          return true;
     }

     return false;
}

//Inserisce se non presente, aggiorna se presente

bool ImplicitDs::Update ( const char* k, int len, void * s )
{
     unsigned int *index_pointer;
     unsigned int *data_pointer;
     unsigned int pos;
     if ( p->Exists ( ( const unsigned char * ) k, len, &pos, &index_pointer, &data_pointer ) ) {
          //aggiorno!
          vs[pos] = s;
          return true;
     } else {
          return Insert ( k, len, s );
     }
}

bool ImplicitDs::Find ( const char* k, int len, void **s )
{
//     unsigned int *index_pointer;
//     unsigned int *data_pointer;
    unsigned int pos;
     if ( p->ExistsFast ( ( const unsigned char * ) k, len, &pos)) { //, &index_pointer, &data_pointer ) ) {
          ( *s ) = vs[pos];
          return true;
     } else {
          return false;
     }

     return false;
}

std::set<std::pair<std::string, void *>> ImplicitDs::GetAll()
{
     std::set<std::pair<std::string, void *>> lsc;

     for ( int i = 0; i < vs.Size(); ++i ) {
          if ( ( *p->v_dati ) [i] != nullptr ) {
               int len;
               unsigned char * val = p->v_dati->Get ( i, &len );
               lsc.insert ( std::make_pair ( std::string ( val, val+len ), vs[i] ) );
          }
     }

     return lsc;
}



/************************************ImplicitDsString stringhe C, che terminano con 0*****************************/

ImplicitDsString::ImplicitDsString()
{
     p = new mtfafs_s2_extended();
     //p = new mtfa_sequence();
}

ImplicitDsString::~ImplicitDsString()
{
     if ( p != nullptr ) {
          delete p;
     }
     p = nullptr;

     vs.Clear();
}

//Da definire come si serializzano elementi del lisp!
void ImplicitDsString::Serialize ( mtfa_binary_stream_save *bs )
{
     p->Serialize ( bs );
     vs.Serialize ( bs );
}

void ImplicitDsString::Deserialize ( mtfa_binary_stream_load *bs )
{
     if ( p != nullptr ) {
          delete p;
     }
     p = nullptr;

     vs.Clear();
     //p = new mtfa_sequence(); // mtfafs_s2_extended();
     p = new mtfafs_s2_extended();

     p->Deserialize ( bs );
     vs.Deserialize ( bs );
}

bool ImplicitDsString::Insert ( char* k, int len, char * s, int lenv )
{
     unsigned int pos = 0;
     bool bret = p->Insert ( ( unsigned char* ) k, len, &pos );
     if ( vs[pos] ) {
          free ( vs[pos] );
     }

     auto v = ( unsigned char* ) malloc ( lenv );
     memcpy ( v, s, lenv );
     vs.put ( pos, v, lenv );
     return true;
}

//Inserisce se non presente, aggiorna se presente
bool ImplicitDsString::Update ( char* k, int len, char * s, int lenv )
{
     unsigned int pos;
     unsigned int *pdata;
     unsigned int *pvalue;
     if ( p->Exists ( ( unsigned char * ) k, len, &pos, &pdata, &pvalue ) ) {
          //aggiorno!
          if ( vs[pos] ) {
               free ( vs[pos] );
          }
          auto v = ( unsigned char* ) malloc ( lenv );
          memcpy ( v, s, lenv );
          vs.put ( pos, v, lenv );
          return true;
     } else {
          return Insert ( k, len, s, lenv );
     }
}

bool ImplicitDsString::Find ( char* k, int len, char **s, int *lenv )
{
     unsigned int pos;
     unsigned int *pdata;
     unsigned int *pvalue;
     if ( p->Exists ( ( unsigned char * ) k, len, &pos, &pdata, &pvalue ) ) {
          ( *s ) = ( char* ) vs.Get ( pos, lenv );
          return true;
     } else {
          return false;
     }
}

std::set<std::pair<std::string, std::string>> ImplicitDsString::GetAll()
{
     std::set<std::pair<std::string, std::string>> lsc;

     for ( int i = 0; i < vs.righe; ++i ) {
          if ( ( *p->v_dati ) [i] != nullptr ) {
               char * value;
               int lenv;
               value = ( char* ) vs.Get ( i, &lenv );
               int lenk;
               char * key = ( char* ) p->v_dati->Get ( i, &lenk );
               lsc.insert ( std::make_pair ( std::string ( key, lenk ), string ( value, lenv ) ) );
          }
     }

     return lsc;
}

