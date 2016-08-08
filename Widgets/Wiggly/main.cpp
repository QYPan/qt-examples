#include <QApplication>

 #include "dialog.h"

 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
 #ifdef Q_OS_SYMBIAN
     bool smallScreen = true;
 #else
     bool smallScreen = QApplication::arguments().contains("-small-screen");
 #endif

 #if defined(Q_OS_SYMBIAN) || defined(Q_WS_HILDON) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
     smallScreen = true;
 #endif

     Dialog dialog(0, smallScreen);

     if (!smallScreen)
         dialog.show();
     else
         dialog.showFullScreen();
     return app.exec();
 }
