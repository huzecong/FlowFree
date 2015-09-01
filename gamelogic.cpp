#include "gamelogic.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <algorithm>
#include <cassert>
using namespace std;

const QString GameLogic::colors[] =
{"none",
 "red", "blue", "teal", "yellow", "orange",
 "cyan", "pink", "brown", "purple", "lightGreen",
 "indigo", "blueGrey", "lime", "deepPurple", "amber",
 "lightBlue", "green", "deepOrange", "black", "white",
 "grey"};

void GameLogic::clear() {
	emit hideAll();
	
	this->m_coveredCount = 0;
	if (this->m_point != NULL) {
		for (int i = 0; i < this->m_rows; ++i)
			for (int j = 0; j < this->m_columns; ++j) {
				int id = index(i, j);
				this->m_color[id] = 0;
				this->m_occupy[id] = false;
			}
	}
	memset(this->m_connected, 0, sizeof(bool) * this->colorCnt);
	this->m_connectedPairs = 0;
	this->m_curColor = 0;
	this->m_lastX = this->m_lastY = -1;
	this->m_lastConPairs = 0;
	this->m_paths.clear();
}

void GameLogic::loadLevel(QString levelName) {
	clear();

	QString url(levelName);
	if (url.startsWith(":")) {
		// qrc
	} else if (url.startsWith("file:///")) {
		// OS X or Windows
		url.remove("file://");
		if (url.contains(":")) {
			// Windows
			url.remove(0, 1);
		}
	}
	qDebug() << url;
	QFile file(url);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		emit loadFailed(QString("Failed to open file\nError: %2 (code:%1)").arg(file.errorString(), file.error()));
		return ;
	}
	QTextStream in(&file);

	int _width, _height, _content;
	in >> _width >> _height;
	setWidth(_width);
	setHeight(_height);

	if (m_point != NULL) delete m_point;
	m_point = new int[width() * height()];
	if (m_color != NULL) delete m_color;
	m_color = new int[width() * height()];
	if (m_occupy!= NULL) delete m_occupy;
	m_occupy = new bool[width() * height()];

	int *colorCheck = new int[this->colorCnt + 1];
	memset(colorCheck, 0, sizeof(int) * (this->colorCnt + 1));
	for (int i = 0; i < this->m_rows; ++i)
		for (int j = 0; j < this->m_columns; ++j) {
			int id = index(i, j);
			in >> _content;
			if (_content < 0 || _content > this->colorCnt) {
				emit loadFailed("Level data invalid");
				return ;
			}
			this->m_point[id] = _content;
			this->m_color[id] = 0;
			if (_content != 0) {
				++colorCheck[_content];
			}
		}
	this->m_pairs = 0;
	for (int i = 1; i <= this->colorCnt; ++i) {
		if (colorCheck[i] == 2) ++this->m_pairs;
		else if (colorCheck[i] != 0) {
			emit loadFailed("Level data invalid");
			return ;
		}
	}
	
	if (in.status() != QTextStream::Ok) {
		emit loadFailed("Level data invalid");
		return ;
	}

	emit loadFinished();
}

void GameLogic::displayCircles() {
	for (int i = 0; i < this->m_rows; ++i)
		for (int j = 0; j < this->m_columns; ++j) {
			int id = index(i, j);
			if (m_point[id] != 0) {
				emit showCircle(i, j, colors[m_point[id]]);
			}
		}
}

void GameLogic::restart() {
	clear();

	emit loadFinished();
}

void GameLogic::repaintLines() {
	static int rows = 0, columns = 0;
	static bool *horLines, *verLines;
	if (rows != this->m_rows || columns != this->m_columns) {
		rows = this->m_rows;
		columns = this->m_columns;
		if (horLines != NULL) delete horLines;
		horLines = new bool[rows * columns];
		if (verLines != NULL) delete verLines;
		verLines = new bool[rows * columns];
	}
	memset(horLines, 0, sizeof(bool) * (rows * columns));
	memset(verLines, 0, sizeof(bool) * (rows * columns));
	
	int covered = 0;
	QHash<int, QList<QPoint> >::iterator it;
	for (it = this->m_paths.begin(); it != this->m_paths.end(); ++it) {
		QPoint last(-1, -1);
		for (QList<QPoint>::iterator i = it->begin(); i != it->end(); ++i) {
			int id = index(i->x(), i->y());
			if (it.key() != this->m_curColor) {
				if (this->m_occupy[id]) break;
			}
//			grids[id] = true;
			if (last.x() >= 0) {
				int x1 = last.x(), y1 = last.y(), x2 = i->x(), y2 = i->y();
				if (x1 > x2 || y1 > y2) swap(x1, x2), swap(y1, y2);
				int id = index(x1, y1);
				++covered;
				if (y2 == y1 + 1) {
					emit showLine(x1, y1, false, this->colors[it.key()]);
					horLines[id] = true;
				} else {
					emit showLine(x1, y1, true, this->colors[it.key()]);
					verLines[id] = true;
				}
			}
			last = *i;
		}
	}
	
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j) {
			int id = index(i, j);
//			if (grids[id] == true) {
//				++covered;
//			}
			if (i + 1 < rows && verLines[id] == false) {
				emit hideLine(i, j, true);
			}
			if (j + 1 < columns && horLines[id] == false) {
				emit hideLine(i, j, false);
			}
		}
	setCoveredCount(covered);
}

