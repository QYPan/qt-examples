#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "renderthread.h"

class QLabel;
class QPushButton;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();
    void loadImage(const QImage &image);

public slots:
    void addBlock(const Block &block);

private slots:
    void loadImage();
    void resetUi();

private:
    QLabel *label;
    QPixmap pixmap;
    QPushButton *loadButton;
    QPushButton *resetButton;
    QString path;
    RenderThread *thread;
};

#endif
