#ifndef SNAKEBOARD_H
#define SNAKEBOARD_H

#include <QFrame>
#include <list>
#include <QBasicTimer>

class QLabel;

enum SquareState {None, Food, SnakeHead, SnakeBody, Wall};
enum {BoardWidth = 10, BoardHeight = 7};

struct SPoint{
	SPoint(){}
	SPoint(int _x, int _y) : x(_x), y(_y) {}
	int x, y;
};

class SnakeBoard : public QFrame
{
	Q_OBJECT
public:
	SnakeBoard(QWidget *parent = 0);
	QSize minimumSizeHint() const;
	enum Player {Computer, Human};
	void setPlayer(Player _player) {player = _player;}
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
	SquareState &stateAt(int y, int x) {return board[y][x];}
	int timeoutTime() {return 1000 / (1 + level);}
	int squareWidth() {return contentsRect().width() / BoardWidth;}
	int squareHeight() {return contentsRect().height() / BoardHeight;}
	bool tryMove(int d1, int d2);
	bool moveSnake(int y, int x);
	void computerGo(SPoint &point);
	void drawSquare(QPainter &painter, int r, int c, int x, int y, SquareState state);
	void initTimeSeed();
	void clearBoard();
	void addBody(int y, int x);
	void addFood();
	SquareState board[BoardHeight][BoardWidth];
	SPoint dboard[BoardHeight][BoardWidth];
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
