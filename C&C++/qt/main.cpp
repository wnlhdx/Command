#include "qtgame.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qtgame w;
    w.show();
    return a.exec();
}
