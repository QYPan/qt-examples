#ifndef PLAYAREA_H
#define PLAYAREA_H

#include <stack>
#include <QtGui>
#include <QDebug>
#include "CalThread.h"
#define MAPSIZE 15

class PlayArea : public QWidget{
	Q_OBJECT
public:
	enum Player {Computer = 0, Human = 1};
	struct State{
		int x, y, player;
		State(){x = 0, y = 0, player = 0;}
		State(int _x, int _y, int _player){
			x = _x;
			y = _y;
			player = _player;
		}
	};
	PlayArea(QWidget *parent = 0);
	void firstGo(int player = 0);
	void setFirstMan(int player);
	CalThread *thread;
public slots:
	bool addChess(int x, int y, int player);
protected:
	void computerTurn();
	void outChessMap(int m[][MAPSIZE+5]);
	void initCurrentPath();
	void backChess();
	void drawChess(int x, int y, int player);
	bool erasePath(int x, int y);
	void drawCurrentPath(int x, int y);

	bool eventFilter(QObject *obj, QEvent *event);
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	int isWin(int x, int y, int p);
	int calculate(int x, int y, int p, int d1, int d2);
private:
	int chessMap[MAPSIZE+5][MAPSIZE+5];
	std::stack<State> chessStack;
	QPixmap *chessPixmap;
	QPainter chessPainter;
	QPainterPath curpath;
	int player;
	int winner;
	int pathx;
	int pathy;
	int RADIUS; // the radius of the chess
	int SUBE; // the size of each block
	int EDGESIZE; // the size of the chess map
	int states;
	double timeUse;
	double pertime;
};

#endif
