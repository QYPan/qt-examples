#include <QtGui>
#include "ActionArea.h"

int main(int argc, char *argv[]){
	QApplication app(argc, argv);
	ActionArea actionarea;
	actionarea.show();
	return app.exec();
}
