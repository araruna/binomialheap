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
#include <stdexcept>
#include <cmath>

MemoryPool::MemoryPool(size_t chunk_size, unsigned int block_nelem, float load_factor)
throw (std::out_of_range)
: nelems(block_nelem), loadf(load_factor) {
	if(loadf <= 0.0f || loadf > 1.0f)
		throw std::out_of_range("Valor deve estar no intervalo (0.0; 1.0].");

	inuse = 0uL;
	maxinuse = (unsigned long) ceil(loadf*nelems);

	available = NULL;
	blocks = NULL;

	chunksize = chunk_size;

	addMemoryBlock(false);
}

MemoryPool::~MemoryPool() {
	MemoryBlock *block, *auxb;
	MemoryUnit *unit, *auxu;

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
	block->units = (MemoryUnit*)new char[nelems*sizeof(MemoryUnit)];

	register unsigned int i;
	MemoryUnit *unit = block->units;

	for(i = 0; i < nelems; ++i, ++unit)
		new (unit) MemoryUnit(this);

	populatePool(block);

	if(likely(more))
		maxinuse += nelems;

	block->next = blocks;
	blocks = block;
}

void
MemoryPool::populatePool(MemoryBlock *block) {
	MemoryUnit *unit = block->units;
	void *address = block->block;
	register unsigned int i;

	for(i = 0; i < nelems; ++i, ++unit, address += chunksize) {
		unit->address = address;
		unit->next = available;
		available = unit;
	}
}

MemoryPool::MemoryUnit const *
MemoryPool::getNewElement() {
	MemoryUnit *unit = available;

	available = available->next;

	// OBS: Usado para reconhecer objetos ja devolvidos
	unit->next = (MemoryUnit*) 0x1;

	if(++inuse > maxinuse)
		addMemoryBlock();

	return unit;
}

void
MemoryPool::returnElement(MemoryUnit *unit)
throw (std::invalid_argument) {
	if(likely(unit->pool != this))
		throw std::invalid_argument("Objeto pertence a outro pool de memoria.");

	// Reconhecendo se ja foi devolvido ao pool
	if(likely(unit->next == (MemoryUnit*) 0x1)) {
		--inuse;

		unit->next = available;
		available = unit;
	}
}
