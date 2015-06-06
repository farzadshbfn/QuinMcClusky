#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;

#define L(a) (int)((a).size())
#define all(a) (a).begin(), (a).end()

// first holds one's, second holds x's
inline ii merge_pi(ii a, ii b) {
	ii ret(0, 0);
	if (a.second == b.second) {
		if (__builtin_popcount(a.first) == __builtin_popcount(a.first & b.first)) {
			ret.first = a.first & b.first;
			ret.second = a.second | (b.first - ret.first);
		}
	}
	return ret;
}

void inject(map<int, vii> & mp, set<ii> & pis, const vi & m) {
	for (int i = 0; i < L(m); i++) {
		mp[__builtin_popcount(m[i])].push_back(ii(m[i], 0));
		pis.insert(ii(m[i], 0));
	}
}

vii find_pi(const vi & m, const vi & d) {
	map<int, vii> mp[2];
	set<ii> pis;

	inject(mp[0], pis, m);
	inject(mp[0], pis, d);

	for(int i = 0; L(mp[i]) != 0; i ^= 1) {
		auto it = mp[i].begin(), it2 = mp[i].begin();
		it2++;
		for (; it2 != mp[i].end(); it++, it2++) {
			for (int j = 0; j < L(it->second); j++)
				for (int k = 0; k < L(it2->second); k++) {
					ii res = merge_pi((it->second)[j], (it2->second)[k]);
					if (res.first + res.second != 0) {
						mp[1-i][__builtin_popcount(res.first)].push_back(res);
						pis.insert(res);
						pis.erase((it->second)[j]);
						pis.erase((it2->second)[k]);
					}
				}
		}
		mp[i].clear();
	}
	return vii(all(pis));
}

inline bool cover(ii pi, int minterm) {
	return (minterm & (~(pi.second))) == pi.first;
}

bool row_dominance(vii & pis, vi & m) {
	for (int i = 0; i < L(pis); i++)
		for (int j = 0; j < L(pis); j++) { // i will be removed if possible
			if (i == j) continue;
			bool dominates = 1;
			for (int k = 0; k < L(m); k++) {
				dominates &= !cover(pis[i], m[k]) || cover(pis[j], m[k]);
			}
			if (dominates) {
				pis.erase(pis.begin() + i);
				return 1;
			}
		}
	return 0;
}

bool col_dominance(vii & pis, vi & m) {
	for (int i = 0; i < L(m); i++)
		for (int j = 0; j < L(m); j++) { // j will be removed if possible
			if (i == j) continue;
			bool dominates = 1;
			for (int k = 0; k < L(pis); k++)
				dominates &= !cover(pis[k], m[i]) || cover(pis[k], m[j]);
			if (dominates) {
				m.erase(m.begin() + j);
				return 1;
			}
		}
	return 0;
}

bool juse_one(vii & pis, vii & epis, vi & m) {
	for (int i = 0; i < L(m); i++) {
		int jForward = 0, jBackward = L(pis)-1;
		while (!cover(pis[jForward], m[i]) && jForward < L(pis)) jForward++;
		while (!cover(pis[jBackward], m[i]) && jBackward >= 0) jBackward--;
		if (jForward == jBackward) { // juse 1 pi covers this minterm -> epi
			for (i = 0; i < L(m); i++)
				if (cover(pis[jForward], m[i]))
					m.erase(m.begin() + i--);
			epis.push_back(pis[jForward]);
			pis.erase(pis.begin() + jForward);
			return 1;
		}
	}
	return 0;
}

int main() {
	vi m, d;
	int mCnt, dCnt;
	while (cout << "#minterms #dontCares (exit with 0 0)" << endl &&
		   cin >> mCnt >> dCnt && (mCnt || dCnt)) {
		cout << "minterms: " << endl;
		for (int i = 0, minterm; i < mCnt && cin >> minterm; i++)
			m.push_back(minterm);
		cout << "dontCares: " << endl;
		for (int i = 0, dontCare; i < dCnt && cin >> dontCare; i++)
			d.push_back(dontCare);

		int bigBit = max(L(m) ? *max_element(all(m)) : 0, L(d) ? *max_element(all(d)) : 0);
		while (bigBit != (bigBit & -bigBit))
			bigBit ^= bigBit & -bigBit;

		vii pis = find_pi(m, d);
		vii epis;
		while (row_dominance(pis, m) || col_dominance(pis, m) || juse_one(pis, epis, m));
		pis.insert(pis.end(), all(epis));

		string top = "    ", bottom = "f = ";
		for (int i = 0; i < L(pis); i++) {
			for (int j = bigBit, alpha = 0; j > 0; j >>= 1, alpha++)
				if (!(j & pis[i].second)) {
					top    += (j & pis[i].first ? " " : "_");
					bottom += char('a' + alpha);
				}
			top    += (i < L(pis)-1 ? "   " : "");
			bottom += (i < L(pis)-1 ? " + " : "");
		}
		if (L(bottom) == 4) bottom += "1";
		cout << top << endl << bottom << endl;
	}
    return 0;
}