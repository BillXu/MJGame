#include "MJRoomPlayer.h"
#include <string>
#include "LogManager.h"
#include "ServerMessageDefine.h"
void CMJRoomPlayer::reset(IRoom::stStandPlayer* pPlayer)
{
	ISitableRoomPlayer::reset(pPlayer) ;
	m_tPeerCard.reset();
	m_nGameOffset = 0 ;
	m_vecBill.clear() ;
	m_nNewFetchCard = 0 ;
	m_listWantedCard.clear();
}

void CMJRoomPlayer::onGameEnd()
{
	ISitableRoomPlayer::onGameEnd();
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();

	for ( auto ref : m_vecBill )
	{
		delete ref ;
		ref = nullptr ;
	}
	m_vecBill.clear() ;
	m_listWantedCard.clear();
}

void CMJRoomPlayer::onGameBegin()
{
	ISitableRoomPlayer::onGameBegin() ;
	m_nGameOffset = 0 ;
	m_tPeerCard.reset();
	m_listWantedCard.clear();
}

void CMJRoomPlayer::doSitdown(uint8_t nIdx )
{
	setIdx(nIdx) ;
}

bool CMJRoomPlayer::removeCard(uint8_t nCardNumber) 
{
	m_tPeerCard.removeCardNumber(nCardNumber);
	m_listWantedCard.clear() ;
	m_tPeerCard.updateWantedCard(m_listWantedCard) ;
	return true ;
}

bool CMJRoomPlayer::isHaveAnCard(uint8_t nCardNumber)
{
	return m_tPeerCard.isHaveAnCard(nCardNumber) ;
}

uint8_t CMJRoomPlayer::getCardByIdx(uint8_t nCardIdx, bool isForExchange )
{
	return m_tPeerCard.getCardByIdx(nCardIdx,isForExchange);
}

void CMJRoomPlayer::addDistributeCard(uint8_t nCardNumber )
{
	m_tPeerCard.addCard(nCardNumber);
}

void CMJRoomPlayer::setMustQueType(uint8_t nType )
{
	m_tPeerCard.setMustQueType((eMJCardType)nType) ;
}

bool CMJRoomPlayer::canHuPai( uint8_t nCard ) // 0 means , self hu ; 
{
	if ( nCard == 0 )
	{
		nCard = m_nNewFetchCard;
	}

	for ( auto refWanted : m_listWantedCard )
	{
		if ( refWanted.eCanInvokeAct != eMJAct_Hu )
		{
			continue;
		}

		if ( refWanted.nNumber == nCard )
		{
			return true ;
		}
	}
	return false ;
}

bool CMJRoomPlayer::canGangWithCard( uint8_t nCard, bool bCardFromSelf )
{
	for ( auto refWanted : m_listWantedCard )
	{
		if ( refWanted.eCanInvokeAct == eMJAct_MingGang || eMJAct_AnGang == refWanted.eCanInvokeAct  )
		{
			if ( nCard == refWanted.nNumber )
			{
				return true ;
			}
		}

		if ( refWanted.eCanInvokeAct == eMJAct_BuGang )
		{
			if ( nCard == refWanted.nNumber )
			{
				if ( bCardFromSelf == false )
				{
					return false ;
				}

				return true ;
			}
		}
		
	}

	return false ;
}

bool CMJRoomPlayer::canPengWithCard(uint8_t nCard)
{
	for ( auto refWanted : m_listWantedCard )
	{
		if ( refWanted.eCanInvokeAct != eMJAct_Peng )
		{
			continue;
		}

		if ( refWanted.nNumber == nCard )
		{
			return true ;
		}
	}

	return false ;
}

uint8_t CMJRoomPlayer::getNewFetchCard()
{
	return m_nNewFetchCard ;
}

void CMJRoomPlayer::fetchCard(uint8_t nCardNumber )
{
	m_nNewFetchCard = nCardNumber ;
	m_eNewFetchCardFrom = eMJAct_Mo ;
	m_tPeerCard.addCard(m_nNewFetchCard);

	m_listWantedCard.clear() ;
	m_tPeerCard.updateWantedCard(m_listWantedCard) ;
}

eMJActType CMJRoomPlayer::getNewFetchedFrom()
{
	return m_eNewFetchCardFrom ;
}

uint8_t CMJRoomPlayer::doHuPaiFanshu( uint8_t nCardNumber , uint8_t& nGenShu )  // nCardNumber = 0 , means self mo ;
{
	return m_tPeerCard.doHuPaiFanshu(nCardNumber,nGenShu) ;
}

bool CMJRoomPlayer::isCardBeWanted(uint8_t nCardNumber , bool bFromSelf )
{
	if ( isHaveState(eRoomPeer_DecideLose) )
	{
		return false ;
	}

	if ( isHaveState(eRoomPeer_AlreadyHu) && bFromSelf == false )
	{
		return false ;
	}

	for ( auto refWanted : m_listWantedCard )
	{
		if ( refWanted.nNumber == nCardNumber )
		{
			if ( bFromSelf )
			{
				if ( refWanted.eWanteddCardFrom == ePos_Self || ePos_Any == refWanted.eWanteddCardFrom )
				{
					return true ;
				}
			}
			else
			{
				if ( refWanted.eWanteddCardFrom == ePos_Other || ePos_Any == refWanted.eWanteddCardFrom )
				{
					return true ;
				}
			}
		}
	}

	return false ;
}

void CMJRoomPlayer::onPengCard(uint8_t nCard )
{
	m_tPeerCard.doAction(eMJAct_Peng,nCard);
	m_eNewFetchCardFrom = eMJAct_Mo ;
}

bool CMJRoomPlayer::isHuaZhu()
{
	return m_tPeerCard.isContainMustQue() ;
}

bool CMJRoomPlayer::isTingPai()
{
	for ( auto refWanted : m_listWantedCard )
	{
		if ( refWanted.eCanInvokeAct == eMJAct_Hu )
		{
			return true ;
		}
	}

	return false ;
}

uint8_t CMJRoomPlayer::getMaxCanHuFanShu( uint8_t& nGenShu )
{
	return m_tPeerCard.getMaxHuPaiFanShu(nGenShu) ;
}

void CMJRoomPlayer::addBill( stBill* pBill )
{
	m_vecBill.push_back(pBill) ;
}

void CMJRoomPlayer::gangPai( uint8_t nGangPai, eMJActType eGangType,uint8_t nNewCard  )
{
	m_nNewFetchCard = nNewCard ;
	m_eNewFetchCardFrom = eGangType ;
	m_tPeerCard.doAction(eGangType,nGangPai);
	m_tPeerCard.addCard(m_nNewFetchCard);

	m_listWantedCard.clear() ;
	m_tPeerCard.updateWantedCard(m_listWantedCard) ;
}

void CMJRoomPlayer::getGangWinBill( std::vector<stBill*>& vecGangWin )
{
	for ( auto refBill : m_vecBill )
	{
		if ( refBill->eType == stBill::eBill_GangWin )
		{
			vecGangWin.push_back(refBill) ;
		}
	}
}


