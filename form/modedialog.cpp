#include "modedialog.h"
#include "ui_modedialog.h"

ModeDialog::ModeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModeDialog)
{
    ui->setupUi(this);
}

ModeDialog::~ModeDialog()
{
    delete ui;
}

void ModeDialog::SetTitle(QString title){
    ui->label->setText(title);
}

void ModeDialog::on_buttonBox_accepted()
{
    //emit sendData(ui->lineEdit->text().toInt());
    para_int = ui->lineEdit->text().toInt();
}

void ModeDialog::on_buttonBox_rejected()
{
    this->reject();
}
