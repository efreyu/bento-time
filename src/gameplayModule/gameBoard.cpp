#include "gameBoard.h"
#include "databaseModule/databaseManager.h"
#include "databaseModule/levelsDatabase.h"
#include "gameplayModule/mapDispatcher.h"
#include "generic/audioModule/audioEngineInstance.h"
#include "generic/coreModule/nodes/types/node3d.h"
#include "generic/debugModule/imGuiLayer.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerStickWidget.h"

using namespace bt::gameplayModule;
using namespace bt::databaseModule;

gameBoard::gameBoard() {
    this->setName("gameBoard");
    initWithProperties(this->getName());
    gameFieldNode = findNode("gameField");
    if (!gameFieldNode) {
        LOG_ERROR("gameField was not loaded correctly!");
    }
    movesLabel = dynamic_cast<cocos2d::Label*>(findNode("movesLabel"));
    loadSettings();

    GET_AUDIO_ENGINE().preload("ui.click");
    GET_AUDIO_ENGINE().preload("music.main");
    //    GET_AUDIO_ENGINE().play("music.main");
    removeJsonData();
}

void gameBoard::loadSettings() {
    if (!hasPropertyObject("settings"))
        return;
    const auto& json = getPropertyObject("settings");
    if (json.HasMember("movesPattern") && json["movesPattern"].IsString()) {
        settings.movesPattern = json["movesPattern"].GetString();
    }
    if (json.HasMember("fadeDuration") && json["fadeDuration"].IsNumber()) {
        settings.fadeDuration = json["fadeDuration"].GetFloat();
    }
    if (json.HasMember("delayDuration") && json["delayDuration"].IsNumber()) {
        settings.delayDuration = json["delayDuration"].GetFloat();
    }
    if (json.HasMember("gridSizeX") && json.HasMember("gridSizeY")) {
        settings.gridSize = cocos2d::Size(json["gridSizeX"].GetFloat(), json["gridSizeY"].GetFloat());
    }
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
    currentLevel = id;
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
        auto scale = cocos2d::Director::getInstance()->getVisibleSize().width / (mapSize.width * width - width);
        gameFieldNode->setScale(scale);
        gameFieldNode->setPositionY((width / 2) * scale);
    }

    if (auto objectsLayer = gameFieldNode->findNode("objectsLayer")) {
        objectsLayer->removeFromParentAndCleanup(true);
        delete objectsLayer;
        objectsLayer = nullptr;
    }
    auto objectsLayer = new generic::coreModule::node3d();
    objectsLayer->setName("objectsLayer");
    gameFieldNode->addChild(objectsLayer);
    boardBlocked = true;
    runShowAnimation([this](){
        boardBlocked = false;
    });
    dispatcher = mapDispatcher::createWithObjectsNode(objectsLayer, tiledMap, id);
    dispatcher->getEmitter()->onWin.connect([this](){
        boardBlocked = true;
        runHideAnimation([this](){
            loadLevel(currentLevel + 1);
        });
    });
    movesCnt = 0;
    updateMovesScore();
    dispatcher->getEmitter()->onPlayerMove.connect([this](){
        movesCnt++;
        updateMovesScore();
    });
}

void gameBoard::attachController(interfaceModule::sControllerStickEvents* emitter) {
    emitter->onPressed.connect([this](auto direction){
        if (!boardBlocked && dispatcher)
            dispatcher->move(direction);
    });
}

void gameBoard::updateMovesScore() {
    if (movesLabel) {
        if (!settings.movesPattern.empty()) {
            movesLabel->setString(cocos2d::StringUtils::format(settings.movesPattern.c_str(), movesCnt));
        } else {
            movesLabel->setString(std::to_string(movesCnt));
        }
    }
}

void gameBoard::runShowAnimation(const std::function<void()>& clb) {
    auto action = cocos2d::FadeOutBLTiles::create(settings.fadeDuration, settings.gridSize);
    auto show = action->reverse();
    auto clbDone = cocos2d::CallFunc::create([clb](){
        if (clb)
            clb();
    });
    CC_SAFE_RETAIN(action);
    runAction(cocos2d::Sequence::create(show, clbDone, nullptr));
}

void gameBoard::runHideAnimation(const std::function<void()>& clb) {
    auto fadeout = cocos2d::FadeOutBLTiles::create(settings.fadeDuration, settings.gridSize);
    auto delay = cocos2d::DelayTime::create(settings.delayDuration);
    auto clbDone = cocos2d::CallFunc::create([clb](){
        if (clb)
            clb();
    });
    runAction(cocos2d::Sequence::create(fadeout, delay, clbDone, nullptr));
}
