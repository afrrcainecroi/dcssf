#if 1

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <mutex>
#include <chrono>
#include <random>
#include <algorithm>
#include <iterator>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <sys/random.h>
#endif

#include "base64.h"
#include "mtfa_utils.h"
#include "mtfabrg.h"
#include "setunion.h"

using namespace std;

extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];

// Get time stamp in milliseconds.
uint64_t millis()
{
	uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::
		now().time_since_epoch()).count();
	return ms;
}

// Get time stamp in microseconds.
uint64_t micros()
{
	uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
		now().time_since_epoch()).count();
	return us;
}

// Get time stamp in nanoseconds.
uint64_t nanos()
{
	uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::
		now().time_since_epoch()).count();
	return ns;
}

vector<char> StrongRandom(int num_bytes)
{
	vector<char> data(num_bytes);

#ifdef _WIN32
	HCRYPTPROV hCryptProv = NULL;        // handle for a cryptographic
	if (CryptAcquireContext(
		&hCryptProv,               // handle to the CSP
		NULL,                     // container name
		NULL,                      // use the default provider
		PROV_RSA_FULL,             // provider type
		CRYPT_VERIFYCONTEXT | CRYPT_SILENT))                      // flag values
	{
		if (!CryptGenRandom(hCryptProv, num_bytes, (unsigned char*)data.data()))
		{
			//devo riempire in altro modo.
			mtfa_fastrng1024* r = new mtfa_fastrng1024(micros());

			for (int i = 0; i < num_bytes; ++i)
			{
				data[i] = r->next() & 0x00000000000000FF;
			}
			delete (r);
		}

		CryptReleaseContext(hCryptProv, 0);
	}
#else
    int len = 0;
    char * buf = new char[num_bytes]();
    while (len < data.size())
        len += getrandom(&buf[len], num_bytes-len, 0);
    std::copy(buf, buf+num_bytes, data.begin());
#endif
    return data;
}

#if 0
unsigned long long mtfa_fastrng64::next()
{
	lock_guard<mutex> lock(*mtx);
	unsigned long long  z = (x += 0x9E3779B97F4A7C15ULL);
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
	return z ^ (z >> 31);
}

mtfa_fastrng1024::mtfa_fastrng1024()
{
	mtx = new mutex();
	p = 0;
	mtfa_fastrng64 r(0);

	for (int i = 0; i < 16; ++i)
		s[i] = r.next();
};
mtfa_fastrng1024::mtfa_fastrng1024(unsigned long long seed)
{
	mtx = new mutex();
	p = 0;
	mtfa_fastrng64 r(seed);

	for (int i = 0; i < 16; ++i)
		s[i] = r.next();
};

unsigned long long mtfa_fastrng1024::next(void)
{
	lock_guard<mutex> lock(*mtx);
	const unsigned long long s0 = s[p];
	unsigned long long s1 = s[p = (p + 1) & 0x0F];
	s1 ^= s1 << 31; // a
	s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c
	return s[p] * 1181783497276652981ULL;

	//11400714819323198483ULL
}


/* This is the jump function for the generator. It is equivalent
 *  to 2^512 calls to next(); it can be used to generate 2^512
 *  non-overlapping subsequences for parallel computations. */

void mtfa_fastrng1024::jump(void)
{
	lock_guard<mutex> lock(*mtx);
	static const unsigned long long JUMP[] = { 0x84242f96eca9c41dULL,
											   0xa3c65b8776f96855ULL, 0x5b34a39f070b5837ULL, 0x4489affce4f31a1eULL,
											   0x2ffeeb0a48316f40ULL, 0xdc2d9891fe68c022ULL, 0x3659132bb12fea70ULL,
											   0xaac17d8efa43cab8ULL, 0xc4cb815590989b13ULL, 0x5ee975283d71c93bULL,
											   0x691548c86c1bd540ULL, 0x7910c41d10a1e6a5ULL, 0x0b5fc64563b3e2a8ULL,
											   0x047f7684e9fc949dULL, 0xb99181f2d8f685caULL, 0x284600e3f30e38c3ULL
	};

	unsigned long long t[16] = { 0 };

	for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
		for (int b = 0; b < 64; b++)
		{
			if (JUMP[i] & 1ULL << b)
				for (int j = 0; j < 16; j++)
					t[j] ^= s[(j + p) & 15];

			next();
		}

	for (int j = 0; j < 16; j++)
		s[(j + p) & 15] = t[j];
}
#endif

#if 0
/*
 * SetUnion e generatore kolmogorof
 */
pair<int, int> BSetUnion::Index(int idx)
{
	return make_pair(idx >> 6, idx & 63);
}

void BSetUnion::setBitValue(const unsigned int index, const bool value)
{
	auto pos = GetIndex(index);

	if (occupied_left[pos.first] > pos.second)
	{
		if (value)
		{
			values_left[pos.first] |= SetBitMask[pos.second];
		}
		else
		{
			values_left[pos.first] &= ~SetBitMask[pos.second];
		}

		return;
	}

	if (occupied_right[pos.first] > (pos.second - occupied_left[pos.first]))
	{
		if (value)
		{
			values_right[pos.first] |= SetBitMask[pos.second - occupied_left[pos.first]];
		}
		else
		{
			values_right[pos.first] &= ~SetBitMask[pos.second - occupied_left[pos.first]];
		}
	}
	else
		throw "Errore il bit non esiste";
}

void BSetUnion::Dump()
{
	if (ranges[0] != nullptr)
	{
		printf("Ranges (size, lastBusyOnLeft, FirstFreeOnRight, Value)\n");

		for (int i = 0; i < size; ++i)
		{
			printf("%4d) %4d \t%4d \t%4d \t%016llX(%d)\t%016llX(%d)\n",
				i,
				ranges[i]->size,
				ranges[i]->lastBusyOnLeft,
				ranges[i]->firstFreeOnRight,
				values_left[i], occupied_left[i],
				values_right[i], occupied_right[i]
			);
		}
	}

	cout << endl;
}

//Il size è in bit!
BSetUnion::BSetUnion(int _size)
{
	bitAfterLastBitLastCall = -1;

	size = _size >> 6;
	total = 0;

	ranges = new TRange*[size](); //vector<TRange*>(size, nullptr);
	the_ranges = new TRange[size](); //vector<TRange>(size);
	values_left = new unsigned long long[size];
	values_right = new unsigned long long[size];
	occupied_left = new unsigned char[size];
	occupied_right = new unsigned char[size];
	occupied_lr = new unsigned char[size];
	fill(occupied_left, occupied_left + size, 0);
	fill(occupied_right, occupied_right + size, 0);
	fill(occupied_lr, occupied_lr + size, 0);
	fill(values_left, values_left + size, 0ULL);
	fill(values_right, values_right + size, 0ULL);

	//All'inizio tutti i posti sono vuoti
	for (int i = 0; i < size; ++i)
	{
		ranges[i] = &the_ranges[i];
		ranges[i]->size = 0; //tieni presente che finchè non raggiunge i 64, no problems, inserisco nel ull
		ranges[i]->firstFreeOnRight = i;
		ranges[i]->lastBusyOnLeft = i;
	}
}

BSetUnion::~BSetUnion()
{
	delete[] the_ranges;
	delete[] ranges;
	delete[] values_left;
	delete[] values_right;
	delete[] occupied_left;
	delete[] occupied_right;
	delete[] occupied_lr;
}

int BSetUnion::Occupied(const int i)
{
	return occupied_lr[i];
	//return occupied_right[i] + occupied_left[i];
}

