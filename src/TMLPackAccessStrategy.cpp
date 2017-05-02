#include "TMLPackAccessStrategy.h"
#include "TMLResourcePackManager.h"
#include <stdlib.h>

bool TMLPackAccessStrategy::hasAsset(std::string const& name) const {
    return manager.hasAsset(name);
}

bool TMLPackAccessStrategy::hasFolder(std::string const& name) const {
    return manager.hasFolder(name);
}

bool TMLPackAccessStrategy::getAsset(std::string const& name, std::string& ret) const {
    return manager.getAsset(name, ret);
}

void TMLPackAccessStrategy::forEachIn(std::string const& name, std::function<void(std::string const&)>) const {
    printf("TMLPackAccessStrategy::forEachIn: %s - unimplemented\n", name.c_str());
}

std::unique_ptr<PackAccessStrategy> TMLPackAccessStrategy::createSubPack(std::string const& path) const {
    printf("TMLPackAccessStrategy::createSubPack: %s %s\n", basePath.c_str(), path.c_str());
    return std::unique_ptr<PackAccessStrategy>(new TMLPackAccessStrategy(manager, basePath + path));
}

void TMLPackAccessStrategy::generateAssetSet() {
    printf("TMLPackAccessStrategy::generateAssetSet - unimplemented\n");
}
