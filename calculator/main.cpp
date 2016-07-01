#include <QApplication>
#include "calculator.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Calculator calc;
#if defined(Q_OS_SYMBIAN)
	calc.showMaximized();
#else
	calc.show();
#endif
	return app.exec();
}
