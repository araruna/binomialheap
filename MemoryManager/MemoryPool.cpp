/*
 * MemoryPool.cpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#ifdef __GNUC__
#ifndef _LIKHOOD__M
#define _LIKHOOD__M
#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
#endif
#endif

#include "MemoryPool.hpp"
#include <cmath>

MemoryPool::MemoryPool(size_t chunk_size, unsigned int block_nelem, float load_factor)
throw (std::out_of_range)
: nelems(block_nelem), loadf(load_factor) {
	if(loadf <= 0.0f || loadf > 1.0f)
		throw std::out_of_range("Valor deve estar no intervalo (0.0; 1.0].");

	inuse = 0uL;
	maxinuse = (unsigned long) ceil(loadf*static_cast<float>(nelems));

	available = NULL;
	blocks = NULL;

	chunksize = chunk_size;

	addMemoryBlock(false);
}

MemoryPool::~MemoryPool() {
	MemoryBlock *block, *auxb;

	for(block = blocks; block != NULL; block = auxb) {
		auxb = block->next;
		delete[] block->block;
		delete[] block->units;
		delete block;
	}
}

void
MemoryPool::addMemoryBlock(bool more) {
	MemoryBlock *block = new MemoryBlock;

	block->block = new char[nelems*chunksize];
	block->units = new MemoryUnit[nelems];

	populatePool(block);

	if(likely(more))
		maxinuse += nelems;

	block->next = blocks;
	blocks = block;
}

void
MemoryPool::populatePool(MemoryBlock *block) {
	MemoryUnit *unit = block->units;
	char *address = block->block;
	register unsigned int i;

	for(i = 0; i < nelems; ++i, ++unit, address += chunksize) {
		unit->address = (void*) address;
		unit->next = available;
		available = unit;
	}
}

void*
MemoryPool::getNewElement() {
	MemoryUnit *unit = available;

	available = available->next;

	if(unlikely(++inuse >= maxinuse))
		addMemoryBlock();

	giveawayUnit(unit);

	return unit;
}

void
MemoryPool::returnElement(void *ptr)
throw (std::invalid_argument) {
	MemoryUnit *unit = returnUnit(ptr);

	if(unlikely(unit == NULL))
		throw std::invalid_argument("O endereco de memoria informado nao pertence a este pool.");

	unit->next = available;
	available = unit;
	--inuse;

	returnUnit(unit);
}

void
MemoryPool::giveawayUnit(MemoryUnit *unit) { // Equivalente a inclusao em AVL
	unit->left = unit->right = unit->next = NULL;
	unit->balance = 0;

	if(unlikely(busy == NULL))
		busy = unit;
	else {
		MemoryUnit *parent, *node, *top;

		parent = node = top = busy;

		while(node != NULL) {
			parent = node;

			if(unit->address < parent->address)
				node = parent->left;
			else
				node = parent->right;

			if(parent->balance != 0)
				top = parent;
		}

		(unit->address < parent->address ? parent->left : parent->right) = unit;
		unit->next = parent;

		if(top->balance == 0)
			AVLRebalancePath(top, unit);
		else {
			MemoryUnit *first  = (unit->address < top->address   ? top->left   : top->right);
			MemoryUnit *second = (unit->address < first->address ? first->left : first->right);

			if((unit->address < top->address && top->balance > 0) || (unit->address > top->address && top->balance < 0)) { // Caminho curto
				top->balance = 0;
				AVLRebalancePath(first, unit);
			} else {
				if((top->left == first && first->left == second) || (top->right == first && first->right == second)) {
					top->balance = first->balance = 0;
					AVLRebalancePath(AVLSingleRotate(top, first), unit);
				} else
					AVLRebalancePath(AVLDoubleRotate(top, first, second, unit), unit);
			}
		}
	}
}

MemoryPool::MemoryUnit *
MemoryPool::returnUnit(void *ptr) {
	MemoryUnit *unit = NULL;

	if(unlikely(busy->left == NULL && busy->right == NULL)) {
		unit = busy;
		busy = NULL;
	} else {
		MemoryUnit *antec, *node;

		node = antec = busy;

		while(node != NULL) {
			antec = node;
			if(ptr <= node->address) {
				if(unlikely(ptr == node->address))
					unit = node;
				node = node->left;
			} else
				node = node->right;
		}

		if(likely(unit != NULL)) {
			if(antec != unit) {
				unit->address = antec->address;
				antec->address = ptr;
				unit = antec;
			}

			char child; // armazena de que lado o no a remover esta com relacao ao seu pai

			if(unit->right != NULL) { // Removendo proprio no
				unit->right->next = unit->next;

				if(likely(unit->next != NULL))
					(unit->next->right == unit ? child = 1, unit->next->right : child = -1, unit->next->left) = unit->right;
				else
					busy = unit->right;
			} else { // Removendo antecessor
				if(unit->left != NULL)
					unit->left->next = unit->next;

				if(likely(unit->next != NULL)) {
					(unit->next->right == unit ? child = 1, unit->next->right : child = -1, unit->next->left) = unit->left;
				} else
					busy = unit->left;
			}

			node = unit->next;

			while(node != NULL) {
				if(node->balance*child >= 0) {
					node->balance -= child;
					break;
				} else { // Necessita rotacao
					MemoryUnit *brother = (child < 0 ? node->right : node->left);

					if(brother->balance*node->balance >= 0) { // Rotacao Simples
						AVLSingleRotate(node, brother);
						// TODO Consertar balancos
						node = brother->next;
						if(brother->balance*node->balance == 0)
							break;
					} else { // Rotacao dupla
						MemoryUnit * gdchild = (brother->balance < 0 ? brother->left : brother->right);
						AVLDoubleRotate(node, brother, gdchild);
						// TODO consertar balancos
						node = gdchild->next;
						child = (node != NULL && node->left == gdchild ? -1 : 1);
					}
				}
			}
		}
	}

	return unit;
}

void
MemoryPool::AVLRebalancePath(MemoryUnit *top, MemoryUnit *unit) {
	while(top != NULL && top != unit) {
		if(unit->address < top->address) {
			--top->balance;
			top = top->left;
		} else {
			++top->balance;
			top = top->right;
		}
	}
}

// OBS: usado tanto pela insercao quanto pela remocao
MemoryPool::MemoryUnit *
MemoryPool::AVLSingleRotate(MemoryUnit *parent, MemoryUnit *child) {
	child->next = parent->next;
	parent->next = child;
	if(likely(child->next != NULL))
		(child->next->left == parent ? child->next->left : child->next->right) = child;
	else
		busy = child;

	if(child->address < parent->address) { // Rotacao a direita
		parent->left = child->right;
		if(child->right != NULL)
			child->right->next = parent;
		child->right = parent;

		return child->left;
	} else {
		parent->right = child->left;
		if(child->left != NULL)
			child->left->next = parent;
		child->left = parent;

		return child->right;
	}
}

// OBS: Chamado pela insercao
MemoryPool::MemoryUnit *
MemoryPool::AVLDoubleRotate(MemoryUnit *parent, MemoryUnit *child, MemoryUnit *gdchild, MemoryUnit *unit) {
	MemoryUnit *incr, *decr;

	gdchild->next = parent->next;
	if(likely(gdchild->next != NULL))
		(gdchild->next->left == parent ? gdchild->next->left : gdchild->next->right) = child;
	else
		busy = gdchild;

	child->next = parent->next = child;
	parent->balance = child->balance = gdchild->balance = 0;

	if(child->address < parent->address) { // Rotacao dupla direita
		incr = parent;
		decr = child;
	} else { // Rotacao dupla esquerda
		incr = child;
		decr = parent;
	}

	incr->left = gdchild->right;
	decr->right = gdchild->left;
	gdchild->right = incr;
	gdchild->left = decr;

	if(incr->left != NULL)
		incr->left->next = parent;
	if(decr->right != NULL)
		decr->right->next = child;

	if(unit == gdchild)
		return NULL;
	else if(unit->address < gdchild->address) {
		++incr->balance;
		return decr->right;
	} else {
		--decr->balance;
		return incr->left;
	}
}

// OBS; Chamado pela remocao
void
MemoryPool::AVLDoubleRotate(MemoryUnit *parent, MemoryUnit *child, MemoryUnit *gdchild) {

}
