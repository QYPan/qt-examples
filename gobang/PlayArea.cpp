#include <string.h>
#include "PlayArea.h"
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
	
	QPixmap pixmap(size);
	pixmap.fill(QColor(170, 85, 0));
	QPainter painter(&pixmap);
	painter.setPen(QPen(QBrush(QColor(0, 0, 0)), 2));
	for(int i = 0; i < 15; i++){
		for(int j = 0; j < 15; j++){
			painter.drawLine(40, 40+i*40, 600, 40+i*40);
			painter.drawLine(40+i*40, 40, 40+i*40, 600);
		}
	}
	setAutoFillBackground(true);
	QPalette pl = palette();
	pl.setBrush(QPalette::Window, QBrush(pixmap));
	setPalette(pl);

	thread = new CalThread();
	connect(thread, SIGNAL(sendSignal(int, int, int)), this, SLOT(addChess(int, int, int)));

	firstGo(0);
	installEventFilter(this);
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

bool PlayArea::
eventFilter(QObject *obj, QEvent *event){
	if(obj == this){
		if(event->type() == QEvent::KeyPress){
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			int kevent = keyEvent->key();
			if(kevent == Qt::Key_Up || kevent == Qt::Key_Down || 
				kevent == Qt::Key_Left || kevent == Qt::Key_Right
				|| kevent == Qt::Key_Enter || kevent == Qt::Key_Return){
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

	int lx = 40 + x * 40;
	int ly = 40 + y * 40;
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
	int lx = 40 + x * 40;
	int ly = 40 + y * 40;
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
	int lx = 40 + x * 40;
	int ly = 40 + y * 40;

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

#if 0
	QTextStream out(stdout);
	for(int i = 0; i < MAPSIZE; i++){
		for(int j = 0; j < MAPSIZE; j++){
			out << tmpM[i][j] << " ";
		}
		out << "\n";
	}
#endif

	thread->setP(player, tmpM);
	thread->start();
}

void PlayArea::
mouseReleaseEvent(QMouseEvent *event){
	int x = event->x();
	int y = event->y();
	if(x < 20 || x > 620 || y < 20 || y > 620)
		return;

	int cx = (x - 20) / 40;
	int cy = (y - 20) / 40;
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
	if(pathx != -1){
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
