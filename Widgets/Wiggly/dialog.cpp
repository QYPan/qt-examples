#include <QtGui>

 #include "dialog.h"
 #include "wigglywidget.h"

 Dialog::Dialog(QWidget *parent, bool smallScreen)
     : QDialog(parent)
 {
     WigglyWidget *wigglyWidget = new WigglyWidget;
     QLineEdit *lineEdit = new QLineEdit;

     QVBoxLayout *layout = new QVBoxLayout;
     layout->addWidget(wigglyWidget);
     layout->addWidget(lineEdit);
     setLayout(layout);

 #ifdef QT_SOFTKEYS_ENABLED
     QAction *exitAction = new QAction(tr("Exit"), this);
     exitAction->setSoftKeyRole(QAction::NegativeSoftKey);
     connect (exitAction, SIGNAL(triggered()),this, SLOT(close()));
     addAction (exitAction);

     Qt::WindowFlags flags = windowFlags();
     flags |= Qt::WindowSoftkeysVisibleHint;
     setWindowFlags(flags);
 #endif

     connect(lineEdit, SIGNAL(textChanged(QString)),
             wigglyWidget, SLOT(setText(QString)));
     if (!smallScreen){
         lineEdit->setText(tr("Hello world!"));
     }
     else{
         lineEdit->setText(tr("Hello!"));
     }
     setWindowTitle(tr("Wiggly"));
     resize(360, 145);
 }
