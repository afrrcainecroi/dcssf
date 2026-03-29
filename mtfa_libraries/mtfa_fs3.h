#pragma once

#include <stdlib.h>

#include <iostream>

#include <string>
#include <list>
#include <set>
#include <utility>

#include "mtfafs_base.h"
#include "mtfafs.h"
#include "mtfa_utils.h"

using namespace std;

//class mtfafs_s2_extended;

/* La classe che rimappo */
class ImplicitDs
{
private:
public:
    mtfafs_s2_extended * p;

public:
    CVettorePVoid vs;

    ImplicitDs();
    ~ImplicitDs();
    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );
    bool Insert ( const char* k, int len, void * s );
    //
    //Inserisce se non presente, aggiorna se presente
    bool Update ( const char * k, int len, void * s );
    bool Find ( const char * k, int len, void ** s );

    set<pair<string, void*>> GetAll();
};

class ImplicitDsString
{
private:
    //mtfafs_s2_extended * p;
    mtfafs_s2_extended * p;
    //mtfa_sequence * p;

public:
    CVettorePString vs;

    ImplicitDsString();
    ~ImplicitDsString();
    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );
    bool Insert ( char* k, int len, char * s, int lenv );
    //
    //Inserisce se non presente, aggiorna se presente
    bool Update ( char * k, int len, char * v, int lenv );
    bool Find ( char * k, int len, char ** v, int *lenv );

    std::set<std::pair<std::string, std::string>> GetAll();
};

