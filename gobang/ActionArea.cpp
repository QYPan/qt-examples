#include "ActionArea.h"

ActionArea::
ActionArea(){
	QHBoxLayout *hlayout = new QHBoxLayout();
	QVBoxLayout *vlayout = new QVBoxLayout();
	QGridLayout *glayout = new QGridLayout();
	playarea = new PlayArea(this);
	hlayout->addWidget(playarea);

	//QLabel *firstGoLabel = new QLabel(tr("first go:"));
	QLabel *firstGoLabel = createLabel(tr("first go:"));
	firstGoComboBox = new QComboBox;
	firstGoComboBox->addItem(tr("computer"), PlayArea::Computer);
	firstGoComboBox->addItem(tr("player"), PlayArea::Human);
	firstGoLabel->setBuddy(firstGoComboBox);
	glayout->addWidget(firstGoLabel, 0, 0, Qt::AlignRight);
	glayout->addWidget(firstGoComboBox, 0, 1, Qt::AlignLeft);

	//QLabel *stateLabel = new QLabel(tr("states:"));
	QLabel *stateLabel = createLabel(tr("states:"));
	glayout->addWidget(stateLabel, 1, 0, Qt::AlignRight);
	//stateNumberLabel = new QLabel(tr("0"));
	stateNumberLabel = createLabel(tr("0"));
	glayout->addWidget(stateNumberLabel, 1, 1, Qt::AlignLeft);

	//QLabel *timeLabel = new QLabel(tr("time:"));
	QLabel *timeLabel = createLabel(tr("time:"));
	glayout->addWidget(timeLabel, 2, 0, Qt::AlignRight);
	//timeUseLabel = new QLabel(tr("0"));
	timeUseLabel = createLabel(tr("0"));
	glayout->addWidget(timeUseLabel, 2, 1, Qt::AlignLeft);

	//QLabel *perLabel = new QLabel(tr("per:"));
	QLabel *perLabel = createLabel(tr("per:"));
	glayout->addWidget(perLabel, 3, 0, Qt::AlignRight);
	//pertimeLabel = new QLabel(tr("0"));
	pertimeLabel = createLabel(tr("0"));
	glayout->addWidget(pertimeLabel, 3, 1, Qt::AlignLeft);

	QPushButton *startButton = new QPushButton(tr("start"));
	startButton->setDefault(true);
	glayout->addWidget(startButton, 4, 0, 1, 2);
	QPushButton *restartButton = new QPushButton(tr("restart"));
	restartButton->setEnabled(false);
	glayout->addWidget(restartButton, 5, 0, 1, 2);

	vlayout->addLayout(glayout);
	hlayout->addLayout(vlayout);
	setLayout(hlayout);

	connect(playarea->thread, SIGNAL(sendCalEnd(int, double, double)),
			this, SLOT(setState(int, double, double)));
	connect(firstGoComboBox, SIGNAL(activated(int)),
			this, SLOT(setFirstGo()));
	connect(startButton, SIGNAL(clicked()),
			this, SLOT(startButtonClicked()));
}

void ActionArea::
startButtonClicked(){
}

QLabel *ActionArea::
createLabel(const QString &str){
	QLabel *lbl = new QLabel(str);
	lbl->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	return lbl;
}

void ActionArea::
setState(int num, double t, double per){
	stateNumberLabel->setText(QString::number(num));
	timeUseLabel->setText(QString::number(t));
	pertimeLabel->setText(QString::number(per));
}

void ActionArea::
setFirstGo(){
	PlayArea::Player player = PlayArea::Player(firstGoComboBox->itemData(
				firstGoComboBox->currentIndex(), Qt::UserRole).toInt());
	playarea->setFirstMan(player);
}
