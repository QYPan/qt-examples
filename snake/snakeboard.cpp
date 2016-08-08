#include <QtGui>
#include <stdlib.h>
#include "snakeboard.h"

SnakeBoard::
SnakeBoard(QWidget *parent)
	: QFrame(parent)
{
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setFocusPolicy(Qt::StrongFocus);
	clearBoard();
	isStarted = false;
	isPaused = false;
	filledcount = 0;
}

QSize SnakeBoard::
minimumSizeHint() const{
	return QSize(BoardWidth * 5 + frameWidth() * 2,
			BoardHeight * 5 + frameWidth() * 2);
}

void SnakeBoard::
start(){
	if(isPaused)
		return;
	isStarted = true;
	score = 0;
	level = 1;

	initTimeSeed();
	clearBoard();
	snakeList.clear();

	keyDire.setY(-1);
	keyDire.setX(0);

	addBody(BoardHeight/2, BoardWidth/2);
	addFood();

	emit scoreChanged(score);
	emit levelChanged(level);

	timer.start(timeoutTime(), this);
}

void SnakeBoard::
pause(){
	if(!isStarted)
		return;
	isPaused = !isPaused;
	if(isPaused){
		timer.stop();
	}
	else{
		timer.start(timeoutTime(), this);
	}
	update();
}

void SnakeBoard::
paintEvent(QPaintEvent *event){
	QFrame::paintEvent(event);
	QPainter painter(this);
	QRect rect = contentsRect();

	for(int i = 0; i < BoardHeight; i++){
		for(int j = 0; j < BoardWidth; j++){
			SquareState state = stateAt(i, j);
			if(state != None){
				drawSquare(painter, rect.left()+j*squareWidth(),
						rect.top()+i*squareHeight(), state);
			}
		}
	}
}

void SnakeBoard::
keyPressEvent(QKeyEvent *event){
	if(!isStarted || isPaused || player == Computer){
		QFrame::keyPressEvent(event);
		return;
	}
	int key = event->key();
	if(key != Qt::Key_Up && key != Qt::Key_Down && key != Qt::Key_Left &&
			key != Qt::Key_Right){
		QFrame::keyPressEvent(event);
		return;
	}
	switch(key){
	case Qt::Key_Up:
		keyDire.setY(-1);
		keyDire.setX(0);
		break;
	case Qt::Key_Down:
		keyDire.setY(1);
		keyDire.setX(0);
		break;
	case Qt::Key_Left:
		keyDire.setY(0);
		keyDire.setX(-1);
		break;
	case Qt::Key_Right:
		keyDire.setY(0);
		keyDire.setX(1);
		break;
	default:
		QFrame::keyPressEvent(event);
	}
}

void SnakeBoard::
timerEvent(QTimerEvent *event){
	if(event->timerId() == timer.timerId()){
		if(snakeList.size() <= 1 || (!(keyDire.y()+dire.y() == 0 && keyDire.x()+dire.x() == 0))){
			dire.setY(keyDire.y());
			dire.setX(keyDire.x());
		}
		if(!tryMove(dire.y(), dire.x())){
			timer.stop();
			isStarted = false;
			QMessageBox::information(this, "Notice", "Game over!", QMessageBox::Ok);
		}
	}
	else{
		QFrame::timerEvent(event);
	}
}

bool SnakeBoard::
tryMove(int d1, int d2){
	int y = snakeList.front().y() + d1;
	int x = snakeList.front().x() + d2;
	if(stateAt(y, x) != None && stateAt(y, x) != Food)
		return false;
	if(stateAt(y, x) == Food){
		addBody(y, x);
		emit scoreChanged(++score);
		if(score % 10 == 0){
			++level;
			timer.start(timeoutTime(), this);
			emit levelChanged(level);
		}
		addFood();
	}
	else{
		moveSnake(y, x);
	}
	dire.setX(d2);
	dire.setY(d1);
	return true;
}

void SnakeBoard::
moveSnake(int y, int x){
	int heady = snakeList.front().y();
	int headx = snakeList.front().x();
	int taily = snakeList.back().y();
	int tailx = snakeList.back().x();
	stateAt(heady, headx) = SnakeBody;
	stateAt(taily, tailx) = None;
	stateAt(y, x) = SnakeHead;
	snakeList.pop_back();
	snakeList.push_front(SPoint(x, y));
	update();
}

void SnakeBoard::
clearBoard(){
	for(int i = 0; i < BoardHeight; i++){
		for(int j = 0; j < BoardWidth; j++){
			if(!i || !j || i == BoardHeight-1 || j == BoardWidth-1)
				board[i][j] = Wall;
			else
				board[i][j] = None;
		}
	}
}

void SnakeBoard::
addBody(int y, int x){
	if(snakeList.size()){
		int tmpx = snakeList.front().x();
		int tmpy = snakeList.front().y();
		stateAt(tmpy, tmpx) = SnakeBody;
	}
	stateAt(y, x) = SnakeHead;
	snakeList.push_front(SPoint(x, y));
	update();
}

void SnakeBoard::
initTimeSeed(){
	static bool flag = 0;
	if(!flag){
		srand((unsigned int)time(0));
		flag = true;
	}
}

void SnakeBoard::
addFood(){
	if(filledcount >= BoardWidth*BoardHeight)
		return;
	int x, y;
	while(true){
		x = rand() % BoardWidth;
		y = rand() % BoardHeight;
		if(stateAt(y, x) == None)
			break;
	}
	stateAt(y, x) = Food;
	food.setX(x);
	food.setY(y);
	update();
}

void SnakeBoard::
drawSquare(QPainter &painter, int x, int y, SquareState state){
	static const QRgb colorTable[5] = {
		0x000000, 0xEF7878, 0xAB7832, 0xCC7839, 0xDD1234
	};
	QColor color = colorTable[int(state)];
	painter.fillRect(x+1, y+1, squareWidth()-2, squareHeight()-2, color);

	painter.setPen(color.light());
	painter.drawLine(x, y+squareHeight()-1, x, y);
	painter.drawLine(x, y, x+squareWidth()-1, y);

	painter.setPen(color.dark());
	painter.drawLine(x+1, y+squareHeight()-1,
			x+squareWidth()-1, y+squareHeight()-1);
	painter.drawLine(x+squareWidth()-1,y+squareHeight()-1,
			x+squareWidth()-1, y+1);
}
