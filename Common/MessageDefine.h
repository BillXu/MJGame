#pragma once 
#pragma pack(push)
#pragma pack(1)
// define message struct , used between Server and Client ;
#include "ServerMessageIdentifer.h"
#include "CommonData.h"
#define PLACE_HOLDER(X)
// WARNNING:±‰≥§◊÷∑˚¥Æ£¨Œ“√«≤ª∞¸¿®÷’Ω·∑˚ \0 ;           
struct stMsg
{
	unsigned char cSysIdentifer ;  // msg target eServerType
	unsigned short usMsgType ;
public:
	stMsg():cSysIdentifer( ID_MSG_PORT_NONE  ),usMsgType(MSG_NONE){}
};

struct stMsgJsonContent
	:public stMsg
{
	stMsgJsonContent(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_JSON_CONTENT ; }
	uint16_t nJsLen ;
};

// client reconnect ;
struct stMsgReconnect
	:public stMsg
{
public:
	stMsgReconnect(){cSysIdentifer = ID_MSG_PORT_GATE ; usMsgType = MSG_RECONNECT ; }
public:
	unsigned int nSessionID ;
};

struct stMsgReconnectRet
	:public stMsg
{
public:
	stMsgReconnectRet(){cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_RECONNECT ; }
public:
	char nRet; // 0 : success , 1 failed ;
};

struct stMsgServerDisconnect
	:public stMsg
{
public:
	stMsgServerDisconnect(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_DISCONNECT_SERVER  ;}
	unsigned char nServerType ; // eServerType ;
};

struct stMsgPlayerLogout
	:public stMsg
{
public: 
	stMsgPlayerLogout(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_LOGOUT ;}
};

struct stMsgControlFlag
	:public stMsg
{
public:
	stMsgControlFlag(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_CONTROL_FLAG ; }
	uint32_t nFlag ;  // 0 , not in check , 1 , means in check , should hide something ;
	uint16_t nVerfion ;
};

// register an login ;
struct stMsgRegister
	:public stMsg
{
	stMsgRegister(){cSysIdentifer = ID_MSG_PORT_LOGIN ; usMsgType = MSG_PLAYER_REGISTER ; }
	unsigned char cRegisterType ; // 0 ±Ì æ”ŒøÕµ«¬º£�?±Ì æ’˝≥£◊¢≤· , 2 ∞Û∂®’À∫≈ 
	char cAccount[MAX_LEN_ACCOUNT] ;
	char cPassword[MAX_LEN_PASSWORD] ;
	unsigned char nChannel; // «˛µ¿±Í æ£¨0. appstore  1. pp ÷˙ ÷£¨2.  91…Ãµ�?3. 360…Ãµ�?4.winphone store
};

struct stMsgRegisterRet
	:public stMsg
{
	stMsgRegisterRet()
	{
		cSysIdentifer = ID_MSG_PORT_CLIENT ;
		usMsgType = MSG_PLAYER_REGISTER ;
	}
	char nRet ; // 0 success ;  1 . account have exsit ;
	unsigned char cRegisterType ; // 0 ±Ì æ”ŒøÕµ«¬º£�?±Ì æ’˝≥£◊¢≤· , 2 ∞Û∂®’À∫≈ 
	char cAccount[MAX_LEN_ACCOUNT] ;
	char cPassword[MAX_LEN_PASSWORD] ;
	unsigned int nUserID ;
};

struct stMsgLogin
	:public stMsg 
{
	stMsgLogin(){ cSysIdentifer = ID_MSG_PORT_LOGIN ; usMsgType = MSG_PLAYER_LOGIN ; }
	char cAccount[MAX_LEN_ACCOUNT] ;
	char cPassword[MAX_LEN_PASSWORD] ;
};

struct stMsgLoginRet
	:public stMsg 
{
	stMsgLoginRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_LOGIN ; }
	unsigned char nRet ; // 0 ; success ; 1 account error , 2 password error, 3 state error  ;
	uint8_t nAccountType ; // 0 gueset , 1 registered ,2 rebinded  .
};

struct stMsgRebindAccount
	:public stMsg
{
	stMsgRebindAccount(){ cSysIdentifer = ID_MSG_PORT_LOGIN; usMsgType = MSG_PLAYER_BIND_ACCOUNT ;}
	unsigned int nCurUserUID ;
	char cAccount[MAX_LEN_ACCOUNT] ;
	char cPassword[MAX_LEN_PASSWORD] ;
};

