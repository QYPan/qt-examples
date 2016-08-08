#include <QtGui>

 #include <stdlib.h>

 #include "tetrixwindow.h"

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     TetrixWindow window;
 #if defined(Q_OS_SYMBIAN)
     window.showMaximized();
 #else
     window.show();
 #endif
     qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
     return app.exec();
 }
