#pragma once

#include <iostream>
#include <sstream>

#include <vector>

#include <stdlib.h>
#include <memory.h>

#include <sys/timeb.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <WinCrypt.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

//#include "localTypes.h"
#include "base64.h"

#include "ULL_Rng.h"
#include "mtfa_utils.h"

#include <gmpxx.h>

#include <list>
#include <string>

using namespace std;

//extern void Stampa(const char *format, ...);




class Utilities
{
public:

    static bool PChar2String ( char * s_in, string & s_out );
    static bool PUChar2String ( unsigned char * s_in, int len_s_in, string & s_out );
    static bool String2PChar ( string s_in, char * & s_out );
    static bool String2PUChar ( string s_in, int & len_s_out, unsigned char * & s_out );

    static bool ToBase64 ( string s_in, string & s_out );
    static bool ToBase64 ( char * s_in, string & s_out );
    static bool ToBase64 ( unsigned char * s_in, int len_s_in, string & s_out );
    static bool ToBase64 ( int l_in, string & s_out );
    static bool ToBase64 ( long long ll_in, string & s_out );
    static bool ToBase64 ( char c_in, string & s_out );

    static bool FromBase64 ( string s_in, string & s_out );
    static bool FromBase64 ( string s_in, char* & s_out );
    static bool FromBase64 ( string s_in, unsigned char * & s_out, int & len_s_out );
    static bool FromBase64 ( string s_in, int & l_out );
    static bool FromBase64 ( string s_in, long long & l_out );
    static bool FromBase64 ( string s_in, char & c_out );

    static string PChar2String ( char * s_in );
    static string PUChar2String ( unsigned char * s_in, int len_s_in );

    static string ToBase64 ( string s_in );
    static string ToBase64 ( char * s_in );
    static string ToBase64 ( unsigned char * s_in, int len_s_in );
    static string ToBase64 ( int l_in );
    static string ToBase64 ( long long ll_in );
    static string ToBase64 ( char c_in );

    static string FromBase64ToString ( string s_in );
    static int FromBase64ToLong ( string s_in );
    static long long FromBase64ToLongLong ( string s_in );
    static char FromBase64ToChar ( string s_in );

    template <typename T>
    static T Unstringify ( string s_in )
    {
        T vout;
        istringstream i;
        i.str ( s_in );
        i >> vout;
        return vout;
    }

    template <typename T>
    static string Stringify ( T vin )
    {
        string sout;
        ostringstream o;
        o << vin;
        sout = o.str();
        return sout;
    }


    static unsigned long long UnstringifyUnsignedLongLong ( string index )
    {
        return Unstringify<unsigned long long> ( index );
    }
    static void Split ( std::vector<string> & vs, const string& str,const string& delimiters );
    static std::vector<string > Split ( const string& str,const string& delimiters );

    ///Ottiene un array di stringhe da una concatenzazione di stringhe in base64 separate da ";"
    static void SplitBase64 ( std::vector<string > & vs, const string& str );
    static string CatenateBase64 ( std::list<string > ls_in );
};


class Ops
{
public:
    static mpz_class PowMod ( mpz_class base, mpz_class esponente, mpz_class modulo );
    static mpz_class PowMod ( mpz_class base, unsigned  int esponente, mpz_class modulo );
    static mpz_class PowMod ( unsigned  int base, mpz_class esponente, mpz_class modulo );
    static mpz_class PowMod ( unsigned  int base, unsigned  int esponente, mpz_class modulo );
    static mpz_class Pow ( mpz_class base, unsigned  int esponente );
    static mpz_class Root ( mpz_class base, unsigned  int radice );

    static bool IsPerfectPower ( mpz_class base );
    static mpz_class Invert ( mpz_class op1, mpz_class op2 );
    static mpz_class Mod ( mpz_class op, mpz_class modulus );
    static int SizeInBase ( mpz_class num, int base );
    static int TestBit ( mpz_class num, int bit );

    static mpz_class UnsignedPChar2Mpz ( unsigned char * p, unsigned  int len );
    static mpz_class BinaryString2Mpz ( string sIn );
    static void Mpz2UnsignedPChar ( mpz_class value, unsigned char * &p, unsigned  int & len );
    static string Mpz2BinaryString ( mpz_class value );
    static string Mpz2Base64String ( mpz_class v );
    static mpz_class Base64String2Mpz ( string s );
    static std::vector<string > Tokenize ( const string &str,const string &delimiters );
    static std::vector<string > * PTokenize ( const string &str,const string &delimiters );
    static void Trim ( string& str );
    static string RandomBits ( int numOfBits );
};

