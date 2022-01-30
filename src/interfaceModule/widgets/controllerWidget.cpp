#include "controllerWidget.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "generic/debugModule/logManager.h"

using namespace bt::interfaceModule;

const std::map<std::string, eMoveDirection> moveTypesMap = {
    {"up", eMoveDirection::UP},
    {"down", eMoveDirection::DOWN},
    {"left", eMoveDirection::LEFT},
    {"right", eMoveDirection::RIGHT}
};

const std::map<eMoveDirection, std::string> animationTypesMap = {
    {eMoveDirection::UNDEFINED, "default"},
    {eMoveDirection::UP, "up_pressed"},
    {eMoveDirection::DOWN, "down_pressed"},
    {eMoveDirection::LEFT, "left_pressed"},
    {eMoveDirection::RIGHT, "right_pressed"}
};

const int buttonActionTag = 1;
const float animDelay = 0.2f;

controllerWidget::controllerWidget() {
    this->setName("controllerWidget");
    initWithProperties("widgets/" + this->getName());
    removeJsonData();
    arrowsNode = dynamic_cast<generic::coreModule::asepriteNode*>(findNode("arrows"));
    if (!arrowsNode)
        LOG_ERROR("Can't find 'arrows' node.");
}

void controllerWidget::initHandler() {
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](auto touch, auto event) {
        return touchProceed(touch, event);
    };
    listener->onTouchMoved = [this](auto touch, auto event) {
        return touchProceed(touch, event);
    };
    listener->onTouchCancelled = listener->onTouchEnded = [this](auto, auto){
        currentPressed = eMoveDirection::UNDEFINED;
        getEmitter()->onReleased.emit();
        return true;
    };
    GET_CURRENT_SCENE()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        auto keyboardDir = eMoveDirection::UNDEFINED;
        switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_W: {}
            keyboardDir = eMoveDirection::UP;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            keyboardDir = eMoveDirection::DOWN;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            keyboardDir = eMoveDirection::LEFT;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            keyboardDir = eMoveDirection::RIGHT;
            break;
        }
        if (keyboardDir != eMoveDirection::UNDEFINED) {
            currentPressed = keyboardDir;
            onButtonHold();
        }
    };
    keyboardListener->onKeyReleased = [this](auto, auto) {
        currentPressed = eMoveDirection::UNDEFINED;
        getEmitter()->onReleased.emit();
    };
    GET_CURRENT_SCENE()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

#endif
}

bool controllerWidget::init() {
    for (const auto& [nodeName, direction] : moveTypesMap) {
        if (auto node = findNode(nodeName)) {
            nodesWithDirections[direction] = node;
        }
    }
    initHandler();
    return Node::init();
}

bool controllerWidget::touchProceed(cocos2d::Touch* touch, cocos2d::Event* event) {
    auto touchLocation = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
    for (const auto& [direction, buttonNode] : nodesWithDirections) {
        auto boundingBox = buttonNode->getBoundingBox();
        boundingBox.origin.x += boundingBox.size.width * buttonNode->getAnchorPoint().x + boundingBox.size.width * buttonNode->getPivotPoint().x;
        boundingBox.origin.y += boundingBox.size.height * buttonNode->getAnchorPoint().y + boundingBox.size.height * buttonNode->getPivotPoint().y;
        bool correctNode = boundingBox.containsPoint(touchLocation);
        if (!correctNode)
            continue;

        currentPressed = direction;
        onButtonHold();

        return true;
    }
    currentPressed = eMoveDirection::UNDEFINED;
    return false;
}

void controllerWidget::onButtonHold() {
    if (!arrowsNode || currentPressed == eMoveDirection::UNDEFINED || getActionByTag(buttonActionTag))
        return;
    auto delayAction = cocos2d::DelayTime::create(animDelay);
    auto clb = cocos2d::CallFunc::create([this]() {
        if (currentPressed == eMoveDirection::UNDEFINED) {
            if (auto currentAction = getActionByTag(buttonActionTag)) {
                stopAction(currentAction);
            }
        } else {
            getEmitter()->onPressed.emit(currentPressed);
        }
        if (animationTypesMap.count(currentPressed)) {
            arrowsNode->setAnimation(animationTypesMap.at(currentPressed));
        }
    });
    auto seq = cocos2d::Sequence::create(clb, delayAction, nullptr);
    auto repeatAction = cocos2d::RepeatForever::create(seq);
    repeatAction->setTag(buttonActionTag);
    runAction(repeatAction);
}
