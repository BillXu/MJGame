#pragma once
#include "MessageIdentifer.h"
#include "NativeTypes.h"
#define MAX_LEN_ACCOUNT 40   // can not big then unsigned char max = 255
#define  MAX_LEN_PASSWORD 25 // can not big then unsigned char max = 255
#define MAX_LEN_CHARACTER_NAME 25 // can not big then unsigned char  max = 255
#define MAX_LEN_SIGURE 60   // can not big then unsigned char  max = 255
#define MAX_LEN_ROOM_NAME 25
#define MAX_LEN_ROOM_DESC 60  // can not big then unsigned char max = 255
#define MAX_LEN_ROOM_INFORM 500  
#define MAX_LEN_EMAIL 50
#define MAX_LEN_SPEAK_WORDS 200  
#define MAX_MSG_BUFFER_LEN 2048*3

#define MAX_ROOM_PEER 5
#define MAX_TAXAS_HOLD_CARD 5
#define MAX_UPLOAD_PIC 4
#define MAX_JOINED_CLUB_CNT 10

#define MATCH_MGR_UID 1349

#define COIN_CONDITION_TO_GET_CHARITY 500
#define TIMES_GET_CHARITY_PER_DAY 3   // 2 HOURE
#define COIN_FOR_CHARITY 800

#ifndef SERVER
#define PIEXL_TO_POINT(px) (px)/CC_CONTENT_SCALE_FACTOR()
#define FOINT_NAME "Helvetica"
#endif

#define MAX_IP_STRING_LEN 17

#define CIRCLE_TOPIC_CNT_PER_PAGE 7
#define MAX_CIRCLE_CONTENT_LEN 700

#define JS_KEY_MSG_TYPE "msgID"

enum ePayChannel
{
	ePay_AppStore,
	ePay_WeChat,
	ePay_ZhiFuBao,
	ePay_XiaoMi,
	ePay_Max,
};

enum  eVipCardType
{
	eCard_None,
	eCard_Week,
	eCard_Month,
	eCard_Max,
};

enum ePlayerType
{
	ePlayer_Normal,
	ePlayer_Robot,
	ePlayer_Mgr,
	ePlayer_Max,
};

enum eNoticeType
{
	eNotice_Text,
	eNotice_BeInvite, // { targetUID : 2345 , addCoin : 34556 }
	eNotice_InvitePrize, // { targetUID : 2345 addCoin : 3555 }
};

enum eSex
{
	eSex_Unknown,
	eSex_Male,
	eSex_Female,
	eSex_Max,
};

enum eRoomPeerAction
{
	eRoomPeerAction_None,
	eRoomPeerAction_EnterRoom,
	eRoomPeerAction_Ready,
	eRoomPeerAction_Follow,
	eRoomPeerAction_Add,
	eRoomPeerAction_PK,
	eRoomPeerAction_GiveUp,
	eRoomPeerAction_ShowCard,
	eRoomPeerAction_ViewCard,
	eRoomPeerAction_TimesMoneyPk,
	eRoomPeerAction_LeaveRoom,
	eRoomPeerAction_Speak_Default,
	eRoomPeerAction_Speak_Text,
	// action for 
	eRoomPeerAction_Pass,
	eRoomPeerAction_AllIn,
	eRoomPeerAction_SitDown,
	eRoomPeerAction_StandUp,
	eRoomPeerAction_Max
};


// mail Module 
#define MAX_KEEP_MAIL_COUNT 50
enum eMailType
{
	eMail_SysOfflineEvent,// { event: concret type , arg:{ arg0: 0 , arg 1 = 3 } }  // processed in svr , will not send to client ;
	eMail_DlgNotice, // content will be send by , stMsgDlgNotice 
	eMail_ReadTimeTag,  // use tell time for public mail ;
	eMail_Sys_End,

	eMail_RealMail_Begin, // will mail will show in golden server windown ;
	eMail_PlainText,  // need not parse , just display the content ;
	eMail_InvitePrize, // { targetUID : 2345 , addCoin : 300 } // you invite player to join game ,and give prize to you 
	eMail_WinMatch, // { gameType:234,roomName:234,rankIdx:2,addCoin:345,cup : 2 , diamomd : 34 }
	eMail_Max,
};

enum eProcessMailAct
{
	ePro_Mail_None,
	ePro_Mail_Delete,
	ePro_Mail_DoYes,
	ePro_Mail_DoNo,
	ePro_Mail_Look,
	ePor_Mail_Max,
};

#define MIN_PEERS_IN_ROOM_ROBOT 6
#define MAX_PEERS_IN_TAXAS_ROOM 9
#define TIME_LOW_LIMIT_FOR_NORMAL_ROOM 10




