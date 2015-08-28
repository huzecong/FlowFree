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
 "red", "pink", "purple", "deepPurple", "indigo",
 "blue", "lightBlue", "cyan", "teal", "green",
 "lightGreen", "lime", "yellow", "amber", "orange",
 "deepOrange", "grey", "blueGrey", "brown", "black",
 "white"};

void GameLogic::clear() {
    emit hideAll();
    
    this->m_coveredCount = 0;
    if (this->m_point != NULL) {
        for (int i = 0; i < this->m_rows; ++i)
            for (int j = 0; j < this->m_columns; ++j) {
                int id = index(i, j);
                this->m_color[id] = this->m_point[id];
                this->m_occupy[id] = false;
            }
    }
    this->m_curColor = 0;
    this->m_lastX = this->m_lastX = -1;
    this->m_paths.clear();
}

void GameLogic::loadLevel(QString levelName) {
    clear();

	QString url(":/levels/" + levelName + ".txt");
	qDebug() << url;
	QFile file(url);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, "Critical failure", "Failed to load level data");
		return ;
	}
	QTextStream in(&file);

	int _width, _height, _content;
	in >> _width >> _height;
	this->setWidth(_width);
	this->setHeight(_height);

	if (m_point != NULL) delete m_point;
	m_point = new int[width() * height()];
	if (m_color != NULL) delete m_color;
	m_color = new int[width() * height()];
	if (m_occupy!= NULL) delete m_occupy;
	m_occupy = new bool[width() * height()];

	for (int i = 0; i < height(); ++i)
		for (int j = 0; j < width(); ++j) {
			int id = index(i, j);
			in >> _content;
			m_point[id] = m_color[id] = _content;
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

void GameLogic::repaint() {
	static int rows = 0, columns = 0;
	static bool *grids, *horLines, *verLines;
	if (rows != this->m_rows || columns != this->m_columns) {
		rows = this->m_rows;
		columns = this->m_columns;
		if (grids != NULL) delete grids;
		grids = new bool[rows * columns];
		if (horLines != NULL) delete horLines;
		horLines = new bool[rows * columns];
		if (verLines != NULL) delete verLines;
		verLines = new bool[rows * columns];
	}
	memset(grids, 0, sizeof(bool) * (rows * columns));
	memset(horLines, 0, sizeof(bool) * (rows * columns));
	memset(verLines, 0, sizeof(bool) * (rows * columns));
	
	QHash<int, QList<QPoint> >::iterator it;
	for (it = this->m_paths.begin(); it != this->m_paths.end(); ++it) {
		QPoint last(-1, -1);
		for (QList<QPoint>::iterator i = it->begin(); i != it->end(); ++i) {
			int id = index(i->x(), i->y());
			if (it.key() != this->m_curColor) {
				if (this->m_occupy[id]) break;
				emit changeGridColor(i->x(), i->y(), this->colors[it.key()]);
				grids[id] = true;
			}
			if (last.x() >= 0) {
				int x1 = last.x(), y1 = last.y(), x2 = i->x(), y2 = i->y();
				if (x1 > x2 || y1 > y2) swap(x1, x2), swap(y1, y2);
				int id = index(x1, y1);
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
	
	int covered = 0;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j) {
			int id = index(i, j);
			if (grids[id] == false) {
				emit changeGridColor(i, j, "grey");
			} else {
				++covered;
			}
			if (i + 1 < rows && verLines[id] == false) {
				emit hideLine(i, j, true);
			}
			if (j + 1 < columns && horLines[id] == false) {
				emit hideLine(i, j, false);
			}
		}
	setCoveredCount(covered);
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
	this->m_curColor = this->m_color[id];
	
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
			this->m_occupy[index(i->x(), i->y())] = true;
		}
		truncatePath(this->m_curColor, curPoint);
		
//		if (this->m_point[id] != 0 && curPoint != startPoint) {
			// Not to be handled here, should be in "clicked" event
//		} else {
			// Halfway along the path
//		}
        
        this->m_lastX = x, this->m_lastY = y;
		
		repaint();
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
		
		repaint();
	}
}

void GameLogic::endPath(int x, int y) {
	if (this->m_curColor != 0) {
		movePath(x, y);
		
		QHash<int, QList<QPoint> >::iterator it = this->m_paths.find(this->m_curColor);
		if (it->size() > 1 && this->m_point[index(it->back().x(), it->back().y())] == this->m_curColor) {
			emit ripple(it->front().x(), it->front().y());
			emit ripple(it->back().x(), it->back().y());
		}
		
		// Save state
		for (it = this->m_paths.begin(); it != this->m_paths.end(); ++it) {
			if (it.key() == this->m_curColor) continue;
			for (QList<QPoint>::iterator i = it->begin(); i != it->end(); ++i) {
				int id = index(i->x(), i->y());
				if (this->m_occupy[id]) {
					it->erase(i, it->end());
					break;
				}
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
		
		repaint();
		
		if (checkVictory()) {
			emit gameFinished();
		}
	}
}

bool GameLogic::checkVictory() {
	return this->m_coveredCount == this->m_rows * this->m_columns;
}
