#ifdef _WIN32
#include <stdlib.h>
#else
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <iostream>
#include <cstdarg>
#include <stdlib.h>

#include <memory.h>
#include <math.h>

#include <sys/timeb.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

#include "NumberTheory.h"
#include <openssl/err.h>
#include <openssl/rsa.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/random.h>
#include <gmpxx.h>

using namespace std;

//Alcune funzioni di uso molto molto comune - INIZIO


bool Utilities::PChar2String ( char* s_in, string& s_out )
{
    s_out.assign ( &s_in[0], &s_in[strlen ( s_in )] );
    return true;
}

bool Utilities::PUChar2String ( unsigned char* s_in, int len_s_in, string& s_out )
{
    s_out.assign ( &s_in[0], &s_in[len_s_in] );
    return true;
}

bool Utilities::String2PChar ( string s_in, char*& s_out )
{
    s_out = ( char* ) malloc ( ( s_in.length () + 1 ) * 1 );
    memcpy ( s_out, s_in.c_str(), s_in.length() );
    return true;
}

bool Utilities::String2PUChar ( string s_in, int& len_s_out, unsigned char*& s_out )
{
    len_s_out = s_in.length();
    s_out = ( unsigned char* ) malloc ( len_s_out + 1 );
    memcpy ( s_out, s_in.c_str(), len_s_out );
    return true;
}

bool Utilities::ToBase64 ( string s_in, string& s_out )
{
    s_out = Base64::encode ( s_in );
    return true;
}

bool Utilities::ToBase64 ( char* s_in, string& s_out )
{
    s_out = Base64::encode ( ( unsigned int ) ( strlen ( s_in ) ), ( unsigned char* ) s_in );
    return true;
}

bool Utilities::ToBase64 ( unsigned char* s_in, int len_s_in, string& s_out )
{
    s_out = Base64::encode ( len_s_in, s_in );
    return true;
}

bool Utilities::ToBase64 ( int l_in, string& s_out )
{
    string s_in = to_string ( ( long long ) l_in );
    s_out = Base64::encode ( s_in );
    return true;
}

bool Utilities::ToBase64 ( long long ll_in, string& s_out )
{
    string s_in = to_string ( ( long long ) ll_in );
    s_out = Base64::encode ( s_in );
    return true;
}

bool Utilities::ToBase64 ( char c_in, string& s_out )
{
    string s_in = "";
    s_in += c_in;
    s_out = Base64::encode ( s_in );
    return true;
}

bool Utilities::FromBase64 ( string s_in, string& s_out )
{
    s_out = Base64::decode ( s_in );
    return true;
}

bool Utilities::FromBase64 ( string s_in, char*& s_out )
{
    string tmp_s_out = Base64::decode ( s_in );
    s_out = ( char* ) malloc ( tmp_s_out.length () + 1 );
    memcpy ( s_out, tmp_s_out.c_str(), tmp_s_out.length() );
    return true;
}

bool Utilities::FromBase64 ( string s_in, unsigned char*& s_out, int& len_s_out )
{
    string tmp_s_out = Base64::decode ( s_in );
    len_s_out = tmp_s_out.length();
    s_out = ( unsigned char* ) malloc ( len_s_out + 1 );
    memcpy ( s_out, tmp_s_out.c_str(), len_s_out );
    return true;
}

bool Utilities::FromBase64 ( string s_in, int& l_out )
{
    string p = Base64::decode ( s_in );
    l_out = atol ( p.c_str() );
    return true;
}

bool Utilities::FromBase64 ( string s_in, long long& ll_out )
{
    string p = Base64::decode ( s_in );
    ll_out = stoll ( p );
    return true;
}

bool Utilities::FromBase64 ( string s_in, char& c_out )
{
    string p = Base64::decode ( s_in );
    c_out = p[0];
    return true;
}

string Utilities::PChar2String ( char* s_in )
{
    string s;
    PChar2String ( s_in, s );
    return s;
}
string Utilities::PUChar2String ( unsigned char* s_in, int len_s_in )
{
    string s;
    PUChar2String ( s_in, len_s_in, s );
    return s;
}

string Utilities::ToBase64 ( string s_in )
{
    string s;
    ToBase64 ( s_in, s );
    return s;
}

string Utilities::ToBase64 ( char* s_in )
{
    string s;
    ToBase64 ( s_in, s );
    return s;
}
string Utilities::ToBase64 ( unsigned char* s_in, int len_s_in )
{
    string s;
    ToBase64 ( s_in, len_s_in, s );
    return s;
}

string Utilities::ToBase64 ( int l_in )
{
    string s;
    ToBase64 ( l_in, s );
    return s;
}

string Utilities::ToBase64 ( long long ll_in )
{
    string s;
    ToBase64 ( ll_in, s );
    return s;
}

string Utilities::ToBase64 ( char c_in )
{
    string s;
    ToBase64 ( c_in, s );
    return s;
}


string Utilities::FromBase64ToString ( string s_in )
{
    string s;
    FromBase64 ( s_in, s );
    return s;
}

int Utilities::FromBase64ToLong ( string s_in )
{
    int l;
    FromBase64 ( s_in, l );
    return l;
}

long long Utilities::FromBase64ToLongLong ( string s_in )
{
    long long ll;
    FromBase64 ( s_in, ll );
    return ll;
}

char Utilities::FromBase64ToChar ( string s_in )
{
    char c;
    FromBase64 ( s_in, c );
    return c;
}

std::vector<string> Utilities::Split ( const string& str, const string& delimiters )
{
    std::vector<string> result;

    if ( str.size() <= 0 )
        return result;

    int pos;
    int inizio = 0;

    while ( true )
    {
        pos = str.find_first_of ( delimiters, inizio );

        if ( pos == string::npos )
        {
            result.push_back ( str.substr ( inizio ) );
            break;
        }
        else
        {
            result.push_back ( str.substr ( inizio, pos - inizio ) );
            inizio = pos + 1;
        }
    }

    return result;
}

void Utilities::Split ( std::vector<string>& result, const string& str, const string& delimiters )
{
    result = Split ( str, delimiters );
}

void Utilities::SplitBase64 ( std::vector<string>& vs, const string& str )
{
    //Prima splitta e poi passa il debase64
    vs.clear();
    Split ( vs, str, string ( ";" ) );

    for ( int i = 0; i < vs.size(); i++ )
        vs[i] = FromBase64ToString ( vs[i] );
}

string Utilities::CatenateBase64 ( std::list<string> ls_in )
{
    string result;
    string pv = ";";

    for ( auto i = ls_in.begin(); i != ls_in.end(); ++i )
    {
        result += ToBase64 ( *i ) + pv;
    }

    //Deve togliere ultimo ";"
    result.erase ( result.length() - 1, 1 );
    return result;
}

mpz_class Ops::PowMod ( mpz_class base, mpz_class esponente, mpz_class modulo )
{
    mpz_class ret;
    mpz_powm ( ret.get_mpz_t(), base.get_mpz_t(), esponente.get_mpz_t(), modulo.get_mpz_t() );
    return ret;
}
mpz_class Ops::PowMod ( mpz_class base, unsigned  int esponente, mpz_class modulo )
{
    mpz_class ret;
    mpz_powm_ui ( ret.get_mpz_t(), base.get_mpz_t(), esponente, modulo.get_mpz_t() );
    return ret;
}
mpz_class Ops::PowMod ( unsigned  int base, mpz_class esponente, mpz_class modulo )
{
    mpz_class tmp = base;
    return PowMod ( tmp, esponente, modulo );
}
mpz_class Ops::PowMod ( unsigned  int base, unsigned  int esponente, mpz_class modulo )
{
    mpz_class tmp = base;
    return PowMod ( tmp, esponente, modulo );
}

mpz_class Ops::Pow ( mpz_class base, unsigned  int esponente )
{
    mpz_class ret;
    mpz_pow_ui ( ret.get_mpz_t(), base.get_mpz_t(), esponente );
    return ret;
}

mpz_class Ops::Root ( mpz_class base, unsigned  int radice )
{
    mpz_class ret;
    mpz_root ( ret.get_mpz_t(), base.get_mpz_t(), radice );
    return ret;
}

bool Ops::IsPerfectPower ( mpz_class base )
{
    int iRet = mpz_perfect_power_p ( base.get_mpz_t() );
    return iRet != 0;
}

mpz_class Ops::Invert ( mpz_class op1, mpz_class op2 )
{
    mpz_class ret;
    mpz_invert ( ret.get_mpz_t(), op1.get_mpz_t(), op2.get_mpz_t() );
    return ret;
}

mpz_class Ops::Mod ( mpz_class op, mpz_class modulus )
{
    mpz_class ret;
    mpz_mod ( ret.get_mpz_t(), op.get_mpz_t(), modulus.get_mpz_t() );
    return ret;
}

int Ops::SizeInBase ( mpz_class num, int base )
{
    return mpz_sizeinbase ( num.get_mpz_t(), base );
}

int Ops::TestBit ( mpz_class num, int bit )
{
    return mpz_tstbit ( num.get_mpz_t(), bit );
}

mpz_class Ops::UnsignedPChar2Mpz ( unsigned char* p, unsigned  int len )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), len, 1, 1, 1, 0, p );
    return value;
}
mpz_class Ops::BinaryString2Mpz ( string sIn )
{
    mpz_class value;
    mpz_import ( value.get_mpz_t(), sIn.size(), 1, 1, 1, 0, sIn.c_str() );
    return value;
}

void Ops::Mpz2UnsignedPChar ( mpz_class value, unsigned char*& p, unsigned  int& len )
{
    unsigned char* vbytes;

    if ( value == 0 )
    {
        //in questo caso non c'� nulla da esportare. Lo faccio diventare un solo byte.
        p = ( unsigned char* ) malloc ( 1 );
        *p = 0;
        len = 1;
    }
    else
    {
        //Quanto devo allocare?
        //Order lo metto a 1, most significant word first
        //Size lo metto a 1, un byte per ogni elemento del vettore
        //Endian sempre a 1, most significant bit first
        //nails lo metto a 0, non tolgo bits dalle parole che produco
        int size = 1;
        int numb = 8 * size - 0;
        int count = ( mpz_sizeinbase ( value.get_mpz_t(), 2 ) + numb - 1 ) / numb;
        count += 16;    //per ogni evenienza!
        vbytes = ( unsigned char* ) malloc ( count * size );

        int countp = 0;

        mpz_export ( vbytes, ( size_t* ) &countp, 1, size, 1, 0, value.get_mpz_t() );

        len = countp;
        p = vbytes;
    }
}

string Ops::Mpz2BinaryString ( mpz_class value )
{
    unsigned char* vbytes;

    unsigned char* p;
    int len;

    string sRes;

    if ( value == 0 )
    {
        //in questo caso non c'� nulla da esportare. Lo faccio diventare un solo byte.
        p = ( unsigned char* ) malloc ( 1 );
        *p = 0;
        len = 1;
        sRes.assign ( &p[0], &p[1] );
    }
    else
    {
        //Quanto devo allocare?
        //Order lo metto a 1, most significant word first
        //Size lo metto a 1, un byte per ogni elemento del vettore
        //Endian sempre a 1, most significant bit first
        //nails lo metto a 0, non tolgo bits dalle parole che produco
        int size = 1;
        int numb = 8 * size - 0;
        int count = ( mpz_sizeinbase ( value.get_mpz_t(), 2 ) + numb - 1 ) / numb;
        vbytes = ( unsigned char* ) malloc ( count * size );

        int countp = 0;

        mpz_export ( vbytes, ( size_t* ) &countp, 1, size, 1, 0, value.get_mpz_t() );
        sRes.assign ( &vbytes[0], &vbytes[countp] );
    }

    return sRes;
}



string Ops::Mpz2Base64String ( mpz_class v )
{
    unsigned char* p;
    unsigned int len;
    Ops::Mpz2UnsignedPChar ( v, p, len );
    string s = Base64::encode ( len, p );
    free ( p );
    return s;
}

mpz_class Ops::Base64String2Mpz ( string s )
{
    unsigned char* p;
    unsigned int len;
    len = Base64::decode ( &p, s );
    mpz_class m = Ops::UnsignedPChar2Mpz ( p, len );
    free ( p );
    return m;
}


std::vector<string> Ops::Tokenize ( const string& str, const string& delimiters )
{
    string client = str;
    string::size_type dPos;
    string element;

    std::vector<string> result;
    result.clear();

    while ( !client.empty() )
    {
        dPos = client.find_first_of ( delimiters );

        if ( dPos == 0 )
        {
            // head is delimiter
            client = client.substr ( delimiters.length() ); // remove header delimiter
            result.push_back ( string ( "" ) );
        }
        else
        {
            // head is a real node
            dPos = client.find_first_of ( delimiters );
            element = client.substr ( 0, dPos );

            result.push_back ( element );

            if ( dPos == string::npos ) // node is last element, no more delimiter
            {
                break;
            }
            else
            {
                client = client.substr ( dPos + delimiters.length() );
            }
        }
    }

    if ( client.empty() ) // last element is delimeter
    {
        result.push_back ( string ( "" ) );
    }

    return result;
}


