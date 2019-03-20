#pragma once
#include <QDialog>

namespace Ui {
    class GKAboutDialog;
}

class GKAboutDialog : public QDialog {
    Q_OBJECT

    public:
    explicit GKAboutDialog(QWidget * parent = nullptr);
    ~GKAboutDialog();

private:
    Ui::GKAboutDialog * ui_;
};