struct stMsgRebindAccountRet
	:public stMsg
{
	stMsgRebindAccountRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ;; usMsgType = MSG_PLAYER_BIND_ACCOUNT ; }
	unsigned char nRet ; // 0 success , 1 double account , 2 uid not exsit, 3 unknown error  ;
};

struct stMsgModifyPassword
	:public stMsg
{
	stMsgModifyPassword(){ cSysIdentifer = ID_MSG_PORT_LOGIN ; usMsgType = MSG_MODIFY_PASSWORD ; }
	unsigned int nUserUID ;
	char cOldPassword[MAX_LEN_PASSWORD] ;
	char cNewPassword[MAX_LEN_PASSWORD] ;
};

struct stMsgModifyPasswordRet
	:public stMsg
{
	stMsgModifyPasswordRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ;usMsgType = MSG_MODIFY_PASSWORD ; }
	unsigned char nRet ; // 0 success , 1 uid not exsit , 2 old passworld error 
};

struct stMsgResetPassword
	:public stMsg
{
	stMsgResetPassword(){ cSysIdentifer = ID_MSG_PORT_LOGIN ; usMsgType = MSG_RESET_PASSWORD ; }
	char cAccount[MAX_LEN_ACCOUNT] ;
	char cNewPassword[MAX_LEN_PASSWORD] ;
};

struct stMsgResetPasswordRet
	:public stMsg
{
	stMsgResetPasswordRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_RESET_PASSWORD ; }
 	uint8_t nRet ; // 0 success , 1 can not find account ;
};


struct stMsgPlayerOtherLogin
	:public stMsg
{
	stMsgPlayerOtherLogin(){cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_OTHER_LOGIN ;}
};



struct stMsgPlayerBaseData
	:public stMsg
{
	stMsgPlayerBaseData(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_BASE_DATA ; }
	stCommonBaseData stBaseData ;
	uint32_t nSessionID ;
};

struct stMsgPlayerBaseDataTaxas
	:public stMsg
{
	stMsgPlayerBaseDataTaxas(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_BASE_DATA_TAXAS ; }
	stPlayerGameData tTaxasData ;
};

struct stMsgPlayerBaseDataNiuNiu
	:public stMsg
{
	stMsgPlayerBaseDataNiuNiu(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_BASE_DATA_NIUNIU ; }
	stPlayerGameData tNiuNiuData ;
};

struct stMsgRequestPlayerData
	:public stMsg
{
	stMsgRequestPlayerData(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_REQUEST_PLAYER_INFO ; }
	bool isDetail ;
	uint32_t nPlayerUID ;
};

struct stMsgRequestPlayerDataRet
	:public stMsg
{
	stMsgRequestPlayerDataRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_PLAYER_INFO ;}
	uint8_t nRet ; // 0 success , 1 can not find player 
	bool isDetail ;
	PLACE_HOLDER(stPlayerBrifData* tData ;);  //or stPlayerDetailDataClient* ; 
};


struct stMsgPlayerUpdateMoney
	:public stMsg
{
	stMsgPlayerUpdateMoney(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_UPDATE_MONEY ; }
	uint32_t nFinalCoin ;
	uint32_t nFinalDiamoned ;
	uint32_t nCupCnt ;
};

struct stMsgDlgNotice
	:public stMsg
{
	stMsgDlgNotice(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_DLG_NOTICE ; }
	uint8_t nNoticeType ;   // eNoticeType ;
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* pJson);
};

// modify name and sigure
struct stMsgPLayerModifyName
	:public stMsg
{
	stMsgPLayerModifyName(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_MODIFY_NAME ; }
	char pNewName[MAX_LEN_CHARACTER_NAME] ;
};

struct stMsgPlayerModifyNameRet
	:public stMsg
{
	stMsgPlayerModifyNameRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_MODIFY_NAME ; }
	unsigned char nRet ; // 0 ok
	char pName[MAX_LEN_CHARACTER_NAME] ;
};

struct stMsgPLayerModifySigure
	:public stMsg
{
	stMsgPLayerModifySigure(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_MODIFY_SIGURE ; }
	char pNewSign[MAX_LEN_SIGURE] ;
};

struct stMsgPlayerModifySigureRet
	:public stMsg
{
	stMsgPlayerModifySigureRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_MODIFY_SIGURE ; }
	unsigned char nRet ;        // 0 ok
};

struct stMsgPlayerModifyPhoto
	:public stMsg
{
	stMsgPlayerModifyPhoto(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_MODIFY_PHOTO ; }
	uint16_t nPhotoID ;
};

