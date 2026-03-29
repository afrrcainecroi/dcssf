#pragma once

#include <iostream>
#include <string>

#include <stdlib.h>

#include <list>

#include "mtfa_utils.h"

/* La classe che rimappo */
class mtfafs_s2_extended;
class mtfa_binary_stream_save;
class mtfa_binary_stream_load;


class PerfectHash
{
private:
public:
    mtfafs_s2_extended * p;
    int user_data; //a disposizione dell'utilizzatore

public:
    void Serialize ( mtfa_binary_stream_save *bs );
    void Deserialize ( mtfa_binary_stream_load *bs );
    PerfectHash();
    ~PerfectHash();
    int Insert ( const char * k, int len );
    int Find ( const char * k, int len );
    bool GetVal ( int idx, std::string & val );
    std::set<std::pair<int, string>> GetAll();
};

