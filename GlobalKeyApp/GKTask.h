#pragma once
#include "GK.h"

class GKTask : private GKNoCopy {
public:
    virtual ~GKTask() {}

    virtual void
    run() = 0;
};

class GKToggleAppTask : public GKTask {
public:
    explicit GKToggleAppTask(size_t appIndex);

    void
    run() override;

private:
    size_t appIndex_;
};
