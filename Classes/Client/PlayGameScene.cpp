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


USING_NS_CC;
using namespace rapidjson;

Scene* PlayGame::createScene(int teamId)
{
	// Khoi tao scene voi thuoc tinh vat ly
	auto scene = Scene::createWithPhysics();
	// Tao physics debug
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	// Thiet lap trong luong bang 0
	scene->getPhysicsWorld()->setGravity(Vect(0.0f , 0.0f));

	auto layer = PlayGame::create(teamId);


	scene->addChild(layer);

	return scene;
}


//=======================================================================================
//=======================================================================================
// INIT GAME


void PlayGame::setLayerPhysicsWorld(PhysicsWorld* world)
{
	_mWorld = world;
}


PlayGame* PlayGame::create(int teamId)
{
	auto layer = new PlayGame();
	if (layer && layer->init(teamId)){
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}


bool PlayGame::init(int teamId)
{
	if (!Layer::init())
	{
		return false;
	}


	_visibleSize = Director::getInstance()->getVisibleSize();


	_teamId = teamId;
	_client = NodeServer::getInstance()->getClient();

	log("Team ID: %d ", _teamId);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayGame::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayGame::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(PlayGame::onTouchEnded, this);


	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(PlayGame::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


	this->scheduleUpdate();


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

		playGameContent();

	}), nullptr);

	this->runAction(sequence);

}

/*
Thuc hien lay du lieu tu server
*/
void PlayGame::getFirstData()
{
	// Create connect with mongodb

	_client->emit("get_data_first", "Get Data First PlayGame");
	_client->on("get_data_first_end", [&](SIOClient* client, const std::string& data){

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
					temp.score = obj[i]["score"].GetInt();

					log("Player%d id : %d ", i, obj[i]["player_id"].GetInt());

					_allPlayers.push_back(temp);

					log("AAAAA : %d", _allPlayers[i].player_id);

				}


			}
		}

	});

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

//=======================================================================================
//=======================================================================================
// PLAY CONTENT


void PlayGame::playGameContent()
{
	// Tao chuyen dong cua bong
	if (_ball->getPhysicsBody() != nullptr){
		log("Thuc hien di chuyen ball");
		Vect force = Vect(1010000.0f, 1010000.0f);
		//_ball->getPhysicsBody()->applyImpulse(force);
	}
	else{
		log("Chua khoi tao ball");
	}
	
}


void PlayGame::update(float dt)
{
	if (_teamId == 1) 
	{
		_client->on("send_position_player2_end", [&](SIOClient* client , const std::string& data ){
			// Thuc hien viec lay du lieu hien tai cua database
			log("Player1 Position : %s ", data.c_str());
			rapidjson::Document document;
			document.Parse<0>(data.c_str());

			bool error = document.HasParseError();
			if (error){
				log("//=============Parse Error!!!");
				return;
			}

			float x = (float)(document["x"].GetDouble());
			_paddle2->setPositionX(x);


		});
	}
	if (_teamId == 2)
	{
		_client->on("send_position_player1_end", [&](SIOClient* client, const std::string& data){
			log("Player1 Position : %s", data.c_str());

			// Thuc hien viec lay du lieu hien tai cua database
			rapidjson::Document document;
			document.Parse<0>(data.c_str());

			bool error = document.HasParseError();
			if (error){
				log("//=============Parse Error!!!");
				return;
			}

			float x = (float)(document["x"].GetDouble());
			_paddle1->setPositionX(x);

		});
	}

}


//=======================================================================================
//=======================================================================================
// CREATE UI