std::vector<string>* Ops::PTokenize ( const string& str, const string& delimiters )
{
    string client = str;
    std::vector<string>* presult = new std::vector<string>();
    std::vector<string> result;

    std::vector<string>::size_type dim = result.size();

    while ( !client.empty() )
    {
        string::size_type dPos = client.find_first_of ( delimiters );

        if ( dPos == 0 )   // head is delimiter
        {
            client = client.substr ( delimiters.length() ); // remove header delimiter
            result.push_back ( "" );
        }
        else     // head is a real node
        {
            string::size_type dPos = client.find_first_of ( delimiters );
            string element = client.substr ( 0, dPos );
            result.push_back ( element );

            if ( dPos == string::npos ) // node is last element, no more delimiter
            {
                {
                    //vStampa(result);
                    break;
                }
            }
            else
            {
                client = client.substr ( dPos + delimiters.length() );
            }
        }
    }

    if ( client.empty() ) // last element is delimeter
    {
        result.push_back ( "" );
    }

    presult->assign ( result.begin(), result.end() );
    return presult;
}


void Ops::Trim ( string& str )
{
    string::size_type pos = str.find_last_not_of ( ' ' );

    if ( pos != string::npos )
    {
        str.erase ( pos + 1 );
        pos = str.find_first_not_of ( ' ' );

        if ( pos != string::npos )
            str.erase ( 0, pos );
    }
    else
        str.erase ( str.begin(), str.end() );
}


string Ops::RandomBits ( int numOfbit )
{
    Random r;
    return r.getNbits ( numOfbit ).get_str ( 62 );
}

//Alcune funzioni di uso molto molto comune - FINE

bool    Random::done = false;
gmp_randclass* Random::r_rand = nullptr;

void Random::StrongRandom ( int bits, mpz_class& mret )     //usa /dev/urandom di unix e usa i lcrypto di microsoft
{
	int len = bits>>3;
	if ((len<<3)!=bits)
		len++;
	unsigned char * pbData  = ( unsigned char* ) malloc ( len ); //[64];
	
#ifdef _WIN32
    HCRYPTPROV hCryptProv = NULL;        // handle for a cryptographic
    if ( CryptAcquireContext (
                &hCryptProv,               // handle to the CSP
                NULL,                     // container name
                NULL,                      // use the default provider
                PROV_RSA_FULL,             // provider type
                CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) )                      // flag values
    {
        if ( !CryptGenRandom ( hCryptProv, len, pbData ) )
        {
            //devo riempire in altro modo.
			mtfa_fastrng1024* r = new mtfa_fastrng1024( Ticks::get_micro() );

            for ( int i = 0; i < len; ++i )
            {
                pbData[i] = r->Next() & 0x00000000000000FF;
            }

            delete ( r );
        }

        CryptReleaseContext ( hCryptProv, 0 );
    }

#else
    {
        //faccio un vettore di bytes lungo 64 usando /dev/urandom
// 		int len = bits>>3;
// 		if ((len<<3)!=bits)
// 			len++;
//         pbData  = ( unsigned char* ) malloc ( len ); //[64];
		//getrandom(pbData, len);
		syscall(SYS_getrandom, pbData, len, 0);
    }
#endif
	mret = Ops::UnsignedPChar2Mpz(pbData, len);
    //mpz_setbit(mret.get_mpz_t(), 0);
	mpz_setbit(mret.get_mpz_t(), bits-1);

    if ( pbData )
    {
        free ( pbData );
    }
}


Random::Random()
{
    Init();
}
void Random::Init()
{
    if ( Random::done )
        return;

    Random::r_rand = new  gmp_randclass ( gmp_randinit_mt ); //nullptr; //(gmp_randinit_mt);
    Random::done = true;
    bool generato = false;
    string val;
    mpz_class num;

    Random::StrongRandom ( 8 * 64, num );
    //cerr  << "Generato il seme casuale: " << num << endl;
    Random::r_rand->seed ( num );
};

Random::~Random()
{
};

mpz_class Random::getNbits ( unsigned int n )
{
    return Random::r_rand->get_z_bits ( n );
};

mpz_class Random::getNbits ( mpz_class n )
{
    return Random::r_rand->get_z_bits ( n );
};

mpz_class Random::getNrange ( mpz_class n )
{
    return Random::r_rand->get_z_range ( n );
};

mpz_class NextPrime ( mpz_class & a );
mpz_class NextPrime ( mpz_class & a )
{
    mpz_nextprime(a.get_mpz_t(), a.get_mpz_t());
    return a;
// ciclo:
//     Random::StrongRandom ( mpz_sizeinbase(a.get_mpz_t(), 2), a );
// 	mpz_clrbit(a.get_mpz_t(), 0);
//     a = a*6+1;
// 	if (mpz_probab_prime_p(a.get_mpz_t(), 25)) {
//         return a;
//     }
//     a = a - 2;
// 	if (mpz_probab_prime_p(a.get_mpz_t(), 25)) {
//         return a;
//     }
//     goto ciclo;
}

mpz_class NextPrime43 ( mpz_class & a, int numbits );
mpz_class NextPrime43 ( mpz_class & a, int numbits )
{
//    mpz_nextprime(a.get_mpz_t(), a.get_mpz_t());
//    return a;
ciclo:
    Random::StrongRandom ( numbits, a );
	mpz_clrbit(a.get_mpz_t(), 0);
    a = a*6+1;
	if (mpz_probab_prime_p(a.get_mpz_t(), 25)) {
        return a;
    }
    a = a - 2;
	if (mpz_probab_prime_p(a.get_mpz_t(), 25)) {
        return a;
    }
    goto ciclo;
}

void NextPrime ( mpz_t a );
void NextPrime ( mpz_t a )
{
    mpz_nextprime(a, a);
    return;
    /*
	mpz_class t;
	Random::StrongRandom ( mpz_sizeinbase(a, 2), t );
	mpz_setbit(t.get_mpz_t(), 0);
	while (!mpz_probab_prime_p(t.get_mpz_t(), 25))
	{
		Random::StrongRandom ( mpz_sizeinbase(a, 2), t );
		mpz_setbit(t.get_mpz_t(), 0);
	}
	mpz_set(a, t.get_mpz_t());
    */
}

BlumInteger::BlumInteger()
{
    assigned_m = false;
    assigned_p = false;
    assigned_q = false;
};

BlumInteger::~BlumInteger()
{
    assigned_m = false;
    assigned_p = false;
    assigned_q = false;
};

///New version
bool BlumInteger::GenerateBlumInt ( int bits, unsigned long long& p, unsigned long long& q, unsigned long long& m )
{
    //li genera come numeri long long!!!
    if ( bits > 32 )
        return false;

    BlumInteger* bl = new  BlumInteger ( bits );
    string sm = bl->getM().get_str();
    string sp = bl->getP().get_str();
    string sq = bl->getQ().get_str();

    m = std::stoull ( sm );
    p = std::stoull ( sp );
    q = std::stoull ( sq );
    return true;
}

BlumInteger::BlumInteger ( mpz_class & base_p, mpz_class & base_q )
{
	//i calcoli li ha fatti qulcun altro
	p = base_p;
	q = base_q;
	m = p * q;
	assigned_m = true;
	assigned_p = true;
	assigned_q = true;
}

