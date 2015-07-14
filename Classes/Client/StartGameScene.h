#ifndef _STARTGAMESCENE_H_
#define _STARTGAMESCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "SelectTeamScene.h"
#include "Server/NodeJSServer.h"

USING_NS_CC;
using namespace ui;

class StartGameScene : public Layer
{
public:
	static Scene* createScene();
	virtual bool init();

	CREATE_FUNC(StartGameScene);

private:

	void startGameCallback(Ref* pSender , Widget::TouchEventType type);

};

#endif //_STARTGAMESCENE_H_
