#include "databaseManager.h"
#include "cocos2d.h"

using namespace bt::databaseModule;

databaseManager* databaseManager::pInstance = nullptr;
bool databaseManager::destroyed = false;

databaseManager& databaseManager::getInstance() {
    if (!pInstance) {
        if (destroyed) {
            onDeadReference();
        } else {
            create();
        }
    }
    return *pInstance;
}

void databaseManager::cleanup() {
    destroyed = true;
    pInstance = nullptr;
}

void databaseManager::create() {
    static databaseManager instance;
    pInstance = &instance;
}

void databaseManager::onDeadReference() {
    CCASSERT(false, "Founded dead reference!");
}

void databaseManager::addDatabase(databaseManager::eDatabaseType id,
                                  const std::string& value,
                                  const std::shared_ptr<generic::databaseModule::databaseInterface>& dbPtr) {
    auto type = static_cast<int>(id);
    registerDatabase({ type, value }, dbPtr);
}
