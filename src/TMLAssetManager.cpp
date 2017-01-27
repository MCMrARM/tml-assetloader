#include "TMLAssetManager.h"

#include <minecraft/client/AppPlatform.h>
#include <string.h>

TMLAssetManager TMLAssetManager::instance;

void TMLAssetManager::addModAssets(tml::Mod* mod, tml::ModResources& resources, const std::string& modPath,
                                   const std::string& overridePath) {
    for (const auto& e : resources.list(modPath)) {
        if (e.isDirectory) {
            addModAssets(mod, resources, modPath + e.fileName + "/", overridePath + e.fileName + "/");
        } else {
            modFileMap[overridePath + e.fileName] = {modPath + e.fileName, mod};
        }
    }
}

std::string TMLAssetManager::getAsset(std::string const& path) {
    const auto& e = modFileMap.at(path);
    auto size = e.second->getResources().getSize(e.first);
    if (size <= 0)
        return Util::EMPTY_STRING;
    std::string ret;
    ret.resize((size_t) size);
    e.second->getResources().open(e.first)->read(&ret[0], (size_t) size);
    return std::move(ret);
}

static std::string removeAssetPathPrefix(AppPlatform* ap, const std::string& path) {
    std::string packagePath = ap->getPackagePath();
    if (path.size() >= packagePath.size() && memcmp(path.c_str(), packagePath.c_str(), packagePath.size()) == 0)
        return path.substr(packagePath.length());
    return path;
}

TInstanceHook(std::string, _ZN11AppPlatform13readAssetFileERKSs, AppPlatform, std::string const& path) {
    std::string fullPath = removeAssetPathPrefix(this, path);
    auto& mgr = TMLAssetManager::instance;
    if (mgr.hasAsset(fullPath) > 0)
        return mgr.getAsset(fullPath);
    return original(this, path);
}
TInstanceHook(std::string, _ZN21AppPlatform_android2313readAssetFileERKSs, AppPlatform, std::string const& path) {
    // we're safe to assume this will be only used to android; we don't need to remove the package path prefix
    auto& mgr = TMLAssetManager::instance;
    if (mgr.hasAsset(path) > 0)
        return mgr.getAsset(path);
    return original(this, path);
}