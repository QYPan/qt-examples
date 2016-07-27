#include <QtGui>

#include <stdio.h>
#include <stdlib.h>

#ifdef Q_WS_S60
const int DataSize = 300;
#else
const int DataSize = 100000;
#endif

const int BufferSize = 8192;
char buffer[BufferSize];

QWaitCondition bufferNotEmpty;
QWaitCondition bufferNotFull;
QMutex mutex;
int numUsedBytes = 0;

class Producer : public QThread
{
public:
    Producer(QObject *parent = NULL) : QThread(parent)
    {
    }

    void run()
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

        for (int i = 0; i < DataSize; ++i) {
            mutex.lock();
            if (numUsedBytes == BufferSize)
                bufferNotFull.wait(&mutex);
            mutex.unlock();

            buffer[i % BufferSize] = "ACGT"[(int)qrand() % 4];

            mutex.lock();
            ++numUsedBytes;
            bufferNotEmpty.wakeAll();
            mutex.unlock();
        }
    }
};

class Consumer : public QThread
{
    Q_OBJECT
public:
    Consumer(QObject *parent = NULL) : QThread(parent)
    {
    }

    void run()
    {
        for (int i = 0; i < DataSize; ++i) {
            mutex.lock();
            if (numUsedBytes == 0)
                bufferNotEmpty.wait(&mutex);
            mutex.unlock();

    #ifdef Q_WS_S60
            emit stringConsumed(QString(buffer[i % BufferSize]));
    #else
            fprintf(stderr, "%c", buffer[i % BufferSize]);
    #endif

            mutex.lock();
            --numUsedBytes;
            bufferNotFull.wakeAll();
            mutex.unlock();
        }
        fprintf(stderr, "\n");
    }

signals:
    void stringConsumed(const QString &text);
};

#ifdef Q_WS_S60
class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    PlainTextEdit(QWidget *parent = NULL) : QPlainTextEdit(parent), producer(NULL), consumer(NULL)
    {
        setTextInteractionFlags(Qt::NoTextInteraction);

        producer = new Producer(this);
        consumer = new Consumer(this);

        QObject::connect(consumer, SIGNAL(stringConsumed(const QString &)), SLOT(insertPlainText(const QString &)), Qt::BlockingQueuedConnection);

        QTimer::singleShot(0, this, SLOT(startThreads()));
    }

protected:
    Producer *producer;
    Consumer *consumer;

protected slots:
    void startThreads()
    {
        producer->start();
        consumer->start();
    }
};
#endif

int main(int argc, char *argv[])
{
#ifdef Q_WS_S60
    QApplication app(argc, argv);

    PlainTextEdit console;
    console.showMaximized();

    return app.exec();
#else
    QCoreApplication app(argc, argv);
    Producer producer;
    Consumer consumer;
    producer.start();
    consumer.start();
    producer.wait();
    consumer.wait();
    return 0;
#endif
}

#include "waitconditions.moc"
