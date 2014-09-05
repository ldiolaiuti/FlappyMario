//
//  GameScene.h
//  FlappyMario
//
//  Created by Luca Diolaiuti on 20/06/14.
//
//

#ifndef __FlappyMario__GameScene__
#define __FlappyMario__GameScene__
#include <iostream>
#include "cocos2d.h"
#include "NDKHelper/NDKHelper.h"

USING_NS_CC;

class GameScene: public Layer{
    
	enum class GameState{
		MAINMENU,
		RUNNING,
		ENDMENU,
		BACKMENU
	};
    
	GameState _state;
    
	Size _screenSize;
	Node * _background;
	Node * _foreground;
	Sprite * _ball;
	Label* _label;
	LayerColor* _mainMenuLayer=NULL;
	LayerColor* _endMenuLayer=NULL;
	LayerColor* _backKeyLayer=NULL;
    
	int _score;
	int _counter=0;
	int _idSoundNewGroupScore;
	float _yVel;
	float _scrollSpeedBackground;
	float _scrollSpeedMidGround;
	float _scrollSpeedForeground;
	bool _isTap;
	bool _isNewRecord;
	bool _isGameOver;
	float _timeWaitLabel;
	float _maxTimeWaitLabel;
	float _rotation=-4;
    int _scale;
    
	Vector<Ref*>* _grounds;
	Vector<Ref*>* _supporters1;
	Vector<Ref*>* _supporters2;
	Vector<Ref*>* _supporters3;
	Vector<Ref*>* _supportersForeground;
	Vector<Ref*>* _logo;
	Vector<Node*>* _obstacles;
	Animation* _supporterSincro;
	Animation* _supporterNonSincro;
	Animation* _supporterOther;
    
	// Draw Layout
	void drawLayout();
	void drawBackground();
	void drawScoreLabel();
	void drawPlayer();
	Sprite* drawSupporter();

	//Menu Layer
	void showExitMenu();
	void showMainMenu();
	void showEndMenu(int score);
    
	// Init
	void flyBall(Ref * obj);
	void callbackFlyBall();
    
	//New Obstacle
	void spawnNewObstacle();
    
	//GameOver
	void gameOver();
	void callbackGameOver();
	void restart(Ref *obj);
	void callbackRestart();
    
	//Reset
	void resetGameAttribute();
	void resetScene();
    
	// Collision
	void checkForCollision(float dt);
	void updateScoreLabel();
    
	//Record
	void showNewRecordLabel();
	void hideNewRecordLabel();
    
	//Banner
	void bannerPressed(Ref * obj);
    
	//Callback
	void confirmLeaveGameCallback(Ref *obj);
	void cancelLeaveGameCallback(Ref *obj);
    
public:
    
	GameScene();
	virtual ~GameScene();
    
	virtual void onEnter();
	virtual void onExit();
	virtual bool init();
	virtual void update(float dt);
	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);
	virtual void onTouchEnded(Touch *pTouch, Event *pEvent);
    
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
	static Scene* scene();
    
	void finishRotateUp();
    
	CREATE_FUNC(GameScene)
	;
    
};

#endif /* defined(__FlappyMario__GameScene__) */
