/*
 * MemoryPool.hpp
 *
 *  Created on: 05/07/2013
 *      Author: eugenio
 */

#ifndef MEMORYPOOL_HPP_
#define MEMORYPOOL_HPP_

#include <cstddef>
#include <stdexcept>

//template<typename T>
class MemoryPool {
public:
	 MemoryPool(size_t chunk_size, unsigned int block_nelem, float load_factor) throw (std::out_of_range);
	~MemoryPool();

	class MemoryUnit {
	protected:
		MemoryUnit() : address(NULL) { next = right = left = NULL; balance = 0; }
	private:
		friend class MemoryPool;

		void *address;
		MemoryUnit *next;         // <-- Uso em Lista encadeada (nos disponiveis)
		MemoryUnit *right, *left; // <-- Uso em Arvore AVL (nos em uso)
		char balance;             // <-- Uso em Arvore AVL (nos em uso)
	};

	void* getNewElement();
	void  returnElement(void *ptr) throw (std::invalid_argument);

private:
	typedef struct __mp_memblock_t {
		struct __mp_memblock_t *next;
		MemoryUnit *units;
		char *block;
	} MemoryBlock;

	unsigned long inuse, maxinuse;
	unsigned int nelems;
	float loadf;
	size_t chunksize;

	MemoryUnit *available;
	MemoryUnit *busy;
	MemoryBlock *blocks;

	void populatePool(MemoryBlock *block);
	void addMemoryBlock(bool more = true);

	void giveawayUnit(MemoryUnit *unit);
	MemoryUnit * returnUnit(void *ptr);

	void AVLRebalancePath(MemoryUnit *top, MemoryUnit *unit);
	MemoryUnit * AVLSingleRotate(MemoryUnit *parent, MemoryUnit *child);
	MemoryUnit * AVLDoubleRotate(MemoryUnit *parent, MemoryUnit *child, MemoryUnit *gdchild, MemoryUnit *unit);
	void AVLDoubleRotate(MemoryUnit *parent, MemoryUnit *child, MemoryUnit *gdchild);
};

#endif /* MEMORYPOOL_HPP_ */
