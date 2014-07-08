//
//  FakeSplash.h
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#ifndef __FlappyMario__FakeSplash__
#define __FlappyMario__FakeSplash__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class FakeSplash : public Layer
{
    
    void showInitialMenu(Node* node);
    
public:
    
    FakeSplash();
    virtual ~FakeSplash();
    
    static Scene* scene();
    CREATE_FUNC(FakeSplash);
    
    virtual bool init();
};

#endif /* defined(__FlappyMario__FakeSplash__) */
