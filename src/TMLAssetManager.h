#pragma once

#include <string>
#include <memory>
#include <tml/mod.h>
#include <minecraft/resource/ResourcePack.h>

class TMLAssetManager {

private:

    std::map<std::string, std::pair<std::string, tml::Mod*>> modFileMap;

public:

    static TMLAssetManager instance;

    void addModAssets(tml::Mod* mod, tml::ModResources& resources,
                      const std::string& modPath = std::string("assets/"),
                      const std::string& overridePath = std::string());

    inline bool hasAsset(std::string const& path) { return modFileMap.count(path) > 0; }

    std::string getAsset(std::string const& path);

};