void GameLogic::repaintGrids() {
	int covered = 0;
	for (int i = 0; i < this->m_rows; ++i)
		for (int j = 0; j < this->m_columns; ++j) {
			int id = index(i, j);
			if (this->m_color[id] == 0) {
				emit changeGridColor(i, j, "grey");
			} else {
				emit changeGridColor(i, j, this->colors[this->m_color[id]]);
				++covered;
			}
		}
}

void GameLogic::truncatePath(int color, QPoint point) {
	QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(color);
	if (it != this->m_paths.end()) {
		QPoint lastPoint;
		while (it->back() != point) {
//			emit changeGridColor(it->back().x(), it->back.y(), "grey");
			this->m_occupy[index(it->back().x(), it->back().y())] = false;
			lastPoint = it->back();
			it->pop_back();
			assert(it->size() > 0);
//			emit hideLine(it->back().x(), it->back.y(), lastPoint.x(), lastPoint.y());
		}
	} else {
		assert(false);
	}
}

inline int sign(int x) {
	return x > 0 ? 1 : x < 0 ? -1 : 0;
}

void GameLogic::extendPath(int color, QPoint point) {
	QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(color);
	if (it != this->m_paths.end()) {
		// Check if path is already connected
		if (it->size() > 1 && this->m_point[index(it->front().x(), it->front().y())] == this->m_curColor
				&& this->m_point[index(it->back().x(), it->back().y())] == this->m_curColor) {
			return ;
		}
		
		int lastChange = -1, lastChange2 = -1;
		while (point != it->back() && !(lastChange == 0 && lastChange2 == 0)) {
			QPoint delta[2];
			// Horizontal (y) first
			delta[0] = QPoint(0, sign(point.y() - it->back().y()));
			delta[1] = QPoint(sign(point.x() - it->back().x()), 0);
			if (abs(point.y() - it->back().y()) < abs(point.x() - it->back().x())) {
				swap(delta[0], delta[1]);
			}
			if (delta[1] == QPoint(0, 0)) {
				delta[1] = delta[0];
			}
			
			int curChange = 0, turn = 0;
			QPoint cur = it->back();
			cur += delta[turn];
			turn ^= 1;
			int id = index(cur.x(), cur.y());
			while (valid(cur.x(), cur.y())
				   && (this->m_point[id] == 0
					   || (this->m_point[id] == color && this->m_occupy[id] == true))) {
				if (this->m_occupy[id] == true) {
					truncatePath(color, cur);
				} else {
					this->m_occupy[id] = true;
					it->append(cur);
				}
//				emit showLine(it->back().x(), it->back.y(), cur.x(), cur.y(), color);
				++curChange;
				if (cur == point) break;
				cur += delta[turn];
				turn ^= 1;
				id = index(cur.x(), cur.y());
			}
			if (this->m_occupy[id] == false && this->m_point[id] == color && it->size() > 1) {
				this->m_occupy[id] = true;
				it->append(cur);
				return ;
			}
			
			lastChange2 = lastChange;
			lastChange = curChange;
		}
	} else {
		assert(false);
	}
}

void GameLogic::startPath(int x, int y) {
	// (x, y) must be valid
	int id = index(x, y);
	QPoint curPoint(x, y);
	if (this->m_point[id] != 0) {
		this->m_curColor = this->m_point[id];
	} else {
		this->m_curColor = this->m_color[id];
	}
	
	if (this->m_curColor != 0) {
		// Initialize "occupy" data (occupied by current path)
		for (int i = 0; i < this->m_rows; ++i)
			for (int j = 0; j < this->m_columns; ++j) {
				int id = index(i, j);
				this->m_occupy[id] = false;
			}
		
		// Extend or truncate path
		QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(this->m_curColor);
		
		if (it == this->m_paths.end() || it->contains(curPoint) == false) {
			// First time visiting
			QList<QPoint> list;
			list.append(curPoint);
			this->m_paths[this->m_curColor] = list;
			it = this->m_paths.find(this->m_curColor);
		}
		emit ripple(it->front().x(), it->front().y());
		for (QList<QPoint>::iterator i = it->begin(); i != it->end(); ++i) {
			int id = index(i->x(), i->y());
			this->m_occupy[id] = true;
			this->m_color[id] = 0;
		}
		truncatePath(this->m_curColor, curPoint);
		
		if (this->m_connected[this->m_curColor] == true) {
			this->m_connected[this->m_curColor] = false;
			--this->m_connectedPairs;
		}
		
//		if (this->m_point[id] != 0 && curPoint != startPoint) {
			// Not to be handled here, should be in "clicked" event
//		} else {
			// Halfway along the path
//		}
		
		this->m_lastX = x, this->m_lastY = y;
		this->m_lastConPairs = this->m_connectedPairs;
		
		repaintLines();
		repaintGrids();
	}
}

