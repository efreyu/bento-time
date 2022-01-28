#include "AppDelegate.h"
#include "generic/audioModule/audioEngineInstance.h"
#include "generic/coreModule/physicsShape/physicsShapeCache.h"
#include "generic/coreModule/resources/resourceManager.h"
#include "generic/coreModule/resources/settings/settingManager.h"
#include "generic/coreModule/scenes/sceneInterface.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "generic/debugModule/logManager.h"

// all profile block header
#include "generic/profileModule/profileManager.h"
#include "localProfile/localProfileBlock.h"
// all databases header
#include "databasesModule/databaseManager.h"
#include "databasesModule/locationsDatabase.h"
#include "databasesModule/mapObjectsDatabase.h"
#include "databasesModule/skillsDatabase.h"
#include "generic/databaseModule/databaseInterface.h"
// all scenes
#include "sceneModule/battleScene.h"
#include "sceneModule/metaScene.h"
// widgets
#include "interfaceModule/customNodeTypes.h"

USING_NS_CC;


AppDelegate::AppDelegate() {
    GET_AUDIO_ENGINE();
    GET_PROFILE();
    GET_DATABASE_MANAGER();
    GET_NODE_FACTORY();
    GET_LOGGER();
    GET_PHYSICS_SHAPE();
    GET_SCENES_FACTORY();
}

AppDelegate::~AppDelegate() {
    generic::audioModule::audioEngineInstance::cleanup();
    generic::profileModule::profileManager::cleanup();
    GET_DATABASE_MANAGER().cleanup();
    generic::coreModule::nodeFactory::cleanup();
    generic::coreModule::scenesFactoryInstance::cleanup();
    generic::debugModule::logManager::cleanup();
    generic::coreModule::physicsShapeCache::cleanup();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto setting = GET_RESOLUTION_SETTING();
    setting->load();
//    cocos2d::FileUtils::getInstance()->setPopupNotify(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    setting->init(false, "frameResolution"); // default development resolution
#else
    setting->init(true);
#endif
    auto currentResolution = setting->getCurrentSize();
    auto director = Director::getInstance();
    auto glView = director->getOpenGLView();
    if (!glView) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glView = GLViewImpl::createWithRect(
          "Swipe RPG", cocos2d::Rect(0, 0, currentResolution->size.width, currentResolution->size.height), currentResolution->scale);
        glView->setDesignResolutionSize(currentResolution->size.width, currentResolution->size.height, ResolutionPolicy::EXACT_FIT);
        glView = GLViewImpl::createWithRect("Bento time", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glView->setFrameZoomFactor(0.60f);
#endif // end (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#else
        glView = GLViewImpl::create("Bento time");
#endif
        director->setOpenGLView(glView);
    }

    // turn on display FPS
    director->setDisplayStats(currentResolution->showStats);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // set project view mode
    director->setProjection(Director::Projection::_3D);
    cocos2d::Sprite::setUsePixelModeGlobal(currentResolution->spritePixel);
    // Set the design resolution
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    director->setContentScaleFactor(1.f);
#else
    glView->setDesignResolutionSize(currentResolution->size.width, currentResolution->size.height, ResolutionPolicy::EXACT_FIT);
#endif

    GET_AUDIO_ENGINE().stopAll();
    // register all profile
    GET_PROFILE().registerBlock("local", []() {
        return new bt::localProfile::localProfileBlock();
    });
    GET_PROFILE().executeLoad();
    // register all databases
    GET_DATABASE_MANAGER().addDatabase(bt::databasesModule::databaseManager::eDatabaseList::LOCATIONS_DB, "properties/database/locations/db.json", new bt::databasesModule::locationsDatabase());
    GET_DATABASE_MANAGER().addDatabase(bt::databasesModule::databaseManager::eDatabaseList::MAP_OBJECTS_DB, "properties/database/mapObjects/db.json", new bt::databasesModule::mapObjectsDatabase());
    GET_DATABASE_MANAGER().executeLoadData();
    // register external node types
    bt::interfaceModule::registerAllCustomNodes();
    // register all states
    GET_SCENES_FACTORY().registerScene("menuScene", []() {
        return new bt::sceneModule::metaScene();//todo rename to menu scene
    });
    GET_SCENES_FACTORY().registerScene("battleScene", []() {
        return new bt::sceneModule::battleScene(); // todo rename to game scene
    });

    // run first scene
    GET_SCENES_FACTORY().runScene("menuScene");

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    GET_AUDIO_ENGINE().pauseAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    GET_AUDIO_ENGINE().resumeAll();
}
