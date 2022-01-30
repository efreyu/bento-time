#ifndef BENTO_TIME_LEVELSTOOL_H
#define BENTO_TIME_LEVELSTOOL_H

#include "base/CCMap.h"
#include "base/CCValue.h"
#include "base/CCVector.h"
#include "databaseModule/levelsDatabase.h"
#include "platform/CCSAXParser.h"
#include <map>
#include <string>

namespace bt::databaseModule {

    enum class eLocationWallType {
        UNDEFINED = 0,
        WALL_TOP,
        WALL_DOWN,
        WALL_LEFT,
        WALL_RIGHT
    };

    enum class eLocationObject {
        UNDEFINED = 0,
        LEVEL_START,
        LEVEL_END,
        FOOD
    };

    struct sObjectData {
        int x;
        int y;
        eLocationObject type = eLocationObject::UNDEFINED;
        std::map<std::string, cocos2d::Value> properties;

        sObjectData() = default;
        sObjectData(const sObjectData& n){
            x = n.x;
            y = n.y;
            type = n.type;
            properties = n.properties;
        }
    };

    class levelsTool : public cocos2d::SAXDelegator {
    public:
        levelsTool();
        ~levelsTool() override;

        std::vector<sObjectData> getAllObjects(cocos2d::TMXTiledMap* map, const sLevelData&);
        int getWallCount() const;
        bool hasWallTypeByString(const std::string& str) const;
        eLocationWallType getWallTypeByString(const std::string& str);

    private:
        /** initializes parsing of an XML file, either a tmx (Map) file or tsx (Tileset) file */
        bool parseXMLFile(const std::string& xmlFilename);
        // implement pure virtual methods of SAXDelegator
        void startElement(void *ctx, const char *name, const char **atts) override;
        void endElement(void *ctx, const char *name) override;
        void textHandler(void *ctx, const char *ch, size_t len) override;
        cocos2d::Value makeValue(cocos2d::Value::Type, const std::string&);

        std::map<std::string, sObjectData> objectProperties;
        std::string lastObject;
    };

}


#endif// BENTO_TIME_LEVELSTOOL_H
