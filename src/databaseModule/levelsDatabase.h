#ifndef BENTO_TIME_LEVELSDATABASE_H
#define BENTO_TIME_LEVELSDATABASE_H

#include "generic/databaseModule/databaseInterface.h"
#include "generic/utilityModule/jsonHelper.h"
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

namespace bt::databaseModule {

    struct sObjectType {
        std::string name;
        std::map<std::string, cocos2d::Value::Type> requiredProps;
        std::map<std::string, cocos2d::Value::Type> allowedProps;
    };

    struct sLevelData {
        int id = 0;
        std::string tmxPath;
        std::string objectTypesPath;
        std::string spawnLayer; // name of spawn layer
        std::string backgroundLayer; // name of background layer
        //walls
        std::string wallsLayer;
        std::string wallPropPattern;
    };

    class levelsDatabase : public generic::databaseModule::databaseInterface {
    public:
        levelsDatabase();
        ~levelsDatabase();
        void load(const rapidjson::Document&) override;
        const std::map<int, sLevelData>& getLevels() {
            return levelsDb;
        }
        bool hasLevelById(int id) const;
        const sLevelData& getLevelById(int id);
        const std::map<std::string, sObjectType>& getObjectTypeMap() {
            return objectTypeMap;
        }

        static cocos2d::Value::Type getValueType(const std::string& str);

    private:
        std::map<int, sLevelData> levelsDb;
        std::map<std::string, sObjectType> objectTypeMap;
    };

}// namespace bt::databaseModule


#endif// BENTO_TIME_LEVELSDATABASE_H
