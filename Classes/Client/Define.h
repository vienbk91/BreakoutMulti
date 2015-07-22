/*
 * Define.h
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#ifndef DEFINE_H_
#define DEFINE_H_

#include "cocos2d.h"

USING_NS_CC;

#define BORDER_TAG 1
#define BALL_TAG 2
#define PADDLE1_TAG 3
#define PADDLE2_TAG 4
#define WALL_DIE_TAG_1 5
#define WALL_DIE_TAG_2 6

/*
CategoryBitmask : 0xFFFFFFFF
ContactTestBitmask : 0x00000000
CollisionBitmask : 0xFFFFFFFF
*/
enum eObjectBitMask
{
	BALL = 0x00000001 ,
	PADDLE = 0x00000001 ,
	BORDER = 0x00000001 ,
	WALL_DIE = 0x00000001
};


struct RoomPlayer
{
	int player_id;
    bool status;
	int score;
};


#endif /* DEFINE_H_ */
