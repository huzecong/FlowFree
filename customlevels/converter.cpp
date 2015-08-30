//Template
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <climits>
#include <cmath>
#include <utility>
#include <set>
#include <map>
#include <queue>
#include <ios>
#include <iomanip>
#include <ctime>
#include <numeric>
#include <functional>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdarg>
#include <complex>
#include <cassert>
using namespace std;

typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;
typedef long double ld;
#define pair(x, y) make_pair(x, y)
#define runtime() ((double)clock() / CLOCKS_PER_SEC)

inline int read() {
	static int r, sign;
	static char c;
	r = 0, sign = 1;
	do c = getchar(); while (c != '-' && (c < '0' || c > '9'));
	if (c == '-') sign = -1, c = getchar();
	while (c >= '0' && c <= '9') r = r * 10 + (int)(c - '0'), c = getchar();
	return sign * r;
}

template <typename T>
inline void print(T *a, int n) {
	for (int i = 1; i < n; ++i) cout << a[i] << " ";
	cout << a[n] << endl;
}
#define PRINT(_l, _r, _s, _t) { cout << #_l #_s "~" #_t #_r ": "; for (int _i = _s; _i != _t; ++_i) cout << _l _i _r << " "; cout << endl; }

#define N 20
int n, m, color[N + 1][N + 1], pairs;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		cerr << "Argument incorrect." << endl;
		cerr << "Usage: converter [pairs_format_file] [matrix_format_file]" << endl;
		return 0;
	}
	
	freopen(argv[1], "r", stdin);
	freopen(argv[2], "w", stdout);
	
	cin >> n >> pairs;
	m = n;
	for (int i = 1; i <= pairs; ++i) {
		int x1, y1, x2, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		color[x1][y1] = color[x2][y2] = i;
	}
	
	cout << n << " " << m << endl;
	for (int i = 1; i <= n; ++i) {
		for (int j = 1; j <= m; ++j)
			cout << "\t" << color[i][j];
		cout << endl;
	}
	
	fclose(stdin);
	fclose(stdout);
	return 0;
}
