//
//  FakeSplash.cpp
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#include "FakeSplash.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

USING_NS_CC;

FakeSplash::FakeSplash()
{
    
}

FakeSplash::~FakeSplash()
{
    
}

Scene* FakeSplash::scene()
{
    Scene* scene = Scene::create();
    
    FakeSplash* splash = FakeSplash::create();
    
    scene->addChild(splash);
    
    return scene;
}


bool FakeSplash::init()
{
    
    if (!Layer::init())
    {
        return false;
    }
    
    Size screenSize = Director::getInstance()->getWinSize();
    
    Sprite* bg = Sprite::create("splashScreen.png");
    bg->setScale(Director::getInstance()->getContentScaleFactor());
    bg->setPosition(Point(screenSize.width/2, screenSize.height/2));
    bg->setOpacity(0);
    addChild(bg);
    
    Sequence* seq = Sequence::create(
                                     DelayTime::create(0.1),
                                     FadeIn::create(0.2f),
                                     DelayTime::create(0.5f),
                                     CCCallFuncN::create(this, callfuncN_selector(FakeSplash::showInitialMenu)),
                                     NULL
                                     );
    
    bg->runAction(seq);
    
    
    return true;
}


void FakeSplash::showInitialMenu(Node *node)
{
    node->removeFromParentAndCleanup(true);
    
    Scene* scene = GameScene::scene();
    Director::getInstance()->replaceScene(scene);
    
}

