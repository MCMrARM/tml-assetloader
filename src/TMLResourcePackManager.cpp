#include "TMLResourcePackManager.h"

#include <tml/mod.h>
#include <minecraft/resource/ResourcePackManager.h>
#include <minecraft/resource/ResourcePack.h>
#include "TMLPackAccessStrategy.h"

TMLResourcePackManager TMLResourcePackManager::instance;

void TMLResourcePackManager::createResourcePack() {
    std::unique_ptr<PackAccessStrategy> source (new TMLPackAccessStrategy(*this, ""));
    mcResourcePack = std::unique_ptr<ResourcePack>(new ResourcePack(std::move(source), (ResourcePack::PackType) 1,
                                                                    (ResourcePackLocation) 0, true));
}

void TMLResourcePackManager::addModResources(tml::Mod* mod, tml::ModResources& resources, const std::string& modPath,
                                             const std::string& overridePath) {
    for (const auto& e : resources.list(modPath)) {
        if (e.isDirectory) {
            addModResources(mod, resources, modPath + e.fileName + "/", overridePath + e.fileName + "/");
        } else {
            modFileMap[overridePath + e.fileName].push_back({modPath + e.fileName, mod});
        }
    }
}

bool TMLResourcePackManager::hasAsset(std::string const& path) {
    return modFileMap.count(path) > 0;
}

bool TMLResourcePackManager::getAsset(std::string const& path, std::string& ret) {
    if (modFileMap.count(path) <= 0)
        return false;
    const auto& e = modFileMap.at(path).back();
    auto size = e.second->getResources().getSize(e.first);
    if (size <= 0)
        return false;
    {
        std::string emptyStr;
        *((void**) &ret) = *((void**) &emptyStr);
        // TODO: get rid of this ugly hack
    }
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
        stack->stack.push_back({TMLResourcePackManager::instance.mcResourcePack.get(), true});
    }
    original(this, std::move(stack), type, b);
}

TInstanceHook(std::vector<std::string>, _ZN19ResourcePackManager17loadAllVersionsOfERK16ResourceLocation,
              ResourcePackManager, ResourceLocation const& loc) {
    ResourcePack* tPack = TMLResourcePackManager::instance.mcResourcePack.get();
    std::vector<std::string> ret;
    for (auto& p : fullStack->stack) {
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