class Random
{
private:
    static bool done;
    static gmp_randclass * r_rand;
public:

    static void StrongRandom ( int bits, mpz_class & mret );
    Random();
    ~Random();

    ///Inizializza il generatore di numeri casuali
    void Init();

    ///Torna un numero casuale rappresentato da n bits
    mpz_class getNbits ( unsigned int n );

    ///Torna un numero casuale rappresentato da n bits
    mpz_class getNbits ( mpz_class n );

    //Generate a random integer in the range 0 to n-1 inclusive
    mpz_class getNrange ( mpz_class n );
};

class BlumInteger
{
private:
// 	gmp_randstate_t rstate;
// 	void ResizeToNumBit(mpz_t q_mpz, int num_bits);
public:
//private:
    mpz_class	p;
    mpz_class	q;
    mpz_class	m;
    bool assigned_m;
    bool assigned_p;
    bool assigned_q;

public:

    static bool GenerateBlumInt ( int bits, unsigned long long &p, unsigned long long &q, unsigned long long &m );

    ~BlumInteger();

	BlumInteger();
	BlumInteger ( mpz_class & base_p, mpz_class & base_q );
    BlumInteger ( int num_bits );

    mpz_class getPm1Qm1();

    mpz_class getM();
    mpz_class getP();
    mpz_class getQ();
    mpz_class getPm1();
    mpz_class getQm1();

    void setM ( mpz_class value );
    void setP ( mpz_class value );
    void setQ ( mpz_class value );

    void setM ( const char * value, int base );
    void setP ( const char * value, int base );
    void setQ ( const char * value, int base );

    //void Serialize(stl_trait_writer pen);
    //void Deserialize(file_trait_reader book);

//	int vgetM(BVector b_m);
//	void vsetM(BVector b_m);

//	int vgetP(BVector b_p);
//	void vsetP(BVector b_p);

//	int vgetQ(BVector b_q);
//	void vsetQ(BVector b_q);

//private:
//	void setValue(BVector b, mpz_class value);
//	int getValue(BVector b, mpz_class value);
};


class BlumAlgo
{
public:


//    BlumAlgo();

    static mpz_class Rand ( mpz_class lower, mpz_class upper );
    static mpz_class Rand ( int numBits );

    static mpz_class getI_esimo_fast ( mpz_class x, mpz_class i, BlumInteger & blum_i );
    static mpz_class getI_esimo_fast ( mpz_class x, int i, BlumInteger & blum_i );

    static mpz_class getI_esimo_slow ( mpz_class x, int indice, BlumInteger & blum_i );

    static mpz_class getK_Previous ( mpz_class y, unsigned  int L, BlumInteger & blum_i );

    static mpz_class getNext ( mpz_class previous, BlumInteger & blum_i );
    static unsigned long long getNext ( unsigned long long previous, unsigned long long m );

    static string getBase64Next ( string previousBase64, BlumInteger & blum_i );

    static string expModMva ( string base, string esponente, BlumInteger & blum_i );

    static mpz_class getPrevious ( mpz_class next, BlumInteger & blum_i );
    static string getBase64Previous ( string nextBase64, BlumInteger & blum_i );


