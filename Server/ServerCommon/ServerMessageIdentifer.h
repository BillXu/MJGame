#pragma once
#include "MessageIdentifer.h"

enum eMsgTypeSvr
{
	MSG_NONE,
	MSG_SVR_USED_BEGIN = 0,
	//--new define begin---
	// the msg title used between servers 
	MSG_SERVERS_USE,
	MSG_VERIFY_BEGIN,
	MSG_VERIFY_GAME,  // verify that is game server ;
	MSG_VERIFY_LOGIN, // verify login server ;
	MSG_VERIFY_CLIENT, // verify that is client ;
	MSG_VERIFY_VERIYF, // verify buy transaction ok ;
	MSG_VERIFY_GATE, // verify that is gate server 
	MSG_VERIFY_DB,  // verify that is DBserver ;
	MSG_VERIFY_APNS, // apple push notification ;
	MSG_VERIFY_LOG, // LOG sever 
	MSG_VERIFY_TAXAS, // TAXAS POKER SERVER 
	MSG_VERIFY_DATA, // VIERIFY DATA SERVER ;
	MSG_VERIFY_NIU_NIU,
	MSG_VERIFY_GOLDEN,
	MSG_VERIFY_END,
	MSG_TRANSER_DATA, // tranfer data between servers ;
	MSG_CONTROL_FLAG,
	MSG_REQUEST_CLIENT_IP,
	MSG_DISCONNECT_SERVER, 
	MSG_DISCONNECT_CLIENT,
	MSG_CONNECT_NEW_CLIENT,
	MSG_VERIFY_TANSACTION,
	MSG_APNS_INFO,   // send push notification ;
	MSG_PUSH_APNS_TOKEN,  // used for apns ; APPLE remote push notification ;
	MSG_SAVE_LOG,
	MSG_LOGIN_INFORM_GATE_SAVE_LOG,
	// msg title used between server and client ;
	MSG_RECONNECT,   // client with gate 
	MSG_SAVE_PLAYER_MONEY, // send to DB ;
	MSG_SAVE_COMMON_LOGIC_DATA,
	MSG_SAVE_DB_LOG, // save log inter log db ;
	MSG_GATESERVER_INFO ,

	MSG_SELECT_DB_PLAYER_DATA,
	MSG_ON_PLAYER_BIND_ACCOUNT,
	// login 

	MSG_REQUEST_ROOM_REWARD_INFO,
	MSG_REQUEST_ROOM_ITEM_DETAIL,

	MSG_PLAYER_BASE_DATA_TAXAS,

	MSG_REQUEST_CREATE_PLAYER_DATA,  // INFORM DB prepare data for player
	MSG_CROSS_SERVER_REQUEST,
	MSG_CROSS_SERVER_REQUEST_RET,
	MSG_READ_PLAYER_TAXAS_DATA,
	MSG_SAVE_TAXAS_ROOM_PLAYER,
	MSG_SAVE_REMOVE_TAXAS_ROOM_PLAYERS,
	MSG_READ_TAXAS_ROOM_PLAYERS,
	MSG_REQUEST_MY_OWN_ROOMS,
	MSG_REQUEST_MY_FOLLOW_ROOMS,
	MSG_SAVE_CREATE_ROOM_INFO,
	MSG_SAVE_UPDATE_ROOM_INFO,
	MSG_READ_ROOM_INFO,
	MSG_PLAYER_BASE_DATA_NIUNIU,
	MSG_READ_PLAYER_NIUNIU_DATA,
	MSG_SAVE_PLAYER_GAME_DATA,
	MSG_DB_CHECK_INVITER,
	MSG_PLAYER_CHECK_INVITER,
	MSG_DLG_NOTICE,
	MSG_REQUEST_EXCHANGE_LIST,
	MSG_REQUEST_EXCHANGE_DETAIL,
	MSG_PLAYER_EXCHANGE,
	MSG_GET_VIP_CARD_GIFT,
	MSG_READ_NOTICE_PLAYER,
	MSG_PUSH_NOTICE,
	MSG_SAVE_NOTICE_PLAYER,
	MSG_READ_EXCHANGE,
	MSG_SAVE_EXCHANGE,
	MSG_READ_PLAYER_BASE_DATA = 250,
	MSG_PLAYER_SAVE_PLAYER_INFO,
	MSG_PLAYER_LOGOUT,
	// friend module
	MSG_READ_FRIEND_LIST = 300,  //;
	MSG_SAVE_FRIEND_LIST,  // send to db 
	MSG_REQUEST_FRIEND_LIST,
	MSG_PLAYER_ADD_FRIEND,  // want to add other player 
	MSG_PLAYER_ADD_FRIEND_REPLAY,  // other player replay my request ;
	MSG_PLAYER_BE_ADDED_FRIEND,   // other player want to add me 
	MSG_PLAYER_BE_ADDED_FRIEND_REPLY,
	MSG_PLAYER_REPLAY_BE_ADD_FRIEND,  // I replay to other player who want to add me ;
	MSG_PLAYER_SERACH_PEERS, // mo hu search , to add other o be firend ;
	MSG_PLAYER_DELETE_FRIEND, // remove friend ;

	// friend invite ;
	MSG_PlAYER_INVITED_FRIEND_TO_JOIN_ROOM,  // invite a friend to join
	MSG_PLAYER_BE_INVITED,   // i were invited by my friend ;
	MSG_PLAYER_REPLAY_BE_INVITED,   // when i were invited by friend, i make a choice , reply;
	MSG_PLAYER_RECIEVED_INVITED_REPLAY, // the player I invited ,replayed me ;
	MSG_REQUEST_ROOM_RANK,
	MSG_REQUEST_LAST_TERM_ROOM_RANK,

