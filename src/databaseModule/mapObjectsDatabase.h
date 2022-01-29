#ifndef BENTO_TIME_MAPOBJECTSDATABASE_H
#define BENTO_TIME_MAPOBJECTSDATABASE_H

#include "cocos2d.h"
#include "databaseModule/mapObjectTypes.h"
#include "generic/databaseModule/databaseInterface.h"
#include "generic/utilityModule/jsonHelper.h"
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace bt::databaseModule {

    enum class eNodeType { SPRITE = 0, ASEPRITE };

    struct sMapObjectsData {
        int id;
        std::string propertyPath;
        eMapObjectType objectType = eMapObjectType::UNDEFINED;
        eNodeType nodeType = eNodeType::SPRITE;
        bool load(const rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstObject&);
    };

    class mapObjectsDatabase : public generic::databaseModule::databaseInterface {
    public:
        mapObjectsDatabase();
        ~mapObjectsDatabase() override;
        void load(const rapidjson::Document&) override;
        const std::map<int, sMapObjectsData>& getMapObjects() {
            return mapObjectsDb;
        }
        const sMapObjectsData& getMapObjectById(int);
        bool hasMapObjectById(int) const;

    private:
        std::map<int, sMapObjectsData> mapObjectsDb;
    };
}// namespace bt::databaseModule


#endif// BENTO_TIME_MAPOBJECTSDATABASE_H