    static bool Square ( string toBeSquared, string & squared, BlumInteger & blum_i );
    static bool Unsquare ( string toBeUnsquared, string & unsquared, BlumInteger & blum_i );

//     static bool Cipher_Classical_old ( string t, string & ciphered, string & xip1, int oneBit, bool compress, BlumInteger &blum_i );
//     static bool Cipher_Classical ( string s_t, string & s_ciphered, string & s_xip1, int & steps, BlumInteger & blum_i );
// 
//     static bool Decipher_Classical_old ( string & t, string ciphered, string xip1, int oneBit, bool compress, BlumInteger & blum_i );
//     static bool Decipher_Classical ( string & s_t, string s_ciphered, string s_xip1, int steps, BlumInteger & blum_i );
// 
//     static bool VaCipher_old ( string & s_ciphered, string toBeCiphered, string seedToUse, string exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
//     static bool VaDecipher_old ( string & s_deciphered, string toBeDeciphered, string seedToUse, string exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
// 
//     static bool VaCipher ( string & sBase64_ciphered, string toBeCiphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
//     static bool VaDecipher ( string & s_deciphered, string sBase64_toBeDeciphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
// 
//     static bool VaCipher_1 ( string & sBase64_ciphered, string toBeCiphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
//     static bool VaDecipher_1 ( string & s_deciphered, string sBase64_toBeDeciphered, string sBase64_seedToUse, string sBase64_exponentToUse, BlumInteger & blum_i, bool usePQ, int passiDaFare, int & passiFatti );
// 
//     static void FastCifraDecifra ( int & passiFatti, unsigned char * ciphered,  int lenCiphered, mpz_class & bxip1, BlumInteger & blum_i );
// 
//     static int ExtendedCipher ( const string &input, string &output, BlumInteger & blum_i );
//     static int ExtendedDecipher ( const string &input, string &output, BlumInteger & blum_i );
// 
//     static int ExtendedCipher ( const string &input, string &output, const string &p_b62, const string &q_b62 );
//     static int ExtendedDecipher ( const string &input, string &output, const string &m_b62 );
// 
//     /// <summary>
//     /// Firma con chiave privata
//     /// </summary>
//     /// <param name="t"></param>
//     /// <param name="r"></param>
//     /// <param name="pm1qm1"></param>
//     /// <param name="m"></param>
//     /// <returns></returns>
//     static string SignPrivate ( string t, int r, BlumInteger & blum_i );
// 
//     /// <summary>
//     /// Firma con sola chiave pubblica
//     /// </summary>
//     /// <param name="t"></param>
//     /// <param name="r"></param>
//     /// <param name="m"></param>
//     /// <returns></returns>
//     static string SignPublic ( string t, int r, BlumInteger & blum_i );
// 
//     /// <summary>
//     /// Verifica firma conoscendo gli steps di firma
//     /// </summary>
//     /// <param name="sign_1"></param>
//     /// <param name="r_1"></param>
//     /// <param name="sign_2"></param>
//     /// <param name="r_2"></param>
//     /// <param name="m"></param>
//     /// <returns></returns>
//     static bool VerifySignature_A ( string sign_1, int r_1, string sign_2, int r_2, BlumInteger & blum_i );
// 
//     /// <summary>
//     /// Verifica firma conoscendo le differenze tra gli steps di firma
//     /// </summary>
//     /// <param name="sign_1"></param>
//     /// <param name="r_1_minus_r_2"></param>
//     /// <param name="sign_2"></param>
//     /// <param name="r_2_minus_r1"></param>
//     /// <param name="m"></param>
//     /// <returns></returns>
//     static bool VerifySignature_B ( string sign_1, int r_1_minus_r_2, string sign_2, int r_2_minus_r1, BlumInteger & blum_i );
// 
//     /// <summary>
//     /// Cifra con chiave privata. Usa il blocco che entra e torna indietro di 1.
//     /// </summary>
//     /// <param name="t"></param>
//     /// <param name="sign"></param>
//     /// <returns></returns>
// //	static bool BlockCipher(T_Atom block, B2dVector& blockCiphered, BlumInteger & blum_i);
// 
//     /// <summary>
//     /// Decifra con chiave pubblica. Fa un passo avanti rispetto al blocco entrato.
//     /// </summary>
//     /// <param name="t"></param>
//     /// <param name="sign"></param>
//     /// <returns></returns>
// //	static bool BlockDecipher(T_Atom& block, B2dVector blockCiphered, BlumInteger & blum_i);
// 
//     /// <summary>
//     /// Decifra con chiave pubblica. Fa un passo avanti rispetto al blocco entrato. Stavolta usa la met\ufffd della lunghezza dell'ingresso
//     /// </summary>
//     /// <param name="t"></param>
//     /// <param name="sign"></param>
//     /// <returns></returns>
// //	static bool BlockDecipher2(T_Atom& blockDeciphered, B2dVector blockCiphered, BlumInteger & blum_i);
};

