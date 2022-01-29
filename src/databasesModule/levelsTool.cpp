#include "levelsTool.h"
#include "databasesModule/databaseManager.h"
#include "databasesModule/levelsDatabase.h"
#include "generic/debugModule/logManager.h"
#include "generic/utilityModule/stringUtility.h"
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace bt::databasesModule;

std::map<std::string, eLocationWallType> wallTypes = {
    { "right", eLocationWallType::WALL_RIGHT },
    { "left", eLocationWallType::WALL_LEFT },
    { "top", eLocationWallType::WALL_TOP },
    { "down", eLocationWallType::WALL_DOWN }
};

std::map<std::string, eLocationObject> locationObjectTypes = {
    { "levelStart", eLocationObject::LEVEL_START },
    { "levelEnd", eLocationObject::LEVEL_END },
    { "enemy", eLocationObject::ENEMY },
    { "staticWall", eLocationObject::STATIC_WALL }
};

levelsTool::levelsTool() = default;

levelsTool::~levelsTool() = default;

std::vector<sObjectData> levelsTool::getAllObjects(cocos2d::TMXTiledMap* map, const sLevelData& levelData) {
    std::vector<sObjectData> result;
    auto layer = map->getObjectGroup(levelData.spawnLayer);
    auto locationDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    auto objectTypeMap = locationDb->getObjectTypeMap();
    if (parseXMLFile(levelData.objectTypesPath)) {
        for (auto& item : objectProperties) {
            auto typeStr = item.second.properties.find("type");
            bool valid = false;
            if (typeStr != item.second.properties.end() && typeStr->second.getType() == cocos2d::Value::Type::STRING) {
                auto type = typeStr->second.asString();
                item.second.type = locationObjectTypes[type];
                if (objectTypeMap.count(type)) {
                    const auto& dbObj = objectTypeMap[type];
                    valid = true;
                    for (const auto& req : dbObj.requiredProps) {
                        if (!valid)
                            continue;
                        if (!item.second.properties.count(req.first) || req.second != item.second.properties[req.first].getType()) {
                            valid = false;
                        }
                    }
                    if (valid) {
                        for (auto it = item.second.properties.begin(); it != item.second.properties.end();) {
                            if (!dbObj.requiredProps.count(it->first) && !dbObj.allowedProps.count(it->first)) {
                                item.second.properties.erase(it++);
                            } else {
                                ++it;
                            }
                        }
                    }
                }
            }
            if (!valid) {
                item.second.type = eLocationObject::UNDEFINED;
            }
        }
        for (auto item : layer->getObjects()) {
            if (item.getType() == cocos2d::Value::Type::MAP) {
                auto mapVal = item.asValueMap();
                if (mapVal.count("type") && mapVal["type"].getType() == cocos2d::Value::Type::STRING
                    && objectProperties.count(mapVal["type"].asString())) {
                    const auto& obj = objectProperties[mapVal["type"].asString()];
                    if (obj.type != eLocationObject::UNDEFINED) {
                        sObjectData data(obj);
                        data.x = static_cast<int>((mapVal["localX"].asFloat() + mapVal["width"].asFloat() / 2) / map->getTileSize().width);
                        data.y =
                          static_cast<int>((mapVal["localY"].asFloat() + mapVal["height"].asFloat() / 2) / map->getTileSize().height);
                        result.push_back(data);
                    } else {
                        LOG_ERROR(CSTRING_FORMAT("Object have incorrect type '%s'", mapVal["type"].asString().c_str()));
                    }
                }
            }
        }
    } else {
        LOG_ERROR("Can't load level, check database.");
    }
    objectProperties.clear();
    lastObject.clear();
    return result;
}

int levelsTool::getWallCount() {
    return static_cast<int>(wallTypes.size());
}

bool levelsTool::hasWallTypeByString(const std::string& str) {
    return wallTypes.count(str);
}

eLocationWallType levelsTool::getWallTypeByString(const std::string& str) {
    if (hasWallTypeByString(str)) {
        return wallTypes[str];
    }
    return eLocationWallType::UNDEFINED;
}

bool levelsTool::parseXMLFile(const std::string& xmlFilename) {
    cocos2d::SAXParser parser;

    if (!parser.init("UTF-8")) {
        return false;
    }

    parser.setDelegator(this);

    return parser.parse(cocos2d::FileUtils::getInstance()->fullPathForFilename(xmlFilename));
}

void levelsTool::startElement(void* /*ctx*/, const char* name, const char** atts) {
    std::string elementName = name;
    cocos2d::ValueMap attributeDict;
    if (atts && atts[0]) {
        for (int i = 0; atts[i]; i += 2) {
            std::string key = atts[i];
            std::string value = atts[i + 1];
            attributeDict.emplace(key, cocos2d::Value(value));
        }
    }
    if (elementName == "objecttype") {
        lastObject = attributeDict["name"].asString();
        if (lastObject.empty()) {
            LOG_ERROR("ObjectType from xml file not have name properties.");
        }
    } else if (elementName == "property" && !lastObject.empty()) {
        auto variableName = attributeDict["name"].asString();
        auto variableType = attributeDict["type"].asString();
        auto value = attributeDict["default"].asString();
        auto type = levelsDatabase::getValueType(variableType);
        if (type != cocos2d::Value::Type::NONE) {
            objectProperties[lastObject].properties[variableName] = makeValue(type, value);
        }
    } else if (elementName == "objecttypes") {
        lastObject.clear();
    } else {
        LOG_ERROR("ObjectType xml file not have valid properties.");
    }
}

void levelsTool::endElement(void* /*ctx*/, const char* name) {}

void levelsTool::textHandler(void* /*ctx*/, const char* ch, size_t len) {}

cocos2d::Value levelsTool::makeValue(cocos2d::Value::Type type, const std::string& string) {
    cocos2d::Value value;
    switch (type) {
    case cocos2d::Value::Type::BOOLEAN:
        if (string == "true") {
            value = true;
        } else if (string == "false") {
            value = false;
        }
        break;
    case cocos2d::Value::Type::INTEGER:
        value = std::stoi(string);
        break;
    case cocos2d::Value::Type::FLOAT:
        value = std::stof(string);
        break;
    case cocos2d::Value::Type::STRING:
        value = string;
        break;
    default:
        break;
    }
    return value;
}
