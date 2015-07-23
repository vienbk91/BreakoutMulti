/*
 * GameOverScene.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "GameOverScene.h"


USING_NS_CC;
using namespace std;
using namespace ui;

Scene* GameOverScene::createScene(int loseTeam)
{
	auto scene = Scene::create();
	auto layer = GameOverScene::create(loseTeam);

	scene->addChild(layer);

	return scene;
}

GameOverScene* GameOverScene::create(int loseTeam)
{
	auto layer = new GameOverScene();
	if (layer && layer->init(loseTeam)){
		layer->autorelease();

		return layer;
	}

	CC_SAFE_DELETE(layer);
	return nullptr;

}

bool GameOverScene::init(int loseTeam)
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// Create layer background
	_background = LayerColor::create(Color4B(0, 0, 0, 255));
	addChild(_background);

	std::stringstream status;

	if (loseTeam == 1)
	{
		status << "Player 1 Lose \n and \n Player 2 Win";
	}
	else
	{
		status << "Player 1 Win \n and \n Player 2 Lose";
	}


	Label* statusLb = Label::create("" , "fonts/arial.ttf" , 50);
	statusLb->setColor(Color3B::WHITE);
	statusLb->setString(status.str());

	statusLb->setHorizontalAlignment(TextHAlignment::CENTER);
	statusLb->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	this->addChild(statusLb);

	return true;
}