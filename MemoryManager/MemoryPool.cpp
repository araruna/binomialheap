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

MemoryPool::MemoryPool(size_t elem_size, unsigned int elems_per_block)
: nelems(elems_per_block) {
	inuse = nunits = 0uL;

	available = NULL;
	blocks = NULL;

	// Os chunks guardam referencia as suas MemoryUnits
	chunksize = sizeof(MemoryUnit*) + elem_size;

	addMemoryBlock();
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
MemoryPool::addMemoryBlock() {
	MemoryBlock *block = new MemoryBlock;

	block->block = new char[nelems*chunksize];
	block->units = new MemoryUnit[nelems];

	populatePool(block);

	nunits += nelems;

	block->next = blocks;
	blocks = block;
}

void
MemoryPool::populatePool(MemoryBlock *block) {
	MemoryUnit *unit = block->units;
	MemoryUnit **ref;
	char *address = block->block;
	register unsigned int i;

	for(i = 0; i < nelems; ++i, ++unit, address += chunksize) {
		*(ref = address) = unit;

		// Pois a porcao saltada guarda o endereco desta MemoryUnit
		// de forma que possamos recupera-lo quando a memoria for devolvida
		unit->address = static_cast<MemoryUnit*>(address) + 1;
		unit->next = available;

		available = unit;
	}
}

void*
MemoryPool::take() {
	MemoryUnit *unit = available;

	available = available->next;

	if(unlikely(++inuse >= nunits))
		addMemoryBlock();

	return unit->address;
}

void
MemoryPool::yield(void *ptr)
throw (std::invalid_argument) {
	MemoryUnit *unit = static_cast<MemoryUnit*>(ptr) - 1;

	if(likely(isValid(unit))) {
		unit->next = available;
		available = unit;
		--inuse;
	} else
		throw std::invalid_argument("O endereco de memoria informado nao pertence a este pool.");
}

bool
MemoryPool::isValid(MemoryUnit *unit) {
	char *address = reinterpret_cast<char*>(unit);
	size_t offset = nelems*chunksize - 1;

	for(MemoryBlock *block = blocks; block != NULL; block = block->next) {
		if(address >= block->block && address <= block->block + offset)
			return true;
	}

	return false;
}
