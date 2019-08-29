#include "GKDialog.h"
#include "ui_GKDialog.h"
#include "QHotkey"
#include <QDebug>
#include <QProcess>
GKDialog::GKDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GKDialog)
{
    ui->setupUi(this);
    //ui->hotkeySequenceEdit_1->keySequence()
    auto hk = new QHotkey(this);
    
    connect(ui->hotkeySequenceEdit_1, &QKeySequenceEdit::keySequenceChanged, [hk](auto ks) {
        hk->setShortcut(ks, true);
    });
    
    connect(hk, &QHotkey::activated, [this] {
        qDebug() << "hello";
        QProcess::startDetached("/Applications/Emacs.app/Contents/MacOS/Emacs");
    });
}

GKDialog::~GKDialog()
{
    delete ui;
}