static mpz_class getGcd(mpz_class a, mpz_class b)
{
	mpz_class gcd;
	mpz_gcd(gcd.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
	return gcd;
}

BlumInteger::BlumInteger ( int num_bits )
{
	//Per partire li prendiamo da StrongRandom
 	Random::StrongRandom ( num_bits, p );
    Random::StrongRandom ( num_bits, q );
	
    //cerca due primi che soddisfano le richieste di blum blum!
	//potrei anche cercare nel seguente modo: 6n+1 e 6n-1 potrebbero essere primi, quindi 
	//parto da p di n-2 bit, lo moltiplico per 6 e poi esamino il -1 e il -2
	
	//primo tentativo, lo faccio in modo random
//	p=4; q=4;
	mpz_setbit(p.get_mpz_t(), num_bits-1);
	mpz_setbit(q.get_mpz_t(), num_bits-1);
	NextPrime43(p, num_bits);
	NextPrime43(q, num_bits);
    
    bool alterna = false;

    while ( true )
    {
// 		while ((p % 4) != 3)
// 		{
// 			NextPrime ( p, num_bits );
// 		}
// 		while ((q % 4) != 3)
// 		{
// 			NextPrime ( q, num_bits );
// 		}
		
		if (getGcd(p-1, q-1) > 2)
        {
            if (alterna) {
                alterna = !alterna;
                NextPrime43 ( p, num_bits );
            } else {
                alterna = !alterna;
                NextPrime43 ( q, num_bits );
            }
            continue;
        }

        if ( p == q )
        {
            if (alterna) {
                alterna = !alterna;
                NextPrime43 ( p, num_bits );
            } else {
                alterna = !alterna;
                NextPrime43 ( q, num_bits );
            }
            continue;
        }
        break;
    }

    //ora che abbiamo p e q, archiviamo anche m;
    m = p * q;
    assigned_m = true;
    assigned_p = true;
    assigned_q = true;
}


mpz_class BlumInteger::getM()
{
    if ( !assigned_m )
    {
        //const string messaggio = "Errore, m non e' stato assegnato.";
        //throw std::exception();
        return mpz_class ( 0 );
    }

    return m;
}

mpz_class BlumInteger::getP()
{
    if ( !assigned_p )
    {
        //string messaggio = "Errore, p non e' stato assegnato.";
        //throw std::exception();
        return mpz_class ( 0 );
    }

    return p;
}

mpz_class BlumInteger::getQ()
{
    if ( !assigned_q )
    {
        //string messaggio = "Errore, q non e' stato assegnato.";
        //throw std::exception();
        return mpz_class ( 0 );
    }

    return q;
}

mpz_class BlumInteger::getPm1()
{
    if ( !assigned_p )
    {
        //string messaggio = "Errore, p non e' stato assegnato.";
        //throw std::exception();
        return mpz_class ( 0 );
    }

    return p - 1;
}

mpz_class BlumInteger::getQm1()
{
    if ( !assigned_q )
    {
        //string messaggio = "Errore, q non e' stato assegnato.";
        //throw std::exception();
        return mpz_class ( 0 );
    }

    return q - 1;
}

void BlumInteger::setM ( mpz_class value )
{
    assigned_m = true;
    m = value;
}

void BlumInteger::setP ( mpz_class value )
{
    assigned_p = true;
    p = value;
}

void BlumInteger::setQ ( mpz_class value )
{
    assigned_q = true;
    q = value;
}

void BlumInteger::setM ( const char* value, int base )
{
    assigned_m = true;
    mpz_class gmp_val;
    gmp_val.set_str ( value, base );
    m = gmp_val;
}

void BlumInteger::setP ( const char* value, int base )
{
    assigned_p = true;
    mpz_class gmp_val;
    gmp_val.set_str ( value, base );
    p = gmp_val;
}

void BlumInteger::setQ ( const char* value, int base )
{
    assigned_q = true;
    mpz_class gmp_val;
    gmp_val.set_str ( value, base );
    q = gmp_val;
}


mpz_class BlumInteger::getPm1Qm1()
{
    if ( !assigned_p )
        return mpz_class ( 0 );

    //throw std::exception("Errore, p non � stato assegnato.");
    if ( !assigned_q )
        return mpz_class ( 0 );

    //throw std::exception("Errore, q non � stato assegnato.");

    mpz_class pm1 = p - 1;
    mpz_class qm1 = q - 1;
    return pm1 * qm1;
}


//mpz_class zero    = 0;
//mpz_class one = 1;
//mpz_class two = 2;
//mpz_class three   = 3;
//mpz_class four    = 4;
//mpz_class eight   = 8;

//BlumAlgo::BlumAlgo()
//{
//  zero = 0;
//  one = 1;
//  two = 2;
//  three = 3;
//  four = 4;
//  eight = 8;
//
//    //rseed = new    GMP.Random();
//    //rseed.Seed(new    System.Random((int)DateTime.Now.Ticks).Next());
//}

/// <summary>
/// Torna un numero casuale compreso tra i due estremi, inclusi
/// </summary>
/// <param name="lower"></param>
/// <param name="upper"></param>
/// <returns></returns>
mpz_class BlumAlgo::Rand ( mpz_class lower, mpz_class upper )
{
    //Devo generare un numero casuale tra lower e upper, estremo inferiore incluso
    Random r;
    return r.getNrange ( upper - lower ) + lower;
}

/// <summary>
/// � una funzione random molto potente, usabile per la crittografia. Genera un numero casuale di numBits bits
/// </summary>
/// <param name="numBits"></param>
/// <returns></returns>
mpz_class BlumAlgo::Rand ( int numBits )
{
    Random r;
    return r.getNbits ( numBits );
}


mpz_class BlumAlgo::getI_esimo_fast ( mpz_class x, mpz_class i, BlumInteger& blum_i )
{
    //Calcolo x alla (due alla i mod pm1_qm1) mod m
    if ( i <= 0 )
        return x;

    if ( blum_i.getPm1Qm1() == 0 )
        return mpz_class ( 0 );
    else
        return Ops::PowMod ( x, Ops::PowMod ( 2, i, blum_i.getPm1Qm1() ), blum_i.getM() );

    //mpz_class tmp;
    //mpz_powm(tmp.get_mpz_t(), two.get_mpz_t(), i.get_mpz_t(), blum_i.getPm1Qm1().get_mpz_t());

    //mpz_class ret;
    //mpz_powm(ret.get_mpz_t(), x.get_mpz_t(), tmp.get_mpz_t(), blum_i.getM().get_mpz_t());
    //return ret;
}

mpz_class BlumAlgo::getI_esimo_fast ( mpz_class x, int i, BlumInteger& blum_i )
{
    if ( i <= 0 )
        return x;

    //Calcolo x alla (due alla i mod pm1_qm1) mod m
    if ( blum_i.getPm1Qm1() == 0 )
        return mpz_class ( 0 );
    else
        return Ops::PowMod ( x, Ops::PowMod ( 2, i, blum_i.getPm1Qm1() ), blum_i.getM() );

    //mpz_class i_mpz = i;

    //mpz_class tmp;
    //mpz_powm(tmp.get_mpz_t(), two.get_mpz_t(), i_mpz.get_mpz_t(), blum_i.getPm1Qm1().get_mpz_t());

    //mpz_class ret;
    //mpz_powm(ret.get_mpz_t(), x.get_mpz_t(), tmp.get_mpz_t(), blum_i.getM().get_mpz_t());
    //return ret;
}

mpz_class BlumAlgo::getI_esimo_slow ( mpz_class x, int indice, BlumInteger& blum_i )
{
    if ( indice <= 0 )
        return x;

    unsigned  int step = ( unsigned  int ) sqrtl ( indice );
    //step = 10000000;
    unsigned  int potenza = 0;

    mpz_class ret = x;
    mpz_class tmp;

    mpz_class two = 2;

    mpz_class twoAllaStep = Ops::Pow ( two, step );

    mpz_t p_twoAllaStep;
    mpz_init ( p_twoAllaStep );
    mpz_set ( p_twoAllaStep, twoAllaStep.get_mpz_t() );

    mpz_t p_m;
    mpz_init ( p_m );
    mpz_set ( p_m, blum_i.getM().get_mpz_t() );

    mpz_t p_ret;
    mpz_init ( p_ret );
    mpz_set ( p_ret, ret.get_mpz_t() );

    for ( unsigned int i = 0; ( i + step ) < indice; i += step )
    {
        potenza += step;

        mpz_powm ( p_ret, p_ret, p_twoAllaStep, p_m );
        //ret = Ops::PowMod(ret, twoAllaStep, blum_i.getM());

        //mpz_pow_ui(dueAstep.get_mpz_t(), two.get_mpz_t(), step);
        //
        //mpz_powm(tmp.get_mpz_t(), ret.get_mpz_t(), dueAstep.get_mpz_t(), blum_i.getM().get_mpz_t());
        //ret = tmp;
    }

    mpz_set ( ret.get_mpz_t(), p_ret );

    mpz_clear ( p_m );
    mpz_clear ( p_ret );
    mpz_clear ( p_twoAllaStep );

    if ( potenza < indice )
    {
        ret = Ops::PowMod ( ret, Ops::Pow ( two, indice - potenza ), blum_i.getM() );
        //mpz_pow_ui(dueAstep.get_mpz_t(), two.get_mpz_t(), indice-potenza);
        //mpz_powm(tmp.get_mpz_t(), ret.get_mpz_t(), dueAstep.get_mpz_t(), blum_i.getM().get_mpz_t());
        //ret = tmp;
    }

    return ret;
}

mpz_class BlumAlgo::getK_Previous ( mpz_class y, unsigned  int L, BlumInteger& blum_i )
{
    if ( L <= 0 )
        return y;

    if ( blum_i.getP() == 0 )
        return mpz_class ( 0 );

    if ( blum_i.getQ() == 0 )
        return mpz_class ( 0 );

    //Formula modificata. Usa i moduli con p-1 e q-1
    mpz_class rp, rq;

    mpz_class tmp, ret;

    rp = Ops::PowMod ( y, Ops::PowMod ( ( blum_i.getP() + 1 ) / 4, L, blum_i.getP() - 1 ), blum_i.getP() );
    rq = Ops::PowMod ( y, Ops::PowMod ( ( blum_i.getQ() + 1 ) / 4, L, blum_i.getQ() - 1 ), blum_i.getQ() );

    mpz_class r1, r2;

    r1 = Ops::Invert ( blum_i.getQ(), blum_i.getP() );
    r2 = Ops::Invert ( blum_i.getP(), blum_i.getQ() );

    r1 *= blum_i.getQ() * rp;
    r2 *= blum_i.getP() * rq;

    return Ops::Mod ( r1 + r2, blum_i.getM() );
}

mpz_class BlumAlgo::getNext ( mpz_class previous, BlumInteger& blum_i )
{
    return Ops::PowMod ( previous, 2, blum_i.getM() );
    //mpz_class ret;
    //mpz_pow_ui(ret.get_mpz_t(),
//   return GMP.Integer.Power(previous, 2, m); // previous.ModPow(GMP.Integer.Two, _m);
//   //mpz_powm_ui (next.get_mpz_t(), previous.get_mpz_t(), 2, M.get_mpz_t());
//   //return next;
}


unsigned long long modmup ( unsigned long long a, unsigned long long b, unsigned long long m );
unsigned long long modmup ( unsigned long long a, unsigned long long b, unsigned long long m )
{
    if ( a > m )
        a = a % m;

    if ( b > m )
        b = b % m;

    unsigned long long ret = 0;

    if ( a < b )
        std::swap ( a, b );

    while ( b )
    {
        while ( a < m )
        {
            if ( b & 1 )
                ret += a;

            a <<= 1;
            b >>= 1;
        }

        a -= m;

        while ( ret >= m )
            ret -= m;

        if ( a < b )
            std::swap ( a, b );
    }

    return ret;
};


unsigned long long BlumAlgo::getNext ( unsigned long long previous, unsigned long long m )
{
    return modmup ( previous, previous, m );
}


string BlumAlgo::getBase64Next ( string previousBase64, BlumInteger& blum_i )
{
    mpz_class prev = Ops::Base64String2Mpz ( previousBase64 );
    return Ops::Mpz2Base64String ( Ops::PowMod ( prev, 2, blum_i.getM() ) );
}

string BlumAlgo::getBase64Previous ( string nextBase64, BlumInteger& blum_i )
{
    mpz_class prox = Ops::Base64String2Mpz ( nextBase64 );
    return Ops::Mpz2Base64String ( Ops::PowMod ( prox, ( blum_i.getPm1Qm1() + 4 ) / 8, blum_i.getM() ) );
}

string BlumAlgo::expModMva ( string base10base, string base10esponente, BlumInteger& blum_i )
{
    mpz_class base, exp, result;
    base.set_str ( base10base, 10 );
    exp.set_str ( base10esponente, 10 );
    result = Ops::PowMod ( base, exp, blum_i.getM() );
    return Ops::Mpz2Base64String ( result );
}


mpz_class BlumAlgo::getPrevious ( mpz_class next, BlumInteger& blum_i )
{
    return Ops::PowMod ( next, ( blum_i.getPm1Qm1() + 4 ) / 8, blum_i.getM() );
}


// void BlumAlgo::FastCifraDecifra(int & passiFatti, unsigned char * ciphered,  int lenCiphered, mpz_class & bxip1, BlumInteger & blum_i)
// {
//     passiFatti = 0; //per tornare quanti passi ho fatto qui
//
//     //Ora posso cifrare... Con il nuovo modello...
//     unsigned char * toXor = (unsigned char*)malloc (lenCiphered);
//     unsigned  int posToXor=0;
//     passiFatti = 0;
//
//     while (posToXor != lenCiphered)
//     {
//         unsigned char * v;
//         unsigned int lenV;
//         Ops::Mpz2UnsignedPChar(bxip1, v, lenV);
//
//         if ((lenV-1+posToXor)<lenCiphered)
//         {
//             //Copio tutto
//             memcpy(&toXor[posToXor], &v[1], lenV-1);
//             posToXor += lenV-1;
//         }
//         else
//         {
//             memcpy(&toXor[posToXor], &v[1], lenCiphered-posToXor);
//             posToXor = lenCiphered;
//         }
//         free(v);
//         bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//         passiFatti++;
//     }
//
//     //Ora posso fare direttamente lo xor tra i due vettori.
//     unsigned char *p_ciphered = ciphered;
//     unsigned char * p_toXor = toXor;
//
//     for (int i=0; i < lenCiphered; i++)
//         *p_ciphered++ ^= *p_toXor++;
//
//     free(toXor);
//
//     return;
// }
//
// ///Ricorda che s_t � una stringa qualsiasi, senza restrizioni se non il fatto di essere una stringa!
// bool BlumAlgo::Cipher_Classical_old(string s_t, string & sBase64_ciphered, string & sBase64_xip1, int oneBit, bool compress, BlumInteger & blum_i)
// {
//     //Per prima cosa comprimo o codifico la stringa in modo da farla diventare una sequenza di bytes.
//     string ciphered;
//
//     compress=false;
//     ciphered = s_t;
//
//     sBase64_xip1.clear();
//
//     //Range del numero casuale. Da m^2/3 a m-m^2/3
//     mpz_class m_2 = Ops::Pow(blum_i.getM(), 2); //.Pow(2);
//
//     //Calcolo della radice cubica di rand_min_2
//     mpz_class m_2_3 = Ops::Root(m_2, 3);
//
//     mpz_class rand_max = blum_i.getM() - m_2_3;
//     mpz_class rand_min = m_2_3;
//
//     //Ora posso generare un numero casuale compreso tra minimo e massimo
//     mpz_class r = Rand(rand_min, rand_max);
//
//     //Ora che ho un numero casuale, n bit alla volta devo cifrare la nostra stringa
//     int nbits = 1;
//     if (oneBit==1)
//         nbits = 1;           //Uso un solo bit
//     else if (oneBit==0)
//     {
//         //Uso loglog n bit
//         double log2_log2_bitlen_m = log((double)Ops::SizeInBase(blum_i.getM(), 2))/log(2.0);
//         nbits = (int)ceilf(log2_log2_bitlen_m);
//     }
//     else if (oneBit==2)
//     {
//         //Uso tutti i bit
//         nbits = Ops::SizeInBase(blum_i.getM(), 2);
//     }
//
//     mpz_class bxip1 = getNext(r, blum_i);
//
//     int totbits = ciphered.size() * 8;
//     int currbit = 0;
//     while (currbit < totbits)
//     {
//         for (int i = 0; i < nbits; i++)
//         {
//             if (currbit < totbits)
//             {
//                 ciphered[currbit / 8] = (unsigned char)(ciphered[currbit / 8] ^ (unsigned char)(((Ops::TestBit(bxip1,i) == 0) ? 0 : 1) << (currbit % 8)));
//                 currbit++;
//             }
//             else
//                 break;
//         }
//         bxip1 = getNext(bxip1, blum_i);
//     }
//
//     //Messaggio cifrato! Convertiamo la codifica in due stringhe in base64!
//     sBase64_ciphered = Base64::encode(ciphered);
//     sBase64_xip1 = Base64::encode(bxip1);
//     return true;
// }
//
// bool BlumAlgo::Decipher_Classical_old(string & s_t, string sBase64_ciphered, string sBase64_xip1, int oneBit, bool compress, BlumInteger & blum_i)
// {
//     //Faccio al contrario. Per prima cosa torno indietro di tanti passi quanti ne ho fatti in avanti
//     T_AtomBase t;
//     T_AtomBase ciphered = Ops::Base64String2Vector(sBase64_ciphered);
//     //T_AtomBase xip1     = Ops::MpzString2Mpz2Vector(s_xip1);
//
//     int nbits = 1;
//     if (oneBit==1)
//         nbits = 1;
//     else if (oneBit == 0)
//     {
//         double log2_log2_bitlen_m = log((double)Ops::SizeInBase(blum_i.getM(), 2))/log(2.0);
//         nbits = (int)ceilf(log2_log2_bitlen_m);
//     }
//     else if (oneBit == 2)
//     {
//         nbits = Ops::SizeInBase(blum_i.getM(), 2);
//     }
//
//     int back = 0;
//     t.clear();
//
//     //Cos� sono sicuro di calcolare esattamente il numero di passi in avanti che ho fatto. Torno indietro dello stesso numero.
//
//     int totbits = ciphered.size() * 8;
//     int currbit = 0;
//
//     back = (int)((totbits / nbits) + (((totbits % nbits) == 0) ? 0 : 1));
//
//     mpz_class bxip1;
//     bxip1 = Ops::Base64String2Mpz(sBase64_xip1);// = Ops::Vector2Mpz(xip1);
//     mpz_class x0 = getK_Previous(bxip1, back, blum_i);
//
//     totbits = (int)ciphered.size() * 8;
//     currbit = 0;
//
//     while (currbit < totbits)
//     {
//         for (int i = 0; i < nbits; i++)
//         {
//             if (currbit < totbits)
//             {
//                 ciphered[currbit / 8] = (unsigned char)(ciphered[currbit / 8] ^ (unsigned char)(((Ops::TestBit(x0, i) == 0) ? 0 : 1) << (currbit % 8)));
//                 currbit++;
//             }
//             else
//                 break;
//         }
//         x0 = getNext(x0, blum_i);
//     }
//
//     //Messaggio decifrato!
//     compress=false;
//     //  if (compress)
//     //t = C_Compress::Decompress(ciphered);
//     //  else
//     t = ciphered;
//
//     s_t = Ops::Vector2String(ciphered);
//     return true;
// }
//
// bool BlumAlgo::Cipher_Classical(string s_t, string & s_ciphered, string & s_xip1, int & steps, BlumInteger & blum_i)
// {
//     //Per prima cosa comprimo o codifico la stringa in modo da farla diventare una sequenza di bytes.
//
//     int lenCiphered = s_t.size();
//     unsigned char * ciphered = (unsigned char*)malloc (lenCiphered);
//     memcpy(ciphered, s_t.c_str(), lenCiphered);
// //   T_AtomBase ciphered = Ops::String2Vector(s_t);
//
// //    s_xip1.clear();
//
//     //Range del numero casuale. Da m^2/3 a m-m^2/3
//     mpz_class m_2 = Ops::Pow(blum_i.getM(), 2); //.Pow(2);
//
//     //Calcolo della radice cubica di rand_min_2
//     mpz_class m_2_3 = Ops::Root(m_2, 3);
//
//     mpz_class rand_max = blum_i.getM() - m_2_3;
//     mpz_class rand_min = m_2_3;
//
//     //Ora posso generare un numero casuale compreso tra minimo e massimo
//     mpz_class r = BlumAlgo::Rand(rand_min, rand_max);
//
//     //Ora che ho un numero casuale, n bit alla volta devo cifrare la nostra stringa
//     //Uso tutti i bit
// //   int nbits = Ops::SizeInBase(blum_i.getM(), 2);
//
//     mpz_class bxip1 = BlumAlgo::getNext(r, blum_i);
//     steps=0;
//     FastCifraDecifra(steps, ciphered, lenCiphered, bxip1, blum_i);
//
//     ////Ma se facessi un vettore identico all'originale con i bytes di bxipi?
//     //T_AtomBase toXor; toXor.reserve(ciphered.size());
//     //int fatti = 0;
//     //int cipheredSize = ciphered.size();
//     //while (fatti!=cipheredSize)
//     //{
//     //   T_AtomBase v = Ops::Mpz2Vector(bxip1);
//     //   if ((v.size()-1+fatti)<cipheredSize)
//     //   {
//     //       T_AtomBase::iterator inizio = v.begin(); inizio++;  //Dal secondo, per eliminare i problemi
//     //       T_AtomBase::iterator fine = v.end();
//     //       toXor.insert(toXor.end(), inizio, fine);
//     //       //std::copy(inizio, fine, toXor.end());
//     //   }
//     //   else
//     //   {
//     //       T_AtomBase::iterator inizio = v.begin(); inizio++;  //Dal secondo, per eliminare i problemi
//     //       T_AtomBase::iterator fine = inizio+(cipheredSize-fatti);
//     //       toXor.insert(toXor.end(), inizio, fine);
//     //       //std::copy(inizio, fine, toXor.end());
//     //   }
//     //   fatti=toXor.size();
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);  //per sicurezza di cifra e per non far leggere una parte del dato cifrato!
//     //   steps++;
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //for (int i=0; i < cipheredSize; i++)
//     //   ciphered[i] ^= toXor[i];
//
//     //Messaggio cifrato! Convertiamo la codifica in un array di bytes.
//     s_ciphered = Base64::encode(lenCiphered, ciphered);// Ops::Vector2Base64String(ciphered);
//     free(ciphered);
//     s_xip1 = Ops::Mpz2Base64String(bxip1);
//     return true;
// }
//
// bool BlumAlgo::Square(string toBeSquared, string & squared, BlumInteger & blum_i)
// {
//     squared = BlumAlgo::getBase64Next(toBeSquared, blum_i);
//     return true;
// }
// bool BlumAlgo::Unsquare(string toBeUnsquared, string & unsquared, BlumInteger & blum_i)
// {
//     unsquared = BlumAlgo::getBase64Previous(toBeUnsquared, blum_i);
//     return true;
// }
//
// bool BlumAlgo::Decipher_Classical(string & s_t, string s_ciphered, string s_xip1, int steps, BlumInteger & blum_i)
// {
//     //Faccio al contrario. Per prima cosa torno indietro di tanti passi quanti ne ho fatti in avanti
//     //T_AtomBase ciphered = Ops::Base64String2Vector(s_ciphered);
//
//     unsigned char * ciphered;
//     int lenCiphered = Base64::decode(&ciphered, s_ciphered);
//
//     mpz_class bxip1 = Ops::Base64String2Mpz(s_xip1);
//     bxip1 = BlumAlgo::getK_Previous(bxip1, steps, blum_i);   //Questo lo ho usato. Ora conto tanti passi indietro finch� non arrivo a un numero di bit coperti parti a totbit
//
//     int passiFatti = 0;
//     FastCifraDecifra(passiFatti, ciphered, lenCiphered, bxip1, blum_i);
//     ////Ma se facessi un vettore identico all'originale con i bytes di bxipi?
//     //T_AtomBase toXor; toXor.reserve(ciphered.size());
//     //int fatti = 0;
//     //int cipheredSize = ciphered.size();
//     //while (fatti!=cipheredSize)
//     //{
//     //   T_AtomBase v = Ops::Mpz2Vector(bxip1);
//     //   if ((v.size()-1+fatti)<cipheredSize)
//     //   {
//     //       T_AtomBase::iterator inizio = v.begin(); inizio++;  //Dal secondo, per eliminare i problemi
//     //       T_AtomBase::iterator fine = v.end();
//     //       toXor.insert(toXor.end(), inizio, fine);
//     //   }
//     //   else
//     //   {
//     //       T_AtomBase::iterator inizio = v.begin(); inizio++;  //Dal secondo, per eliminare i problemi
//     //       T_AtomBase::iterator fine = inizio+(cipheredSize-fatti);
//     //       toXor.insert(toXor.end(), inizio, fine);
//     //   }
//     //   fatti=toXor.size();
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);  //per sicurezza di cifra e per non far leggere una parte del dato cifrato!
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //for (int i=0; i<cipheredSize; i++)
//     //   ciphered[i] ^= toXor[i];
//
//     s_t.assign(&ciphered[0], &ciphered[lenCiphered]);  //Ops::Vector2String(ciphered);
//     free(ciphered);
//
// //   Ops::Mpz2Vector(Ops::Vector2Mpz(Ops::String2Vector(s_t)));
//
//     return true;
// }
//
//
// bool BlumAlgo::VaCipher_old(string & sBase64_ciphered, string toBeCiphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //Per prima cosa comprimo o codifico la stringa in modo da farla diventare una sequenza di bytes.
//     T_AtomBase t = Ops::String2Vector(toBeCiphered);
//     T_AtomBase ciphered;
//
//
//     //  if (compress)
//     //ciphered = C_Compress::Compress(t);
//     //  else
//     ciphered = t;
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);// r.set_str(seedToUse, 62);  //Questo � il seed che devo usare in partenza
//
//     //Ora che ho un numero casuale, n bit alla volta devo cifrare la nostra stringa. Uso la tecnica logaritmica dei bit.
//     int nbits = 1;
//     double log2_log2_bitlen_m = log((double)Ops::SizeInBase(blum_i.getM(), 2))/log(2.0);
//     nbits = (int)ceilf(log2_log2_bitlen_m);
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit. L'ultimo non so!
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//     string esponente_base_2_1 = esponente_base_due.substr(0,16);
//     string esponente_base_2_2 = esponente_base_due.substr(16,16);
//     string esponente_base_2_3 = esponente_base_due.substr(32, esponente_base_due.size()-32);
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     mpz_class bxip1;
//
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//
//
//     //Per risolvere un petit probl�me di loop
//     if (bxip1 < 2)
//         bxip1 = 2;
//
//
//     //ora aggiungo i passi in pi� che devo fare...
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(bxip1, passiDaFare, blum_i);
//     }
//
//     passiFatti = 0; //per tornare quanti passi ho fatto qui
//     int totbits = ciphered.size() * 8;
//     int currbit = 0;
//     while (currbit < totbits)
//     {
//         for (int i = 0; i < nbits; i++)
//         {
//             if (currbit < totbits)
//             {
//                 ciphered[currbit / 8] = (unsigned char)(ciphered[currbit / 8] ^ (unsigned char)(((Ops::TestBit(bxip1,i) == 0) ? 0 : 1) << (currbit % 8)));
//                 currbit++;
//             }
//             else
//                 break;
//         }
//         bxip1 = getNext(bxip1, blum_i);
//         passiFatti++;
//     }
//
//     //Messaggio cifrato! Convertiamo la codifica in una base64 string!
//     sBase64_ciphered = Ops::Vector2Base64String(ciphered);
//     //s_xip1 = bxip1.get_str(62);    //Questo non mi serve in quanto conosco da dove sono partito!!!
//     return true;
// }
//
// bool BlumAlgo::VaDecipher_old(string & s_deciphered, string sBase64_toBeDeciphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //Per prima cosa riprendo il vettore della stringa cifrata
//     T_AtomBase t = Ops::Base64String2Vector(sBase64_toBeDeciphered);
//     T_AtomBase deciphered;
//
//     //  if (compress)
//     //deciphered = C_Compress::Decompress(t);
//     //  else
//     deciphered = t;
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);  //Questo � il seed che devo usare in partenza
//
//     //Ora che ho un numero casuale, n bit alla volta devo cifrare la nostra stringa. Uso la tecnica logaritmica dei bit.
//     int nbits = 1;
//     double log2_log2_bitlen_m = log((double)Ops::SizeInBase(blum_i.getM(), 2))/log(2.0);
//     nbits = (int)ceilf(log2_log2_bitlen_m);
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit, tranne l'ultimo che non so...
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//     string esponente_base_2_1 = esponente_base_due.substr(0,16);
//     string esponente_base_2_2 = esponente_base_due.substr(16,16);
//     string esponente_base_2_3 = esponente_base_due.substr(32,esponente_base_due.size()-32);
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     //Sono nella sezione server, posso, anzi, devo utilizzare gli algoritmi veloci!!
//
//     mpz_class bxip1;
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//
//     int totbits = deciphered.size() * 8;
//     int currbit = 0;
//     passiFatti = 0;
//     while (currbit < totbits)
//     {
//         for (int i = 0; i < nbits; i++)
//         {
//             if (currbit < totbits)
//             {
//                 deciphered[currbit / 8] = (unsigned char)(deciphered[currbit / 8] ^ (unsigned char)(((Ops::TestBit(bxip1,i) == 0) ? 0 : 1) << (currbit % 8)));
//                 currbit++;
//             }
//             else
//                 break;
//         }
//         bxip1 = getNext(bxip1, blum_i);
//         passiFatti++;
//     }
//
//     //Messaggio decifrato! Convertiamo la codifica in una stringa.
//     s_deciphered = Ops::Vector2String(deciphered);
//     //s_xip1 = bxip1.get_str(62);    //Questo non mi serve in quanto conosco da dove sono partito!!!
//     return true;
// }
//
//
// bool BlumAlgo::VaCipher(string & sBase64_ciphered, string toBeCiphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //Per prima cosa comprimo o codifico la stringa in modo da farla diventare una sequenza di bytes.
//     int lenCiphered = toBeCiphered.size();
//     unsigned char * ciphered = (unsigned char*)malloc (lenCiphered);
//     memcpy(ciphered, toBeCiphered.c_str(), lenCiphered); //Ora ho una stringa, non terminante per 0
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);// r.set_str(seedToUse, 62);  //Questo � il seed che devo usare in partenza
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit. L'ultimo non so!
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//
//     string esponente_base_2_1;
//     string esponente_base_2_2;
//     string esponente_base_2_3;
//
//     if (esponente_base_due.length()>=16)
//     {
//         esponente_base_2_1 = esponente_base_due.substr(0,16);
//         if (esponente_base_due.length()>=32)
//         {
//             esponente_base_2_2 = esponente_base_due.substr(16,16);
//             esponente_base_2_3 = esponente_base_due.substr(32, esponente_base_due.length()-32);
//         }
//         else
//         {
//             esponente_base_2_2 = esponente_base_due.substr(16, esponente_base_due.length()-16);
//             esponente_base_2_3 = esponente_base_due.substr(16, esponente_base_due.length()-16);
//         }
//     }
//     else
//     {
//         esponente_base_2_1 = esponente_base_due;
//         esponente_base_2_2 = esponente_base_due;
//         esponente_base_2_3 = esponente_base_due;
//     }
//
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     mpz_class bxip1;
//
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui() % 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui() % 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//
//     //Per risolvere un petit probl�me di loop
//     if (bxip1 < 2)
//         bxip1 = 2;
//
//     //ora aggiungo i passi in pi� che devo fare...
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(bxip1, passiDaFare, blum_i);
//     }
//
//     FastCifraDecifra(passiFatti, ciphered, lenCiphered, bxip1, blum_i);
//     sBase64_ciphered = Base64::encode(lenCiphered, ciphered);
//     free(ciphered);
//
//     //passiFatti = 0; //per tornare quanti passi ho fatto qui
//
//     ////Ora posso cifrare... Con il nuovo modello...
//     //unsigned char * toXor = (unsigned char*)gestione_memoria_malloc(lenCiphered);
//     //unsigned long int posToXor=0;
//     //passiFatti = 0;
//     //while (posToXor != lenCiphered)
//     //{
//     //   unsigned char * v;
//     //   unsigned long lenV;
//     //   Ops::Mpz2UnsignedPChar(bxip1, v, lenV);
//
//     //   if ((lenV-1+posToXor)<lenCiphered)
//     //   {
//     //       //Copio tutto
//     //       memcpy(&toXor[posToXor], &v[1], lenV-1);
//     //       posToXor += lenV-1;
//     //   }
//     //   else
//     //   {
//     //       memcpy(&toXor[posToXor], &v[1], lenCiphered-posToXor);
//     //       posToXor = lenCiphered;
//     //   }
//     //   gestione_memoria_free(v);
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     //   passiFatti++;
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //unsigned char *p_ciphered = ciphered;
//     //unsigned char * p_toXor = toXor;
//
//     //for (int i=0; i < lenCiphered; i++)
//     //   *p_ciphered++ ^= *p_toXor++;
//
//     //Messaggio cifrato! Convertiamo la codifica in una base64 string!
//     return true;
// }
//
// bool BlumAlgo::VaCipher_1(string & sBase64_ciphered, string toBeCiphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //Per prima cosa comprimo o codifico la stringa in modo da farla diventare una sequenza di bytes.
//     int lenCiphered = toBeCiphered.size();
//     unsigned char * ciphered = (unsigned char*)malloc (lenCiphered);
//     memcpy(ciphered, toBeCiphered.c_str(), lenCiphered); //Ora ho una stringa, non terminante per 0
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);// r.set_str(seedToUse, 62);  //Questo � il seed che devo usare in partenza
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit. L'ultimo non so!
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//
//     string esponente_base_2_1;
//     string esponente_base_2_2;
//     string esponente_base_2_3;
//
//     if (esponente_base_due.length()>=16)
//     {
//         esponente_base_2_1 = esponente_base_due.substr(0,16);
//         if (esponente_base_due.length()>=32)
//         {
//             esponente_base_2_2 = esponente_base_due.substr(16,16);
//             esponente_base_2_3 = esponente_base_due.substr(32, esponente_base_due.length()-32);
//         }
//         else
//         {
//             esponente_base_2_2 = esponente_base_due.substr(16, esponente_base_due.length()-16);
//             esponente_base_2_3 = esponente_base_due.substr(16, esponente_base_due.length()-16);
//         }
//     }
//     else
//     {
//         esponente_base_2_1 = esponente_base_due;
//         esponente_base_2_2 = esponente_base_due;
//         esponente_base_2_3 = esponente_base_due;
//     }
//
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     mpz_class bxip1;
//
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//     }
//
//     //Per risolvere un petit probl�me di loop
//     if (bxip1 < 2)
//         bxip1 = 2;
//
//
//     //ora aggiungo i passi in pi� che devo fare...
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(bxip1, passiDaFare, blum_i);
//     }
//
//     //FastCifraDecifra(passiFatti, ciphered, lenCiphered, bxip1, blum_i);
//     unsigned char * cifrata;
//     int len_cifrata;
//     unsigned char * chiave;
//     unsigned int len_chiave;
//     Ops::Mpz2UnsignedPChar(bxip1, chiave, len_chiave);
//     BlowfishCypher(ciphered, lenCiphered, &cifrata, &len_cifrata, chiave, len_chiave);
//     bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     passiFatti++;
//
//     free(ciphered);
//     free(chiave);
//     ciphered = cifrata;
//     lenCiphered = len_cifrata;
//
//     sBase64_ciphered = Base64::encode(lenCiphered, ciphered);
//     free(ciphered);
//
//     //passiFatti = 0; //per tornare quanti passi ho fatto qui
//
//     ////Ora posso cifrare... Con il nuovo modello...
//     //unsigned char * toXor = (unsigned char*)gestione_memoria_malloc(lenCiphered);
//     //unsigned long int posToXor=0;
//     //passiFatti = 0;
//     //while (posToXor != lenCiphered)
//     //{
//     //   unsigned char * v;
//     //   unsigned long lenV;
//     //   Ops::Mpz2UnsignedPChar(bxip1, v, lenV);
//
//     //   if ((lenV-1+posToXor)<lenCiphered)
//     //   {
//     //       //Copio tutto
//     //       memcpy(&toXor[posToXor], &v[1], lenV-1);
//     //       posToXor += lenV-1;
//     //   }
//     //   else
//     //   {
//     //       memcpy(&toXor[posToXor], &v[1], lenCiphered-posToXor);
//     //       posToXor = lenCiphered;
//     //   }
//     //   gestione_memoria_free(v);
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     //   passiFatti++;
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //unsigned char *p_ciphered = ciphered;
//     //unsigned char * p_toXor = toXor;
//
//     //for (int i=0; i < lenCiphered; i++)
//     //   *p_ciphered++ ^= *p_toXor++;
//
//     //Messaggio cifrato! Convertiamo la codifica in una base64 string!
//     return true;
// }
//
//
// bool BlumAlgo::VaDecipher(string & s_deciphered, string sBase64_toBeDeciphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //assegno a deciphered la stringa base.
//     unsigned char * deciphered;
//
//     int lenDeciphered = Base64::decode(&deciphered, sBase64_toBeDeciphered);
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);  //Questo � il seed che devo usare in partenza
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit, tranne l'ultimo che non so...
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//     string esponente_base_2_1 = esponente_base_due.substr(0,16);
//     string esponente_base_2_2 = esponente_base_due.substr(16,16);
//     string esponente_base_2_3 = esponente_base_due.substr(32,esponente_base_due.size()-32);
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     //Sono nella sezione server, posso, anzi, devo utilizzare gli algoritmi veloci!!
//
//     mpz_class bxip1;
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//
//     FastCifraDecifra(passiFatti, deciphered, lenDeciphered, bxip1, blum_i);
//     //Messaggio decifrato! Convertiamo la codifica in una stringa.
//     s_deciphered.assign(&deciphered[0], &deciphered[lenDeciphered]);
//     free(deciphered);
//
//     //unsigned char * toXor = (unsigned char*)gestione_memoria_malloc(lenDeciphered);
//     //unsigned long int posToXor=0;
//     //passiFatti = 0;
//     //while (posToXor != lenDeciphered)
//     //{
//     //   unsigned char * v;
//     //   unsigned long lenV;
//     //   Ops::Mpz2UnsignedPChar(bxip1, v, lenV);
//
//     //   if ((lenV-1+posToXor)<lenDeciphered)
//     //   {
//     //       //Copio tutto
//     //       memcpy(&toXor[posToXor], &v[1], lenV-1);
//     //       posToXor += lenV-1;
//     //   }
//     //   else
//     //   {
//     //       memcpy(&toXor[posToXor], &v[1], lenDeciphered-posToXor);
//     //       posToXor = lenDeciphered;
//     //   }
//     //   gestione_memoria_free(v);
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     //   passiFatti++;
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //unsigned char *p_deciphered = deciphered;
//     //unsigned char * p_toXor = toXor;
//
//     //for (int i=0; i < lenDeciphered; i++)
//     //   *p_deciphered++ ^= *p_toXor++;
//
//
//     return true;
// }
//
// bool BlumAlgo::VaDecipher_1(string & s_deciphered, string sBase64_toBeDeciphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti)
// {
//     //assegno a deciphered la stringa base.
//     unsigned char * deciphered;
//     int lenDeciphered = Base64::decode(&deciphered, sBase64_toBeDeciphered);
//
//     mpz_class r = Ops::Base64String2Mpz(sBase64_seedToUse);  //Questo � il seed che devo usare in partenza
//
//     //L'esponente da usare ha il seguente problema, non pu� superare i 10000, e quindi non � un'esponente ma sono tre esponenti di seguito
//     //L'esponente � un numero casuale di 48 bit. Lo scompongo in tre parti.
//     mpz_class esponente = Ops::Base64String2Mpz(sBase64_exponentToUse);
//
//     mpz_class exp1, exp2, exp3;  //ognuno a 16 bit, tranne l'ultimo che non so...
//     string esponente_base_due = ToGCString(esponente.get_str(2));
//     string esponente_base_2_1 = esponente_base_due.substr(0,16);
//     string esponente_base_2_2 = esponente_base_due.substr(16,16);
//     string esponente_base_2_3 = esponente_base_due.substr(32,esponente_base_due.size()-32);
//
//     //Effettivamente io ora elever� a potenza per un valore totale che non � pari all'esponente ma � pari alla somma dei tre esponenti a 16 bit. Ma non posso fare altrimenti
//     //in quanto quello che accade � che altrimenti sarebbe troppo lento da calcolare, purtroppo. !!!
//
//     exp1.set_str(FromGCString(esponente_base_2_1),2);
//     exp2.set_str(FromGCString(esponente_base_2_2),2);
//     exp3.set_str(FromGCString(esponente_base_2_3),2);
//
//     //Sono nella sezione server, posso, anzi, devo utilizzare gli algoritmi veloci!!
//
//     mpz_class bxip1;
//     if (usePQ)
//     {
//         bxip1 =  getI_esimo_fast(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_fast(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//     else
//     {
//         bxip1 =  getI_esimo_slow(r, exp1.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp2.get_ui()% 256, blum_i);
//         bxip1 =  getI_esimo_slow(bxip1, exp3.get_ui()% 256, blum_i);
//         //Per risolvere un petit probl�me di loop
//         if (bxip1 < 2)
//             bxip1 = 2;
//         bxip1 =  getI_esimo_fast(bxip1, passiDaFare, blum_i);
//     }
//
//     //FastCifraDecifra(passiFatti, deciphered, lenDeciphered, bxip1, blum_i);
//     unsigned char * decifrata;
//     int len_decifrata;
//     unsigned char * chiave;
//     unsigned int len_chiave;
//
//     Ops::Mpz2UnsignedPChar(bxip1, chiave, len_chiave);
//
//     BlowfishDecypher(deciphered, lenDeciphered, &decifrata, &len_decifrata, chiave, len_chiave);
//     bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     passiFatti++;
//
//     free(deciphered);
//     free(chiave);
//     deciphered = decifrata;
//     lenDeciphered = len_decifrata;
//
//     //Messaggio decifrato! Convertiamo la codifica in una stringa.
//     s_deciphered.assign(&deciphered[0], &deciphered[lenDeciphered]);
//     free(deciphered);
//
//     //unsigned char * toXor = (unsigned char*)gestione_memoria_malloc(lenDeciphered);
//     //unsigned long int posToXor=0;
//     //passiFatti = 0;
//     //while (posToXor != lenDeciphered)
//     //{
//     //   unsigned char * v;
//     //   unsigned long lenV;
//     //   Ops::Mpz2UnsignedPChar(bxip1, v, lenV);
//
//     //   if ((lenV-1+posToXor)<lenDeciphered)
//     //   {
//     //       //Copio tutto
//     //       memcpy(&toXor[posToXor], &v[1], lenV-1);
//     //       posToXor += lenV-1;
//     //   }
//     //   else
//     //   {
//     //       memcpy(&toXor[posToXor], &v[1], lenDeciphered-posToXor);
//     //       posToXor = lenDeciphered;
//     //   }
//     //   gestione_memoria_free(v);
//     //    bxip1 = BlumAlgo::getNext(bxip1, blum_i);
//     //   passiFatti++;
//     //}
//
//     ////Ora posso fare direttamente lo xor tra i due vettori.
//     //unsigned char *p_deciphered = deciphered;
//     //unsigned char * p_toXor = toXor;
//
//     //for (int i=0; i < lenDeciphered; i++)
//     //   *p_deciphered++ ^= *p_toXor++;
//
//
//     return true;
// }
//
// #define MAX_TRIES 1000
// #define TRY_CIPHER_ERROR "!!!TRY_CIPHER_ERROR!!!"
// static string TryCipher(BlumInteger & blum_i, string input)
// {
//     int add=0;
//     int tries = 0;
//     string result;
//
//     while (true)
//     {
//         if (tries++ > MAX_TRIES)
//             return string(TRY_CIPHER_ERROR);
//
//         string toAdd = ToGCString((long long)add) + string("\n");
//         add++;
//         //Faccio il base64
//         string toBeUnsquared = Base64::encode(string(toAdd+input));
//
//         string unsquared;
//         //int iRet = Unsquare(csec, toBeUnsquared, unsquared);
//         BlumAlgo::Unsquare(toBeUnsquared, unsquared, blum_i);
//
//         string squared;
//         //iRet = Square(csec, unsquared, squared);
//         BlumAlgo::Square(unsquared, squared, blum_i);
//
//         if (squared.compare(toBeUnsquared)==0)
//             return unsquared;
//     }
// }
//
// static std::vector<string, gc_allocator<string>> SplitString(const string & input, int len)
// {
//     std::vector<string, gc_allocator<string>> vs;
//     string sAppo = input;
//     while (sAppo.size()>0)
//     {
//         string sTmp = sAppo.substr(0, len);  //I primi len caratteri
//         if (len>=sAppo.size())
//             sAppo = "";
//         else
//             sAppo = sAppo.substr(len);           //Da len in fondo
//         vs.push_back(sTmp);
//     }
//
//     //int dim = 0;
//     //for (int i=0; i<vs.size(); i++)
//     //   dim += vs[i].size();
//
//     return vs;
// }
//
// int BlumAlgo::ExtendedCipher(const string input, string & output, const string p_b62, const string q_b62)
// {
//     BlumInteger vaPriv;
//     vaPriv.setP((char*)(p_b62.c_str()), 62);
//     vaPriv.setQ((char*)(q_b62.c_str()), 62);
//     vaPriv.setM(vaPriv.getP()*vaPriv.getQ());
//
//     return BlumAlgo::ExtendedCipher(input, output, vaPriv);
// }
//
// int BlumAlgo::ExtendedCipher(const string input, string & output, BlumInteger & blum_i)
// {
//     //90 per consentire di gestire anche le modifiche che faccio al testo
//     std::vector<string, gc_allocator<string>> vs = SplitString(input, 90);
//
//     //Ora, per ogni elemento del vettore, provo a cifrare e decifrare. Se ci sono problemi, aggiungo un numero crescente in testa.
//     for (int i=0; i<vs.size(); i++)
//     {
//         vs[i] = TryCipher(blum_i, vs[i]);    //Esce se sono riuscito a cifrare, altrimenti esce con condizione di errore (stringa vuota)!
//         if (vs[i].compare(TRY_CIPHER_ERROR)==0)
//         {
//             //cout << "Errore cifrando l'input." << endl;
//             output = "";
//             return -1;
//         }
//     }
//
//     output = "";
//     for (int i=0; i<vs.size(); i++)
//     {
//         if ((i+1)==vs.size())
//             output += vs[i];
//         else
//             output += (vs[i] + string(";"));
//     }
//     return 0;
// }
//
// #define TRY_DECIPHER_ERROR "!!!TRY_DECIPHER_ERROR!!!"
// static string TryDecipher(BlumInteger & blum_i, string input)
// {
//     string squared;
//     //int iRet = Square(csec, input, squared);
//     BlumAlgo::Square(input, squared, blum_i);
//
//     //Ora deve togliere la somma iniziale e il primo fine linea. MA prima deve debasare
//     string debased = Base64::decode(squared);
//
//     //ora cerco il primo fine linea
//     int pos = debased.find_first_of('\n');
//
//     if (pos<=0)
//         return string(TRY_DECIPHER_ERROR);
//     else
//         return debased.substr(pos+1);
// }
//
// int BlumAlgo::ExtendedDecipher(const string input, string &output, const string m_b62)
// {
//     BlumInteger vaPub;
//     vaPub.setM((char*)(m_b62.c_str()), 62);
//     return BlumAlgo::ExtendedDecipher(input, output, vaPub);
// }
//
// int BlumAlgo::ExtendedDecipher(const string input, string &output, BlumInteger & blum_i)
// {
//     std::vector<string, gc_allocator<string>> vs = Ops::Tokenize(input, string(";"));
//     for (int i=0; i<vs.size(); i++)
//     {
//         vs[i] = TryDecipher(blum_i, vs[i]);  //Esce se sono riuscito a decifrare, altrimenti esce con condizione di errore (stringa vuota)!
//         if (vs[i].compare(TRY_DECIPHER_ERROR)==0)
//         {
//             //cout << "Errore decifrando l'input." << endl;
//             output = "";
//             return -1;
//         }
//     }
//     output = "";
//     for (int i=0; i<vs.size(); i++)
//     {
//         output += vs[i];
//     }
//     return 0;
// }


