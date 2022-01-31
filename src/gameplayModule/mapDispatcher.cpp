#include "mapDispatcher.h"
#include "databaseModule/databaseManager.h"
#include "databaseModule/levelsDatabase.h"
#include "gameplayModule/bentoNode.h"
#include "generic/coreModule/nodes/types/node3d.h"
#include "generic/debugModule/logManager.h"


using namespace bt::gameplayModule;
using namespace bt::databaseModule;

mapDispatcher* mapDispatcher::createWithObjectsNode(cocos2d::Node* node, cocos2d::TMXTiledMap* tiled, int levelId) {
    auto levelsDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    if (!levelsDb->hasLevelById(levelId)) {
        LOG_ERROR(cocos2d::StringUtils::format("level %d not found!", levelId));
        return nullptr;
    }
    auto levelData = levelsDb->getLevelById(levelId);
    auto dispatcher = new mapDispatcher();
    dispatcher->setObjectNode(node);
    dispatcher->loadWalls(levelData, tiled);
    dispatcher->spawnObjects(levelData, tiled);
    return dispatcher;
}

mapDispatcher::~mapDispatcher() {
    for (auto& [_, list] : cells) {
        for (auto& [_y, cell] : list) {
            CC_SAFE_RELEASE_NULL(cell.first);
            CC_SAFE_DELETE(cell.first);
        }
    }
    cells.clear();
}

bool mapDispatcher::move(eMoveDirection direction) {
    return false;
}

void mapDispatcher::loadWalls(const databaseModule::sLevelData& levelData, cocos2d::TMXTiledMap* tiled) {
    auto wallsLayerName = levelData.wallsLayer;
    auto wallsPropPattern = levelData.wallPropPattern;
    auto wallsLayer = tiled->getLayer(wallsLayerName);
    if (!wallsLayer) {
        LOG_ERROR(CSTRING_FORMAT("wallsLayerName %s not found!", wallsLayerName.c_str()));
        return;
    }
    const auto& layerSize = wallsLayer->getLayerSize();
    const auto wallsCount = levelTool.getWallCount();
    std::vector<std::string> wallsIds;
    for (int i = 0; i < wallsCount; ++i) {
        wallsIds.push_back(cocos2d::StringUtils::format(wallsPropPattern.c_str(), i));
    }
    for (auto x = 0; x < static_cast<int>(layerSize.width); ++x) {
        for (auto y = 0; y < static_cast<int>(layerSize.height); ++y) {
            auto gid = wallsLayer->getTileGIDAt({ static_cast<float>(x), static_cast<float>(y) });
            auto prop = tiled->getPropertiesForGID(gid);
            if (prop.getType() != cocos2d::Value::Type::MAP) {
                continue;
            }
            auto val = prop.asValueMap();
            for (const auto& key : wallsIds) {
                if (val.count(key)) {
                    const auto& item = val[key];
                    if (item.getType() == cocos2d::Value::Type::STRING) {
                        auto wallType = levelTool.getWallTypeByString(item.asString());
                        if (wallType != eLocationWallType::UNDEFINED) {
                            cells[x][y].second.insert(wallType);
                        }
                    }
                }
            }
        }
    }
    wallsLayer->setVisible(false);
}

void mapDispatcher::spawnObjects(const databaseModule::sLevelData& levelData, cocos2d::TMXTiledMap* tiled) {
    if (!objectsNode) {
        LOG_ERROR("ObjectNode is not inited.");
        return;
    }
    auto characterDb = GET_DATABASE_MANAGER().getDatabase<mapObjectsDatabase>(databaseManager::eDatabaseType::MAP_OBJECTS_DB);
    auto layer = tiled->getLayer(levelData.backgroundLayer);
    auto allSpawnPos = levelTool.getAllObjects(tiled, levelData);

    for (const auto& item : allSpawnPos) {
        auto tile = layer->getTileAt({ static_cast<float>(item.x), static_cast<float>(item.y) });
        if (!tile) {
            LOG_ERROR(cocos2d::StringUtils::format("Can't find element on layer by pos %d, %d", item.x, item.y));
            continue;
        }
        switch (item.type) {
        case eLocationObject::FOOD:
        case eLocationObject::LEVEL_START: {
            auto id = 0;
            if (item.properties.count("id") && item.properties.at("id").getType() == cocos2d::Value::Type::INTEGER) {
                id = item.properties.at("id").asInt();
            } else {
                LOG_ERROR("Object don't have 'id'");
                continue;
            }
            auto unitObject = new bentoNode();
            unitObject->type = databaseModule::eMapObjectType::HERO;
            unitObject->objectId = id;
            if (!characterDb->hasMapObjectById(unitObject->objectId)) {
                LOG_ERROR(CSTRING_FORMAT("Character with id '%d' not found!", unitObject->objectId));
                CC_SAFE_DELETE(unitObject);
                continue;
            }
            auto characterData = characterDb->getMapObjectById(unitObject->objectId);
            unitObject->initWithData(characterData);
            objectsNode->addChild(unitObject);
            unitObject->setPosition(tile->getPosition());
            unitObject->setContentSize(tiled->getTileSize());

            cells[item.x][item.y].first = unitObject;
        } break;
        default:
            break;
        }
    }
}