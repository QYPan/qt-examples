#include <QtGui>
#include "snakewindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	SnakeWindow window;
	window.show();
	return app.exec();
}
