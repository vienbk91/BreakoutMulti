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


	


	return true;
}


void PlayGame::onEnter()
{
	Layer::onEnter();

	// Lay du lieu dau vao

	createContent();
	createGameBorder();

	playGameContent();

	this->scheduleUpdate();

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


	// Create socre label
	Label* connectLb1;
	Label* connectLb2;

	if (_teamId == 1){
		connectLb1 = Label::create("Player1 : ", "fonts/arial.ttf", 30);
		connectLb2 = Label::create("Player2 : ", "fonts/arial.ttf", 30);
	}
	else
	{
		connectLb1 = Label::create("Player2 : ", "fonts/arial.ttf", 30);
		connectLb2 = Label::create("Player1 : ", "fonts/arial.ttf", 30);
	}

	//=========================================================================
	// Score Label 1

	connectLb1->setColor(Color3B::WHITE);
	connectLb1->setHorizontalAlignment(TextHAlignment::CENTER);
	connectLb1->setPosition(Vec2(_visibleSize.width/2 - 20 - connectLb1->getContentSize().width/2 , HEIGHT_OFFSET/2));
	this->addChild(connectLb1);


	_player1Score = createScoreLabel();
	_player1Score->setPosition(connectLb1->getPosition() + Vec2(connectLb1->getContentSize().width / 2 + 20 + 100, 0));
	_player1Score->setString("5");
	
	//=========================================================================
	// Score Label 2

	connectLb2->setColor(Color3B::WHITE);
	connectLb2->setHorizontalAlignment(TextHAlignment::CENTER);
	connectLb2->setPosition(Vec2(_visibleSize.width / 2 - 20 - connectLb2->getContentSize().width / 2, _visibleSize.height - HEIGHT_OFFSET / 2));
	this->addChild(connectLb2);
	

	_player2Score = createScoreLabel();
	_player2Score->setPosition(connectLb2->getPosition() + Vec2(connectLb2->getContentSize().width / 2 + 20 + 100, 0));
	_player2Score->setString("5");


	_score1 = 5;
	_score2 = 5;

	log("//======================>>>> End create content UI");

}

//=======================================================================================
//=======================================================================================
// PLAY CONTENT


void PlayGame::playGameContent()
{
	// Tao chuyen dong cua bong

	// Bong chi chuyen dong tai client player 1
	if (_ball->getPhysicsBody() != nullptr){
		log("Thuc hien di chuyen ball");

		//float offset = 1010000.0f; // Luc day ball
		float offset = 1000000.0f;

		
		// Thuc hien gui vi tri va luc force cua ball len server
		std::stringstream msg;
		msg << "{\"ball_x\" : " << _ball->getPositionX() << " , \"ball_y\" : " << _ball->getPositionY() << " , \"offset_force\" : " << offset << " }";

		_client->emit("send_ball_info", msg.str());


		

		if (_teamId == 1)
		{
			_client->on("send_ball_info_end", [&](SIOClient* client, const std::string& data){
				log("Thong tin ball ban dau: %s ", data.c_str());

				rapidjson::Document document;
				document.Parse<0>(data.c_str());

				bool error = document.HasParseError();
				if (error){
					log("//=============Parse Error!!!");
					return;
				}

				float ball_x = (float)(document["ball_x"].GetDouble());
				float ball_y = (float)(document["ball_y"].GetDouble());
				float offset_force = (float)(document["offset_force"].GetDouble());
				

				// Thuc hien day ball theo 1 goc 45 do
				log("Player 1 thiet lap vi tri cua ball");
				Vect force1 = Vect(1.0f, 1.0f);
				_ball->setPosition(Vec2(ball_x, ball_y));
				_ball->getPhysicsBody()->applyImpulse(force1 * offset_force);

			});
		}


		if (_teamId == 2)
		{
			_client->on("send_ball_info_end", [&](SIOClient* client, const std::string& data){
				log("Thong tin ball ban dau: %s ", data.c_str());

				rapidjson::Document document;
				document.Parse<0>(data.c_str());

				bool error = document.HasParseError();
				if (error){
					log("//=============Parse Error!!!");
					return;
				}

				float ball_x = (float)(document["ball_x"].GetDouble());
				float ball_y = (float)(document["ball_y"].GetDouble());
				float offset_force = (float)(document["offset_force"].GetDouble());


				// Thuc hien day bong theo huong nguoc lai voi teamid = 1
				log("Player2 thiet lap lai vi tri cua ball");
				Vect force2 = Vect(-1.0f, -1.0f);
				_ball->setPosition(Vec2(_visibleSize.width - ball_x, _visibleSize.height - ball_y));
				_ball->getPhysicsBody()->applyImpulse(force2 * offset_force);

			});
		}

	}
	else{
		log("Chua khoi tao ball");
	}
	
}


