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
    for (auto& cell : cells) {
        CC_SAFE_RELEASE_NULL(cell->node);
        CC_SAFE_DELETE(cell);
    }
    cells.clear();
}

bool mapDispatcher::move(eMoveDirection direction) {
    auto player = std::find_if(cells.begin(), cells.end(), [](mapCell* c) {
        return c->node && c->node->type == databaseModule::eMapObjectType::HERO;
    });
    if (player == cells.end())
        return false;
    //todo move player cell
//    auto playerPos = std::make_pair((*player)->x, (*player)->y);
    getNextCell(direction, (*player)->pos);
    auto nextCoordinates = getNextPosition(direction, (*player)->node->getPosition(), (*player)->node->getContentSize(), (*player)->node->getScaleX());
    auto moveAction = cocos2d::MoveTo::create(0.2f, nextCoordinates);
    (*player)->node->runAction(moveAction);
    //todo find connected cell and move them too

    return true;
}

void mapDispatcher::loadWalls(const databaseModule::sLevelData& levelData, cocos2d::TMXTiledMap* tiled) {
    auto wallsLayerName = levelData.wallsLayer;
    auto wallsPropPattern = levelData.wallPropPattern;
    auto wallsLayer = tiled->getLayer(wallsLayerName);
    if (!wallsLayer) {
        LOG_ERROR(cocos2d::StringUtils::format("wallsLayerName %s not found!", wallsLayerName.c_str()));
        return;
    }
    const auto& layerSize = wallsLayer->getLayerSize();
    mapSize = std::make_pair(static_cast<unsigned>(layerSize.width), static_cast<unsigned>(layerSize.height));
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
                            walls[x][y].insert(wallType);
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
            if (item.type == eLocationObject::FOOD) {
                unitObject->type = databaseModule::eMapObjectType::FOOD;
            } else {
                unitObject->type = databaseModule::eMapObjectType::HERO;
            }
            unitObject->objectId = id;
            if (!characterDb->hasMapObjectById(unitObject->objectId)) {
                LOG_ERROR(cocos2d::StringUtils::format("Character with id '%d' not found!", unitObject->objectId));
                CC_SAFE_DELETE(unitObject);
                continue;
            }
            auto characterData = characterDb->getMapObjectById(unitObject->objectId);
            unitObject->initWithData(characterData);
            objectsNode->addChild(unitObject);
            unitObject->setPosition(tile->getPosition());
            unitObject->setContentSize(tiled->getTileSize());
            cells.push_back(new mapCell(unitObject, { item.x, item.y }));
        } break;
        default:
            break;
        }
    }
}

void mapDispatcher::getNextCell(eMoveDirection direction, std::pair<int, int>& nextPosition) {
    switch (direction) {
    case eMoveDirection::UP: nextPosition.second += 1; break;
    case eMoveDirection::DOWN: nextPosition.second -= 1; break;
    case eMoveDirection::RIGHT: nextPosition.first -= 1; break;
    case eMoveDirection::LEFT: nextPosition.first += 1; break;
    default: break;
    }
}

cocos2d::Vec2 mapDispatcher::getNextPosition(eMoveDirection direction, cocos2d::Vec2 pos, const cocos2d::Size& size, float scale) {
    switch (direction) {
    case eMoveDirection::UP: pos.y += size.height * scale; break;
    case eMoveDirection::DOWN: pos.y -= size.height * scale; break;
    case eMoveDirection::RIGHT: pos.x += size.width * scale; break;
    case eMoveDirection::LEFT: pos.x -= size.width * scale; break;
    default: break;
    }
    return pos;
}
