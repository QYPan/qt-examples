#include <string.h>
#include "PlayArea.h"
#define JUDGE_EDGE(x, y) \
	((x < 0 || x >= MAPSIZE || y < 0 || y >= MAPSIZE) ? 0 : 1)

PlayArea::
PlayArea(QWidget *parent)
	: QWidget(parent), player(0), winner(-1), pathx(-1), pathy(-1)
{
	QSize size(640, 640);
	setFixedSize(size);
	EDGESIZE = qMin(width(), height());
	SUBE = EDGESIZE / 16;
	RADIUS = 2 * SUBE / 5;

	memset(chessMap, -1, sizeof(chessMap));

	chessPixmap = new QPixmap(size);
	chessPixmap->fill(Qt::transparent);
	update();

	initCurrentPath();
	
	QPixmap pixmap(size);
	pixmap.fill(QColor(170, 85, 0));
	QPainter painter(&pixmap);
	painter.setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
	for(int i = 0; i < 15; i++){
		for(int j = 0; j < 15; j++){
			painter.drawLine(SUBE, SUBE+i*SUBE, SUBE*15, SUBE+i*SUBE);
			painter.drawLine(SUBE+i*SUBE, SUBE, SUBE+i*SUBE, SUBE*15);
		}
	}
	setAutoFillBackground(true);
	QPalette pl = palette();
	pl.setBrush(QPalette::Window, QBrush(pixmap));
	setPalette(pl);

	thread = new CalThread();
	connect(thread, SIGNAL(sendSignal(int, int, int)), this, SLOT(addChess(int, int, int)));

	firstGo(1);
	installEventFilter(this);
}

void PlayArea::
initCurrentPath(){
	RADIUS++;
	curpath.moveTo(-RADIUS, RADIUS);
	curpath.lineTo(-(3*RADIUS/4), RADIUS);
	curpath.moveTo(-RADIUS, RADIUS);
	curpath.lineTo(-RADIUS, (3*RADIUS/4));

	curpath.moveTo(RADIUS, RADIUS);
	curpath.lineTo((3*RADIUS/4), RADIUS);
	curpath.moveTo(RADIUS, RADIUS);
	curpath.lineTo(RADIUS, (3*RADIUS/4));

	curpath.moveTo(-RADIUS, -RADIUS);
	curpath.lineTo(-RADIUS, -(3*RADIUS/4));
	curpath.moveTo(-RADIUS, -RADIUS);
	curpath.lineTo(-(3*RADIUS/4), -RADIUS);

	curpath.moveTo(RADIUS, -RADIUS);
	curpath.lineTo(RADIUS, -(3*RADIUS/4));
	curpath.moveTo(RADIUS, -RADIUS);
	curpath.lineTo((3*RADIUS/4), -RADIUS);
	RADIUS--;
}

