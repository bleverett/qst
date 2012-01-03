#include "mainwindow.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "ui_config.h"
#include "ui_about.h"
#include "qextserialenumerator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), port(0), logFile(NULL)
{
    setupUi(this);
    textEdit->installEventFilter(this);

    connect(actionConfig, SIGNAL(triggered()), this, SLOT(config()));
    connect(actionStart_Stop_Comm, SIGNAL(triggered()), this, SLOT(startStopComm()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));
    connect(actionSend, SIGNAL(triggered()), this, SLOT(sendFile()));
    connect(actionSave_Screen, SIGNAL(triggered()), this, SLOT(saveScreen()));
    connect(actionBegin_Logging, SIGNAL(triggered()), this, SLOT(startLogging()));
    connect(actionEnd_Logging, SIGNAL(triggered()), this, SLOT(endLogging()));
    connect(actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));

    baudRates[0] = BAUD300;
    baudRates[1] = BAUD2400;
    baudRates[2] = BAUD4800;
    baudRates[3] = BAUD9600;
    baudRates[4] = BAUD19200;
    baudRates[5] = BAUD38400;
    baudRates[6] = BAUD57600;
    baudRates[7] = BAUD115200;
    baudRateStrings.append("300");
    baudRateStrings.append("2400");
    baudRateStrings.append("4800");
    baudRateStrings.append("9600");
    baudRateStrings.append("19200");
    baudRateStrings.append("38400");
    baudRateStrings.append("57600");
    baudRateStrings.append("115200");

    connect(&timer, SIGNAL(timeout()), this, SLOT(pollSerial()));
    timer.start(100);

    // Get settings from config file (or, god forbid, registry)
    QSettings settings("QTapps","QST");
    baudNdx     = settings.value("baudNdx").toInt();
    hwFlow      = settings.value("hwflow").toBool();
    openAtStart = settings.value("openAtStart").toBool();
    deviceName  = settings.value("device").toString();
    if (openAtStart)
        startStopComm();

    layout()->setSpacing(1);

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Panel);
    frame->setFrameShadow(QFrame::Sunken);
    sbList.append(frame);
    frame->setFixedWidth(33);
    txLed = new QLed(frame, QLed::green);
    txLed->setGeometry(1,1,15,15);
    rxLed = new QLed(frame, QLed::red);
    rxLed->setGeometry(16,1,15,15);

    for (int i=0;i<5;i++)
    {
        // Put a label in each box
        QLabel *statusLbl = new QLabel("Hey");
        statusLbl->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        statusLbl->setFrameShape(QFrame::Panel);
        statusLbl->setFrameShadow(QFrame::Sunken);
        sbList.append(statusLbl);
    }
    verticalLayout->setMargin(1);
    updateStatusBar();
}

// Grab keypresses meant for edit, send to serial port.
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (port)
        {
            QString s = keyEvent->text();
            if (s.length())
            {
                char ch = s.at(0).toAscii();
                port->putChar(ch);
                txLed->setActive(true);
            }
        }
        return true;
    }
    else
        // standard event processing
        return QObject::eventFilter(obj, event);
}

void MainWindow::updateStatusBar(void)
{
    for (int i=0;i<6;i++)
    {
        sbList.at(i)->hide();
        statusBar()->removeWidget(sbList.at(i));
    }

    if (port)
    {
        for (int i=0;i<6;i++)
        {
            statusBar()->addWidget(sbList.at(i), (i == 5));
            sbList.at(i)->show();
        }
        QLabel *lbl;
        lbl = static_cast<QLabel*> (sbList.at(1));
        QString s = port->portName();
        lbl->setText(s);
        lbl = static_cast<QLabel*> (sbList.at(2));
        s = baudRateStrings.at(baudNdx);
        lbl->setText(s);
        lbl = static_cast<QLabel*> (sbList.at(3));
        s = QString("N-8-1");
        lbl->setText(s);
        lbl = static_cast<QLabel*> (sbList.at(4));
        s = QString("No Flow Control");
        lbl->setText(s);
        lbl = static_cast<QLabel*> (sbList.at(5));
        s = QString("UNIX CR/LF");
        lbl->setText(s);
        for (int i=1;i<5;i++)
        {
            QLabel *lbl = static_cast<QLabel*>(sbList.at(i));
            lbl->setFixedWidth(lbl->minimumSizeHint().width());
        }
    }
    else
    {
        statusBar()->addWidget(sbList.at(5),100);
        QString status("Serial port closed");
        QLabel *lbl = static_cast<QLabel *>(sbList.at(5));
        lbl->setText(status);
        lbl->show();
    }
}

