#include <QtGui>
#include "window.h"

Window::Window()
{
    thread = new RenderThread();

    label = new QLabel();
    label->setAlignment(Qt::AlignCenter);

    loadButton = new QPushButton(tr("&Load image..."));
    resetButton = new QPushButton(tr("&Stop"));
    resetButton->setEnabled(false);

    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(resetButton, SIGNAL(clicked()), thread, SLOT(stopProcess()));
    connect(thread, SIGNAL(finished()), this, SLOT(resetUi()));
    connect(thread, SIGNAL(sendBlock(Block)), this, SLOT(addBlock(Block)));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addLayout(buttonLayout);

    setWindowTitle(tr("Queued Custom Type"));
}

void Window::loadImage()
{
    QStringList formats;
    foreach (QByteArray format, QImageReader::supportedImageFormats())
        if (format.toLower() == format)
            formats.append("*." + format);

    QString newPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
        path, tr("Image files (%1)").arg(formats.join(" ")));

    if (newPath.isEmpty())
        return;

    QImage image(newPath);
    if (!image.isNull()) {
        loadImage(image);
        path = newPath;
    }
}

void Window::loadImage(const QImage &image)
{
    QDesktopWidget desktop;
    QImage useImage;
    QRect space = desktop.availableGeometry();
    if (image.width() > 0.75*space.width() || image.height() > 0.75*space.height())
        useImage = image.scaled(0.75*space.width(), 0.75*space.height(),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
    else
        useImage = image;

    pixmap = QPixmap(useImage.width(), useImage.height());
    pixmap.fill(qRgb(255, 255, 255));
    label->setPixmap(pixmap);
    loadButton->setEnabled(false);
    resetButton->setEnabled(true);
    thread->processImage(useImage);
}

void Window::addBlock(const Block &block)
{
    QColor color = block.color();
    color.setAlpha(64);

    QPainter painter;
    painter.begin(&pixmap);
    painter.fillRect(block.rect(), color);
    painter.end();
    label->setPixmap(pixmap);
}

void Window::resetUi()
{
    loadButton->setEnabled(true);
    resetButton->setEnabled(false);
}
