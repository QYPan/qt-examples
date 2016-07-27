#include "CalThread.h"

void CalThread::
run(){
	computerGo(arr, player, x, y);
	emit sendSignal(x, y, player);
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
