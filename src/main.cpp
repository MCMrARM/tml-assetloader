#include <tml/mod.h>
#include <tml/modloader.h>

#include "TMLResourcePackManager.h"
#include "TMLAssetManager.h"

extern "C" {

void tml_init(tml::Mod& mod) {
    TMLAssetManager& am = TMLAssetManager::instance;
    TMLResourcePackManager& rpm = TMLResourcePackManager::instance;
    for (tml::Mod* m : mod.getLoader().getMods()) {
        am.addModAssets(m, m->getResources());
        rpm.addModResources(m, m->getResources());
    }
}

}
