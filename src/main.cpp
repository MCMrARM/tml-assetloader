#include <tml/mod.h>
#include <tml/modloader.h>
#include <map>
#include <minecraft/app/AppPlatform.h>
#include <minecraft/util/Util.h>

static std::map<std::string, std::pair<std::string, tml::Mod*>> assetModMap;

static std::string removeAssetPathPrefix(AppPlatform* ap, const std::string& path) {
    std::string packagePath = ap->getPackagePath();
    if (path.size() >= packagePath.size() && memcmp(path.c_str(), packagePath.c_str(), packagePath.size()) == 0)
        return path.substr(packagePath.length());
    return path;
}
static std::string readCustomAssetFile(const std::string& path) {
    const auto& e = assetModMap.at(path);
    auto size = e.second->getResources().getSize(e.first);
    if (size <= 0)
        return Util::EMPTY_STRING;
    std::string ret;
    ret.resize(size);
    e.second->getResources().open(e.first)->read(&ret[0], size);
    return std::move(ret);
}

TInstanceHook(std::string, _ZN11AppPlatform13readAssetFileERKSs, AppPlatform, std::string const& path) {
    std::string fullPath = removeAssetPathPrefix(this, path);
    if (assetModMap.count(fullPath) > 0)
        return readCustomAssetFile(fullPath);
	  return original(path);
}
TInstanceHook(std::string, _ZN21AppPlatform_android2313readAssetFileERKSs, AppPlatform, std::string const& path) {
    // we're safe to assume this will be only used to android; we don't need to remove the package path prefix
    if (assetModMap.count(path) > 0)
        return readCustomAssetFile(path);
	  return original(path);
}

static void addAssetModMapEntry(tml::Mod* mod, tml::ModResources& resources, const std::string& path, const std::string& overridePath) {
    for (const auto& e : resources.list(path)) {
        if (e.isDirectory) {
            addAssetModMapEntry(mod, resources, path + e.fileName + "/", overridePath + e.fileName + "/");
        } else {
            assetModMap[overridePath + e.fileName] = {path + e.fileName, mod};
        }
    }
}

extern "C" {

void tml_init(tml::Mod& mod) {
    for (tml::Mod* m : mod.getLoader().getMods()) {
        addAssetModMapEntry(m, m->getResources(), "assets/", "");
        addAssetModMapEntry(m, m->getResources(), "resource_pack/", "resource_packs/vanilla/");
    }
}

}
