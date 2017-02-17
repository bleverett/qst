#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "mainwindow.h"
#include "ui_config.h"
#include "ui_about.h"

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

    baudRates[0] = QSerialPort::Baud9600;
    baudRates[1] = QSerialPort::Baud19200;
    baudRates[2] = QSerialPort::Baud38400;
    baudRates[3] = QSerialPort::Baud57600;
    baudRates[4] = QSerialPort::Baud115200;

    baudRateStrings.append("9600");
    baudRateStrings.append("19200");
    baudRateStrings.append("38400");
    baudRateStrings.append("57600");
    baudRateStrings.append("115200");
    baudRateStrings.append("230400");
    baudRateStrings.append("460800");
    baudRateStrings.append("921600");

    connect(&timer, SIGNAL(timeout()), this, SLOT(pollSerial()));
    timer.start(100);

    // Get settings from config file (or, god forbid, registry)
    QSettings settings("QTapps","QST");
    baudNdx     = settings.value("baudNdx").toInt();
    hwFlow      = settings.value("hwflow").toBool();
    openAtStart = settings.value("openAtStart").toBool();
    deviceName  = settings.value("device").toString();

    uint brcount = sizeof(baudRates)/sizeof(uint);
    if (baudNdx >= brcount)
        baudNdx = 0;


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
    if (openAtStart)
        startStopComm();
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
                char ch = s.at(0).toLatin1();
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
    if(sbList.isEmpty())return;
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
#endif
        device += deviceName;
        QSerialPortInfo pinfo;
        port = new QSerialPort(device, this);
        port->setBaudRate(baudRates[baudNdx]);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(hwFlow ? QSerialPort::HardwareControl : QSerialPort::NoFlowControl);

        if (!port->open(QIODevice::ReadWrite))
        {
            delete port;
            port = NULL;
            QString s("Cannot open port ");
            s += device;
            QMessageBox::critical(this,"Error",s);
        }
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
    bg.addButton(dlgUi.rb9600, 0);
    bg.addButton(dlgUi.rb19200, 1);
    bg.addButton(dlgUi.rb38400, 2);
    bg.addButton(dlgUi.rb57600, 3);
    bg.addButton(dlgUi.rb115200, 4);

    // Load settings
    dlgUi.cbHwFlow->setChecked(hwFlow);
    dlgUi.cbOpenStart->setChecked(openAtStart);
    if (bg.buttons().count() > (int)baudNdx)
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
    QList< QSerialPortInfo > list = QSerialPortInfo::availablePorts();
    QStringList devices;
    for (int i=0;i<list.count();i++)
    {
        QString s = list.at(i).portName();
        for(int j=0;j<s.length();j++)
        {
            if (!isprint((int)s.at(j).toLatin1()))
            {
                s.resize(j);
                break;
            }
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
        if(dlgUi.listPorts->selectedItems().count()!=1)
            dlgUi.listPorts->setCurrentRow(0);
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
//    if (bytes.contains(8))
//    {
//        // Must parse backspace commands manually
//        for (int i=0;i<bytes.count();i++)
//        {
//            char ch = bytes.at(i);
//            if (ch == 8)
//            {
//                // Backspace
//                QString s = textEdit->toPlainText();
//                s.chop(1);
//                textEdit->setPlainText(s);
//            }
//            else
//            {
//                // Add char to edit
//                QString s(ch);
//                textEdit->insertPlainText(s);
//            }
//            textEdit->moveCursor(QTextCursor::End);
//        }
//    }
//    else
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

    file.write(textEdit->toPlainText().toLatin1());

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
