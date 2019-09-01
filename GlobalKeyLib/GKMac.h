#pragma once

#include <vector>

#include "GK.h"

class GKMacAppId : public GKAppId{
public:

    explicit GKMacAppId(std::string path);

    const std::string &
    path() const;

    std::string
    descriptor() const override;

private:
    std::string path_;
};

class GKMacApp : public GKAppProxy {
public:
    explicit GKMacApp(const GKMacAppId & appId);

    ~GKMacApp();
    
    const GKMacAppId &
    ref() const override;
 
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
        
    GKPtr<GKAppProxy>
    getOrCreateApp(GKPtr<const GKAppId> appId) override;
};


class GKMacConfig : public GKConfig {
private:
    struct Entry {
        std::string   keySequence;
        std::string   bundlePath;
    };
public:
    GKMacConfig();

    std::string
    path() const override;
    
    size_t
    appCount() const override;

    std::string
    keySequence(size_t index) const override;
    
    GKPtr<const GKAppId>
    appId(size_t index) const override;

private:
    std::string file_;

    std::vector<Entry> entries_;
};


class GKMacSystem : public GKSystem {
public:
    void
    postNotification(const std::string & title, const std::string & message) override;
};
