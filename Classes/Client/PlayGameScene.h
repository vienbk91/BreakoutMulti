/*
 * PlayGameScene.h
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#ifndef PLAYGAMESCENE_H_
#define PLAYGAMESCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "SelectTeamScene.h"
#include "Define.h"

USING_NS_CC;
using namespace ui;

class PlayGame : public Layer
{
public:
	static Scene* createScene();
	virtual bool init();

	CREATE_FUNC(PlayGame);

	Size _visibleSize;

private:
	Sprite* _ball;
	Sprite* _paddle1;
	Sprite* _paddle2;
	Node* _border;

	PhysicsWorld* _mWorld;

	void setLayerPhysicsWorld(PhysicsWorld* world);

	void createGameBorder();
	void createContent();
	
	Sprite* createBall();
	Sprite* createPaddle();

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	bool onContactBegin(PhysicsContact& contact);

};


#endif /* PLAYGAMESCENE_H_ */
