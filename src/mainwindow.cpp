#include "mainwindow.h"
#include "ui_mainwindow.h"
#define LOADANI(i) loading[((i)/100)%4]
static QStringList loading={"",".","..","..."};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    pSMM = new SkyMapMatching();
}

MainWindow::~MainWindow()
{
    delete ui;
    terminate();
}
void MainWindow::loadPicture(QString fileName)
{
    this->setAcceptDrops(false);
    this->skyImg = QImage(fileName);
    if(this->skyImg.isNull())
    {
        QMessageBox skyImgNullMsgBox;
        skyImgNullMsgBox.setText(tr("无效的图片！"));
        skyImgNullMsgBox.exec();
    }
    else
    {
        if(fileName.right(3).compare("jpg",Qt::CaseInsensitive)||fileName.right(3).compare("jpeg",Qt::CaseInsensitive))
        {
            const char *photoPath = fileName.toStdString().c_str();
            // Read the JPEG file into a buffer
            FILE *fp = fopen(photoPath, "rb");
            if (!fp) {
                printf("Can't open file.\n");
            }
            fseek(fp, 0, SEEK_END);
            unsigned long fsize = ftell(fp);
            rewind(fp);
            unsigned char *buf = new unsigned char[fsize];easyexif::EXIFInfo result;
            if (fread(buf, 1, fsize, fp) != fsize) {
                printf("Can't read file.\n");
                delete[] buf;
            }
            else
                {
                // Parse EXIF

                int code = result.parseFrom(buf, fsize);
                delete[] buf;
                if (code) {
                    printf("Error parsing EXIF: code %d\n", code);
                }
                this->posX=result.GeoLocation.Longitude;
                this->posY=result.GeoLocation.Latitude;
                this->focus=result.FocalLength;
            }
            fclose(fp);
        }
        else {
            this->posX=0;
            this->posY=0;
            this->focus=DEFAULT_FOCAL_LENGTH;
        }
        if(abs(this->focus)>=EPSINON)
        {
            this->ui->picFocusInput->setText(QString::number(this->focus));
            this->ui->picFocusInput->setReadOnly(true);
        }
        this->ui->picPosXLabelDis->setText(QString::number(this->posX));
        this->ui->picPosYLabelDis->setText(QString::number(this->posY));
        ui->picFocusInput->clear();
        ui->picPosXLabelDis->clear();
        ui->picPosYLabelDis->clear();
        ui->starList->clear();
        ui->pushButton->blockSignals(true);
        ui->statusBar->showMessage(tr("请稍候……正在处理图片"));
        ui->picDisplayArea->setPixmap(QPixmap::fromImage(this->skyImg));
        QElapsedTimer timer;
        timer.start();
        QFuture<vector<StarPoint>> futureIP = QtConcurrent::run(loadStarPoint,fileName);
        while(!futureIP.isFinished())
        {
            ui->statusBar->showMessage(tr("请稍候……正在处理图片")+LOADANI(int(timer.elapsed())));
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
                item->setText(QString::number(this->starRecs[i].index+1)+tr("-坐标：(")+QString::number(this->starRecs[i].x)+tr(",")+QString::number(this->starRecs[i].y)+tr(")"));
                ui->starList->addItem(item);
            }
        }
         ui->pushButton->blockSignals(false);
    }
    ui->starList->blockSignals(true);
    ui->statusBar->showMessage(tr("请稍候……正在加载并处理星表"));
    starNames=loadStarNames(name_path);
    image_properties prop(skyImg.width(),skyImg.height(),skyImg.width()*25.4/skyImg.logicalDpiX(),skyImg.height()*25.4/skyImg.logicalDpiY(),this->focus);
    //this->pSMM->initPara(skyImg.width(),skyImg.height(),skyImg.width()*25.4/skyImg.logicalDpiX(),skyImg.height()*25.4/skyImg.logicalDpiY(),this->focus);
    QElapsedTimer timer;
    timer.start();
    QFuture<vector<StarPoint> > futureSMM = QtConcurrent::run(initStarMapMatching,this->pSMM,prop);
    while(!futureSMM.isFinished())
    {
        QCoreApplication::processEvents();
        ui->statusBar->showMessage(tr("请稍候……正在加载并处理星表")+LOADANI(int(timer.elapsed())));
    }
    this->starMap=futureSMM.result();
    ui->statusBar->clearMessage();
    this->setAcceptDrops(true);
    ui->starList->blockSignals(false);
}


int MainWindow::findMatchingStar(int targetIndex,int algorithm)
{
    this->pSMM->SelectTargetStar(targetIndex);
    this->pSMM->Match(algorithm);
    return this->pSMM->CheckAllCandidates();
}

vector<StarPoint> loadStarPoint(QString fileName)
{
    ImageProcessing IP(fileName.toStdString(),"./tmp.csv");
    vector<StarPoint> starRecs = IP.Process();
    return starRecs;
}

vector<StarPoint> initStarMapMatching(SkyMapMatching* pSMM, image_properties prop)
{
    QString dataset = data_path;
    QString picture = "./tmp.csv";
    vector<StarPoint> ret=pSMM->LoadSky(dataset);
    image_properties property;
    pSMM->LoadImage(picture, prop);
    return ret;
}

double evalStarMapMatching(SkyMapMatching* pSMM,int algortithm,evalArgs arg)
{
    QString dataset = data_path;
    //if(pSMM->sky_.stars_.empty())
    pSMM->LoadSky(dataset);
    return pSMM->ExeSimulation(static_cast<size_t>(algortithm) ,static_cast<size_t>(arg.round),
                               static_cast<size_t>(arg.missing),static_cast<size_t>(arg.redundance),
                               arg.deviation).accuracy;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->simCheckBox->checkState()==Qt::CheckState::Unchecked)
    {
        QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),"",tr("所有文件(*.*);;图片文件(*.jpg *.png *.bmp *.jpeg)"));
        loadPicture(fileName);
    }
    else
    {
        simDialog* sD = new simDialog();
        connect(sD,SIGNAL(sendData(evalArgs)),this,SLOT(receiveData(evalArgs)));
        sD->exec();
    }
}

