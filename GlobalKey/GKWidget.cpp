#include "GKWidget.h"
#include "ui_GKWidget.h"

GKWidget::GKWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GKWidget)
{
    ui->setupUi(this);
}

GKWidget::~GKWidget()
{
    delete ui;
}
