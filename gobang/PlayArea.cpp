#include <QtGui>
#include "PlayArea.h"
#include <string.h>

PlayArea::
PlayArea(QWidget *parent)
	: QWidget(parent), player(0), winner(-1)
{
	QSize size(PLAYWIDTH, PLAYHEIGHT);
	resize(size);
	memset(chessMap, -1, sizeof(chessMap));

	pixmap = new QPixmap(size);
	pixmap->fill(QColor(0, 127, 127, 131));
	QPainter painter(pixmap);
	painter.setPen(QPen(QBrush(QColor(127, 0, 127, 200)), 3));
	for(int i = 0; i < 15; i++){
		for(int j = 0; j < 15; j++){
			painter.drawLine(40, 40+i*40, 600, 40+i*40);
			painter.drawLine(40+i*40, 40, 40+i*40, 600);
		}
	}
	update();
}

void PlayArea::
paintEvent(QPaintEvent *event){
	QPainter painter(this);
	QRect dirtyRect = event->rect();
	painter.drawPixmap(dirtyRect, *pixmap, dirtyRect);
}

void PlayArea::
mouseReleaseEvent(QMouseEvent *event){
	int x = event->x();
	int y = event->y();
	QPainter painter(pixmap);
	if(event->button() == Qt::LeftButton){
		if(x >= 20 && x <= 620 && y >= 20 && y <= 620){
			int cx = (x - 20) / 40;
			int cy = (y - 20) / 40;
			if(chessMap[cx][cy] != -1)
				return;
			chessMap[cx][cy] = player;
			int lx = 40 + cx * 40;
			int ly = 40 + cy * 40;
			painter.setPen(Qt::NoPen);
			if(player)
				painter.setBrush(QColor(66, 134, 222, 200));
			else
				painter.setBrush(QColor(166, 34, 22, 200));
			painter.drawEllipse(QPoint(lx, ly), 19, 19);
			update(QRect(QPoint(lx-19, ly-19), QSize(38, 38)).
					normalized().adjusted(-2, -2, 2, 2));
			winner = isWin(cx, cy, player);
			player ^= 1;
		}
	}
}

bool PlayArea::
isWin(int x, int y, int p){
}
