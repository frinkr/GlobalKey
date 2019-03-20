#include "GKAboutDialog.h"
#include "ui_GKAboutDialog.h"

GKAboutDialog::GKAboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui_(new Ui::GKAboutDialog) {
    ui_->setupUi(this);
}

GKAboutDialog::~GKAboutDialog() {
    delete ui_;
}
