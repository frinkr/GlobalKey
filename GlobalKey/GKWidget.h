#ifndef GKWIDGET_H
#define GKWIDGET_H

#include <QWidget>

namespace Ui {
class GKWidget;
}

class GKWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GKWidget(QWidget *parent = nullptr);
    ~GKWidget();

private:
    Ui::GKWidget *ui;
};

#endif // GKWIDGET_H
