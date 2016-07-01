#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

class QLineEdit;
class Button;

class Calculator : public QWidget{
	Q_OBJECT
public:
	Calculator(QWidget *parent = 0);
private slots:
	void digitClicked();
	void unaryOperatorClicked(); //点击非四则运算
	void additiveOperatorClicked(); //点击加减运算
	void multiplicativeOperatorClicked(); //点击乘除运算
	void equalClicked();
	void pointClicked();
	void changeSignClicked();
	void backspaceClicked();
	void clear();
	void clearAll();
	void clearMemory();
	void readMemory();
	void setMemory();
	void addToMemory();
private:
	Button *createButton(const QString &text, const char *member);
	void abortOperation();
	bool calculate(double rightOperand, const QString &pendingOperator);

	double sumInMemory; //在内存中的值 (MS, M+, or MC)
	double sumSoFar; //当前已经计算的值，当等号被点击时，此值被重新计算并显示在屏幕上，ClearAll 把此值重置为 0
	double factorSoFar; //做乘除法时保存运算符左值
	QString pendingAdditiveOperator; //当前最新的加减操作符
	QString pendingMultiplicativeOperator; //当前最新的乘除操作符
	bool waitingForOperand; //是否在等待一个输入
	QLineEdit *display; //显示框
	enum{NumDigitButtons = 10};
	Button *digitButtons[NumDigitButtons];
};

#endif
