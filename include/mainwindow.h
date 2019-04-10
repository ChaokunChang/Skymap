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
#include "ImageProcessing.h"
#include "SkyMapMatching.h"

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

private:
    Ui::MainWindow *ui;
    void loadPicture();
    int findMatchingStar(int);
    vector<pair<double, double>> centroids;
    SkyMapMatching SMM;
};
vector<pair<double, double>> loadStarPoint(QString);
void initStarMapMatching(SkyMapMatching*);
#endif // MAINWINDOW_H
