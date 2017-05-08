#include "TMLResourcePackManager.h"

#include <tml/mod.h>
#include <minecraft/resource/ResourcePackManager.h>
#include <minecraft/resource/ResourcePack.h>
#include <minecraft/resource/PackManifest.h>
#include "TMLPackAccessStrategy.h"

TMLResourcePackManager TMLResourcePackManager::instance;

void TMLResourcePackManager::createResourcePack() {
    std::unique_ptr<PackAccessStrategy> source (new TMLPackAccessStrategy(*this, ""));
    mcResourcePack = std::unique_ptr<ResourcePack>(new ResourcePack(std::move(source), PackCategory::FREE,
                                                                    (PackOrigin) 0, true));
    mcResourcePack->manifest = std::unique_ptr<PackManifest>(new PackManifest((ManifestType) 0));
}

void TMLResourcePackManager::addModResources(tml::Mod* mod, tml::ModResources& resources, const std::string& modPath,
                                             const std::string& overridePath) {
    for (const auto& e : resources.list(modPath)) {
        if (e.isDirectory) {
            modDirectories.insert(overridePath + e.fileName + "/");
            addModResources(mod, resources, modPath + e.fileName + "/", overridePath + e.fileName + "/");
        } else {
            modFileMap[overridePath + e.fileName].push_back({modPath + e.fileName, mod});
        }
    }
}

bool TMLResourcePackManager::hasAsset(std::string const& path) {
    return modFileMap.count(path) > 0;
}

bool TMLResourcePackManager::hasFolder(std::string const& path) {
    return modDirectories.count(path) > 0;
}

bool TMLResourcePackManager::getAsset(std::string const& path, std::string& ret) {
    if (modFileMap.count(path) <= 0)
        return false;
    const auto& e = modFileMap.at(path).back();
    auto size = e.second->getResources().getSize(e.first);
    if (size <= 0)
        return false;
    ret.resize((size_t) size);
    e.second->getResources().open(e.first)->read(&ret[0], (size_t) size);
    return true;
}

void TMLResourcePackManager::getAllAssets(std::vector<std::string>& ret, std::string const& path) {
    if (modFileMap.count(path) <= 0)
        return;
    for (const auto& e : modFileMap.at(path)) {
        auto size = e.second->getResources().getSize(e.first);
        if (size <= 0)
            continue;
        std::string str;
        str.resize((size_t) size);
        e.second->getResources().open(e.first)->read(&str[0], (size_t) size);
        ret.push_back(std::move(str));
    }
}

TInstanceHook(void, _ZN19ResourcePackManager8setStackESt10unique_ptrI17ResourcePackStackSt14default_deleteIS1_EE21ResourcePackStackTypeb, ResourcePackManager, std::unique_ptr<ResourcePackStack> stack, int type, bool b) {
    if (type == 0) {
        if (!TMLResourcePackManager::instance.mcResourcePack)
            TMLResourcePackManager::instance.createResourcePack();
        ResourcePackRepository* np = nullptr;
        stack->add(TMLResourcePackManager::instance.mcResourcePack.get(), *np, false);
    }
    original(this, std::move(stack), type, b);
}

TInstanceHook(std::vector<std::string>, _ZNK17ResourcePackStack17loadAllVersionsOfERK16ResourceLocation,
              ResourcePackStack, ResourceLocation const& loc) {
    ResourcePack* tPack = TMLResourcePackManager::instance.mcResourcePack.get();
    std::vector<std::string> ret;
    for (auto& p : stack) {
        if (p.pack->hasResource(loc.path)) {
            if (p.pack == tPack) {
                TMLResourcePackManager::instance.getAllAssets(ret, loc.path);
                continue;
            }
            std::string res = " ";
            if (!p.pack->getResource(loc.path, res))
                continue;
            ret.push_back(std::move(res));
        }
    }
    return std::move(ret);
}
