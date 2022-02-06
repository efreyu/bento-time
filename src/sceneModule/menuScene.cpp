#include "menuScene.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "generic/coreModule/resources/resourceManager.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "interfaceModule/widgets/buttonWidget.h"
#include "gameplayModule/moveEnum.h"

using namespace bt::sceneModule;
using namespace bt::interfaceModule;
using namespace bt::gameplayModule;
using namespace cocos2d;

std::map<eMenuPageType, std::string> menuTypes = {
    {eMenuPageType::MAIN_MENU, "mainMenu"},
    {eMenuPageType::OPTIONS, "optionsMenu"},
    {eMenuPageType::PLAY, "play"}
};

menuScene::menuScene() {
    this->setName("menuScene");
    initWithProperties("scenes/" + this->getName());
    if (!hasPropertyObject("settings"))
        return;
    const auto& json = getPropertyObject("settings");
    auto color = cocos2d::Color3B::BLACK;
    if (json.HasMember("bgColor") && json["bgColor"].IsArray()) {
        color.r = static_cast<uint8_t>(json["bgColor"][0u].GetInt());
        color.g = static_cast<uint8_t>(json["bgColor"][1u].GetInt());
        color.b = static_cast<uint8_t>(json["bgColor"][2u].GetInt());
    }
    initLayerColor(color);
    if (json.HasMember("menuFile") && json["menuFile"].IsString()) {
        initMenu(json["menuFile"].GetString());
    }
}

void menuScene::onSceneLoading() {
    sceneInterface::onSceneLoading();
    auto controllerNode = dynamic_cast<interfaceModule::controllerStickWidget*>(findNode("controller"));
    auto buttonA = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonA"));
    auto buttonB = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonB"));
    if (!controllerNode || !buttonA || !buttonB) {
        LOG_ERROR("Problems with loading nodes.");
        return;
    }
    controllerNode->init();
    buttonA->initListener();
    buttonB->initListener();
    controllerNode->getEmitter()->onPressed.connect([this](eMoveDirection direction){
        if (menuList.empty() || direction == eMoveDirection::RIGHT || direction == eMoveDirection::LEFT)
            return;
        auto menuListIt = std::find_if(menuList.begin(), menuList.end(), [](const std::shared_ptr<sActiveMenu>& a){
            return a->selected;
        });
        if (menuListIt == menuList.end()) {
            menuListIt = menuList.begin();
        }
        menuListIt->get()->selected = false;
        if (auto btn = dynamic_cast<buttonWidget*>(menuListIt->get()->node)) {
            btn->setSelect(false);
        }
        if (direction == eMoveDirection::DOWN) {
            auto next = std::next(menuListIt);
            if (next != menuList.end())
                menuListIt = next;
        } else {
            auto next = std::prev(menuListIt);
            if (next != menuList.end())
                menuListIt = next;
        }
        menuListIt->get()->selected = true;
        if (auto btn = dynamic_cast<buttonWidget*>(menuListIt->get()->node)) {
            btn->setSelect(true);
        }
    });
    buttonA->setOnTouchEnded([this](){
        if (menuList.empty())
            return;
        auto menuListIt = std::find_if(menuList.begin(), menuList.end(), [](const std::shared_ptr<sActiveMenu>& a){
            return a->selected;
        });
        if (menuListIt != menuList.end() && menuListIt->get()->clb) {
            menuListIt->get()->clb();
        }
    });
    buttonB->setOnTouchEnded([](){
        GET_SCENES_FACTORY().runScene("menuScene");
    });
    loadPage(menuTypes[eMenuPageType::MAIN_MENU]);
}

