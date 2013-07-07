/*
 * MemoryPool.hpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#ifndef MEMORYPOOL_HPP_
#define MEMORYPOOL_HPP_

#include <cstddef>

//template<typename T>
class MemoryPool {
public:
	 MemoryPool(size_t chunk_size, unsigned int block_nelem, float load_factor) throw (std::out_of_range);
	~MemoryPool();

	class MemoryUnit {
	public:
		inline void* getAddress() const { return address; }
	protected:
		MemoryUnit() : address(NULL), pool(NULL), next(NULL) {}
		MemoryUnit(MemoryPool *pool) : address(NULL), pool(pool), next(NULL) {}
	private:
		void *address;
		MemoryPool *const pool;
		MemoryUnit *next;
		//* TODO Implementar ponteiros de arvore para poder realizar a limpeza dos nos alocados
		//*      no momento da destruicao do pool.
	};

	MemoryUnit const * getNewElement();
	void returnElement(MemoryUnit *unit) throw (std::invalid_argument);

private:
	typedef struct __mp_memblock_t {
		struct __mp_memblock_t *next;
		MemoryUnit *units;
		void *block;
	} MemoryBlock;

	unsigned long inuse, maxinuse;
	unsigned int nelems;
	float loadf;
	size_t chunksize;

	MemoryUnit *available;
	MemoryBlock *blocks;

	void populatePool(MemoryBlock *block);
	void addMemoryBlock(bool more = true);
};

#endif /* MEMORYPOOL_HPP_ */
