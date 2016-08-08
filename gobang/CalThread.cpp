#include "CalThread.h"

void CalThread::
run(){
	int num;
	double t, p;
	computerGo(arr, player, x, y, num, t, p);
	emit sendSignal(x, y, player);
	emit sendCalEnd(num, t, p);
}

void CalThread::
setP(int _player, int _arr[][MAPSIZE+5]){
	player = _player;
	for(int i = 0; i < MAPSIZE; i++){
		for(int j = 0; j < MAPSIZE; j++){
			arr[i][j] = _arr[i][j];
		}
	}
}