void menuScene::initMenu(const std::string& path) {
    auto data = GET_JSON(path);
    if (data.HasParseError() || !data.IsObject()) {
        LOG_ERROR(cocos2d::StringUtils::format("Can't read file %s.", path.c_str()));
        return;
    }
    for (auto it = data.MemberBegin(); it != data.MemberEnd(); ++it) {
        auto page = std::make_shared<menuPage>();
        page->pageId = it->name.GetString();
        auto obj = it->value.GetObject();
        if (obj.HasMember("hint") && obj["hint"].IsString()) {
            page->hintText = obj["hint"].GetString();
        }
        if (obj.HasMember("buttons") && obj["buttons"].IsArray()) {
            auto array = obj["buttons"].GetArray();
            for (auto item = array.Begin(); item != array.End(); ++item) {
                if (item->IsObject()) {
                    auto bObj = item->GetObject();
                    auto button = std::make_shared<menuItem>();
                    if (bObj.HasMember("type") && bObj["type"].IsString()) {
                        button->type = bObj["type"].GetString();
                    }
                    if (bObj.HasMember("text") && bObj["text"].IsString()) {
                        button->text = bObj["text"].GetString();
                    }
                    if (bObj.HasMember("backBtn") && bObj["backBtn"].IsBool()) {
                        button->backBtn = bObj["backBtn"].GetBool();
                    }
                    if (bObj.HasMember("enabled") && bObj["enabled"].IsBool()) {
                        button->enabled = bObj["enabled"].GetBool();
                    }
                    page->buttons.emplace_back(button);
                }
            }
        }
        menuPagesMap[page->pageId] = page;
    }
}

void menuScene::loadPage(const std::string& page) {
    auto menuHolder = findNode("menuHolder");
    auto backBtnHolder = findNode("backBtnHolder");
    auto tipsHolder = findNode("tipsHolder");

    if (menuTypes[eMenuPageType::PLAY] == page) {
        GET_SCENES_FACTORY().runScene("gameScene");
    }
    if (!menuPagesMap.count(page) || !menuHolder || !backBtnHolder || !tipsHolder) {
        LOG_ERROR("Problems with loading nodes.");
        return;
    }
    auto pagePtr = menuPagesMap[page];
    menuList.clear();
    menuHolder->removeAllChildren();
    backBtnHolder->removeAllChildren();
    tipsHolder->removeAllChildren();
    bool backBtn = false;
    if (!pagePtr->buttons.empty()) {
        backBtn = pagePtr->buttons.front()->backBtn;
        if (backBtn) {
            auto btnIt = pagePtr->buttons.front();
            auto menuItem = std::make_shared<sActiveMenu>();
            auto btn = new buttonWidget();
            menuItem->node = btn;
            btn->setText(btnIt->text);
            if (!menuHolder->getChildren().empty())
                btn->setPositionY(0 - menuHolder->getChildren().size() * btn->getContentSize().height);
            backBtnHolder->addChild(btn);
            if (!btnIt->enabled)
                btn->setDisabled();
            menuItem->clb = [this, data = btnIt->type](){
                loadPage(data);
            };
            menuList.emplace_back(menuItem);
        }
    }

    for (const auto& item : pagePtr->buttons) {
        if (!item->backBtn) {
            auto menuItem = std::make_shared<sActiveMenu>();
            auto btn = new buttonWidget();
            menuItem->node = btn;
            btn->setText(item->text);
            if (!menuHolder->getChildren().empty())
                btn->setPositionY(0 - menuHolder->getChildren().size() * btn->getContentSize().height);
            menuHolder->addChild(btn);
            if (!item->enabled)
                btn->setDisabled();
            menuItem->clb = [this, data = item->type](){
                loadPage(data);
            };
            menuList.emplace_back(menuItem);
        }
    }

    if (!menuList.empty()) {
        auto btnIt = menuList.begin();
        if (backBtn) {
            btnIt++;
        }
        if (btnIt != menuList.end()) {
            btnIt->get()->selected = true;
        }
        if (auto btn = dynamic_cast<buttonWidget*>(btnIt->get()->node)) {
            btn->setSelect(true);
        }
    }

    if (!pagePtr->hintText.empty()) {
        auto tipsLabel = cocos2d::Label::create();
        tipsLabel->setName("tipsLabel");
        loadProperty(tipsLabel, "tipsLabel");
        tipsLabel->setString(pagePtr->hintText);
        tipsHolder->addChild(tipsLabel);
    }

}
