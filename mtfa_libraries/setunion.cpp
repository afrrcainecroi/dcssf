
#include <iostream>
#include <vector>
#include <cstdint>
#include <boost/dynamic_bitset.hpp>
#include <tuple>

#include "setunion.h"

using namespace std;

SetUnion::SetUnion(const int32_t _size, bool filled)
{
	size = _size;
	ranges.resize(size, nullptr);
	if (filled) {
		auto tp = new TPair();
		tp->ssize = size;
		tp->right = -1;
		std::fill(ranges.begin(), ranges.end(), tp);
	}

}

SetUnion::~SetUnion()
{
	//tipo la print
}

bool SetUnion::Find(const int32_t index)
{
	return ranges[index] != nullptr;
}
//Inserisce l'elemento index nella struttura.
//Ricorda che ogni elemento punta al primo libero a destra
//e all'ultimo occupato a sinistra
//c'è un puntatore che punta all'indice occupato più basso (a partire da 0)
bool SetUnion::Insert(const int32_t index)
{
	//Se già presente, esce
	if (ranges[index])
		return true;

	//E' libero e quindi vuoto! Inserisco!

	//il primo libero a destra è il primo libero a destra dell'elemento in posizione index+1
	//l'altra informazione che mi serve è la dimensione del set che si sta formando
	//inoltre se il set a destra coincide con il set a sinistra, allora sto inserendo l'ultimo elemento!!
	auto setOnRight = ranges[Add(index, 1)];
	auto setOnLeft = ranges[Sub(index, 1)];
	if (setOnLeft && setOnLeft == setOnRight)
	{
		//è tutto completato tranne l'elemento corrente. Aggiungo anche questo.
		ranges[index] = setOnLeft;
		setOnLeft->ssize++;

		//I seguenti valori per gestire il set completamente riempito!
		setOnLeft->right = -1;
		return true;
	}

	//il set a destra non è uguale al set a sinistra, posso fare la merge tra i tre puntatori (sinistra, centro, destra)

	//in base alla presenza o assenza a destra e a sinistra
	if (setOnRight)
	{
		if (setOnLeft)
		{
			//A destra e a sinistra c'è qualcosa
			//Ora qui è anche palese che setonleft e setonright saranno fusi insieme e quindi il gioco dei puntatori di root
			//va aggiornato ma questo lo faccio nei punti giusti del codice
			if (setOnLeft->ssize <= setOnRight->ssize)
			{
				//left is smaller, aggiorno lo smaller con il puntatore puntato dal right
				//e aggiorno anche il corrente che prima non puntava nulla
				ranges[index] = setOnRight;

				//Aggiorno la dimensione del set finale
				setOnRight->ssize += 1 + setOnLeft->ssize;

				//ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
				auto pos = Sub(setOnLeft->right, setOnLeft->ssize);

				for (int i = 0; i < setOnLeft->ssize; ++i)
				{
					ranges[Add(pos, i)] = setOnRight;
				}
				delete(setOnLeft);
				return true;
			}
			else
			{
				//right is smaller, aggiorno lo smaller con il puntatore puntato dal left
				//e aggiorno anche il corrente che prima non puntava nulla
				ranges[index] = setOnLeft;

				//Aggiorno la dimensione del set finale
				setOnLeft->ssize += 1 + setOnRight->ssize;
				setOnLeft->right = setOnRight->right;

				//ora devo inserire il puntatore al nuovo insieme nell'insieme più piccolo, il sinistro
				auto pos = Sub(setOnRight->right, setOnRight->ssize);

				for (int i = 0; i < setOnRight->ssize; ++i)
				{
					ranges[Add(pos, i)] = setOnLeft;
				}
				delete(setOnRight);
				return true;
			}
		}
		else
		{
			//a sinistra il nulla e a destra qualcosa
			//mi estendo a destra mentre a sinistra nulla cambia
			ranges[index] = setOnRight;

			//Aggiorno la dimensione del set finale
			setOnRight->ssize += 1;
			return true;
		}
	}
	else
	{
		if (setOnLeft)
		{
			//a destra il nulla
			//a sinistra puntava a me, quindi devo aggiornare il sinistra
			ranges[index] = setOnLeft;

			//Aggiorno la dimensione del set finale
			setOnLeft->ssize += 1;
			setOnLeft->right = Add(index, 1);
			return true;
		}
		else
		{
			//a destra e a sinistra il nulla
			//modifico solo me stesso
			ranges[index] = new TPair();
			ranges[index]->ssize = 1;
			ranges[index]->right = Add(index, 1);
			return true;
		}
	}

	return true;
}