struct stMsgPlayerModifyPhotoRet
	:public stMsg
{
	stMsgPlayerModifyPhotoRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_MODIFY_PHOTO ; }
	uint8_t nRet ; // 0 means success ;
};

struct stMsgPlayerModifySex
	:public stMsg
{
	stMsgPlayerModifySex(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_MODIFY_SEX ; }
	uint8_t nNewSex ;
};

struct stMsgPlayerModifySexRet
	:public stMsg
{
	stMsgPlayerModifySexRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_MODIFY_SEX ; }
	uint8_t nRet ; // 0 means success ;
};



// friend 
struct stMsgPlayerRequestFriendList
	:public stMsg
{
	stMsgPlayerRequestFriendList(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_REQUEST_FRIEND_LIST ; }
};

struct stMsgPlayerRequestFriendListRet
	:public stMsg
{
	stMsgPlayerRequestFriendListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_FRIEND_LIST ; }
	uint16_t nFriendCount ; 
	PLACE_HOLDER(uint32_t* nFriendUIDs);
};

struct stMsgPlayerAddFriend
	:public stMsg
{
	stMsgPlayerAddFriend(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_ADD_FRIEND ; }
	uint32_t nTargetUID ;
};

struct stMsgPlayerAddFriendRet 
	:public stMsg
{
	stMsgPlayerAddFriendRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_ADD_FRIEND ; }
	uint8_t nRet ; // 0 target agree, 1 target disagree , 2 your friend list is full , 3 target player friend list is full , 4 target offline , 5 , already friend ;
	uint32_t nTaregtUID;
	char pReplayerName[MAX_LEN_CHARACTER_NAME] ;
};

struct stMsgPlayerBeAddedFriend  
	:public stMsg
{
	stMsgPlayerBeAddedFriend(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_BE_ADDED_FRIEND ; }
	uint32_t nPlayerUserUID; // who want add you 
	char pPlayerName[MAX_LEN_CHARACTER_NAME] ; // who want add you 
};

struct stMsgPlayerBeAddedFriendReply
	:public stMsg
{
	stMsgPlayerBeAddedFriendReply(){ cSysIdentifer = ID_MSG_PORT_DATA; usMsgType = MSG_PLAYER_BE_ADDED_FRIEND_REPLY ; }
	uint32_t nReplayToPlayerUserUID ;  // who you relay to ;
	bool bAgree ; // 1 agree to make friend , 0  don't want to make friend ;
};

struct stMsgPlayerBeAddedFriendReplyRet
	:public stMsg
{
	stMsgPlayerBeAddedFriendReplyRet(){cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_BE_ADDED_FRIEND_REPLY ;}
	uint8_t nRet ; // 0 success ; 1 target list full , 2 self list full  3 target offline
	uint32_t nNewFriendUserUID ;  // who you relay to ;
};

// mail
struct stMsgInformNewMail
	:public stMsg
{
	stMsgInformNewMail(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_INFORM_NEW_MAIL ; }
	uint8_t nUnreadMailCount ;
};

struct stMsgRequestMailList
	:public stMsg
{
	stMsgRequestMailList(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_REQUEST_MAIL_LIST ; }
};

struct stMail
{
	uint32_t nPostTime ;
	uint8_t eType;
	uint16_t nContentLen ;
	PLACE_HOLDER(char* pJsonContent) ;
	stMail(){ nContentLen = 0 ; }
};

struct stMsgRequestMailListRet
	:public stMsg
{
	stMsgRequestMailListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_REQUEST_MAIL_LIST ; }
	bool isFinal ;
	stMail tMail;
};

struct stMsgPlayerAdvice
	:public stMsg
{
	stMsgPlayerAdvice(){ cSysIdentifer = ID_MSG_PORT_DATA; usMsgType = MSG_PLAYER_ADVICE ; }
	uint16_t nLen ;
	PLACE_HOLDER(char* pAdviceContent);
};

struct stMsgPlayerAdviceRet
	:public stMsg
{
	stMsgPlayerAdviceRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_ADVICE ; }
	uint8_t nRet ;
};

// exchange module 
struct stMsgPlayerExchange
	:public stMsg
{
	stMsgPlayerExchange(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_EXCHANGE ; }
	uint16_t nExchangeID ;
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* json); // {remark : "my phone name is xxxx" }
};

struct stMsgPlayerExchangeRet
	:public stMsg
{
	stMsgPlayerExchangeRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_EXCHANGE ;}
	uint8_t nRet ; // 0 success , 1 can not find target exchange id , 2 diamond is not engough ; 3 you are not log in ;
	uint16_t nExchageID ;
};

