/*
* SelectTeamScene.h
*
*  Created on: 13 Jul 2015
*      Author: chu
*/

#ifndef SELECTTEAMSCENE_H_
#define SELECTTEAMSCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "StartGameScene.h"
#include "PlayGameScene.h"
#include "Server/NodeJSServer.h"


USING_NS_CC;
using namespace ui;


#define PLAYER_NUM "PLAYER_NUM"

class SelectTeamScene : public Layer
{
public:
	static Scene* createScene(vector<RoomPlayer> allPlayer);
	static SelectTeamScene* create(vector<RoomPlayer> allPlayer);
	virtual bool init(vector<RoomPlayer> allPlayer);


	Size _visibleSize;

	vector<RoomPlayer> _allPlayer;

public:

	bool _player1ConnectedFlg = false;
	bool _player2ConnectedFlg = false;


private:

	Button* _teamABtn;
	Button* _teamBBtn;

	void SelectTeamBtnCallback(Ref* pSender, Widget::TouchEventType type, int teamId);
	void connectedCallback(SIOClient* client, const string& data);

	void update(float dt);
	void realtimeCheckData();

};

#endif /* SELECTTEAMSCENE_H_ */
