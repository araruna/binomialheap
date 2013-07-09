/*
 * main.cpp
 *
 *  Created on: 07/07/2013
 *      Author: eugenio
 */

#include <iostream>
#include <list>
#include "MemoryManager/MemoryPool.hpp"

using namespace std;

/*
class Test {
	static MemoryPool mpool;

	void* operator new () {
		return mpool.getNewElement();
	}

	void operator delete(void* ptr) {
		mpool.returnElement((Test*)ptr);
	}
};

MemoryPool Test::mpool(sizeof(Test), 5, 0.6);*/

int main() {
	MemoryPool pool(sizeof(int), 3, 0.5);
	list<void*> pointers;

	for(int i = 0; i < 50; ++i) {
		pointers.push_back(pool.getNewElement());
		*((int*)pointers.back()) = i;
	}

	for(list<MemoryPool::MemoryUnit*>::iterator it = pointers.begin(), end = pointers.end(); it != end; ++it) {
		static int i = 0;
		cout << "indice " << i++ << " vale: " << *((int*)*it) << '\n';
	}

	cout << endl;

	for(int i = 0; i < 25; ++i) {
		pool.returnElement(pointers.front());
		pointers.pop_front();
	}

	for(int i = 0; i < 25; ++i) {
		pointers.push_back(pool.getNewElement());
		*((int*)pointers.back()) = 50-i;
	}

	for(list<MemoryPool::MemoryUnit*>::iterator it = pointers.begin(), end = pointers.end(); it != end; ++it) {
		static int i = 0;
		cout << "indice " << i++ << " vale: " << *((int*)*it) << '\n';
	}

	for(int i = 0; i < 50; ++i) {
		pool.returnElement(pointers.front());
		pointers.pop_front();
	}

	return 0;
}