//Inserisce a un indirizzo di bit, all'interno lo converto in indirizzo di cluster (64bit)
bool BSetUnion::InsertValue(int _index, bool value, bool right) //right means: grow to right otherwise grow to left
{
	unsigned int index = _index >> 6; /// 64;

	//Quando è piena la struttura? Quando è rimasto un solo puntatore a null
	if (ranges[index] == nullptr || total >= (size << 6))
	{
		cout << "Terminata!\n" << endl;
		throw "Errore, spazio terminato!";
	}

	int firstNotFull;

	//Da quale lato devo inserire? Per costruzione ho sempre spazio per inserire
	if (right)
	{
		firstNotFull = ranges[index]->firstFreeOnRight;
		values_left[firstNotFull] = values_left[firstNotFull] << 1 | (value ? 1 : 0);
		occupied_left[firstNotFull]++;
		occupied_lr[firstNotFull]++;
	}
	else
	{
		//devo inserire a sinistra
		int idx_sinistra = ranges[index]->lastBusyOnLeft - 1;

		if (idx_sinistra < 0)
			idx_sinistra = size - 1;

		firstNotFull = ranges[idx_sinistra]->firstFreeOnRight;
		values_right[firstNotFull] = values_right[firstNotFull] << 1 | (value ? 1 : 0);
		occupied_right[firstNotFull]++;
		occupied_lr[firstNotFull]++;
	}

	//ne ho aggiunto 1 a firstNotFull, ora devo fare la union?
	total++;

	if (Occupied(firstNotFull) < 64)
		return true;

	//Devo fare la union sempre a destra di firstNotNull
	int idx_sinistra, idx_destra;

	idx_sinistra = firstNotFull;
	idx_destra = (ranges[firstNotFull]->firstFreeOnRight + 1) % size;

	ranges[firstNotFull]->size++;

	if (ranges[firstNotFull]->size >= size)
		return true; //non devo fare union, non c'è più spazio

	auto num_sinistra = ranges[idx_sinistra]->size;
	auto num_destra = ranges[idx_destra]->size;

	if ((num_destra + num_sinistra) >= size)
		return true; //non devo fare union, non c'è più spazio

	if (num_sinistra >= num_destra)
	{
		ranges[idx_sinistra]->size += num_destra;
		//cout << ranges[idx_sinistra]->firstFreeOnRight << ", " << ranges[idx_destra]->firstFreeOnRight << endl;
		ranges[idx_sinistra]->firstFreeOnRight = ranges[idx_destra]->firstFreeOnRight;

		int start, howmany;
		start = ranges[idx_destra]->lastBusyOnLeft;
		howmany = ranges[idx_destra]->size;

		for (int i = 0; i <= howmany; ++i)
			ranges[(i + start) % size] = ranges[idx_sinistra];

		return true;
	}
	else
	{
		ranges[idx_destra]->size += num_sinistra;
		ranges[idx_destra]->lastBusyOnLeft = ranges[idx_sinistra]->lastBusyOnLeft;

		int start, howmany;
		start = ranges[idx_sinistra]->lastBusyOnLeft;
		howmany = ranges[idx_sinistra]->size;

		for (int i = 0; i <= howmany; ++i)
			ranges[(i + start) % size] = ranges[idx_destra];

		return true;
	}

}

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
int BSetUnion::getAddressOfNextBit(const unsigned int index, const bool occupied, const bool avanti)
{
	//a partire da index (indirizzo lungo) cerca la posizione del prossimo bit occupato o libero
	auto pos = GetIndex(index);
	pos.first %= size;

	//consideriamo come se gli occupati a sinistra proseguano negli occupati a destra
	if (avanti)
	{
		if (occupied)
		{
			while (true)
			{
				//cerco il primo occupato a partire dall'indirizzo dato
				if (Occupied(pos.first) > pos.second)
				{
					//Ci sono più occupati di quanto richiesto
					//                             if (Opos.first != pos.first || Opos.second != pos.second)
					//                             {
					//                                 cout << "C1(" << Opos.first << ", " << Opos.second << "): " << (Opos.first << 6) + Opos.second << endl;
					//                                 cout << "C2(" << pos.first << ", " << pos.second << "): " << (pos.first << 6) + pos.second << endl;
					//                             }
					return (pos.first << 6) + pos.second;
				}
				else
				{
					//Devo andare al prossimo cluster
					pos.second = 0;
					pos.first = (pos.first + 1) % size;
					continue;
				}
			}
		}
		else
		{
			//cerco il primo indirizzo libero a partire da quello dato
			while (true)
			{
				//se non sono tutti occupati allora ce nè uno libero
				if (Occupied(pos.first) < pos.second)
					return (pos.first << 6) + pos.second;

				if (Occupied(pos.first) < 64)
					return (pos.first << 6) + Occupied(pos.first);

				pos.second = 0;
				pos.first = (pos.first + 1) % size;
			}
		}
	}
	else
	{
		//cerca indietro
		if (occupied)
		{
			while (true)
			{
				//cerco il primo occupato a partire dalla posizione data andando verso sinistra
				if (Occupied(pos.first) > pos.second)
					return (pos.first << 6) + pos.second;

				//ma ce ne sono di occupati?
				if (Occupied(pos.first) > 0)
					return (pos.first << 6) + Occupied(pos.first);

				//ho esaurito gli occupati a sinistra, passo al cluster precedente
				pos.second = 63;
				pos.first = pos.first - 1;

				if (pos.first < 0)
					pos.first = size - 1;

				continue;
			}
		}
		else
		{
			//cerco il primo indirizzo libero a partire da quallo dato verso sinistra !
			while (true)
			{
				if (Occupied(pos.first) < pos.second)
					return (pos.first << 6) + pos.second;

				//ho esaurito i liberi a sinistra, passo al cluster precedente
				pos.second = 63;
				pos.first = pos.first - 1;

				if (pos.first < 0)
					pos.first = size - 1;

				continue;
			}
		}
	}
}

//presuppongo che il bit sia occupato!!!
bool BSetUnion::getBitValue(const unsigned int index)
{
	auto pos = GetIndex(index);

	if (occupied_left[pos.first] > pos.second)
	{
		bool val = (SetBitMask[pos.second] & values_left[pos.first]) ? true : false;
		return val;
	}

	if (occupied_right[pos.first] > (pos.second - occupied_left[pos.first]))
	{
		bool val = (SetBitMask[pos.second - occupied_left[pos.first]] & values_right[pos.first]) ? true : false;
		return val;
	}
	else
		throw "Errore il bit non esiste";
}

unsigned int BSetUnion::getNBitsFromIndex(const int idx, int numBits)
{
	//ricordo l'ultimo bit della chiamata precedente (quello dopo l'ultimo)
	//cout << "Chiede " << numBits << " a partire da " << idx << endl;
	bitAfterLastBitLastCall = -1;
	if (bitAfterLastBitLastCall < 0)
	{
		unsigned int address = 0;
		int pos = idx;

		while (numBits-- > 0)
		{
			pos = getAddressOfNextBit(pos, true, true);
			bool value = getBitValue(pos);
			address = (address << 1) | (value ? 1 : 0);
			pos = (pos + 1) % (size << 6);
		}

		bitAfterLastBitLastCall = pos;
		oldAddress = address;
		return address;
	}
	else
	{
		// mi servono due bit a partire da idx e 1 bit da bitAfter..
		bool bithh, bith, bitl;
		int pos = idx;
		pos = getAddressOfNextBit(pos, true, true);
		bithh = getBitValue(pos);
		pos = (pos + 1) % (size << 6);
		pos = getAddressOfNextBit(pos, true, true);
		bith = getBitValue(pos);
		pos = (pos + 1) % (size << 6);

		//ora mi serve l'ultimo bit
		pos = getAddressOfNextBit(bitAfterLastBitLastCall, true, true);
		bitl = getBitValue(pos);
		pos = (pos + 1) % (size << 6);
		bitAfterLastBitLastCall = pos;

		//ora devo modificare il vecchio address
		if (bithh)
			oldAddress |= SetBitMask[numBits - 1];
		else
			oldAddress &= ~SetBitMask[numBits - 1];

		if (bith)
			oldAddress |= SetBitMask[numBits - 2];
		else
			oldAddress &= ~SetBitMask[numBits - 2];

		if (bitl)
			oldAddress |= 1;
		else
			oldAddress &= ~SetBitMask[0];

		return oldAddress;
	}
}

//da 0 a 63
void BSetUnion::SetBit(unsigned long long& w, //the word to modify:  if (f) w |= m; else w &= ~m;
	const unsigned int _m, // the bit mask
	const bool f           // conditional flag
)
{
	unsigned long long m = SetBitMask[_m];
	w = (w & ~m) | (-f & m);
}

pair<int, int> BSetUnion::GetIndex(int i)
{
	return make_pair(i >> 6, i & 63);
}

//posizione, dimensione dell'insieme
list<pair<int, int>> BSetUnion::EnumerateSets()
{
	list<pair<int, int>> li;

	//parto dalla posizione 0 e poi avanti fino alla fine
	//ovvio che non è detto che in posizione 0 ci sia qualcosa e quindi inizio a cercare,
	//al primo occupato, lo utilizzo e poi proseguo di conserva
	int idx = 0;

	while (true)      //Quando gira ho terminato, poichè sono partito da 0
	{
		if (Occupied(idx) == 0)
		{
			++idx;
			if (idx >= size)
				break;
			else
				continue;
		}

		pair<int, int> currentPair;
		//se sono qui, c'è qualcosa.
		//devo valutare il range di occupazione
		int left = the_ranges[idx].lastBusyOnLeft;      //se == idx, allora inizia qui
		int right = the_ranges[idx].firstFreeOnRight;   //se == idx, allora termina qui

		if (left <= right)
		{
			currentPair = make_pair(left << 6, ((right - left) << 6) + Occupied(right));
			li.push_back(currentPair);
			idx = right + 1;
		}
		else
		{
			//ho girato, devo terminare
			//ho pieni da left a size
			//e poi da 0 a right, incluso
			currentPair = make_pair(left << 6, ((size - left) << 6) + (right << 6) + Occupied(right));
			li.push_back(currentPair);
			break;
		}
	}

	return li;
}


boost::dynamic_bitset<> concatLoopDyn(const boost::dynamic_bitset<>& bs1, const boost::dynamic_bitset<>& bs2)
{
	boost::dynamic_bitset<> res(bs1);
	res.resize(bs1.size() + bs2.size());
	size_t bs1Size = bs1.size();
	size_t bs2Size = bs2.size();

	for (size_t i = 0; i < bs2Size; i++)
		res.set(i + bs1Size, bs2[i]);
	return res;
}


boost::dynamic_bitset<> BSetUnion::ToBitSet() {

	boost::dynamic_bitset<> btot;

	int totale = 0;
	for (int i = 0; i < size; ++i) {
		if (Occupied(i) <= 0) {
			continue;
		}

		if (occupied_left[i] > 0) {
			btot.resize(totale + occupied_left[i]);
			unsigned long long val = 1;
			for (int k = 0; k < occupied_left[i]; ++k)
			{
				btot.set(totale + occupied_left[i] - k - 1, values_left[i] & val);
				val <<= 1;
			}
			totale += occupied_left[i];
		}
		if (occupied_right[i] > 0) {
			btot.resize(totale + occupied_right[i]);
			unsigned long long val = 1;
			for (int k = 0; k < occupied_right[i]; ++k)
			{
				btot.set(totale + occupied_right[i] - k - 1, values_right[i] & val);
				val <<= 1;
			}
			totale += occupied_right[i];
		}
	}

	return btot;

}


Pushing::Pushing() {
    v.clear();
    byte = 0;
    numbit = 0;
}
void Pushing::push ( int bit ) {
    byte = ( byte << 1 ) | bit;
    numbit++;

    if ( numbit >= 8 ) {
        v.push_back ( byte );
        byte = 0;
        numbit = 0;
    }
}
#endif


