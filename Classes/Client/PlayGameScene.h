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
	static Scene* createScene(int teamId);
	virtual bool init(int teamId);
	static PlayGame* create(int teamId); // tuong tu ham CC_FUNC(PlayGame);

	Size _visibleSize;

	vector<RoomPlayer> _allPlayers;
	RoomPlayer _player1Data;
	RoomPlayer _player2Data;

	void onEnter();

public :
	int _teamId;

	SIOClient* _client;

	Sprite* _paddle1;
	Sprite* _paddle2;
	Sprite* _ball;

private:
	
	
	Node* _border;
	Node* _wallDie1;
	Node* _wallDie2;

	Label* _player1Score;
	Label* _player2Score;

	PhysicsWorld* _mWorld;

	void getFirstData();

	void setLayerPhysicsWorld(PhysicsWorld* world);

	void createGameBorder();
	void createContent();
	
	Sprite* createBall();
	Sprite* createPaddle();
	Label* createScoreLabel();


	void playGameContent();
	void update(float dt);

	bool onContactBegin(PhysicsContact& contact);

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

};


#endif /* PLAYGAMESCENE_H_ */
