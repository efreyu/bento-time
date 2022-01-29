#include "gameBoard.h"
#include "generic/debugModule/imGuiLayer.h"
#include "databasesModule/databaseManager.h"
#include "databasesModule/levelsDatabase.h"
#include "generic/audioModule/audioEngineInstance.h"
#include "generic/coreModule/nodes/types/node3d.h"
#include "generic/debugModule/logManager.h"
#include "generic/utilityModule/stringUtility.h"
#include <iterator>

using namespace bt::gameplayModule;
using namespace bt::databasesModule;

gameBoard::gameBoard() {
    this->setName("gameBoard");
    initWithProperties(this->getName());
    gameFieldNode = findNode("gameField");
    if (!gameFieldNode) {
        LOG_ERROR("gameField was not loaded correctly!");
    }

    GET_AUDIO_ENGINE().preload("ui.click");
    GET_AUDIO_ENGINE().preload("music.main");
//    GET_AUDIO_ENGINE().play("music.main");
}

gameBoard::~gameBoard() {}

void gameBoard::loadLevel(int id) {
    auto levelsDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    if (!levelsDb->hasLevelById(id)) {
        LOG_ERROR(CSTRING_FORMAT("level %d not found!", id));
        return;
    }
    currentLevelId = id;
    if (tiledMap) {
        tiledMap->removeFromParentAndCleanup(true);
        delete tiledMap;
        tiledMap = nullptr;
    }
    auto levelData = levelsDb->getLevelById(id);
    tiledMap = new cocos2d::TMXTiledMap();
    tiledMap->initWithTMXFile(levelData.tmxPath);
    gameFieldNode->addChild(tiledMap);
    // update battleField size
    {
        auto size = tiledMap->getTileSize();
        auto mapSize = tiledMap->getMapSize();
        gameFieldNode->setContentSize({mapSize.width * size.width, mapSize.height * size.height});
    }
    // update timeMap scale and position
    {
        auto width = tiledMap->getTileSize().width;
        auto mapSize = tiledMap->getMapSize();
        auto scale = cocos2d::Director::getInstance()->getVisibleSize().width / (mapSize.width * width);
        gameFieldNode->setScale(scale);
        gameFieldNode->setPositionY((cocos2d::Director::getInstance()->getVisibleSize().height / 2) - (gameFieldNode->getContentSize().height * gameFieldNode->getScaleY() / 2));
    }

    reloadWalls(levelData);
    spawnObjects(id);
}

void gameBoard::reloadWalls(const databasesModule::sLevelData& levelData) {
    auto wallsLayerName = levelData.wallsLayer;
    auto wallsPropPattern = levelData.wallPropPattern;
    wallOnMap.clear();
    if (auto wallsLayer = tiledMap->getLayer(wallsLayerName)) {
        const auto& layerSize = wallsLayer->getLayerSize();
        auto wallsCount = levelTool.getWallCount();
        for (auto x = 0; x < static_cast<int>(layerSize.width); ++x) {
            for (auto y = 0; y < static_cast<int>(layerSize.height); ++y) {
                auto gid = wallsLayer->getTileGIDAt({ static_cast<float>(x), static_cast<float>(y) });
                auto prop = tiledMap->getPropertiesForGID(gid);
                if (prop.getType() == cocos2d::Value::Type::MAP) {
                    auto val = prop.asValueMap();
                    for (int i = 0; i < wallsCount; ++i) {
                        auto key = STRING_FORMAT(wallsPropPattern.c_str(), i);
                        if (val.count(key)) {
                            auto& item = val[key];
                            if (item.getType() == cocos2d::Value::Type::STRING) {
                                auto wallType = levelTool.getWallTypeByString(item.asString());
                                if (wallType != eLocationWallType::UNDEFINED) {
                                    wallOnMap[x][y].emplace_back(static_cast<databasesModule::eLocationWallType>(wallType));
                                }
                            }
                        } else {
                            i = wallsCount;
                        }
                    }
                }
            }
        }
        wallsLayer->setVisible(false);

    } else {
        LOG_ERROR(CSTRING_FORMAT("wallsLayerName %s not found!", wallsLayerName.c_str()));
        return;
    }
}

