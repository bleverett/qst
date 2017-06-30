#include "led.h"

#include <QPixmap>

QLed::QLed(QWidget *parent, ledColor clr)
        : QLabel(parent)
{
    color = clr;

    Hled=new HLed();

     if (color == red) {
        Hled->setColor(Qt::red);
     }else
        Hled->setColor(Qt::green);

     Hled->turnOff();
     Hled->setSize(16,16);

     QPixmap pixmap=Hled->grab();
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
        Hled->turnOn();
    else
        Hled->turnOff();

    QPixmap pixmap=Hled->grab();
    setPixmap(pixmap);
}
