/*
 * GameOverScene.h
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#ifndef GAMEOVERSCENE_H_
#define GAMEOVERSCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;


class GameOverScene : public Layer
{
public:

	static Scene* createScene(int loseTeam);
	static GameOverScene* create(int loseTeam);

	virtual bool init(int loseTeam);


private:


	LayerColor* _background;

};


#endif /* GAMEOVERSCENE_H_ */
