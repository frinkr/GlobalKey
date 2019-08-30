#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QKeySequence>

#include "QHotkey"
#include "GKAppFactory.h"
#include "GKConfig.h"

static void toggleApp(GKPtr<GKApp> app) {
    if (!app->running())
        app->launch();
                
    if (app->atFrontmost()) {
        app->hide();
    }
    else {
        if (GKErr::noErr != app->bringFront()) {
            qDebug() << "Failed to bring front";
        }
    }    
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    auto & cfg = GKConfig::instance();
    for (size_t i = 0; i < cfg.appCount(); ++ i) {
        QKeySequence keySeq(QString::fromStdString(cfg.appKeySequence(i)));
        QHotkey * hotKey = new QHotkey(keySeq, true, &app);
        app.connect(hotKey, &QHotkey::activated, [i]() {
            auto id = GKConfig::instance().appId(i);
            if (auto app = GKAppFactory::instance().getOrCreateApp(id)) 
                toggleApp(app);
        });
    }
    return app.exec();
}
