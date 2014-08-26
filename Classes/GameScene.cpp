//
//  GameScene.cpp
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#include "GameScene.h"
#include "VisibleRect.h"
#include "Obstacle.h"
#include "SimpleAudioEngine.h"
#include "ChartboostX.h"
#include <string>
#include <sstream>

#define PTM_RATIO 32
#define GRAVITY 2500
#define DEFAULT_ROTATION_UP 270
#define DEFAULT_ROTATION_DOWN 90
#define FLOATING_GRAVITY_OPPOSITE GRAVITY - 5
#define FORCE_Y 650
#define FIXED_TIME_STEP 1/60

using namespace CocosDenshion;

enum {
	kTagNewRecordLabel,
};

static const float firstObstaclePosition = 400;
static const float distanceBetweenObstacles = 350.f;

static const char s_FontText[] = "font/Brazil.fnt";

typedef enum {
	LayerDrawingBackground, LayerDrawingMidground, LayerDrawingForeground,

} LayerDrawing;

typedef enum {
	DrawingOrderGround,
	DrawingOrderClouds,
	DrawingOrderSupporters3,
	DrawingOrderSupporters2,
	DrawingOrderSupporters1,
	DrawingOrderSupportersForeground,
	DrawingOrderBalzoLogo,
	DrawingOrderPipes,
	DrawingOrderHero,

} DrawingOrder;

GameScene::GameScene() {

}

GameScene::~GameScene() {
	delete _grounds;
	delete _obstacles;
	delete _supporters1;
	delete _supporters2;
	delete _supporters3;
	delete _supportersForeground;
	delete _logo;
	CC_SAFE_RELEASE(_supporterNonSincro);
	CC_SAFE_RELEASE(_supporterSincro);
	CC_SAFE_RELEASE(_supporterOther);
}

Scene* GameScene::scene() {
	Scene* scene = Scene::create();

	GameScene* layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
			listener, this);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	auto keyListener = EventListenerKeyboard::create();
	keyListener->setEnabled(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
			keyListener, this);
	this->setKeypadEnabled(true);
#endif

	int indexOfSpriteSheet = (CC_CONTENT_SCALE_FACTOR() == 2) ? 2 : 1;

	for (int i = 0; i < indexOfSpriteSheet; i++) {
		ostringstream s1;
		s1 << "GameAtlas" << i << ".plist";
		ostringstream s2;
		s2 << "GameAtlas" << i << ".png";
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(s1.str(),
				s2.str());
	}
	_screenSize = Director::getInstance()->getWinSize();
	_scrollSpeedBackground = 200.f;
	_scrollSpeedMidGround = 40;
	_scrollSpeedForeground = _scrollSpeedBackground;
	_yVel = 0;
	_isTap = false;
	_isGameOver = false;
	_isNewRecord = false;
	_timeWaitLabel = 0;
	_maxTimeWaitLabel = 1;
	_idSoundNewGroupScore = 10;
	_scale = Director::getInstance()->getContentScaleFactor();

	//Aggiungo gli ostacoli
	_obstacles = new Vector<Node*>;
	_grounds = new Vector<Ref*>();

	_supporters1 = new Vector<Ref*>();
	_supporters2 = new Vector<Ref*>();
	_supporters3 = new Vector<Ref*>();
	_supportersForeground = new Vector<Ref*>();
	_logo = new Vector<Ref*>();

	_background = CCNode::create();
	_background->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_background->setAnchorPoint(Point(0, 0));
	this->addChild(_background, LayerDrawingBackground);

	_midground = CCNode::create();
	_midground->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_midground->setAnchorPoint(Point(0, 0));
	this->addChild(_midground, LayerDrawingMidground);

	_foreground = CCNode::create();
	_foreground->setContentSize(CCDirector::getInstance()->getVisibleSize());
	_foreground->setAnchorPoint(Point(0, 0));
	this->addChild(_foreground, LayerDrawingForeground);

	//Supporters Animations
	char str[100] = {0};
	_supporterNonSincro = Animation::create();
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "tonyEsultante/nonSincro/a_0%d.png", i);
		_supporterNonSincro->addSpriteFrameWithFile(str);
	}
	_supporterNonSincro->setDelayPerUnit(0.15f);
	_supporterNonSincro->setRestoreOriginalFrame(true);
	_supporterNonSincro->retain();

	_supporterSincro = Animation::create();
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "tonyEsultante/sincro/A_0%d.png", i);
		_supporterSincro->addSpriteFrameWithFile(str);
	}
	_supporterSincro->setDelayPerUnit(0.08f);
	_supporterSincro->setRestoreOriginalFrame(true);
	_supporterSincro->retain();

	_supporterOther = Animation::create();
	for (int i = 1; i <= 8; i++)
	{
		sprintf(str, "tonyEsultante/esultanza/b_0%d.png", i);
		_supporterOther->addSpriteFrameWithFile(str);
	}
	_supporterOther->setDelayPerUnit(0.08f);
	_supporterOther->setRestoreOriginalFrame(true);
	_supporterOther->retain();

	_layerColorWait = LayerColor::create(Color4B(9, 13, 73, 0),
			_screenSize.width, _screenSize.height);
	_layerColorWait->setAnchorPoint(Point(0, 0));
	this->addChild(_layerColorWait, LayerDrawingForeground + 1);

	this->showMainMenu();
	this->drawLayout();

	return true;
}

