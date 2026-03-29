#pragma once

#include <stdlib.h>
#include <string>

class ULL_Rng
{
public:
    ULL_Rng();

    ULL_Rng ( unsigned long long seed );

    unsigned long long Next();

    ~ULL_Rng();

private:
    //unsigned long long radice;

    unsigned long long Next ( unsigned long long seed );

    unsigned int       steps;
    unsigned long long original_seed;

public:
    long long          wlen;
    long long          r;
    long long          s;
    long long          a;
    long long          b;
    long long          c;
    long long          d;
    unsigned long long w;
    unsigned long long x[64];
    unsigned long long weil;    // = ((long long)0x61c88646<<32) +  (long long)0x80b583eb;
    unsigned long long t;
    unsigned long long v;
    int                i;       // = -1 ;                   // i < 0 indicates first call
    int                k;
};