struct stMsgRequestExchangeList
	:public stMsg
{
	stMsgRequestExchangeList(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_REQUEST_EXCHANGE_LIST ; }
};

struct stExchangeItem
{
	uint16_t nExchangeID ;
	uint32_t nExchangedCnt ;
};

struct stMsgRequestExchangeListRet 
	:public stMsg
{
	stMsgRequestExchangeListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_EXCHANGE_LIST ; }
	uint8_t nCnt ;
	PLACE_HOLDER(stExchangeItem* allExchangeIDs);
};

struct stMsgRequestExchangeDetail
	:public stMsg
{
	stMsgRequestExchangeDetail(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_REQUEST_EXCHANGE_DETAIL ; }
	uint16_t nExchangeID ;
};

struct stMsgRequestExchangeDetailRet 
	:public stMsg
{
	stMsgRequestExchangeDetailRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_EXCHANGE_DETAIL ;  }
	uint8_t nRet ; // 0 success , 1 can not find target exchange id ;
	uint16_t nExchangeID ;
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* pJsonDetail); // {desc: "this is describle" , diamond : 25, icon : "fee.jpg" }
};

// vip card 
struct stMsgGetVipcardGift
	:public stMsg
{
	stMsgGetVipcardGift(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_GET_VIP_CARD_GIFT ; }
	uint8_t nVipCardType ; // eCardType 
};

struct stMsgGetVipcardGiftRet
	:public stMsg
{
	stMsgGetVipcardGiftRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_GET_VIP_CARD_GIFT ; }
	uint8_t nRet ; // 0 success , 1 you do not have card , 2 card expire , 3 already got today ;
	uint8_t nVipCardType ;
	uint32_t nAddCoin ;
};

// Charity module ;
struct stMsgPlayerRequestCharityState
	:public stMsg
{
	stMsgPlayerRequestCharityState(){ cSysIdentifer = ID_MSG_PORT_DATA; usMsgType = MSG_PLAYER_REQUEST_CHARITY_STATE ; }
};

struct stMsgPlayerRequestCharityStateRet
	:public stMsg
{
	stMsgPlayerRequestCharityStateRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_REQUEST_CHARITY_STATE ; }
	uint8_t nState ; // 0 can get charity , 1 you coin is enough , do not need charity, 2 times reached ;
	uint8_t nLeftTimes ;
};

struct stMsgPlayerGetCharity
	:public stMsg
{
	stMsgPlayerGetCharity(){ cSysIdentifer = ID_MSG_PORT_DATA; usMsgType = MSG_PLAYER_GET_CHARITY; }
};

struct stMsgPlayerGetCharityRet
	:public stMsg
{
	stMsgPlayerGetCharityRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_PLAYER_GET_CHARITY; }
	uint8_t nRet ; // 0 success ,  1 you coin is enough , do not need charity, 2 times limit ;
	uint32_t nGetCoin ;
	uint32_t nFinalCoin ;
	uint8_t nLeftTimes ;
};

// shop
struct stMsgPlayerBuyShopItem
	:public stMsg 
{
public:
	stMsgPlayerBuyShopItem(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_BUY_SHOP_ITEM ; }
	unsigned short nShopItemID ;
	unsigned int nBuyShopItemForUserUID ; // 0 means buy it for self , other means buy it for other player ;
	unsigned int nCount ;
	// below only used when RMB purchase 
	uint32_t nMiUserUID ;
	unsigned char nChannelID ; // ePayChannel 
	unsigned short nBufLen ;   // based64 string , for app store purchase ;, or xiao mi cporder id , other may not use ;
	PLACE_HOLDER(char* pBuffer);
};

struct stMsgPlayerBuyShopItemRet 
	:public stMsg
{
	stMsgPlayerBuyShopItemRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_BUY_SHOP_ITEM ; }
	unsigned short nShopItemID; // buyed shoped;
	unsigned int nBuyShopItemForUserUID;
	unsigned char nRet ; // 0 success , 1 money not enough , 2 verify failed , 3 buy times limit , 4 shop item out of date, 5 shopitem don't exsit , 6 unknown error;
	uint64_t nFinalyCoin ;
	unsigned int nDiamoned ;
	unsigned int nSavedMoneyForVip ;  // a vip player can buy discont shop item , this is saved money compare to normal player ;
};

struct stMsgPlayerShopBuyItemOrder
	:public stMsg
{
	stMsgPlayerShopBuyItemOrder(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_SHOP_BUY_ITEM_ORDER ; }
	uint16_t nShopItemID ;
	uint8_t nChannel ; // ePayChannel 
};