//class PKCS1
//{
//
//private:
//    static bool Compare(T_AtomBase array1, T_AtomBase array2);
//
//    static T_AtomBase my_xor(T_AtomBase array1, T_AtomBase array2);
//
//    static T_AtomBase emptySHA1;
//    static T_AtomBase emptySHA256;
//    static T_AtomBase emptySHA384;
//    static T_AtomBase emptySHA512;
//
//    enum hashAlgoEnum
//    {
//        e_sha1,
//        e_sha256,
//        e_sha384,
//        e_sha512
//    };
//
//    static T_AtomBase GetEmptyHash(enum hashAlgoEnum hash);
//
//    // PKCS #1 v.2.1, Section 4.1
//    // I2OSP converts a non-negative integer to an octet string of a specified length.
//    static T_AtomBase I2OSP(int x, int size);
//
//    static T_AtomBase I2OSP(T_AtomBase x, int size);
//
//    // PKCS #1 v.2.1, Section 4.2
//    // OS2IP converts an octet string to a nonnegative integer.
//    static T_AtomBase OS2IP(T_AtomBase x);
//
//    /// <summary>
//    /// Aggiunge il padding a una stringa da cifrare.
//    /// la lunghezza massima e': keysize/8 -42
//    /// </summary>
//    /// <param name="size" remarks="number of bits of m" ></param>
//    /// <param name="M" remarks="the message to be padded"></param>
//    /// <returns></returns>
//public:
//    static T_AtomBase Padding_OAEP(int size, T_AtomBase M);
//
//    /// <summary>
//    /// Toglie il padding a una stringa decifrata.
//    /// </summary>
//    /// <param name="size"  remarks="number of bits of m"></param>
//    /// <param name="C"  remarks="the message to be unpadded" ></param>
//    /// <returns></returns>
//    static T_AtomBase Unpadding_OAEP(int size, T_AtomBase C);
//
//    // PKCS #1 v.2.1, Section 7.2.1
//    // RSAES-PKCS1-V1_5-ENCRYPT ((n, e), M)
//    //static BVector Encrypt_v15(RSA rsa, RandomNumberGenerator rng, BVector M);
//
//    // PKCS #1 v.2.1, Section 7.2.2
//    // RSAES-PKCS1-V1_5-DECRYPT (K, C)
//    //static BVector Decrypt_v15(RSA rsa, BVector C);
//
//    //// PKCS #1 v.2.1, Section 8.2.1
//    //// RSASSA-PKCS1-V1_5-SIGN (K, M)
//    //public static byte[] Sign_v15(RSA rsa, HashAlgorithm hash, byte[] hashValue)
//    //{
//    //    int size = (rsa.KeySize >> 3); // div 8
//    //    byte[] EM = Encode_v15(hash, hashValue, size);
//    //    byte[] m = OS2IP(EM);
//    //    byte[] s = RSASP1(rsa, m);
//    //    byte[] S = I2OSP(s, size);
//    //    return S;
//    //}
//
//    //// PKCS #1 v.2.1, Section 8.2.2
//    //// RSASSA-PKCS1-V1_5-VERIFY ((n, e), M, S)
//    //public static bool Verify_v15(RSA rsa, HashAlgorithm hash, byte[] hashValue, byte[] signature)
//    //{
//    //    int size = (rsa.KeySize >> 3); // div 8
//    //    byte[] s = OS2IP(signature);
//    //    byte[] m = RSAVP1(rsa, s);
//    //    byte[] EM2 = I2OSP(m, size);
//    //    byte[] EM = Encode_v15(hash, hashValue, size);
//    //    bool result = Compare(EM, EM2);
//    //    if (!result)
//    //    {
//    //        // NOTE: some signatures don't include the hash OID (pretty lame but real)
//    //        // and compatible with MS implementation
//    //        if ((EM2[0] != 0x00) || (EM2[1] != 0x01))
//    //            return false;
//    //        // TODO: add more validation
//    //        byte[] decryptedHash = new GCNEW   byte[hashValue.Length];
//    //        Buffer.BlockCopy(EM2, EM2.Length - hashValue.Length, decryptedHash, 0, decryptedHash.Length);
//    //        result = Compare(decryptedHash, hashValue);
//    //    }
//    //    return result;
//    //}
//
//    //// PKCS #1 v.2.1, Section 9.2
//    //// EMSA-PKCS1-v1_5-Encode
//    //public static byte[] Encode_v15(HashAlgorithm hash, byte[] hashValue, int emLength)
//    //{
//    //    if (hashValue.Length != (hash.HashSize >> 3))
//    //        throw new GCNEW   CryptographicException("bad hash length for " + hash.ToString());
//
//    //    // DigestInfo ::= SEQUENCE {
//    //    //	digestAlgorithm AlgorithmIdentifier,
//    //    //	digest OCTET STRING
//    //    // }
//
//    //    byte[] t = null;
//
//    //    string oid = CryptoConfig.MapNameToOID(hash.ToString());
//    //    if (oid != null)
//    //    {
//    //        ASN1 digestAlgorithm = new GCNEW   ASN1(0x30);
//    //        digestAlgorithm.Add(new GCNEW   ASN1(CryptoConfig.EncodeOID(oid)));
//    //        digestAlgorithm.Add(new GCNEW   ASN1(0x05));		// NULL
//    //        ASN1 digest = new GCNEW   ASN1(0x04, hashValue);
//    //        ASN1 digestInfo = new GCNEW   ASN1(0x30);
//    //        digestInfo.Add(digestAlgorithm);
//    //        digestInfo.Add(digest);
//
//    //        t = digestInfo.GetBytes();
//    //    }
//    //    else
//    //    {
//    //        // There are no valid OID, in this case t = hashValue
//    //        // This is the case of the MD5SHA hash algorithm
//    //        t = hashValue;
//    //    }
//
//    //    Buffer.BlockCopy(hashValue, 0, t, t.Length - hashValue.Length, hashValue.Length);
//
//    //    int PSLength = System.Math.Max(8, emLength - t.Length - 3);
//    //    // PS = PSLength of 0xff
//
//    //    // EM = 0x00 | 0x01 | PS | 0x00 | T
//    //    byte[] EM = new GCNEW   byte[PSLength + t.Length + 3];
//    //    EM[1] = 0x01;
//    //    for (int i = 2; i < PSLength + 2; i++)
//    //        EM[i] = 0xff;
//    //    Buffer.BlockCopy(t, 0, EM, PSLength + 3, t.Length);
//
//    //    return EM;
//    //}
//
//    // PKCS #1 v.2.1, Section B.2.1
//    //static BVector MGF1(HashAlgorithm hash, BVector mgfSeed, int maskLen);
//};

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>

