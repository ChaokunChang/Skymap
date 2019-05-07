#include "mainwindow.h"
#include "ui_mainwindow.h"
#define LOADANI(i) loading[((i)/100)%4]
#define simDev 5
static QStringList loading={"",".","..","..."};
static QStringList AN={"三角匹配","无标定参数","径向环向特征","Log-Polar"};
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    qApp->setWindowIcon(QIcon(":/Data/Data/skymap_logo.ico"));

    QStringList starPointTableHeader;
    starPointTableHeader<<tr("编号")<<tr("X")<<tr("Y");
    ui->starPointTable->setHorizontalHeaderLabels(starPointTableHeader);
    QStringList simStarPointTableHeader;
    simStarPointTableHeader<<tr("星表编号")<<tr("X")<<tr("Y")<<tr("识别结果");
    ui->simStarPointTable->setHorizontalHeaderLabels(simStarPointTableHeader);
    pSMM = new SkyMapMatching();
    connect(ui->open,SIGNAL(triggered()),this,SLOT(on_openButton_clicked()));
    connect(ui->quit,SIGNAL(triggered()),qApp,SLOT(quit()));
    connect(ui->about,SIGNAL(triggered()),this,SLOT(showAboutDialog()));
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
        skyImgNullMsgBox.setWindowTitle(tr("错误"));
        skyImgNullMsgBox.setText(tr("无效的图片！"));
        skyImgNullMsgBox.exec();
    }
    else
    {
        ui->focalLengthInput->clear();
        ui->posXInput->clear();
        ui->posYInput->clear();
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
            this->ui->focalLengthInput->setText(QString::number(this->focus));
            this->ui->focalLengthInput->setReadOnly(true);
        }
        this->ui->posXInput->setText(QString::number(this->posX));
        this->ui->posYInput->setText(QString::number(this->posY));
        ui->posXInput->setReadOnly(true);
        ui->posYInput->setReadOnly(true);
        ui->starPointTable->setRowCount(0);
        ui->openButton->blockSignals(true);
        ui->evalButton->blockSignals(true);
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
            unableFindStarMsgBox.setWindowTitle(tr("错误"));
            unableFindStarMsgBox.setText(tr("无法找到星点！"));
            unableFindStarMsgBox.exec();
        }
        else {
            for(size_t i=0;i!=this->starRecs.size();i++)
            {
                int row_count=ui->starPointTable->rowCount();
                ui->starPointTable->insertRow(row_count);
                QTableWidgetItem *num_item=new QTableWidgetItem();
                num_item->setText(QString::number(this->starRecs[i].index+1));
                ui->starPointTable->setItem(row_count,0,num_item);
                QTableWidgetItem *x_item=new QTableWidgetItem();
                x_item->setText(QString::number(this->starRecs[i].x));
                ui->starPointTable->setItem(row_count,1,x_item);
                QTableWidgetItem *y_item=new QTableWidgetItem();
                y_item->setText(QString::number(this->starRecs[i].y));
                ui->starPointTable->setItem(row_count,2,y_item);
            }
        }
         ui->openButton->blockSignals(false);
    }
    ui->starPointTable->blockSignals(true);
    ImageProperties prop(skyImg.width(),skyImg.height(),skyImg.logicalDpiX(),this->focus);
    ui->statusBar->showMessage(tr("请稍候……正在加载并处理星表"));
    starNames=loadStarNames(name_path);
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
    ui->starPointTable->blockSignals(false);
    ui->evalButton->blockSignals(true);
    ui->tabWidget->setCurrentIndex(0);
}


int MainWindow::findMatchingStar(int targetIndex)
{
    this->pSMM->SelectTargetStar(targetIndex);
    getAlgorithm();
    this->pSMM->Match(algorithm);
    return this->pSMM->CheckAllCandidates();
}

vector<StarPoint> loadStarPoint(QString fileName)
{
    ImageProcessing IP(fileName.toStdString(),"./tmp.csv");
    vector<StarPoint> starRecs = IP.Process();
    return starRecs;
}

