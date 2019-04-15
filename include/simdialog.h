#ifndef SIMDIALOG_H
#define SIMDIALOG_H

#include <QDialog>
#include "SkyMapMatching.h"
namespace Ui {
class simDialog;
}

class simDialog : public QDialog
{
    Q_OBJECT

public:
    explicit simDialog(QWidget *parent = nullptr);
    ~simDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void  sendData(evalArgs);

private:
    Ui::simDialog *ui;
};

#endif // SIMDIALOG_H