/*
Khoi tao game border
*/
void PlayGame::createGameBorder()
{
	//===================================================================================
	//===================================================================================
	// CREATE BORDER
	_border = Node::create();
	_border->setTag(BORDER_TAG);
	// Tao body cho khung vat ly PhysicsMaterial theo thu tu (Density , Restitution  , Friction)
	// Density : ty trong, mat do ,  Restitution : Dan hoi , Friction : ma sat
	PhysicsBody* borderBody = PhysicsBody::createEdgeBox(_visibleSize - Size( WIDTH_OFFSET ,  HEIGHT_OFFSET ) * 2 , PhysicsMaterial(1.0f, 1.0f, 0.0f));
	borderBody->setGravityEnable(false);
	borderBody->setContactTestBitmask(eObjectBitMask::BORDER);


	_border->setPhysicsBody(borderBody);
	_border->setPosition(Vec2(_visibleSize.width/2 , _visibleSize.height/2));

	this->addChild(_border);

	//===================================================================================
	//===================================================================================
	// CREATE WALL1

	_wallDie1 = Node::create();
	_wallDie1->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_wallDie1->setTag(WALL_DIE_TAG_1);

	Vec2 vec[2] = {
		Vec2(0 , 0) ,
		Vec2(_visibleSize.width - WIDTH_OFFSET * 2 , 0)
	};

	PhysicsBody* wallBody1 = PhysicsBody::createEdgeChain(vec , 2 ,PhysicsMaterial(1.0f, 1.0f, 0.0f));
	wallBody1->setGravityEnable(false);
	wallBody1->setDynamic(true);
	wallBody1->setRotationEnable(false);
	wallBody1->setContactTestBitmask(eObjectBitMask::WALL_DIE);
	_wallDie1->setPhysicsBody(wallBody1);

	_wallDie1->setPosition(Vec2( WIDTH_OFFSET , HEIGHT_OFFSET + 3));

	this->addChild(_wallDie1);


	//===================================================================================
	//===================================================================================
	// CREATE WALL2

	_wallDie2 = Node::create();
	_wallDie2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_wallDie2->setTag(WALL_DIE_TAG_2);

	
	PhysicsBody* wallBody2 = PhysicsBody::createEdgeChain(vec , 2 , PhysicsMaterial(1.0f, 1.0f, 0.0f));
	wallBody2->setGravityEnable(false);
	wallBody2->setDynamic(true);
	wallBody2->setRotationEnable(false);
	wallBody2->setContactTestBitmask(eObjectBitMask::WALL_DIE);

	_wallDie2->setPhysicsBody(wallBody2);

	_wallDie2->setPosition(Vec2(WIDTH_OFFSET , _visibleSize.height - HEIGHT_OFFSET - 3));
	
	this->addChild(_wallDie2);
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


//=======================================================================================
//=======================================================================================
// TOUCH EVENT


bool PlayGame::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void PlayGame::onTouchMoved(Touch* touch, Event* event)
{
	Vec2 touchPoint = touch->getLocation();

	if (_teamId == 1) {


		if (_paddle1->getBoundingBox().containsPoint(touchPoint)){
			_paddle1->setPositionX(touchPoint.x);
		}

		// Gui du lieu toa do cua player len server

		std::stringstream pos1;
		pos1 << "{\"x\":" << _paddle1->getPositionX() << "}";
		_client->emit("send_position_player1", pos1.str());
		log("Gui vi tri cua player 1 len server");

	}
	
	if (_teamId == 2) {

		if (_paddle2->getBoundingBox().containsPoint(touchPoint)){
			_paddle2->setPositionX(touchPoint.x);
		}

		// Gui du lieu toa do cua player len server

		std::stringstream pos2;
		pos2 << "{\"x\":" << _paddle2->getPositionX() << "}";
		_client->emit("send_position_player2", pos2.str());
		log("Gui vi tri cua layer2 len server");

	}
	
}

void PlayGame::onTouchEnded(Touch* touch, Event* event)
{

}


//=======================================================================================
//=======================================================================================
// COLLISION EVENT


bool PlayGame::onContactBegin(PhysicsContact& contact)
{

	auto nodeA = (Sprite*)contact.getShapeA()->getBody()->getNode();
	int tagA = nodeA->getTag();

	auto nodeB = (Sprite*)contact.getShapeB()->getBody()->getNode();
	int tagB = nodeB->getTag();

	if ((tagA == BALL_TAG && tagB == WALL_DIE_TAG_1) || (tagA == WALL_DIE_TAG_1 && tagB == BALL_TAG))
	{
		//log("Player1 mat diem");
		// Tai day se xu ly hien thi diem

	}

	if ((tagA == BALL_TAG && tagB == WALL_DIE_TAG_2) || (tagA == WALL_DIE_TAG_2 && tagB == BALL_TAG))
	{
		//log("Player2 mat diem");
		// Tai day se xu ly hien thi diem

	}



	return true;
}