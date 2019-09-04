#pragma once
#include <map>
#include "GK.h"

class GKCommand : private GKNoCopy {
public:
    virtual ~GKCommand() {}

    virtual std::unique_ptr<GKCommand>
    clone() const = 0;
    
    virtual void
    run(const std::string & cmd, const std::vector<std::string> & args) = 0;
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
    run(const std::string & cmd, const std::vector<std::string>& args) override;
};

class GKSystemCommand : public GKClonableCommand<GKSystemCommand> {
public:
    void
    run(const std::string & cmd, const std::vector<std::string>& args) override;
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

template <typename T>
class GKCommandAutoRegister {
public:
    explicit GKCommandAutoRegister(const std::string & name) {
        GKCommandEngine::instance().regiseterCommand(name, std::make_unique<T>());
    }
};

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define GK_REGISTER_COMMNAND(name, T)                               \
    namespace {                                                     \
        GKCommandAutoRegister<T> COMBINE(register, __LINE__)(name); \
    }
