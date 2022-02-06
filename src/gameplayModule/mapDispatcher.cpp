#include "mapDispatcher.h"
#include "databaseModule/databaseManager.h"
#include "databaseModule/levelsDatabase.h"
#include "gameplayModule/bentoNode.h"
#include "generic/coreModule/nodes/types/node3d.h"
#include "generic/debugModule/logManager.h"


using namespace bt::gameplayModule;
using namespace bt::databaseModule;

const float animDelay = 0.13f;

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
    for (auto& [_, col] : cells) {
        for (auto& [_, cell] : col) {
            CC_SAFE_RELEASE_NULL(cell->node);
            CC_SAFE_DELETE(cell);
        }
    }
    cells.clear();
}

bool mapDispatcher::move(eMoveDirection direction) {
    std::for_each(playerCells.begin(), playerCells.end(), [this, direction](mapCell* p){
        auto connectedCells = std::make_shared<mapCellItems>(p);
        std::vector<mapCell*> exclude = { p };
        getClosestCells(connectedCells, exclude, direction);
        if (connectedCells->move(direction)) {
            getEmitter()->onPlayerMove.emit();
            auto tempCells = std::make_shared<mapCellItems>(p);
            std::vector<mapCell*> sleepBlocks;
            getClosestCells(tempCells, sleepBlocks, direction);
            std::for_each(sleepBlocks.begin(), sleepBlocks.end(), [this, p](mapCell* c){
                auto delay = cocos2d::DelayTime::create(animDelay);
                auto clb = cocos2d::CallFunc::create([bento = c->node](){
                    bento->setAnimation(eBentoAnimation::IDLE);
                });
                c->node->runAction(cocos2d::Sequence::create(delay, clb, nullptr));
            });
        }
        connectedCells->reset();
        auto it = std::find_if(exitCells.begin(), exitCells.end(), [this](const std::pair<int, int>& p){
            return !getCellByPos(p);
        });
        if (it == exitCells.end()) {
            p->node->setAnimation(eBentoAnimation::WIN);
            getEmitter()->onWin.emit();
            std::vector<mapCell*> sleepBlocks;
            auto tempCells = std::make_shared<mapCellItems>(p);
            getClosestCells(tempCells, sleepBlocks, direction);
            std::for_each(sleepBlocks.begin(), sleepBlocks.end(), [](mapCell* c){
                c->node->setAnimation(eBentoAnimation::WIN);
            });
        }
    });

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
    mapSize = std::make_pair(static_cast<int>(layerSize.width), static_cast<int>(layerSize.height));
    const auto wallsCount = levelTool.getWallCount();
    std::vector<std::string> wallsIds;
    for (int i = 0; i < wallsCount; ++i) {
        wallsIds.push_back(cocos2d::StringUtils::format(wallsPropPattern.c_str(), i));
    }
    for (auto x = 0; x < mapSize.first; ++x) {
        for (auto y = 0; y < mapSize.second; ++y) {
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
    auto mapObjectsDb = GET_DATABASE_MANAGER().getDatabase<mapObjectsDatabase>(databaseManager::eDatabaseType::MAP_OBJECTS_DB);
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
            if (!mapObjectsDb->hasMapObjectById(unitObject->objectId)) {
                LOG_ERROR(cocos2d::StringUtils::format("Character with id '%d' not found!", unitObject->objectId));
                CC_SAFE_DELETE(unitObject);
                continue;
            }
            auto characterData = mapObjectsDb->getMapObjectById(unitObject->objectId);
            unitObject->initWithData(characterData);
            objectsNode->addChild(unitObject);
            unitObject->setPosition(tile->getPosition());
            unitObject->setContentSize(tiled->getTileSize());
            if (!getCellByPos({ item.x, item.y })) {
                auto cell = new mapCell(unitObject, { item.x, item.y });
                cell->registerMoveClb([this](auto dir, auto nextPos){
                    return moveCell(dir, nextPos);
                });
                cells[item.x][item.y] = cell;
                if (unitObject->type == databaseModule::eMapObjectType::HERO) {
                    unitObject->setAnimation(eBentoAnimation::IDLE);
                    playerCells.push_back(cell);
                }
            } else {
                LOG_ERROR(cocos2d::StringUtils::format("Node '%d' has position same as the placed one.", unitObject->objectId));
                CC_SAFE_DELETE(unitObject);
            }
        } break;
        case eLocationObject::LEVEL_END:
            exitCells.emplace_back(item.x, item.y);
        default:
            break;
        }
    }
}

std::pair<int, int> mapDispatcher::getNextIndex(eMoveDirection direction, const std::pair<int, int>& nextPosition) {
    auto result = nextPosition;
    switch (direction) {
    case eMoveDirection::UP: result.second -= 1; break;
    case eMoveDirection::DOWN: result.second += 1; break;
    case eMoveDirection::RIGHT: result.first += 1; break;
    case eMoveDirection::LEFT: result.first -= 1; break;
    default: break;
    }
    return result;
}

