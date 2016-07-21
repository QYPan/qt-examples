#include <QtGui>
#include "PlayArea.h"
#include <string.h>
#define JUDGE_EDGE(x, y) \
	((x < 0 || x >= MAPSIZE || y < 0 || y >= MAPSIZE) ? 0 : 1)

PlayArea::
PlayArea(QWidget *parent)
	: QWidget(parent), player(0), winner(-1), pathx(-1), pathy(-1)
{
	PLAYWIDTH = 640;
	PLAYHEIGHT = 640;
	RADIUS = 16;
	QSize size(PLAYWIDTH, PLAYHEIGHT);
	setFixedSize(size);
	memset(chessMap, -1, sizeof(chessMap));

	chessPixmap = new QPixmap(size);
	chessPixmap->fill(Qt::transparent);
	update();

	initCurrentPath();
	
	QPixmap *pixmap = new QPixmap(size);
	pixmap->fill(QColor(170, 85, 0));
	QPainter painter(pixmap);
	painter.setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
	for(int i = 0; i < 15; i++){
		for(int j = 0; j < 15; j++){
			painter.drawLine(40, 40+i*40, 600, 40+i*40);
			painter.drawLine(40+i*40, 40, 40+i*40, 600);
		}
	}
	setAutoFillBackground(true);
	QPalette pl = palette();
	pl.setBrush(QPalette::Window, QBrush(*pixmap));
	setPalette(pl);
}

void PlayArea::
initCurrentPath(){
	RADIUS++;
	curpath.moveTo(-RADIUS, RADIUS);
	curpath.lineTo(-(RADIUS-6), RADIUS);
	curpath.moveTo(-RADIUS, RADIUS);
	curpath.lineTo(-RADIUS, (RADIUS-6));

	curpath.moveTo(RADIUS, RADIUS);
	curpath.lineTo((RADIUS-6), RADIUS);
	curpath.moveTo(RADIUS, RADIUS);
	curpath.lineTo(RADIUS, (RADIUS-6));

	curpath.moveTo(-RADIUS, -RADIUS);
	curpath.lineTo(-RADIUS, -(RADIUS-6));
	curpath.moveTo(-RADIUS, -RADIUS);
	curpath.lineTo(-(RADIUS-6), -RADIUS);

	curpath.moveTo(RADIUS, -RADIUS);
	curpath.lineTo(RADIUS, -(RADIUS-6));
	curpath.moveTo(RADIUS, -RADIUS);
	curpath.lineTo((RADIUS-6), -RADIUS);
	RADIUS--;
}

void PlayArea::
paintEvent(QPaintEvent *event){
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QRect dirtyRect = event->rect();
	painter.drawPixmap(dirtyRect, *chessPixmap, dirtyRect);
}

void PlayArea::
drawChess(int x, int y, int player){

	QColor blackColor(0, 0, 0);
	QColor whiteColor(255, 255, 255);

	int lx = 40 + x * 40;
	int ly = 40 + y * 40;
	chessPainter.setPen(Qt::NoPen);
	if(player)
		chessPainter.setBrush(whiteColor);
	else
		chessPainter.setBrush(blackColor);
	chessPainter.drawEllipse(QPoint(lx, ly), RADIUS, RADIUS);
	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
}

void PlayArea::
drawCurrentPath(int x, int y){
	pathx = x;
	pathy = y;
	int lx = 40 + x * 40;
	int ly = 40 + y * 40;
	chessPainter.setPen(QPen(QBrush(Qt::red), 2));
	QPainterPath tmppath;
	tmppath.addPath(curpath);
	tmppath.translate(lx, ly);
	chessPainter.drawPath(tmppath);

	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
}

bool PlayArea::
erasePath(int x, int y){
	if((pathx != x && pathy != y) && chessMap[x][y] == -1)
		return false;

	QColor emptyColor(0, 0, 0, 0);
	int lx = 40 + x * 40;
	int ly = 40 + y * 40;

	chessPainter.setPen(Qt::NoPen);
	chessPainter.setBrush(emptyColor);
	chessPainter.drawRect(QRect(lx-RADIUS, ly-RADIUS, 2*RADIUS, 2*RADIUS).
			normalized().adjusted(-4, -4, 4, 4));
	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
	return true;
}

