#include <algorithm>
#include <cctype>
#include <vector>
#include "GKProxyApp.h"
#include "GKCommand.h"
#include "GKSystemService.h"

namespace {
    std::pair<std::string, std::vector<std::string>> 
    splitCommandText(const std::string& commandText) {
        auto pos = commandText.find_first_of(' ');
        if (pos != commandText.npos) {
            auto cmd = commandText.substr(0, pos);
            auto arg = commandText.substr(pos + 1);
            
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c); });
            return { cmd, {arg} };
        }
        else {
            return { commandText, {} };
        }
    }
}


void
GKCommand::notifyBadCommand(const std::string & cmd, const std::vector<std::string> & args) {
    GKSystemService::postNotification("Bad command '", cmd, "'");
}

GK_REGISTER_COMMNAND("toggle", GKToggleAppCommand);

void
GKToggleAppCommand::run(const std::string & cmd, const std::vector<std::string>& args) {
    auto appDesc = args.front();
    auto appProxy = std::make_shared<GKAppProxy>(appDesc);
    if (!appProxy) {
        GKSystemService::postNotification("Failed to find application ", appDesc);
        return;
    }

    if (!appProxy->running())
        if (GKErr::noErr != appProxy->launch()) {
            GKSystemService::postNotification("Failed to launch application ", appDesc);
            return;
        }
    if (appProxy->atFrontmost())
        appProxy->hide();
    else
        appProxy->bringFront();
}


GK_REGISTER_COMMNAND("volume", GKSystemCommand);
GK_REGISTER_COMMNAND("mute", GKSystemCommand);
GK_REGISTER_COMMNAND("open", GKSystemCommand);
GK_REGISTER_COMMNAND("openurl", GKSystemCommand);

void
GKSystemCommand::run(const std::string & cmd, const std::vector<std::string>& args) {
    if (cmd == "volume") {
        if (args.size() == 1) {
            int value = std::stoi(args[0]);
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
            GKSystemService::open(args[0]);
    }
    else if (cmd == "openurl") {
        if (args.empty())
            notifyBadCommand(cmd, args);
        else
            GKSystemService::openUrl(args[0]);
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
        GKSystemService::postNotification("Command '", cmd, "' not found!");
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
