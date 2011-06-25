#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "qextserialport.h"
#include <QTimer>
#include <QFile>
#include "ui_mainwindow.h"
#include "led.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow,
        private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void startStopComm(void);
    void aboutQt(void);

private slots:
    void config(void);
    void pollSerial(void);
    void helpAbout(void);
    void sendFile(void);
    void saveScreen(void);
    void startLogging(void);
    void endLogging(void);

private:
    bool eventFilter(QObject *obj, QEvent *event);
    QextSerialPort *port;

    int baudRates[8];
    QStringList baudRateStrings;
    QTimer timer;
    void updateStatusBar(void);
    QWidgetList sbList;

    // Settings from config file
    int baudNdx;
    bool hwFlow;
    bool openAtStart;
    QString deviceName;

    QLed *txLed;  // Green left LED
    QLed *rxLed;  // Red right LED
    QFile *logFile;
};

#endif // MAINWINDOW_H