bool PlayArea::
addChess(int x, int y, int player){
	if(chessMap[x][y] != -1)
		return false;
	chessMap[x][y] = player;

	if(pathx != -1){
		erasePath(pathx, pathy);
		int perplayer = chessMap[pathx][pathy];
		if(perplayer != -1)
			drawChess(pathx, pathy, perplayer);
	}
	drawChess(x, y, player);
	drawCurrentPath(x, y);
	chessStack.push(State(x, y, player));

	winner = isWin(x, y, player);
	if(winner != -1){
		setEnabled(false);
		QMessageBox::information(this, "Win", "winner", QMessageBox::Ok);
	}

	this->player ^= 1;
	return true;
}

void PlayArea::
backChess(){
	if(chessStack.size() >= 2){
		erasePath(pathx, pathy);
		for(int i = 0; i < 2; i++){
			State s = chessStack.top();
			erasePath(s.x, s.y);
			chessStack.pop();
			chessMap[s.x][s.y] = -1;
		}
		if(!chessStack.empty()){
			State s = chessStack.top();
			drawCurrentPath(s.x, s.y);
		}
	}
}

void PlayArea::
mouseReleaseEvent(QMouseEvent *event){
	int x = event->x();
	int y = event->y();
	if(x < 20 || x > 620 || y < 20 || y > 620)
		return;

	chessPainter.begin(chessPixmap);
	chessPainter.setRenderHint(QPainter::Antialiasing, true);
	chessPainter.setCompositionMode(QPainter::CompositionMode_Source);

	int cx = (x - 20) / 40;
	int cy = (y - 20) / 40;
	if(event->button() == Qt::LeftButton){
		if(addChess(cx, cy, player) == false){
			chessPainter.end();
			return;
		}
	}
	else if(event->button() == Qt::RightButton){
		backChess();	
	}
	chessPainter.end();
}

void PlayArea::
keyPressEvent(QKeyEvent *event){
	if(pathx != -1){
		chessPainter.begin(chessPixmap);
		chessPainter.setRenderHint(QPainter::Antialiasing, true);
		chessPainter.setCompositionMode(QPainter::CompositionMode_Source);

		if(event->key() == Qt::Key_Space){
			addChess(pathx, pathy, player); 
			chessPainter.end();
			return;
		}

		erasePath(pathx, pathy);
		int perplayer = chessMap[pathx][pathy];
		if(perplayer != -1)
			drawChess(pathx, pathy, perplayer);

		switch(event->key()){
			case Qt::Key_Up :
				drawCurrentPath(pathx, (pathy-1<0?MAPSIZE-1:pathy-1));
				break;
			case Qt::Key_Down :
				drawCurrentPath(pathx, (pathy+1>=MAPSIZE?0:pathy+1));
				break;
			case Qt::Key_Left :
				drawCurrentPath((pathx-1<0?MAPSIZE-1:pathx-1), pathy);
				break;
			case Qt::Key_Right :
				drawCurrentPath((pathx+1>=MAPSIZE?0:pathx+1), pathy);
				break;
			default :
				break;
		}
		chessPainter.end();
	}
}

int PlayArea::
isWin(int x, int y, int p){
	int ans1 = calculate(x, y, p, 1, 0);
	int ans2 = calculate(x, y, p, 0, 1);
	int ans3 = calculate(x, y, p, 1, 1);
	int ans4 = calculate(x, y, p, -1, 1);
	if(ans1 != -1 || ans2 != -1 || ans3 != -1 || ans4 != -1)
		return p;
	return -1;
}

int PlayArea::
calculate(int x, int y, int p, int d1, int d2){
	int i = x - d1;
	int j = y - d2;
	int count = 1;
	while(JUDGE_EDGE(i, j) && chessMap[i][j] == p){
		count++;
		i -= d1;
		j -= d2;
	}
	i = x + d1;
	j = y + d2;
	while(JUDGE_EDGE(i, j) && chessMap[i][j] == p){
		count++;
		i += d1;
		j += d2;
	}
	if(count >= 5)
		return p;
	return -1;
}
