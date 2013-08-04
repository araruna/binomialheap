/*
 * BinomialHeap.hpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#ifndef BINOMIALHEAP_HPP_
#define BINOMIALHEAP_HPP_

#include <new>
#include <stdexcept>
#include "../MemoryManager/MemoryPool.hpp"

class T;

class BinomialHeap {
public:

	class Comparator {
	public:
		virtual bool operator()(double key1, double key2) {
			return key1 <= key2;
		}
	};

	 BinomialHeap(Comparator &precedes = leqdef);
	~BinomialHeap();

	void insert(T* elem);
	void insert(T* elem, double key);

	T*     peekFirstElem();
	double peekFirstKey();

	T* remove();

	unsigned long getSize();

private:

	class Node {
	private:
		static MemoryPool pool;

		T* element;
		double key;
		unsigned long size;

		// *next* aponta para o proximo elemento irmao deste no
		// *children* aponta para o primeiro no na lista de descendentes deste no
		// *last* aponta o ultimo filho na lista de descendentes deste no (para melhorar eficiencia de tempo e memoria na mesclagem de arvores)
		Node *next, *children, *last;

	public:
		friend class BinomialHeap;

		static void* operator new(size_t size) throw (std::length_error);
		static void  operator delete(void* ptr);
	};

	static Comparator leqdef;

	Comparator precedes;
	unsigned long numElems;

	Node *head, *lead;

};

#endif /* BINOMIALHEAP_HPP_ */
