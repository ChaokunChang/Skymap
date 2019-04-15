#include "simdialog.h"
#include "ui_simdialog.h"

simDialog::simDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::simDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

simDialog::~simDialog()
{
    delete ui;
}

void simDialog::on_buttonBox_accepted()
{
    emit sendData({ui->roundInput->text().toInt(),ui->missingInput->text().toInt(),ui->redundanceInput->text().toInt(),ui->deviationInput->text().toDouble()});
}

void simDialog::on_buttonBox_rejected()
{
    this->reject();
}
