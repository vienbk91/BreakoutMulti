/*
 * SelectTeamScene.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "SelectTeamScene.h"

Scene* SelectTeamScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SelectTeamScene::create();

	scene->addChild(layer);

	return scene;
}


bool SelectTeamScene::init()
{
	if(!Layer::init()){
		return false;
	}

	_visibleSize = Director::getInstance()->getVisibleSize();

	//======================================================
	// Select team A

	auto teamABtn = Button::create();
	teamABtn->loadTextureNormal("start.png");
	teamABtn->setTouchEnabled(true);
	teamABtn->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 + 100));
	teamABtn->addTouchEventListener(CC_CALLBACK_2(SelectTeamScene::SelectTeamBtnCallback , this , 1));
	this->addChild(teamABtn);



	//======================================================
	// Select team B

	auto teamBBtn = Button::create();
	teamBBtn->loadTextureNormal("start.png");
	teamBBtn->setTouchEnabled(true);
	teamBBtn->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 100));
	teamBBtn->addTouchEventListener(CC_CALLBACK_2(SelectTeamScene::SelectTeamBtnCallback, this , 2));
	this->addChild(teamBBtn);

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		teamABtn->setScale(1.0f);
		teamBBtn->setScale(1.0f);
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		teamABtn->setScale(2.0f);
		teamBBtn->setScale(2.0f);
	}



	return true;
}

void SelectTeamScene::SelectTeamBtnCallback(Ref* pSender, Widget::TouchEventType type , int teamId)
{
	if (teamId == 1){
		auto client = NodeServer::getInstance()->getClient();

	}
	Director::getInstance()->replaceScene(TransitionMoveInR::create(0.25f , PlayGame::createScene() ) );
}