typedef struct mk
{
    mpz_class m;
    unsigned long int k;
}* p_mk_t, mk_t;

/* Function prototypes (Miller-Rabin) */
static p_mk_t get_mk ( mpz_class& );
static bool millerrabin ( mpz_class& );
static bool mrcore ( mpz_class&, mpz_class&, unsigned long int, unsigned long int, mpz_class&, mpz_class&, mpz_class&, mpz_class& );
static bool in_array ( mpz_class&, unsigned long int*, int );

/* Function prototype (Fermat) */
static bool fermat ( mpz_class& );

/* Function implementations */
/* n must be an odd integer */
static p_mk_t get_mk ( mpz_class& n )
{
    unsigned long int k = 1;
    unsigned long int pow2 = 2;
    mpz_class quot;
    mpz_class n_cpy;
    p_mk_t mk;
    mk = new  mk_t();
    /* Do not clobber the value of n */
    n_cpy = n;
    /* n_cpy-- */
    n_cpy -= 1;
    /* n_cpy div pow2 and get the quotient */
    mpz_cdiv_q_ui ( quot.get_mpz_t(), n_cpy.get_mpz_t(), pow2 );

    while ( pow2 != 0x80000000 && mpz_even_p ( quot.get_mpz_t() ) )
    {
        pow2 *= 2;
        k++;
        mpz_cdiv_q_ui ( quot.get_mpz_t(), n_cpy.get_mpz_t(), pow2 );
    }

    mk->m = quot;
    mk->k = k;
    /* Free up no longer used stuffs */
    return mk;
}

