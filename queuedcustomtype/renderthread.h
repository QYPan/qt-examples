#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QImage>
#include <QMutex>
#include <QThread>
#include "block.h"

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QObject *parent = 0);
    ~RenderThread();

    void processImage(const QImage &image);

signals:
    void sendBlock(const Block &block);

public slots:
    void stopProcess();

protected:
    void run();

private:
    bool m_abort;
    QImage m_image;
    QMutex mutex;
};

#endif
