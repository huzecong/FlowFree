#ifndef GAMESOLVER_H
#define GAMESOLVER_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <map>
#include <utility>
#include <cassert>

class GameSolver : public QObject {
    Q_OBJECT
public:
    static const int MAXN = 16;
    
    explicit GameSolver(QObject *parent = 0) : QObject(parent) {
        this->timer.start();
/*        this->board = new int[MAXN * MAXN];
        this->color = new int[MAXN * MAXN];
        this->conn = new int[MAXN * MAXN];
        this->h = new std::map<State, std::pair<State, int> >[MAXN * MAXN + 1];
*/    }
    ~GameSolver() {
//        recycle();
    }
    /*
    void recycle() {
        qDebug() << "start recycle";
        if (this->color != NULL) {
            delete this->color;
            this->color = NULL;
        }
        if (this->board != NULL) {
            delete this->board;
            this->board = NULL;
        }
        if (this->conn != NULL) {
            delete this->conn;
            this->conn = NULL;
        }
        if (this->h != NULL) {
            delete this->h;
            this->h = NULL;
        }
        qDebug() << "done recycle";
    }
    */
    
public slots:
    void init(int n, int m) {
        this->timer.restart();
        this->n = n;
        this->m = m;
        memset(this->board, 0, sizeof(int) * (n * m));
        memset(this->color, 0, sizeof(int) * (n * m));
        memset(this->conn, 0, sizeof(int) * (n * m));
        for (int i = 0; i <= n * m; ++i)
            h[i].clear();
    }

    void setBoardColor(int x, int y, int color) {
        this->board[index(x, y)] = color;
    }
    
    void solve();
    void showSolution();
    
signals:
    void solutionFound();
    void noSolution();
    void solveFinished(int time);
    void paintGrid(int x, int y, int color);
    void paintLine(int x1, int y1, bool vertical, int color);
    
private:
    typedef unsigned long long ull;
    
    static const int BITS = 4;
    
    struct State {
        ull state;
        State(ull _state = 0ULL) : state(_state) {}
        
        int get(int p) const {			// Get state of pos p
            return this->state >> (p * BITS) & ((1ULL << BITS) - 1);
        }
        State &set(int p, int v) {	// Set state of pos p to v
            this->state &= ~(((1ULL << BITS) - 1) << (p * BITS));
            this->state |= ((ull)v) << (p * BITS);
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
                assert(false && "cannot find right"); return -1;
            } else if (get(p) == 2) {
                int cover = 1;
                for (int x = p - 1; cover > 0 && x >= 0; --x) {
                    int v = get(x);
                    if (v == 2) ++cover;
                    else if (v == 1) --cover;
                    if (cover == 0) return x;
                }
                assert(false && "cannot find left"); return -1;
            } else {
                assert(false && "invalid plug"); return -1;
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
    
    static const int dir[4][2];
    
    void bfs(int x, int y, int color);
    void print(const State &x, int pos);
    
    inline int index(int x, int y) {
        return x * this->m + y;
    }
    
    QTime timer;
//    int n, m, *color, *board, *conn;
    int n, m, color[MAXN * MAXN], board[MAXN * MAXN], conn[MAXN * MAXN];
    std::map<State, std::pair<State, int> > h[MAXN * MAXN + 1];
    
    typedef std::map<State, std::pair<State, int> >::iterator iter;
};

#endif // GAMESOLVER_H
