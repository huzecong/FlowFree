#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QString>
#include <QPoint>
#include <QThread>
#include "gamesolver.h"

class GameLogic : public QObject {
	Q_OBJECT

	Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
	Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
	Q_PROPERTY(double coveredPercent READ coveredPercent WRITE setCoveredCount NOTIFY coveredPercentChanged)

public:
	static const int colorCnt = 21;
	static const QString colors[colorCnt + 1];
	QHash<QString, int> colorMap;

	GameLogic(QObject *parent = 0) : QObject(parent) {
		this->colorMap.clear();
		this->m_color = NULL;
		this->m_occupy = NULL;
		this->m_point = NULL;
 //       this->m_solver = NULL;
//        this->m_solverThread = NULL;
		this->m_lastX = this->m_lastY = -1;
		for (int i = 1; i <= colorCnt; ++i)
			this->colorMap[colors[i]] = i;
        
        this->m_solver.moveToThread(&this->m_solverThread);
        this->m_solverThread.start();
        
        QObject::connect(this, SIGNAL(__startSolverThread()), &this->m_solver, SLOT(solve()), Qt::QueuedConnection);
        QObject::connect(&this->m_solver, SIGNAL(paintGrid(int,int,int)), this, SLOT(__showCircle(int,int,int)), Qt::QueuedConnection);
        QObject::connect(&this->m_solver, SIGNAL(paintLine(int,int,bool,int)), this, SLOT(__showLine(int,int,bool,int)), Qt::QueuedConnection);
        QObject::connect(&this->m_solver, SIGNAL(noSolution()), this, SIGNAL(noSolution()), Qt::QueuedConnection);
        QObject::connect(&this->m_solver, SIGNAL(solutionFound()), this, SLOT(__loadSolution()), Qt::QueuedConnection);
        QObject::connect(&this->m_solver, SIGNAL(solveFinished(int)), this, SLOT(__solveFinished(int)), Qt::QueuedConnection);
        
        QObject::connect(this, SIGNAL(__solverInit(int,int)), &this->m_solver, SLOT(init(int,int)), Qt::QueuedConnection);
        QObject::connect(this, SIGNAL(__solverSetBoardColor(int,int,int)), &this->m_solver, SLOT(setBoardColor(int,int,int)), Qt::QueuedConnection);
        QObject::connect(this, SIGNAL(__solverShowSolution()), &this->m_solver, SLOT(showSolution()), Qt::QueuedConnection);
	}
    ~GameLogic() {
        delete this->m_color;
        delete this->m_occupy;
        delete this->m_point;
/*      // Let them flow by
        if (this->m_solverThread != NULL) {
            this->m_solverThread->terminate();
        }
        delete this->m_solverThread;
        delete this->m_solver;
        */
    }

	int width() const { return m_columns; }
	void setWidth(const int &width) {
		if (width != this->m_columns) {
			this->m_columns = width;
			emit widthChanged(width);
		}
	}
	int height() const { return m_rows; }
	void setHeight(const int &height) {
		if (height != this->m_rows) {
			this->m_rows = height;
			emit heightChanged(height);
		}
	}
	double coveredPercent() const {
		return (double)this->m_coveredCount / (this->m_rows * this->m_columns - this->m_pairs);
	}
	void setCoveredCount(const int &coveredCount) {
		if (coveredCount != this->m_coveredCount) {
			this->m_coveredCount = coveredCount;
			emit coveredPercentChanged(coveredCount);
		}
	}

	Q_INVOKABLE void loadLevel(QString levelName);
	Q_INVOKABLE void displayCircles();
	Q_INVOKABLE void restart();
    
    Q_INVOKABLE bool canSolve();
    Q_INVOKABLE void solve();
    Q_INVOKABLE void abortSolve();

	// Path drawing methods. Requires coordinates of grid, not mouse
	Q_INVOKABLE void startPath(int x, int y);	// onPressed
	Q_INVOKABLE void movePath(int x, int y);	// onPositionChanged
	Q_INVOKABLE void endPath(int x, int y);		// onReleased
	
	Q_INVOKABLE QString colorAt(int x, int y);

public slots:
    void __showCircle(int x, int y, int color);
    void __showLine(int x1, int y1, bool vertical, int color);
    void __loadSolution();
    void __solveFinished(int time);
    
signals:
	void widthChanged(int);
	void heightChanged(int);
	void coveredPercentChanged(int);
    
    void loadFailed(QString message);
    void loadFinished();
    void noSolution();
    void solveFinished(int time);
    void __startSolverThread();
    void __solverInit(int n, int m);
    void __solverSetBoardColor(int x, int y, int color);
    void __solverShowSolution();

	void hideAll();
	void ripple(int x, int y);
	void changeGridColor(int x, int y, QString color);
	void showCircle(int x, int y, QString color);
	void hideCircle(int x, int y);
	void showLine(int x1, int y1, bool vertical, QString color);
	void hideLine(int x1, int y1, bool vertical);
	
	void playDisconnectedSound();
	void playConnectedSound();
	
	void gameFinished();
	void gameNeedFill();		// All pairs are connected, but board is not filled

private:
	int index(int x, int y) {
		return x * width() + y;
	}
	bool valid(int x, int y) {
		return x >= 0 && x < this->m_rows && y >= 0 && y < this->m_columns;
	}
	
	void truncatePath(int color, QPoint point);
	void extendPath(int color, QPoint point);
	
	void clear();
	bool checkVictory();
	void repaintLines();
	void repaintGrids();

	int m_columns, m_rows, m_coveredCount, m_pairs, m_connectedPairs;
	bool m_connected[colorCnt];
	QHash<int, QList<QPoint> > m_paths;
	int m_curColor, m_lastX, m_lastY, m_lastConPairs;
	int *m_point, *m_color;
	bool *m_occupy;
    
    QThread m_solverThread;
    GameSolver m_solver;
};

#endif // GAMELOGIC_H
