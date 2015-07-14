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


bool PlayGame::init()
{
	if(!Layer::init())
	{
		return false;
	}

	_visibleSize = Director::getInstance()->getVisibleSize();

	createContent();
	createGameBorder();


	auto touchListener = EventListenerTouchOneByOne::create();
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


/*
Khoi tao noi dung game
*/
void PlayGame::createContent()
{
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
	paddleBody->setContactTestBitmask(eObjectBitMask::PADDLE);

	paddle->setPhysicsBody(paddleBody);
	this->addChild(paddle);

	return paddle;
}

Label* PlayGame::createScoreLabel()
{
	Label* statusLabel = Label::create("", "fonts/arial.ttf", 30);
	statusLabel->setColor(Color3B::WHITE);
	statusLabel->setHorizontalAlignment(TextHAlignment::CENTER);

	this->addChild(statusLabel);

	return statusLabel;
}


void PlayGame::update(float dt){

	
	/*
	if (_paddle1->getPositionX() < (_paddle1->getContentSize().width / 2 + WIDTH_OFFSET) || _paddle1->getPositionX() > (_visibleSize.width - _paddle1->getContentSize().width / 2 - WIDTH_OFFSET)){
		_paddle1->getPhysicsBody()->setCollisionBitmask(0x00000000);
		_paddle1->getPhysicsBody()->setGravityEnable(false);
		_paddle1->getPhysicsBody()->setRotationEnable(false);
		return;
	}
	else{
		_paddle1->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
	}

	if (_paddle2->getPositionX() < (_paddle2->getContentSize().width / 2 + WIDTH_OFFSET) || _paddle2->getPositionX() > (_visibleSize.width - _paddle2->getContentSize().width / 2 - WIDTH_OFFSET)){
		_paddle2->getPhysicsBody()->setCollisionBitmask(0x00000000);
		_paddle2->getPhysicsBody()->setGravityEnable(false);
		_paddle2->getPhysicsBody()->setRotationEnable(false);
		return;
	}
	else{
		_paddle2->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
	}
	*/

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