vector<StarPoint> initStarMapMatching(SkyMapMatching* pSMM, ImageProperties prop)
{
    /**
     * @brief logic
     * Maybe the logic can be splited here.
     * In this function we only initilize the skymap, without the sky_image.
     * Each time we want to load a new image, just load image only.
     * (The current logic will cause that each time we load image, the skymap is loaded and propessing again,
     * which is needless.)
     */
    QString dataset = data_path;
    if(pSMM->sky_.stars_.empty()) pSMM->LoadSky(dataset);

    QString picture = "./tmp.csv";
    pSMM->LoadImage(picture, prop);

    return pSMM->sky_.stars_;
}

double MainWindow::evalStarMapMatching(EvalArgs arg)
{
    QString dataset = data_path;
    if(pSMM->sky_.stars_.empty()) pSMM->LoadSky(dataset);
    return pSMM->ExeEvaluation(algorithm,static_cast<size_t>(arg.round),
                               static_cast<size_t>(arg.missing),static_cast<size_t>(arg.redundance),
                               arg.deviation).accuracy;
}

void MainWindow::simStarMapMatching(GeneratedImage gi)
{
    if(gi.stars_.empty())
    {
        return;
    }
    vector<StarPoint> found=loadStarPoint(QString::fromStdString(gi.image_path_));
    QString picture = "./tmp.csv";
    pSMM->LoadImage(picture, gi.propery_);
    int false_pos=0,error_num=0,suc_num=0,total_num=gi.stars_.size(),found_num=found.size();
    for(vector<StarPoint>::iterator it=found.begin();it!=found.end();it++)
    {
        int ans=findMatchingStar(it->index),true_ans=0;
        bool endFlag=true;
        for(vector<StarPoint>::iterator cit=gi.stars_.begin();cit!=gi.stars_.end();cit++)
        {
            if(abs(cit->x-it->x)+abs(cit->y-it->y)<=simDev)
            {
                if(ans!=cit->index)
                {
                    error_num++;
                }
                else {
                    suc_num++;
                }
                true_ans=cit->index;
                endFlag=false;
                break;
            }
        }
        if(endFlag)
        {
            false_pos++;
        }
        int row_count=ui->simStarPointTable->rowCount();
        ui->simStarPointTable->insertRow(row_count);
        QTableWidgetItem *true_ans_item=new QTableWidgetItem();
        true_ans_item->setText(QString::number(true_ans));
        ui->simStarPointTable->setItem(row_count,0,true_ans_item);
        QTableWidgetItem *x_item=new QTableWidgetItem();
        x_item->setText(QString::number(it->x));
        ui->simStarPointTable->setItem(row_count,1,x_item);
        QTableWidgetItem *y_item=new QTableWidgetItem();
        y_item->setText(QString::number(it->y));
        ui->simStarPointTable->setItem(row_count,2,y_item);
        QTableWidgetItem *ans_item=new QTableWidgetItem();
        ans_item->setText(QString::number(ans));
        ui->simStarPointTable->setItem(row_count,3,ans_item);
    }
    sr={total_num,found_num,false_pos,error_num,suc_num*1.0/total_num};
}

void MainWindow::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),"",tr("所有文件(*.*);;图片文件(*.jpg *.png *.bmp *.jpeg)"));
    loadPicture(fileName);
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

void MainWindow::on_focalLengthInput_editingFinished()
{
    this->focus=this->ui->focalLengthInput->text().toDouble();
}

