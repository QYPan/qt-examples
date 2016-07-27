#ifndef AUDIOINPUT_H
 #define AUDIOINPUT_H

 #include <QPixmap>
 #include <QWidget>
 #include <QObject>
 #include <QMainWindow>
 #include <QPushButton>
 #include <QComboBox>
 #include <QByteArray>

 #include <qaudioinput.h>

 class AudioInfo : public QIODevice
 {
     Q_OBJECT
 public:
     AudioInfo(const QAudioFormat &format, QObject *parent);
     ~AudioInfo();

     void start();
     void stop();

     qreal level() const { return m_level; }

     qint64 readData(char *data, qint64 maxlen);
     qint64 writeData(const char *data, qint64 len);

 private:
     const QAudioFormat m_format;
     quint16 m_maxAmplitude;
     qreal m_level; // 0.0 <= m_level <= 1.0

 signals:
     void update();
 };

 class RenderArea : public QWidget
 {
     Q_OBJECT

 public:
     RenderArea(QWidget *parent = 0);

     void setLevel(qreal value);

 protected:
     void paintEvent(QPaintEvent *event);

 private:
     qreal m_level;
     QPixmap m_pixmap;
 };

 class InputTest : public QMainWindow
 {
     Q_OBJECT
 public:
     InputTest();
     ~InputTest();

 private:
     void initializeWindow();
     void initializeAudio();
     void createAudioInput();

 private slots:
     void refreshDisplay();
     void notified();
     void readMore();
     void toggleMode();
     void toggleSuspend();
     void stateChanged(QAudio::State state);
     void deviceChanged(int index);

 private:
     // Owned by layout
     RenderArea *m_canvas;
     QPushButton *m_modeButton;
     QPushButton *m_suspendResumeButton;
     QComboBox *m_deviceBox;

     QAudioDeviceInfo m_device;
     AudioInfo *m_audioInfo;
     QAudioFormat m_format;
     QAudioInput *m_audioInput;
     QIODevice *m_input;
     bool m_pullMode;
     QByteArray m_buffer;

     static const QString PushModeLabel;
     static const QString PullModeLabel;
     static const QString SuspendLabel;
     static const QString ResumeLabel;
 };

 #endif