static bool mrcore ( mpz_class& min_one, mpz_class& a, unsigned long int a_given, unsigned long int k, mpz_class& b0, mpz_class& b, mpz_class& m, mpz_class& n )
{
    unsigned long int i;
    /* Choose a random integer a with 1 < a < n-1 */
    a = a_given;
    /* Compute b0 = a^m mod n */
    mpz_powm ( b0.get_mpz_t(), a.get_mpz_t(), m.get_mpz_t(), n.get_mpz_t() );

    /* If b0 = +/- 1 mod n, then n is probably prime */
    if ( ( b0 == 1 ) ||
            ( b0 == min_one ) )
    {
        return true;
    }

    /* b = b0 */
    b = b0;

    for ( i = 1; i <= k - 1; i++ )
    {
        mpz_powm_ui ( b.get_mpz_t(), b.get_mpz_t(), 2, n.get_mpz_t() );

        if ( ( b == 1 ) )
        {
            return false;
        }
        else if ( ( b == min_one ) )
        {
            return true;
        }
    }

    return false;
}

static bool millerrabin ( mpz_class& n )
{
    const int SIZE = 30;
    p_mk_t mk;
    mpz_class a, b0, b, m, min_one;
    unsigned long int k;
    unsigned long int as[] =
    {
        2,   3,   5,   7,  11,
        13,  17,  19,  23,  29,
        31,  37,  41,  43,  47,
        53,  59,  61,  67,  71,
        73,  79,  83,  89,  97,
        101, 103, 107, 109, 113
    };
    bool mr_rslt = true;
    int i;

    /* Check for all the a's, if it is one of the a's, it's automatically a
    * prime.
    */
    if ( in_array ( n, as, SIZE ) )
    {
        return true;
    }

    /* Let n > 1 be an odd integer */
    if ( ( n <= 1 ) )
    {
        return false;
    }

    if ( mpz_even_p ( n.get_mpz_t() ) )
    {
        return false;
    }

    /* Write n - 1 = 2^k * m where m is odd */
    mk = get_mk ( n );
    m = mk->m;
    k = mk->k;
    delete mk;
    mk = NULL;
    /* Compute n-1 which is -1 */
    min_one = n - 1;

    /* Call mrcore for a = first 30 primes */
    for ( i = 0; i < SIZE; i++ )
    {
        mr_rslt = mr_rslt && mrcore ( min_one, a, as[i], k, b0, b, m, n );

        if ( !mr_rslt )
        {
            return false;
        }
    }

    return mr_rslt;
}