void GameScene::showMainMenu() {
	_layerColorWait->removeAllChildrenWithCleanup(true);
	Sprite *logo = Sprite::create("logo.png");
	logo->setScale(_scale);
	logo->setAnchorPoint(Point(0.5, 0.5));
	logo->setPosition(
			Point(_layerColorWait->getContentSize().width / 2,
					_layerColorWait->getContentSize().height / 2));
	_layerColorWait->addChild(logo);
	Sprite* ball = Sprite::create("palla.png");
	ball->setScale(_scale);
	ball->setAnchorPoint(Point(0.5, 0.5));
	ball->setPosition(
			Point(_layerColorWait->getContentSize().width * 0.25,
					_layerColorWait->getContentSize().height * 0.75));
	_layerColorWait->addChild(ball);

	Label* playLabel = Label::createWithBMFont(s_FontText, "Tap to play");
	playLabel->setColor(Color3B::YELLOW);
	MenuItemLabel* play = MenuItemLabel::create(playLabel);
	play->setCallback(CC_CALLBACK_1(GameScene::flyTony, this));

	Menu* menu = CCMenu::create(play, NULL);
	menu->setScale(0.7 * _scale);
	menu->setAnchorPoint(Point::ZERO);
	menu->setPosition(
			Point(_layerColorWait->getContentSize().width / 2,
					_layerColorWait->getContentSize().height / 4));

	_layerColorWait->addChild(menu);

	//--Set game state--
	_state = GameState::MAINMENU;
}

void GameScene::onEnter() {
	Layer::onEnter();

	this->spawnNewObstacle();
	this->spawnNewObstacle();
	this->spawnNewObstacle();

	//---BACKGROUND MUSIC---
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playBackgroundMusic(
			"Suoni/musica/musica_1.mp3", true);
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.3);

}

void GameScene::flyTony(Ref *obj) {
	int idSound = (rand() % 4) + 1;
	ostringstream idSoundString;
	idSoundString << "Suoni/play/play_0" << idSound << ".mp3";
	SimpleAudioEngine::getInstance()->preloadEffect(
			idSoundString.str().c_str());
	SimpleAudioEngine::getInstance()->playEffect(idSoundString.str().c_str());

	//--Set game state
	_state = GameState::RUNNING;

	MenuItemSprite* btn = (MenuItemSprite*) obj;
	Vector<Node*> btnChildren = btn->getChildren();
	for (Vector<Node*>::iterator it = btnChildren.begin();
			it < btnChildren.end(); it++) {
		Label* node = dynamic_cast<Label *>(*it);

		if (node != NULL) {
			node->setVisible(false);
		}
	}

	this->callbackFlyTony();
}

void GameScene::callbackFlyTony() {
	_ball->setVisible(true);
	_label->setVisible(true);
	_layerColorWait->setVisible(false);

	this->scheduleUpdate();
}

void GameScene::onExit() {
	CCLayer::onExit();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
}

void GameScene::drawLayout() {
	this->drawBackground();
	this->drawScoreLabel();
	this->drawPlayer();
}

