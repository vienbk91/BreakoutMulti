/*
 * PlayGameScene.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "PlayGameScene.h"


// Su dung cho socket.io
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"


#define WIDTH_OFFSET 5
#define HEIGHT_OFFSET 100

Scene* PlayGame::createScene()
{
	// Khoi tao scene voi thuoc tinh vat ly
	auto scene = Scene::createWithPhysics();
	// Tao physics debug
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	// Thiet lap trong luong bang 0
	scene->getPhysicsWorld()->setGravity(Vect(0.0f , 0.0f));

	auto layer = PlayGame::create();


	scene->addChild(layer);

	return scene;
}


void PlayGame::setLayerPhysicsWorld(PhysicsWorld* world)
{
	_mWorld = world;
}


PlayGame* PlayGame::create()
{
	auto layer = new PlayGame();
	if (layer && layer->init()){
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}


bool PlayGame::init()
{
	if (!Layer::init())
	{
		return false;
	}


	_visibleSize = Director::getInstance()->getVisibleSize();


	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayGame::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayGame::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(PlayGame::onTouchEnded, this);


	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(PlayGame::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}


void PlayGame::onEnter()
{
	Layer::onEnter();

	// Lay du lieu dau vao

	auto sequence = Sequence::create(CallFuncN::create([&](Ref* pSender){
		getFirstData();
	}), DelayTime::create(1.0f) , CallFuncN::create([&](Ref* pSender){
		// Tao UI game
		createContent();
		createGameBorder();
	}), nullptr);

	this->runAction(sequence);


	// Tao chuyen dong cua bong

	Vect force = Vect(1010000.0f, 1010000.0f);
	_ball->getPhysicsBody()->applyImpulse(force);

}

/*
Khoi tao noi dung game
*/
void PlayGame::createContent()
{
	log("//======================>>>> Create content UI");
	// Create ball
	_ball = createBall();
	_ball->setPosition(Vec2(_visibleSize.width/2 , _visibleSize.height/2));

	// Create paddle

	float offset;

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		offset = 30;
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		offset = 60;
	}

	_paddle1 = createPaddle();
	_paddle1->setPosition(Vec2(_visibleSize.width / 2, offset + HEIGHT_OFFSET  ));
	_paddle2 = createPaddle();
	_paddle2->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - HEIGHT_OFFSET - offset ));


	// Create status 
	Label* connectLb1 = Label::create("Player1 : ", "fonts/arial.ttf", 30);
	connectLb1->setColor(Color3B::WHITE);
	connectLb1->setHorizontalAlignment(TextHAlignment::CENTER);
	connectLb1->setPosition(Vec2(_visibleSize.width/2 - 20 - connectLb1->getContentSize().width/2 , HEIGHT_OFFSET/2));
	this->addChild(connectLb1);


	_player1Score = createScoreLabel();
	_player1Score->setPosition(connectLb1->getPosition() + Vec2(connectLb1->getContentSize().width / 2 + 20 + 100, 0));
	_player1Score->setString("0");
	
	Label* connectLb2 = Label::create("Player2 : ", "fonts/arial.ttf", 30);
	connectLb2->setColor(Color3B::WHITE);
	connectLb2->setHorizontalAlignment(TextHAlignment::CENTER);
	connectLb2->setPosition(Vec2(_visibleSize.width / 2 - 20 - connectLb2->getContentSize().width / 2, _visibleSize.height - HEIGHT_OFFSET / 2));
	this->addChild(connectLb2);
	

	_player2Score = createScoreLabel();
	_player2Score->setPosition(connectLb2->getPosition() + Vec2(connectLb2->getContentSize().width / 2 + 20 + 100, 0));
	_player2Score->setString("0");


	log("//======================>>>> End create content UI");

}

