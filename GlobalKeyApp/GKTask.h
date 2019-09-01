#pragma once
#include "GK.h"

class GKTask : private GKNoCopy {
public:
    virtual ~GKTask() {}

    virtual void
    run(const std::vector<std::string> & args) = 0;
};

class GKToggleAppTask : public GKTask {
public:
    void
    run(const std::vector<std::string>& args) override;
};


class GKTaskEngine : private GKNoCopy {
public:

    static GKTaskEngine&
    instance();

    void
    runTask(const std::string & taskCommand);

    std::unique_ptr<GKTask>
    createTask(const std::string& taskName);
};
