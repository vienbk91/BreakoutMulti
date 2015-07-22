


#include "StartGameScene.h"


// Su dung cho socket.io
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

USING_NS_CC;
using namespace rapidjson;


Scene* StartGameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = StartGameScene::create();

	scene->addChild(layer);

	return scene;
}

bool StartGameScene::init()
{
	if (!Layer::init()){
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	//=====================================================
	//Create button Start Game
	Button* startGameBtn = Button::create();

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		startGameBtn->setScale(1.0f);
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		startGameBtn->setScale(2.0f);
	}

	startGameBtn->loadTextureNormal("start.png");
	startGameBtn->setTouchEnabled(true);
	startGameBtn->addTouchEventListener(CC_CALLBACK_2(StartGameScene::startGameCallback , this));
	startGameBtn->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(startGameBtn);


	auto testBtn = Button::create();
	testBtn->loadTextureNormal("test.png");
	testBtn->setTouchEnabled(true);
	testBtn->addTouchEventListener(CC_CALLBACK_2(StartGameScene::testBtnCallback, this));
	testBtn->setPosition(Vec2(200, 200));

	this->addChild(testBtn);



	auto client = NodeServer::getInstance();


	return true;
}

void StartGameScene::startGameCallback(Ref* pSender , Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{

		//======================================================
		// Create connect with mongodb

		auto client = NodeServer::getInstance()->getClient();

		// Gui toi server 1 event yeu cau check data tu mongodb
		string connectMsg;

		connectMsg = "Connect to mongoDB";
		client->emit("create_db", connectMsg);
		client->on("create_db_end", [&](SIOClient* client, const std::string& data){
			
			log("Callback tai day");

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
			if (document.IsObject() == true){
				// Neu ton tai truong co key = value
				if (document.HasMember("room"))
				{
					// Lay gia tri cua truong value
					log("=====================================");
					const rapidjson::Value& obj = document["room"];
					rapidjson::SizeType num = obj.Size();

					for (rapidjson::SizeType i = 0; i < num ; i++)
					{
						RoomPlayer temp;

						temp.player_id = obj[i]["player_id"].GetInt();
						temp.status = obj[i]["status"].GetBool();

						log("PlayerId : %d", obj[i]["player_id"].GetInt());

						_allPlayer.push_back(temp);

					}

				}
			}


			Director::getInstance()->replaceScene(TransitionMoveInR::create(0.5f, SelectTeamScene::createScene(_allPlayer)));
		});

		
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}


void StartGameScene::testBtnCallback(Ref* pSender, Widget::TouchEventType type){
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//======================================================
		// Khoi tao du lieu test

		auto client = NodeServer::getInstance()->getClient();

		std::stringstream connectMsg;

		connectMsg << "[{\"player_id\":" << 1 << " , \"status\":" << true << ", \"score\":" << 0
			<< "} , {\"player_id\":" << 2 << " , \"status\":" <<true << " , \"score\":"<< 0  << "}]";

		client->emit("player_test", connectMsg.str());

		Director::getInstance()->replaceScene(TransitionMoveInR::create(0.5f , PlayGame::createScene(1)));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}