void GameScene::drawBackground() {

	int x = 0;
	for (int i = 0; i < 4; i++) {
		Sprite *fxBackground1 = Sprite::create("bg_ripetere.png");
		fxBackground1->setScale(_scale);
		fxBackground1->setAnchorPoint(Point(0, 0));
		fxBackground1->setPosition(Point(x, 0));
		_background->addChild(fxBackground1);
		_grounds->pushBack(fxBackground1);
		x += fxBackground1->getContentSize().width * _scale;
	}

	//supporters
	x = -100;
	int y = 492;
	while (x < _screenSize.width) {
		Sprite* supporter = drawSupporter();
		supporter->setPosition(Point(x, y));
		supporter->setAnchorPoint(Point::ZERO);
		_foreground->addChild(supporter, DrawingOrderSupporters1);
		x += 62;
		_supporters1->pushBack(supporter);
	}
	x = -70;
	y += 35;
	while (x < _screenSize.width + 30) {
		Sprite* supporter = drawSupporter();
		supporter->setPosition(Point(x, y));
		supporter->setAnchorPoint(Point::ZERO);
		_foreground->addChild(supporter, DrawingOrderSupporters2);
		x += 62;
		_supporters2->pushBack(supporter);
	}
	x = -100;
	y += 35;
	while (x < _screenSize.width) {
		Sprite* supporter = drawSupporter();
		supporter->setPosition(Point(x, y));
		supporter->setAnchorPoint(Point::ZERO);
		_foreground->addChild(supporter, DrawingOrderSupporters3);
		x += 62;
		_supporters3->pushBack(supporter);
	}
	x = -100;
	y = 500;
	while (x < _screenSize.width + 64) {
		Sprite* supporterForeground = Sprite::create("tifosiForeground.png");
		supporterForeground->setAnchorPoint(Point::ZERO);
		supporterForeground->setScale(_scale);
		supporterForeground->setOpacity(120);
		supporterForeground->setPosition(Point(x, y));
		supporterForeground->setAnchorPoint(Point::ZERO);
		_foreground->addChild(supporterForeground,
				DrawingOrderSupportersForeground);
		x += supporterForeground->getContentSize().width;
		_supportersForeground->pushBack(supporterForeground);
	}

	//Balzo Logo
	x = 150;
	y = 711;
	while (x < _screenSize.width + 150) {
		Sprite* logoBalzo = Sprite::create("LogoBalzo_white.png");
		logoBalzo->setScale(_scale);
		logoBalzo->setPosition(Point(x, y));
		logoBalzo->setAnchorPoint(Point(0.5, 0.5));
		_foreground->addChild(logoBalzo, DrawingOrderBalzoLogo);
		x += 300;
		_logo->pushBack(logoBalzo);
	}
}

void GameScene::drawScoreLabel() {
	_score = 0;

	ostringstream s1;
	s1 << _score;
	_label = Label::createWithBMFont(s_FontText, s1.str());
	_label->setScale(_scale);

	_label->setPosition(
			Point(_screenSize.width / 2, _screenSize.height * 0.82));

	this->addChild(_label, LayerDrawingForeground);
	_label->setVisible(false);
}

void GameScene::drawPlayer() {

	_ball = Sprite::create("palla.png");
	_ball->setScale(0.7 * _scale);
	_ball->setAnchorPoint(Point(0.5, 0.5));
	this->addChild(_ball, DrawingOrderHero);
	_ball->setPosition(Point(200, 400));
	_ball->setVisible(false);
}

Sprite* GameScene::drawSupporter() {
	Sprite* supporter;
	int random = rand() % 3 + 1;
	//int random = 3;
	switch (random) {
	case 1:
		supporter = Sprite::create("tonyEsultante/nonSincro/a_01.png");
		supporter->runAction(
				RepeatForever::create(Animate::create(_supporterNonSincro)));
		break;
	case 2:
		supporter = Sprite::create("tonyEsultante/sincro/A_01.png");
		supporter->runAction(
				RepeatForever::create(Animate::create(_supporterSincro)));
		break;
	case 3:
		supporter = Sprite::create("tonyEsultante/esultanza/b_01.png");
		supporter->runAction(
				RepeatForever::create(Animate::create(_supporterOther)));
		break;
	}
	supporter->setScale(_scale);
	return supporter;
}

// Update

