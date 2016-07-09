#include <QtGui>
#include "PlayArea.h"

int main(int argc, char *argv[]){
	QApplication app(argc, argv);
	PlayArea playArea;
	playArea.show();
	return app.exec();
}
