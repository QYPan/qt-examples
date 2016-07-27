#include <QtGui>

 #include "audioinput.h"

 int main(int argv, char **args)
 {
     QApplication app(argv, args);
     app.setApplicationName("Audio Input Test");

     InputTest input;
 #if defined(Q_OS_SYMBIAN)
     input.showMaximized();
 #else
     input.show();
 #endif

     return app.exec();
 }