bool mapDispatcher::hasCollision(eMoveDirection direction, const std::pair<int, int>& currentPos) {
    std::set<eLocationWallType> wallsCurrent;
    if (walls.count(currentPos.first) && walls[currentPos.first].count(currentPos.second)) {
        wallsCurrent = walls[currentPos.first][currentPos.second];
    }
    auto nextPosition = getNextIndex(direction, currentPos);
    auto currentType = eLocationWallType::UNDEFINED;
    auto nextType = eLocationWallType::UNDEFINED;
    switch (direction) {
    case eMoveDirection::UNDEFINED: {
        return true;
    } break;
    case eMoveDirection::UP: {
        currentType = eLocationWallType::WALL_TOP;
        nextType = eLocationWallType::WALL_DOWN;
    } break;
    case eMoveDirection::DOWN: {
        currentType = eLocationWallType::WALL_DOWN;
        nextType = eLocationWallType::WALL_TOP;
    } break;
    case eMoveDirection::RIGHT: {
        currentType = eLocationWallType::WALL_RIGHT;
        nextType = eLocationWallType::WALL_LEFT;
    } break;
    case eMoveDirection::LEFT: {
        currentType = eLocationWallType::WALL_LEFT;
        nextType = eLocationWallType::WALL_RIGHT;
    } break;
    }
    if (wallsCurrent.count(currentType))
        return true;
    if (walls.count(nextPosition.first) && walls[nextPosition.first].count(nextPosition.second)) {
        auto wallsNext = walls[nextPosition.first][nextPosition.second];
        if (wallsNext.count(nextType))
            return true;
    }
    return false;
}

mapCell* mapDispatcher::getCellByPos(const std::pair<int, int>& pos) {
    if (cells.count(pos.first) && cells[pos.first].count(pos.second) && cells[pos.first][pos.second]) {
        return cells[pos.first][pos.second];
    }
    return nullptr;
}

bool mapDispatcher::moveCell(eMoveDirection direction, const std::pair<int, int>& currentPos) {
    if (currentPos.first < 0 || currentPos.second < 0 || currentPos.first > mapSize.first || currentPos.second > mapSize.second)
        return false;
    if (hasCollision(direction, currentPos))
        return false;
    auto nextPosition = getNextIndex(direction, currentPos);
    if (auto neighborCell = getCellByPos(nextPosition)) {
        if (!neighborCell->move(direction))
            return false;
    }
    if (!cells.count(nextPosition.first) || !cells[nextPosition.first].count(nextPosition.second) || !cells[nextPosition.first][nextPosition.first]) {
        cells[nextPosition.first][nextPosition.second] = {};
    }
    auto& prevPos = cells[currentPos.first][currentPos.second];
    auto& nextPos = cells[nextPosition.first][nextPosition.second];
    std::swap(prevPos, nextPos);
    nextPos->pos = nextPosition;
    return true;
}

void mapDispatcher::getClosestCells(mapCellItemsPtr& closest, std::vector<mapCell*>& exclude, eMoveDirection direction) {
    std::set<eMoveDirection> closestPattern = {
        eMoveDirection::UP, eMoveDirection::DOWN, eMoveDirection::LEFT, eMoveDirection::RIGHT
    };
    for (auto& dir : closestPattern) {
        auto nextPosition = getNextIndex(dir, closest->cell->pos);
        if (auto neighborCell = getCellByPos(nextPosition)) {
            auto it = std::find(exclude.begin(), exclude.end(), neighborCell);
            if (it == exclude.end()) {
                exclude.push_back(neighborCell);
                closest->cells[dir] = std::make_shared<mapCellItems>(neighborCell);
                getClosestCells(closest->cells[dir], exclude, dir);
            }
        }
    }
}

bool mapCell::move(eMoveDirection direction) {
    if (!moved && moveClb && moveClb(direction, pos)) {
        moved = true;
        auto actionPos = node->getPosition();
        switch (direction) {
        case eMoveDirection::UP: actionPos.y += node->getContentSize().height * node->getScale(); break;
        case eMoveDirection::DOWN: actionPos.y -= node->getContentSize().height * node->getScale(); break;
        case eMoveDirection::RIGHT: actionPos.x += node->getContentSize().width * node->getScale(); break;
        case eMoveDirection::LEFT: actionPos.x -= node->getContentSize().width * node->getScale(); break;
        default: break;
        }
        node->setAnimation(eBentoAnimation::MOVE);
        auto action = cocos2d::MoveTo::create(animDelay, actionPos);
        auto clb = cocos2d::CallFunc::create([this, bento = node](){
            bento->setAnimation(eBentoAnimation::IDLE);
        });
        node->runAction(cocos2d::Sequence::create(action, clb, nullptr));
        return true;
    }
    return false;
}

bool mapCellItems::move(eMoveDirection direction) {
    if (cell->moved || cell->move(direction)) {
        for (const auto& [_, item] : cells) {
            item->move(direction);
        }
        return true;
    }
    return false;
}

void mapCellItems::reset() {
    for (const auto& [_, item] : cells) {
        item->reset();
    }
    cell->moved = false;
}
