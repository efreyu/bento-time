#ifndef BENTO_TIME_GAMESCENE_H
#define BENTO_TIME_GAMESCENE_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/scenes/sceneInterface.h"

namespace bt::sceneModule {

    class gameScene
      : public generic::coreModule::nodeProperties
        , public generic::coreModule::sceneInterface
        , public taskHolder {
    public:
        gameScene();
        ~gameScene() override = default;
        std::deque<nodeTasks> getTasks() override;
    };
}// namespace bt::sceneModule


#endif// BENTO_TIME_GAMESCENE_H