class c_rsa
{
  RSA * key;

public:
    c_rsa();
    ~c_rsa();

    ///Padding 0: pkcs1 v1.5, 1:pkcs1 v2, 2: sslv3, 3:nopadding
    bool PubCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
    bool PrivCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
    bool PubDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
    bool PrivDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );

    ///Tipo della firma. 0: 0: sha1, 1: ripemd160, 2: md5, 3: sha1+md5, 4: NID_sha224, 5: NID_sha256, 6: NID_sha384, 7: NID_sha512
    bool Sign ( unsigned char **s_signed, int * len_signed, unsigned char * digest, int len_digest, int tipo=7 );
    bool Verify ( unsigned char *s_signed, int len_signed, unsigned char * digest, int len_digest, int tipo=7 );

    //bool CifraVeloce(unsigned char ** s_cifrato, long int * len_cifrato, unsigned char * s_messaggio, long int len_messaggio);
    //bool Decifra(unsigned char ** s_decifrato, long int * len_decifrato, unsigned char * s_cifrato, long int len_cifrato);
    //void MakePQ(unsigned char * v, int dim_v);
    void MakeKey ( int bits );
    void MakeKey ( int bits, mpz_t p, mpz_t q );
    char * GetPublic();
    char * GetPrivate();
    bool SetPubKey ( const char * s );
    bool SetPrivKey ( const char * s );
};

/* DA INSERIRE
class c_ecurve
{
	EC_KEY * key;
	
public:
	c_ecurve();
	~c_ecurve();
	
	///Padding 0: pkcs1 v1.5, 1:pkcs1 v2, 2: sslv3, 3:nopadding
	bool PubCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
	bool PrivCifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
	bool PubDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
	bool PrivDecifra ( unsigned char ** s_cifrato,  int * len_cifrato, unsigned char * s_messaggio,  int len_messaggio, int padding=0 );
	
	///Tipo della firma. 0: 0: sha1, 1: ripemd160, 2: md5, 3: sha1+md5, 4: NID_sha224, 5: NID_sha256, 6: NID_sha384, 7: NID_sha512
	bool Sign ( unsigned char **s_signed, int * len_signed, unsigned char * digest, int len_digest, int tipo=7 );
	bool Verify ( unsigned char *s_signed, int len_signed, unsigned char * digest, int len_digest, int tipo=7 );
	
	//bool CifraVeloce(unsigned char ** s_cifrato, long int * len_cifrato, unsigned char * s_messaggio, long int len_messaggio);
	//bool Decifra(unsigned char ** s_decifrato, long int * len_decifrato, unsigned char * s_cifrato, long int len_cifrato);
	//void MakePQ(unsigned char * v, int dim_v);
	void MakeKey ( int bits );
	void MakeKey ( int bits, mpz_t p, mpz_t q );
	char * GetPublic();
	char * GetPrivate();
	bool SetPubKey ( const char * s );
	bool SetPrivKey ( const char * s );
};
*/