void GameLogic::movePath(int x, int y) {
	if (this->m_curColor != 0 && valid(x, y)
			&& (x != this->m_lastX || y != this->m_lastY)) {
		QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(this->m_curColor);
		if (it != this->m_paths.end()) {
			QPoint curPoint(x, y);
			if (it->contains(curPoint)) {
				truncatePath(this->m_curColor, curPoint);
			} else {
				extendPath(this->m_curColor, QPoint(x, y));
			}
		}
		
		this->m_lastX = x, this->m_lastY = y;
		
		repaintLines();
	}
}

void GameLogic::endPath(int x, int y) {
	if (this->m_curColor != 0) {
		int color = this->m_curColor;
		
		movePath(x, y);
		
		QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(this->m_curColor);
		if (it->size() > 1 && this->m_point[index(it->back().x(), it->back().y())] == this->m_curColor) {
			emit ripple(it->front().x(), it->front().y());
			emit ripple(it->back().x(), it->back().y());
			this->m_connected[this->m_curColor] = true;
			++this->m_connectedPairs;
		}
		
		// Save state
		for (int i = 0; i < this->m_rows; ++i)
			for (int j = 0; j < this->m_columns; ++j) {
				int id = index(i, j);
				this->m_color[id] = 0;
			}
		for (it = this->m_paths.begin(); it != this->m_paths.end(); ++it) {
			for (QList<QPoint>::iterator i = it->begin(); i != it->end(); ++i) {
				int id = index(i->x(), i->y());
				if (it.key() != this->m_curColor && this->m_occupy[id]) {
					it->erase(i, it->end());
					if (this->m_connected[it.key()] == true) {
						this->m_connected[it.key()] = false;
						--this->m_connectedPairs;
					}
					break;
				}
				this->m_color[id] = it.key();
			}
		}
		
		for (int i = 0; i < this->m_rows; ++i)
			for (int j = 0; j < this->m_columns; ++j) {
				int id = index(i, j);
				if (this->m_occupy[id]) {
					this->m_color[id] = this->m_curColor;
					this->m_occupy[id] = false;
				}
			}
		this->m_curColor = 0;
		
		repaintLines();
		repaintGrids();
		
		if (checkVictory()) {
			emit gameFinished();
		} else if (this->m_connectedPairs == this->m_pairs) {
			emit gameNeedFill();
		} else {
			int cur = (int)this->m_connected[color];
			if (cur == 1 && this->m_lastConPairs + 1 == this->m_connectedPairs) {
				emit playConnectedSound();
			} else if (this->m_lastConPairs + cur > this->m_connectedPairs) {
				emit playDisconnectedSound();
			}
		}
	}
}

bool GameLogic::checkVictory() {
	return coveredPercent() == 1.0;
}

QString GameLogic::colorAt(int x, int y) {
	int id = index(x, y);
	if (this->m_point[id] != 0) {
		return this->colors[this->m_point[id]];
	} else {
		return this->colors[this->m_color[id]];
	}
}

bool GameLogic::canSolve() {
	return this->m_pairs <= 13 && this->m_columns <= 16;
}

void GameLogic::__showCircle(int x, int y, int color) {
	emit changeGridColor(x, y, this->colors[color]);
	if (this->m_point[index(x, y)] == color) {
		emit showCircle(x, y, this->colors[color]);
	}
}

void GameLogic::__showLine(int x1, int y1, bool vertical, int color) {
	emit showLine(x1, y1, vertical, this->colors[color]);
}

void GameLogic::solve() {
	/*
	this->m_solver.init(this->m_rows, this->m_columns);
	for (int i = 0; i < this->m_rows; ++i)
		for (int j = 0; j < this->m_columns; ++j) {
			int id = index(i, j);
			if (this->m_point[id] != 0) {
				this->m_solver.setBoardColor(i, j, this->m_point[id]);
			}
		}
		*/
	
	emit __solverInit(this->m_rows, this->m_columns);
	for (int i = 0; i < this->m_rows; ++i)
		for (int j = 0; j < this->m_columns; ++j) {
			int id = index(i, j);
			if (this->m_point[id] != 0) {
				emit __solverSetBoardColor(i, j, this->m_point[id]);
			}
		}
	
	emit __startSolverThread();
//	this->m_solver.solve();
}

void GameLogic::__loadSolution() {
	restart();
	emit __solverShowSolution();
}

void GameLogic::__solveFinished(int time) {
	emit solveFinished(time);
}

void GameLogic::abortSolve() {
	assert(false && "unimplemented");

	return;

	this->m_solverThread.terminate();
	this->m_solverThread.exit();
	this->m_solverThread.quit();
}
