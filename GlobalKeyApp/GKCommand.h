#pragma once
#include <map>
#include "GK.h"

class GKCommand : private GKNoCopy {
public:
    virtual ~GKCommand() {}

    virtual std::unique_ptr<GKCommand>
    clone() const = 0;
    
    virtual void
    run(const std::vector<std::string> & args) = 0;
};

template <typename T>
class GKClonableCommand : public GKCommand {
public:
    virtual std::unique_ptr<GKCommand>
    clone() const override {
        return std::make_unique<T>();
    }
};

class GKToggleAppCommand : public GKClonableCommand<GKToggleAppCommand> {
public:
    void
    run(const std::vector<std::string>& args) override;
};

class GKSystemVolumeCommand : public GKClonableCommand<GKSystemVolumeCommand> {
public:
    void
    run(const std::vector<std::string>& args) override;
};



class GKCommandEngine : private GKNoCopy {
public:
    static GKCommandEngine&
    instance();

    void
    runCommand(const std::string & commandText) const;

    std::unique_ptr<GKCommand>
    createCommand(const std::string& commandName) const;

    void
    regiseterCommand(const std::string& commandName, std::unique_ptr<GKCommand> commandPrototype);

private:
    std::map<std::string, std::unique_ptr<GKCommand>> commands_;
};
