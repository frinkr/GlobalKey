#pragma once
#include "GK.h"

class GKCommand : private GKNoCopy {
public:
    virtual ~GKCommand() {}

    virtual void
    run(const std::vector<std::string> & args) = 0;
};

class GKToggleAppCommand : public GKCommand {
public:
    void
    run(const std::vector<std::string>& args) override;
};


class GKSystemVolumeCommand : public GKCommand {
public:
    void
    run(const std::vector<std::string>& args) override;
};

class GKCommandEngine : private GKNoCopy {
public:

    static GKCommandEngine&
    instance();

    void
    runCommand(const std::string & commandText);

    std::unique_ptr<GKCommand>
    createCommand(const std::string& commandName);
};