int KGenerator::Log2 ( unsigned long long _v ) {
    auto v = ( unsigned long ) _v;
    static const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    static const unsigned int S[] = {1, 2, 4, 8, 16};
    int i;

    unsigned int r = 0; // result of log2(v) will go here

    if ( v & b[4] ) {
        v >>= S[4];
        r |= S[4];
    }

    if ( v & b[3] ) {
        v >>= S[3];
        r |= S[3];
    }

    if ( v & b[2] ) {
        v >>= S[2];
        r |= S[2];
    }

    if ( v & b[1] ) {
        v >>= S[1];
        r |= S[1];
    }

    if ( v & b[0] ) {
        v >>= S[0];
        r |= S[0];
    }

    return r;
}

//void KGenerator::PreparePa ( SetUnion& nva, Pushing& pa ) {
//    for ( int i = 0; i < nva.size; ++i ) {
//        if ( nva.Occupied ( i ) <= 0 )
//            continue;
//
//// 			boost::dynamic_bitset<> bsl (64, nva.values_left[i] );
//// 			boost::dynamic_bitset<> bsr (64, nva.values_right[i] );
//        bitset<64> bsl ( nva.values_left[i] );
//        bitset<64> bsr ( nva.values_right[i] );
//
//        if ( nva.occupied_left[i] > 0 ) {
//            auto s = bsl.to_string().substr ( 64 - nva.occupied_left[i] );
//
//            for ( auto c : s )
//                pa.push ( ( c == '0' ) ? 0 : 1 );
//        }
//
//        if ( nva.occupied_right[i] > 0 ) {
//            auto s = bsr.to_string().substr ( 64 - nva.occupied_right[i] );
//
//            for ( auto c : s )
//                pa.push ( ( c == '0' ) ? 0 : 1 );
//        }
//    }
//}

string KGenerator::ToString ( const vector<bool>& v ) {
    string s;

    for ( auto it : v )
        s = s + ( it ? "1" : "0" );

    return s;
}

void KGenerator::RotateBit ( bool& a, bool& b, bool& a1, bool& b1, bool direction) {
    bool tmp = a;

    if ( direction ) {
        //Destra
        a = a1;
        a1 = b1;
        b1 = b;
        b = tmp;
    } else {
        //Sinistra
        a = b;
        b = b1;
        b1 = a1;
        a1 = tmp;
    }
}

tuple<int, int, int, int> KGenerator::EvalABXY (
    BooleanBuffer& nva,
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
    const unsigned int iy1 ) {
    static unsigned short mapper[256] = {
        0x0F00, 0x0E02, 0x0120, 0x0022, 0x0B08, 0x0A18, 0x0541, 0x0451, 0x0480, 0x0590, 0x0A05, 0x0B15, 0x0088, 0x0146, 0x0EA8, 0x0F66, 0x0D01, 0x0F03, 0x0321, 0x0123, 0x0928, 0x0B38, 0x0761, 0x0571, 0x06A0, 0x04B0, 0x0825, 0x0A35, 0x0245, 0x008B, 0x0C65, 0x0EAB, 0x0E10, 0x0C12, 0x0030, 0x0232, 0x0A42, 0x0852, 0x040B, 0x061B, 0x0506, 0x0716, 0x0B83, 0x0993, 0x0198, 0x0356, 0x0FB8, 0x0D76, 0x0C11, 0x0D13, 0x0231, 0x0333, 0x0862, 0x0972, 0x062B, 0x073B, 0x0726, 0x0636, 0x09A3, 0x08B3, 0x0355, 0x029B, 0x0D75, 0x0CBB, 0x0704, 0x0A82, 0x0924, 0x04A2, 0x030C, 0x0E1C, 0x0DC1, 0x00D1, 0x0C84, 0x0194, 0x0285, 0x0F95, 0x088C, 0x05C6, 0x06AC, 0x0BE6, 0x0681, 0x0B07, 0x08A1, 0x0527, 0x022C, 0x0F3C, 0x0CE1, 0x01F1, 0x0DA4, 0x00B4, 0x03A5, 0x0EB5, 0x09C5, 0x048F, 0x07E5, 0x0AAF, 0x0514, 0x0892, 0x0B34, 0x06B2, 0x01C2, 0x0CD2, 0x0F0F, 0x021F, 0x0E86, 0x0396, 0x0087, 0x0D97, 0x0A9C, 0x07D6, 0x04BC, 0x09F6, 0x0491, 0x0917, 0x0AB1, 0x0737, 0x00E2, 0x0DF2, 0x0E2F, 0x033F, 0x0FA6, 0x02B6, 0x01A7, 0x0CB7, 0x0BD5, 0x069F, 0x05F5, 0x08BF, 0x0B40, 0x060A, 0x0560, 0x082A, 0x0F48, 0x0258, 0x0149, 0x0C59, 0x00C0, 0x0DD0, 0x0E0D, 0x031D, 0x04C8, 0x094E, 0x0AE8, 0x076E, 0x0909, 0x0443, 0x0729, 0x0A63, 0x0D68, 0x0078, 0x0369, 0x0E79, 0x02E0, 0x0FF0, 0x0C2D, 0x013D, 0x064D, 0x0BCB, 0x086D, 0x05EB, 0x0A50, 0x071A, 0x0470, 0x093A, 0x0E4A, 0x035A, 0x004B, 0x0D5B, 0x010E, 0x0C1E, 0x0FC3, 0x02D3, 0x05D8, 0x085E, 0x0BF8, 0x067E, 0x0819, 0x0553, 0x0639, 0x0B73, 0x0C6A, 0x017A, 0x026B, 0x0F7B, 0x032E, 0x0E3E, 0x0DE3, 0x00F3, 0x075D, 0x0ADB, 0x097D, 0x04FB, 0x0344, 0x028A, 0x0D64, 0x0CAA, 0x074C, 0x065C, 0x09C9, 0x08D9, 0x08C4, 0x09D4, 0x068D, 0x079D, 0x0CCC, 0x0DCE, 0x02EC, 0x03EE, 0x0289, 0x0047, 0x0CA9, 0x0E67, 0x066C, 0x047C, 0x08E9, 0x0AF9, 0x09E4, 0x0BF4, 0x07AD, 0x05BD, 0x0DCD, 0x0FCF, 0x03ED, 0x01EF, 0x0154, 0x039A, 0x0F74, 0x0DBA, 0x05CA, 0x07DA, 0x0B4F, 0x095F, 0x0A8E, 0x089E, 0x04C7, 0x06D7, 0x0EDC, 0x0CDE, 0x00FC, 0x02FE, 0x0099, 0x0157, 0x0EB9, 0x0F77, 0x04EA, 0x05FA, 0x0A6F, 0x0B7F, 0x0BAE, 0x0ABE, 0x05E7, 0x04F7, 0x0FDD, 0x0EDF, 0x01FD, 0x00FF
    };

    bool na, nb, nx, ny;

    bool a = nva.Get( ia );
    bool b = nvb.Get( ib );
    bool x = nvx.Get( ix );
    bool y = nvy.Get( iy );
    bool a1 = nva.Get( ia1 );
    bool b1 = nvb.Get( ib1 );
    bool x1 = nvx.Get( ix1 );
    bool y1 = nvy.Get( iy1 );

    unsigned char idx = 0;

    if ( a ) idx |= 0x01;
    else idx &= ~0x01;

    if ( b ) idx |= 0x02;
    else idx &= ~0x02;

    if ( x ) idx |= 0x04;
    else idx &= ~0x04;

    if ( y ) idx |= 0x08;
    else idx &= ~0x08;

    if ( a1 ) idx |= 0x10;
    else idx &= ~0x10;

    if ( b1 ) idx |= 0x20;
    else idx &= ~0x20;

    if ( x1 ) idx |= 0x40;
    else idx &= ~0x40;

    if ( y1 ) idx |= 0x80;
    else idx &= ~0x80;

    unsigned short hres = mapper[idx];

    a = ( hres & 1 ) != 0;
    b = ( hres & 2 ) != 0;
    x = ( hres & 4 ) != 0;
    y = ( hres & 8 ) != 0;
    a1 = ( hres & 16 ) != 0;
    b1 = ( hres & 32 ) != 0;
    x1 = ( hres & 64 ) != 0;
    y1 = ( hres & 128 ) != 0;

    na = ( hres & 0x0100 ) != 0;
    nb = ( hres & 0x0200 ) != 0;
    nx = ( hres & 0x0400 ) != 0;
    ny = ( hres & 0x0800 ) != 0;

    nva.Update ( ia, a );
    nvb.Update( ib, b );
    nvx.Update( ix, x );
    nvy.Update( iy, y );
    nva.Update( ia, a );
    nvb.Update( ib, b );
    nvx.Update( ix, x );
    nvy.Update( iy, y );
    return make_tuple ( na, nb, nx, ny ); //in questo modo i nuovi valgono 0: 2 e 1: 3
}

//Questa variabile deve essere reinizializzata per ogni loop in modo da iniziare una nuova valutazione degli indirizzi
//anche se la procedura dovrebbe accorgersi di questo per i valori di ix e iy
//ma potrebbe esserci un caso in cui questo non avviene (con n/2 calcolati questo non avviene mai ma con n calcolati, sì).
pair<int, int> KGenerator::GetNewAddressesAB(
	BooleanBuffer& vx,
	BooleanBuffer& vy,
	int ix, int iy,
	unsigned long long currentSize, //dimensione della chiave, sia vx sia vy
	int addressSize                 //numero di bit per indirizzare la chiave
) {
    unsigned int address_x = vx.getNBitsFromIndex ( ix, addressSize );
    unsigned int address_y = vy.getNBitsFromIndex ( iy, addressSize );

    return make_pair ( address_x % ( 1 << addressSize ), address_y % ( 1 << addressSize ) );
}

