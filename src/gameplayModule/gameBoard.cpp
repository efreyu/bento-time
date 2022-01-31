#include "gameBoard.h"
#include "databaseModule/databaseManager.h"
#include "databaseModule/levelsDatabase.h"
#include "gameplayModule/mapDispatcher.h"
#include "generic/audioModule/audioEngineInstance.h"
#include "generic/coreModule/nodes/types/node3d.h"
#include "generic/debugModule/imGuiLayer.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerWidget.h"

using namespace bt::gameplayModule;
using namespace bt::databaseModule;

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

gameBoard::~gameBoard() {
    delete dispatcher;
    dispatcher = nullptr;
}

void gameBoard::loadLevel(int id) {
    auto levelsDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    if (!levelsDb->hasLevelById(id)) {
        LOG_ERROR(cocos2d::StringUtils::format("level %d not found!", id));
        return;
    }
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
        gameFieldNode->setContentSize({ mapSize.width * size.width, mapSize.height * size.height });
    }
    // update timeMap scale and position
    {
        auto width = tiledMap->getTileSize().width;
        auto mapSize = tiledMap->getMapSize();
        auto scale = cocos2d::Director::getInstance()->getVisibleSize().width / (mapSize.width * width);
        gameFieldNode->setScale(scale);
    }

    if (auto objectsLayer = gameFieldNode->findNode("objectsLayer")) {
        objectsLayer->removeFromParentAndCleanup(true);
        delete objectsLayer;
        objectsLayer = nullptr;
    }
    auto objectsLayer = new generic::coreModule::node3d();
    objectsLayer->setName("objectsLayer");
    gameFieldNode->addChild(objectsLayer);
    dispatcher = mapDispatcher::createWithObjectsNode(objectsLayer, tiledMap, id);
}

void gameBoard::attachController(interfaceModule::sControllerEvents* emitter) {
    emitter->onPressed.connect([this](auto direction){
        if (dispatcher)
            dispatcher->move(direction);
            LOG_ERROR(cocos2d::StringUtils::format("%d", direction).c_str());
    });
}
