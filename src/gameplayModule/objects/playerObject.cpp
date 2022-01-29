#include "playerObject.h"
#include "databaseModule/mapObjectsDatabase.h"
#include "generic/coreModule/nodes/types/asepriteNode.h"

using namespace bt::gameplayModule;

playerObject::playerObject() {
    this->setName("playerObject");
    initWithProperties("battle/" + this->getName());
    removeJsonData();
}

playerObject::~playerObject() {}

void playerObject::initWithData(const databaseModule::sMapObjectsData& data) {
    this->setName("playerObject");
    if (data.nodeType == databaseModule::eNodeType::SPRITE) {
        auto sprite = new generic::coreModule::nodeWithProperties<cocos2d::Sprite>();
        sprite->setName("spriteNode");
        sprite->loadJson(data.propertyPath);
        sprite->loadProperty(sprite, "spriteNode");
        sprite->removeJsonData();
        addChild(sprite);
    } else if (data.nodeType == databaseModule::eNodeType::ASEPRITE) {
        auto node = new generic::coreModule::nodeWithProperties<generic::coreModule::asepriteNode>();
        node->setName("asepriteNode");
        node->loadJson(data.propertyPath);
        node->loadProperty(node, "asepriteNode");
        node->removeJsonData();
        addChild(node);
    }
}