void GameScene::update(float dt) {
	_background->setPosition(
			Point(
					_background->getPositionX()
							- (_scrollSpeedBackground * FIXED_TIME_STEP),
					_background->getPositionY()));
	_midground->setPosition(
			Point(
					_midground->getPositionX()
							- (_scrollSpeedMidGround * FIXED_TIME_STEP),
					_midground->getPositionY()));
	_foreground->setPosition(
			Point(
					_background->getPositionX()
							- (_scrollSpeedForeground * FIXED_TIME_STEP),
					_background->getPositionY()));

	if (_isTap) {
		_rotation = -40;
		_yVel = (FORCE_Y);
		_isTap = false;
	}

	_yVel -= (dt * GRAVITY);

	if (_yVel >= FORCE_Y) {
		_yVel = FORCE_Y;

	} else if (_yVel <= 2 * -GRAVITY) {
		_yVel = 2 * -GRAVITY;
	}

	_ball->setPositionY(_ball->getPositionY() + (_yVel * FIXED_TIME_STEP));

	if (_ball->getPositionY()
			> VisibleRect::top().y + _ball->getContentSize().height) {
		_yVel -= 1.5 * FORCE_Y;
		_ball->setPositionY(_ball->getPositionY() + (_yVel * FIXED_TIME_STEP));
		this->ccTouchEnded(NULL, NULL);
	} else if (_ball->getPositionY()
			< VisibleRect::bottom().y - _ball->getContentSize().height) {
		this->gameOver();
		this->unscheduleUpdate();
	}

	//Ball Rotation
	Action* action = Sequence::create(
			EaseExponentialOut::create(RotateBy::create(dt, _rotation)),
			CallFunc::create(CC_CALLBACK_0(GameScene::finishRotateUp, this)),
			NULL);
	_ball->runAction(action);
	if (_rotation < -4)
		_rotation += 1;

	// Grounds Movement
	for (Vector<Ref*>::iterator it = _grounds->begin(); it < _grounds->end();
			it++) {
		Sprite* ground = (Sprite*) *it;

		// get the world position of the ground
		Point groundWorldPosition = _background->convertToWorldSpace(
				ground->getPosition());

		// get the screen position of the ground
		Point groundScreenPosition = this->convertToNodeSpace(
				groundWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (groundScreenPosition.x
				<= (-1 * ground->getContentSize().width * _scale)) {
			ground->setPosition(
					Point(
							ground->getPositionX()
									+ 4 * _scale
											* ground->getContentSize().width,
							ground->getPositionY()));
		}

	}

	//Supporters Movement
	for (Vector<Ref*>::iterator it = _supporters1->begin();
			it < _supporters1->end(); it++) {
		Sprite* supporter = (Sprite*) *it;

		// get the world position of the supporter
		Point supporterWorldPosition = _foreground->convertToWorldSpace(
				supporter->getPosition());

		// get the screen position of the supporter
		Point supporterScreenPosition = this->convertToNodeSpace(
				supporterWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (supporterScreenPosition.x
				<= (-1 * supporter->getContentSize().width) * _scale) {
			supporter->setPosition(
					Point(supporter->getPositionX() + _supporters1->size() * 62,
							supporter->getPositionY()));
		}
	}

	for (Vector<Ref*>::iterator it = _supporters2->begin();
			it < _supporters2->end(); it++) {
		Sprite* supporter = (Sprite*) *it;

		// get the world position of the supporter
		Point supporterWorldPosition = _foreground->convertToWorldSpace(
				supporter->getPosition());

		// get the screen position of the supporter
		Point supporterScreenPosition = this->convertToNodeSpace(
				supporterWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (supporterScreenPosition.x
				<= (-1 * supporter->getContentSize().width * _scale)) {
			supporter->setPosition(
					Point(supporter->getPositionX() + _supporters2->size() * 62,
							supporter->getPositionY()));
		}
	}

	for (Vector<Ref*>::iterator it = _supporters3->begin();
			it < _supporters3->end(); it++) {
		Sprite* supporter = (Sprite*) *it;

		// get the world position of the supporter
		Point supporterWorldPosition = _foreground->convertToWorldSpace(
				supporter->getPosition());

		// get the screen position of the supporter
		Point supporterScreenPosition = this->convertToNodeSpace(
				supporterWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (supporterScreenPosition.x
				<= (-1 * supporter->getContentSize().width * _scale)) {
			supporter->setPosition(
					Point(supporter->getPositionX() + _supporters3->size() * 62,
							supporter->getPositionY()));
		}
	}

	for (Vector<Ref*>::iterator it = _supportersForeground->begin();
			it < _supportersForeground->end(); it++) {
		Sprite* supporter = (Sprite*) *it;

		// get the world position of the supporter
		Point supporterWorldPosition = _foreground->convertToWorldSpace(
				supporter->getPosition());

		// get the screen position of the supporter
		Point supporterScreenPosition = this->convertToNodeSpace(
				supporterWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (supporterScreenPosition.x
				<= (-1 * supporter->getContentSize().width * _scale)) {
			supporter->setPosition(
					Point(
							supporter->getPositionX()
									+ _supportersForeground->size()
											* supporter->getContentSize().width,
							supporter->getPositionY()));
		}
	}

	//Logo Movement
	for (Vector<Ref*>::iterator it = _logo->begin(); it < _logo->end(); it++) {
		Sprite* logo = (Sprite*) *it;

		// get the world position of the supporter
		Point logoWorldPosition = _foreground->convertToWorldSpace(
				logo->getPosition());

		// get the screen position of the supporter
		Point logoScreenPosition = this->convertToNodeSpace(logoWorldPosition);

		// if the left corner is one complete width off the screen, move it to the right
		if (logoScreenPosition.x <= (-1 * logo->getContentSize().width)) {
			logo->setPosition(
					Point(logo->getPositionX() + _logo->size() * 300,
							logo->getPositionY()));
		}
	}

	//Remove obstacles
	Vector<Node*>* offScreenObstacles = new Vector<Node*>;

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Node* obstacle = (Node*) *it;

		Point obstacleWorldPosition = _foreground->convertToWorldSpace(
				obstacle->getPosition());
		Point obstacleScreenPosition = this->convertToNodeSpace(
				obstacleWorldPosition);

		if (obstacleScreenPosition.x + obstacle->getContentSize().width
				< -obstacle->getContentSize().width) {
			offScreenObstacles->pushBack(obstacle);
		}
	}

	for (Vector<Node*>::iterator it = offScreenObstacles->begin();
			it < offScreenObstacles->end(); it++) {
		Node* obstacleToRemove = (Node *) *it;
		obstacleToRemove->removeFromParentAndCleanup(true);
		_obstacles->eraseObject(obstacleToRemove);

		this->spawnNewObstacle();
	}

	checkForCollision(dt);
}

// Obstacle

void GameScene::spawnNewObstacle() {
	Node* previousObstacle = NULL;
	if (_obstacles->size() > 0) {
		previousObstacle = (Node*) _obstacles->back();
	}
	float previousObstacleXPosition;

	if (previousObstacle == NULL) {
		// this is the first obstacle
		previousObstacleXPosition = firstObstaclePosition;
	}

	else {
		previousObstacleXPosition = previousObstacle->getPositionX();
	}

	Point position = Point(previousObstacleXPosition + distanceBetweenObstacles,
			0);

	Obstacle* obstacle = Obstacle::create();
	obstacle->setPosition(position);
	obstacle->setupRandomPosition();

	_foreground->addChild(obstacle, DrawingOrderPipes);
	_obstacles->pushBack(obstacle);
}

// Collision

void GameScene::checkForCollision(float dt) {
	_timeWaitLabel += dt;
	CollisionType type;

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Obstacle* obstacle = (Obstacle *) *it;

		Point obstacleWorldPosition = _foreground->convertToWorldSpace(
				obstacle->getPosition());
		Point obstacleScreenPosition = this->convertToNodeSpace(
				obstacleWorldPosition);

		type = obstacle->checkCollisionWithPlayer(_ball,
				obstacleScreenPosition);

		if (type == CollisionTypePipe) {
			gameOver();
			this->unscheduleUpdate();
		} else if (type == CollisionTypeGoal) {
			if (_timeWaitLabel > _maxTimeWaitLabel) {
				int idSound = (rand() % 66) + 10;
				ostringstream idSoundString;
				idSoundString << "Suoni/passaggi/pass_" << idSound << ".mp3";
				SimpleAudioEngine::getInstance()->preloadEffect(
						idSoundString.str().c_str());
				SimpleAudioEngine::getInstance()->playEffect(
						idSoundString.str().c_str());

				_timeWaitLabel = 0;
				updateScoreLabel();
			}
		}
	}
}

// Touch

bool GameScene::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) {
	if (!_isGameOver) {
		_isTap = true;

		int idSound = (rand() % 6) + 1;

		ostringstream idSoundString;
		idSoundString << "Suoni/ball/ball_" << idSound << ".mp3";

		SimpleAudioEngine::getInstance()->preloadEffect(
				idSoundString.str().c_str());
		SimpleAudioEngine::getInstance()->playEffect(
				idSoundString.str().c_str());

	}
	return true;
}

void GameScene::onTouchEnded(Touch *pTouch, Event *pEvent) {
	if (!_isGameOver) {
		finishRotateUp();
	}
}

void GameScene::finishRotateUp() {
	_ball->stopActionByTag(10);
	Action* action = EaseExponentialIn::create(
			RotateTo::create(0.6, DEFAULT_ROTATION_DOWN));
	action->setTag(10);
	_ball->runAction(action);
}

void GameScene::gameOver() {
	SimpleAudioEngine::getInstance()->stopAllEffects();
	SimpleAudioEngine::getInstance()->preloadEffect("Suoni/splat.mp3");
	SimpleAudioEngine::getInstance()->playEffect("Suoni/splat.mp3");
	int idSound = (rand() % 5) + 1;
	ostringstream idSoundString;
	idSoundString << "Suoni/lose/lose_0" << idSound << ".mp3";
	SimpleAudioEngine::getInstance()->preloadEffect(
			idSoundString.str().c_str());
	SimpleAudioEngine::getInstance()->playEffect(idSoundString.str().c_str());
	_isGameOver = true;

	_label->setVisible(false);

	this->hideNewRecordLabel();
	_ball->stopAllActions();
	_ball->runAction(
			MoveTo::create(_ball->getPositionY() / _screenSize.height,
					Point(_ball->getPositionX(),
							VisibleRect::bottom().y
									- _ball->getContentSize().height)));
	_ball->runAction(
			EaseExponentialIn::create(
					RotateTo::create(0.3, DEFAULT_ROTATION_DOWN)));

	LayerColor* layer = LayerColor::create(Color4B(255, 255, 255, 255),
			_screenSize.width, _screenSize.height);
	this->addChild(layer, LayerDrawingForeground);
	layer->setOpacity(0);

	layer->runAction(
			Sequence::create(FadeIn::create(0.1), FadeOut::create(0.1),
					RemoveSelf::create(), NULL));

	float offset = 3;

	Point initialPosition = this->getPosition();

	MoveTo* moveUpL = MoveTo::create(0.02,
			Point(this->getPositionX() - offset,
					this->getPositionY() + offset));
	MoveTo* moveUpR = MoveTo::create(0.02,
			Point(this->getPositionX() + offset,
					this->getPositionY() + offset));
	MoveTo* moveDownL = MoveTo::create(0.02,
			Point(this->getPositionX() - offset,
					this->getPositionY() - offset));
	MoveTo* moveDownR = MoveTo::create(0.02,
			Point(this->getPositionX() + offset,
					this->getPositionY() - offset));

	this->runAction(
			CCSequence::create(moveUpL, moveUpR, moveDownR, moveDownL,
					moveDownR, moveUpL, moveDownL, moveUpR, moveUpL, moveUpR,
					moveDownR, moveDownL, moveDownR, moveUpL, moveDownL,
					moveUpR, moveDownR, moveDownL, moveDownR, moveUpL,
					moveDownL, moveUpR, Place::create(initialPosition),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::callbackGameOver, this)),
					NULL));

}

