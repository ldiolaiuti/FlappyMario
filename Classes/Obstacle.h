//
//  Obstacle.h
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#ifndef __FlappyMario__Obstacle__
#define __FlappyMario__Obstacle__

#include <iostream>
#include "cocos2d.h"
#include "GameScene.h"


USING_NS_CC;

typedef enum
{
    CollisionTypeNone,
    CollisionTypeGoal,
    CollisionTypePipe,
    
}CollisionType;


class Obstacle : public Node
{
private:
    Size _screenSize;
    int _scale;
    
    Sprite* _topBalo;
    Sprite* _bottomBalo;
    Sprite* _mouthTop;
    Sprite* _mouthBottom;
    Node*   _goal;
    Animation* _anMouth;
    
    GameScene* _game;
    
    void runAnimation();

public:
    
    Obstacle();
    virtual ~Obstacle();
    
    bool init();
    
    void setupRandomPosition();
    
    CREATE_FUNC(Obstacle);
    
    CollisionType checkCollisionWithPlayer(Sprite * sprite, const Point& point);
    
};

#endif /* defined(__FlappyMario__Obstacle__) */