struct stMsgPlayerShopBuyItemOrderRet
	:public stMsg
{
	stMsgPlayerShopBuyItemOrderRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_SHOP_BUY_ITEM_ORDER ; }
	uint8_t nRet ; // 0 success , 1 can not find shop item ,2 client ip is null ,3 argument error ; 
	uint16_t nShopItemID ;
	uint8_t nChannel ; // ePayChannel 
	char cPrepayId[64] ;
	char cOutTradeNo[32];
};

// invite 
struct stMsgCheckInviter
	:public stMsg
{
	stMsgCheckInviter(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_CHECK_INVITER ; }
	uint32_t nInviterUID ;
};

struct stMsgCheckInviterRet
	:public stMsg
{
	stMsgCheckInviterRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_CHECK_INVITER ; }
	uint8_t nRet ; // 0 , success , 1 inviter not exsit , 2 you already have inviter ;
	uint32_t nInviterUID ;
};

// room msg 

struct stMsgToRoom
	:public stMsg
{
	stMsgToRoom(){ cSysIdentifer = ID_MSG_PORT_TAXAS; nSubRoomIdx = -1 ;}
	uint32_t nRoomID ;
	int8_t nSubRoomIdx ;
};

struct stMsgToNNRoom
	:public stMsgToRoom
{
	stMsgToNNRoom(){ cSysIdentifer = ID_MSG_PORT_NIU_NIU ;}
};

struct stMsgRequestRoomList
	:public stMsg
{
	stMsgRequestRoomList(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_TP_REQUEST_ROOM_LIST; }
};

struct stMsgRequestRoomListRet
	:public stMsg
{
	stMsgRequestRoomListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_ROOM_LIST ; }
	uint8_t nRoomType ;
	uint8_t nRoomCnt ;
	PLACE_HOLDER(uint32_t* vRoomIDs);
};

struct stMsgRequestRoomItemDetail
	:public stMsgToRoom
{
	stMsgRequestRoomItemDetail(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_REQUEST_ROOM_ITEM_DETAIL ;}
};

struct stMsgRequestRoomItemDetailRet
	:public stMsg
{
	stMsgRequestRoomItemDetailRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_ROOM_ITEM_DETAIL ; }
	uint8_t nRet ; // 0 success , 1 not find room ;
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint32_t nOwnerUID ; // 0 means public rooms , other value , private room ;
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* pLen ); 
	// public room : { configID : 23 ,name : "number 1 poker", openTime : 23345 , closeTime: 2345, state : 23  }
	// private room:  { configID : 23 , closeTime: 2345, state : 23 ,createTime : 2345 };
};

struct stMsgRequestRoomRewardInfo
	:public stMsgToRoom
{
	stMsgRequestRoomRewardInfo(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_REQUEST_ROOM_REWARD_INFO ;}
};

struct stMsgRequestRoomRewardInfoRet
	:public stMsg
{
	stMsgRequestRoomRewardInfoRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_ROOM_REWARD_INFO ; }
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* pLen ); // { 0 : "1 reward desc",1 : "1 reward desc" , 2 : "2 reward desc",3 : "3 reward desc" };
};

struct stMsgRequestRoomInfo
	:public stMsgToRoom
{
	stMsgRequestRoomInfo(){ cSysIdentifer = ID_MSG_PORT_TAXAS; usMsgType = MSG_REQUEST_ROOM_INFO ; }
};

// create room
struct stMsgCreateRoom
	:public stMsg
{
	stMsgCreateRoom(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_CREATE_ROOM ; }
	uint8_t nRoomType ; // eRoomType ;
	uint16_t nConfigID ;
	uint16_t nMinites ;
	char vRoomName[MAX_LEN_ROOM_NAME] ;
};

struct stMsgCreateRoomRet
	:public stMsg
{
	stMsgCreateRoomRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_CREATE_ROOM ; }
	uint8_t nRoomType ; // eRoomType ;
	uint8_t nRet ; // 0 success , 1 config error , 2 no more chat room id , 3 can not connect to chat svr , 4 coin not enough , 5 reach your own room cnt up limit , 6 unknown room type ;
	uint32_t nRoomID ; 
	uint64_t nFinalCoin ; 
};

struct stMsgDeleteRoom
	:public stMsg
{
	stMsgDeleteRoom(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_DELETE_ROOM ; }
	uint8_t nRoomType ;
	uint32_t nRoomID ;
};