	// msg id for room 
	MSG_CREATE_ROOM,
	MSG_DELETE_ROOM,
	// msg request math list ;
	MSG_REQUEST_MATCH_ROOM_LIST,
	// message id for taxas poker


	// new room msg are here ;
	MSG_PLAYER_ENTER_ROOM,
	MSG_PLAYER_SITDOWN,
	MSG_PLAYER_STANDUP,
	MSG_ROOM_SITDOWN,
	MSG_ROOM_STANDUP,
	MSG_SVR_ENTER_ROOM,
	MSG_SVR_DO_LEAVE_ROOM,
	MSG_SVR_DELAYED_LEAVE_ROOM,
	MSG_GET_MAX_ROOM_ID,
	MSG_TP_BEGIN = 450,
	MSG_TP_CREATE_ROOM,

	MSG_TP_ROOM_OWNER_BEGIN ,
	MSG_TP_MODIFY_ROOM_NAME,
	MSG_TP_MODIFY_ROOM_DESC,
	MSG_ADD_RENT_TIME,
	MSG_TP_ADD_RENT_TIME = MSG_ADD_RENT_TIME,
	MSG_TP_MODIFY_ROOM_INFORM,
	MSG_CACULATE_ROOM_PROFILE,
	MSG_TP_CACULATE_ROOM_PROFILE = MSG_CACULATE_ROOM_PROFILE,
	MSG_TP_REMIND_NEW_ROOM_INFORM,
	MSG_REMIND_NEW_ROOM_INFORM = MSG_TP_REMIND_NEW_ROOM_INFORM,
	MSG_TP_REQUEST_ROOM_LIST,
	MSG_REQUEST_ROOM_LIST = MSG_TP_REQUEST_ROOM_LIST,
	MSG_TP_ROOM_OWNER_END,

	MSG_TP_QUICK_ENTER,
	MSG_TP_REQUEST_PLAYER_DATA,
	unUsed_MSG_TP_ROOM_BASE_INFO,
	MSG_TP_ROOM_VICE_POOL,
	MSG_TP_ROOM_PLAYER_DATA,

	MSG_TP_WITHDRAWING_MONEY,

	MSG_TP_SYNC_PLAYER_DATA,
	// in room msg 
	MSG_TP_ENTER_STATE,
	MSG_TP_START_ROUND,
	MSG_TP_PRIVATE_CARD,
	MSG_TP_WAIT_PLAYER_ACT,
	MSG_TP_PLAYER_ACT,
	MSG_TP_ROOM_ACT,
	MSG_TP_ONE_BET_ROUND_RESULT,
	MSG_TP_PUBLIC_CARD,
	MSG_TP_GAME_RESULT,
	MSG_TP_UPDATE_PLAYER_STATE,

	MSG_TP_READ_MY_OWN_ROOMS,
	MSG_READ_MY_OWN_ROOMS = MSG_TP_READ_MY_OWN_ROOMS,
	MSG_TP_CHANGE_ROOM,
	MSG_REQUEST_ROOM_INFO,
	MSG_TP_END = 700,
	// mail module
	MSG_PLAYER_SAVE_MAIL ,
	MSG_PLAYER_SET_MAIL_STATE,
	MSG_PLAYER_READ_MAIL_LIST,
	MSG_PLAYER_REQUEST_MAIL_LIST,
	MSG_PLAYER_NEW_MAIL_ARRIVED,
	MSG_PLAYER_INFORM_NEW_MAIL,
	MSG_PLAYER_LOOK_MAIL,
	MSG_GAME_SERVER_GET_MAX_MAIL_UID,
	MSG_PLAYER_PROCESSED_MAIL,
	MSG_SAVE_PLAYER_ADVICE,
	MSG_PLAYER_ADVICE,
	MSG_PLAYER_MAIL_MODULE = 750,
	MSG_PLAYER_REQUEST_CHARITY_STATE,
	MSG_PLAYER_GET_CHARITY,
	MSG_BUY_SHOP_ITEM,

	MSG_SAVE_ROOM_PLAYER,
	MSG_READ_ROOM_PLAYER,
	MSG_REMOVE_ROOM_PLAYER,

	MSG_SHOP_BUY_ITEM_ORDER,
	MSG_VERIFY_ITEM_ORDER,
	MSG_ROOM_ENTER_NEW_STATE = 800,
	// poker circle 
	MSG_CIRCLE_BEGIN = 1000,
	MSG_CIRCLE_READ_TOPICS,
	MSG_CIRCLE_SAVE_ADD_TOPIC,
	MSG_CIRCLE_SAVE_DELETE_TOPIC,
	MSG_CIRCLE_PUBLISH_TOPIC,
	MSG_CIRCLE_DELETE_TOPIC,
	MSG_CIRCLE_REQUEST_TOPIC_DETAIL,
	MSG_CIRCLE_REQUEST_TOPIC_LIST,

	// robot specail msg 
	MSG_ADD_MONEY = 1300, 
	MSG_TELL_PLAYER_TYPE,
	MSG_TELL_ROBOT_IDLE,
	MSG_TELL_ROBOT_ENTER_ROOM,
	MSG_REQ_ROBOT_ENTER_ROOM,
	MSG_TELL_ROBOT_LEAVE_ROOM,
	MSG_REQ_CUR_GAME_OFFSET,
	MSG_REQ_TOTAL_GAME_OFFSET,
	MSG_ADD_TEMP_HALO,
	MSG_MODIFY_ROOM_RANK,
};