#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QFile>
#include "ui_mainwindow.h"
#include "led.h"

#include <QPlainTextEdit>
#include "terminalwidget.h"

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
    QSerialPort *port;

    QLed *txLed;  // Green left LED
    QLed *rxLed;  // Red right LED
    static MainWindow* GetInstance(void);//QWidget* parent = 0);

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

    void on_actionTerminal_triggered();

private:
    //bool eventFilter(QObject *obj, QEvent *event);
    //QSerialPort *port;

    QSerialPort::BaudRate baudRates[8];
    QStringList baudRateStrings;
    QTimer timer;
    void updateStatusBar(void);
    QWidgetList sbList;

    // Settings from config file
    uint baudNdx;
    bool hwFlow;
    bool openAtStart;
    QString deviceName;

    // TerminalWidget
    //Ui::MainWindow *ui;
    TerminalWidget *terminalWidget;
    //QPlainTextEdit *textEdit;
    //QLed *txLed;  // Green left LED
    //QLed *rxLed;  // Red right LED
    QFile *logFile;
};

#endif // MAINWINDOW_H