void MainWindow::startStopComm(void)
{
    if (port)
    {
        // close it
        delete port;
        port = NULL;
    }
    else
    {
        // Start comm
        QString device;
#ifdef _TTY_POSIX_
        device = QString("/dev/");
#else
        device.clear();
#endif
        device += deviceName;
        port = new QextSerialPort(device, QextSerialPort::Polling);
        port->setBaudRate((BaudRateType)baudRates[baudNdx]);
        port->setDataBits(DATA_8);
        port->setParity(PAR_NONE);
        port->setStopBits(STOP_1);
        port->setFlowControl(hwFlow ? FLOW_HARDWARE : FLOW_OFF);
        port->setTimeout(100);

        if (!port->open(QIODevice::ReadWrite))
        {
            delete port;
            port = NULL;
            QString s("Cannot open port ");
            s += device;
            QMessageBox::critical(this,"Error",s);
        }
#ifdef _TTY_WIN_
        // For win32, you have to re-do this crap after opening.
        port->setBaudRate((BaudRateType)baudRates[baudNdx]);
        port->setDataBits(DATA_8);
        port->setParity(PAR_NONE);
        port->setStopBits(STOP_1);
        port->setFlowControl(flowControl ? FLOW_HARDWARE : FLOW_OFF);
        port->setTimeout(0);
#endif
    }
    // Update status bar
    updateStatusBar();
}

void MainWindow::config(void)
{
    Ui_configDlg dlgUi;
    QDialog config;
    QString selectedDevice;

    dlgUi.setupUi(&config);

    // Setup button group for baud rate settings
    QButtonGroup bg;
    bg.addButton(dlgUi.rb300, 0);
    bg.addButton(dlgUi.rb2400, 1);
    bg.addButton(dlgUi.rb4800, 2);
    bg.addButton(dlgUi.rb9600, 3);
    bg.addButton(dlgUi.rb19200, 4);
    bg.addButton(dlgUi.rb38400, 5);
    bg.addButton(dlgUi.rb57600, 6);
    bg.addButton(dlgUi.rb115200, 7);

    // Load settings
    dlgUi.cbHwFlow->setChecked(hwFlow);
    dlgUi.cbOpenStart->setChecked(openAtStart);
    bg.button(baudNdx)->setChecked(true);

    // Find serial ports available
#ifdef _TTY_POSIX_
    QDir dir("/dev");
    QStringList filterList("ttyS*");
    filterList += QStringList("ttyUSB*");
    filterList += QStringList("ttyACM*");
    dir.setNameFilters(filterList);
    QStringList devices = dir.entryList(QDir::System);
#elif defined(_TTY_WIN_)
    QList< QextPortInfo > list = QextSerialEnumerator::getPorts();
    QStringList devices;
    for (int i=0;i<list.count();i++)
    {
        QString s = list.at(i).portName;
        char t[20];
        for(int j=0;j<s.length();j++)
        {
            if (!isprint((int)s.at(j).toAscii()))
            {
                s.resize(j);
                break;
            }
            else
                t[j] = s.at(j).toAscii();
        }
        devices.append(s);
    }
#endif

    for(int i=0;i<devices.count();i++)
    {
        if (!devices.at(i).contains("ttys"))
            dlgUi.listPorts->addItem(devices.at(i));
        if (devices.at(i) == deviceName)
            selectedDevice = devices.at(i);
    }
    if (!selectedDevice.isEmpty())
    {
        for (int i=0;i<dlgUi.listPorts->count();i++)
            if (selectedDevice == dlgUi.listPorts->item(i)->text())
                dlgUi.listPorts->setCurrentRow(i);
    }

    config.exec();

    if (config.result() == QDialog::Accepted)
    {
        // Save settings
        QSettings settings("QTapps","QST");
        settings.setValue("hwflow", dlgUi.cbHwFlow->isChecked());
        settings.setValue("openAtStart", dlgUi.cbOpenStart->isChecked());
        settings.setValue("baudNdx", bg.checkedId());
        settings.setValue("device", dlgUi.listPorts->currentItem()->text());
        // Open serial port
        if (port)
        {
            delete port;
            port = NULL;
        }

        hwFlow = dlgUi.cbHwFlow->isChecked();
        baudNdx = bg.checkedId();
        deviceName = dlgUi.listPorts->currentItem()->text();

        startStopComm();
    }
}

