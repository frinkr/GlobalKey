#pragma once

#include <vector>

#include "GKConfig.h"
#include "GKAppFactory.h"

class GKMacAppId : public GKAppId{
public:

    explicit GKMacAppId(std::string path);

    const std::string &
    path() const;

    std::string
    description() const override;

private:
    std::string path_;
};

class GKMacApp : public GKApp {
public:
    explicit GKMacApp(const GKMacAppId & appId);

    ~GKMacApp();
    
    const GKMacAppId &
    id() const override;
 
    GKErr
    bringFront() override;

    GKErr
    show() override;

     GKErr
    hide() override;

    bool
    visible() const override;

    bool
    atFrontmost() const override;

    bool
    running() const override;

    GKErr
    launch() override;

private:
    GKMacAppId id_;
    struct Imp;
    std::unique_ptr<Imp> imp_;
};


class GKMacAppFactory : public GKAppFactory {
public:
    GKMacAppFactory();
        
    GKPtr<GKApp>
    getOrCreateApp(GKPtr<const GKAppId> appId) override;
};


class GKMacConfig : public GKConfig {
private:
    struct Entry {
        std::string   keySequence;
        std::string   bundlePath;
    };
public:
    explicit GKMacConfig(std::string file);

    std::string
    path() const override;
    
    size_t
    appCount() const override;

    std::string
    appKeySequence(size_t index) const override;
    
    GKPtr<const GKAppId>
    appId(size_t index) const override;

private:
    std::string file_;

    std::vector<Entry> entries_;
};
