//
//  Obstacle.cpp
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#include "Obstacle.h"
#include "VisibleRect.h"

#define PTM_RATIO 32

// visibility on a 3,5-inch iPhone ends a 88 points and we want some meat
static const float minimumYPositionTopPipe = 80;

// visibility ends at 480 and we want some meat
static const float maximumYPositionBottomPipe = 700.f;

// distance between top and bottom pipe
static const float pipeDistance = 180.0f;

// calculate the end of the range of top pipe
static const float maximumYPositionTopPipe = maximumYPositionBottomPipe
		- pipeDistance;

// radius of ball
static const int radius = 56;


Obstacle::Obstacle() {

}

Obstacle::~Obstacle() {
	CC_SAFE_RELEASE(_anMouth);
}

bool Obstacle::init() {
	if (!CCNode::init()) {
		return false;
	}
	_screenSize = Director::getInstance()->getWinSize();
    _scale = Director::getInstance()->getContentScaleFactor();


	this->setContentSize(Size(80, 640));
	this->setAnchorPoint(Point(0, 0));

	_topBalo = Sprite::create("PoseMario/baloBusto.png");
    _topBalo->setFlippedY(true);
    _topBalo->setScale(_scale);
	_topBalo->setAnchorPoint(Point(0.5, 0));
	_topBalo->setPosition(Point(this->getContentSize().width / 2, 0));

	_mouthTop = Sprite::create("PoseMario/bocca_01.png");
	_mouthTop->setFlippedY(true);
	_mouthTop->setAnchorPoint(Point(0.5, 0));
	_mouthTop->setPosition(Point(_topBalo->getContentSize().width/2, 0));
	_topBalo->addChild(_mouthTop);

	Sprite* basePipeUp = Sprite::create("PoseMario/pezzoCapitello.png");
	basePipeUp->setAnchorPoint(Point(0.5f, 0));
	basePipeUp->setPosition(
			Point(_topBalo->getContentSize().width / 2,
					_topBalo->getContentSize().height-1));
	_topBalo->addChild(basePipeUp);

	_bottomBalo = CCSprite::create("PoseMario/baloBusto.png");
    _bottomBalo->setScale(_scale);
	_bottomBalo->setAnchorPoint(Point(0.5, 1.0));
	_bottomBalo->setPositionX(this->getContentSize().width / 2);

	_mouthBottom = Sprite::create("PoseMario/bocca_01.png");
	_mouthBottom->setAnchorPoint(Point(0.5, 1.0));
	_mouthBottom->setPosition(
			Point(_bottomBalo->getContentSize().width / 2,
					_bottomBalo->getContentSize().height));
	_bottomBalo->addChild(_mouthBottom);

	Sprite* basePipeDown = Sprite::create("PoseMario/pezzoCapitello.png");
	basePipeDown->setAnchorPoint(Point(0.5f, 1.0));
	basePipeDown->setPosition(
			Point(_bottomBalo->getContentSize().width / 2,
					0));
	_bottomBalo->addChild(basePipeDown);

	_goal = Node::create();
	_goal->setContentSize(Size(20, 640));
	_goal->setAnchorPoint(Point(0, 0));
	_goal->setPosition(
			Point(
					this->getContentSize().width / 2
							- _goal->getContentSize().width / 2, 0));

	this->addChild(_topBalo);
	this->addChild(_bottomBalo);
	this->addChild(_goal);

	_anMouth = Animation::create();
	_anMouth->addSpriteFrameWithFile("PoseMario/bocca_01.png");
	_anMouth->addSpriteFrameWithFile("PoseMario/bocca_02.png");
	_anMouth->addSpriteFrameWithFile("PoseMario/bocca_01.png");
	_anMouth->addSpriteFrameWithFile("PoseMario/bocca_02.png");
	_anMouth->setDelayPerUnit(0.15);
	_anMouth->setRestoreOriginalFrame(true);
	_anMouth->retain();

	return true;
}

void Obstacle::setupRandomPosition() {
	// value between 0.f and 1.f
	float random = (float) ((rand() % 10) * 0.1);
	float range = maximumYPositionTopPipe - minimumYPositionTopPipe;

	_topBalo->setPosition(
			Point(_topBalo->getPositionX(),
					_screenSize.height
							- (minimumYPositionTopPipe + (random * range))));

	_bottomBalo->setPosition(
			Point(_bottomBalo->getPositionX(),
					_topBalo->getPositionY() - pipeDistance));

}

CollisionType Obstacle::checkCollisionWithPlayer(Sprite *sprite, const Point& point) {
	Rect bottomBaloRect = Rect(point.x, point.y, 70, _bottomBalo->getPositionY());
	Rect topBaloRect = Rect(point.x, _topBalo->getPositionY(), 70, _screenSize.height);
	Rect goalRect = Rect(point.x, point.y, _goal->getContentSize().width,
			VisibleRect::getVisibleRect().size.height);

	Rect ballBoundingBox = Rect(
			sprite->getPositionX() - sprite->getContentSize().width / 4,
			sprite->getPositionY() - sprite->getContentSize().height / 4,
			sprite->getContentSize().width / 2 * sprite->getScale(),
			sprite->getContentSize().height / 2 * sprite->getScale());

	CollisionType type = CollisionTypeNone;
	if (bottomBaloRect.intersectsRect(ballBoundingBox)) {
		type = CollisionTypePipe;
	} else if (topBaloRect.intersectsRect(ballBoundingBox)) {
		type = CollisionTypePipe;
	} else if (goalRect.intersectsRect(ballBoundingBox)) {
		type = CollisionTypeGoal;
		this->runAnimation();
	}

	return type;
}

void Obstacle::runAnimation(){
	_mouthBottom->runAction(Animate::create(_anMouth));
	_mouthTop->runAction(Animate::create(_anMouth));
}
