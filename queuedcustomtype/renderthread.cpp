#include "renderthread.h"

RenderThread::RenderThread(QObject *parent)
    : QThread(parent)
{
    m_abort = false;
}

RenderThread::~RenderThread()
{
    mutex.lock();
    m_abort = true;
    mutex.unlock();

    wait();
}

void RenderThread::processImage(const QImage &image)
{
    if (image.isNull())
        return;

    m_image = image;
    m_abort = false;
    start();
}

void RenderThread::run()
{
    int size = qMax(m_image.width()/20, m_image.height()/20);
    for (int s = size; s > 0; --s) {
        for (int c = 0; c < 400; ++c) {
            int x1 = qMax(0, (qrand() % m_image.width()) - s/2);
            int x2 = qMin(x1 + s/2 + 1, m_image.width());
            int y1 = qMax(0, (qrand() % m_image.height()) - s/2);
            int y2 = qMin(y1 + s/2 + 1, m_image.height());
            int n = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            for (int i = y1; i < y2; ++i) {
                for (int j = x1; j < x2; ++j) {
                    QRgb pixel = m_image.pixel(j, i);
                    red += qRed(pixel);
                    green += qGreen(pixel);
                    blue += qBlue(pixel);
                    n += 1;
                }
            }
            Block block(QRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1),
                        QColor(red/n, green/n, blue/n));
            emit sendBlock(block);
            if (m_abort)
                return;
            sleep(1);
        }
    }
}

void RenderThread::stopProcess()
{
    mutex.lock();
    m_abort = true;
    mutex.unlock();
}
