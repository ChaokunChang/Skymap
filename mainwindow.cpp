#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::loadPicture()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),"",tr("所有文件(*.*);;图片文件(*.jpg *.png *.bmp)"));
    QImage skyImg = QImage(fileName);
    if(skyImg.isNull())
    {
        QMessageBox skyImgNullMsgBox;
        skyImgNullMsgBox.setText(tr("无效的图片！"));
        skyImgNullMsgBox.exec();
    }
    else {
        ui->picDisplayArea->setPixmap(QPixmap::fromImage(skyImg));
    }
    return skyImg;
}

void MainWindow::on_pushButton_clicked()
{
    loadPicture();
}