void MainWindow::on_evalButton_clicked()
{
    QString dataset = data_path;
    EvalArgs arg={ui->roundInput->text().toInt(),ui->missingInput->text().toInt(),ui->redundanceInput->text().toInt(),ui->deviationInput->text().toDouble()};
    getAlgorithm();
    ui->starNoDisplay->setText(algorithmNames.join(','));
    ui->starNameDisplay->setText(QString::number(arg.missing));
    ui->starPosXDisplay->setText(QString::number(arg.redundance));
    ui->starPosYDisplay->setText(QString::number(arg.deviation));
    ui->starConsDisplay->setText(QString::number(arg.round));
    double res;
    QElapsedTimer timer;
    timer.start();
    getAlgorithm();
    QFuture<double> futureEval = QtConcurrent::run(this,&MainWindow::evalStarMapMatching,arg);
    while(!futureEval.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在进行验证")+LOADANI(int(timer.elapsed())));
        QApplication::processEvents();
    }
    res=futureEval.result();
    ui->statusBar->clearMessage();
    ui->starDescriptionDisplay->setText(QString::number(res*100)+"%");
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index==0)
    {
        ui->infoGroupBox->setTitle(tr("星点信息"));
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
        ui->posXInput->setReadOnly(true);
        ui->posYInput->setReadOnly(true);
    }
    else if(index==1)
    {
        ui->infoGroupBox->setTitle(tr("算法验证"));
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
        ui->posXInput->setReadOnly(true);
        ui->posYInput->setReadOnly(true);
    }
    else
    {
        ui->infoGroupBox->setTitle(tr("算法仿真"));
        ui->starNoLabel->setText(tr("算法"));
        ui->starNameLabel->setText(tr("包含星数"));
        ui->starPosXLabel->setText(tr("发现星数"));
        ui->starPosYLabel->setText(tr("假阳性数"));
        ui->starConsLabel->setText(tr("错误识别数"));
        ui->starDescriptionLabel->setText(tr("准确率"));
        ui->starNoDisplay->clear();
        ui->starNameDisplay->clear();
        ui->starPosXDisplay->clear();
        ui->starPosYDisplay->clear();
        ui->starConsDisplay->clear();
        ui->starDescriptionDisplay->clear();
        ui->posXInput->setReadOnly(false);
        ui->posYInput->setReadOnly(false);
    }
}

void MainWindow::getAlgorithm()
{
    algorithmNames.clear();
    algorithm[0]=ui->TMCheckbox->isChecked();
    algorithm[1]=ui->NOMCheckbox->isChecked();
    algorithm[2]=ui->RCFICheckbox->isChecked();
    algorithm[3]=ui->LPFICheckbox->isChecked();
    for(int i=0;i!=4;i++)
    {
        if(algorithm[i])
        {
            algorithmNames.push_back(AN[i]);
        }
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox aboutMsgBox;
    aboutMsgBox.setWindowTitle(tr("关于"));
    aboutMsgBox.setText(tr("    星图识别 版本：1.1\n \
    Copyright (C) 2019  常朝坤 赵伟丞 陈中钰\n \
\n \
    This program is free software: you can redistribute it and/or modify        \n \
    it under the terms of the GNU General Public License as published by        \n \
    the Free Software Foundation, either version 3 of the License, or       \n \
    (at your option) any later version. \n \
\n \
    This program is distributed in the hope that it will be useful,     \n \
    but WITHOUT ANY WARRANTY; without even the implied warranty of      \n \
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       \n \
    GNU General Public License for more details.        \n \
\n \
    You should have received a copy of the GNU General Public License       \n \
    along with this program.  If not, see <https://www.gnu.org/licenses/>.      \n"));
    aboutMsgBox.exec();
}

void MainWindow::on_starPointTable_cellDoubleClicked(int row, int column)
{
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(this->skyImg));
    ui->starNoDisplay->clear();
    ui->starNameDisplay->clear();
    ui->starPosXDisplay->clear();
    ui->starPosYDisplay->clear();
    ui->starConsDisplay->clear();
    ui->starDescriptionDisplay->clear();
    this->setAcceptDrops(false);
    double x=ui->starPointTable->item(row,1)->text().toDouble();
    double y=ui->starPointTable->item(row,2)->text().toDouble();
    this->ui->picDisplayScrollArea->ensureVisible(ceil(x),ceil(y));
    int res;
    QElapsedTimer timer;
    timer.start();
    ui->starPointTable->blockSignals(true);
    ui->statusBar->showMessage(tr("请稍候……正在寻找匹配"));
    getAlgorithm();
    QFuture<int> futureFMS = QtConcurrent::run(this,&MainWindow::findMatchingStar,ui->starPointTable->item(row,0)->text().toInt());
    while(!futureFMS.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在寻找匹配")+LOADANI(int(timer.elapsed())));
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
        matchingFailMsgBox.setWindowTitle(tr("错误"));
        matchingFailMsgBox.setText(tr("匹配失败！"));
        matchingFailMsgBox.exec();
   }
    ui->starPointTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->starPointTable->blockSignals(false);
    QImage tImg = this->skyImg;
    QPainter painter(&tImg);
    painter.setPen(QPen(QColor(255, 255, 255), 3));
    painter.drawEllipse(QPointF(x,y), 8, 8);
    painter.end();
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(tImg));
}