void PlayGame::update(float dt)
{
	//==================================================================================
	// Lay vi tri cua client con lai va update vi tri cho _paddle2
	// Tuy nhien nen nho rang vi tri cua _paddle2 la hinh anh chieu cua _paddle1 qua diem trung tam man hinh
	// Nhu vay neu x la toa do X cua _paddle1 thi (w-x) la toa doa X cua _paddle2
	_client->on("send_position_player_end", [&](SIOClient* client, const std::string& data){
		log("Player1 Position : %s ", data.c_str()); // {"x" : 123.45} -> Dang json nhan duoc
		rapidjson::Document document;
		document.Parse<0>(data.c_str());

		bool error = document.HasParseError();
		if (error){
			log("//=============Parse Error!!!");
			return;
		}

		float x = (float)(document["x"].GetDouble());
		_paddle2->setPositionX(_visibleSize.width - x);

	});


	


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


	/*
	Theo nguyen ly thuyet ke UI thi player1 hay player2 thi deu o duoi - tung ung la _paddle1
	Voi player1: _paddle1 la player1 , _paddle2 la player2
	Voi player2: _paddle1 la player2 , _paddle2 la player1
	*/
	if (_paddle1->getBoundingBox().containsPoint(touchPoint)){
		_paddle1->setPositionX(touchPoint.x);
	}

	// Gui du lieu toa do cua _paddle1 len server
	// Client tuong ung se gui vi tri cua minh len server va server se gui lai cho client kia
	if (_score1 != 0 && _score2 != 0)
	{
		std::stringstream pos;
		pos << "{\"x\":" << _paddle1->getPositionX() << "}";
		_client->emit("send_position_player", pos.str());
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
		_score1--;

	}

	if ((tagA == BALL_TAG && tagB == WALL_DIE_TAG_2) || (tagA == WALL_DIE_TAG_2 && tagB == BALL_TAG))
	{
		_score2--;
	}

	if (_score1 == 0 || _score2 == 0)
	{
		unscheduleUpdate();
	}


	if (_teamId == 1)
	{
		//===========================================================================================
		// Thuc hien gui diem len server va cap nhat diem cho player khac

		std::stringstream score;

		score << "{\"score1\":" << _score1 << " , \"score2\": " << _score2 << "}";
		_client->emit("send_score", score.str());

		int loseTeam;

		if (_score1 == 0 || _score2 == 0)
		{

			if (_score1 == 0)
			{
				loseTeam = 1;
			}
			else{
				loseTeam = 2;
			}

			//===========================================================================================
			// Thuc hien dung game va chuyen qua man hinh game over

			Director::getInstance()->replaceScene(GameOverScene::createScene(loseTeam));
		}
		else
		{
			_player1Score->setString(String::createWithFormat("%d", _score1)->getCString());
			_player2Score->setString(String::createWithFormat("%d", _score2)->getCString());
		}

	} else {

		_client->on("send_score_end", [&](SIOClient* client , const std::string& data){
			log("Score: %s ", data.c_str());

			rapidjson::Document document;
			document.Parse<0>(data.c_str());

			bool error = document.HasParseError();
			if (error){
				log("//=============Parse Error!!!");
				return;
			}

			int score1 = document["score1"].GetInt();
			int score2 = document["score2"].GetInt();

			int loseTeam;

			if (score1 == 0 || score2 == 0)
			{

				if (_score1 == 0)
				{
					loseTeam = 1;
				}
				else{
					loseTeam = 2;
				}


				//===========================================================================================
				// Thuc hien dung game va chuyen qua man hinh game over

				Director::getInstance()->replaceScene(GameOverScene::createScene(loseTeam));
			}
			else
			{
				_player2Score->setString(String::createWithFormat("%d", score1)->getCString());
				_player1Score->setString(String::createWithFormat("%d", score2)->getCString());
			}

		});
	}


	return true;
}