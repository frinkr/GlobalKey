#include "GKWidgetItem.h"
#include "ui_GKWidgetItem.h"

GKWidgetItem::GKWidgetItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GKWidgetItem)
{
    ui->setupUi(this);
}

GKWidgetItem::~GKWidgetItem()
{
    delete ui;
}
