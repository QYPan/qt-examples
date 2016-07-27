#include <QCoreApplication>
#include <QVariant>
#include "message.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList headers;
    headers << "Subject: Hello World"
            << "From: qt-info@nokia.com";
    QString body = "This is a test.\r\n";

    Message message(body, headers);
    qDebug() << "Original:" << message;

    QVariant stored;
    stored.setValue(message);

    qDebug() << "Stored:" << stored;

    Message retrieved = stored.value<Message>();
    qDebug() << "Retrieved:" << retrieved;
    retrieved = qvariant_cast<Message>(stored);
    qDebug() << "Retrieved:" << retrieved;

    return 0;
}
