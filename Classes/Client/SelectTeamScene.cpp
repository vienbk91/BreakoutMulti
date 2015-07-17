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

Scene* SelectTeamScene::createScene(vector<RoomPlayer> allPlayer)
{
	auto scene = Scene::create();
	auto layer = SelectTeamScene::create(allPlayer);

	scene->addChild(layer);

	return scene;
}

SelectTeamScene* SelectTeamScene::create(vector<RoomPlayer> allPlayer)
{
	auto layer = new SelectTeamScene();
	if (layer && layer->init(allPlayer)){
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}


bool SelectTeamScene::init(vector<RoomPlayer> allPlayer)
{
	if(!Layer::init()){
		return false;
	}



	_allPlayer = allPlayer;

	_visibleSize = Director::getInstance()->getVisibleSize();

	//======================================================
	// Select team A

	_teamABtn = Button::create();
	_teamABtn->loadTextureNormal("player1.png");
	_teamABtn->setTouchEnabled(true);
	_teamABtn->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 + 100));
	_teamABtn->addTouchEventListener(CC_CALLBACK_2(SelectTeamScene::SelectTeamBtnCallback , this , 1));
	this->addChild(_teamABtn);



	//======================================================
	// Select team B

	_teamBBtn = Button::create();
	_teamBBtn->loadTextureNormal("player2.png");
	_teamBBtn->setTouchEnabled(true);
	_teamBBtn->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 100));
	_teamBBtn->addTouchEventListener(CC_CALLBACK_2(SelectTeamScene::SelectTeamBtnCallback, this, 2));
	this->addChild(_teamBBtn);

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		_teamABtn->setScale(1.0f);
		_teamBBtn->setScale(1.0f);
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		_teamABtn->setScale(2.0f);
		_teamBBtn->setScale(2.0f);
	}

	//========================================================
	// Kiem tra du lieu trong mongodb de kiem tra trang thai cua player
	/*
	Khoi tao csdl ten la room bang nodejs
	Co chua 2 nguoi choi , default info do la player_id = {1,2} , status : false( database da duoc khoi tao bang mongodb)
	Thuc hien ket noi voi 1 lop Model de co the thao tac voi du lieu mongodb

	Kiem tra du lieu da khoi tao
	Khi opemRoom thi se check du lieu trong database, neu status la false thi de o trang thai ban dau, neu status = true thi chuyen trang thai da connect
	*/

	if (allPlayer[0].player_id == 1 && (allPlayer[0].status == true) ){
		_teamABtn->loadTextureNormal("player1_cnt.png");
		_teamABtn->setTouchEnabled(false);
	}

	if (allPlayer[1].player_id == 2 && (allPlayer[1].status == true)){
		_teamBBtn->loadTextureNormal("player2_cnt.png");
		_teamBBtn->setTouchEnabled(false);
	}



	return true;
}

void SelectTeamScene::SelectTeamBtnCallback(Ref* pSender, Widget::TouchEventType type , int teamId)
{


	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{

		// Update thong tin cua database khi player connect

		//======================================================
		// Update mongodb after button click , change status

		auto client = NodeServer::getInstance()->getClient();

		switch (teamId)
		{
		case 1:
			_allPlayer[0].status = true;
			break;
		case 2:
			_allPlayer[1].status = true;
		default:
			break;
		}

		std::stringstream connectMsg;

		connectMsg << "[{\"player_id\":" << _allPlayer[0].player_id << " , \"status\":" << _allPlayer[0].status 
			<< "} , {\"player_id\":" << _allPlayer[1].player_id << " , \"status\":" << _allPlayer[1].status << "}]";

		client->emit("player_connect", connectMsg.str());

		client->on("player_connect_end", [&](SIOClient* client, const std::string& data){

			// Thuc hien viec lay du lieu ban dau cua database
			log("Data : %s", data.c_str());

			rapidjson::Document document;

			document.Parse<0>(data.c_str());

			bool error = document.HasParseError();
			if (error){
				log("//=============Parse Error!!!");
				return;
			}

			// Lay data
			if (document.IsObject() == true)
			{
				// Neu ton tai truong co key = value
				if (document.HasMember("room"))
				{
					// Lay gia tri cua truong value
					log("=====================================");
					const rapidjson::Value& obj = document["room"];
					rapidjson::SizeType num = obj.Size();

					for (rapidjson::SizeType i = 0; i < num; i++)
					{
						RoomPlayer temp;

						temp.player_id = obj[i]["player_id"].GetInt();
						temp.status = obj[i]["status"].GetBool();

						log("PlayerId : %d", obj[i]["player_id"].GetInt());

						if (_allPlayer[i].player_id == temp.player_id){
							_allPlayer[i].status = temp.status;
						}
					}
				}
			}

			log("Player1 : Id = %d , status = %d ", _allPlayer[0].player_id, _allPlayer[0].status);
			log("Player2 : Id = %d , status = %d ", _allPlayer[1].player_id, _allPlayer[1].status);


			if (_allPlayer[0].player_id == 1 && (_allPlayer[0].status == true)){
				_teamABtn->loadTextureNormal("player1_cnt.png");
				_teamABtn->setTouchEnabled(false);
			}

			if (_allPlayer[1].player_id == 2 && (_allPlayer[1].status == true)){
				_teamBBtn->loadTextureNormal("player2_cnt.png");
				_teamBBtn->setTouchEnabled(false);
			}
		});


		


		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void SelectTeamScene::connectedCallback(SIOClient* client, const string& data)
{

}

