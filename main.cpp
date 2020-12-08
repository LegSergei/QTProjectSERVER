#include "QtSERVER.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtSERVER w;
    w.show();
    return a.exec();
}
