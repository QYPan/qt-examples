#ifndef SNAKEWINDOW_H
#define SNAKEWINDOW_H

#include <QFrame>
#include <QWidget>

class QLCDNumber;
class QLabel;
class QPushButton;
class QComboBox;

class SnakeBoard;

class SnakeWindow : public QWidget
{
	Q_OBJECT
public:
	SnakeWindow();
public slots:
	void setPlayer();
private:
	QLabel *createLabel(const QString &text);
	SnakeBoard *board;
	QLCDNumber *scoreLcd;
	QLCDNumber *levelLcd;
	QComboBox *whoPlay;
	QPushButton *startButton;
	QPushButton *quitButton;
	QPushButton *pauseButton;
};

#endif
