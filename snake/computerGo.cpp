#include "snakeboard.h"
#include <math.h>
#include <stdio.h>
#include <QtGui>
#include <string.h>
#include <queue>

int goy[4] = {-1, 1, 0, 0};
int gox[4] = {0, 0, -1, 1};

SquareState tboard[BoardHeight][BoardWidth];
std::list<SPoint> tsnakeList;

struct APoint{
	int d1, d2;
	int x, y;
	int f, g, h;
};

bool operator < (const APoint &p1, const APoint &p2){
	return p1.f > p2.f;
}

int getH(int x1, int y1, int x2, int y2){
	return (abs(y1-y2) + abs(x1-x2));
}

bool A_star(const SPoint &beg, const SPoint &end, SPoint &ans){
	bool vis[BoardHeight][BoardWidth];
	std::priority_queue<APoint> pque;
	memset(vis, 0, sizeof(vis));
	APoint tbeg;
	tbeg.x = beg.x;
	tbeg.y = beg.y;
	tbeg.d1 = tbeg.d2 = 0;
	tbeg.g = 0;
	tbeg.f = tbeg.h = getH(beg.x, beg.y, end.x, end.y);
	pque.push(tbeg);
	vis[tbeg.y][tbeg.x] = true;
	while(!pque.empty()){
		APoint cur = pque.top();
		pque.pop();
		if(cur.y == end.y && cur.x == end.x){
			ans.y = cur.d1;
			ans.x = cur.d2;
			return true;
		}
		for(int i = 0; i < 4; i++){
			APoint next;
			next.y = cur.y + goy[i];
			next.x = cur.x + gox[i];
			if(vis[next.y][next.x]) continue;
			if((tboard[next.y][next.x] == None ||
					tboard[next.y][next.x] == Food) || 
					(end.y == next.y && end.x == next.x)){
				if(cur.d1 == 0 && cur.d2 == 0){
					next.d1 = goy[i];
					next.d2 = gox[i];
				}
				else{
					next.d1 = cur.d1;
					next.d2 = cur.d2;
				}
				next.g = cur.g + 1;
				next.f = next.g + getH(next.x, next.y, end.x, end.y);
				vis[next.y][next.x] = true;
				pque.push(next);
			}
		}
	}
	return false;
}

void changeTboard(int y, int x, bool isEnd){
	SPoint head = tsnakeList.front();
	tboard[head.y][head.x] = SnakeBody;
	tboard[y][x] = SnakeHead;
	tsnakeList.push_front(SPoint(x, y)); // notic the order of x and y !!!!!!!!!!!!
	if(!isEnd){ // if snake had eat the food, no need to cut the tail
		SPoint tail = tsnakeList.back();
		tsnakeList.pop_back();
		tboard[tail.y][tail.x] = None;
	}
}

void Nothing(const SPoint &end, SPoint &point){
	SPoint head = tsnakeList.front();
	SPoint tail = tsnakeList.back();
	tboard[head.y][head.x] = SnakeBody;
	tboard[tail.y][tail.x] = None;
	tsnakeList.pop_back();
	tail = tsnakeList.back();
	int Dist = 0;
	for(int i = 0; i < 4; i++){
		SPoint beg;
		beg.y = head.y + goy[i];
		beg.x = head.x + gox[i];
		if(tboard[beg.y][beg.x] == None){
			tboard[beg.y][beg.x] = SnakeHead;
			SPoint ans;
			if(A_star(beg, tail, ans)){
				int dist = getH(beg.x, beg.y, end.x, end.y);
				if(dist > Dist){
					Dist = dist;
					point.y = goy[i];
					point.x = gox[i];
				}
			}
		}
	}
}

// check if snake can eat food
bool virtualMove(const SPoint &beg, const SPoint &end, SPoint &point){
	SPoint tbeg = beg;
	bool flag = false;
	bool isEnd = false;
	while(!isEnd){
		SPoint ans;
		if(!A_star(tbeg, end, ans)){
			return false;
		}
		if(!flag){ // just need the first step
			point.y = ans.y;
			point.x = ans.x;
			flag = true;
		}
		tbeg.y = tbeg.y + ans.y;
		tbeg.x = tbeg.x + ans.x;
		isEnd = (tbeg.x == end.x && tbeg.y == end.y);
		changeTboard(tbeg.y, tbeg.x, isEnd);
	}
	return true;
}

void SnakeBoard::
computerGo(SPoint &point){

	tsnakeList.clear();
	tsnakeList = snakeList;

	for(int i = 0; i < BoardHeight; i++){
		for(int j = 0; j < BoardWidth; j++){
			tboard[i][j] = board[i][j];
		}
	}

	SPoint head = tsnakeList.front();
	SPoint nextStep;
	bool canEatFood = virtualMove(head, food, nextStep); // check if snake can eat food
	bool canTouchTail = true;
	if(canEatFood){
		if(tsnakeList.size() > 2){ // snake's length must longer then 2
			canTouchTail = false;
			SPoint shead = tsnakeList.front();
			SPoint stail = tsnakeList.back();
			SPoint tpoint;
			canTouchTail = A_star(shead, stail, tpoint); // check if snake can touch it's tail
		}
	}
	if((canEatFood && filledcount + 2 == (BoardHeight-2) * (BoardWidth-2)) || (canEatFood && canTouchTail)){
		point.y = nextStep.y;
		point.x = nextStep.x;
	}
	else{
		for(int i = 0; i < BoardHeight; i++){
			for(int j = 0; j < BoardWidth; j++){
				tboard[i][j] = board[i][j];
			}
		}
		tsnakeList.clear();
		tsnakeList = snakeList;
		Nothing(food, point);
	}
}


