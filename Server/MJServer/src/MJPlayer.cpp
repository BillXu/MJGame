#include "MJPlayer.h"
#include "MessageIdentifer.h"
#include "ServerDefine.h"
#include "LogManager.h"
void MJPlayer::init(stEnterRoomData* pData)
{
	setState(eRoomPeer_WaitNextGame);
	m_nUserUID = pData->nUserUID;
	m_nSessioID = pData->nUserSessionID;
	m_nCoin = pData->nCoin;
	m_nIdx = -1;
	m_nOffset = 0;
}

void MJPlayer::onWillStartGame()
{
	m_nOffset = 0;
	clearGangFlag();
}

void MJPlayer::onStartGame()
{
	setState(eRoomPeer_CanAct);
}

void MJPlayer::onGameDidEnd()
{
	setState(eRoomPeer_WaitNextGame);
}

void MJPlayer::onGameEnd()
{
	setState(eRoomPeer_WaitNextGame);
	clearGangFlag();
}

uint8_t MJPlayer::getIdx()
{
	return m_nIdx;
}

void MJPlayer::setIdx(uint8_t nIdx)
{
	m_nIdx = nIdx;
}

int32_t MJPlayer::getCoin()
{
	return m_nCoin;
}

void MJPlayer::addOffsetCoin(int32_t nOffset)
{
	if (nOffset < 0 && (-1 * nOffset) >(int32_t)getCoin())
	{
		CLogMgr::SharedLogMgr()->ErrorLog( "do not have so much money to offset = %d ,have = %u",nOffset,getCoin() );
		m_nOffset -= m_nCoin;
		m_nCoin = 0;
		return;
	}
	m_nOffset += nOffset;
	m_nCoin = (int32_t)m_nCoin + nOffset;
}

int32_t MJPlayer::getOffsetCoin()
{
	return m_nOffset;
}

void MJPlayer::setState(uint32_t eState)
{
	m_eState = eState;
}

bool MJPlayer::haveState(uint32_t eState)
{
	return (getState() & eState) == eState;
}

uint32_t MJPlayer::getState()
{
	return m_eState;
}

uint32_t MJPlayer::getSessionID()
{
	return m_nSessioID;
}

uint32_t MJPlayer::getUID()
{
	return m_nUserUID;
}

void MJPlayer::signGangFlag()
{
	m_isHaveGang = true;
}

void MJPlayer::clearGangFlag()
{
	m_isHaveGang = false;
}

bool MJPlayer::haveGangFalg()
{
	return m_isHaveGang;
}
