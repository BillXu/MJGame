#pragma once 

enum eMJActType
{
	eMJAct_Mo,
	eMJAct_Peng,
	eMJAct_MingGang,
	eMJAct_AnGang,
	eMJAct_BuGang,
	eMJAct_BuGang_Pre,
	eMJAct_BuGang_Done,
	eMJAct_Hu,
	eMJAct_Chi,
	eMJAct_Chu,
	eMJAct_Pass,
	eMJAct_Max,
};

enum eTime
{
	eTime_ExeGameStart = 5,
	eTime_WaitChoseExchangeCard = 5,
	eTime_DoExchangeCard = 3,
	eTime_WaitDecideQue = 5,
	eTime_DoDecideQue = 2,
	eTime_WaitPlayerAct = 8,
	eTime_DoPlayerMoPai = 1 ,
	eTime_DoPlayerActChuPai = 2,
	eTime_DoPlayerAct_Gang = 2,
	eTime_DoPlayerAct_Hu = 3,
};

enum eMsgPort
{
	ID_MSG_PORT_NONE , // client to game server 
	ID_MSG_PORT_CLIENT,
	ID_MSG_PORT_GATE,
	ID_MSG_PORT_CENTER,
	ID_MSG_PORT_LOGIN,
	ID_MSG_PORT_VERIFY,
	ID_MSG_PORT_APNS,
	ID_MSG_PORT_LOG,
	ID_MSG_PORT_DATA,
	ID_MSG_PORT_DB,
	ID_MSG_PORT_ALL_SERVER,
	ID_MSG_PORT_NIU_NIU,
	ID_MSG_PORT_MJ = ID_MSG_PORT_NIU_NIU ,
	ID_MSG_VERIFY,
	ID_MSG_PORT_TAXAS,
	ID_MSG_PORT_GOLDEN,
};


enum eMsgType 
{
	MSG_CLIENT = 10000,
	MSG_JSON_CONTENT,

	// login 
	MSG_PLAYER_REGISTER,     
	// client : {acc: "account" , pwd : "password",regType : 1 , regChannel : 0  } 
	// svr : { ret: 0  , regType : 1 , UID : 2345 }
	// ret : 0 success , 1 account already exist
	// regType : 0 visitor , 1 normal , 2 reBindAccount ;
	// regChannel : 0 appstore 

	MSG_PLAYER_LOGIN,  // check an account is valid ; 
	// client : {acc: "account" , pwd : "password" }
	// svr : { ret : 0 , regType : 2 }
	// ret : 0 ,success , 1 account error , 2 password error , 3 state error 

	MSG_PLAYER_BIND_ACCOUNT, //  a quick enter player need to bind a real account and password ; 
	// client : {acc: "account" , pwd : "password", UID : 2330 }
	// svr :	{ ret: 0 }
	// ret : 0 success , 1 double account , 2 uid not exsit, 3 unknown error  ;

	MSG_MODIFY_PASSWORD,  
	// client : { oldPwd: "oldPassword", pwd : "password", UID : 2000 }
	// svr : { ret : 0 }
	// ret :  0 success , 1 uid not exist , 2 old password error 

	MSG_PLAYER_OTHER_LOGIN,  // more than one place login , prelogin need disconnect ; client recived must disconnect from server

	MSG_PLAYER_BASE_DATA, // self player base data ,
	// svr : { name: "nickName",sex : "male",coin : 235 , diamond: 500, }

	// modify name and sigure
	MSG_PLAYER_MODIFY_NAME,
	MSG_PLAYER_MODIFY_SIGURE,
	MSG_PLAYER_MODIFY_PHOTO,
	MSG_PLAYER_UPDATE_MONEY,  // USE WHEN OTHER MAIL A GITF  ;
	MSG_PLAYER_MODIFY_SEX,
	MSG_RESET_PASSWORD,
	MSG_REQUEST_PLAYER_INFO,
	
	// mj room msg 
	MSG_REQ_ENTER_ROOM,
	// client : { roomType: "blood" , configID : "12" }
	// svr : { ret : 0  }
	// ret : 0 ,success , 1 argument error , 2 already in room , 3 coin not enough ;

	MSG_ROOM_INFO,
	// svr : { configID : 23 , players : [ {idx : 0 , uid : 233, coin : 2345 }, {idx : 0 , uid : 233, coin : 2345 },{idx : 0 , uid : 233, coin : 2345 } , ... ] }

	MSG_ROOM_PLAYER_ENTER,
	// svr : {idx : 0 , uid : 233, coin : 2345 }

	MSG_PLAYER_SET_READY,
	// client : null ;

	MSG_ROOM_PLAYER_READY,
	// svr : { idx : 2 }

	MSG_ROOM_START_GAME,
	// svr : { banker: 2 , dice : 3 , peerCards : [ { cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] },{cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] } ] }

	MSG_ROOM_WAIT_CHOSE_EXCHANG,
	// svr : null 

	MSG_PLAYER_CHOSED_EXCHANGE,
	// client : { cards: [ 3, 1,2] }
	// svr : { ret : 0 }
	// ret : 0 success , 1 you do not have card , 2 chose card count not equal 3 . 3 you are not sit join this game , 4 do not chose twice;

	MSG_ROOM_FINISH_EXCHANGE,
	// svr : { mode : 0 , result : [ { idx = 0 , cards : [ 2, 4 ,5]}, { idx = 1 , cards : [ 2, 4 ,5]},{ idx = 2 , cards : [ 2, 4 ,5]},{ idx = 3 , cards : [ 2, 4 ,5]}  ] }
	// mode : 0 shun shi zhen , 1 ni shi zhen, 2 dui 

	MSG_ROOM_WAIT_DECIDE_QUE,
	// svr : null ;

	MSG_PLAYER_DECIDE_QUE,
	// client : { type : 2 }
	// type: 0 wan , 1 tiao , 2 tong 

	MSG_ROOM_FINISH_DECIDE_QUE,
	// svr : { ret : [{type0, type 1 , type 2, type3 ] }

	MSG_ROOM_PLAYER_FETCH_CARD,
	// svr : { card : 23} 

	MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,  // you can shou , hu , peng , or gang 
	// svr : null ;

	MSG_PLAYER_ACT,
	// client : { actType : 0 , card : 23}
	// actType : eMJActType
	// svr : { ret : 0 }
	// ret : 0 success , 1 not your turn , 2 not allow , 3 argument error , 4 state error ;

	MSG_ROOM_ACT,
	// svr : { idx : 0 , actType : 234, card : 23 }

	MSG_ROOM_WAIT_RECHARGE,
	// svr: { players: [0,1,3]} 
	
	MSG_ROOM_GAME_OVER,
	// svr : { players : [ {idx : 0 , coin : 2345 } ,{idx : 1 , coin : 2345 } ,{idx : 2 , coin : 2345 },{idx : 3 , coin : 2345 } ]  } 
};
