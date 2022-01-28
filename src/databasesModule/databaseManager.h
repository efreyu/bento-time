#ifndef BENTO_TIME_DATABASEMANAGER_H
#define BENTO_TIME_DATABASEMANAGER_H

#include "generic/databaseModule/databaseInterface.h"
#include "generic/databaseModule/databaseManagerInterface.h"
#include <map>
#include <string>

#define GET_DATABASE_MANAGER() bt::databasesModule::databaseManager::getInstance()

namespace bt::databasesModule {
    class databaseManager : public generic::databaseModule::databaseManagerInterface {
    public:
        enum class eDatabaseList {
            LOCATIONS_DB = 0,
            MAP_OBJECTS_DB
        };
        databaseManager();
        ~databaseManager() override;

        static databaseManager& getInstance();
        void cleanup() override;

        void addDatabase(eDatabaseList id, const std::string& value, generic::databaseModule::databaseInterface* db);

        template<typename T>
        T* getDatabase(eDatabaseList key) {
            return getRegisteredDatabase<T>(static_cast<int>(key));
        }
    };
}// namespace bt::databasesModule


#endif// BENTO_TIME_DATABASEMANAGER_H
