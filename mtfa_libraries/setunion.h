#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <boost/dynamic_bitset.hpp>

using namespace std;

class SetUnion
{
private:
	int32_t Add(const int32_t a1, const int32_t a2) { return (a1 + a2) % size; }
	int32_t Sub(const int32_t s1, const int32_t s2) { auto s = s1 - s2; if (s < 0) return size + s; else return s; }
	typedef struct __pair__ {
		int32_t ssize;  //indice del primo elemento del Set
		int32_t right;  //indice del primo elemento libero a destra del set (oppure dimensione del set)
	} TPair;
	int32_t size;
public:
	vector<TPair*> ranges; //Uso i puntatori poiché si semplifica il modello. All'inizio tutti i puntatori puntano a loro stessi
	SetUnion(const int32_t _size, bool filled=false);
	~SetUnion();
	bool Insert(const int32_t index);
	bool Delete(const int32_t index);
	bool Find(const int32_t index);
	void Print();
};



class BusyAndFree
{
public:
	SetUnion *su;
	SetUnion *sun;
	BusyAndFree(int size) {
		su = new SetUnion(size);
		sun = new SetUnion(size, true);
	}
	~BusyAndFree() {
		delete su;
		delete sun;
	}
	int IndexOfFirstFreeAfter(int pos)
	{
		//cerco il primo libero dopo o pari a pos
		if (su->Find(pos)) {
			//è occupato, cerco il primo posto libero alla sua destra
			return su->ranges[pos]->right;
		}
		else {
			//è libero. Tutto ok
			return pos;
		}
	}
	int MarkFirstFreeAfter(int pos) {
		//lo inserisce nel primo posto libero!!!
		auto fre = IndexOfFirstFreeAfter(pos);
		if (fre == -1)
			cout << "Ferma qui!" << endl;
		su->Insert(fre);
		sun->Delete(fre);
		return fre;
	}
	int IndexOfFirstBusyAfter(int pos)
	{
		//cerco il primo occupato dopo o pari a pos
		if (su->Find(pos)) {
			return pos;
		}
		else {
			//è libero. cerco il prossimo
			return sun->ranges[pos]->right;
		}
	}
	list<int> IndexOfFirstBusyAfter(int pos, int howmany)
	{
		//cerco il primo occupato dopo o pari a pos
		list<int> li;
		while (howmany > 0) {
			if (su->Find(pos)) {
				auto rightlim = su->ranges[pos]->right;
				if (rightlim < pos)
					rightlim = su->ranges.size();
				for (int i = pos; (howmany-- > 0) && (i < rightlim); ++i)
					li.push_back(i);
				pos = su->ranges[pos]->right;
			}
			else {
				//è libero. cerco il prossimo
				pos = sun->ranges[pos]->right;
			}
		}
		return li;
	}
};


class BooleanBuffer
{
public:
	BusyAndFree * baf;
	bool * values;
	int vsize;
	//boost::dynamic_bitset<> values;

	BooleanBuffer(int size) {
		vsize = size;
		baf = new BusyAndFree(size);
		values = new bool[size]();
	}
	~BooleanBuffer() {
		delete baf;
		delete values;
	}
	int Update(int pos, bool value) {
		values[pos]=value;
		return pos;
	}
	int Insert(int pos, bool value) {
		pos = baf->MarkFirstFreeAfter(pos);
		values[pos]=value;
		return pos;
	}
	bool Get(int pos)
	{
		return values[pos];
	}
	boost::dynamic_bitset<> GetBitSet()
	{
		boost::dynamic_bitset<> bs;
		bs.resize(vsize, false);
		for (int i = 0; i < vsize; ++i)
			if (values[i])
				bs.set(i, true);
		return bs;
	}

	//a partire da index (indirizzo lungo) cerca la posizione del prossimo bit occupato o libero
	int getAddressOfNextBit(const unsigned int pos, const bool occupied)
	{
		return occupied?baf->IndexOfFirstBusyAfter(pos): baf->IndexOfFirstFreeAfter(pos);
	}

	unsigned int getNBitsFromIndex(const int _idx, int numBits) {
		int idx = _idx;
		unsigned int bits = 0;
#if 1
		//cerco il primo occupato dopo o pari a pos e poi proseguo
		while (numBits > 0) {
			if (baf->su->Find(idx)) {
				auto rightlim = baf->su->ranges[idx]->right;
				if (rightlim < idx)
					rightlim = baf->su->ranges.size();
				for (int i = idx; (numBits-- > 0) && (i < rightlim); ++i)
				{
					if (values[i]) {
						bits <<= 1;
						bits |= 1;
					}
					else {
						bits <<= 1;
					}
				}
				idx = baf->su->ranges[idx]->right;
			}
			else {
				//è libero. cerco il prossimo
				idx = baf->sun->ranges[idx]->right;
			}
		}
#else
		int pos = idx;
		while (numBits-- > 0) {
		 	int newpos = baf->IndexOfFirstBusyAfter(pos);
		 	if (values[newpos]) {
		 		bits <<= 1;
		 		bits |= 1;
		 	} else {
		 		bits <<= 1;
		 	}
		 	pos = (newpos + 1) % vsize;
		}
#endif
		return bits;
	}

};