/*
Thuc hien lay du lieu tu server
*/
void PlayGame::getFirstData()
{
	// Create connect with mongodb

	auto client = NodeServer::getInstance()->getClient();

	client->emit("get_data_first", "Get Data First PlayGame");
	client->on("get_data_first_end", [&](SIOClient* client, const std::string& data){

		// Thuc hien viec lay du lieu hien tai cua database
		log("Data First : %s", data.c_str());

		rapidjson::Document document;

		document.Parse<0>(data.c_str());

		bool error = document.HasParseError();
		if (error){
			log("//=============Parse Error!!!");
			return;
		}

		// Lay data tu database
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

					log("Player%d id : %d ", i, obj[i]["player_id"].GetInt());

					_allPlayers.push_back(temp);

					log("AAAAA : %d", _allPlayers[i].player_id);

				}

				
			}
		}

	});

}


/*
Khoi tao game border
*/
void PlayGame::createGameBorder()
{
	_border = Node::create();

	// Tao body cho khung vat ly PhysicsMaterial theo thu tu (Density , Restitution  , Friction)
	// Density : ty trong, mat do
	// Restitution : Dan hoi
	// Friction : ma sat
	PhysicsBody* borderBody = PhysicsBody::createEdgeBox(_visibleSize - Size( WIDTH_OFFSET ,  HEIGHT_OFFSET ) * 2 , PhysicsMaterial(1.0f, 1.0f, 0.0f));
	borderBody->setGravityEnable(false);
	borderBody->setContactTestBitmask(eObjectBitMask::BORDER);


	_border->setPhysicsBody(borderBody);
	_border->setPosition(Vec2(_visibleSize.width/2 , _visibleSize.height/2));
	_border->setTag(BORDER_TAG);

	this->addChild(_border);
}


Sprite* PlayGame::createBall()
{
	auto ball = Sprite::create("ball.png" , Rect(0,0,52,52));
	float radius;

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		ball->setScale(1.0f);
		radius = ball->getContentSize().width / 2;
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		ball->setScale(2.0f);
		radius = ball->getContentSize().width;
	}
	ball->setTag(BALL_TAG);

	auto ballBody = PhysicsBody::createCircle(radius , PhysicsMaterial(1.0f, 1.0f, 0.0f));
	ballBody->setGravityEnable(false); // Khong chiu tac dung cua trong luc
	ballBody->setContactTestBitmask(eObjectBitMask::BALL);

	ball->setPhysicsBody(ballBody);

	this->addChild(ball);

	return ball;
}

Sprite* PlayGame::createPaddle()
{
	auto paddle = Sprite::create("paddle.png");
	Size paddleBodySize;

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		paddle->setScale(1.0f);
		paddleBodySize = paddle->getContentSize();
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		paddle->setScale(2.0f);
		paddleBodySize = paddle->getContentSize() *2 ;

	}

	auto paddleBody = PhysicsBody::createBox(paddleBodySize , PhysicsMaterial(1.0f, 1.0f, 0.0f));
	paddleBody->setGravityEnable(false);
	paddleBody->setDynamic(false);
	paddleBody->setContactTestBitmask(eObjectBitMask::PADDLE);

	paddle->setPhysicsBody(paddleBody);
	this->addChild(paddle);

	return paddle;
}

Label* PlayGame::createScoreLabel()
{
	Label* statusLabel = Label::create("", "fonts/arial.ttf", 40);
	statusLabel->setColor(Color3B::WHITE);
	statusLabel->setHorizontalAlignment(TextHAlignment::CENTER);

	this->addChild(statusLabel);

	return statusLabel;
}

bool PlayGame::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void PlayGame::onTouchMoved(Touch* touch, Event* event)
{
	Vec2 touchPoint = touch->getLocation();

	// Xac dinh doi tuong touch thong qua bounding box
	if (_paddle1->getBoundingBox().containsPoint(touchPoint)){
		_paddle1->setPositionX(touchPoint.x);
	}
	

	if (_paddle2->getBoundingBox().containsPoint(touchPoint)){
		_paddle2->setPositionX(touchPoint.x);
	}
	
}

void PlayGame::onTouchEnded(Touch* touch, Event* event)
{

}


bool PlayGame::onContactBegin(PhysicsContact& contact)
{
	return true;
}