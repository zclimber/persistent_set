#include <bits/stdc++.h>

#include <persistentset.h>

using namespace std;

int main() {
	set<int> ss;
	persistent_set ps;
	ps.printset();
	vector<persistent_set> vps(5);
	for (int i = 0; i < 5; i++) {
		ps.insert(i);
		ps.printset();
		vps[i] = ps;
		ps.insert(i);
		ps.printset();
	}
	persistent_set::iterator pi = ps.end();
	for(int i = 0; i < 5; i++){
		auto it = ps.find(i);
		cout << *it << " !\n";
	}
	auto it = ps.find(2);
	ps.erase(it);
	ps.printset();
	cerr << "OK\n";
}
