#ifndef MODEDIALOG_H
#define MODEDIALOG_H

#include <QDialog>

namespace Ui {
class ModeDialog;
}

class ModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModeDialog(QWidget *parent = nullptr);
    ~ModeDialog();
    void SetTitle(QString);
    int para_int;

signals:
    void  sendData(int);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ModeDialog *ui;
};

#endif // MODEDIALOG_H