bool SetUnion::Delete(const int32_t index)
{
	//Se non presente, esce
	if (!ranges[index])
		return true;

	//Se fully allora splitto in due
	if (ranges[index]->right < 0)
	{
		//Ti spiezzo in due! cioè ho ancora un solo set che va da index+1 a index-1
		ranges[index]->right = index;
		ranges[index]->ssize--;
		ranges[index] = nullptr;
		return true;
	}

	//Presente, allora che devo fare? se sono solo, è sufficiente autoeliminarmi
	auto setOnRight = ranges[Add(index, 1)];
	auto setOnLeft = ranges[Sub(index, 1)];
	if (!setOnLeft && !setOnRight)
	{
		//nulla intorno a me! mi autoelimino
		delete ranges[index];
		ranges[index] = nullptr;
		return true;
	}
	if (setOnLeft == setOnRight)
	{
		//Appartengo a un insieme che si estende da destra a sinistra.
		//devo aggiornare l'insieme più corto
		auto leftStart = Sub(setOnLeft->right, setOnLeft->ssize);

		auto rightEnd = setOnLeft->right;
		auto sizeOnLeft = Sub(index, leftStart);
		auto sizeOnRight = Sub(rightEnd, index) - 1;
		if (sizeOnLeft <= sizeOnRight)
		{
			//creo un nuovo ptr e lo assegno a sinistra
			//a destra cambia il size ma non l'estremo
			if (sizeOnLeft)
			{
				auto ptr = new TPair();
				ptr->right = index;   //primo libero a destra
				ptr->ssize = sizeOnLeft;
				for (int i = 0; i < sizeOnLeft; ++i)
					ranges[Add(i, leftStart)] = ptr;
			}
			//aggiorno il ptr sulla destra
			setOnRight->ssize -= (sizeOnLeft + 1);

			//Rimuovo l'elemento che devo rimuovere
			ranges[index] = nullptr;
			return true;
		}
		else
		{
			//creo un nuovo ptr e lo assegno a destra
			//a sinistra cambia il size e l'estremo libero che diventa index
			if (sizeOnRight)
			{
				auto ptr = new TPair();
				ptr->right = setOnRight->right;   //primo libero a destra
				ptr->ssize = sizeOnRight;
				for (int i = 0; i < sizeOnRight; ++i)
					ranges[Add(i + 1, index)] = ptr;
			}
			//aggiorno il ptr sulla sinistra
			setOnLeft->ssize -= (sizeOnRight + 1);
			setOnLeft->right = index;

			//Rimuovo l'elemento che devo rimuovere
			ranges[index] = nullptr;
			return true;
		}
	}

	//Se sono qui, allora il nostro index è estremo di un insieme (posizionato alla sua destra o alla sua sinistra)
	if (setOnLeft)
	{
		setOnLeft->ssize--;
		setOnLeft->right = index;
		ranges[index] = nullptr;
		return true;
	}
	else
	{
		//setOnRight
		setOnRight->ssize--;
		ranges[index] = nullptr;
		return true;
	}
}

void SetUnion::Print()
{
	for (int i = 0; i < size; ++i)
	{
		printf("%2d) ", i);
		if (ranges[i]) {
			printf("size: %2d, right: %2d\n", ranges[i]->ssize, ranges[i]->right);
		}
		else {
			printf("NULL\n");
		}
	}
	//int i = 0;
	//int tot = 0;
	//while (tot < size)
	//{
	//	if (ranges[i] && ranges[i]->ssize)
	//	{
	//		cout << "Set: " << i << " size: " << ranges[i]->ssize << " right: " << ranges[i]->right << endl;
	//		tot += ranges[i]->ssize;
	//		i = ranges[i]->right;
	//	}
	//	else
	//	{
	//		tot++;
	//		i++;
	//	}
	//}
}