void gameBoard::spawnObjects(int id) {
    for (auto& item : tilesOnMap) {
        for (auto& _item : item.second) {
            CC_SAFE_RELEASE_NULL(_item.second);
            CC_SAFE_DELETE(_item.second);
        }
    }
    tilesOnMap.clear();
    auto locationDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    auto characterDb = GET_DATABASE_MANAGER().getDatabase<mapObjectsDatabase>(databaseManager::eDatabaseType::MAP_OBJECTS_DB);
    if (!locationDb->hasLevelById(id)) {
        LOG_ERROR(CSTRING_FORMAT("level %d not found!", id));
        return;
    }
    auto levelData = locationDb->getLevelById(id);
    auto layer = tiledMap->getLayer(levelData.backgroundLayer);
    auto allSpawnPos = levelTool.getAllObjects(tiledMap, levelData);
    if (objectsLayer) {
        objectsLayer->removeFromParentAndCleanup(true);
        delete objectsLayer;
        objectsLayer = nullptr;
    }
    objectsLayer = new generic::coreModule::node3d();
    objectsLayer->setName("objectsLayer");
    gameFieldNode->addChild(objectsLayer);
    for (const auto& item : allSpawnPos) {
        auto tile = layer->getTileAt({static_cast<float>(item.x), static_cast<float>(item.y)});
        if (!tile) {
            LOG_ERROR(CSTRING_FORMAT("Can't find element on layer by pos %d, %d", item.x, item.y));
            continue;
        }
        objectOnMap* unitObject = nullptr;
        switch (item.type) {
        case eLocationObject::FOOD:
        /*case eLocationObject::LEVEL_START: {
            if (item.type == eLocationObject::LEVEL_START) {
                unitObject = new playerObject();
                auto player = dynamic_cast<playerObject*>(unitObject);
                unitObject->type = databasesModule::eObjectType::HERO;
                // todo change id after testing, need setup id when we run scene
                unitObject->objectId = 20011;
                if (unitObject->objectId == 0 || !characterDb->hasMapObjectById(unitObject->objectId)) {
                    LOG_ERROR(CSTRING_FORMAT("Character with id '%d' not found!", unitObject->objectId));
                    CC_SAFE_DELETE(unitObject);
                    return;
                }
                unitObject->setName("playerObject");
                auto characterData = characterDb->getMapObjectById(unitObject->objectId);
                player->initWithData(characterData);
            } else {
                unitObject = new enemyObject();
                auto enemy = dynamic_cast<enemyObject*>(unitObject);
                unitObject->type = databasesModule::eObjectType::ENEMY;
                auto characterId = item.properties.find("id");
                if (characterId != item.properties.end() && characterId->second.getType() == cocos2d::Value::Type::INTEGER) {
                    unitObject->objectId = characterId->second.asInt();
                }
                if (unitObject->objectId == 0 || !characterDb->hasMapObjectById(unitObject->objectId)) {
                    LOG_ERROR(CSTRING_FORMAT("Character with id '%d' not found!", unitObject->objectId));
                    CC_SAFE_DELETE(unitObject);
                    return;
                }
                unitObject->setName("enemyObject");
                auto characterData = characterDb->getMapObjectById(unitObject->objectId);
                enemy->initWithData(characterData);
            }
            objectsLayer->addChild(unitObject);
            unitObject->setPosition(tile->getPosition());
            unitObject->setContentSize(tiledMap->getTileSize());

            tilesOnMap[item.x][item.y] = unitObject;
        } break;*/
        case eLocationObject::LEVEL_END:
        case eLocationObject::UNDEFINED:
            break;
        }
    }
}
