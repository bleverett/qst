#include <QApplication>
#include "mainwindow.h"

MainWindow* mainInstance;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    mainInstance=&w;
    w.show();
    return a.exec();
}
