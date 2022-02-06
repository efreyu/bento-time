#ifndef BENTO_TIME_MENUSCENE_H
#define BENTO_TIME_MENUSCENE_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/scenes/sceneInterface.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace bt::sceneModule {

    enum class eMenuPageType {
        MAIN_MENU = 0,
        OPTIONS,
        PLAY
    };

    struct menuItem {
        std::string type;
        std::string text;
        bool enabled = true;
        bool backBtn = false;
        std::function<void()> clb = nullptr;
    };

    struct menuPage {
        std::string pageId;
        std::vector<std::shared_ptr<menuItem>> buttons;
        std::string hintText;
        bool small = false;
    };

    struct sActiveMenu {
        cocos2d::Node* node = nullptr;
        bool selected = false;
        std::function<void()> clb = nullptr;
        ~sActiveMenu() {
            CC_SAFE_RELEASE(node);
        }
    };

    class menuScene
      : public generic::coreModule::nodeProperties
      , public generic::coreModule::sceneInterface {
    public:
        menuScene();
        void onSceneLoading() override;

    private:
        void loadSettings();
        void initMenu(const std::string& path);
        void loadPage(const std::string& page);
        std::map<std::string, std::shared_ptr<menuPage>> menuPagesMap;
        std::list<std::shared_ptr<sActiveMenu>> menuList;

        struct menuSceneSettings {
            int allowedItemCount;
            std::string moreButtonText;
            std::string levelProgressPattern;
        };
        menuSceneSettings settings;
    };
}// namespace bt::sceneModule


#endif// BENTO_TIME_MENUSCENE_H