static bool in_array ( mpz_class& needle, unsigned long int* arr, int arr_sz )
{
    for ( int i = 0; i < arr_sz; i++ )
    {
        if ( ( needle == arr[i] ) )
        {
            return true;
        }
    }

    return false;
}

static bool fermat ( mpz_class& n )
{
    unsigned long int i, a;
    mpz_class g, fer, base, exp1;
    i = 0;
    a = 2;

    while ( i < 10 )
    {
        mpz_gcd_ui ( g.get_mpz_t(), n.get_mpz_t(), a );

        if ( ( g == 1 ) )
        {
            i++;
            base = a;
            exp1 = n - 1;
            mpz_powm ( fer.get_mpz_t(), base.get_mpz_t(), exp1.get_mpz_t(), n.get_mpz_t() );

            if ( ( fer != 1 ) )
            {
                return false;
            }
        }

        a++;
    }

    return ( fer == 1 );
}

bool CheckForPrimality ( mpz_t in_n );
bool CheckForPrimality ( mpz_t in_n )
{
    mpz_class n;
    mpz_set ( n.get_mpz_t(), in_n );
    return ( millerrabin ( n ) && fermat ( n ) );
}


static void BN2mpz ( mpz_t s, const BIGNUM* bn )
{
    size_t len;
    unsigned char* p;

    len = BN_num_bytes ( bn );
    p = ( unsigned char* ) malloc ( len );
    BN_bn2bin ( bn, p );
    mpz_init ( s );
    mpz_import ( s, len, 1, 1, 1, 0, p );

    free ( p );
}

static BIGNUM* mpz2BN ( mpz_t s )
{
    size_t size;
    BIGNUM* bn;
    unsigned char* p;

    int numb = 8 * 1 - 0; //8*size - nail;
    int count = ( mpz_sizeinbase ( s, 2 ) + numb - 1 ) / numb;
    p = ( unsigned char* ) malloc ( count * 1 /*size*/ );

    if ( p == NULL )
        return NULL;

    //         void *rop, size_t *countp, int order, size_t size, int endian, size_t nails, const mpz_t op
    mpz_export ( p,              &size,          1,           1,          1,            0,              s );
    bn = BN_bin2bn ( p, size, NULL );
    free ( p );
    return bn;
}

static int gmp_rsa_generate_key ( RSA* rsa, int bits, mpz_t in_p, mpz_t in_q ) //, BIGNUM *e, BN_GENCB *cb)
{
    //Random random;
    mpz_t el, p, q, n, d, dmp1, dmq1, iqmp, t1, t2, t3;
    int counter, ret;

    ret = -1;

    mpz_init ( el );
    mpz_init ( p );
    mpz_init ( q );
    mpz_init ( n );
    mpz_init ( d );
    mpz_init ( dmp1 );
    mpz_init ( dmq1 );
    mpz_init ( iqmp );
    mpz_init ( t1 );
    mpz_init ( t2 );
    mpz_init ( t3 );

    //BN2mpz(el, e);
    mpz_set_ui ( el, 65537 ); //3 oppure 65537!!

    /* generate p and q so that p != q and bits(pq) ~ bits */

    if ( mpz_cmp_ui ( in_p, 0 ) == 0 )
    {
        mpz_class mr;
        Random::StrongRandom ( bits / 2, mr );
        mpz_set ( p, mr.get_mpz_t() );
    }
    else
    {
        int num_bits = bits / 2;
        mpz_class cp;
        mpz_set ( cp.get_mpz_t(), in_p );

        if ( cp <= 2 )
            cp = 11111 - cp;

        while ( mpz_sizeinbase ( cp.get_mpz_t(), 2 ) != num_bits )
        {
            if ( mpz_sizeinbase ( cp.get_mpz_t(), 2 ) > num_bits )
            {
                cp >>= 1;
                cp += 57;
            }
            else if ( mpz_sizeinbase ( cp.get_mpz_t(), 2 ) < num_bits )
            {
                cp <<= 1;
                cp += 103;
            }
        }

        mpz_set ( p, cp.get_mpz_t() );
    }

    counter = 0;

    do
    {
        NextPrime ( p );

        mpz_sub_ui ( t1, p, 1 );
        mpz_gcd ( t2, t1, el );
    }
    while ( mpz_cmp_ui ( t2, 1 ) != 0 );

    counter = 0;

    if ( mpz_cmp_ui ( in_q, 0 ) == 0 )
    {
        mpz_class qm;
        Random::StrongRandom ( bits / 2, qm );
        mpz_set ( q, qm.get_mpz_t() );
    }
    else
    {
        int num_bits = bits / 2;
        mpz_class cq;
        mpz_set ( cq.get_mpz_t(), in_q );

        if ( cq <= 2 )
            cq = 10000 - cq;

        while ( mpz_sizeinbase ( cq.get_mpz_t(), 2 ) != num_bits )
        {
            if ( mpz_sizeinbase ( cq.get_mpz_t(), 2 ) > num_bits )
            {
                cq >>= 1;
                cq += 67;
            }
            else if ( mpz_sizeinbase ( cq.get_mpz_t(), 2 ) < num_bits )
            {
                cq <<= 1;
                cq += 97;
            }
        }

        mpz_set ( q, cq.get_mpz_t() );
    }

    do
    {
        NextPrime ( q );

        mpz_sub_ui ( t1, q, 1 );
        mpz_gcd ( t2, t1, el );
    }
    while ( mpz_cmp_ui ( t2, 1 ) != 0 );

    /* make p > q */
    if ( mpz_cmp ( p, q ) < 0 )
        mpz_swap ( p, q );

    //BN_GENCB_call(cb, 3, 1);

    /* calculate n,         n = p * q */
    mpz_mul ( n, p, q );

    /* calculate d,         d = 1/e mod (p - 1)(q - 1) */
    mpz_sub_ui ( t1, p, 1 );
    mpz_sub_ui ( t2, q, 1 );
    mpz_mul ( t3, t1, t2 );
    mpz_invert ( d, el, t3 );

    /* calculate dmp1       dmp1 = d mod (p-1) */
    mpz_mod ( dmp1, d, t1 );
    /* calculate dmq1       dmq1 = d mod (q-1) */
    mpz_mod ( dmq1, d, t2 );
    /* calculate iqmp       iqmp = 1/q mod p */
    mpz_invert ( iqmp, q, p );

    /* fill in RSA key */
    //     int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
    //     int RSA_set0_factors(RSA *r, BIGNUM *p, BIGNUM *q);
    //     int RSA_set0_crt_params(RSA *r,BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp);

    RSA_set0_key ( rsa, mpz2BN ( n ), mpz2BN ( el ), mpz2BN ( d ));
    RSA_set0_factors(rsa, mpz2BN ( p ), mpz2BN ( q ));
    RSA_set0_crt_params(rsa, mpz2BN ( dmp1 ), mpz2BN ( dmq1 ), mpz2BN ( iqmp ));
//     rsa->e = mpz2BN ( el );
//     rsa->p = mpz2BN ( p );
//     rsa->q = mpz2BN ( q );
//     rsa->n = mpz2BN ( n );
//     rsa->d = mpz2BN ( d );
//     rsa->dmp1 = mpz2BN ( dmp1 );
//     rsa->dmq1 = mpz2BN ( dmq1 );
//     rsa->iqmp = mpz2BN ( iqmp );

    ret = 1;

    mpz_clear ( el );
    mpz_clear ( p );
    mpz_clear ( q );
    mpz_clear ( n );
    mpz_clear ( d );
    mpz_clear ( dmp1 );
    mpz_clear ( dmq1 );
    mpz_clear ( iqmp );
    mpz_clear ( t1 );
    mpz_clear ( t2 );
    mpz_clear ( t3 );

    return ret;
}