void MainWindow::pollSerial(void)
{
    // Check for serial chars

    rxLed->setActive(false);
    txLed->setActive(false);

    if (!port) return;
    
    QByteArray bytes = port->readAll();

    if (bytes.isEmpty()) return;
    if (logFile)
    {
        logFile->write(bytes);
        logFile->flush();
    }
    bytes.replace("\r", "");
    if (bytes.contains(8))
    {
        // Must parse backspace commands manually
        for (int i=0;i<bytes.count();i++)
        {
            char ch = bytes.at(i);
            if (ch == 8)
            {
                // Backspace
                QString s = textEdit->toPlainText();
                s.chop(1);
                textEdit->setPlainText(s);
            }
            else
            {
                // Add char to edit
                QString s(ch);
                textEdit->insertPlainText(s);
            }
            textEdit->moveCursor(QTextCursor::End);
        }
    }
    else
    {
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(bytes);
    }
    textEdit->ensureCursorVisible();
    rxLed->setActive(true);
}

void MainWindow::sendFile(void)
{
    if (!port) return;

    QString name = QFileDialog::getOpenFileName(this,
                                                "Open File", QDir::currentPath(), "All Files (*)");
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly))
    {
        QString s("Cannot read file ");
        s += name;
        QMessageBox::critical(this,"File read error", s);
        return;
    }

    QByteArray bytes = file.readAll();
    port->write(bytes);
    file.close();
}

void MainWindow::helpAbout(void)
{
    QDialog dlg;
    Ui_aboutDialog dlg_ui;
    dlg_ui.setupUi(&dlg);

    dlg.exec();
}

void MainWindow::saveScreen(void)
{
    QString name = QFileDialog::getSaveFileName(this,
                                                "Save Screen", QDir::currentPath(), "All Files (*)");
    QFile file(name);
    if (!file.open(QIODevice::WriteOnly))
    {
        QString s("Cannot write file ");
        s += name;
        QMessageBox::critical(this,"File read error", s);
        return;
    }

    file.write(textEdit->toPlainText().toAscii());

    file.close();
}

void MainWindow::startLogging(void)
{
    if (logFile)
    {
        QMessageBox::critical(this, "Error", "Logging already active");
        return;
    }

    QString name = QFileDialog::getSaveFileName(this,
                                                "Select log file", QDir::currentPath(), "All Files (*)");
    if (name.length() == 0)
        return;
    logFile = new QFile(name);
    if (!logFile->open(QIODevice::WriteOnly))
    {
        QString s("Cannot write file ");
        s += name;
        QMessageBox::critical(this,"File read error", s);
        logFile = NULL;
        return;
    }
}

void MainWindow::endLogging(void)
{
    if (logFile)
    {
        logFile->close();
        delete logFile;
        logFile = NULL;
    }
}

void MainWindow::aboutQt(void)
{
    QMessageBox::aboutQt(this, "About Qt");
}
