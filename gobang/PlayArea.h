#ifndef PLAYAREA_H
#define PLAYAREA_H

#include <stack>

class QWidget;
class QPixmap;
class QPaintEvent;

class PlayArea : public QWidget{
public:
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
protected:
	void initCurrentPath();
	bool addChess(int x, int y, int player);
	void backChess();
	void drawChess(int x, int y, int player);
	bool erasePath(int x, int y);
	void drawCurrentPath(int x, int y);
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	int isWin(int x, int y, int p);
	int calculate(int x, int y, int p, int d1, int d2);
private:
	static const int MAPSIZE = 15;
	int chessMap[MAPSIZE][MAPSIZE];
	std::stack<State> chessStack;
	QPixmap *chessPixmap;
	QPainter chessPainter;
	QPainterPath curpath;
	int player;
	int winner;
	int pathx;
	int pathy;
	int RADIUS;
	int PLAYWIDTH;
	int PLAYHEIGHT;
};

#endif
