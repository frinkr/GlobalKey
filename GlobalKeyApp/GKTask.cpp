#include <vector>
#include "GKTask.h"
#include "GKSystem.h"
#include "GKApplicationService.h"

namespace {
    std::pair<std::string, std::vector<std::string>> 
    splitTaskCommand(const std::string& taskCommand) {
        auto pos = taskCommand.find_first_of(' ');
        if (pos != taskCommand.npos) {
            auto cmd = taskCommand.substr(0, pos);
            auto arg = taskCommand.substr(pos + 1);
            return { cmd, {arg} };
        }
        else {
            return { taskCommand, {} };
        }
    }

    template <typename ... T> void
    postNotification(T ... args) {
        auto message = (std::string() + ... + args);
        GKSystem::postNotification("GlobalKey", message);
    }
}

void
GKToggleAppTask::run(const std::vector<std::string>& args) {
    auto appDesc = args.front();
    auto appProxy = std::make_shared<GKAppProxy>(appDesc);
    if (!appProxy) {
        postNotification("Failed to find application ", appDesc);
        return;
    }

    if (!appProxy->running())
        if (GKErr::noErr != appProxy->launch()) {
            postNotification("Failed to launch application ", appDesc);
            return;
        }
    if (appProxy->atFrontmost())
        appProxy->hide();
    else
        appProxy->bringFront();
}

GKTaskEngine&
GKTaskEngine::instance() {
    static GKTaskEngine engine;
    return engine;
}

void
GKTaskEngine::runTask(const std::string& taskCommand) {
    auto [cmd, args] = splitTaskCommand(taskCommand);
    if (auto task = createTask(cmd))
        task->run(args);
    else
        postNotification("Command ", cmd, " not found!");
}

std::unique_ptr<GKTask>
GKTaskEngine::createTask(const std::string& taskName) {
    if (taskName == "toggle")
        return std::make_unique<GKToggleAppTask>();

    return std::unique_ptr<GKTask>();
}
