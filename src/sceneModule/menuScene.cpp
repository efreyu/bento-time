#include "menuScene.h"
#include "databaseModule/databaseManager.h"
#include "databaseModule/levelsDatabase.h"
#include "gameplayModule/moveEnum.h"
#include "generic/audioModule/audioEngineInstance.h"
#include "generic/coreModule/resources/resourceManager.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "generic/profileModule/profileManager.h"
#include "generic/utilityModule/convertUtility.h"
#include "interfaceModule/widgets/buttonWidget.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "profileModule/progressProfileBlock.h"

using namespace bt::sceneModule;
using namespace bt::interfaceModule;
using namespace bt::gameplayModule;
using namespace bt::databaseModule;
using namespace cocos2d;

std::map<eMenuPageType, std::string> menuTypes = {
    {eMenuPageType::MAIN_MENU, "mainMenu"},
    {eMenuPageType::PLAY, "play"}
};

menuScene::menuScene() {
    this->setName("menuScene");
    initWithProperties("scenes/" + this->getName());
    loadSettings();
}

void menuScene::loadSettings() {
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
    if (json.HasMember("fadeTransitionTime") && json["fadeTransitionTime"].IsNumber()) {
        fadeTransitionTime = json["fadeTransitionTime"].GetFloat();
    }
    if (json.HasMember("allowedItemCount") && json["allowedItemCount"].IsNumber()) {
        settings.allowedItemCount = json["allowedItemCount"].GetInt();
    }
    if (json.HasMember("moreButtonText") && json["moreButtonText"].IsString()) {
        settings.moreButtonText = json["moreButtonText"].GetString();
    }
    if (json.HasMember("levelProgressPattern") && json["levelProgressPattern"].IsString()) {
        settings.levelProgressPattern = json["levelProgressPattern"].GetString();
    }
    if (json.HasMember("lightening") && json["lightening"].IsNumber()) {
        settings.lightening = json["lightening"].GetFloat();
    }
    if (json.HasMember("selectDuration") && json["selectDuration"].IsNumber()) {
        settings.selectDuration = json["selectDuration"].GetFloat();
    }
    if (json.HasMember("menuFile") && json["menuFile"].IsString()) {
        initMenu(json["menuFile"].GetString());
    }
}

void menuScene::onSceneLoading() {
    sceneInterface::onSceneLoading();
    GET_AUDIO_ENGINE().playOnce("music.main", true);
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
        GET_AUDIO_ENGINE().play("ui.choose");
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
        if (obj.HasMember("small") && obj["small"].IsBool()) {
            page->small = obj["small"].GetBool();
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

    //level progress page
    auto page = std::make_shared<menuPage>();
    if (menuPagesMap.count(menuTypes[eMenuPageType::PLAY])) {
        page = menuPagesMap[menuTypes[eMenuPageType::PLAY]];
    }
    page->pageId = menuTypes[eMenuPageType::PLAY];
    auto progressBlock = GET_PROFILE().getBlock<profileModule::progressProfileBlock>("progress");
    auto levelDb = GET_DATABASE_MANAGER().getDatabase<levelsDatabase>(databaseManager::eDatabaseType::LEVELS_DB);
    for (const auto& [id, info] : levelDb->getLevels()) {
        if (page->buttons.size() > settings.allowedItemCount) {
            auto newId = cocos2d::StringUtils::format("%s_%d", menuTypes[eMenuPageType::PLAY].c_str(), id);
            auto button = std::make_shared<menuItem>();
            button->text = settings.moreButtonText;
            button->type = newId;
            page->buttons.emplace_back(button);
            menuPagesMap[page->pageId] = page;
            auto firstBtn = page->buttons.front();
            auto prevType = page->pageId;
            page = std::make_shared<menuPage>(*page);;
            page->buttons.clear();
            page->pageId = newId;
            //back Btn
            button = std::make_shared<menuItem>(*firstBtn);
            button->type = prevType;
            page->buttons.emplace_back(button);
        }
        auto button = std::make_shared<menuItem>();
        auto progress = progressBlock->getProgressForLevel(id);
        if (progress && !settings.levelProgressPattern.empty()) {
            button->text = cocos2d::StringUtils::format(settings.levelProgressPattern.c_str(), id, progress->getMoves());
        } else {
            button->text = std::to_string(id);
        }
        button->enabled = id == 1 || (progress && progress->getMoves() > 0);
        //run game with level
        button->clb = [levelId = id](){
            cocos2d::ValueMap data;
            data["levelId"] = cocos2d::Value(levelId);
            GET_SCENES_FACTORY().runSceneWithParameters("gameScene", data);
        };
        page->buttons.emplace_back(button);
    }
    menuPagesMap[page->pageId] = page;

}

void menuScene::loadPage(const std::string& page) {
    auto menuHolder = findNode("menuHolder");
    auto backBtnHolder = findNode("backBtnHolder");
    auto tipsHolder = findNode("tipsHolder");

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
            if (btnIt->clb) {
                menuItem->clb = btnIt->clb;
            } else {
                menuItem->clb = [this, data = btnIt->type, clb = btnIt->clb](){
                    if (clb)
                        clb();
                    loadPage(data);
                };
            }
            menuList.emplace_back(menuItem);
        }
    }
    for (const auto& item : pagePtr->buttons) {
        if (!item->backBtn) {
            auto menuItem = std::make_shared<sActiveMenu>();
            auto btn = new buttonWidget();
            menuItem->node = btn;
            btn->setText(item->text);
            if (pagePtr->small) {
                btn->setSmallText();
            }
            if (!menuHolder->getChildren().empty()) {
                auto last = menuHolder->getChildren().rbegin();
                btn->setPositionY((*last)->getPositionY() - (*last)->getContentSize().height);
            }

            menuHolder->addChild(btn);
            if (!item->enabled)
                btn->setDisabled();
            if (item->clb) {
                menuItem->clb = item->clb;
            } else {
                menuItem->clb = [this, data = item->type](){
                    loadPage(data);
                };
            }
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
        auto color = tipsLabel->getColor();
        auto nextColor = generic::utilityModule::convertUtility::changeColorByPercent(color, settings.lightening);
        auto tint = cocos2d::TintTo::create(settings.selectDuration, nextColor);
        auto reverse = cocos2d::TintTo::create(settings.selectDuration, color);
        auto seq = cocos2d::Sequence::create(tint, reverse, nullptr);
        tipsLabel->runAction(cocos2d::RepeatForever::create(seq));
    }
}
