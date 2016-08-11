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
	player = Computer;
	isStarted = false;
	isPaused = false;
	boardSize = (BoardWidth - 2) * (BoardHeight - 2);
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
	filledcount = 0;
	if(player == Computer)
		level = 10;
	else
		level = 1;

	initTimeSeed();
	clearBoard();
	snakeList.clear();

	keyDire.y = -1;
	keyDire.x = 0;

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
				drawSquare(painter, i, j, rect.left()+j*squareWidth(),
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
		keyDire.y = -1;
		keyDire.x = 0;
		break;
	case Qt::Key_Down:
		keyDire.y = 1;
		keyDire.x = 0;
		break;
	case Qt::Key_Left:
		keyDire.y = 0;
		keyDire.x = -1;
		break;
	case Qt::Key_Right:
		keyDire.y = 0;
		keyDire.x = 1;
		break;
	default:
		QFrame::keyPressEvent(event);
	}
}

void SnakeBoard::
timerEvent(QTimerEvent *event){
	if(event->timerId() == timer.timerId()){
		if(player == Human){
			if(snakeList.size() <= 1 || (!(keyDire.y+dire.y == 0 && keyDire.x+dire.x == 0))){
				dire.y = keyDire.y;
				dire.x = keyDire.x;
			}
		}
		else{
			computerGo(dire);
		}
		if(!tryMove(dire.y, dire.x)){
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
	int y = snakeList.front().y + d1;
	int x = snakeList.front().x + d2;
	if(stateAt(y, x) == Food){
		addBody(y, x);
		filledcount++;
		emit scoreChanged(++score);
		if(player == Human){
			if(score % 10 == 0){
				++level;
				timer.start(timeoutTime(), this);
				emit levelChanged(level);
			}
		}
		if(filledcount + 1 == boardSize) // had fill the board !
			return false;
		addFood();
	}
	else{
		if(!moveSnake(y, x))
			return false;
	}
	dire.x = d2;
	dire.y = d1;
	return true;
}

bool SnakeBoard::
moveSnake(int y, int x){
	SPoint head = snakeList.front();
	SPoint tail = snakeList.back();

	if(stateAt(y, x) != None){
		if(!(tail.y == y && tail.x == x)) // check if the new head is the body or tail
			return false;
	}

	stateAt(head.y, head.x) = SnakeBody;
	dboard[head.y][head.x].y = y - head.y;
	dboard[head.y][head.x].x = x - head.x;

	stateAt(tail.y, tail.x) = None;
	dboard[tail.y][tail.x].y = 0;
	dboard[tail.y][tail.x].x = 0;

	stateAt(y, x) = SnakeHead;
	snakeList.pop_back();
	snakeList.push_front(SPoint(x, y));
	update();
	return true;
}

void SnakeBoard::
clearBoard(){
	for(int i = 0; i < BoardHeight; i++){
		for(int j = 0; j < BoardWidth; j++){
			dboard[i][j].x = 0;
			dboard[i][j].y = 0;
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
		int tmpx = snakeList.front().x;
		int tmpy = snakeList.front().y;
		stateAt(tmpy, tmpx) = SnakeBody;
		dboard[tmpy][tmpx].y = y - tmpy;
		dboard[tmpy][tmpx].x = x - tmpx;
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
	int x, y;
	while(true){
		x = rand() % BoardWidth;
		y = rand() % BoardHeight;
		if(stateAt(y, x) == None)
			break;
	}
	stateAt(y, x) = Food;
	food.x = x;
	food.y = y;
	update();
}

void SnakeBoard::
drawSquare(QPainter &painter, int r, int c, int x, int y, SquareState state){
	static const QRgb colorTable[5] = {
		0x000000, 0xEF7878, 0xAB7832, 0xCC7839, 0xDD1234
	};
	QColor color = colorTable[int(state)];
	int d = 2;
	if(state == Wall){
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
	else{
		painter.fillRect(x+d, y+d, squareWidth()-2*d, squareHeight()-2*d, color);
		int d1 = dboard[r][c].y;
		int d2 = dboard[r][c].x;
		if(!(d1 && d2)){
			if(d1 == -1 && d2 == 0)
				painter.fillRect(x+d, y-d, squareWidth()-2*d, 2*d, color);
			else if(d1 == 1 && d2 == 0)
				painter.fillRect(x+d, y+d+(squareHeight()-2*d), squareWidth()-2*d, 2*d, color);
			else if(d1 == 0 && d2 == -1)
				painter.fillRect(x-d, y+d, 2*d, squareHeight()-2*d, color);
			else if(d1 == 0 && d2 == 1)
				painter.fillRect(x+d+(squareWidth()-2*d), y+d, 2*d, squareHeight()-2*d, color);
		}
	}
}
