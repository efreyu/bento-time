#ifndef BENTO_TIME_DATABASEMANAGER_H
#define BENTO_TIME_DATABASEMANAGER_H

#include "generic/databaseModule/databaseInterface.h"
#include "generic/databaseModule/databaseManagerInterface.h"
#include <map>
#include <string>
#include <memory>

#define GET_DATABASE_MANAGER() bt::databaseModule::databaseManager::getInstance()

namespace bt::databaseModule {
    class databaseManager : public generic::databaseModule::databaseManagerInterface {
    public:
        enum class eDatabaseType {
            LEVELS_DB = 0,
            MAP_OBJECTS_DB
        };

        static databaseManager& getInstance();
        static void cleanup();

        void addDatabase(eDatabaseType id, const std::string& value, generic::databaseModule::databaseInterface* db);

        template<typename T>
        T* getDatabase(eDatabaseType key) {
            return getRegisteredDatabase<T>(static_cast<int>(key));
        }

    private:
        databaseManager() = default;
        ~databaseManager() override;
        databaseManager(const databaseManager&) = default;
        databaseManager& operator=(const databaseManager&) = default;
        static void create();
        static void onDeadReference();

        static databaseManager* pInstance;
        static bool destroyed;
    };
}// namespace bt::databaseModule


#endif// BENTO_TIME_DATABASEMANAGER_H
