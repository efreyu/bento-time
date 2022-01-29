#include "mapObjectsDatabase.h"
#include "generic/debugModule/logManager.h"
#include "generic/utilityModule/stringUtility.h"
#include <cassert>

using namespace bt::databaseModule;

const std::map<std::string, eNodeType> mapNodeTypes = {
    {"sprite", eNodeType::SPRITE},
    {"aseprite", eNodeType::ASEPRITE}
};

mapObjectsDatabase::mapObjectsDatabase() = default;

mapObjectsDatabase::~mapObjectsDatabase() = default;

void mapObjectsDatabase::load(const rapidjson::Document& json) {
    if (getPath().empty()) {
        LOG_ERROR("Path is empty!");
        return;
    }
    if (!json.IsObject()) {
        LOG_ERROR(CSTRING_FORMAT("File from path '%s' is not object!", getPath().c_str()));
        return;
    }
    auto mObject = json.FindMember("mapObjects");
    if (mObject == json.MemberEnd() || !mObject->value.IsArray()) {
        LOG_ERROR("Can't load mapObjects database!");
        return;
    }
    for (auto mObjectIt = mObject->value.Begin(); mObjectIt != mObject->value.End(); ++mObjectIt) {
        if (!mObjectIt->IsObject())
            continue;
        auto obj = mObjectIt->GetObject();
        sMapObjectsData item;
        if (item.load(obj)) {
            mapObjectsDb.insert({ item.id, item });
        }
    }
}
const sMapObjectsData& mapObjectsDatabase::getMapObjectById(int id) {
    assert(hasMapObjectById(id) && "Can't find mapObjects, use method 'hasMapObjectById' first!");
    return mapObjectsDb.at(id);
}

bool mapObjectsDatabase::hasMapObjectById(int id) const {
    return mapObjectsDb.count(id);
}

bool sMapObjectsData::load(const rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstObject& object) {
    if (object.HasMember("id") && object["id"].IsNumber()) {
        id = object["id"].GetInt();
    } else {
        return false;
    }
    if (object.HasMember("propertyPath") && object["propertyPath"].IsString())
        propertyPath = object["propertyPath"].GetString();
    if (object.HasMember("objectType") && object["objectType"].IsString()) {
        objectType = getMapObjectTypeByString(object["objectType"].GetString());
    }
    if (object.HasMember("nodeType") && object["nodeType"].IsString() && mapNodeTypes.count(object["nodeType"].GetString())) {
        nodeType = mapNodeTypes.at(object["nodeType"].GetString());
    }
    return true;
}
