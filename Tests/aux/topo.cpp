#include <bits/stdc++.h>

using namespace std;

#define _ ios_base::sync_with_stdio(0);cin.tie(0);
#define endl '\n'
#define f first
#define s second
#define pb push_back

typedef long long ll;
typedef pair<int, int> ii;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3fll;

int main(int argc, char** argv) { _
	int n = 0;
	map<string, int> mp;
	map<int, string> volta;
	vector<vector<int>> g;
	auto get = [&](string& s) -> int {
		auto it = mp.find(s);
		if (it != mp.end()) return it->s;
		g.push_back(vector<int>());
		volta[n] = s;
		mp[s] = n++;
		return n-1;
	};
	string a, b;
	ifstream in; in.open("in.txt");
	while (in >> a >> b) {
		int aa = get(a), bb = get(b);
		g[aa].pb(bb);
	}
	in.close();
	vector<int> topo, vis(n);
	function<void(int)> dfs = [&](int i) {
		vis[i] = 2;
		for (int j : g[i]) {
			if (!vis[j]) dfs(j);
			else assert(vis[j] != 2);
		}
		topo.pb(i);
		vis[i] = 1;
	};
	for (int i = 0; i < n; i++) if (vis[i] == 0) dfs(i);
	cout << "Topological Sorting:" << endl << endl;
	for (int i : topo) cout << volta[i] << endl;
	
	if (argc > 1) {
		string x = string(argv[1]);
		if (mp.find(x) == mp.end()) return 0;

		cout << endl;
		int cara = mp[x];
		for (int i = 0; i < n; i++) {
			vis = vector<int>(n, 0);
			dfs(i);
			if (vis[cara]) cout << volta[i] << endl;
		}
	}

	exit(0);
}