void KGenerator::MakeSpace(
	const boost::dynamic_bitset<>& va,
	const boost::dynamic_bitset<>& vx,
	BooleanBuffer& nva) {
	//Qui lo spazio lo metto tutto in testa oppure casualmente.Se lo metto casualmente allora dei bit potrebbero tornare in altre generazioni
	int ksize = va.size();
#if 1
	for (int i = 0; i < ksize; ++i)
	{
		nva.Insert(i, va[i]); // ^vx[i]);
	}
#else

	//ci metto un random shuffle, seeded by vx
	//calcolo xor di vx

	unsigned long long totxorx = 0;
	for (int i = 0; i < ksize; i += 64)
	{
		unsigned long long xorx = 0;
		for (int j = i; j < (i + 64); ++j)
		{
			xorx |= vx[i]; xorx <<= 1;
		}
		totxorx ^= xorx;
	}
	std::mt19937_64 rd(totxorx);

	//faccio un vettore doppio delle dimensioni della chiave
	vector<int> vidx(2 * ksize);
	for (int i = 0; i < (2*ksize); ++i)
		vidx[i] = i;

	std::shuffle(vidx.begin(), vidx.end(), rd);

	//e a questo punto ci metto i valori precedenti
	//metà vettore sta a -1 e quindi non lo uso
	//in uscita nva contiene ksize/2 spazi vuoti
	int pos = 0;
	for (int i = 0; i < ksize; ++i)
	{
		nva.Insert(vidx[i], va[pos++]);
	}

	//{
	//	cout << "MakeSpace. Size: " << nva.baf->su->ranges.size() << ". ";
	//	int count = 0;
	//	for (int i = 0; i < nva.baf->su->ranges.size(); ++i) {
	//		if (nva.baf->su->ranges[i])
	//			count++;
	//	}
	//	cout << "Occupati: " << count << ". Liberi: " << nva.baf->su->ranges.size() - count << endl;
	//}
#endif


    return ;
}


