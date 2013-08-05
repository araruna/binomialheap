/*
 * main.cpp
 *
 *  Created on: 07/07/2013
 *      Author: eugenio
 */

#include <functional>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "BinomialHeap/BinomialHeap.hpp"

using namespace std;

int main() {
	BinomialHeap heap;
	vector<int> v(50000);
	srand(20);

	for(int i = 0; i < 50000; ++i) {
		v[i] = rand();
		heap.insert(reinterpret_cast<int*>(v[i]));
	}

	sort(v.begin(), v.end(), less<int>());

	for(int i = 0; i < 50000; ++i) {
		if(v[i] != reinterpret_cast<int>(heap.remove())) {
			cerr << "Deu pau: " << __LINE__ << endl;
			return 1;
		}
	}

	for(int i = 0; i < 50000; ++i) {
		v[i] = rand();
		heap.insert(reinterpret_cast<int*>(v[i]));
	}

	sort(v.begin(), v.end(), less<int>());

	for(int i = 0; i < 50000; ++i) {
		if(v[i] != reinterpret_cast<int>(heap.remove())) {
			cerr << "Deu pau: " << __LINE__ << endl;
			return 1;
		}
	}

	for(int i = 0; i < 50000; ++i) {
		v[i] = rand();
		heap.insert(reinterpret_cast<int*>(v[i]));
	}

	sort(v.begin(), v.end(), less<int>());

	for(int i = 0; i < 50000; ++i) {
		if(v[i] != reinterpret_cast<int>(heap.remove())) {
			cerr << "Deu pau: " << __LINE__ << endl;
			return 1;
		}
	}

	return 0;
}
