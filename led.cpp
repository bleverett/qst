#include "led.h"

#include <QPixmap>

QLed::QLed(QWidget *parent, ledColor clr)
        : QLabel(parent)
{
    color = clr;

    QString ledFile;
    if (color == red)
        ledFile = QString(":/img/RedLEDOff.bmp");
    else
        ledFile = QString(":/img/GrnLEDOff.bmp");

    QPixmap pixmap(ledFile);
    setPixmap(pixmap);
}

void QLed::setActive(bool active)
{
    // Turn LED on/off
    if (active == isOnVal)
        return;

    isOnVal = active;
    QString ledFile;
    if (active)
    {
        if (color == red)
            ledFile = QString(":/img/RedLEDOn.bmp");
        else
            ledFile = QString(":/img/GrnLEDOn.bmp");
    }
    else
    {
        if (color == red)
            ledFile = QString(":/img/RedLEDOff.bmp");
        else
            ledFile = QString(":/img/GrnLEDOff.bmp");
    }

    QPixmap pixmap(ledFile);
    setPixmap(pixmap);
}
