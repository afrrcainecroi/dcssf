//
//  mtfa_qt.h
//
//  Created by franco on 03/07/15.
//  Copyright (c) 2015 Franco Arcieri. All rights reserved.
//
#pragma once

#include <map>
#include <set>
#include <tuple>
#include <list>
#include <string>
#include <stack>
#include <memory>
#include <unordered_map>

#include "mtfa_utils.h"
#include "morton.h"

using namespace std;

//Morton, lato, valore
typedef tuple<unsigned int, unsigned int, unsigned int> t_ui3;
struct t_ui3_compare {
    bool operator() ( const t_ui3& v1, const t_ui3& v2 ) const
    {
        return get<0> ( v1 ) < get<0> ( v2 );
    }
};
typedef set<t_ui3, t_ui3_compare> T_Lqt;


typedef tuple<unsigned int, unsigned int, unsigned int, unsigned int> t_ui4;
struct t_ui4_compare {
    bool operator() ( const t_ui4& v1, const t_ui4& v2 ) const
    {
        return get<0> ( v1 ) < get<0> ( v2 );
    }
};
typedef set<t_ui4, t_ui4_compare> T_Lqt_Overlay;

//int BuildLinearQT(T_Lqt &mnodi, int ** m, int rows, int cols, int idbase);
int BuildLinearQT ( T_Lqt &mnodi, int lato, int as_r, int as_c, int bd_r, int bd_c, int idbase );
void OverlayLqt ( T_Lqt lqt0, T_Lqt lqt1, T_Lqt_Overlay & result );
bool RebuildLqt ( T_Lqt & lqt );
char ** MakeMatrix ( T_Lqt qt, unsigned int &lato );
char ** MakeMatrix ( T_Lqt_Overlay qt, unsigned int &lato );



typedef tuple<unsigned long long, unsigned long long, unsigned long long> T_ull3;
typedef list<T_ull3> T_LinearRange;
T_LinearRange BuildLinearRange ( unsigned long long inizio, unsigned long long fine, unsigned long long int id );
T_LinearRange JoinLinearRange ( T_LinearRange lr1, T_LinearRange lr2, unsigned long long &new_id, map<unsigned long long int, mtfa_bitset*> & mapping );


unsigned int MortonEncode ( unsigned short x, unsigned short y );
//unsigned int MortonEncode ( unsigned int x, unsigned int y, unsigned int z );
void MortonDecode ( unsigned int code, unsigned short &outX, unsigned short &outY );
//void MortonDecode ( unsigned int code, unsigned int &outX, unsigned int &outY, unsigned int &outZ );
unsigned int MortonAdd ( unsigned int code, unsigned short r, unsigned short c );

unsigned long long MortonEncode64 ( unsigned int x, unsigned int y );
void MortonDecode64 ( unsigned long long code, unsigned int &outX, unsigned int &outY );
unsigned long long MortonAdd64 ( unsigned long long code, unsigned int r, unsigned int c );

// unsigned int GetMortonFromRC ( unsigned int r, unsigned int c );
// void GetRC_FromMorton ( unsigned int mor, unsigned int & r, unsigned int & c );


unsigned int _pdep_u32 ( unsigned int a, unsigned int uimask );
unsigned int _pext_u32 ( unsigned int a, unsigned int uimask );
int CTZ ( unsigned int v );
bool SquareIntersect ( unsigned int r1, unsigned int c1, unsigned int l1, unsigned int r2, unsigned int c2, unsigned int l2 );
bool SquareInclude ( unsigned int r1, unsigned int c1, unsigned int l1, unsigned int r2, unsigned int c2, unsigned int l2 );
int SquareComparison ( unsigned int mor1, unsigned int l1, unsigned int mor2, unsigned int l2 );


/*
MAT-01
11|11|aa|aa
11|11|aa|aa
--|--|--|--
11|11|aa|aa
11|1W|22|2a
--|--|--|----------------
cc|c2|22|2a
cc|c2|22|2a
--|--|--|-------
cc|c2|22|2a
  |  |  |  |
*/
