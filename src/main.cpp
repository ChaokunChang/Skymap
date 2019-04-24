#include "mainwindow.h"
#include <QApplication>
#include "commonhelper.h"

int main(int argc, char *argv[])
{
    QApplication *a = new QApplication(argc, argv);
    CommonHelper::setStyle(":/Data/Data/ElegantDark.qss");
    MainWindow *w = new MainWindow;
    w->show();

    int ret = a->exec();
    delete w;
    delete a;
    return ret;
}
