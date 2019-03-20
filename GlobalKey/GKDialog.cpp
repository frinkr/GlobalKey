#include "GKDialog.h"
#include "ui_GKDialog.h"

GKDialog::GKDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GKDialog)
{
    ui->setupUi(this);
}

GKDialog::~GKDialog()
{
    delete ui;
}
