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
#include "Server/NodeJSServer.h"


USING_NS_CC;
using namespace ui;
using namespace std;

class PlayGame : public Layer
{
public:
	static Scene* createScene(vector<RoomPlayer> allPlayer);
	virtual bool init(vector<RoomPlayer> allPlayer);

	static PlayGame* create(vector<RoomPlayer> allPlayer);

	Size _visibleSize;

	vector<RoomPlayer> _allPlayer;

private:
	Sprite* _ball;
	Sprite* _paddle1;
	Sprite* _paddle2;
	Node* _border;

	Label* _player1Status;
	Label* _player2Status;

	PhysicsWorld* _mWorld;

	void setLayerPhysicsWorld(PhysicsWorld* world);

	void createGameBorder();
	void createContent();
	
	Sprite* createBall();
	Sprite* createPaddle();
	Label* createStatusLabel();

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	bool onContactBegin(PhysicsContact& contact);

	void update(float dt);

	void checkPlayerConnectEvent(SIOClient* client, const string& data);

};


#endif /* PLAYGAMESCENE_H_ */
