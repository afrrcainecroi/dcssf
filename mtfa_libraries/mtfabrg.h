#pragma once


#if 1

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <mutex>
//#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include "setunion.h"

#include <mtfa_utils.h>

using namespace std;

uint64_t millis();
uint64_t micros();
uint64_t nanos();

vector<char> StrongRandom(int num_bytes);

#if 0
class mtfa_fastrng64
{
private:
	mutex * mtx;
	unsigned long long x; // The state can be seeded with any value.
public:
	~mtfa_fastrng64() { delete mtx; };
	mtfa_fastrng64() { x = 0ULL; mtx = new mutex(); };
	mtfa_fastrng64(unsigned long long seed) { x = seed; mtx = new mutex(); };
	unsigned long long next();
};

class mtfa_fastrng1024
{
private:
	mutex * mtx;
	unsigned long long s[16];
	int p;

public:
	mtfa_fastrng1024();
	mtfa_fastrng1024(unsigned long long);
	unsigned long long next(void);
	void jump(void);
};
#endif


#if 0
/*
 * BSetUnion e generatore kolmogorof
 */
class BSetUnion
{
	//in questa versione utilizziamo int64 per memorizzare i posti liberi e occupati
	//quindi 2 livelli: vettore e uint64. Inoltre aggiungiamo una funzione di accumulazione dei bit in un cluster da 64,
	//avendo cura di utilizzare solo il risultato finale dopo 64 insert
private:
	int bitAfterLastBitLastCall;
	unsigned int oldAddress;
public:

	//Da 0 a 63. Seleziona tutti i bit più significativi di quello indicato. 0 indica prendi dallo 0 al 63
	static constexpr unsigned long long /*__int128*/ BitMaskLH[] =
	{
		0XFFFFFFFFFFFFFFFFULL, 0XFFFFFFFFFFFFFFFEULL, 0XFFFFFFFFFFFFFFFCULL, 0XFFFFFFFFFFFFFFF8ULL, 0XFFFFFFFFFFFFFFF0ULL,
		0XFFFFFFFFFFFFFFE0ULL, 0XFFFFFFFFFFFFFFC0ULL, 0XFFFFFFFFFFFFFF80ULL, 0XFFFFFFFFFFFFFF00ULL, 0XFFFFFFFFFFFFFE00ULL,
		0XFFFFFFFFFFFFFC00ULL, 0XFFFFFFFFFFFFF800ULL, 0XFFFFFFFFFFFFF000ULL, 0XFFFFFFFFFFFFE000ULL, 0XFFFFFFFFFFFFC000ULL,
		0XFFFFFFFFFFFF8000ULL, 0XFFFFFFFFFFFF0000ULL, 0XFFFFFFFFFFFE0000ULL, 0XFFFFFFFFFFFC0000ULL, 0XFFFFFFFFFFF80000ULL,
		0XFFFFFFFFFFF00000ULL, 0XFFFFFFFFFFE00000ULL, 0XFFFFFFFFFFC00000ULL, 0XFFFFFFFFFF800000ULL, 0XFFFFFFFFFF000000ULL,
		0XFFFFFFFFFE000000ULL, 0XFFFFFFFFFC000000ULL, 0XFFFFFFFFF8000000ULL, 0XFFFFFFFFF0000000ULL, 0XFFFFFFFFE0000000ULL,
		0XFFFFFFFFC0000000ULL, 0XFFFFFFFF80000000ULL, 0XFFFFFFFF00000000ULL, 0XFFFFFFFE00000000ULL, 0XFFFFFFFC00000000ULL,
		0XFFFFFFF800000000ULL, 0XFFFFFFF000000000ULL, 0XFFFFFFE000000000ULL, 0XFFFFFFC000000000ULL, 0XFFFFFF8000000000ULL,
		0XFFFFFF0000000000ULL, 0XFFFFFE0000000000ULL, 0XFFFFFC0000000000ULL, 0XFFFFF80000000000ULL, 0XFFFFF00000000000ULL,
		0XFFFFE00000000000ULL, 0XFFFFC00000000000ULL, 0XFFFF800000000000ULL, 0XFFFF000000000000ULL, 0XFFFE000000000000ULL,
		0XFFFC000000000000ULL, 0XFFF8000000000000ULL, 0XFFF0000000000000ULL, 0XFFE0000000000000ULL, 0XFFC0000000000000ULL,
		0XFF80000000000000ULL, 0XFF00000000000000ULL, 0XFE00000000000000ULL, 0XFC00000000000000ULL, 0XF800000000000000ULL,
		0XF000000000000000ULL, 0XE000000000000000ULL, 0XC000000000000000ULL, 0X8000000000000000ULL
	};

