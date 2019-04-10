#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication *a = new QApplication(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();

    int ret = a->exec();
    delete w;
    delete a;
    return ret;
}
