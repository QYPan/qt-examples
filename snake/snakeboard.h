#ifndef SNAKEBOARD_H
#define SNAKEBOARD_H

#include <QFrame>
#include <list>
#include <QBasicTimer>

class QLabel;

enum SquareState {None, Food, SnakeHead, SnakeBody, Wall};

class SnakeBoard : public QFrame
{
	Q_OBJECT
public:
	SnakeBoard(QWidget *parent = 0);
	QSize minimumSizeHint() const;
	enum Player {Computer, Human};
	void setPlayer(Player _player) {player = _player;}
	class SPoint{
	public:
		SPoint(){}
		SPoint(int _x, int _y) : ix(_x), iy(_y) {}
		void setX(int _x) {ix = _x;}
		void setY(int _y) {iy = _y;}
		int x() {return ix;}
		int y() {return iy;}
	private:
		int ix, iy;
	};
public slots:
	void start();
	void pause();
signals:
	void scoreChanged(int score);
	void levelChanged(int level);
protected:
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void timerEvent(QTimerEvent *event);
private:
	enum {BoardWidth = 16, BoardHeight = 10};
	SquareState &stateAt(int y, int x) {return board[y][x];}
	int timeoutTime() {return 1000 / (1 + level);}
	int squareWidth() {return contentsRect().width() / BoardWidth;}
	int squareHeight() {return contentsRect().height() / BoardHeight;}
	bool tryMove(int d1, int d2);
	void drawSquare(QPainter &painter, int x, int y, SquareState state);
	void initTimeSeed();
	void clearBoard();
	void moveSnake(int y, int x);
	void addBody(int y, int x);
	void addFood();
	SquareState board[BoardHeight][BoardWidth];
	QBasicTimer timer;
	std::list<SPoint> snakeList;
	SPoint food, keyDire, dire;
	Player player;
	bool isStarted;
	bool isPaused;
	int filledcount; 
	int score;
	int level;
};

#endif
