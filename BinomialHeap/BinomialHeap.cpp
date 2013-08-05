/*
 * BinomialHeap.cpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#include <cfloat>
#include "BinomialHeap.hpp"

#ifndef _LIKHOOD__M
#define _LIKHOOD__M
#ifdef __GNUC__
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif
#endif

MemoryPool BinomialHeap::Node::pool(sizeof(BinomialHeap::Node), 30000u);
BinomialHeap::Comparator BinomialHeap::leqdef;

void*
BinomialHeap::Node::operator new(size_t size)
throw (std::length_error) {
	if(unlikely(size != sizeof(Node)))
		throw std::length_error("Tentativa de alocacao de mais de um elemento.");

	return pool.take();
}

void
BinomialHeap::Node::operator delete(void* ptr) {
	pool.yield(ptr);
}


BinomialHeap::BinomialHeap(Comparator &precedes)
 : precedes(precedes) {

	head = lead = NULL;
	numElems = 0;
}

BinomialHeap::~BinomialHeap() {
	// TODO Percorrer toda a arvore apagando os nos
}

void
BinomialHeap::insert(T* elem) {
	insert(elem, static_cast<double>(reinterpret_cast<unsigned long>(elem)));
}

unsigned long
BinomialHeap::getSize() {
	return numElems;
}

void
BinomialHeap::insert(T* elem, double key) {
	Node *i = head, *j = new Node;

	j->size = 1;
	j->next = j->children = j->last = NULL;
	j->element = elem;
	j->key = key;

	if(unlikely(head == NULL)) {
		head = lead = j;
	} else {
		if(head->size > 1) {
			j->next = head;
			head = j;
		} else {
			while(i != NULL && i->size == j->size) {
				if(precedes(i->key, j->key)) { // *i* deve ficar em cima; *j* vira filho
					if(head == j)
						head = i;

					if(unlikely(i->last == NULL))
						i->children = j;
					else
						i->last->next = j;

					i->last = j;
					j->next = NULL;

					// Dobramos o numero de elementos na arvore *i*
					i->size <<= 1;

					j = i;
					i = i->next;
				} else { // *j* deve ficar em cima; *i* vira filho
					if(head == i)
						head = j;

					if(unlikely(j->last == NULL))
						j->children = i;
					else
						j->last->next = i;

					j->last = i;
					j->next = i->next;
					i->next = NULL;

					// Dobramos o numero de elementos na arvore *j*
					j->size <<= 1;

					i = j->next;
				}
			}

			if(i == NULL)
				lead = j;
		}
	}

	++numElems;
}

double
BinomialHeap::peekFirstKey() {
	if(unlikely(numElems == 0))
		return DBL_MAX;

	double min = head->key;
	Node *node;

	for(node = head->next; node != NULL; node = node->next) {
		if(precedes(node->key, min))
			min = node->key;
	}

	return min;
}

T*
BinomialHeap::peekFirstElem() {
	if(unlikely(numElems == 0))
		return NULL;

	double minv = head->key;
	Node *node, *minn = head;

	for(node = head->next; node != NULL; node = node->next) {
		if(precedes(node->key, minv)) {
			minv = node->key;
			minn = node;
		}
	}

	return minn->element;
}

T*
BinomialHeap::remove() {
	if(unlikely(numElems == 0))
		return NULL;

	T *elem;

	if(head == lead) { // Caso haja apenas uma arvore binomial no heap, o novo heap sera formado por seus filhos
		Node *node = head;

		elem = head->element;
		lead = head->last;
		head = head->children;

		delete node;
	} else { // Caso haja ao menos duas arvores
		Node *node, *prevn, *minn, *prevm;
		double minv;

		prevm = minn = head;
		minv = head->key;

		for(prevn = head, node = head->next; node != NULL; node = node->next) {
			if(precedes(node->key, minv)) {
				minv = node->key;
				minn = node;
				prevm = prevn;
			}

			prevn = node;
		}

		elem = minn->element;

		if(lead == minn) // Como ha >= 2 arvores, minn != prevm se lead == minn
			lead = prevm;
		else if(head == minn)
			head = minn->next;

		prevm->next = minn->next;

		node = minn->children; // *node* agora representa o novo heap que devemos mesclar com o anterior

		delete minn;

		minn = head; // *min* agora representa o novo heap, sem o elemento removido
		prevm = head;

		while(node != NULL && minn != NULL) {
			if(node->size < minn->size) {
				prevn = node->next;
				node->next = minn;

				if(unlikely(minn == head))
					head = node;
				else
					prevm->next = node;

				prevm = node;
				node = prevn;
			} else if(node->size > minn->size) {
				prevm = minn;
				minn = minn->next;
			} else { // Ambos os nos tem o mesmo tamanho
				prevn = node->next;

				while(minn != NULL && minn->size == node->size) {
					if(precedes(minn->key, node->key)) { // *minn* deve ficar em cima; *node* vira filho
						if(head == node)
							head = minn;
						else if(head != minn)
							prevm->next = minn;

						if(unlikely(minn->last == NULL))
							minn->children = node;
						else
							minn->last->next = node;

						minn->last = node;
						node->next = NULL;

						// Dobramos o numero de elementos na arvore *minn*
						minn->size <<= 1;

						node = minn;
						minn = minn->next;
					} else { // *node* deve ficar em cima; *minn* vira filho
						if(head == minn)
							head = node;
						else if(head != node)
							prevm->next = node;

						if(unlikely(node->last == NULL))
							node->children = minn;
						else
							node->last->next = minn;

						node->last = minn;
						node->next = minn->next;
						minn->next = NULL;

						// Dobramos o numero de elementos na arvore *node*
						node->size <<= 1;

						minn = node->next;
					}
				}

				if(minn == NULL)
					lead = node;

				minn = node;
				node = prevn;
			}
		}

		if(unlikely(node != NULL && minn == NULL)) { // Ainda ha elementos a incluir, mas serao incluidos no final
			prevm->next = node;

			while(node->next != NULL)
				node = node->next;

			lead = node;
		}
	}

	--numElems;
	return elem;
}
