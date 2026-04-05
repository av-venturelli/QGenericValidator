#include "mainwindowpanel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowPanel w;
    w.show();
    return QCoreApplication::exec();
}