	//Da 0 a 63. Seleziona tutti i bit meno significativi di quello dato. 0: seleziona il meno significativo
	//63 prendili dal bit 63 al bit 0
	static constexpr unsigned long long BitMaskHL[] =
	{
		0X0000000000000001ULL, 0X0000000000000003ULL, 0X0000000000000007ULL, 0X000000000000000FULL, 0X000000000000001FULL,
		0X000000000000003FULL, 0X000000000000007FULL, 0X00000000000000FFULL, 0X00000000000001FFULL, 0X00000000000003FFULL,
		0X00000000000007FFULL, 0X0000000000000FFFULL, 0X0000000000001FFFULL, 0X0000000000003FFFULL, 0X0000000000007FFFULL,
		0X000000000000FFFFULL, 0X000000000001FFFFULL, 0X000000000003FFFFULL, 0X000000000007FFFFULL, 0X00000000000FFFFFULL,
		0X00000000001FFFFFULL, 0X00000000003FFFFFULL, 0X00000000007FFFFFULL, 0X0000000000FFFFFFULL, 0X0000000001FFFFFFULL,
		0X0000000003FFFFFFULL, 0X0000000007FFFFFFULL, 0X000000000FFFFFFFULL, 0X000000001FFFFFFFULL, 0X000000003FFFFFFFULL,
		0X000000007FFFFFFFULL, 0X00000000FFFFFFFFULL, 0X00000001FFFFFFFFULL, 0X00000003FFFFFFFFULL, 0X00000007FFFFFFFFULL,
		0X0000000FFFFFFFFFULL, 0X0000001FFFFFFFFFULL, 0X0000003FFFFFFFFFULL, 0X0000007FFFFFFFFFULL, 0X000000FFFFFFFFFFULL,
		0X000001FFFFFFFFFFULL, 0X000003FFFFFFFFFFULL, 0X000007FFFFFFFFFFULL, 0X00000FFFFFFFFFFFULL, 0X00001FFFFFFFFFFFULL,
		0X00003FFFFFFFFFFFULL, 0X00007FFFFFFFFFFFULL, 0X0000FFFFFFFFFFFFULL, 0X0001FFFFFFFFFFFFULL, 0X0003FFFFFFFFFFFFULL,
		0X0007FFFFFFFFFFFFULL, 0X000FFFFFFFFFFFFFULL, 0X001FFFFFFFFFFFFFULL, 0X003FFFFFFFFFFFFFULL, 0X007FFFFFFFFFFFFFULL,
		0X00FFFFFFFFFFFFFFULL, 0X01FFFFFFFFFFFFFFULL, 0X03FFFFFFFFFFFFFFULL, 0X07FFFFFFFFFFFFFFULL, 0X0FFFFFFFFFFFFFFFULL,
		0X1FFFFFFFFFFFFFFFULL, 0X3FFFFFFFFFFFFFFFULL, 0X7FFFFFFFFFFFFFFFULL, 0XFFFFFFFFFFFFFFFFULL
	};

	static constexpr unsigned long long SetBitMask[] =
	{
		0X0000000000000001ULL, 0X0000000000000002ULL, 0X0000000000000004ULL, 0X0000000000000008ULL, 0X0000000000000010ULL,
		0X0000000000000020ULL, 0X0000000000000040ULL, 0X0000000000000080ULL, 0X0000000000000100ULL, 0X0000000000000200ULL,
		0X0000000000000400ULL, 0X0000000000000800ULL, 0X0000000000001000ULL, 0X0000000000002000ULL, 0X0000000000004000ULL,
		0X0000000000008000ULL, 0X0000000000010000ULL, 0X0000000000020000ULL, 0X0000000000040000ULL, 0X0000000000080000ULL,
		0X0000000000100000ULL, 0X0000000000200000ULL, 0X0000000000400000ULL, 0X0000000000800000ULL, 0X0000000001000000ULL,
		0X0000000002000000ULL, 0X0000000004000000ULL, 0X0000000008000000ULL, 0X0000000010000000ULL, 0X0000000020000000ULL, 0X0000000040000000ULL,
		0X0000000080000000ULL, 0X0000000100000000ULL, 0X0000000200000000ULL, 0X0000000400000000ULL, 0X0000000800000000ULL, 0X0000001000000000ULL,
		0X0000002000000000ULL, 0X0000004000000000ULL, 0X0000008000000000ULL, 0X0000010000000000ULL, 0X0000020000000000ULL, 0X0000040000000000ULL,
		0X0000080000000000ULL, 0X0000100000000000ULL, 0X0000200000000000ULL, 0X0000400000000000ULL, 0X0000800000000000ULL, 0X0001000000000000ULL,
		0X0002000000000000ULL, 0X0004000000000000ULL, 0X0008000000000000ULL, 0X0010000000000000ULL, 0X0020000000000000ULL, 0X0040000000000000ULL,
		0X0080000000000000ULL, 0X0100000000000000ULL, 0X0200000000000000ULL, 0X0400000000000000ULL, 0X0800000000000000ULL, 0X1000000000000000ULL,
		0X2000000000000000ULL, 0X4000000000000000ULL, 0X8000000000000000ULL
	};

