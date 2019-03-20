#include "GKAboutDialog.h"
#include "ui_GKAboutDialog.h"

GKAboutDialog::GKAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GKAboutDialog)
{
    ui->setupUi(this);
}

GKAboutDialog::~GKAboutDialog()
{
    delete ui;
}
