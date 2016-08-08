#ifndef ACTIONAREA_H
#define ACTIONAREA_H

#include <QtGui>
#include "PlayArea.h"

class ActionArea : public QWidget{
	Q_OBJECT
public:
	ActionArea();
public slots:
	void setState(int num, double t, double per);
	void setFirstGo();
	void startButtonClicked();
private:
	QLabel *createLabel(const QString &str);
	PlayArea *playarea;
	QLabel *stateNumberLabel;
	QLabel *timeUseLabel;
	QLabel *pertimeLabel;
	QComboBox *firstGoComboBox;
};

#endif