struct stMsgDeleteRoomRet
	:public stMsg
{
	stMsgDeleteRoomRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_DELETE_ROOM ; }
	uint8_t nRet ; // 0 success , 1 you don't have target room , 2 unknown room type 
	uint8_t nRoomType ;
	uint32_t nRoomID ;
};

struct stMsgCaculateRoomProfit
	:public stMsg
{
	stMsgCaculateRoomProfit(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_TP_CACULATE_ROOM_PROFILE ; }
	uint32_t nRoomID ;
	uint8_t nRoomType ;
};

struct stMsgCaculateRoomProfitRet
	:public stMsg
{
	stMsgCaculateRoomProfitRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_TP_CACULATE_ROOM_PROFILE ; }
	uint8_t nRet ; // 0 sucess , 1 you are not creator , 2 unknown room type , 3 can not find room ;
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	bool bDiamond ;
	uint64_t nProfitMoney;
};

struct stMsgAddRoomRentTime
	:public stMsg
{
	stMsgAddRoomRentTime(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_ADD_RENT_TIME ; }
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint16_t nAddDays ;
};

struct  stMsgAddRoomRentTimeRet
	: public stMsg
{
	stMsgAddRoomRentTimeRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_ADD_RENT_TIME ; }
	uint8_t nRet ; // 0 success , 1 you are not creator , 2 coin not enough , 3 unknown room Type, 4 can not find room ;
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint16_t nAddDays ;
};

struct stMsgRoomEnterNewState
	:public stMsg
{
	stMsgRoomEnterNewState(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_ROOM_ENTER_NEW_STATE ; }
	uint16_t nNewState ;   // eRoomState 
	float m_fStateDuring ; 
};

// room info 
struct stMsgRoomInfo
	:public stMsg
{
	stMsgRoomInfo(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_ROOM_INFO ; }
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint8_t nMaxSeat;
	uint32_t nChatRoomID;
	uint32_t eCurRoomState ; // eRoomState ;
	uint32_t nDeskFee;
	float fChouShuiRate ; 
	uint32_t nCloseTime ;
	uint8_t nSubIdx ; 
	uint16_t nJsonLen ;
	PLACE_HOLDER(char* jsonstr);
	//taxas js {"litBlind":20,"minTakIn":200,"maxTakIn":300, "bankIdx":3 ,"litBlindIdx":2,"bigBlindIdx" : 0,"curActIdx" : 3,"curPool":4000,"mostBet":200,"pubCards":[0,1] }	

	// niu niu js {"bankIdx":3 ,"baseBet" : 20 , "bankerTimes" : 2 };
	
	// golden js { "betRound" = 23, "bankIdx":3 ,"baseBet" : 20 ,"curBet" : 40 ,"mainPool" : 1000 ,curActIdx : 3 };
};

// enter and leave 
struct stMsgPlayerEnterRoom
	:public stMsgToRoom
{
	stMsgPlayerEnterRoom(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_PLAYER_ENTER_ROOM ; nSubIdx = -1 ; }
	uint8_t nRoomGameType ;
	uint32_t nRoomID ;
	int8_t nSubIdx ; // -1 means sys decide ;
};

struct stMsgPlayerEnterRoomRet 
	:public stMsg
{
	stMsgPlayerEnterRoomRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_ENTER_ROOM ; }
	uint8_t nRet ; // 0 success , 1 already in this room , 2 not register player  can not enter ; 3 player coin is too few ; 4 ;  player coin is too many ; 5 can not find room id ,  6 room type error 
};

struct stMsgPlayerLeaveRoom
	:public stMsgToRoom
{
	stMsgPlayerLeaveRoom(){ usMsgType = MSG_PLAYER_LEAVE_ROOM ;}
};

struct stMsgPlayerLeaveRoomRet
	:public stMsg
{
	stMsgPlayerLeaveRoomRet(){ usMsgType = MSG_PLAYER_LEAVE_ROOM ; cSysIdentifer = ID_MSG_PORT_CLIENT ; }
	uint8_t nRet ; // 0 success , 1 you are not in room ;
};

// sit down ;
struct stMsgPlayerSitDown
	:public stMsgToRoom
{
	stMsgPlayerSitDown(){ usMsgType = MSG_PLAYER_SITDOWN ;}
	uint32_t nTakeInCoin ; // 0 ,means take in all ; 
	uint8_t nIdx ;
};

struct stMsgPlayerSitDownRet
	:public stMsg
{
	stMsgPlayerSitDownRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_SITDOWN ; }
	uint8_t nRet ; // 0 success , 1 coin not engouht , 2 target have player , 3 not in room , 4 you already sit down ;
};

