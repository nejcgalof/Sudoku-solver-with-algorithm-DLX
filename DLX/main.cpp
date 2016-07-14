#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QApplication>
#include <QtCore>
#include <QtWidgets>

/*
ctrl+e,r
ctrl+e,ctrl+v
ctrl+i
*/
static int polje[9][9];
int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    MainWindow window;
    window.show();
    return a.exec();
}