void MainWindow::on_simButton_clicked()
{
    QString dataset = data_path;
    if(pSMM->sky_.stars_.empty()) pSMM->LoadSky(dataset);
    ui->starNoDisplay->clear();
    ui->starNameDisplay->clear();
    ui->starPosXDisplay->clear();
    ui->starPosYDisplay->clear();
    ui->starConsDisplay->clear();
    ui->starDescriptionDisplay->clear();
    double theta = ui->thetaInput->text().toDouble();
    if(theta<0)
        theta=0;
    if(theta>360)
        theta=360;
    ui->thetaInput->setText(QString::number(theta));
    double x=ui->posXInput->text().toDouble();
    double y=ui->posYInput->text().toDouble();
    double scopeWdith = ui->viewWidthInput->text().toDouble();
    double scopeHeight = ui->viewHeightInput->text().toDouble();
    int imageWidth = ui->imageWidthInput->text().toInt();
    int imageHeight = ui->imageHeightInput->text().toInt();
    int ppi = ui->ppiInput->text().toInt();
    ui->simStarPointTable->setRowCount(0);
    ImageProperties ip(imageWidth,imageHeight,ppi,this->focus);
    GeneratedImage gi=this->pSMM->GenerateSimImage({-1,x,y,0},scopeWdith,scopeHeight,ip);
    QImage simImg = cvMat2QImage(gi.image_);
    this->skyImg=QImage(QString::fromStdString(gi.image_path_));
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(simImg));
    QElapsedTimer timer;
    timer.start();
    ui->starPointTable->blockSignals(true);
    ui->statusBar->showMessage(tr("请稍候……正在进行仿真"));
    getAlgorithm();
    QFuture<void> futureSim = QtConcurrent::run(this,&MainWindow::simStarMapMatching,gi);
    while(!futureSim.isFinished())
    {
        ui->statusBar->showMessage(tr("请稍候……正在进行仿真")+LOADANI(int(timer.elapsed())));
        QApplication::processEvents();
    }
    ui->statusBar->clearMessage();
    getAlgorithm();
    ui->starNoDisplay->setText(algorithmNames.join(','));
    ui->starNameDisplay->setText(QString::number(sr.total_num));
    ui->starPosXDisplay->setText(QString::number(sr.found_num));
    ui->starPosYDisplay->setText(QString::number(sr.false_pos));
    ui->starConsDisplay->setText(QString::number(sr.error_num));
    ui->starDescriptionDisplay->setText(QString::number(sr.accuracy*100)+"%");
}

void MainWindow::on_simStarPointTable_cellDoubleClicked(int row, int column)
{
    QImage tImg = this->skyImg;
    double x=ui->simStarPointTable->item(row,1)->text().toDouble();
    double y=ui->simStarPointTable->item(row,2)->text().toDouble();
    QPainter painter(&tImg);
    painter.setPen(QPen(QColor(255, 255, 255), 3));
    painter.drawEllipse(QPointF(x,y), 8, 8);
    painter.end();
    ui->picDisplayArea->setPixmap(QPixmap::fromImage(tImg));
}
