#include "mainwindow.h"
#include "ui_mainwindow.h"
#define LOADANI(i) loading[((i)/100)%4]
static QStringList loading={"",".","..","..."};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    terminate();
}
void MainWindow::loadPicture(QString fileName)
{
    this->setAcceptDrops(false);
    QImage skyImg = QImage(fileName);
    if(skyImg.isNull())
    {
        QMessageBox skyImgNullMsgBox;
        skyImgNullMsgBox.setText(tr("无效的图片！"));
        skyImgNullMsgBox.exec();
    }
    else
    {
        ui->starList->clear();
        ui->statusBar->showMessage(tr("请稍候……正在处理图片"));
        ui->picDisplayArea->setPixmap(QPixmap::fromImage(skyImg));
        QElapsedTimer timer;
        timer.start();
        QFuture<vector<StarPoint>> futureIP = QtConcurrent::run(loadStarPoint,fileName);
        while(!futureIP.isFinished())
        {
            ui->statusBar->showMessage(tr("请稍候……正在处理图片")+LOADANI((int)timer.elapsed()));
            QCoreApplication::processEvents();
        }
        this->starRecs = futureIP.result();
        if(this->starRecs.empty())
        {
            QMessageBox unableFindStarMsgBox;
            unableFindStarMsgBox.setText(tr("无法找到星点！"));
            unableFindStarMsgBox.exec();
        }
        else {
            for(size_t i=0;i!=this->starRecs.size();i++)
            {
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(QString::number(this->starRecs[i].index+1)+tr("-坐标：（")+QString::number(this->starRecs[i].x)+tr("，")+QString::number(this->starRecs[i].y)+tr("）"));
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
    this->setAcceptDrops(true);
}


int MainWindow::findMatchingStar(int targetIndex)
{
    this->SMM.SelectTargetStar(targetIndex);
    this->SMM.Match();
    return this->SMM.Check();
}

vector<StarPoint> loadStarPoint(QString fileName)
{
    ImageProcessing IP(fileName.toStdString(),"./tmp.csv");
    vector<StarPoint> starRecs = IP.Process();
    return starRecs;
}

void initStarMapMatching(SkyMapMatching* pSMM)
{
    QString dataset = ":/Data/Data/skymaps.csv";
    QString picture = "./tmp.csv";
    pSMM->LoadSky(dataset);
    pSMM->LoadImage(picture);
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),"",tr("所有文件(*.*);;图片文件(*.jpg *.png *.bmp *.jpeg)"));
    loadPicture(fileName);
}

void MainWindow::on_starList_itemDoubleClicked(QListWidgetItem *item)
{
    this->setAcceptDrops(false);
    int res;
    QElapsedTimer timer;
    timer.start();
    ui->statusBar->showMessage(tr("请稍候……正在寻找匹配"));
    QFuture<int> futureFMS = QtConcurrent::run(this,&MainWindow::findMatchingStar,item->text().section('-',0,0).toInt()-1);
    while(!futureFMS.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在寻找匹配")+LOADANI((int)timer.elapsed()));
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
    this->setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if(!event->mimeData()->urls()[0].toLocalFile().right(3).compare("jpg",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(3).compare("png",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(3).compare("bmp",Qt::CaseInsensitive)
        ||!event->mimeData()->urls()[0].toLocalFile().right(4).compare("jpeg",Qt::CaseInsensitive))
    {
        event->acceptProposedAction();//接受鼠标拖入事件
    }
    else
    {
        event->ignore();//否则不接受鼠标事件
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    //窗口部件放下一个对象时,调用该函数
    const QMimeData *qm=event->mimeData();//获取MIMEData
    this->loadPicture(qm->urls()[0].toLocalFile());//使用图片函数
}
