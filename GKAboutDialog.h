#ifndef GKABOUTDIALOG_H
#define GKABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class GKAboutDialog;
}

class GKAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GKAboutDialog(QWidget *parent = nullptr);
    ~GKAboutDialog();

private:
    Ui::GKAboutDialog *ui;
};

#endif // GKABOUTDIALOG_H
