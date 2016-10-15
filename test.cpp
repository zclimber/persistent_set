#include <bits/stdc++.h>

#include <persistentset.h>

using namespace std;

int main() {
	persistent_set ps;
	mt19937 mt(11);
	int size = 0;
	// memory leak test
	for(int i = 0;i < 100000; i++){
		if(size > 2000){
			ps.erase(ps.begin());
			size--;
		}
		int rnd = mt();
		auto it = ps.find(rnd);
		if(it != ps.end()){
			ps.erase(it);
			size--;
		} else {
			ps.insert(rnd);
			size++;
		}
	}
	set<int> ss;
	persistent_set pss;
	// correctness test
	for(int i = 0; i < 10000; i++){
		int cr = mt();
		ss.insert(cr);
		pss.insert(cr);
		auto cp = ss;
		for(auto it = ps.begin(); it != ps.end(); it++){
			assert(cp.count(*it) == 1);
			cp.erase(*it);
		}
	}
	cerr << "OK\n";
}
