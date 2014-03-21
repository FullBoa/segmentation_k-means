#include <QtGui/QApplication>
#include <QDateTime>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qsrand(QDateTime::currentDateTime().time().msec());
    MainWindow w;
    w.show();
    
    return a.exec();
}
