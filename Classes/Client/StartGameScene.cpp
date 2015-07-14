


#include "StartGameScene.h"

USING_NS_CC;

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
		auto server = NodeServer::getInstance();

		Director::getInstance()->replaceScene(TransitionMoveInR::create(0.5f, SelectTeamScene::createScene() ) );
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
