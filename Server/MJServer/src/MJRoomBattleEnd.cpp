#include "MJRoomBattleEnd.h"
#include "MJRoomPlayer.h"
#include "log4z.h"
#include "IRoomDelegate.h"
bool CMJRoomBattleEnd::isGameOver()
{
	if ( getDelegate() && getDelegate()->canGameOver(this) )
	{
		return true ;
	}

	if ( getLeftCardCnt() <= 0 )
	{
		return true ;
	}

	if ( getPlayerCntWithState(eRoomPeer_AlreadyHu) >= getSeatCount() - 1 )
	{
		return true ;
	}
	return false ;
}

bool CMJRoomBattleEnd::checkPlayersNeedTheCard( uint8_t nCardNumber ,std::vector<stWaitIdx>& nNeedCardPlayerIdxs, uint8_t nExptPlayerIdx )
{
	for ( uint8_t nIdx = 0 ; nIdx < getSeatCount() ; ++nIdx )
	{
		if ( nIdx == nExptPlayerIdx )
		{
			continue; 
		}

		auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pPlayer == nullptr || pPlayer->isHaveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}

		uint8_t nActType = 0 ;
		if ( pPlayer->isCardBeWanted(nCardNumber,nActType,false) )
		{
			stWaitIdx wid ;
			wid.nIdx = nIdx ;
			wid.nMaxActExePrio = nActType ;
			nNeedCardPlayerIdxs.push_back(wid) ;
			LOGFMTD("player idx = %u , need the card : %u,max Act Type = %u",nIdx,nCardNumber,nActType) ;
		}
	}

	return nNeedCardPlayerIdxs.empty() == false ;
}

uint8_t CMJRoomBattleEnd::getNextActPlayerIdx( uint8_t nCurActIdx )
{
	for ( ++nCurActIdx ; nCurActIdx < getSeatCount() *2 ; ++nCurActIdx )
	{
		auto nIdx = nCurActIdx % getSeatCount() ;
		auto pPlayer = (CMJRoomPlayer*)getPlayerByIdx(nIdx) ;
		if ( pPlayer == nullptr || pPlayer->isHaveState(eRoomPeer_AlreadyHu) )
		{
			continue;
		}
		return nIdx ;
	}

	LOGFMTE("who do not have proper player to be next") ;
	return 0 ;
}