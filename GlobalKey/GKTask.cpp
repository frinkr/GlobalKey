#include "GKTask.h"
#include "GKConfig.h"

GKToggleAppTask::GKToggleAppTask(size_t appIndex)
    : appIndex_(appIndex) {}

void
GKToggleAppTask::run() {
    auto appDescriptor = GKConfig::instance().appDescriptor(appIndex_);
    auto appProxy = std::make_shared<GKAppProxy>(appDescriptor);
    if (!appProxy) {
        GKSystem::instance().postNotification("GlobalKey", "Failed to find application " + appDescriptor);
        return;
    }

    if (!appProxy->running())
        if (GKErr::noErr != appProxy->launch()) {
            GKSystem::instance().postNotification("GlobalKey", "Failed to launch application " + appDescriptor);
            return;
        }
    if (appProxy->atFrontmost())
        appProxy->hide();
    else
        appProxy->bringFront();
}