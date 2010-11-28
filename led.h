#ifndef LED_H
#define LED_H

#include <QLabel>

class QLed : public QLabel
{
public:
    enum ledColor { red, green};
    QLed(QWidget *parent, ledColor clr);
    void setActive(bool active);
    bool isOn(void) { return isOnVal; }

private:
    bool isOnVal;
    ledColor color;
};


#endif // LED_H