bool PlayArea::
eventFilter(QObject *obj, QEvent *event){
	if(obj == this){
		if(event->type() == QEvent::KeyPress){
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			int kevent = keyEvent->key();
			if(kevent == Qt::Key_Up || kevent == Qt::Key_Down || 
				kevent == Qt::Key_Left || kevent == Qt::Key_Right
				|| kevent == Qt::Key_Enter || kevent == Qt::Key_Return
				|| kevent == Qt::Key_Q){
				return false;
			}
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return QWidget::eventFilter(obj, event);
	}
}

void PlayArea::
paintEvent(QPaintEvent *event){
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QRect dirtyRect = event->rect();
	painter.drawPixmap(dirtyRect, *chessPixmap, dirtyRect);
}

void PlayArea::
outChessMap(int m[][MAPSIZE+5]){
	for(int i = 0; i < MAPSIZE; i++){
		for(int j = 0; j < MAPSIZE; j++){
			m[i][j] = chessMap[i][j];
		}
	}
}

void PlayArea::
firstGo(int player){
	this->player = player;
	if(!player){
		addChess(MAPSIZE/2, MAPSIZE/2, player);
	}
}

void PlayArea::
drawChess(int x, int y, int player){

	QColor blackColor(0, 0, 0);
	QColor whiteColor(255, 255, 255);

	int lx = SUBE + x * SUBE;
	int ly = SUBE + y * SUBE;
	chessPainter.begin(chessPixmap);
	chessPainter.setRenderHint(QPainter::Antialiasing, true);
	chessPainter.setCompositionMode(QPainter::CompositionMode_Source);
	chessPainter.setPen(Qt::NoPen);
	if(player)
		chessPainter.setBrush(whiteColor);
	else
		chessPainter.setBrush(blackColor);
	chessPainter.drawEllipse(QPoint(lx, ly), RADIUS, RADIUS);
	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
	chessPainter.end();
}

void PlayArea::
drawCurrentPath(int x, int y){
	pathx = x;
	pathy = y;
	int lx = SUBE + x * SUBE;
	int ly = SUBE + y * SUBE;
	chessPainter.begin(chessPixmap);
	chessPainter.setRenderHint(QPainter::Antialiasing, true);
	chessPainter.setCompositionMode(QPainter::CompositionMode_Source);
	chessPainter.setPen(QPen(QBrush(Qt::red), 2));
	QPainterPath tmppath;
	tmppath.addPath(curpath);
	tmppath.translate(lx, ly);
	chessPainter.drawPath(tmppath);

	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
	chessPainter.end();
}

bool PlayArea::
erasePath(int x, int y){
	if((pathx != x && pathy != y) && chessMap[y][x] == -1)
		return false;

	QColor emptyColor(0, 0, 0, 0);
	int lx = SUBE + x * SUBE;
	int ly = SUBE + y * SUBE;

	chessPainter.begin(chessPixmap);
	chessPainter.setRenderHint(QPainter::Antialiasing, true);
	chessPainter.setCompositionMode(QPainter::CompositionMode_Source);
	chessPainter.setPen(Qt::NoPen);
	chessPainter.setBrush(emptyColor);
	chessPainter.drawRect(QRect(lx-RADIUS, ly-RADIUS, 2*RADIUS, 2*RADIUS).
			normalized().adjusted(-4, -4, 4, 4));
	update(QRect(QPoint(lx-RADIUS, ly-RADIUS), QSize(2*RADIUS, 2*RADIUS)).
			normalized().adjusted(-4, -4, 4, 4));
	chessPainter.end();
	return true;
}

bool PlayArea::
addChess(int x, int y, int player){
	if(chessMap[y][x] != -1) // x, y reverse..
		return false;
	chessMap[y][x] = player;

	if(pathx != -1){
		erasePath(pathx, pathy);
		int perplayer = chessMap[pathy][pathx];
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
		int perplayer = chessMap[pathy][pathx];
		if(perplayer != -1)
			drawChess(pathx, pathy, perplayer);
		for(int i = 0; i < 2; i++){
			State s = chessStack.top();
			erasePath(s.x, s.y);
			chessStack.pop();
			chessMap[s.y][s.x] = -1;
		}
		if(!chessStack.empty()){
			State s = chessStack.top();
			drawCurrentPath(s.x, s.y);
		}
	}
}
	
void PlayArea::
computerTurn(){
	if(player)
		return;
	int tmpM[MAPSIZE+5][MAPSIZE+5];
	outChessMap(tmpM);

	thread->setP(player, tmpM);
	thread->start();
}

void PlayArea::
mouseReleaseEvent(QMouseEvent *event){
	int x = event->x();
	int y = event->y();
	if(!player || x < SUBE/2 || x > EDGESIZE-SUBE/2 || y < SUBE/2 || y > EDGESIZE-SUBE/2)
		return;

	int cx = (x - SUBE/2) / SUBE;
	int cy = (y - SUBE/2) / SUBE;
	if(event->button() == Qt::LeftButton){
		addChess(cx, cy, player);
		computerTurn();
	}
	else if(event->button() == Qt::RightButton){
		backChess();	
	}
}

void PlayArea::
keyPressEvent(QKeyEvent *event){
	if(player && pathx != -1){
		if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
			addChess(pathx, pathy, player); 
			computerTurn();
			return;
		}

		erasePath(pathx, pathy);
		int perplayer = chessMap[pathy][pathx];
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
			case Qt::Key_Q:
				backChess();
				break;
			default :
				QWidget::keyPressEvent(event);
				break;
		}
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
	int i = y - d1;
	int j = x - d2;
	int count = 1;
	while(JUDGE_EDGE(i, j) && chessMap[i][j] == p){
		count++;
		i -= d1;
		j -= d2;
	}
	i = y + d1;
	j = x + d2;
	while(JUDGE_EDGE(i, j) && chessMap[i][j] == p){
		count++;
		i += d1;
		j += d2;
	}
	if(count >= 5)
		return p;
	return -1;
}