//MakeKey keylen power of 2, oneof8 from 0 to 7, startin65536 from 0 to 65536-4*keylen, whichbit is a mask of 64bit ex: 1 << 24
KGenerator::TBrgKey KGenerator::MakeKey ( int keylen, unsigned long long seed ) {
    KGenerator::TBrgKey key; //  = new KGenerator::TBrgKey();
    key.Va.resize ( keylen, 0 );
    key.Va1.resize ( keylen, 0 );
    key.Vb.resize ( keylen, 0 );
    key.Vb1.resize ( keylen, 0 );
	
    mtfa_fastrng1024 r ( seed );

    for ( int i = 0; i < keylen; i++ ) {

        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Va[i]=true;
        } else {
            key.Va[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Va1[i]=true;
        } else {
            key.Va1[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Vb[i]=true;
        } else {
            key.Vb[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Vb1[i]=true;
        } else {
            key.Vb1[i]=false;
        }
    }

    return key;
}

//MakeKey keylen power of 2, oneof8 from 0 to 7, startin65536 from 0 to 65536-4*keylen, whichbit is a mask of 64bit ex: 1 << 24
KGenerator::TBrgKey KGenerator::MakeKey ( int keylen, const vector<char> & v, int * pos, mtfa_fastrng1024 & r ) {
    KGenerator::TBrgKey key; //  = new KGenerator::TBrgKey();
    key.Va.resize ( keylen, 0 );
    key.Va1.resize ( keylen, 0 );
    key.Vb.resize ( keylen, 0 );
    key.Vb1.resize ( keylen, 0 );
	
    for ( int i = 0; i < keylen; i++ ) {
        unsigned long long seed = *(unsigned long long*)&(v.data()[(*pos)++ % (v.size()-8)]);
        //cout << "POS: " << *pos << ". SEED: " << seed << endl;
        //mtfa_fastrng1024 r (seed);

        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Va[i]=true;
        } else {
            key.Va[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Va1[i]=true;
        } else {
            key.Va1[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Vb[i]=true;
        } else {
            key.Vb[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Vb1[i]=true;
        } else {
            key.Vb1[i]=false;
        }
    }

    return key;
}



pair<KGenerator::TBrgKey, KGenerator::TBrgKey> KGenerator::mfa_generator (KGenerator::TBrgKey originalKey, KGenerator::TBrgKey hiddenKey)
{
    //Con la set union
	unsigned long long finalSize = originalKey.Va.size()*2;
	BooleanBuffer nva (finalSize);
	BooleanBuffer nvb (finalSize);
	BooleanBuffer nvx (finalSize);
	BooleanBuffer nvy (finalSize);

	BooleanBuffer nva_2 (finalSize);
	BooleanBuffer nvb_2 (finalSize);
	BooleanBuffer nvx_2 (finalSize);
	BooleanBuffer nvy_2 (finalSize);

	//in questo modo uso anche hidden per preparare la nuova chiave
    MakeSpace ( originalKey.Va, hiddenKey.Va, nva );
    MakeSpace ( originalKey.Vb, hiddenKey.Vb, nvb );
	//MakeSpace(hiddenKey.Va, originalKey.Vb, nvx);
	//MakeSpace(hiddenKey.Vb, originalKey.Va, nvy);
	MakeSpace(hiddenKey.Va, originalKey.Va, nvx);
	MakeSpace(hiddenKey.Vb, originalKey.Vb, nvy);

    MakeSpace ( originalKey.Va1, hiddenKey.Va1, nva_2 );
	MakeSpace ( originalKey.Vb1, hiddenKey.Vb1, nvb_2 );
	//MakeSpace(hiddenKey.Va1, originalKey.Vb1, nvx_2);
	//MakeSpace(hiddenKey.Vb1, originalKey.Va1, nvy_2);
	MakeSpace(hiddenKey.Va1, originalKey.Va1, nvx_2);
	MakeSpace(hiddenKey.Vb1, originalKey.Vb1, nvy_2);

    //Riempio gli spazi vuoti
    int pos_a = 0;
    int pos_b = 0;
    int pos_x = 0;
    int pos_y = 0;

    int pos_a_2 = 0;
    int pos_b_2 = 0;
    int pos_x_2 = 0;
    int pos_y_2 = 0;


	//auto contaitem = [](BooleanBuffer &bb)
	//{
	//	int count = 0;
	//	for (int i = 0; i < bb.baf->su->ranges.size(); ++i)
	//	{
	//		if (bb.baf->su->ranges[i])
	//			count++;
	//	}
	//	return count;
	//};
	//auto docount = [&]()
	//{
	//	printf("NVA: %3d. NVB: %3d. NVX: %3d. NVY: %3d. NA2: %3d. NB2: %3d. NX2: %3d. NY2: %3d.\n",
	//		contaitem(nva), contaitem(nvb), contaitem(nvx), contaitem(nvy),
	//		contaitem(nva_2), contaitem(nvb_2), contaitem(nvx_2), contaitem(nvy_2));
	//};

	//ogni volta riaggiungo una lunghezza pari alla chiave
    for ( int i = 0; i < originalKey.Va.size(); ++i ) {
        //Calcolo indirizzi di a b e di x y
        auto addresses_ab   = GetNewAddressesAB ( nvx_2, nvy_2, pos_x_2, pos_y_2, finalSize, Log2 (finalSize) );
        auto addresses_xy   = GetNewAddressesAB ( nva_2, nvb_2, pos_a_2, pos_b_2, finalSize, Log2 (finalSize) );
        auto addresses_ab_2 = GetNewAddressesAB ( nvx, nvy, pos_x, pos_y, finalSize, Log2 (finalSize) );
        auto addresses_xy_2 = GetNewAddressesAB ( nva, nvb, pos_a, pos_b, finalSize, Log2 (finalSize) );

        //Calcolo i nuovi a, b, x e y e ruoto i precedenti
        //Per farlo devo cercare i primi occupati a partire dalla posizione corrente, che appartengano alla chiave attuale
        unsigned int fa  = nva.getAddressOfNextBit ( pos_a, true);
        unsigned int fb  = nvb.getAddressOfNextBit ( pos_b, true);
        unsigned int fx  = nvx.getAddressOfNextBit ( pos_x, true);
        unsigned int fy  = nvy.getAddressOfNextBit ( pos_y, true);
        unsigned int fa1 = nva.getAddressOfNextBit ( fa + 1, true);
        unsigned int fb1 = nvb.getAddressOfNextBit ( fb + 1, true);
        unsigned int fx1 = nvx.getAddressOfNextBit ( fx + 1, true);
        unsigned int fy1 = nvy.getAddressOfNextBit ( fy + 1, true);

        unsigned int fa_2  = nva_2.getAddressOfNextBit ( pos_a_2, true);
        unsigned int fb_2  = nvb_2.getAddressOfNextBit ( pos_b_2, true);
        unsigned int fx_2  = nvx_2.getAddressOfNextBit ( pos_x_2, true);
        unsigned int fy_2  = nvy_2.getAddressOfNextBit ( pos_y_2, true);
        unsigned int fa1_2 = nva_2.getAddressOfNextBit ( fa_2 + 1, true );
        unsigned int fb1_2 = nvb_2.getAddressOfNextBit ( fb_2 + 1, true );
        unsigned int fx1_2 = nvx_2.getAddressOfNextBit ( fx_2 + 1, true );
        unsigned int fy1_2 = nvy_2.getAddressOfNextBit ( fy_2 + 1, true );


        //Calcolo i nuovi valori della quaterna
        auto nuovi_abxy   = EvalABXY ( nva, nvb, nvx, nvy, fa, fb, fx, fy, fa1, fb1, fx1, fy1 );
        auto nuovi_abxy_2 = EvalABXY ( nva_2, nvb_2, nvx_2, nvy_2, fa_2, fb_2, fx_2, fy_2, fa1_2, fb1_2, fx1_2, fy1_2 );

        //cout << get<0>(nuovi_abxy) << " " << get<1>(nuovi_abxy) << " " << get<2>(nuovi_abxy) << " " << get<3>(nuovi_abxy) << endl;

		nva.Insert( addresses_ab.first, get<0> ( nuovi_abxy ));
        nvb.Insert( addresses_ab.second, get<1> ( nuovi_abxy ));
        nvx.Insert( addresses_xy.first, get<2> ( nuovi_abxy ));
        nvy.Insert( addresses_xy.second, get<3> ( nuovi_abxy ));

        nva_2.Insert( addresses_ab_2.first, get<0> ( nuovi_abxy_2 ));
        nvb_2.Insert( addresses_ab_2.second, get<1> ( nuovi_abxy_2 ));
        nvx_2.Insert( addresses_xy_2.first, get<2> ( nuovi_abxy_2 ));
        nvy_2.Insert( addresses_xy_2.second, get<3> ( nuovi_abxy_2 ));

        pos_a = fa1;
        pos_b = fb1;
        pos_x = fx1;
        pos_y = fy1;

        pos_a_2 = fa1_2;
        pos_b_2 = fb1_2;
        pos_x_2 = fx1_2;
        pos_y_2 = fy1_2;
    }

    KGenerator::TBrgKey korig; // = new KGenerator::TBrgKey();
	KGenerator::TBrgKey khidd; // = new KGenerator::TBrgKey();
	korig.Va = nva.GetBitSet();
	korig.Va1 = nva_2.GetBitSet();
	korig.Vb = nvb.GetBitSet();
	korig.Vb1 = nvb_2.GetBitSet();

	khidd.Va = nvx.GetBitSet();
	khidd.Va1 = nvx_2.GetBitSet();
	khidd.Vb = nvy.GetBitSet();
	khidd.Vb1 = nvy_2.GetBitSet();

    return make_pair ( korig, khidd );
}

// BRG::BRG (string _id, int _keylen, int _keygen, vector<unsigned char> bytes)
// {
// 	id = _id;
// 	keylen = _keylen;
// 	keygen = _keygen;
// 	
// 	if 
// 	
// 	//Genero la kOriginal e poi la kHidden
// 	for (int i=0; i<keylen; ++i)
// 	{
// 		
// 	}
// 	for ( int i = 0; i < keylen; i++ ) {
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Va[i]=true;
// 		} else {
// 			key.Va[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Va1[i]=true;
// 		} else {
// 			key.Va1[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Vb[i]=true;
// 		} else {
// 			key.Vb[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Vb1[i]=true;
// 		} else {
// 			key.Vb1[i]=false;
// 		}
// 	}
// 	
// 	kOriginal = KGenerator::MakeKey(keylen, rng.next());
// 	kHidden   = KGenerator::MakeKey(keylen, rng.next());
// 	
// 	newOriginal.Va.clear();
// 	newOriginal.Va1.clear();
// 	newOriginal.Vb.clear();
// 	newOriginal.Vb1.clear();
// 	newHidden.Va.clear();
// 	newHidden.Va1.clear();
// 	newHidden.Vb.clear();
// 	newHidden.Vb1.clear();
// 	
// 	//Per generare automaticamente un array di bytes per ogni Next
// 	bufOriginal.clear();
// 	steps = 0;
// }


BRG::BRG (string _id, unsigned long long _seed)
{
	auto rng = mtfa_fastrng1024(_seed);
	
	//cout << rng.next() << ", " << rng.next() << endl;
	id = _id;
	keylen = 1024;
	kOriginal = KGenerator::MakeKey(keylen, rng.next());
	kHidden   = KGenerator::MakeKey(keylen, rng.next());
		
	newOriginal.Va.clear();
	newOriginal.Va1.clear();
	newOriginal.Vb.clear();
	newOriginal.Vb1.clear();
	newHidden.Va.clear();
	newHidden.Va1.clear();
	newHidden.Vb.clear();
	newHidden.Vb1.clear();
	
	//Per generare automaticamente un array di bytes per ogni Next
	bufOriginal.clear();
	steps = 0;
}

BRG::BRG (string _id, const vector<char> & v)
{
	id = _id;
	keylen = v.size()*8;  //in bit
    int pos=0;

	//faccio un seed
	unsigned long long seed = 0;
	for (int i=0; i<(v.size()/8); i+=8)
		seed ^= *(unsigned long long*)&(v.data()[i]);  //8 byte a 8 byte

    auto rng = mtfa_fastrng1024(seed);

	kOriginal = KGenerator::MakeKey(keylen, v, &pos, rng);  //la chiave è esattamente quanto richiesto
	kHidden   = KGenerator::MakeKey(keylen, v, &pos, rng);

	newOriginal.Va.clear();
	newOriginal.Va1.clear();
	newOriginal.Vb.clear();
	newOriginal.Vb1.clear();
	newHidden.Va.clear();
	newHidden.Va1.clear();
	newHidden.Vb.clear();
	newHidden.Vb1.clear();
	
	//Per generare automaticamente un array di bytes per ogni Next
	bufOriginal.clear();
	steps = 0;
}

string BRG::GetId() {
	return id;
}

string BRG::GetBufAsBase64String() {
	return Base64::encode(bufOriginal.size(), bufOriginal.data());
}

vector<unsigned char> BRG::GetBufAsVByte() {
	return bufOriginal;
}

//Il vantaggio è che le dimensioni sono sempre multiplo di 64
vector<unsigned long long> ConvertBitSetToVUint64 ( const boost::dynamic_bitset<> & bs ) {
    vector<unsigned long long> vull ( bs.size() / 64, 0 );
    for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
		//cout << "POS: " << i << endl;
        auto pos = i >> 6;
        auto bit = i & 0x0000003F;
        vull[pos] |= 1ULL << bit;
    }
    return vull;
}

vector<unsigned char> ConvertBitSetToVUint8 ( const boost::dynamic_bitset<> & bs ) {
    vector<unsigned char> vuc ( bs.size() /8, 0 );

    for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
        auto pos = i >> 3;
        auto bit = i & 0x00000007;
        vuc[pos] |= 1 << bit;
    }

    return vuc;
}

boost::dynamic_bitset<> GiveSecondHalfBitSet (const boost::dynamic_bitset<> &setin) {
	int newlen = setin.size() >> 1;
	auto setout = setin >> newlen;
	setout.resize(newlen);
 	return setout;
}

vector<unsigned char> GetFirstHalfAsVbyte(const boost::dynamic_bitset<> &setin) {
	int newlen = setin.size() >> 1;
	auto setout = setin;
	setout.resize(newlen);
	return ConvertBitSetToVUint8(setout);
}

void BRG::Next()
{
	steps++;
	if (newHidden.Va.size() == 0) {
		auto knew = KGenerator::mfa_generator(kOriginal, kHidden);
		newOriginal = knew.first;
		newHidden = knew.second;
	}
	//Sempre i vecchi nei nuovi e poi genero i prossimi
	kOriginal.Va  = GiveSecondHalfBitSet(newOriginal.Va);
	kOriginal.Va1 = GiveSecondHalfBitSet(newOriginal.Va1);
	kOriginal.Vb  = GiveSecondHalfBitSet(newOriginal.Vb);
	kOriginal.Vb1 = GiveSecondHalfBitSet(newOriginal.Vb1);

	kHidden.Va  = GiveSecondHalfBitSet(newHidden.Va);
	kHidden.Va1 = GiveSecondHalfBitSet(newHidden.Va1);
	kHidden.Vb  = GiveSecondHalfBitSet(newHidden.Vb);
	kHidden.Vb1 = GiveSecondHalfBitSet(newHidden.Vb1);

	auto knew = KGenerator::mfa_generator(kOriginal, kHidden);
	newOriginal = knew.first;
	newHidden = knew.second;

	//Converto gli uint64 in byte e prendo la prima metà
	bufOriginal.clear();

	auto b = GetFirstHalfAsVbyte(newOriginal.Va);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Vb);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Va1);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Vb1);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());
}










#else    //vecchia versione

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <bitset>

#include "base64.h"
#include "mtfa_utils.h"
#include "mtfabrg.h"

using namespace std;

extern unsigned long long mtfa_mat_cypher_base_ext[8][65536];

Pushing::Pushing() {
    v.clear();
    byte = 0;
    numbit = 0;
}
void Pushing::push ( int bit ) {
    byte = ( byte << 1 ) | bit;
    numbit++;

    if ( numbit >= 8 ) {
        v.push_back ( byte );
        byte = 0;
        numbit = 0;
    }
}


int KGenerator::Log2 ( unsigned long long _v ) {
    auto v = ( unsigned long ) _v;
    static const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    static const unsigned int S[] = {1, 2, 4, 8, 16};
    int i;

    unsigned int r = 0; // result of log2(v) will go here

    if ( v & b[4] ) {
        v >>= S[4];
        r |= S[4];
    }

    if ( v & b[3] ) {
        v >>= S[3];
        r |= S[3];
    }

    if ( v & b[2] ) {
        v >>= S[2];
        r |= S[2];
    }

    if ( v & b[1] ) {
        v >>= S[1];
        r |= S[1];
    }

    if ( v & b[0] ) {
        v >>= S[0];
        r |= S[0];
    }

    return r;
}

void KGenerator::PreparePa ( BSetUnion& nva, Pushing& pa ) {
    for ( int i = 0; i < nva.size; ++i ) {
        if ( nva.Occupied ( i ) <= 0 )
            continue;

// 			boost::dynamic_bitset<> bsl (64, nva.values_left[i] );
// 			boost::dynamic_bitset<> bsr (64, nva.values_right[i] );
        bitset<64> bsl ( nva.values_left[i] );
        bitset<64> bsr ( nva.values_right[i] );

        if ( nva.occupied_left[i] > 0 ) {
            auto s = bsl.to_string().substr ( 64 - nva.occupied_left[i] );

            for ( auto c : s )
                pa.push ( ( c == '0' ) ? 0 : 1 );
        }

        if ( nva.occupied_right[i] > 0 ) {
            auto s = bsr.to_string().substr ( 64 - nva.occupied_right[i] );

            for ( auto c : s )
                pa.push ( ( c == '0' ) ? 0 : 1 );
        }
    }
}