void GameScene::callbackGameOver() {

	//--INTERSTITIAL CHARTBOOST--
	if (++_counter % 3 == 0) {
		ChartboostX::sharedChartboostX()->showInterstitial();
	}

	//--Set game state
	_state = GameState::ENDMENU;

//Remove obstacles from screen
	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Node* obstacleToRemove = (Node *) *it;
		obstacleToRemove->removeFromParentAndCleanup(true);
	}

	_ball->setVisible(false);

	_layerColorWait->removeAllChildrenWithCleanup(true);
	_layerColorWait->setVisible(true);

	Sprite* balo = Sprite::create("PoseMario/baloBusto.png");
	balo->setScale(_scale);
	balo->setAnchorPoint(Point(0.5, 0));
	balo->setPosition(Point(_layerColorWait->getContentSize().width / 2, 164));
	_layerColorWait->addChild(balo);
	Sprite* bocca = Sprite::create("PoseMario/bocca_01.png");
	bocca->setAnchorPoint(Point(0, 0));
	bocca->setPosition(Point::ZERO);
	balo->addChild(bocca);

	//Score Label
	ostringstream scoreStr;
	scoreStr << _score;
	Label* score = Label::createWithBMFont(s_FontText, scoreStr.str());
	score->setScale(_scale);
	score->setAnchorPoint(Point(0.5, 0));
	score->setPosition(Point(_layerColorWait->getContentSize().width / 2, 560));
	_layerColorWait->addChild(score);

	//Restart Button
	MenuItemImage* restartItem = MenuItemImage::create("rootBtn.png",
			"rootBtnTap.png", CC_CALLBACK_1(GameScene::restart,this));
	restartItem->setAnchorPoint(Point(0.5, 0));
	restartItem->setScale(_scale);

	restartItem->setPosition(Point(0, -246));

	Label* label = Label::createWithBMFont(s_FontText, "restart");
	label->setScale(0.6);
	label->setPosition(
			Point(restartItem->getContentSize().width / 2,
					restartItem->getContentSize().height / 2));
	restartItem->addChild(label);

	Menu* pMenu = Menu::create(restartItem, NULL);

	_layerColorWait->addChild(pMenu);
}

