/*
 * PlayGameScene.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "PlayGameScene.h"

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
	_paddle1 = createPaddle();

	float offset;

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		offset = 50;
	}
	else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		offset = 60;
	}

	_paddle1->setPosition(Vec2(_visibleSize.width / 2, offset));
	_paddle2 = createPaddle();
	_paddle2->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - offset ));

}

/*
Khoi tao game border
*/
void PlayGame::createGameBorder()
{
	_border = Node::create();

	// Tao body cho khung vat ly
	// Density : ty trong, mat do
	// Restitution : Dan hoi
	// Friction : ma sat
	PhysicsBody* borderBody = PhysicsBody::createEdgeBox(_visibleSize - Size(10,10), PhysicsMaterial(1.0f, 1.0f, 0.0f));
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

		if (_paddle1->getPositionX() < (_paddle1->getContentSize().width) || _paddle1->getPositionX() > (_visibleSize.width - _paddle1->getContentSize().width)){
			return;
		}
	}
	

	if (_paddle2->getBoundingBox().containsPoint(touchPoint)){
		_paddle2->setPositionX(touchPoint.x);

		if (_paddle2->getPositionX() < (_paddle2->getContentSize().width) || _paddle2->getPositionX() > (_visibleSize.width - _paddle2->getContentSize().width)){
			return;
		}
	}
	
}
void PlayGame::onTouchEnded(Touch* touch, Event* event)
{

}


bool PlayGame::onContactBegin(PhysicsContact& contact)
{
	return true;
}