string KGenerator::ToString ( const vector<bool>& v ) {
    string s;

    for ( auto it : v )
        s = s + ( it ? "1" : "0" );

    return s;
}

void KGenerator::RotateBit ( bool& a,
                             bool& b,
                             bool& a1,
                             bool& b1,
                             bool direction
                           ) {
    bool tmp = a;

    if ( direction ) {
        //Destra
        a = a1;
        a1 = b1;
        b1 = b;
        b = tmp;
    } else {
        //Sinistra
        a = b;
        b = b1;
        b1 = a1;
        a1 = tmp;
    }
}

tuple<int, int, int, int> KGenerator::EvalABXY (
    BSetUnion& nva,
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
    const unsigned int iy1 ) {
    static unsigned short mapper[256] = {
        0x0F00, 0x0E02, 0x0120, 0x0022, 0x0B08, 0x0A18, 0x0541, 0x0451, 0x0480, 0x0590, 0x0A05, 0x0B15, 0x0088, 0x0146, 0x0EA8, 0x0F66, 0x0D01, 0x0F03, 0x0321, 0x0123, 0x0928, 0x0B38, 0x0761, 0x0571, 0x06A0, 0x04B0, 0x0825, 0x0A35, 0x0245, 0x008B, 0x0C65, 0x0EAB, 0x0E10, 0x0C12, 0x0030, 0x0232, 0x0A42, 0x0852, 0x040B, 0x061B, 0x0506, 0x0716, 0x0B83, 0x0993, 0x0198, 0x0356, 0x0FB8, 0x0D76, 0x0C11, 0x0D13, 0x0231, 0x0333, 0x0862, 0x0972, 0x062B, 0x073B, 0x0726, 0x0636, 0x09A3, 0x08B3, 0x0355, 0x029B, 0x0D75, 0x0CBB, 0x0704, 0x0A82, 0x0924, 0x04A2, 0x030C, 0x0E1C, 0x0DC1, 0x00D1, 0x0C84, 0x0194, 0x0285, 0x0F95, 0x088C, 0x05C6, 0x06AC, 0x0BE6, 0x0681, 0x0B07, 0x08A1, 0x0527, 0x022C, 0x0F3C, 0x0CE1, 0x01F1, 0x0DA4, 0x00B4, 0x03A5, 0x0EB5, 0x09C5, 0x048F, 0x07E5, 0x0AAF, 0x0514, 0x0892, 0x0B34, 0x06B2, 0x01C2, 0x0CD2, 0x0F0F, 0x021F, 0x0E86, 0x0396, 0x0087, 0x0D97, 0x0A9C, 0x07D6, 0x04BC, 0x09F6, 0x0491, 0x0917, 0x0AB1, 0x0737, 0x00E2, 0x0DF2, 0x0E2F, 0x033F, 0x0FA6, 0x02B6, 0x01A7, 0x0CB7, 0x0BD5, 0x069F, 0x05F5, 0x08BF, 0x0B40, 0x060A, 0x0560, 0x082A, 0x0F48, 0x0258, 0x0149, 0x0C59, 0x00C0, 0x0DD0, 0x0E0D, 0x031D, 0x04C8, 0x094E, 0x0AE8, 0x076E, 0x0909, 0x0443, 0x0729, 0x0A63, 0x0D68, 0x0078, 0x0369, 0x0E79, 0x02E0, 0x0FF0, 0x0C2D, 0x013D, 0x064D, 0x0BCB, 0x086D, 0x05EB, 0x0A50, 0x071A, 0x0470, 0x093A, 0x0E4A, 0x035A, 0x004B, 0x0D5B, 0x010E, 0x0C1E, 0x0FC3, 0x02D3, 0x05D8, 0x085E, 0x0BF8, 0x067E, 0x0819, 0x0553, 0x0639, 0x0B73, 0x0C6A, 0x017A, 0x026B, 0x0F7B, 0x032E, 0x0E3E, 0x0DE3, 0x00F3, 0x075D, 0x0ADB, 0x097D, 0x04FB, 0x0344, 0x028A, 0x0D64, 0x0CAA, 0x074C, 0x065C, 0x09C9, 0x08D9, 0x08C4, 0x09D4, 0x068D, 0x079D, 0x0CCC, 0x0DCE, 0x02EC, 0x03EE, 0x0289, 0x0047, 0x0CA9, 0x0E67, 0x066C, 0x047C, 0x08E9, 0x0AF9, 0x09E4, 0x0BF4, 0x07AD, 0x05BD, 0x0DCD, 0x0FCF, 0x03ED, 0x01EF, 0x0154, 0x039A, 0x0F74, 0x0DBA, 0x05CA, 0x07DA, 0x0B4F, 0x095F, 0x0A8E, 0x089E, 0x04C7, 0x06D7, 0x0EDC, 0x0CDE, 0x00FC, 0x02FE, 0x0099, 0x0157, 0x0EB9, 0x0F77, 0x04EA, 0x05FA, 0x0A6F, 0x0B7F, 0x0BAE, 0x0ABE, 0x05E7, 0x04F7, 0x0FDD, 0x0EDF, 0x01FD, 0x00FF
    };

    bool na, nb, nx, ny;

    bool a = nva.getBitValue ( ia );
    bool b = nvb.getBitValue ( ib );
    bool x = nvx.getBitValue ( ix );
    bool y = nvy.getBitValue ( iy );
    bool a1 = nva.getBitValue ( ia1 );
    bool b1 = nvb.getBitValue ( ib1 );
    bool x1 = nvx.getBitValue ( ix1 );
    bool y1 = nvy.getBitValue ( iy1 );

    unsigned char idx = 0;

    if ( a ) idx |= 0x01;
    else idx &= ~0x01;

    if ( b ) idx |= 0x02;
    else idx &= ~0x02;

    if ( x ) idx |= 0x04;
    else idx &= ~0x04;

    if ( y ) idx |= 0x08;
    else idx &= ~0x08;

    if ( a1 ) idx |= 0x10;
    else idx &= ~0x10;

    if ( b1 ) idx |= 0x20;
    else idx &= ~0x20;

    if ( x1 ) idx |= 0x40;
    else idx &= ~0x40;

    if ( y1 ) idx |= 0x80;
    else idx &= ~0x80;

    unsigned short hres = mapper[idx];

    a = ( hres & 1 ) != 0;
    b = ( hres & 2 ) != 0;
    x = ( hres & 4 ) != 0;
    y = ( hres & 8 ) != 0;
    a1 = ( hres & 16 ) != 0;
    b1 = ( hres & 32 ) != 0;
    x1 = ( hres & 64 ) != 0;
    y1 = ( hres & 128 ) != 0;

    na = ( hres & 0x0100 ) != 0;
    nb = ( hres & 0x0200 ) != 0;
    nx = ( hres & 0x0400 ) != 0;
    ny = ( hres & 0x0800 ) != 0;

    nva.setBitValue ( ia, a );
    nvb.setBitValue ( ib, b );
    nvx.setBitValue ( ix, x );
    nvy.setBitValue ( iy, y );
    nva.setBitValue ( ia, a );
    nvb.setBitValue ( ib, b );
    nvx.setBitValue ( ix, x );
    nvy.setBitValue ( iy, y );
    return make_tuple ( na, nb, nx, ny ); //in questo modo i nuovi valgono 0: 2 e 1: 3
}

//Questa variabile deve essere reinizializzata per ogni loop in modo da iniziare una nuova valutazione degli indirizzi
//anche se la procedura dovrebbe accorgersi di questo per i valori di ix e iy
//ma potrebbe esserci un caso in cui questo non avviene (con n/2 calcolati questo non avviene mai ma con n calcolati, sì).
pair<int, int> KGenerator::GetNewAddressesAB ( BSetUnion& vx,
        BSetUnion& vy,
        int ix, int iy,
        unsigned long long currentSize,
        int addressSize
                                             ) {
    unsigned int address_x = vx.getNBitsFromIndex ( ix, addressSize << 1 );
    unsigned int address_y = vy.getNBitsFromIndex ( iy, addressSize << 1 );

    return make_pair ( address_x % ( 1 << addressSize ), address_y % ( 1 << addressSize ) );
}

void KGenerator::MakeSpace ( const boost::dynamic_bitset<>& va, 
							 const boost::dynamic_bitset<>& vx, 
							 unsigned long long size, 
							 BSetUnion& nva ) {
	int ksize = va.size();

	//calcolo xor di vx
    int xorx = 0;

    for ( int i = 0; i < ksize; ++i ) xorx ^= vx[i];

    int tot = 0;

    //Ne devo mettere ksize in size:= avanzo di size/ksize
    int pos = 0;

    if ( xorx ) {
        for ( int i = 0; i < size; i += ( size / ksize ) ) {
			//cout << "Insert at " << i << " value: " << va[pos];
            nva.InsertValue ( i, va[pos++], true );
			//cout << ". Total: " << nva.total << endl;
// 			for (int ii=0; ii<nva.size; ++ii)
// 			{
// 				if (nva.Occupied(ii) > 0)
// 					cout << ii << ") Ol: " << (uint)nva.occupied_left[ii] << ". Or: " << (uint)nva.occupied_right[ii] << endl;
// 			}
// 			for (auto it: ConvertBitSetToVUint64(nva.ToBitSet()))
// 				cout << it << " ";
// 			cout << endl;

			if ( ++tot >= ksize )
                return;
        }
    } else {
        //parto con vuoto
        for ( int i = 0; i < size; i += ( size / ksize ) ) {
			//cout << "Insert at " << ( i + 1 ) % size << " value: " << va[pos];
			nva.InsertValue ( ( i + 1 ) % size, va[pos++], true);
			//cout << ". Total: " << nva.total << endl;
// 			for (int ii=0; ii<nva.size; ++ii)
// 			{
// 				if (nva.Occupied(ii) > 0)
// 					cout << ii << ") Ol: " << (uint)nva.occupied_left[ii] << ". Or: " << (uint)nva.occupied_right[ii] << endl;
// 			}
			// 			for (auto it: ConvertBitSetToVUint64(nva.ToBitSet()))
// 				cout << it << " ";
// 			cout << endl;
			
			if ( ++tot >= ksize )
                return;
        }
    }
    
    return ;
}


