#include "HZMJRoom.h"
#include "MJRoomStateWaitReady.h"
#include "MJRoomStateWaitPlayerChu.h"
#include "MJRoomStateWaitPlayerAct.h"
#include "MJRoomStateStartGame.h"
#include "MJRoomStateGameEnd.h"
#include "MJRoomStateDoPlayerAct.h"
#include "MJRoomStateAskForPengOrHu.h"
#include "HZMJPlayerCard.h"
#include "HZMJPlayer.h"
#include "RoomConfig.h"
bool HZMJRoom::init(IRoomManager* pRoomMgr, stBaseRoomConfig* pConfig, uint32_t nRoomID, Json::Value& vJsValue)
{
	IMJRoom::init(pRoomMgr,pConfig,nRoomID,vJsValue);
	// create room state ;
	IMJRoomState* vState[] = {
		new CMJRoomStateWaitReady(), new MJRoomStateWaitPlayerChu(), new MJRoomStateWaitPlayerAct(), new MJRoomStateStartGame()
		, new MJRoomStateGameEnd(), new MJRoomStateDoPlayerAct(), new MJRoomStateAskForPengOrHu()
	};
	for (uint8_t nIdx = 0; nIdx < sizeof(vState) / sizeof(IMJRoomState*); ++nIdx)
	{
		addRoomState(vState[nIdx]);
	}
	setInitState(vState[0]);
	m_tPoker.initAllCard(eMJ_HZ);
	m_nContinueBankes = 0;
	memset(m_vCaiPiaoFlag,0,sizeof(m_vCaiPiaoFlag));
	return true;
}

void HZMJRoom::startGame()
{
	IMJRoom::startGame();
	// distribute card ;
	m_tPoker.shuffle();
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (!pPlayer)
		{
			CLogMgr::SharedLogMgr()->ErrorLog("why player is null hz mj must all player is not null");
			continue;
		}

		for (uint8_t nIdx = 0; nIdx < 12; ++nIdx)
		{
			pPlayer->getPlayerCard()->addDistributeCard(distributeOneCard());
		}

		if (getBankerIdx() == pPlayer->getIdx())
		{
			pPlayer->getPlayerCard()->onMoCard(distributeOneCard());
		}
	}

}

void HZMJRoom::onGameEnd()
{
	// send game result ;
	IMJRoom::onGameEnd();
}

void HZMJRoom::willStartGame()
{
	IMJRoom::willStartGame();
	memset(m_vCaiPiaoFlag, 0, sizeof(m_vCaiPiaoFlag));
	if (getBankerIdx() == (uint8_t)-1)
	{
		setBankIdx(rand() % getSeatCnt());
	}
}

void HZMJRoom::onGameDidEnd()
{
	uint8_t nHuIdx = -1;
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer && pPlayer->haveState(eRoomPeer_AlreadyHu))
		{
			nHuIdx = pPlayer->getIdx();
		}
	}

	if (getBankerIdx() == nHuIdx)
	{
		++m_nContinueBankes;
	}
	IMJRoom::onGameDidEnd();
}

bool HZMJRoom::isAnyPlayerPengOrHuThisCard(uint8_t nInvokeIdx, uint8_t nCard)
{
	for (auto& ref : m_vCaiPiaoFlag)
	{
		if (ref)
		{
			return false;
		}
	}
	return IMJRoom::isAnyPlayerPengOrHuThisCard(nInvokeIdx,nCard);
}

bool HZMJRoom::isAnyPlayerRobotGang(uint8_t nInvokeIdx, uint8_t nCard)
{
	return false;
}

bool HZMJRoom::isGameOver()
{
	if (!isCanGoOnMoPai())
	{
		return true;
	}

	// is any one hu 
	for (auto& pPlayer : m_vMJPlayers)
	{
		if (pPlayer && pPlayer->haveState(eRoomPeer_AlreadyHu))
		{
			return true;
		}
	}
	return false;
}

bool HZMJRoom::isCanGoOnMoPai()
{
	return m_tPoker.getLeftCardCount() > 20;
}

IMJPlayer* HZMJRoom::doCreateMJPlayer()
{

}

uint8_t HZMJRoom::distributeOneCard()
{
	return m_tPoker.getCard();
}

void HZMJRoom::onPlayerChu(uint8_t nIdx, uint8_t nCard)
{
	HZMJPlayer* pPlayer = (HZMJPlayer*)getMJPlayerByIdx(nIdx);
	auto nCaiShen = make_Card_Num(eCT_Jian, 3);
	m_vCaiPiaoFlag[nIdx] = nCaiShen == nCard;
	if (nCaiShen == nCard) // cai piao 
	{
		bool bIsGAngPiao = pPlayer->haveGangFalg() && pPlayer->getPlayerCard()->isHoldCardCanHu();
		pPlayer->increasePiaoTimes(bIsGAngPiao);
	}
	else
	{
		pPlayer->clearPiaoTimes();
	}
	IMJRoom::onPlayerChu(nIdx,nCard);
}

void HZMJRoom::onPlayerHu(std::vector<uint8_t>& vHuIdx, uint8_t nCard, uint8_t nInvokeIdx)
{
	// check only can self mo hu ;

	// do check base bet ;
	HZMJPlayer* pPlayer = (HZMJPlayer*)getMJPlayerByIdx(vHuIdx.front());
	auto pConfig = (stMJRoomConfig*)getRoomConfig();
	
	bool isQiDui = false;
	uint8_t nHaoHuaCnt = 0;
	bool isBaoTiao = false;
	uint8_t nPiaoCnt = 0;
	bool isGangKai = pPlayer->haveGangFalg();

	uint32_t nBasetBet = pConfig->nBaseBet;
	auto pHZCard = (HZMJPlayerCard*)pPlayer->getPlayerCard();
	
	// check qidui 
	isQiDui = pHZCard->canHoldCard7PairHu();
	if ( isQiDui )
	{
		nBasetBet *= 2;
		uint8_t n = pHZCard->get7PairHuHaoHuaCnt();
		nHaoHuaCnt = n;
		while ( n-- > 0 )
		{
			nBasetBet *= 4;
		}
	}

	// check bao tou 
	isBaoTiao = pHZCard->isBaoTou();
	if (isBaoTiao)
	{
		nBasetBet *= 2;
	}

	// check cai piao 
	nPiaoCnt = pPlayer->getPiaoTimes();
	auto nPiCnt = nPiaoCnt;
	while (nPiCnt-- > 0)
	{
		nBasetBet *= 2;
	}

	// check gang kai 
	if ( isGangKai )
	{
		nBasetBet *= 2;
		if (nPiaoCnt > 0)  // piao gang ; 
		{
			nBasetBet *= 8;
		}
	}

	// gang piao 
	if (pPlayer->haveGangPiao())
	{
		nBasetBet *= 8;
	}

	/// do caculate per player ;
}