void MainWindow::on_starList_itemDoubleClicked(QListWidgetItem *item)
{
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(this->skyImg));
    ui->starNoDisplay->clear();
    ui->starNameDisplay->clear();
    ui->starPosXDisplay->clear();
    ui->starPosYDisplay->clear();
    ui->starConsDisplay->clear();
    ui->starDescriptionDisplay->clear();
    this->setAcceptDrops(false);
    double x=item->text().section('(',1,1).section(',',0,0).toDouble();
    double y=item->text().section(',',1,1).section(')',0,0).toDouble();
    this->ui->picDisplayScrollArea->ensureVisible(ceil(x),ceil(y));
    QImage tImg = this->skyImg;
    QPainter painter(&tImg);
    painter.setPen(QPen(QColor(255, 255, 255), 3));
    painter.drawEllipse(QPointF(x,y), 8, 8);
    painter.end();
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(tImg));
    int res;
    QElapsedTimer timer;
    timer.start();
    ui->starList->blockSignals(true);
    ui->statusBar->showMessage(tr("请稍候……正在寻找匹配"));
    QFuture<int> futureFMS = QtConcurrent::run(this,&MainWindow::findMatchingStar,item->text().section('-',0,0).toInt()-1,ui->algorithmComboBox->currentIndex());
    while(!futureFMS.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在寻找匹配")+LOADANI(int(timer.elapsed())));
        timer.restart();
        QApplication::processEvents();
    }
    res=futureFMS.result();
    ui->statusBar->clearMessage();
    if(res>=0)
    {
        ui->starNoDisplay->setText(QString::number(res));
        ui->starNameDisplay->setText(starNames[res]);
        ui->starPosXDisplay->setText(QString::number(starMap[res].x));
        ui->starPosYDisplay->setText(QString::number(starMap[res].y));
        ui->starConsDisplay->setText(QString::number(starMap[res].magnitude));
    }
    else {
        QMessageBox matchingFailMsgBox;
        matchingFailMsgBox.setText(tr("匹配失败！"));
        matchingFailMsgBox.exec();
   }
    ui->starList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->starList->blockSignals(false);

//    ModelEvaluation eval = this->pSMM->ExeSimulation(0,10);
//    cout<<"The Correctness: "<<eval.correctness<<endl;
//    ui->picFocusInput->setText(QString::number(eval.correctness,10,3));
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

void MainWindow::on_picFocusInput_editingFinished()
{
    this->focus=this->ui->picFocusInput->text().toDouble();
}

void MainWindow::receiveData(evalArgs arg)
{
    QString dataset = data_path;
    //if(this->pSMM->sky_.stars_.empty())
    ui->starNoDisplay->setText(ui->algorithmComboBox->currentText());
    ui->starNameDisplay->setText(QString::number(arg.missing));
    ui->starPosXDisplay->setText(QString::number(arg.redundance));
    ui->starPosYDisplay->setText(QString::number(arg.deviation));
    ui->starConsDisplay->setText(QString::number(arg.round));
    double res;
    QElapsedTimer timer;
    timer.start();
    QFuture<double> futureEval = QtConcurrent::run(evalStarMapMatching,this->pSMM,ui->algorithmComboBox->currentIndex()+1,arg);
    while(!futureEval.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在进行仿真")+LOADANI(int(timer.elapsed())));
        QApplication::processEvents();
    }
    res=futureEval.result();
    ui->statusBar->clearMessage();
    ui->starDescriptionDisplay->setText(QString::number(res));
}

void MainWindow::on_simCheckBox_stateChanged(int arg1)
{
    if(arg1==0)
    {
        ui->pushButton->setText(tr("打开图片"));
        ui->algorithmComboBox->insertItem(0,tr("综合"));
        ui->starNoLabel->setText(tr("星编号"));
        ui->starNameLabel->setText(tr("星名"));
        ui->starPosXLabel->setText(tr("赤经"));
        ui->starPosYLabel->setText(tr("赤纬"));
        ui->starConsLabel->setText(tr("亮度"));
        ui->starDescriptionLabel->setText(tr("描述"));
        ui->starNoDisplay->clear();
        ui->starNameDisplay->clear();
        ui->starPosXDisplay->clear();
        ui->starPosYDisplay->clear();
        ui->starConsDisplay->clear();
        ui->starDescriptionDisplay->clear();
    }
    else
    {
        ui->pushButton->setText(tr("开始仿真"));
        ui->algorithmComboBox->removeItem(0);
        ui->starNoLabel->setText(tr("算法"));
        ui->starNameLabel->setText(tr("缺失数"));
        ui->starPosXLabel->setText(tr("冗余数"));
        ui->starPosYLabel->setText(tr("偏移量"));
        ui->starConsLabel->setText(tr("轮数"));
        ui->starDescriptionLabel->setText(tr("准确率"));
        ui->starNoDisplay->clear();
        ui->starNameDisplay->clear();
        ui->starPosXDisplay->clear();
        ui->starPosYDisplay->clear();
        ui->starConsDisplay->clear();
        ui->starDescriptionDisplay->clear();
    }
}

vector<QString> loadStarNames(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        throw "File Not Found!";
    std::vector<QString> starNameList;
    QTextStream in(&file);
    while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            starNameList.push_back("SAO "+fields[1]);
    }
    return starNameList;
}
