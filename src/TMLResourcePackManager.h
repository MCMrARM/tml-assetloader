#pragma once

#include <string>
#include <memory>
#include <set>
#include <tml/mod.h>
#include <minecraft/resource/ResourcePack.h>

class TMLResourcePackManager {

private:

    std::map<std::string, std::vector<std::pair<std::string, tml::Mod*>>> modFileMap;
    std::set<std::string> modDirectories;

public:

    static TMLResourcePackManager instance;

    std::unique_ptr<ResourcePack> mcResourcePack;

    void createResourcePack();

    void addModResources(tml::Mod* mod, tml::ModResources& resources,
                         const std::string& modPath = std::string("resource_pack/"),
                         const std::string& overridePath = std::string());

    bool hasAsset(std::string const& path);

    bool hasFolder(std::string const& path);

    bool getAsset(std::string const& path, std::string& ret);

    void getAllAssets(std::vector<std::string>& ret, std::string const& path);

};



