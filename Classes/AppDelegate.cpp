#include "AppDelegate.h"
#include "FakeSplash.h"
#include "AnalyticX.h"
#include "ChartboostX.h"


// The app key of flurry
#define FLURRY_KEY_IOS          "36DNQ9MNW36CYV7ZKSY8"
#define FLURRY_KEY_ANDROID      "4T3BMRNBYQDZNZF88NST"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    
    auto pDirector = Director::getInstance();
    auto pEGLView = pDirector->getOpenGLView();
    if(!pEGLView) {
        pEGLView = GLView::create("Fly Tony");
        pDirector->setOpenGLView(pEGLView);
    }
	
    pEGLView->setDesignResolutionSize(640, 960, ResolutionPolicy::NO_BORDER);
    
    // turn on display FPS
    pDirector->setDisplayStats(false);
    
    Size screenSize = pEGLView->getFrameSize();
    
    std::vector<std::string> searchPath;
    
    if (screenSize.width > 640)
    {
        searchPath.push_back("sd");
        pDirector->setContentScaleFactor(2);
    }
    else
    {
        searchPath.push_back("sd");
        pDirector->setContentScaleFactor(1);
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPath);
    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);
    
    // create a scene. it's an autorelease object
    Scene *pScene = FakeSplash::scene();

    //Load all plugins
    loadPlugins();

    // run
    pDirector->runWithScene(pScene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}


void AppDelegate::loadPlugins(){
	//--LOAD FLURRY--
	std::string flurryKey = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    flurryKey = "NZYPB97M9C8T6V29DW8J";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    flurryKey = "QYB7M66P8VPCXM9H3Q5T";
#endif
    AnalyticX::flurryStartSession(flurryKey.c_str());
}
