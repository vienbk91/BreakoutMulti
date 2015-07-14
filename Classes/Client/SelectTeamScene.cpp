/*
 * SelectTeamScene.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "SelectTeamScene.h"

// Su dung cho socket.io
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

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
	teamABtn->loadTextureNormal("player1.png");
	teamABtn->setTouchEnabled(true);
	teamABtn->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 + 100));
	teamABtn->addTouchEventListener(CC_CALLBACK_2(SelectTeamScene::SelectTeamBtnCallback , this , 1));
	this->addChild(teamABtn);



	//======================================================
	// Select team B

	auto teamBBtn = Button::create();
	teamBBtn->loadTextureNormal("player2.png");
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

	string textTest;

	std::stringstream str;
	str << teamId;

	textTest = "{\"value\":\"" + str.str() +"\"}";

	auto client = NodeServer::getInstance()->getClient();
	client->emit("hello", textTest);


	// Lay trang thai connect cuar player tu server
	client->on("connect_end", CC_CALLBACK_2(SelectTeamScene::checkPlayerConnectEvent, this));
}


/*
Check player connect status using get event "hello" data from server
*/

void SelectTeamScene::checkPlayerConnectEvent(SIOClient* client, const string& data)
{
	log("Co vao day khong vay");
	log("Data : %s", data.c_str());

	rapidjson::Document document;
	document.Parse<0>(data.c_str());

	log("Data Value : %s", document["value"].GetString());

	if (document.HasParseError()){
		log("//=============Parse Error!!!");
		return;
	}

	// Lay data
	if (document.IsObject()){
		// Neu ton tai truong co key = value
		if (document.HasMember("value"))
		{
			// Lay gia tri cua truong value
			std::string value = document["value"].GetString();

			// Ca 2 nguoi choi da dang nhap
			if (strcmp(value.c_str(), "2") == 0)
			{
				Director::getInstance()->replaceScene(TransitionMoveInR::create(0.25f, PlayGame::createScene()));
			}

		}
	}
}

