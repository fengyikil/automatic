#include "widget.h"
#include <QApplication>
#include <stdio.h>
#include <fifo.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    unsigned short d,b,c;
    d = 3;
    b = 65533;
    c = d -b;

    return a.exec();
}



