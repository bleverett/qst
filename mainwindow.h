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
#include "customplotzoomwidget.h"

//Extends
class BaudrateExt { //: public QSerialPort {

public:
    enum Baudrate
    {
        Baud9600=9600,
        Baud19200=19200,
        Baud38400=38400,
        Baud57600=57600,
        Baud115200=115200,
        Baud230400=230400,
        Baud460800=460800,
        Baud921600=921600,
        UnknownBaud = -1
    };
};


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

    //TextEdit specific
    void clear(void);
    void select(void);
    void copy(void);
    void paste(void);

    void close(void);

    void view(QAction *action);

private:
    //bool eventFilter(QObject *obj, QEvent *event);
    //QSerialPort *port;

    BaudrateExt::Baudrate baudRates[8]; //QSerialPort::Baudrate baudRates[8]
    QStringList baudRateStrings;
    QTimer timer;
    void updateStatusBar(void);
    QWidgetList sbList;

    // Settings from config file
    uint baudNdx;                                     // baud rate index matching flat combo box
    bool hwFlow;
    bool openAtStart;
    int dataBitsIndex;                                // index of data bits combo box
    int parityIndex;                                  // index of parity combo box
    int stopBitsIndex;                                // index of stop bits combo box

    QString deviceName;
    QActionGroup *fromDeviceActionGroup;


    // TerminalWidget
    //Ui::MainWindow *ui;
    TerminalWidget *terminalWidget;
    //QPlainTextEdit *textEdit;
    //QLed *txLed;  // Green left LED
    //QLed *rxLed;  // Red right LED
    QFile *logFile;


    CustomPlotZoomWidget *customPlotZoomWidget;

};

#endif // MAINWINDOW_H
