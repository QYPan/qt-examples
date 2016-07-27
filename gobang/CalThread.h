#include <QThread>
#include "fun.h"

class CalThread : public QThread{
	Q_OBJECT
public:
	CalThread(){}
	void setP(int _player, int _arr[][MAPSIZE+5]);
signals:
	void sendSignal(int x, int y, int player);
protected:
	void run();
private:
	int x, y, player;
	int arr[MAPSIZE+5][MAPSIZE+5];
};
