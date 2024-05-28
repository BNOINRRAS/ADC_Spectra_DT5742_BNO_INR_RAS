#include "mainwindow.h"
#include <iostream>

#include <CAENDigitizer.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1280 + 200, 720 + 40);
    w.show();
    return a.exec();
}
