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

const int BITS = 4;

#define N 15
int n, m, board[N + 1][N + 1];

/*
 BITS bits per plug
 0: empty
 1: left plug
 2: right plug
 3~15: colored single plugs
*/
struct State {
	ull state;
	State(ull _state = 0ULL) : state(_state) {}
	
	int get(int p) const {			// Get state of pos p
		return this->state >> ((p - 1) * BITS) & ((1ULL << BITS) - 1);
	}
	State &set(int p, int v) {	// Set state of pos p to v
		this->state &= ~(((1ULL << BITS) - 1) << ((p - 1) * BITS));
		this->state |= ((ull)v) << ((p - 1) * BITS);
		return *this;
	}
	int counterpart(int p) {		// If p is a left or right plug, find its counterpart
		if (get(p) == 1) {
			int cover = 1;
			for (int x = p + 1; cover > 0 && x <= 100; ++x) {
				int v = get(x);
				if (v == 1) ++cover;
				else if (v == 2) --cover;
				if (cover == 0) return x;
			}
			assert(false); return -1;
		} else if (get(p) == 2) {
			int cover = 1;
			for (int x = p - 1; cover > 0 && x >= 0; --x) {
				int v = get(x);
				if (v == 2) ++cover;
				else if (v == 1) --cover;
				if (cover == 0) return x;
			}
			assert(false); return -1;
		} else {
			assert(false); return -1;
		}
	}
	
	inline bool operator < (const State &s) const {
		return state < s.state;
	}
} ;

enum {
	UP = 1,
	LEFT = 2,
	DOWN = 4,
	RIGHT = 8
} ;
struct Info {
	int connection;
	Info(int _con = 0) : connection(_con) {}
} ;

map<State, pair<State, Info>> h[N * N + 2];
Info conn[N + 1][N + 1];
int color[N + 1][N + 1];