void GameScene::restart(Ref *obj) {
	int idSound = (rand() % 5) + 1;
	ostringstream idSoundString;
	idSoundString << "Suoni/tasti/tasti_0" << idSound << ".mp3";
	SimpleAudioEngine::getInstance()->preloadEffect(
			idSoundString.str().c_str());
	SimpleAudioEngine::getInstance()->playEffect(idSoundString.str().c_str());
	_layerColorWait->removeAllChildrenWithCleanup(true);
	this->callbackRestart();

	//--FLURRY EVENT--
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	AnalyticX::flurryLogEvent("RESTART CLICKED");
#endif
}

void GameScene::callbackRestart() {
	this->resetGameAttribute();

	this->runAction(
			CCSequence::createWithTwoActions(DelayTime::create(0.8),
					CallFunc::create(
							CC_CALLBACK_0(GameScene::resetScene, this))));
}

void GameScene::resetGameAttribute() {
	_yVel = 0;
	_isTap = false;
	_isGameOver = false;
	_isNewRecord = false;
	_timeWaitLabel = 0;
	_score = -1;
	_idSoundNewGroupScore = 10;
	_label->setVisible(true);

	_layerColorWait->removeAllChildrenWithCleanup(true);
	_layerColorWait->setVisible(false);
	_background->setPosition(Point::ZERO);

	_midground->setPosition(Point::ZERO);
	_foreground->setPosition(Point::ZERO);

	int x = 0;
	//Reset Ground
	for (Vector<Ref*>::iterator it = _grounds->begin(); it < _grounds->end();
			it++) {
		Sprite* bg = (Sprite*) *it;
		bg->setPosition(Point(x, bg->getPositionY()));
		x += bg->getContentSize().width * _scale;
	}

	//Reset Supporters
	x = -100;
	for (Vector<Ref*>::iterator it = _supporters1->begin();
			it < _supporters1->end(); it++) {
		Sprite* supporter = (Sprite*) *it;
		supporter->setPosition(Point(x, supporter->getPositionY()));
		x += 62;
	}
	x = -70;
	for (Vector<Ref*>::iterator it = _supporters2->begin();
			it < _supporters2->end(); it++) {
		Sprite* supporter = (Sprite*) *it;
		supporter->setPosition(Point(x, supporter->getPositionY()));
		x += 62;
	}
	x = -100;
	for (Vector<Ref*>::iterator it = _supporters3->begin();
			it < _supporters3->end(); it++) {
		Sprite* supporter = (Sprite*) *it;
		supporter->setPosition(Point(x, supporter->getPositionY()));
		x += 62;
	}
	x = -100;
	for (Vector<Ref*>::iterator it = _supportersForeground->begin();
			it < _supportersForeground->end(); it++) {
		Sprite* supporter = (Sprite*) *it;
		supporter->setPosition(Point(x, supporter->getPositionY()));
		x += supporter->getContentSize().width;
	}
	x = 150;
	for (Vector<Ref*>::iterator it = _logo->begin(); it < _logo->end(); it++) {
		Sprite* logo = (Sprite*) *it;
		logo->setPosition(Point(x, logo->getPositionY()));
		x += 300;
	}

	for (Vector<Node*>::iterator it = _obstacles->begin();
			it < _obstacles->end(); it++) {
		Node* obstacleToRemove = (Node *) *it;
		obstacleToRemove->removeFromParentAndCleanup(true);
	}

	_obstacles->clear();

	_ball->setPosition(Point(200, 400));
	_ball->setRotation(0);
	_ball->setVisible(true);

	this->spawnNewObstacle();
	this->spawnNewObstacle();
	this->spawnNewObstacle();
}

