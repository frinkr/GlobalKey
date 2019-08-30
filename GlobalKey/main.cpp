#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QIcon>
#include <QFileIconProvider>
#include <QKeySequence>
#include <QMenu>
#include <QUrl>
#include <QSystemTrayIcon>

#include "QHotkey"

#include "GKAppFactory.h"
#include "GKConfig.h"
#include "GKSystemTray.h"

#define gkLog qDebug

static void toggleApp(GKPtr<GKApp> app) {
    if (!app->running())
        app->launch();
                
    if (app->atFrontmost()) 
        app->hide();
    else 
        app->bringFront();
}

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(Resources);
    
    QApplication app(argc, argv);
    auto & cfg = GKConfig::instance();

    std::vector<QHotkey*> hotKeys;
    // Register the hotkeys
    for (size_t i = 0; i < cfg.appCount(); ++ i) {
        QKeySequence keySeq(QString::fromStdString(cfg.appKeySequence(i)));
        QHotkey * hotKey = new QHotkey(keySeq, true, &app);
        app.connect(hotKey, &QHotkey::activated, [i]() {
            auto appId = GKConfig::instance().appId(i);
            if (auto app = GKAppFactory::instance().getOrCreateApp(appId)) 
                toggleApp(app);
            else
                gkLog() << "Not able to find application " << QString::fromStdString(appId->description());
        });
        hotKeys.push_back(hotKey);
    }

    // Create system tray
    QIcon trayIconEnabled(":/SON.png");
    QIcon trayIconDisabled(":/SOFF.png");
    auto tray = new QSystemTrayIcon(trayIconEnabled, &app);
    auto menu = new QMenu();
    
    QAction * disableAction;
    disableAction = menu->addAction("Disable", [trayIconEnabled, trayIconDisabled, tray, &disableAction, &hotKeys]() {
        if (hotKeys.empty())
            return;
        QHotkey * key = hotKeys.front();
        bool state = key->isRegistered();
        for (auto key : hotKeys)
            key->setRegistered(!state);
        
        if (state) {
            disableAction->setText("Enable");
            tray->setIcon(trayIconDisabled);
        }
        else {
            disableAction->setText("Disable");
            tray->setIcon(trayIconEnabled);
        }
    });
    
    menu->addAction("Edit Shortcuts", [&cfg]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(cfg.path())));
    });
    
    menu->addSeparator();
    menu->addAction("Quit", [&app]() {
        app.quit();
    });
    
    tray->setContextMenu(menu);
    tray->setToolTip("Global Key Shortcuts");
    tray->show();
    
    return app.exec();
}
