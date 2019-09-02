#include <vector>
#include "GKProxyApp.h"
#include "GKCommand.h"
#include "GKSystem.h"
#include "GKSystemService.h"

namespace {
    std::pair<std::string, std::vector<std::string>> 
    splitCommandText(const std::string& commandText) {
        auto pos = commandText.find_first_of(' ');
        if (pos != commandText.npos) {
            auto cmd = commandText.substr(0, pos);
            auto arg = commandText.substr(pos + 1);
            return { cmd, {arg} };
        }
        else {
            return { commandText, {} };
        }
    }

    template <typename ... T> void
    postNotification(T ... args) {
        auto message = (std::string() + ... + args);
        GKSystem::postNotification("GlobalKey", message);
    }
}

GK_REGISTER_COMMNAND("toggle", GKToggleAppCommand);

void
GKToggleAppCommand::run(const std::vector<std::string>& args) {
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


GK_REGISTER_COMMNAND("volume", GKSystemVolumeCommand);

void
GKSystemVolumeCommand::run(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        int value = std::stoi(args[0]);
        GKSystemService::adjustVolume(value);
    }
}

GKCommandEngine&
GKCommandEngine::instance() {
    static GKCommandEngine engine;
    return engine;
}

void
GKCommandEngine::runCommand(const std::string& commandText) const {
    auto [cmd, args] = splitCommandText(commandText);
    if (auto task = createCommand(cmd))
        task->run(args);
    else
        postNotification("Command '", cmd, "' not found!");
}

std::unique_ptr<GKCommand>
GKCommandEngine::createCommand(const std::string& commandName) const {
    auto itr = commands_.find(commandName);
    if (itr != commands_.end()) 
        return itr->second->clone();    
    return std::unique_ptr<GKCommand>();
}

void
GKCommandEngine::regiseterCommand(const std::string& commandName, std::unique_ptr<GKCommand> commandPrototype) {
    commands_[commandName] = std::move(commandPrototype);
}
