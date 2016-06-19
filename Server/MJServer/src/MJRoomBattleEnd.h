#pragma once
#include "MJRoom.h"
class CMJRoomBattleEnd
	:public CMJRoom
{
public:
	uint8_t getRoomType()override{ return eRoom_MJ_Blood_End ;}
	bool isGameOver()override ;
	bool checkPlayersNeedTheCard( uint8_t nCardNumber ,std::vector<stWaitIdx>& nNeedCardPlayerIdxs, uint8_t nExptPlayerIdx )override;
	uint8_t getNextActPlayerIdx( uint8_t nCurActIdx )override;
};