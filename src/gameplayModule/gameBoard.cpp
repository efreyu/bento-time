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