const int dir[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
const char label[] = "ULDR";

void bfs(int x, int y, int col) {
	queue<pair<int, int>> q;
	q.push(make_pair(x, y));
	color[x][y] = col;
	while (!q.empty()) {
		pair<int, int> cur = q.front();
		q.pop();
		int x = cur.first, y = cur.second;
		for (int i = 0; i < 4; ++i) {
			if (conn[x][y].connection >> i & 1) {
				int nx = x + dir[i][0], ny = y + dir[i][1];
				if (color[nx][ny]) continue;
				color[nx][ny] = col;
				q.push(make_pair(nx, ny));
			}
		}
	}
}

void print(const State &x, int pos) {
	cerr << x.state << ": ";
	for (int i = 1; i <= m + 1; ++i) {
		if (i == pos) cerr << "(";
		cerr << x.get(i);
		if (i == pos + 1) cerr << ")";
		if (i != m + 1) cerr << " ";
	}
}

int main(int argc, char *argv[]) {
#ifdef KANARI
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif
	
	cin >> n >> m;
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j)
			cin >> board[i][j];
	
	h[1][State(0ULL)] = make_pair(State(0ULL), Info());
	
	for (int i = 1; i <= n; ++i)
		for (int j = 1; j <= m; ++j) {
			int id = (i - 1) * m + j;
			cerr << i << " " << j << " " << h[id].size() << endl;
			for (auto it = h[id].begin(); it != h[id].end(); ++it) {
				State cur = it->first, _cur = cur;
				if (j == 1) {
					if (cur.get(m + 1) != 0) continue;
					cur.state = (cur.state << BITS) & ((1ULL << ((m + 1) * BITS)) - 1);
				}
				
/*				cerr << "\t";
				print(cur, j);
				cerr << "\t";
				if (j == 2) cerr << (it->second.first.state << BITS) << endl;
				else cerr << it->second.first.state << endl;
*/				
				int a = cur.get(j), b = cur.get(j + 1);
				
				if (board[i][j] == 0) {
					if (a == 0 && b == 0) {
						State next = cur;
						next.set(j, 1).set(j + 1, 2);
						h[id + 1][next] = make_pair(_cur, Info(DOWN | RIGHT));
					} else if (a == 0 && b != 0) {
						State next = cur;
						h[id + 1][next] = make_pair(_cur, Info(UP | RIGHT));
						next.set(j, b).set(j + 1, 0);
						h[id + 1][next] = make_pair(_cur, Info(UP | DOWN));
					} else if (a != 0 && b == 0) {
						State next = cur;
						h[id + 1][next] = make_pair(_cur, Info(LEFT | DOWN));
						next.set(j, 0).set(j + 1, a);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | RIGHT));
					}
					
					else if (a == 1 && b == 2) {
						// Would form a loop
						// So no transitions here
					} else if (a == 1 && b == 1) {
						int p = cur.counterpart(j + 1);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, 1);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
					} else if (a == 2 && b == 2) {
						int p = cur.counterpart(j);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, 2);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
					} else if (a == 2 && b == 1) {
						State next = cur;
						next.set(j, 0).set(j + 1, 0);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
					}
					
					else if (a > 2 && b > 2) {
						if (a == b) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
						}
					} else if (a > 2 && b <= 2) {
						int p = cur.counterpart(j + 1);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, a);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
					} else if (a <= 2 && b > 2) {
						int p = cur.counterpart(j);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, b);
						h[id + 1][next] = make_pair(_cur, Info(LEFT | UP));
					} else { assert(false); }
				
				} else {
					if (a == 0 && b == 0) {
						State next = cur;
						next.set(j, board[i][j] + 2);
						h[id + 1][next] = make_pair(_cur, Info(DOWN));
						next = cur;
						next.set(j + 1, board[i][j] + 2);
						h[id + 1][next] = make_pair(_cur, Info(RIGHT));
					} else if (a == 0 && b != 0) {
						if (b <= 2) {
							int p = cur.counterpart(j + 1);
							State next = cur;
							next.set(j, 0).set(j + 1, 0).set(p, board[i][j] + 2);
							h[id + 1][next] = make_pair(_cur, Info(UP));
						} else if (b > 2 && b == board[i][j] + 2) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, Info(UP));
						}
					} else if (a != 0 && b == 0) {
						if (a <= 2) {
							int p = cur.counterpart(j);
							State next = cur;
							next.set(j, 0).set(j + 1, 0).set(p, board[i][j] + 2);
							h[id + 1][next] = make_pair(_cur, Info(LEFT));
						} else if (a > 2 && a == board[i][j] + 2) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, Info(LEFT));
						}
					}
					else {
						// Should not have two plugs
						// So all these are invalid
					}
				}
			}
		}
	
	cerr << h[n * m + 1].size() << endl;
/*	for (auto &it : h[n * m + 1]) {
		print(it.first, m);
		cerr << endl;
	}
*/	
	auto it = h[n * m + 1].find(State(0ULL));
	if (it != h[n * m + 1].end()) {
		cerr << "Found" << endl;
		State state(0ULL);
		for (int x = n; x > 0; --x)
			for (int y = m; y > 0; --y) {
				int id = (x - 1) * m + y;
				auto it = h[id + 1].find(state);
				assert(it != h[id + 1].end());
				conn[x][y] = it->second.second;
				state = it->second.first;
			}
			
		for (int i = 1; i <= n; ++i) {
			for (int j = 1; j <= m; ++j) {
				for (int k = 0; k < 4; ++k)
					cout << ((conn[i][j].connection >> k & 1) ? label[k] : '.');
				cout << " ";
			}
			cout << endl;
		}
		cout << endl;
		
		for (int i = 1; i <= n; ++i)
			for (int j = 1; j <= m; ++j)
				if (board[i][j]) {
					if (!color[i][j]) bfs(i, j, board[i][j]);
					else assert(board[i][j] == color[i][j]);
				}
		
		for (int i = 1; i <= n; ++i) {
			for (int j = 1; j <= m; ++j)
				cout << color[i][j] << " ";
			cout << endl;
		}
	}
	
	fclose(stdin);
	fclose(stdout);
	return 0;
}
