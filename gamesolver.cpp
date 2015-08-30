#include "gamesolver.h"
#include <QDebug>
#include <QThread>
#include <iostream>
#include <queue>
using namespace std;

const int GameSolver::dir[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

void GameSolver::bfs(int x, int y, int col) {
	queue<pair<int, int> > q;
	q.push(make_pair(x, y));
	color[index(x, y)] = col;
	while (!q.empty()) {
		pair<int, int> cur = q.front();
		q.pop();
		int x = cur.first, y = cur.second;
		for (int i = 0; i < 4; ++i) {
			if (conn[index(x, y)] >> i & 1) {
				int nx = x + dir[i][0], ny = y + dir[i][1];
				int id = index(nx, ny);
				if (color[id]) continue;
				color[id] = col;
				q.push(make_pair(nx, ny));
			}
		}
	}
}

void GameSolver::print(const State &x, int pos) {
	cerr << x.state << ": ";
	for (int i = 1; i <= m + 1; ++i) {
		if (i == pos) cerr << "(";
		cerr << x.get(i);
		if (i == pos + 1) cerr << ")";
		if (i != m + 1) cerr << " ";
	}
}

void GameSolver::solve() {
	qDebug() << "Thread:" << QThread::currentThreadId();
	
	h[0][State(0ULL)] = make_pair(State(0ULL), 0);
	
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j) {
			int id = i * m + j;
//			qDebug() << i << j << h[id].size();
			for (iter it = h[id].begin(); it != h[id].end(); ++it) {
				State cur = it->first, _cur = cur;
				if (j == 0) {
					if (cur.get(m) != 0) continue;
					cur.state = (cur.state << BITS) & ((1ULL << ((m + 1) * BITS)) - 1);
				}
				
				int a = cur.get(j), b = cur.get(j + 1);
				
/*				cerr << "\t";
				print(cur, j);
				cerr << "\t";
				if (j == 1) cerr << (it->second.first.state << BITS) << endl;
				else cerr << it->second.first.state << endl;
 */
				if (board[id] == 0) {
					if (a == 0 && b == 0) {
						State next = cur;
						next.set(j, 1).set(j + 1, 2);
						h[id + 1][next] = make_pair(_cur, DOWN | RIGHT);
					} else if (a == 0 && b != 0) {
						State next = cur;
						h[id + 1][next] = make_pair(_cur, UP | RIGHT);
						next.set(j, b).set(j + 1, 0);
						h[id + 1][next] = make_pair(_cur, UP | DOWN);
					} else if (a != 0 && b == 0) {
						State next = cur;
						h[id + 1][next] = make_pair(_cur, LEFT | DOWN);
						next.set(j, 0).set(j + 1, a);
						h[id + 1][next] = make_pair(_cur, LEFT | RIGHT);
					}
					
					else if (a == 1 && b == 2) {
						// Would form a loop
						// So no transitions here
					} else if (a == 1 && b == 1) {
						int p = cur.counterpart(j + 1);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, 1);
						h[id + 1][next] = make_pair(_cur, LEFT | UP);
					} else if (a == 2 && b == 2) {
						int p = cur.counterpart(j);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, 2);
						h[id + 1][next] = make_pair(_cur, LEFT | UP);
					} else if (a == 2 && b == 1) {
						State next = cur;
						next.set(j, 0).set(j + 1, 0);
						h[id + 1][next] = make_pair(_cur, LEFT | UP);
					}
					
					else if (a > 2 && b > 2) {
						if (a == b) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, LEFT | UP);
						}
					} else if (a > 2 && b <= 2) {
						int p = cur.counterpart(j + 1);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, a);
						h[id + 1][next] = make_pair(_cur, LEFT | UP);
					} else if (a <= 2 && b > 2) {
						int p = cur.counterpart(j);
						State next = cur;
						next.set(j, 0).set(j + 1, 0).set(p, b);
						h[id + 1][next] = make_pair(_cur, LEFT | UP);
					} else { assert(false); }
				
				} else {
					if (a == 0 && b == 0) {
						State next = cur;
						next.set(j, board[id] + 2);
						h[id + 1][next] = make_pair(_cur, DOWN);
						next = cur;
						next.set(j + 1, board[id] + 2);
						h[id + 1][next] = make_pair(_cur, RIGHT);
					} else if (a == 0 && b != 0) {
						if (b <= 2) {
							int p = cur.counterpart(j + 1);
							State next = cur;
							next.set(j, 0).set(j + 1, 0).set(p, board[id] + 2);
							h[id + 1][next] = make_pair(_cur, UP);
						} else if (b > 2 && b == board[id] + 2) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, UP);
						}
					} else if (a != 0 && b == 0) {
						if (a <= 2) {
							int p = cur.counterpart(j);
							State next = cur;
							next.set(j, 0).set(j + 1, 0).set(p, board[id] + 2);
							h[id + 1][next] = make_pair(_cur, LEFT);
						} else if (a > 2 && a == board[id] + 2) {
							State next = cur;
							next.set(j, 0).set(j + 1, 0);
							h[id + 1][next] = make_pair(_cur, LEFT);
						}
					}
					else {
						// Should not have two plugs
						// So all these are invalid
					}
				}
			}
		}
	
	qDebug() << "end" << h[n * m].size();
	iter it = h[n * m].find(State(0ULL));
	if (it != h[n * m].end()) {
		State state(0ULL);
		for (int x = n - 1; x >= 0; --x)
			for (int y = m - 1; y >= 0; --y) {
				int id = index(x, y);
				it = h[id + 1].find(state);
				assert(it != h[id + 1].end());
				conn[id] = it->second.second;
				state = it->second.first;
			}
			
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < m; ++j) {
				int id = index(i, j);
				if (board[id]) {
					if (!color[id]) bfs(i, j, board[id]);
					else assert(board[id] == color[id]);
				}
			}
		emit solutionFound();
	} else {
		emit noSolution();
	}
}

void GameSolver::showSolution() {
	for (int i = 0; i <= n * m; ++i)
		h[i].clear();
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j) {
			int id = index(i, j);
			emit paintGrid(i, j, color[id]);
			if (i < n && (conn[id] & DOWN) == DOWN) {
				emit paintLine(i, j, true, color[id]);
			}
			if (j < m && (conn[id] & RIGHT) == RIGHT) {
				emit paintLine(i, j, false, color[id]);
			}
		}
	emit solveFinished(this->timer.elapsed());
//	recycle();
}
