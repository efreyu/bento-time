#include "bentoNode.h"
#include "databaseModule/mapObjectsDatabase.h"
#include "generic/coreModule/nodes/types/asepriteNode.h"
#include <string>
#include <map>

using namespace bt::gameplayModule;

std::map<eBentoAnimation, std::string> animationTypes = {
    {eBentoAnimation::WIN, "win"},
    {eBentoAnimation::IDLE, "idle"},
    {eBentoAnimation::MOVE, "move"},
    {eBentoAnimation::SLEEP, "sleep"}
};

bentoNode::bentoNode() {
    this->setName("bentoNode");
    initWithProperties(this->getName());
    removeJsonData();
}

bentoNode::~bentoNode() {}

void bentoNode::initWithData(const databaseModule::sMapObjectsData& data) {
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

void bentoNode::setAnimation(eBentoAnimation type) {
    if (lastAnimation != eBentoAnimation::WIN) {
        lastAnimation = type;
        if (auto asepriteNode = dynamic_cast<generic::coreModule::asepriteNode*>(findNode("asepriteNode"))) {
            asepriteNode->setAnimation(animationTypes[type], true);
        }
    }
}
