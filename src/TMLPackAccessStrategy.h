#pragma once

#include <minecraft/resource/PackAccessStrategy.h>
#include <minecraft/resource/ResourceLocation.h>

class TMLResourcePackManager;

class TMLPackAccessStrategy : public PackAccessStrategy {

public:
    TMLResourcePackManager& manager;
    ResourceLocation location;
    std::string name = "TML Resource Pack";
    std::string basePath;

    TMLPackAccessStrategy(TMLResourcePackManager& manager, std::string const& basePath) : manager(manager),
                                                                                          basePath(basePath) {
    }

    virtual unsigned int getPackSize() const { return 7; }
    virtual ResourceLocation const& getPackLocation() const { return location; }
    virtual std::string const& getPackName() const { return name; }
    virtual bool hasAsset(std::string const&) const;
    virtual bool getAsset(std::string const&, std::string&) const;
    virtual void forEachIn(std::string const&, std::function<void (std::string const&)>) const;
    virtual int getStrategyType() const { return 2; }
    virtual std::unique_ptr<PackAccessStrategy> createSubPack(std::string const&) const;

};