	static const unsigned long long fullCluster = 0xFFFFFFFFFFFFFFFF;
	int size;   //quanti ce ne sono in totale
	unsigned long long total;
	typedef struct __range__
	{
		int size;
		int firstFreeOnRight;
		int lastBusyOnLeft;
		__range__()
		{
			size = 0;
			firstFreeOnRight = 0;
			lastBusyOnLeft = 0;
		}
	} TRange;
	TRange** ranges;
	TRange* the_ranges;
	unsigned long long* values_left;
	unsigned char* occupied_left;
	unsigned long long* values_right;
	unsigned char* occupied_right;
	unsigned char* occupied_lr;
	pair<int, int> Index(int idx);

	void Dump();
	//Il size è in bit!
	int Occupied(const int i);
	void setBitValue(const unsigned int index, const bool value);
	BSetUnion(int _size);
	~BSetUnion();
	//Inserisce a un indirizzo di bit, all'interno lo converto in indirizzo di cluster (64bit)
	bool InsertValue(int _index, bool value, bool right); //right means: grow to right otherwise grow to left
	/**
	 * @brief Cerca il prossimo bit "occupato" (inserito nella struttura) a partire dalla posizione index
	 *
	 * @param index p_index: posizione da cui partire
	 * @param occupied p_occupied: indica se cercare occupato oppure libero
	 * @param avanti p_avanti: indica la direzione di ricerca (avanti/indietro)
	 * @return int: torna la posizione del bit "occupato"
	 *
	 * Considerare che i bit messi nella parola i 64 bit sono aggiunti a "sinistra" oppure a "destra"
	 * e sono considerati crescenti da sinistra a destra. Quindi i bit di destra ???
	 */
	int getAddressOfNextBit(const unsigned int index, const bool occupied, const bool avanti);

	//presuppongo che il bit sia occupato!!!
	bool getBitValue(const unsigned int index);
	unsigned int getNBitsFromIndex(const int idx, int numBits);

	//da 0 a 63
	void SetBit(unsigned long long& w, //the word to modify:  if (f) w |= m; else w &= ~m;
		const unsigned int _m, // the bit mask
		const bool f           // conditional flag
	);
	pair<int, int> GetIndex(int i);

	list<pair<int, int>> EnumerateSets();

	boost::dynamic_bitset<> ToBitSet();
};


class Pushing
{
public:
	vector<unsigned char> v;
	unsigned char byte;
	int numbit;
	Pushing();
	void push ( int bit );
};
#endif

class KGenerator
{
private:

public:
// 	static const int HALF_KEY_SIZE = 1024;
// 	static const int FULL_KEY_SIZE = 65536;

	typedef struct __TBrgKey__ {
		boost::dynamic_bitset<> Va;
		boost::dynamic_bitset<> Va1;
		boost::dynamic_bitset<> Vb;
		boost::dynamic_bitset<> Vb1;
	} TBrgKey;
	
// 	KGenerator(){};
// 	
// 	~KGenerator();
	
	static int Log2 ( unsigned long long _v );

	//static void PreparePa(SetUnion& nva, Pushing& pa);

	static string ToString ( const vector<bool>& v );
	
	static void RotateBit(bool& a, bool& b, bool& a1, bool& b1, bool direction);
	
	static tuple<int, int, int, int> EvalABXY(BooleanBuffer& nva,
		BooleanBuffer& nvb,
		BooleanBuffer& nvx,
		BooleanBuffer& nvy,
		const unsigned int ia,
		const unsigned int ib,
		const unsigned int ix,
		const unsigned int iy,
		const unsigned int ia1,
		const unsigned int ib1,
		const unsigned int ix1,
		const unsigned int iy1);
	static pair<int, int> GetNewAddressesAB(BooleanBuffer& vx,
		BooleanBuffer& vy,
		int ix, int iy,
		unsigned long long currentSize,
		int addressSize);
	static void MakeSpace(const boost::dynamic_bitset<>& va, const boost::dynamic_bitset<>& vx, BooleanBuffer& nva);

	static TBrgKey MakeKey (int keylen, unsigned long long seed);
    static TBrgKey MakeKey (int keylen, const vector<char> & v , int * pos, mtfa_fastrng1024 & r);

	static pair<TBrgKey, TBrgKey> mfa_generator ( TBrgKey originalKey, TBrgKey hiddenKey );
};

