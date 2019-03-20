#ifndef GKDIALOG_H
#define GKDIALOG_H

#include <QDialog>

namespace Ui {
class GKDialog;
}

class GKDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GKDialog(QWidget *parent = nullptr);
    ~GKDialog();

private:
    Ui::GKDialog *ui;
};

#endif // GKDIALOG_H
