#ifndef PLAYAREA_H
#define PLAYAREA_H

#define MAPSIZE 15
#define PLAYWIDTH 640
#define PLAYHEIGHT 640

class QWidget;
class QPixmap;
class QPaintEvent;

class PlayArea : public QWidget{
public:
	PlayArea(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	int isWin(int x, int y, int p);
private:
	int chessMap[MAPSIZE][MAPSIZE];
	QPixmap *pixmap;
	int player;
	int winner
};

#endif
