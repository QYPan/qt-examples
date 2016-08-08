#include <QtGui>
#include "snakewindow.h"
#include "snakeboard.h"

SnakeWindow::
SnakeWindow(){
	board = new SnakeBoard;
	scoreLcd = new QLCDNumber(5);
	scoreLcd->setSegmentStyle(QLCDNumber::Filled);
	levelLcd = new QLCDNumber(5);
	levelLcd->setSegmentStyle(QLCDNumber::Filled);

	whoPlay = new QComboBox;
	whoPlay->addItem(tr("Computer"), SnakeBoard::Computer);
	whoPlay->addItem(tr("Human"), SnakeBoard::Human);
	whoPlay->setFocusPolicy(Qt::NoFocus);

	startButton = new QPushButton(tr("&Start"));
	startButton->setFocusPolicy(Qt::NoFocus);
	startButton->setShortcut(QKeySequence(Qt::Key_Return));
	quitButton = new QPushButton(tr("&Quit"));
	quitButton->setFocusPolicy(Qt::NoFocus);
	quitButton->setShortcut(QKeySequence(Qt::Key_Q));
	pauseButton = new QPushButton(tr("&Pause"));
	pauseButton->setFocusPolicy(Qt::NoFocus);
	pauseButton->setShortcut(QKeySequence(Qt::Key_Space));

	connect(whoPlay, SIGNAL(activated(int)), this, SLOT(setPlayer()));
	connect(startButton, SIGNAL(clicked()), board, SLOT(start()));
	connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(pauseButton, SIGNAL(clicked()), board, SLOT(pause()));
	connect(board, SIGNAL(scoreChanged(int)), scoreLcd, SLOT(display(int)));
	connect(board, SIGNAL(levelChanged(int)), levelLcd, SLOT(display(int)));

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(board, 0, 0, 9, 4);
	layout->addWidget(createLabel(tr("SCORE")), 0, 4);
	layout->addWidget(scoreLcd, 1, 4);
	layout->addWidget(createLabel(tr("LEVEL")), 2, 4);
	layout->addWidget(levelLcd, 3, 4);
	layout->addWidget(createLabel(tr("WHO PLAY")), 4, 4);
	layout->addWidget(whoPlay, 5, 4);
	layout->addWidget(startButton, 6, 4);
	layout->addWidget(pauseButton, 7, 4);
	layout->addWidget(quitButton, 8, 4);
	setLayout(layout);

	setWindowTitle(tr("Snake"));
	resize(550, 300);
}

void SnakeWindow::
setPlayer(){
	SnakeBoard::Player player = SnakeBoard::Player(whoPlay->itemData(
				whoPlay->currentIndex(), Qt::UserRole).toInt());
	board->setPlayer(player);
}

QLabel *SnakeWindow::
createLabel(const QString &text){
	QLabel *lbl = new QLabel(text);
	lbl->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	return lbl;
}
