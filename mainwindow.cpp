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
void MainWindow::loadPicture()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),"",tr("所有文件(*.*);;图片文件(*.jpg *.png *.bmp)"));
    QImage skyImg = QImage(fileName);
    if(skyImg.isNull())
    {
        QMessageBox skyImgNullMsgBox;
        skyImgNullMsgBox.setText(tr("无效的图片！"));
        skyImgNullMsgBox.exec();
    }
    else
    {
        ui->statusBar->showMessage(tr("请稍候……正在处理图片"));
        ui->picDisplayArea->setPixmap(QPixmap::fromImage(skyImg));
        QFuture<vector<pair<double, double>>> futureIP = QtConcurrent::run(loadStarPoint,fileName);
        while(!futureIP.isFinished())
        {
            QCoreApplication::processEvents();
        }
        this->centroids = futureIP.result();
        if(this->centroids.empty())
        {
            QMessageBox unableFindStarMsgBox;
            unableFindStarMsgBox.setText(tr("无法找到星点！"));
            unableFindStarMsgBox.exec();
        }
        else {
            for(size_t i=0;i!=this->centroids.size();i++)
            {
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(QString::number(i+1)+tr("-坐标：（")+QString::number(this->centroids[i].first)+tr("，")+QString::number(this->centroids[i].second)+tr("）"));
                ui->starList->addItem(item);
            }
        }

    }
    ui->statusBar->showMessage(tr("请稍候……正在加载星表"));
    QFuture<void> futureSMM = QtConcurrent::run(initStarMapMatching,&this->SMM);
    while(!futureSMM.isFinished())
    {
        QCoreApplication::processEvents();
    }
    ui->statusBar->clearMessage();
}


int MainWindow::findMatchingStar(int targetIndex)
{
    this->SMM.SelectTargetStar(targetIndex);
    this->SMM.Match();
    return this->SMM.Check();
}

vector<pair<double, double>> loadStarPoint(QString fileName)
{
    ImageProcessing IP(fileName.toStdString(),"tmp.csv");
    vector<pair<double, double>> centroids = IP.Process();
    return centroids;
}

void initStarMapMatching(SkyMapMatching* pSMM)
{
    string dataset = "skymaps.csv";
    string picture = "tmp.csv";
    pSMM->LoadSky(dataset);
    pSMM->LoadImage(picture);
}

void MainWindow::on_pushButton_clicked()
{
    loadPicture();
}

void MainWindow::on_starList_itemDoubleClicked(QListWidgetItem *item)
{
    int res;
    ui->statusBar->showMessage(tr("请稍候……正在寻找匹配"));
    QFuture<int> futureFMS = QtConcurrent::run(this,&MainWindow::findMatchingStar,item->text().section('-',0,0).toInt()-1);
    while(!futureFMS.isFinished())
    {
        QApplication::processEvents();
    }
    res=futureFMS.result();
    ui->statusBar->clearMessage();
    if(res>=0)
    {
        ui->starNoDisplay->setText(QString::number(res));
    }
    else {
        QMessageBox matchingFailMsgBox;
        matchingFailMsgBox.setText(tr("匹配失败！"));
        matchingFailMsgBox.exec();
   }
}
