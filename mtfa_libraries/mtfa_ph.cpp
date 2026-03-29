#include <iostream>
#include <string>

#include <stdlib.h>

#include <list>

#include "mtfafs.h"

#include "mtfa_ph.h"

#include "mtfa_utils.h"

/* La classe che rimappo */

void PerfectHash::Serialize ( mtfa_binary_stream_save *bs )
{
    p->Serialize ( bs );
}

void PerfectHash::Deserialize ( mtfa_binary_stream_load *bs )
{
//      if ( p != nullptr )
//      {
//           delete p;
//      }
//      p = nullptr;
//      p = new mtfafs_s2_extended();
// 
     p->Deserialize ( bs );
}

PerfectHash::PerfectHash()
{
     p = new mtfafs_s2_extended();
}

PerfectHash::~PerfectHash()
{
     if ( p != nullptr ) {
         delete p;
     }
     p = nullptr;
}

int PerfectHash::Insert ( const char * k, int len )
{
     unsigned int pos = 0;
     bool bRet = p->Insert ( ( unsigned char* ) k, len, &pos );
     if ( bRet ) {
          return pos;
     }

     return -1;
}

int PerfectHash::Find ( const char *k, int len )
{
     unsigned int *index_pointer;
     unsigned int *data_pointer;
     unsigned int pos;
     if ( p->Exists ( ( const unsigned char* ) k, len, &pos, &index_pointer, &data_pointer ) ) {
          return pos;
     } else {
          return -1;
     }
}

bool PerfectHash::GetVal ( int idx, std::string & val )
{
     val.clear();
     if ( p->v_dati->righe <= idx ) {
          return false;
     }
     val.assign ( & ( *p->v_dati ) [idx][0], & ( *p->v_dati ) [idx][p->v_dati->v_len[idx]] );
     return true;
}

std::set<std::pair<int, string>> PerfectHash::GetAll()
{
     std::set<std::pair<int, string>> lsc;
     for ( int i = 0; i < p->v_dati->righe; ++i ) {
          lsc.insert ( std::make_pair ( i, string ( & ( *p->v_dati ) [i][0], & ( *p->v_dati ) [i][p->v_dati->v_len[i]] ) ) );
     }
     return lsc;
}


