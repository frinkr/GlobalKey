#ifndef GKWIDGETITEM_H
#define GKWIDGETITEM_H

#include <QWidget>

namespace Ui {
class GKWidgetItem;
}

class GKWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit GKWidgetItem(QWidget *parent = nullptr);
    ~GKWidgetItem();

private:
    Ui::GKWidgetItem *ui;
};

#endif // GKWIDGETITEM_H