struct stMsgRoomSitDown
	:public stMsg
{
	stMsgRoomSitDown(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_ROOM_SITDOWN ; }
	uint32_t nSitDownPlayerUserUID ;
	uint8_t nIdx ;
	uint32_t nTakeInCoin ;
};

// stand up 
struct stMsgPlayerStandUp
	:public stMsgToRoom
{
	stMsgPlayerStandUp(){ usMsgType = MSG_PLAYER_STANDUP ; }
};

struct stMsgPlayerStandUpRet
	:public stMsg
{
	stMsgPlayerStandUpRet() { cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PLAYER_STANDUP ; }
	uint8_t nRet ; // 0 success , 1 you are not sit down , 2 other error ; 
};

struct stMsgRoomStandUp
	:public stMsg
{
	stMsgRoomStandUp(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_ROOM_STANDUP ; }
	uint8_t nIdx ;
};



struct stRoomRankEntry
{
	uint32_t nUserUID ;
	int64_t nGameOffset ;
	int64_t nOtherOffset ;
};

struct stMsgRequestRoomRankRet
	:public stMsg
{
	stMsgRequestRoomRankRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_ROOM_RANK ; }
	uint8_t nCnt ;
	int16_t nSelfRankIdx ;  // -1 means , you are not in rank , other means you rank idx ;
	PLACE_HOLDER(stRoomRankEntry*);
};

struct stMsgRequestLastTermRoomRank
	:public stMsgToRoom
{
	stMsgRequestLastTermRoomRank(){ usMsgType = MSG_REQUEST_LAST_TERM_ROOM_RANK ;}
};

struct stMsgRequestLastTermRoomRankRet
	:public stMsg
{
	stMsgRequestLastTermRoomRankRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_LAST_TERM_ROOM_RANK ; }
	uint8_t nCnt ;
	PLACE_HOLDER(stRoomRankEntry*);
};


struct stMsgRequestNiuNiuMatchRoomList
	:public stMsg
{
public:
	stMsgRequestNiuNiuMatchRoomList(){ cSysIdentifer = ID_MSG_PORT_NIU_NIU ; usMsgType = MSG_REQUEST_MATCH_ROOM_LIST;}

};

struct stMsgRequestTaxasMatchRoomList
	:public stMsg
{
public:
	stMsgRequestTaxasMatchRoomList(){ cSysIdentifer = ID_MSG_PORT_TAXAS ; usMsgType = MSG_REQUEST_MATCH_ROOM_LIST;}

};

struct stMsgMatchRoomItem 
{
	uint32_t nRoomID ;
	char pRoomName[MAX_LEN_ROOM_NAME];
	uint32_t nBaseBet ; // or small bet ;
	uint32_t nChapionUID ;
	uint32_t nEndTime ;
};

struct stMsgRequestMatchRoomListRet
	:public stMsg
{
	stMsgRequestMatchRoomListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQUEST_MATCH_ROOM_LIST;}
	uint8_t nRoomType ;
	uint8_t nItemCnt ;
	PLACE_HOLDER(stMsgMatchRoomItem*) ;
};

// poker circle 
struct stMsgPublishTopic
	:public stMsg
{
	stMsgPublishTopic(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_CIRCLE_PUBLISH_TOPIC ;}
	uint16_t nContentLen ;
	PLACE_HOLDER(char* jsonFormatContentString);
};

struct stMsgPublishTopicRet
	:public stMsg
{
	stMsgPublishTopicRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_CIRCLE_PUBLISH_TOPIC ; }
	uint8_t nRet ; // 0 success , 1 coin not enough, 2 reach limit , 3 you are not register ;
	uint64_t nTopicID ;
};

struct stMsgDeleteTopic
	:public stMsg
{
	stMsgDeleteTopic(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_CIRCLE_DELETE_TOPIC ; }
	uint64_t nDelTopicID ;
};

struct stMsgRequestTopicList
	:public stMsg
{
	stMsgRequestTopicList(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_CIRCLE_REQUEST_TOPIC_LIST ; }
	uint16_t nPageIdx ;
};

struct stMsgRequestTopicListRet
	:public stMsg
{
	stMsgRequestTopicListRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_CIRCLE_REQUEST_TOPIC_LIST ; }
	uint8_t nRet ; // 0 success , 1 overflow pageIdx ;
	uint16_t nPageIdx ;
	uint16_t nTotalPageCnt ;
	uint64_t vTopicIDs[CIRCLE_TOPIC_CNT_PER_PAGE];
};