c_rsa::c_rsa()
{
	unsigned long long p[32];   //dipende da quello che sta nello stack!
	ULL_Rng* rng = new  ULL_Rng ( Ticks::get_micro() );
	
	for ( int i = 0; i < 32; ++i )
		p[i] = rng->Next();
	
	delete rng;
	RAND_seed ( p, 32 * sizeof ( unsigned long long ) ); /* or OAEP may fail */
	key = nullptr;
}
// c_ecurve::c_ecurve()
// {
// 	unsigned long long p[32];   //dipende da quello che sta nello stack!
// 	ULL_Rng* rng = new  ULL_Rng ( Ticks::get_micro() );
// 	
// 	for ( int i = 0; i < 32; ++i )
// 		p[i] = rng->Next();
// 	
// 	delete rng;
// 	RAND_seed ( p, 32 * sizeof ( unsigned long long ) ); /* or OAEP may fail */
// 	key = nullptr;
// }

bool c_rsa::SetPubKey ( const char* s )
{
    int keylen;

    RSA* k = NULL;  //RSA_new();

    BIO* bio = BIO_new_mem_buf ( ( void* ) s, strlen ( s ) );

    k = PEM_read_bio_RSAPublicKey ( bio, NULL, NULL, NULL );

    //k = PEM_read_bio_RSAPublicKey(bio, &k, NULL, NULL);
    if ( k == NULL )
    {
        BIO_free_all ( bio );
        RSA_free ( k );
        return false;
    }

    if ( key )
        RSA_free ( key );

    key = k;

    BIO_free_all ( bio );
    return true;
}

bool c_rsa::SetPrivKey ( const char* s )
{
    int keylen;

    RSA* k = RSA_new();

    BIO* bio = BIO_new_mem_buf ( ( void* ) s, strlen ( s ) );

    k = PEM_read_bio_RSAPrivateKey ( bio, &k, NULL, NULL );

    if ( k == NULL )
    {
        BIO_free_all ( bio );
        RSA_free ( k );
        return false;
    }

    if ( key )
        RSA_free ( key );

    key = k;

    BIO_free_all ( bio );
    return true;
}


//bool c_rsa::CifraVeloce(unsigned char ** s_cifrato, long int * len_cifrato, unsigned char * s_messaggio, long int len_messaggio)
//{
//if ((len_messaggio*8) > (mpz_sizeinbase (n.get_mpz_t(), 2)-1))
//  return false;   //Ammetto di non utilizzare padding ma almeno non devi rischiare di bucare la futura decifratura

//mpz_class messaggio;

//for (int i=0; i<len_messaggio; ++i)
//{
//  messaggio <<= 8;
//  messaggio += s_messaggio[i];
//}

//mpz_class m1, m2, cifrato;

//mpz_powm(m1.get_mpz_t(), messaggio.get_mpz_t(), dmp1.get_mpz_t(), p.get_mpz_t());
//mpz_powm(m2.get_mpz_t(), messaggio.get_mpz_t(), dmq1.get_mpz_t(), q.get_mpz_t());

//cifrato = ((iqmp%p)*(m1-m2))*q + m2;

//*len_cifrato = 0;
//mpz_class tmp = cifrato;
//while (tmp != 0)
//{
//  tmp >>= 8;
//  (*len_cifrato)++;
//}
//*s_cifrato = (unsigned char*)malloc((size_t)(*len_cifrato));
//
//*len_cifrato = 0;
//tmp = cifrato;
//while (tmp != 0)
//{
//  mpz_class tmpmod100 = tmp % 100;
//  (*s_cifrato)[*len_cifrato] = tmpmod100.get_ui();
//  tmp >>= 8;
//  (*len_cifrato)++;
//}
//  return true;
//}

//bool c_rsa::Decifra(unsigned char ** s_decifrato, long int * len_decifrato, unsigned char * s_cifrato, long int len_cifrato)
//{
//if ((len_cifrato*8) > (mpz_sizeinbase (n.get_mpz_t(), 2)-1))
//  return false;   //Ammetto di non utilizzare padding ma almeno non devi rischiare di bucare la futura decifratura
//mpz_class cifrato, decifrato;

//for (int i=0; i<len_cifrato; ++i)
//{
//  cifrato <<= 8;
//  cifrato += s_cifrato[i];
//}

////Decifratura con chiave pubblica: c=m^e mod n
//mpz_powm(decifrato.get_mpz_t(), cifrato.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());

//*len_decifrato = 0;
//mpz_class tmp = decifrato;
//while (tmp != 0)
//{
//  tmp >>= 8;
//  (*len_decifrato)++;
//}
//*s_decifrato = (unsigned char*)malloc((size_t)(*len_decifrato));
//
//*len_decifrato = 0;
//tmp = decifrato;
//while (tmp != 0)
//{
//  mpz_class tmpmod100 = tmp % 100;
//  (*s_decifrato)[*len_decifrato] = tmpmod100.get_ui();
//  tmp >>= 8;
//  (*len_decifrato)++;
//}
//  return true;
//}

void c_rsa::MakeKey ( int bits )
{
    mpz_t p, q;
    mpz_init ( p );
    mpz_init ( q );
    mpz_set_ui ( p, 0 );
    mpz_set_ui ( q, 0 );
    MakeKey ( bits, p, q );
    mpz_clear ( p );
    mpz_clear ( q );
}

void c_rsa::MakeKey ( int bits, mpz_t p, mpz_t q )
{
    RSA* rsa = RSA_new();
    //Con questa sostituisco solo i valori di interesse!
    gmp_rsa_generate_key ( rsa, bits, p, q );

    if ( key )
        RSA_free ( key );

    key = rsa;
}

char* c_rsa::GetPrivate()
{
    if ( !key )
    {
        return nullptr;
    }

    int keylen;

    BIO* bio = BIO_new ( BIO_s_mem() );
    int iret = PEM_write_bio_RSAPrivateKey ( bio, key, NULL, NULL, 0, NULL, NULL );

    if ( iret == 0 )
    {
        BIO_free_all ( bio );
        return nullptr;
    }

    keylen = BIO_pending ( bio );
    char* pcKey = ( char* ) malloc ( keylen + 1 ); /* Null-terminate */
    BIO_read ( bio, pcKey, keylen );

    BIO_free_all ( bio );
    return pcKey;
}

char* c_rsa::GetPublic()
{
    if ( !key )
    {
        return nullptr;
    }

    int keylen;
    BIO* bio = BIO_new ( BIO_s_mem() );
    int iret = PEM_write_bio_RSAPublicKey ( bio, key );

    if ( iret == 0 )
    {
        BIO_free_all ( bio );
        return nullptr;
    }

    keylen = BIO_pending ( bio );
    char* pubKey = ( char* ) malloc ( keylen + 1 ); /* Null-terminate */
    BIO_read ( bio, pubKey, keylen );

    BIO_free_all ( bio );
    return pubKey;
}

c_rsa::~c_rsa()
{
    if ( key )
        RSA_free ( key );

    key = nullptr;
}

bool c_rsa::Sign ( unsigned char** s_signed, int* len_signed, unsigned char* digest, int len_digest, int tipo )
{
    if ( !key )
    {
        return false;
    }

    //int RSA_sign(int type, const unsigned char *m, unsigned int m_len, unsigned char *sigret, unsigned int *siglen, RSA *rsa);
    ///Tipo della firma. 0: 0: sha1, 1: ripemd160, 2: md5, 3: sha1+md5, 4: NID_sha224, 5: NID_sha256, 6: NID_sha384, 7: NID_sha512
    int flag;

    switch ( tipo )
    {
        case 0:
            flag = NID_sha1;
            break;
        case 1:
            flag = NID_ripemd160;
            break;
        case 2:
            flag = NID_md5;
            break;
        case 3:
            flag = NID_md5_sha1;
            break;
        case 4:
            flag = NID_sha224;
            break;
        case 5:
            flag = NID_sha256;
            break;
        case 6:
            flag = NID_sha384;
            break;
        case 7:
            flag = NID_sha512;
            break;
//         case 8:
//             flag = NID_sha512_224;
//             break;
//         case 9:
//             flag = NID_sha512_256;
//             break;
//         case 10:
//             flag = NID_sha3_224;
//             break;
//         case 11:
//             flag = NID_sha3_256;
//             break;
//         case 12:
//             flag = NID_sha3_384;
//             break;
//         case 13:
//             flag = NID_sha3_512;
//             break;
    }

    *len_signed = RSA_size ( key );
    *s_signed = ( unsigned char* ) malloc ( RSA_size ( key ) );
    int iret = RSA_sign ( flag, digest, len_digest, *s_signed, ( unsigned int* ) len_signed, key );

    if ( iret == 1 )
        return true;
    else
    {
        free ( *s_signed );
        *len_signed = 0;
        return false;
    }
}

bool c_rsa::Verify ( unsigned char* s_signed, int len_signed, unsigned char* digest, int len_digest, int tipo )
{
    if ( !key )
    {
        return false;
    }

    //int RSA_verify(int type, const unsigned char *m, unsigned int m_len, unsigned char *sigbuf, unsigned int siglen, RSA *rsa);
    ///Tipo della firma. 0: 0: sha1, 1: ripemd160, 2: md5, 3: sha1+md5, 4: NID_sha224, 5: NID_sha256, 6: NID_sha384, 7: NID_sha512
    int flag;

    switch ( tipo )
    {
        case 0:
            flag = NID_sha1;
            break;

        case 1:
            flag = NID_ripemd160;
            break;

        case 2:
            flag = NID_md5;
            break;

        case 3:
            flag = NID_md5_sha1;
            break;

        case 4:
            flag = NID_sha224;
            break;

        case 5:
            flag = NID_sha256;
            break;

        case 6:
            flag = NID_sha384;
            break;

        case 7:
            flag = NID_sha512;
            break;
    }

    int iret = RSA_verify ( flag, digest, len_digest, s_signed, len_signed, key );

    if ( iret == 1 )
        return true;
    else
    {
        return false;
    }
}


bool c_rsa::PrivDecifra ( unsigned char** s_decifrato,  int* len_decifrato, unsigned char* s_cifrato,  int len_cifrato, int padding )
{
    if ( !key )
    {
        return false;
    }

    int flag;
    int dim;

    switch ( padding )
    {
        case 0:
            flag = RSA_PKCS1_PADDING;
            dim = 11;
            break;

        case 1:
            flag = RSA_PKCS1_OAEP_PADDING;
            dim = 41;
            break;

//         case 2:
//             flag = RSA_SSLV23_PADDING;
//             dim = 11;
//             break;

        case 5:
            flag = RSA_X931_PADDING;
//             dim = 41;
            break;

        case 3:
            flag = RSA_NO_PADDING;
            dim = 0;
            break;
    }

    if ( len_cifrato > RSA_size ( key ) )
        return false;   //non posso decifrare!

    *s_decifrato = ( unsigned char* ) malloc ( RSA_size ( key ) ); //Max len
    *len_decifrato = RSA_private_decrypt ( len_cifrato, s_cifrato, *s_decifrato, key, flag );

    if ( *len_decifrato == -1 )
    {
        free ( *s_decifrato );
        len_decifrato = 0;
        return false;
    }

    return true;
}