void GameScene::resetScene() {
	this->updateScoreLabel();

	//--Set game state
	_state = GameState::RUNNING;

	this->scheduleUpdate();
}

void GameScene::updateScoreLabel() {
	_score++;

	if (_score > 0 && _score % 10 == 0) {

		ostringstream idSoundString;
		idSoundString << "Suoni/" << _idSoundNewGroupScore << ".mp3";

		SimpleAudioEngine::getInstance()->playEffect(
				idSoundString.str().c_str());

		_idSoundNewGroupScore++;

		if (_idSoundNewGroupScore > 34) {
			_idSoundNewGroupScore = 34;
		}
	}

	if (_score
			> atoi(
					CCUserDefault::getInstance()->getStringForKey(
							"currentScore", "").c_str())) {
//		showNewRecordLabel();

		ostringstream score;
		score << _score;
		CCUserDefault::getInstance()->setStringForKey("currentScore",
				score.str().c_str());
		CCUserDefault::getInstance()->flush();
	}

	ostringstream score;
	score << _score;
	_label->setString(score.str());
}

void GameScene::showNewRecordLabel() {
//	if (!_isNewRecord) {
//		_isNewRecord = true;
//
//		_newRecordLabel->runAction(
//				EaseBackOut::create(
//						MoveTo::create(0.5,
//								Point(_screenSize.width / 2,
//										_screenSize.height - 80))));
//		_recordRays->runAction(FadeIn::create(0.5));
//		_recordRays->runAction(
//				RepeatForever::create(RotateBy::create(8, -360)));
//
//		this->runAction(
//				Sequence::create(DelayTime::create(3),
//						CallFunc::create(
//								CC_CALLBACK_0(GameScene::hideNewRecordLabel, this)),
//						NULL));
//
//		SimpleAudioEngine::getInstance()->preloadEffect("Suoni/new_record.mp3");
//		SimpleAudioEngine::getInstance()->playEffect("Suoni/new_record.mp3");
//	}
}

