//
//  VisibleRect.h
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#ifndef __FlappyMario__VisibleRect__
#define __FlappyMario__VisibleRect__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class VisibleRect
{
public:
    static Rect getVisibleRect();
    
    static Point left();
    static Point right();
    static Point top();
    static Point bottom();
    static Point center();
    static Point leftTop();
    static Point rightTop();
    static Point leftBottom();
    static Point rightBottom();
private:
    static void lazyInit();
    static Rect s_visibleRect;
};

#endif /* defined(__FlappyMario__VisibleRect__) */