bool c_rsa::PrivCifra ( unsigned char** s_cifrato,  int* len_cifrato, unsigned char* s_msg,  int len_msg, int padding )
{
    if ( !key )
    {
        return false;
    }

    int flag;
    int dim;

    switch ( padding )
    {
        case 0:
            flag = RSA_PKCS1_PADDING; //Ammessi solo questo e nopadding
            dim = 11;
            break;

        case 1:
            flag = RSA_PKCS1_OAEP_PADDING;
            dim = 41;
            break;

//         case 2:
//             flag = RSA_SSLV23_PADDING;
//             dim = 11;
//             break;

        case 5:
            flag = RSA_X931_PADDING;
//             dim = 41;
            break;

        default:
            flag = RSA_NO_PADDING;
            dim = 0;
            break;
    }

    if ( len_msg > RSA_size ( key ) )
        return false;   //non posso cifrare!

    *len_cifrato = RSA_size ( key );
    *s_cifrato = ( unsigned char* ) malloc ( RSA_size ( key ) );
    int iRet = RSA_private_encrypt ( len_msg, s_msg, *s_cifrato, key, flag );

    if ( iRet == -1 )
    {
        ERR_load_crypto_strings();
        auto errore = ERR_get_error();
        char buf[256];
        ERR_error_string ( errore, buf );
        cout << "Errore: " << buf << endl;
        free ( *s_cifrato );
        len_cifrato = 0;
        return false;
    }

    return true;
}

bool c_rsa::PubDecifra ( unsigned char** s_decifrato,  int* len_decifrato, unsigned char* s_cifrato,  int len_cifrato, int padding )
{
    if ( !key )
    {
        return false;
    }

    int flag;
    //int dim;

    switch ( padding )
    {
        case 0:
            flag = RSA_PKCS1_PADDING; //solo questo e nopad
//             dim = 11;
            break;

        case 1:
            flag = RSA_PKCS1_OAEP_PADDING;
//             dim = 41;
            break;

//         case 2:
//             flag = RSA_SSLV23_PADDING;
// //             dim = 11;
//             break;

        case 5:
            flag = RSA_X931_PADDING;
//             dim = 41;
            break;
        default:
            flag = RSA_NO_PADDING;
//             dim = 0;
            break;
    }

    if ( len_cifrato > RSA_size ( key ) )
        return false;   //non posso decifrare!

    *s_decifrato = ( unsigned char* ) malloc ( RSA_size ( key ) ); //Max len
    *len_decifrato = RSA_public_decrypt ( len_cifrato, s_cifrato, *s_decifrato, key, flag );

    if ( *len_decifrato == -1 )
    {
        free ( *s_decifrato );
        len_decifrato = 0;
        return false;
    }

    return true;
}

bool c_rsa::PubCifra ( unsigned char** s_cifrato,  int* len_cifrato, unsigned char* s_messaggio,  int len_messaggio, int padding )
{
    if ( !key )
    {
        return false;
    }

    /*
    RSA_public_encrypt()
    encrypts the flen bytes at from (usually a session key) using the public key rsa and stores the ciphertext in to. to must point to RSA_size(rsa) bytes of memory.

    Padding denotes one of the following modes:

    RSA_PKCS1_PADDING:      PKCS #1 v1.5 padding. This currently is the most widely used mode.
    RSA_PKCS1_OAEP_PADDING: EME-OAEP as defined in PKCS #1 v2.0 with SHA-1, MGF1 and an empty encoding parameter. This mode is recommended for all new applications.
    RSA_SSLV23_PADDING:     PKCS #1 v1.5 padding with an SSL-specific modification that denotes that the server is SSL3 capable.
    RSA_NO_PADDING:         Raw RSA encryption. This mode should only be used to implement cryptographically sound padding modes in the application code. Encrypting user data directly with RSA is insecure.

    flen must be less than RSA_size(rsa) - 11 for the PKCS #1 v1.5 based padding modes,
    less than RSA_size(rsa) - 41 for RSA_PKCS1_OAEP_PADDING and
    exactly RSA_size(rsa) for RSA_NO_PADDING.

    The random number generator must be seeded prior to calling RSA_public_encrypt().
    RSA_private_decrypt() decrypts the flen bytes at from using the private key rsa and stores the plaintext in to.
    to must point to a memory section large enough to hold the decrypted data (which is smaller than RSA_size(rsa)).
    padding is the padding mode that was used to encrypt the data.

    int RSA_public_encrypt (int flen, unsigned char *from, unsigned char *to, RSA *rsa, int padding);
    int RSA_private_decrypt(int flen, unsigned char *from, unsigned char *to, RSA *rsa, int padding);
    */
    int flag;
//     int dim;

    switch ( padding )
    {
        case 1:
            flag = RSA_PKCS1_PADDING;
//             dim = 11;
            break;

//         case 2:
//             flag = RSA_SSLV23_PADDING;
// //             dim = 11;
//             break;

        case 3:
            flag = RSA_NO_PADDING;
//             dim = 0;
            break;

        case 4:
            flag = RSA_PKCS1_OAEP_PADDING;
//             dim = 41;
            break;

        case 5:
            flag = RSA_X931_PADDING;
//             dim = 41;
            break;

    }

//     if ( len_messaggio >= ( RSA_size ( key ) - dim ) )
//         return false;   //non posso cifrare!

    *len_cifrato = RSA_size ( key );
    *s_cifrato = ( unsigned char* ) malloc ( *len_cifrato );
    int iret = RSA_public_encrypt ( len_messaggio, s_messaggio, *s_cifrato, key, flag );

    if ( iret == -1 )
    {
        free ( *s_cifrato );
        len_cifrato = 0;
        return false;
    }

    return true;
}

/*CURVE ELLITTICHE, DA INSERIRE
c_ecurve::c_ecurve()
{
	unsigned long long p[32];   //dipende da quello che sta nello stack!
	ULL_Rng* rng = new  ULL_Rng ( Ticks::get_micro() );
	
	for ( int i = 0; i < 32; ++i )
		p[i] = rng->Next();
	
	delete rng;
	RAND_seed ( p, 32 * sizeof ( unsigned long long ) );
	key = nullptr;
}

c_ecurve::~c_ecurve()
{
    if ( key )
        EC_KEY_free( key );
    key = nullptr;
}

///Padding 0: pkcs1 v1.5, 1:pkcs1 v2, 2: sslv3, 3:nopadding
bool c_ecurve::PubCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding)
{
    if ( !key )
    {
        return false;
    }

    int flag;

    switch ( padding )
    {
        case 1:
            flag = RSA_PKCS1_PADDING;
            break;

        case 2:
            flag = RSA_SSLV23_PADDING;
            break;

        case 3:
            flag = RSA_NO_PADDING;
            break;

        case 4:
            flag = RSA_PKCS1_OAEP_PADDING;
            break;

        case 5:
            flag = RSA_X931_PADDING;
            break;
    }

    *len_cifrato = EC_KEY_size( key );
    *s_cifrato = ( unsigned char* ) malloc ( *len_cifrato );
    E
    int iret = RSA_public_encrypt ( len_messaggio, s_messaggio, *s_cifrato, key, flag );

    if ( iret != RSA_size ( key ) )
    {
        free ( s_cifrato );
        len_cifrato = 0;
        return false;
    }

    return true;
}
bool c_ecurve::PrivCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 )
{
    
}
bool c_ecurve::PubDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 )
{
    
}
bool c_ecurve::PrivDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 )
{
    
}

///Tipo della firma. 0: 0: sha1, 1: ripemd160, 2: md5, 3: sha1+md5, 4: NID_sha224, 5: NID_sha256, 6: NID_sha384, 7: NID_sha512
bool c_ecurve::Sign ( unsigned char **s_signed, int * len_signed, unsigned char * digest, int len_digest, int tipo=7 )
{
    
}
bool c_ecurve::Verify ( unsigned char *s_signed, int len_signed, unsigned char * digest, int len_digest, int tipo=7 )
{
    
}

void c_ecurve::MakeKey ( int bits )
{
    
}
void c_ecurve::MakeKey ( int bits, mpz_t p, mpz_t q )
{
    
}
char * c_ecurve::GetPublic()
{
    
}
char * c_ecurve::GetPrivate()
{
    
}
bool c_ecurve::SetPubKey ( const char * s )
{
    
}
bool c_ecurve::SetPrivKey ( const char * s )
{
    
}
*/


/*
crea certificato x509 e ci scrive dentro!!!
int mkcert(X509 **x509p, EVP_PKEY **pkeyp, int bits, int serial, int days);
int add_ext(X509 *cert, int nid, char *value);

int main(int argc, char **argv)
    {
    BIO *bio_err;
    X509 *x509=NULL;
    EVP_PKEY *pkey=NULL;

    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

    bio_err=BIO_new_fp(stderr, BIO_NOCLOSE);

    mkcert(&x509,&pkey,512,0,365);

    RSA_print_fp(stdout,pkey->pkey.rsa,0);
    X509_print_fp(stdout,x509);

    PEM_write_PrivateKey(stdout,pkey,NULL,NULL,0,NULL, NULL);
    PEM_write_X509(stdout,x509);

    X509_free(x509);
    EVP_PKEY_free(pkey);

#ifndef OPENSSL_NO_ENGINE
    ENGINE_cleanup();
#endif
    CRYPTO_cleanup_all_ex_data();

    CRYPTO_mem_leaks(bio_err);
    BIO_free(bio_err);
    return(0);
    }

static void callback(int p, int n, void *arg)
    {
    char c='B';

    if (p == 0) c='.';
    if (p == 1) c='+';
    if (p == 2) c='*';
    if (p == 3) c='\n';
    fputc(c,stderr);
    }

int mkcert(X509 **x509p, EVP_PKEY **pkeyp, int bits, int serial, int days)
    {
    X509 *x;
    EVP_PKEY *pk;
    RSA *rsa;
    X509_NAME *name=NULL;

    if ((pkeyp == NULL) || (*pkeyp == NULL))
        {
        if ((pk=EVP_PKEY_new()) == NULL)
            {
            abort();
            return(0);
            }
        }
    else
        pk= *pkeyp;

    if ((x509p == NULL) || (*x509p == NULL))
        {
        if ((x=X509_new()) == NULL)
            goto err;
        }
    else
        x= *x509p;

    rsa=RSA_generate_key(bits,RSA_F4,callback,NULL);
    if (!EVP_PKEY_assign_RSA(pk,rsa))
        {
        abort();
        goto err;
        }
    rsa=NULL;

    X509_set_version(x,2);
    ASN1_INTEGER_set(X509_get_serialNumber(x),serial);
    X509_gmtime_adj(X509_get_notBefore(x),0);
    X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);
    X509_set_pubkey(x,pk);

    name=X509_get_subject_name(x);

     //This function creates and adds the entry, working out the
     //correct string type and performing checks on its length.
     //Normally we'd check the return value for errors...

    X509_NAME_add_entry_by_txt(name,"C",
                MBSTRING_ASC, "UK", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name,"CN",
                MBSTRING_ASC, "OpenSSL Group", -1, -1, 0);

    //Its self signed so set the issuer name to be the same as the
 //  subject.

    X509_set_issuer_name(x,name);

    // Add various extensions: standard extensions
    add_ext(x, NID_basic_constraints, "critical,CA:TRUE");
    add_ext(x, NID_key_usage, "critical,keyCertSign,cRLSign");

    add_ext(x, NID_subject_key_identifier, "hash");

    // Some Netscape specific extensions
    add_ext(x, NID_netscape_cert_type, "sslCA");

    add_ext(x, NID_netscape_comment, "example comment extension");


#ifdef CUSTOM_EXT
    // Maybe even add our own extension based on existing
    {
        int nid;
        nid = OBJ_create("1.2.3.4", "MyAlias", "My Test Alias Extension");
        X509V3_EXT_add_alias(nid, NID_netscape_comment);
        add_ext(x, nid, "example comment alias");
    }
#endif

    if (!X509_sign(x,pk,EVP_md5()))
        goto err;

    *x509p=x;
    *pkeyp=pk;
    return(1);
err:
    return(0);
    }

 //Add extension using V3 code: we can set the config file as NULL
 //because we wont reference any other sections.


int add_ext(X509 *cert, int nid, char *value)
    {
    X509_EXTENSION *ex;
    X509V3_CTX ctx;
    // This sets the 'context' of the extensions.
    // No configuration database
    X509V3_set_ctx_nodb(&ctx);
    // Issuer and subject certs: both the target since it is self signed,
    // no request and no CRL

    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if (!ex)
        return 0;

    X509_add_ext(cert,ex,-1);
    X509_EXTENSION_free(ex);
    return 1;
    }
*/


