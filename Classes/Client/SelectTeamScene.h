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

class SelectTeamScene : public Layer
{
public:
	static Scene* createScene();
	virtual bool init();

	CREATE_FUNC(SelectTeamScene);

	Size _visibleSize;

private:

	void SelectTeamBtnCallback(Ref* pSender , Widget::TouchEventType type , int teamId);

};

#endif /* SELECTTEAMSCENE_H_ */