void GameScene::hideNewRecordLabel() {
//	_newRecordLabel->runAction(
//			EaseBackIn::create(
//					MoveTo::create(0.5,
//							Point(_screenSize.width / 2,
//									_screenSize.height
//											+ _newRecordLabel->getContentSize().height
//													* 2))));
//
//	_recordRays->stopAllActions();
//
//	if (_recordRays->getOpacity() > 0) {
//		_recordRays->runAction(CCFadeOut::create(0.5));
//	}
}

void GameScene::bannerPressed(Ref *obj) {
	//NdkHelper
	//SendMessageWithParams(string("openApp"),NULL);
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		if (_state == GameState::MAINMENU) {
			if (_backKeyLayer) {
				_backKeyLayer->removeFromParentAndCleanup(true);
				_backKeyLayer = NULL;
				return;
			}

			int idSound = (rand() % 8) + 1;
			ostringstream idSoundString;
			idSoundString << "Suoni/exit/exit_0" << idSound << ".mp3";
			SimpleAudioEngine::getInstance()->preloadEffect(
					idSoundString.str().c_str());
			SimpleAudioEngine::getInstance()->playEffect(
					idSoundString.str().c_str());

			_backKeyLayer = LayerColor::create(Color4B(9, 13, 73, 102),
					_screenSize.width, _screenSize.height);
			_backKeyLayer->setPosition(Point::ZERO);
			_backKeyLayer->setOpacity(0);

			//--CANCEL BUTTON--
			auto pCancelItem = MenuItemImage::create("rootBtn.png",
					"rootBtnTap.png",
					CC_CALLBACK_1(GameScene::cancelLeaveGameCallback, this));
			pCancelItem->setPosition(
					Point(0,
							-2 * _scale
									* pCancelItem->getContentSize().height));
			pCancelItem->setScale(_scale);

			//FLY AGAIN label
			auto label = Label::createWithBMFont(s_FontText, "Play Again!");
			label->setScale(0.7);
			label->setPosition(
					Point(pCancelItem->getContentSize().width / 2,
							pCancelItem->getContentSize().height / 2));
			pCancelItem->addChild(label);

			//--CONFIRM BUTTON--
			auto pConfirmItem = MenuItemImage::create("rootBtn.png",
					"rootBtnTap.png",
					CC_CALLBACK_1(GameScene::confirmLeaveGameCallback,this));

			//YES, PLEASE BUTTON
			label = Label::createWithBMFont(s_FontText, "Please Exit");
			label->setScale(0.6);
			label->setPosition(
					Point(pConfirmItem->getContentSize().width / 2,
							pConfirmItem->getContentSize().height / 2));
			pConfirmItem->addChild(label);
			pConfirmItem->setPosition(
					Point(0,
							-3 * _scale * pCancelItem->getContentSize().height
									- 20));
			pConfirmItem->setScale(0.8f * _scale);

			Menu* pMenu = Menu::create(pConfirmItem, pCancelItem, NULL);

			//Really quit Fly Tony?
			label = Label::createWithBMFont(s_FontText,
					"\tReally quit\nFlappy Balo?");

			auto menuLabel = MenuItemLabel::create(label, [&](Ref* sender) {});
			menuLabel->setScale(0.7 * _scale);
			menuLabel->setPosition(
					Point(0, label->getContentSize().height / 2));

			pMenu->addChild(menuLabel);

			_backKeyLayer->addChild(pMenu);

			this->addChild(_backKeyLayer, 100);

			_backKeyLayer->runAction(FadeTo::create(0.2, 200));
		} else if (_state == GameState::ENDMENU) {
			this->resetGameAttribute();
			this->updateScoreLabel();
			_layerColorWait->setVisible(true);
			_ball->setVisible(false);
			_label->setVisible(false);
			this->showMainMenu();
		}
	}
}

void GameScene::confirmLeaveGameCallback(Ref* pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//exit(0);
#endif

	CCLOG("exited 0");
}

void GameScene::cancelLeaveGameCallback(Ref* pSender) {
	_backKeyLayer->removeFromParentAndCleanup(true);
	_backKeyLayer = NULL;
}

