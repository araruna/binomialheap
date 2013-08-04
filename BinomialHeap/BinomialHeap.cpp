/*
 * BinomialHeap.cpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

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
