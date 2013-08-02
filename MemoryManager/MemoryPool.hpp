/*
 * MemoryPool.hpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#ifndef MEMORYPOOL_HPP_
#define MEMORYPOOL_HPP_

#include <new>
#include <cstddef>
#include <stdexcept>

//template<typename T>
class MemoryPool {
public:
	 MemoryPool(size_t chunk_size, unsigned int block_nelem) throw (std::out_of_range);
	~MemoryPool();

	typedef struct __munit_ {
		void *address;
		struct __munit_ *next;         // <-- Uso em Lista encadeada (nos disponiveis)
	} MemoryUnit;

	void* take();
	void  yield(void *ptr) throw (std::invalid_argument);

private:
	typedef struct __mp_memblock_t {
		struct __mp_memblock_t *next;
		MemoryUnit *units;
		char *block;
	} MemoryBlock;

	unsigned long inuse, nunits;
	unsigned int nelems;
	size_t chunksize;

	MemoryUnit *available;
	MemoryUnit *busy;
	MemoryBlock *blocks;

	void populatePool(MemoryBlock *block);
	void addMemoryBlock();
	bool isValid(MemoryUnit *unit);
};

#endif /* MEMORYPOOL_HPP_ */
