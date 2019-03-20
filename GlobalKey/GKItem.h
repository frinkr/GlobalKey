#pragma once

#include <QKeySequence>
#include <QObject>
#include <QString>

class GKItem : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;

private:
    QKeySequence  keySequence_;
    QString       appPath_;
};
