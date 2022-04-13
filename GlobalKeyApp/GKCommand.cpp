#include <algorithm>
#include <cctype>
#include <vector>
#include "GKArgs.h"
#include "GKProxyApp.h"
#include "GKCommand.h"
#include "GKSystemService.h"

namespace {
    std::pair<std::string, std::string>
    splitCommandText(const std::string& commandText) {
        GKArgs args(commandText);
        auto cmd = args.take_head();
        auto arg = args.rest();
            
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c); });
        return { cmd, arg };
    }
}


void
GKCommand::notifyBadCommand(const std::string & cmd, const std::string & args) {
    GKSystemService::showMessage("Bad command '", cmd, "' with arguments ", args);
}

GK_REGISTER_COMMNAND("toggle", GKToggleAppCommand);

void
GKToggleAppCommand::run(const std::string & cmd, const std::string & args) {
    auto appProxy = std::make_shared<GKProxyApp>(args);
    if (!appProxy) {
        GKSystemService::showMessage("Failed to find application ", args);
        return;
    }

    if (!appProxy->running()) {
        if (GKErr::noErr != appProxy->launch()) {
            GKSystemService::showMessage("Failed to launch application ", args);
            return;
        }
    }
    if (appProxy->atFrontmost() && appProxy->visible())
        appProxy->hide();
    else
        appProxy->bringFront();
}


GK_REGISTER_COMMNAND("volume", GKSystemCommand);
GK_REGISTER_COMMNAND("mute", GKSystemCommand);
GK_REGISTER_COMMNAND("open", GKSystemCommand);
GK_REGISTER_COMMNAND("openurl", GKSystemCommand);
GK_REGISTER_COMMNAND("lockscreen", GKSystemCommand);
GK_REGISTER_COMMNAND("computersleep", GKSystemCommand);

void
GKSystemCommand::run(const std::string & cmd, const std::string & args) {
    if (cmd == "volume") {
        if (!args.empty()) {
            int value = std::stoi(args);
            GKSystemService::adjustVolume(value);
        } 
    }
    else if (cmd == "mute") {
        if (GKSystemService::audioMuted())
            GKSystemService::unmuteAudio();
        else
            GKSystemService::muteAudio();
    }
    else if (cmd == "open") {
        if (args.empty())
            notifyBadCommand(cmd, args);
        else
            GKSystemService::open(args);
    }
    else if (cmd == "openurl") {
        if (args.empty())
            notifyBadCommand(cmd, args);
        else
            GKSystemService::openUrl(args);
    }
    else if (cmd == "lockscreen") {
        GKSystemService::lockScreen();
    }
    else if (cmd == "computersleep") {
        GKSystemService::computerSleep();
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
        task->run(cmd, args);
    else
        GKSystemService::showMessage("Command '", cmd, "' not found!");
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
