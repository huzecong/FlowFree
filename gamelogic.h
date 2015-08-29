#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QString>
#include <QPoint>

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
		this->m_lastX = this->m_lastY = -1;
		for (int i = 1; i <= colorCnt; ++i)
			this->colorMap[colors[i]] = i;
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

	// Path drawing methods. Requires coordinates of grid, not mouse
	Q_INVOKABLE void startPath(int x, int y);	// onPressed
	Q_INVOKABLE void movePath(int x, int y);	// onPositionChanged
	Q_INVOKABLE void endPath(int x, int y);		// onReleased
    
    Q_INVOKABLE QString colorAt(int x, int y);

signals:
	void loadFinished();
	void widthChanged(int);
	void heightChanged(int);
	void coveredPercentChanged(int);

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
    void gameNeedFill();        // All pairs are connected, but board is not filled

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
};

#endif // GAMELOGIC_H
