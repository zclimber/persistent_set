#include <cassert>
#include <iostream>
#include <random>
#include <set>
#include <ctime>

#include "persistentset.h"

using namespace std;

int main() {
	persistent_set pset_1;
	mt19937 mt(time(0));
	int size = 0;
	// memory leak test
	for (int i = 0; i < 100000; i++) {
		if (size > 2000) {
			pset_1.erase(pset_1.begin());
			size--;
		}
		int rnd = mt();
		auto it = pset_1.find(rnd);
		if (it != pset_1.end()) {
			pset_1.erase(it);
			size--;
		} else {
			pset_1.insert(rnd);
			size++;
		}
	}
	set<int> std_set;
	persistent_set pset_2;
	// correctness test + versions test
	vector<persistent_set> vpset;
	vector<set<int>> vset;
	for (int i = 0; i < 50000; i++) {
		int cr = mt();
		std_set.insert(cr);
		pset_2.insert(cr);
		if (std_set.size() > 500) {
			auto it = std_set.lower_bound(mt());
			if (it == std_set.end()) {
				it--;
			}
			pset_2.erase(pset_2.find(*it));
			std_set.erase(it);
		}
		if (i % 1000 == 0) {
			vset.push_back(std_set);
			vpset.push_back(pset_2);
		}
		auto cp = std_set;
		for (auto it = pset_2.begin(); it != pset_2.end(); it++) {
			assert(cp.count(*it) == 1);
			cp.erase(*it);
		}
		assert(cp.size() == 0);
	}
	for (int i = 0; i < (int) vset.size(); i++) {
		for (auto it = vpset[i].begin(); it != vpset[i].end(); it++) {
			assert(vset[i].count(*it) == 1);
			vset[i].erase(*it);
		}
		assert(vset[i].size() == 0);
	}
	cerr << "OK\n";
}