class c_hashes
{
public:
    static string hmac ( int hash, const unsigned char * key, int len_key, const unsigned char * message, int len_message )
    {
//        unsigned char *HMAC(const EVP_MD *evp_md, const void *key,
//                      int key_len, const unsigned char *d, int n,
//                      unsigned char *md, unsigned int *md_len);
        unsigned char md[EVP_MAX_MD_SIZE];
        unsigned int md_len=EVP_MAX_MD_SIZE;
        EVP_MD * evpmd;
        switch ( hash ) {
        case 0:
            evpmd= ( EVP_MD * ) EVP_sha1();
            break;
        case 1:
            evpmd= ( EVP_MD * ) EVP_sha1();
            break;
        case 2:
            evpmd= ( EVP_MD * ) EVP_sha224();
            break;
        case 3:
            evpmd= ( EVP_MD * ) EVP_sha256();
            break;
        case 4:
            evpmd= ( EVP_MD * ) EVP_sha384();
            break;
        case 5:
            evpmd= ( EVP_MD * ) EVP_sha512();
            break;
        case 6:
            evpmd= ( EVP_MD * ) EVP_md5();
            break;
        case 7:
            evpmd= ( EVP_MD * ) EVP_ripemd160();
            break;
        default:
            evpmd= ( EVP_MD * ) EVP_sha256();
            break;
        }
        HMAC ( ( const EVP_MD * ) evpmd, key, len_key, message, len_message, md, &md_len );
        return string ( md, &md[md_len] );
    }


//     static string sha ( char * s, int len )
//     {
//         unsigned char hash[SHA_DIGEST_LENGTH];
//         SHA_CTX sha;
//         SHA_Init ( &sha );
//         SHA_Update ( &sha, s, len );
//         SHA_Final ( hash, &sha );
//         
//         return string ( hash, &hash[SHA_DIGEST_LENGTH] );
//     }
    
    static string sha1 ( char * s, int len )
    {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA_CTX sha;
        SHA1_Init( &sha );
        SHA1_Update ( &sha, s, len );
        SHA1_Final ( hash, &sha );

        return string ( hash, &hash[SHA_DIGEST_LENGTH] );
    }


    static string sha224 ( char * s, int len )
    {
        unsigned char hash[SHA224_DIGEST_LENGTH];
        SHA256_CTX sha;
        SHA224_Init ( &sha );
        SHA224_Update ( &sha, s, len );
        SHA224_Final ( hash, &sha );

        return string ( hash, &hash[SHA224_DIGEST_LENGTH] );
    }

    static string sha256 ( char * s, int len )
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha;
        SHA256_Init ( &sha );
        SHA256_Update ( &sha, s, len );
        SHA256_Final ( hash, &sha );

        return string ( hash, &hash[SHA256_DIGEST_LENGTH] );
    }

    static string sha384 ( char * s, int len )
    {
        unsigned char hash[SHA384_DIGEST_LENGTH];
        SHA512_CTX sha;
        SHA384_Init ( &sha );
        SHA384_Update ( &sha, s, len );
        SHA384_Final ( hash, &sha );

        return string ( hash, &hash[SHA384_DIGEST_LENGTH] );
    }

    static string sha512 ( char * s, int len )
    {
        unsigned char hash[SHA512_DIGEST_LENGTH];
        SHA512_CTX sha;
        SHA512_Init ( &sha );
        SHA512_Update ( &sha, s, len );
        SHA512_Final ( hash, &sha );

        return string ( hash, &hash[SHA512_DIGEST_LENGTH] );
    }

    static string md5 ( char * s, int len )
    {
        unsigned char hash[MD5_DIGEST_LENGTH];
        MD5_CTX sha;
        MD5_Init ( &sha );
        MD5_Update ( &sha, s, len );
        MD5_Final ( hash, &sha );

        return string ( hash, &hash[MD5_DIGEST_LENGTH] );
    }

    static string ripemd160 ( char * s, int len )
    {
        unsigned char hash[RIPEMD160_DIGEST_LENGTH];
        RIPEMD160_CTX sha;
        RIPEMD160_Init ( &sha );
        RIPEMD160_Update ( &sha, s, len );
        RIPEMD160_Final ( hash, &sha );

        return string ( hash, &hash[RIPEMD160_DIGEST_LENGTH] );
    }
};

#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/ec.h>