struct stMsgRequestTopicDetail
	:public stMsg
{
	stMsgRequestTopicDetail(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_CIRCLE_REQUEST_TOPIC_DETAIL ; }
	uint64_t nTopicID ;
};

struct stMsgRequestTopicDetailRet
	:public stMsg
{
	stMsgRequestTopicDetailRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_CIRCLE_REQUEST_TOPIC_DETAIL ; }
	uint8_t nRet ; // 0 success , 1 can not find topic 
	uint64_t nTopicID ;
	uint32_t nAuthorUID ;
	uint32_t nPublishTime ; // utc time 
	uint16_t nContentLen ;
	PLACE_HOLDER(char* pContent);
};

// robot specail
struct stMsgTellPlayerType
	:public stMsg
{
	stMsgTellPlayerType(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_TELL_PLAYER_TYPE ; }
	uint8_t nPlayerType ; // ePlayerType ;
};

struct stMsgAddTempHalo
	:public stMsgToRoom
{
	stMsgAddTempHalo(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_ADD_TEMP_HALO ; }
	uint8_t nTempHalo ; // ePlayerType ;
	uint32_t nTargetUID ;
};

struct stMsgRobotAddMoney
	:public stMsg
{
	stMsgRobotAddMoney()
	{
		cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_ADD_MONEY ;
	}
	int32_t nWantCoin ;
};

struct stMsgRobotAddMoneyRet
	:public stMsg
{
	stMsgRobotAddMoneyRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT; usMsgType = MSG_ADD_MONEY ; }
	uint8_t cRet ; // 0 success ;
	uint64_t nFinalCoin ;
};

struct stMsgRobotModifyRoomRank
	:public stMsgToRoom
{
	stMsgRobotModifyRoomRank(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_MODIFY_ROOM_RANK ; }
	uint32_t nTargetUID ;
	int32_t nOffset ;
};

struct stMsgTellRobotIdle
	:public stMsg
{
	stMsgTellRobotIdle(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_TELL_ROBOT_IDLE ; }
	uint32_t nRobotUID ;
	uint8_t nRobotLevel ;
};

struct stMsgTellRobotEnterRoom
	:public stMsg
{
	stMsgTellRobotEnterRoom(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_TELL_ROBOT_ENTER_ROOM ; }
	uint8_t nRoomType ;
	uint32_t nRoomID ;
	uint8_t nSubRoomIdx ;
};

struct stMsgTellRobotLeaveRoom
	:public stMsg
{
	stMsgTellRobotLeaveRoom(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_TELL_ROBOT_LEAVE_ROOM ; }
};

struct stMsgReqRobotTotalGameOffset
	:public stMsg
{
	stMsgReqRobotTotalGameOffset(){ cSysIdentifer = ID_MSG_PORT_DATA ; usMsgType = MSG_REQ_TOTAL_GAME_OFFSET ; }
};

struct stMsgReqRobotTotalGameOffsetRet
	:public stMsg
{
	stMsgReqRobotTotalGameOffsetRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQ_TOTAL_GAME_OFFSET ; }
	int32_t nTotalGameOffset ;
};

struct stMsgReqRobotCurGameOffset
	:public stMsg
{
	stMsgReqRobotCurGameOffset(){ cSysIdentifer = ID_MSG_PORT_NONE ; usMsgType = MSG_REQ_CUR_GAME_OFFSET ; }
};

struct stMsgReqRobotCurGameOffsetRet
	:public stMsg
{
	stMsgReqRobotCurGameOffsetRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_REQ_CUR_GAME_OFFSET ; }
	int32_t nCurGameOffset ;
};

// apns 
struct stMsgPushAPNSToken
	:public stMsg
{
	stMsgPushAPNSToken(){ cSysIdentifer = ID_MSG_PORT_APNS ; usMsgType = MSG_PUSH_APNS_TOKEN ; }
	uint8_t nReqTokenRet ; // 0 success ; 1 use disabled notification ;
	uint32_t nUserUID ;
	uint32_t nFlag ; 
	char vAPNsToken[32] ;  // must proccesed in client ; change to htonl();  // change to network big endain ;
};

struct stMsgPushAPNSTokenRet
	:public stMsg
{
	stMsgPushAPNSTokenRet(){ cSysIdentifer = ID_MSG_PORT_CLIENT ; usMsgType = MSG_PUSH_APNS_TOKEN ; }
	unsigned char nReqTokenRet ; // 0 success ; 1 use disabled notification ;
};

// end 
#pragma pack(pop)