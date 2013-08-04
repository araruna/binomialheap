/*
 * BinomialHeap.cpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#include "BinomialHeap.hpp"

#ifdef __GNUC__
#ifndef _LIKHOOD__M
#define _LIKHOOD__M
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
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


BinomialHeap::BinomialHeap(Comparator &comp)
 : comp(comp) {

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
	Node *i, *j, *p;

	i = p = head;
	j = new Node;

	j->size = 1;
	j->next = j->children = j->last = NULL;
	j->element = elem;
	j->key = key;

	if(unlikely(head == NULL)) {
		head = lead = j;
	} else {
		while(i != NULL && i->size < j->size) {
			p = i;
			i = i->next;
		}

		if(i == NULL || i->size > j->size) {
			j->next = i;
			p->next = j;
		} else while(i != NULL && i->size == j->size) {
			if(comp(i->key, j->key)) {
				p->next = j;
				j->next = i->next;
				i->next = NULL;

				if(j->last == NULL)
					j->children = j->last = i;
				else {
					j->last->next = i;
					j->last = i;
				}

				// Dobramos o numero de elementos na arvore *j*
				j->size <<= 1;

				// *j* eh sempre a nova arvore
				i = j->next;
			} else {
				if(i->last == NULL)
					i->children = i->last = i;
				else {
					i->last->next = j;
					i->last = j;
				}

				// Dobramos o numero de elementos na arvore *i*
				i->size <<= 1;

				// *j* eh sempre a nova arvore
				j = i;
				i = i->next;
			}
		}

		if(i == NULL)
			lead = j;
	}

	++numElems;
}