class c_scyphers
{
public:
    static bool aes_cifra_js ( unsigned char ** cifrato, int *len_cifrato,
                               unsigned char * messaggio, int len_messaggio,
                               unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        //unsigned long long salt = 3414871868054505381ULL;
        //unsigned int salt[] = {12345, 54321}; //3414871868054505381ULL;
        //come unsigned char vale: 39 30 00 00 31 D4 00 00

        //int i, nrounds = 500;
        //unsigned char key[32], iv[32];
        unsigned char iv[32] = { 0x6b, 0xbd, 0xa7, 0x89, 0x2a, 0xd3, 0x44, 0xe0, 0x6c, 0x31, 0xe6, 0x45, 0x64, 0xa6, 0x9a, 0x9a,
                                 0x6b, 0xbd, 0xa7, 0x89, 0x2a, 0xd3, 0x44, 0xe0, 0x6c, 0x31, 0xe6, 0x45, 0x64, 0xa6, 0x9a, 0x9a
                               };

        unsigned char key[32];
        memset ( key, 0, 32 );

        string sp = c_hashes::sha256 ( ( char* ) key_data, key_data_len );
        for ( int i=0; i<sp.size(); ++i ) {
            key[i] = sp[i];
        }

        //i = EVP_BytesToKey(EVP_aes_256_ccm(), EVP_sha1(), (unsigned char*)salt, key_data, key_data_len, nrounds, key, iv);

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_EncryptInit_ex ( en, EVP_aes_256_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_cifrato = len_messaggio + AES_BLOCK_SIZE;
        int f_len = 0;
        *cifrato = ( unsigned char * ) malloc ( *len_cifrato );

        iret = EVP_EncryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptUpdate ( en, *cifrato, len_cifrato, messaggio, len_messaggio );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptFinal_ex ( en, *cifrato + *len_cifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_cifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    static bool aes_decifra_js ( unsigned char ** cifrato, int *len_cifrato,
                                 unsigned char * messaggio, int len_messaggio,
                                 unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        //unsigned long long salt = 3414871868054505381ULL;
        unsigned int salt[] = {12345, 54321}; //3414871868054505381ULL;
        //come unsigned char vale: 39 30 00 00 31 D4 00 00

        int i, nrounds = 500;
        //unsigned char key[32], iv[32];
        unsigned char iv[32] = { 0x6b, 0xbd, 0xa7, 0x89, 0x2a, 0xd3, 0x44, 0xe0, 0x6c, 0x31, 0xe6, 0x45, 0x64, 0xa6, 0x9a, 0x9a,
                                 0x6b, 0xbd, 0xa7, 0x89, 0x2a, 0xd3, 0x44, 0xe0, 0x6c, 0x31, 0xe6, 0x45, 0x64, 0xa6, 0x9a, 0x9a
                               };

        unsigned char key[32];
        memset ( key, 0, 32 );

        string sp = c_hashes::sha256 ( ( char* ) key_data, key_data_len );
        for ( int i=0; i<sp.size(); ++i ) {
            key[i] = sp[i];
        }

        //i = EVP_BytesToKey(EVP_aes_256_ccm(), EVP_sha1(), (unsigned char*)salt, key_data, key_data_len, nrounds, key, iv);

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_DecryptInit_ex ( en, EVP_aes_256_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_cifrato = len_messaggio + AES_BLOCK_SIZE;
        int f_len = 0;
        *cifrato = ( unsigned char * ) malloc ( *len_cifrato );

        iret = EVP_DecryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptUpdate ( en, *cifrato, len_cifrato, messaggio, len_messaggio );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptFinal_ex ( en, *cifrato + *len_cifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_cifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool aes_cifra ( unsigned char ** cifrato, int *len_cifrato, unsigned char * messaggio, int len_messaggio, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        //unsigned long long salt = 3414871868054505381ULL;
        unsigned int salt[] = {12345, 54321}; //3414871868054505381ULL;
        //come unsigned char vale: 39 30 00 00 31 D4 00 00

        int i, nrounds = 500;
        unsigned char key[32], iv[32];

        i = EVP_BytesToKey ( EVP_aes_256_ccm(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_EncryptInit_ex ( en, EVP_aes_256_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_cifrato = len_messaggio + AES_BLOCK_SIZE;
        int f_len = 0;
        *cifrato = ( unsigned char * ) malloc ( *len_cifrato );

        iret = EVP_EncryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptUpdate ( en, *cifrato, len_cifrato, messaggio, len_messaggio );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptFinal_ex ( en, *cifrato + *len_cifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_cifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool aes_decifra ( unsigned char ** decifrato, int *len_decifrato, unsigned char * cifrato, int len_cifrato, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        //unsigned long long salt = 3414871868054505381ULL;
        unsigned int salt[] = {12345, 54321}; //3414871868054505381ULL;

        int i, nrounds = 5;
        unsigned char key[32], iv[32];

        i = EVP_BytesToKey ( EVP_aes_256_cbc(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_DecryptInit_ex ( en, EVP_aes_256_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_decifrato = len_cifrato + AES_BLOCK_SIZE;
        int f_len = 0;
        *decifrato = ( unsigned char * ) malloc ( *len_decifrato );

        iret = EVP_DecryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptUpdate ( en, *decifrato, len_decifrato, cifrato, len_cifrato );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptFinal_ex ( en, *decifrato + *len_decifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_decifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool des_cifra ( unsigned char ** cifrato, int *len_cifrato, unsigned char * messaggio, int len_messaggio, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        unsigned int salt[] = {12345, 54321, 12345, 54321};

        int i, nrounds = 5;
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_KEY_LENGTH];

        i = EVP_BytesToKey ( EVP_des_ede3_cbc(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_EncryptInit_ex ( en, EVP_des_ede3_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_cifrato = len_messaggio + EVP_MAX_BLOCK_LENGTH;
        int f_len = 0;
        *cifrato = ( unsigned char * ) malloc ( *len_cifrato );

        iret = EVP_EncryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptUpdate ( en, *cifrato, len_cifrato, messaggio, len_messaggio );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptFinal_ex ( en, *cifrato + *len_cifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_cifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool des_decifra ( unsigned char ** decifrato, int *len_decifrato, unsigned char * cifrato, int len_cifrato, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        unsigned int salt[] = {12345, 54321, 12345, 54321};

        int i, nrounds = 5;
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_KEY_LENGTH];

        i = EVP_BytesToKey ( EVP_des_ede3_cbc(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_DecryptInit_ex ( en, EVP_des_ede3_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_decifrato = len_cifrato + EVP_MAX_BLOCK_LENGTH;
        int f_len = 0;
        *decifrato = ( unsigned char * ) malloc ( *len_decifrato );

        iret = EVP_DecryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptUpdate ( en, *decifrato, len_decifrato, cifrato, len_cifrato );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptFinal_ex ( en, *decifrato + *len_decifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_decifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool bl_cifra ( unsigned char ** cifrato, int *len_cifrato, unsigned char * messaggio, int len_messaggio, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        unsigned int salt[] = {12345, 54321, 12345, 54321};

        int i, nrounds = 5;
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_KEY_LENGTH];

        i = EVP_BytesToKey ( EVP_bf_cbc(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_EncryptInit_ex ( en, EVP_bf_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_cifrato = len_messaggio + EVP_MAX_BLOCK_LENGTH;
        int f_len = 0;
        *cifrato = ( unsigned char * ) malloc ( *len_cifrato );

        iret = EVP_EncryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptUpdate ( en, *cifrato, len_cifrato, messaggio, len_messaggio );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_EncryptFinal_ex ( en, *cifrato + *len_cifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_cifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }

    //Ricorda. la chiave deve essere: 128, 192 o 256 bit
    static bool bl_decifra ( unsigned char ** decifrato, int *len_decifrato, unsigned char * cifrato, int len_cifrato, unsigned char * key_data, int key_data_len )
    {
        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
        unsigned int salt[] = {12345, 54321, 12345, 54321};

        int i, nrounds = 5;
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_KEY_LENGTH];

        i = EVP_BytesToKey ( EVP_bf_cbc(), EVP_sha1(), ( unsigned char* ) salt, key_data, key_data_len, nrounds, key, iv );

        EVP_CIPHER_CTX_init ( en );
        int iret = EVP_DecryptInit_ex ( en, EVP_bf_cbc(), NULL, key, iv );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }
        *len_decifrato = len_cifrato + EVP_MAX_BLOCK_LENGTH;
        int f_len = 0;
        *decifrato = ( unsigned char * ) malloc ( *len_decifrato );

        iret = EVP_DecryptInit_ex ( en, NULL, NULL, NULL, NULL );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptUpdate ( en, *decifrato, len_decifrato, cifrato, len_cifrato );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        iret = EVP_DecryptFinal_ex ( en, *decifrato + *len_decifrato, &f_len );
        if ( iret != 1 ) {
            EVP_CIPHER_CTX_cleanup ( en );
            EVP_CIPHER_CTX_free(en);
            return false;
        }

        *len_decifrato += f_len;

        EVP_CIPHER_CTX_cleanup ( en );
        EVP_CIPHER_CTX_free(en);
        return true;
    }
};