//MakeKey keylen power of 2, oneof8 from 0 to 7, startin65536 from 0 to 65536-4*keylen, whichbit is a mask of 64bit ex: 1 << 24
KGenerator::TBrgKey KGenerator::MakeKey ( int keylen, unsigned long long seed ) {
    KGenerator::TBrgKey key; //  = new KGenerator::TBrgKey();
    key.Va.resize ( keylen, 0 );
    key.Va1.resize ( keylen, 0 );
    key.Vb.resize ( keylen, 0 );
    key.Vb1.resize ( keylen, 0 );
	
    mtfa_fastrng1024 r ( seed );

    for ( int i = 0; i < keylen; i++ ) {

        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Va[i]=true;
        } else {
            key.Va[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Va1[i]=true;
        } else {
            key.Va1[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Vb[i]=true;
        } else {
            key.Vb[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
            key.Vb1[i]=true;
        } else {
            key.Vb1[i]=false;
        }
    }

    return key;
}

//MakeKey keylen power of 2, oneof8 from 0 to 7, startin65536 from 0 to 65536-4*keylen, whichbit is a mask of 64bit ex: 1 << 24
KGenerator::TBrgKey KGenerator::MakeKey ( int keylen, vector<char> & v, int * pos, mtfa_fastrng1024 & r ) {
    KGenerator::TBrgKey key; //  = new KGenerator::TBrgKey();
    key.Va.resize ( keylen, 0 );
    key.Va1.resize ( keylen, 0 );
    key.Vb.resize ( keylen, 0 );
    key.Vb1.resize ( keylen, 0 );
	
    for ( int i = 0; i < keylen; i++ ) {
        unsigned long long seed = *(unsigned long long*)&(v.data()[(*pos)++ % (v.size()-8)]);
        //cout << "POS: " << *pos << ". SEED: " << seed << endl;
        //mtfa_fastrng1024 r (seed);

        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Va[i]=true;
        } else {
            key.Va[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Va1[i]=true;
        } else {
            key.Va1[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Vb[i]=true;
        } else {
            key.Vb[i]=false;
        }
        if ( ( mtfa_mat_cypher_base_ext[r.next() %8][(seed ^ r.next()) %65536] & ( 1ULL << ( (seed ^ r.next()) %64 ) ) ) != 0 ) {
            key.Vb1[i]=true;
        } else {
            key.Vb1[i]=false;
        }
    }

    return key;
}



pair<KGenerator::TBrgKey, KGenerator::TBrgKey> KGenerator::mfa_generator ( 
	unsigned long long finalSize, 
	KGenerator::TBrgKey originalKey, 
	KGenerator::TBrgKey hiddenKey )
{
    int logfinalsize = Log2 ( finalSize );
    if ( ( 1 << logfinalsize ) != finalSize ) {
        finalSize = ( 1 << ( logfinalsize+1 ) );
    }

    int current_size;
    finalSize /= 4;		//uso i 4 elementi della original key a, b, a1, b1
    current_size = finalSize;

    //Con la set union
    BSetUnion nva ( current_size );
    BSetUnion nvb ( current_size );
    BSetUnion nvx ( current_size );
    BSetUnion nvy ( current_size );

    BSetUnion nva_2 ( current_size );
    BSetUnion nvb_2 ( current_size );
    BSetUnion nvx_2 ( current_size );
    BSetUnion nvy_2 ( current_size );

    MakeSpace ( originalKey.Va, hiddenKey.Va, current_size, nva );
    MakeSpace ( originalKey.Vb, hiddenKey.Vb, current_size, nvb );
    MakeSpace ( hiddenKey.Va, originalKey.Va, current_size, nvx );
    MakeSpace ( hiddenKey.Vb, originalKey.Vb, current_size, nvy );

    MakeSpace ( originalKey.Va1, hiddenKey.Va1, current_size, nva_2 );
	MakeSpace ( originalKey.Vb1, hiddenKey.Vb1, current_size, nvb_2 );
	MakeSpace ( hiddenKey.Va1, originalKey.Va1, current_size, nvx_2 );
	MakeSpace ( hiddenKey.Vb1, originalKey.Vb1, current_size, nvy_2 );
	
// 	//ma c'è qualcosa dentro le setunion?
// 	cout << nva.ToBitSet() << endl;
// 	cout << nvb.ToBitSet() << endl;
// 	cout << nvx.ToBitSet() << endl;
// 	cout << nvy.ToBitSet() << endl;
// 	cout << nva_2.ToBitSet() << endl;
// 	cout << nvb_2.ToBitSet() << endl;
// 	cout << nvx_2.ToBitSet() << endl;
// 	cout << nvy_2.ToBitSet() << endl;
// 	
// 	cout << "Fine" << endl;
// 	
// 	exit(0);
	
    //Riempio gli spazi vuoti
    int pos_a = 0;
    int pos_b = 0;
    int pos_x = 0;
    int pos_y = 0;

    int pos_a_2 = 0;
    int pos_b_2 = 0;
    int pos_x_2 = 0;
    int pos_y_2 = 0;

    unsigned long long limit = finalSize - originalKey.Va.size(); //finalSize-HALF_KEY_SIZE;

	//cout << "Da fare: " << limit << " elementi\n";
	
    for ( int fatti = 0; fatti < limit; ++fatti ) { //qui posso gestire la creazione di 1/2 in più per volta
        //Calcolo indirizzi di a b e di x y
        auto addresses_ab   = GetNewAddressesAB ( nvx_2, nvy_2, pos_x_2, pos_y_2, current_size, Log2 ( current_size ) );
// 		cout << "addresses_ab: " << addresses_ab.first << ", " << addresses_ab.second << endl;
        auto addresses_xy   = GetNewAddressesAB ( nva_2, nvb_2, pos_a_2, pos_b_2, current_size, Log2 ( current_size ) );
        auto addresses_ab_2 = GetNewAddressesAB ( nvx, nvy, pos_x, pos_y, current_size, Log2 ( current_size ) );
        auto addresses_xy_2 = GetNewAddressesAB ( nva, nvb, pos_a, pos_b, current_size, Log2 ( current_size ) );

        //Calcolo i nuovi a, b, x e y e ruoto i precedenti
        //Per farlo devo cercare i primi occupati a partire dalla posizione corrente, che appartengano alla chiave attuale
        unsigned int fa  = nva.getAddressOfNextBit ( pos_a, true, true );
        unsigned int fb  = nvb.getAddressOfNextBit ( pos_b, true, true );
        unsigned int fx  = nvx.getAddressOfNextBit ( pos_x, true, true );
        unsigned int fy  = nvy.getAddressOfNextBit ( pos_y, true, true );
        unsigned int fa1 = nva.getAddressOfNextBit ( fa + 1, true, true );
        unsigned int fb1 = nvb.getAddressOfNextBit ( fb + 1, true, true );
        unsigned int fx1 = nvx.getAddressOfNextBit ( fx + 1, true, true );
        unsigned int fy1 = nvy.getAddressOfNextBit ( fy + 1, true, true );

        unsigned int fa_2  = nva_2.getAddressOfNextBit ( pos_a_2, true, true );
        unsigned int fb_2  = nvb_2.getAddressOfNextBit ( pos_b_2, true, true );
        unsigned int fx_2  = nvx_2.getAddressOfNextBit ( pos_x_2, true, true );
        unsigned int fy_2  = nvy_2.getAddressOfNextBit ( pos_y_2, true, true );
        unsigned int fa1_2 = nva_2.getAddressOfNextBit ( fa_2 + 1, true, true );
        unsigned int fb1_2 = nvb_2.getAddressOfNextBit ( fb_2 + 1, true, true );
        unsigned int fx1_2 = nvx_2.getAddressOfNextBit ( fx_2 + 1, true, true );
        unsigned int fy1_2 = nvy_2.getAddressOfNextBit ( fy_2 + 1, true, true );


        //Ora devo calcolare se, in caso di collisione, devo andare avanti o indietro. Questo lo faccio utilizzando tutti e 4 i valori
        bool avanti_a = nvy.getBitValue ( fy ) == nvy.getBitValue ( fy1 );
        bool avanti_b = nvx.getBitValue ( fx ) == nvx.getBitValue ( fx1 );
        bool avanti_x = nvb.getBitValue ( fb ) == nvb.getBitValue ( fb1 );
        bool avanti_y = nva.getBitValue ( fa ) == nva.getBitValue ( fa1 );

        bool avanti_a_2 = nvy_2.getBitValue ( fy_2 ) == nvy_2.getBitValue ( fy1_2 );
        bool avanti_b_2 = nvx_2.getBitValue ( fx_2 ) == nvx_2.getBitValue ( fx1_2 );
        bool avanti_x_2 = nvb_2.getBitValue ( fb_2 ) == nvb_2.getBitValue ( fb1_2 );
        bool avanti_y_2 = nva_2.getBitValue ( fa_2 ) == nva_2.getBitValue ( fa1_2 );

        //Calcolo i nuovi valori della quaterna
        auto nuovi_abxy   = EvalABXY ( nva, nvb, nvx, nvy, fa, fb, fx, fy, fa1, fb1, fx1, fy1 );
        auto nuovi_abxy_2 = EvalABXY ( nva_2, nvb_2, nvx_2, nvy_2, fa_2, fb_2, fx_2, fy_2, fa1_2, fb1_2, fx1_2, fy1_2 );

        //cout << get<0>(nuovi_abxy) << " " << get<1>(nuovi_abxy) << " " << get<2>(nuovi_abxy) << " " << get<3>(nuovi_abxy) << endl;

        //in modo deterministico, avanti e indietro cambiano semantica ogni operazione: una volta true è avanti, la volta successiva true è indietro...
        nva.InsertValue ( addresses_ab.first, get<0> ( nuovi_abxy ), avanti_a);
        nvb.InsertValue ( addresses_ab.second, get<1> ( nuovi_abxy ), avanti_b);
        nvx.InsertValue ( addresses_xy.first, get<2> ( nuovi_abxy ), avanti_x);
        nvy.InsertValue ( addresses_xy.second, get<3> ( nuovi_abxy ), avanti_y);

        nva_2.InsertValue ( addresses_ab_2.first, get<0> ( nuovi_abxy_2 ), avanti_a_2);
        nvb_2.InsertValue ( addresses_ab_2.second, get<1> ( nuovi_abxy_2 ), avanti_b_2);
        nvx_2.InsertValue ( addresses_xy_2.first, get<2> ( nuovi_abxy_2 ), avanti_x_2);
        nvy_2.InsertValue ( addresses_xy_2.second, get<3> ( nuovi_abxy_2 ), avanti_y_2);

        pos_a = fa1;
        pos_b = fb1;
        pos_x = fx1;
        pos_y = fy1;

        pos_a_2 = fa1_2;
        pos_b_2 = fb1_2;
        pos_x_2 = fx1_2;
        pos_y_2 = fy1_2;
    }

    KGenerator::TBrgKey korig; // = new KGenerator::TBrgKey();
	KGenerator::TBrgKey khidd; // = new KGenerator::TBrgKey();
	korig.Va = nva.ToBitSet();
	korig.Va1 = nva_2.ToBitSet();
	korig.Vb = nvb.ToBitSet();
	korig.Vb1 = nvb_2.ToBitSet();

	khidd.Va = nvx.ToBitSet();
	khidd.Va1 = nvx_2.ToBitSet();
	khidd.Vb = nvy.ToBitSet();
	khidd.Vb1 = nvy_2.ToBitSet();

    return make_pair ( korig, khidd );
}

// BRG::BRG (string _id, int _keylen, int _keygen, vector<unsigned char> bytes)
// {
// 	id = _id;
// 	keylen = _keylen;
// 	keygen = _keygen;
// 	
// 	if 
// 	
// 	//Genero la kOriginal e poi la kHidden
// 	for (int i=0; i<keylen; ++i)
// 	{
// 		
// 	}
// 	for ( int i = 0; i < keylen; i++ ) {
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Va[i]=true;
// 		} else {
// 			key.Va[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Va1[i]=true;
// 		} else {
// 			key.Va1[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Vb[i]=true;
// 		} else {
// 			key.Vb[i]=false;
// 		}
// 		if ( ( mtfa_mat_cypher_base_ext[r.next() %8][r.next() %65536] & ( 1ULL << ( r.next() %64 ) ) ) != 0 ) {
// 			key.Vb1[i]=true;
// 		} else {
// 			key.Vb1[i]=false;
// 		}
// 	}
// 	
// 	kOriginal = KGenerator::MakeKey(keylen, rng.next());
// 	kHidden   = KGenerator::MakeKey(keylen, rng.next());
// 	
// 	newOriginal.Va.clear();
// 	newOriginal.Va1.clear();
// 	newOriginal.Vb.clear();
// 	newOriginal.Vb1.clear();
// 	newHidden.Va.clear();
// 	newHidden.Va1.clear();
// 	newHidden.Vb.clear();
// 	newHidden.Vb1.clear();
// 	
// 	//Per generare automaticamente un array di bytes per ogni Next
// 	bufOriginal.clear();
// 	steps = 0;
// }


BRG::BRG (string _id, unsigned long long _seed)
{
	auto rng = mtfa_fastrng1024(_seed);
	
	//cout << rng.next() << ", " << rng.next() << endl;
	id = _id;
	keylen = 1024;
	keygen = 65536;
	kOriginal = KGenerator::MakeKey(keylen, rng.next());
	kHidden   = KGenerator::MakeKey(keylen, rng.next());
		
	newOriginal.Va.clear();
	newOriginal.Va1.clear();
	newOriginal.Vb.clear();
	newOriginal.Vb1.clear();
	newHidden.Va.clear();
	newHidden.Va1.clear();
	newHidden.Vb.clear();
	newHidden.Vb1.clear();
	
	//Per generare automaticamente un array di bytes per ogni Next
	bufOriginal.clear();
	steps = 0;
}

BRG::BRG (string _id, vector<char> & v)
{
//	auto rng = mtfa_fastrng1024(_seed);
	
	//cout << rng.next() << ", " << rng.next() << endl;
	id = _id;
	keylen = 1024;
	keygen = 65536;
    int pos=0;

    unsigned long long seed = *(unsigned long long*)&(v.data()[0]);  //i primi 8
    auto rng = mtfa_fastrng1024(seed);

	kOriginal = KGenerator::MakeKey(keylen, v, &pos, rng);
	kHidden   = KGenerator::MakeKey(keylen, v, &pos, rng);
		
	newOriginal.Va.clear();
	newOriginal.Va1.clear();
	newOriginal.Vb.clear();
	newOriginal.Vb1.clear();
	newHidden.Va.clear();
	newHidden.Va1.clear();
	newHidden.Vb.clear();
	newHidden.Vb1.clear();
	
	//Per generare automaticamente un array di bytes per ogni Next
	bufOriginal.clear();
	steps = 0;
}

string BRG::GetId() {
	return id;
}

string BRG::GetBufAsBase64String() {
	return Base64::encode(bufOriginal.size(), bufOriginal.data());
}

vector<unsigned char> BRG::GetBufAsVByte() {
	return bufOriginal;
}

//Il vantaggio è che le dimensioni sono sempre multiplo di 64
vector<unsigned long long> ConvertBitSetToVUint64 ( const boost::dynamic_bitset<> & bs ) {
    vector<unsigned long long> vull ( bs.size() / 64, 0 );
    for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
		//cout << "POS: " << i << endl;
        auto pos = i >> 6;
        auto bit = i & 0x0000003F;
        vull[pos] |= 1ULL << bit;
    }
    return vull;
}

vector<unsigned char> ConvertBitSetToVUint8 ( const boost::dynamic_bitset<> & bs ) {
    vector<unsigned char> vuc ( bs.size() /8, 0 );

    for ( int i=bs.find_first(); i< bs.size(); i = bs.find_next ( i ) ) {
        auto pos = i >> 3;
        auto bit = i & 0x00000007;
        vuc[pos] |= 1 << bit;
    }

    return vuc;
}

boost::dynamic_bitset<> GiveSecondHalfBitSet (const boost::dynamic_bitset<> &setin) {
	int newlen = setin.size() >> 1;
	auto setout = setin >> newlen;
	setout.resize(newlen);
 	return setout;
}

vector<unsigned char> GetFirstHalfAsVbyte(const boost::dynamic_bitset<> &setin) {
	int newlen = setin.size() >> 1;
	auto setout = setin;
	setout.resize(newlen);
	return ConvertBitSetToVUint8(setout);
}

void BRG::Next()
{
	steps++;
	if (newHidden.Va.size() == 0) {
		//Qui ok, poichè gli originali sono ognuno 2048 bit (vedi inizializzazione) e i generati sono 4096 bit ognuno
		auto knew = KGenerator::mfa_generator(keygen, kOriginal, kHidden);
		newOriginal = knew.first;
		newHidden = knew.second;
	}
	//Sempre i vecchi nei nuovi e poi genero i prossimi
	kOriginal.Va  = GiveSecondHalfBitSet(newOriginal.Va);
	kOriginal.Va1 = GiveSecondHalfBitSet(newOriginal.Va1);
	kOriginal.Vb  = GiveSecondHalfBitSet(newOriginal.Vb);
	kOriginal.Vb1 = GiveSecondHalfBitSet(newOriginal.Vb1);

	kHidden.Va  = GiveSecondHalfBitSet(newHidden.Va);
	kHidden.Va1 = GiveSecondHalfBitSet(newHidden.Va1);
	kHidden.Vb  = GiveSecondHalfBitSet(newHidden.Vb);
	kHidden.Vb1 = GiveSecondHalfBitSet(newHidden.Vb1);

// 	cout << "kOriginalVa: ";
// 	for (auto it: ConvertBitSetToVUint64(kOriginal->Va))
// 	{
// 		cout << it << " ";
// 	}
// 	cout << endl;
// 	cout << "newOriginalVa: ";
// 	for (auto it: ConvertBitSetToVUint64(newOriginal->Va))
// 	{
// 		cout << it << " ";
// 	}
// 	cout << endl;
// 	exit(0);
	

	auto knew = KGenerator::mfa_generator(keygen, kOriginal, kHidden);
	newOriginal = knew.first;
	newHidden = knew.second;

	//Converto gli uint64 in byte e prendo la prima metà
	bufOriginal.clear();

	auto b = GetFirstHalfAsVbyte(newOriginal.Va);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Vb);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Va1);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());

	b = GetFirstHalfAsVbyte(newOriginal.Vb1);
	bufOriginal.insert(bufOriginal.end(), b.begin(), b.end());
}


#endif
