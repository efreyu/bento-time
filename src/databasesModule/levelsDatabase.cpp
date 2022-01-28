#include "levelsDatabase.h"
#include "generic/utilityModule/stringUtility.h"
#include <cassert>
#include <map>
#include <string>
#include <tuple>

using namespace bt::databasesModule;

std::map<std::string, cocos2d::Value::Type> typesMap = {
  {"int", cocos2d::Value::Type::INTEGER},
  {"double", cocos2d::Value::Type::DOUBLE},
  {"float", cocos2d::Value::Type::FLOAT},
  {"string", cocos2d::Value::Type::STRING},
  {"bool", cocos2d::Value::Type::BOOLEAN}
};

levelsDatabase::levelsDatabase() = default;

levelsDatabase::~levelsDatabase() {
    levelsDb.clear();
}

void levelsDatabase::load(const rapidjson::Document& data) {
    if (getPath().empty()) {
        LOG_ERROR("Path is empty!");
        return;
    }
    if (!data.IsObject()) {
        LOG_ERROR(CSTRING_FORMAT("File from path %s is not object!", getPath().c_str()));
        return;
    }
    auto levels = data.FindMember("levels");
    if (levels != data.MemberEnd() && levels->value.IsArray()) {
        auto array = levels->value.GetArray();
        for (auto levelIt = array.Begin(); levelIt != array.End(); ++levelIt) {
            if (!levelIt->IsObject())
                continue;
            auto id = levelIt->FindMember("id");
            auto tmxPath = levelIt->FindMember("tmxPath");
            auto objectTypes = levelIt->FindMember("objectTypes");
            if (id == levelIt->MemberEnd() || tmxPath == levelIt->MemberEnd() || objectTypes == levelIt->MemberEnd()) {
                continue;
            }
            if (!id->value.IsInt() || !tmxPath->value.IsString() || !objectTypes->value.IsString()) {
                continue;
            }
            auto itemId = id->value.GetInt();
            sLevelData levelData;
            levelData.id = itemId;
            levelData.tmxPath = tmxPath->value.GetString();
            levelData.objectTypesPath = objectTypes->value.GetString();
            //spawn layer
            auto spawnLayerIt = levelIt->FindMember("spawnLayer");
            if (spawnLayerIt != levelIt->MemberEnd() && spawnLayerIt->value.IsString()) {
                levelData.spawnLayer = spawnLayerIt->value.GetString();
            }
            //background layer
            auto backgroundLayerIt = levelIt->FindMember("backgroundLayer");
            if (backgroundLayerIt != levelIt->MemberEnd() && backgroundLayerIt->value.IsString()) {
                levelData.backgroundLayer = backgroundLayerIt->value.GetString();
            }
            //wall layer
            auto wallLayerIt = levelIt->FindMember("wallsLayer");
            if (wallLayerIt != levelIt->MemberEnd() && wallLayerIt->value.IsString()) {
                levelData.wallsLayer = wallLayerIt->value.GetString();
            }
            auto wallPatternIt = levelIt->FindMember("wallPropPattern");
            if (wallPatternIt != levelIt->MemberEnd() && wallPatternIt->value.IsString()) {
                levelData.wallPropPattern = wallPatternIt->value.GetString();
            }
            levelsDb.insert({ itemId, levelData });
        }
    }

    auto typesIt = data.FindMember("objectsTypes");
    if (typesIt != data.MemberEnd() && typesIt->value.IsObject()) {
        auto objectsTypes = typesIt->value.GetObject();
        for (auto type = objectsTypes.MemberBegin(); type != objectsTypes.MemberEnd(); ++type) {
            if (type->name.IsString() && type->value.IsObject()) {
                auto name = type->name.GetString();
                sObjectType params;
                auto required = type->value.FindMember("required");
                if (required != type->value.MemberEnd() && required->value.IsObject()) {
                    auto requireObj = required->value.GetObject();
                    for (auto it = requireObj.MemberBegin(); it != requireObj.MemberEnd(); ++it) {
                        if (it->name.IsString() && it->value.IsString() && typesMap.count(it->value.GetString())) {
                            params.requiredProps[it->name.GetString()] = typesMap[it->value.GetString()];
                        }
                    }
                }
                auto allowed = type->value.FindMember("allowed");
                if (allowed != type->value.MemberEnd() && allowed->value.IsObject()) {
                    auto allowObj = allowed->value.GetObject();
                    for (auto it = allowObj.MemberBegin(); it != allowObj.MemberEnd(); ++it) {
                        if (it->name.IsString() && it->value.IsString() && typesMap.count(it->value.GetString())) {
                            params.allowedProps[it->name.GetString()] = typesMap[it->value.GetString()];
                        }
                    }
                }
                objectTypeMap.insert({ name, params });
            }
        }
    } else {
        LOG_ERROR("File not have 'objectsTypes' property!");
    }
}

const sLevelData& levelsDatabase::getLevelById(int id) {
    assert(hasLevelById(id) && "Can't find level, use method 'hasLevelById' first!");
    return levelsDb.at(id);
}

bool levelsDatabase::hasLevelById(int id) const {
    return levelsDb.count(id);
}
cocos2d::Value::Type levelsDatabase::getValueType(const std::string& str) {
    if (typesMap.count(str)) {
        return typesMap[str];
    }
    return cocos2d::Value::Type::NONE;
}