//Il vantaggio è che le dimensioni sono sempre multiplo di 64
static vector<unsigned long long> ConvertBitSetToVUint64 ( const boost::dynamic_bitset<>& bs );
static vector<unsigned char> ConvertBitSetToVUint8 ( const boost::dynamic_bitset<>& bs );
static boost::dynamic_bitset<> GiveSecondHalfBitSet (const boost::dynamic_bitset<> &setin);
static vector<unsigned char> GetFirstHalfAsVbyte(const boost::dynamic_bitset<> &setin);

class BRG {
private:
public:
	string id;
	int keylen;
	KGenerator::TBrgKey kOriginal;
	KGenerator::TBrgKey kHidden;
	KGenerator::TBrgKey newOriginal;
	KGenerator::TBrgKey newHidden;
	
	vector<unsigned char>bufOriginal;
	int steps;
	
	BRG (string id, unsigned long long seed);
	BRG (string id, const vector<char> & v);
	//BRG (string id, int keylen, int keygen, vector<unsigned char> bytes);
	void Next();
	string GetId();
	string GetBufAsBase64String();
	vector<unsigned char> GetBufAsVByte();
};






















#else   //vecchia versione

#include <iostream>
#include <string>
#include <list>
#include <vector>
//#include <bitset>
#include <boost/dynamic_bitset.hpp>

#include <mtfa_utils.h>

using namespace std;

class Pushing
{
public:
	vector<unsigned char> v;
	unsigned char byte;
	int numbit;
	Pushing();
	void push ( int bit );
};

class KGenerator
{
private:

public:
// 	static const int HALF_KEY_SIZE = 1024;
// 	static const int FULL_KEY_SIZE = 65536;

	typedef struct __TBrgKey__ {
		boost::dynamic_bitset<> Va;
		boost::dynamic_bitset<> Va1;
		boost::dynamic_bitset<> Vb;
		boost::dynamic_bitset<> Vb1;
	} TBrgKey;
	
// 	KGenerator(){};
// 	
// 	~KGenerator();
	
	static int Log2 ( unsigned long long _v );

	static void PreparePa ( BSetUnion& nva, Pushing& pa );
	
	static string ToString ( const vector<bool>& v );
	
	static void RotateBit ( bool& a,
					 bool& b,
				  bool& a1,
				  bool& b1,
				  bool direction
	);
	
	static tuple<int, int, int, int> EvalABXY ( BSetUnion& nva,
										 BSetUnion& nvb,
									  BSetUnion& nvx,
									  BSetUnion& nvy,
									  const unsigned int ia,
									  const unsigned int ib,
									  const unsigned int ix,
									  const unsigned int iy,
									  const unsigned int ia1,
									  const unsigned int ib1,
									  const unsigned int ix1,
									  const unsigned int iy1 );
	static pair<int, int> GetNewAddressesAB ( BSetUnion& vx,
							 BSetUnion& vy,
						  int ix, int iy,
						  unsigned long long currentSize,
						  int addressSize);
	static void MakeSpace ( const boost::dynamic_bitset<>& va, const boost::dynamic_bitset<>& vx, unsigned long long size, BSetUnion& nva );

	static TBrgKey MakeKey (int keylen, unsigned long long seed);
    static TBrgKey MakeKey (int keylen, vector<char> & v , int * pos, mtfa_fastrng1024 & r);

	static pair<TBrgKey, TBrgKey> mfa_generator ( unsigned long long finalSize, TBrgKey originalKey, TBrgKey hiddenKey );
};

//Il vantaggio è che le dimensioni sono sempre multiplo di 64
static vector<unsigned long long> ConvertBitSetToVUint64 ( const boost::dynamic_bitset<>& bs );
static vector<unsigned char> ConvertBitSetToVUint8 ( const boost::dynamic_bitset<>& bs );
static boost::dynamic_bitset<> GiveSecondHalfBitSet (const boost::dynamic_bitset<> &setin);
static vector<unsigned char> GetFirstHalfAsVbyte(const boost::dynamic_bitset<> &setin);

class BRG {
private:
public:
	string id;
	int keylen;
	int keygen;
	KGenerator::TBrgKey kOriginal;
	KGenerator::TBrgKey kHidden;
	KGenerator::TBrgKey newOriginal;
	KGenerator::TBrgKey newHidden;
	
	vector<unsigned char>bufOriginal;
	int steps;
	
	BRG (string id, unsigned long long seed);
	BRG (string id, vector<char> & v);
	//BRG (string id, int keylen, int keygen, vector<unsigned char> bytes);
	void Next();
	string GetId();
	string GetBufAsBase64String();
	vector<unsigned char> GetBufAsVByte();
};
#endif
