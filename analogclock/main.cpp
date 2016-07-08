#include <QApplication>

#include "analogclock.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AnalogClock clock;
#if defined(Q_OS_SYMBIAN)
    clock.showMaximized();
#else
    clock.show();
#endif
    return app.exec();
}
