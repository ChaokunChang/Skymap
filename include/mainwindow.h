#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <QDrag>
#include <QDropEvent>
#include <QPainter>
#include <QAbstractItemView>
#include "ImageProcessing.h"
#include "SkyMapMatching.h"
#include "exif.h"
#define EPSINON 1e-6
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_starList_itemDoubleClicked(QListWidgetItem*);
    void on_pushButton_clicked();

    void on_picFocusInput_editingFinished();

protected:
    void dragEnterEvent(QDragEnterEvent*event);//拖动进入事件
    void dropEvent(QDropEvent*event);

private:
    Ui::MainWindow *ui;
    void loadPicture(QString);
    int findMatchingStar(int);
    vector<StarPoint> starRecs;
    SkyMapMatching SMM;
    QImage skyImg;
    double posX,posY,focus;
};
vector<StarPoint> loadStarPoint(QString);
void initStarMapMatching(SkyMapMatching*,image_properties prop);
#endif // MAINWINDOW_H
