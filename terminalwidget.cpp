/*
 * Copyright (C) 2012 Jorge Aparicio <jorge.aparicio.r@gmail.com>
 *           (c) 2017 Olivier Lutzwiller <all-informatic̍̍̍̍̍̍@free.fr>
 *
 * This file is part of qSerialTerm
 *
 * qSerialTerm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.

 * qSerialTerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with qSerialTerm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "terminalwidget.h"
#include "ui_terminalwidget.h"

#include "mainwindow.h"
//#include "qextserialport.h"
//#include "qextserialenumerator.h"
#include <QSerialPort>
#include <QSerialPortInfo>


extern MainWindow* mainInstance;

TerminalWidget::TerminalWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TerminalWidget)
{
  ui->setupUi(this);
}

TerminalWidget::~TerminalWidget()
{
  delete ui;
}

// Grab keypresses meant for edit, send to serial port.
bool TerminalWidget::eventFilter(QObject *obj, QEvent *event)
{
    //!NOTE MainWindow* parent = qobject_cast<MainWindow*>(this->parent()); sucks!
    MainWindow* parent = mainInstance;

     if (parent == 0) {
         return false; // or some other error handling
     }

    //QextSerialPort *port= qobject_cast<QextSerialPort*>(parent->port);
    QSerialPort *port= qobject_cast<QSerialPort*>(parent->port);

    QLed *txLed=parent->txLed;  // Green left LED
    QLed *rxLed=parent->rxLed;  // Red right LED

    if (event->type() == QEvent::KeyPress)
    {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (port)
        {
            QString s = keyEvent->text();
            if (s.length())
            {
                char ch = s.at(0).toLatin1();//.toAscii(); is obsolete
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


void TerminalWidget::pollSerial(QByteArray bytes)
{

        QPlainTextEdit* textEdit=getTerm();

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
}

void TerminalWidget::display(QByteArray const &text)
{
  if (text.length() != 0) {
    ui->terminalTextEdit->moveCursor(QTextCursor::End,
                                     QTextCursor::MoveAnchor);

    ui->terminalTextEdit->insertPlainText(text);
  }
}

void TerminalWidget::on_cleanPushButton_clicked()
{
  ui->terminalTextEdit->clear();
}

void TerminalWidget::enableTerm()
{
  this->show();
  QTimer::singleShot(0, ui->terminalTextEdit, SLOT(setFocus()));
}

void TerminalWidget::disableTerm()
{
  this->hide();
}

QPlainTextEdit* TerminalWidget::getTerm()
{
   return  ui->terminalTextEdit;
}
