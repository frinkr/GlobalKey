#include "GKTask.h"
#include "GKSystem.h"
#include "GKApplicationService.h"

GKToggleAppTask::GKToggleAppTask(size_t appIndex)
    : appIndex_(appIndex) {}

void
GKToggleAppTask::run() {
    auto taskDescriptor = GKConfig::instance().taskDescriptor(appIndex_);
    auto appProxy = std::make_shared<GKAppProxy>(taskDescriptor);
    if (!appProxy) {
        GKSystem::postNotification("GlobalKey", "Failed to find application " + taskDescriptor);
        return;
    }

    if (!appProxy->running())
        if (GKErr::noErr != appProxy->launch()) {
            GKSystem::postNotification("GlobalKey", "Failed to launch application " + taskDescriptor);
            return;
        }
    if (appProxy->atFrontmost())
        appProxy->hide();
    else
        appProxy->bringFront();
}