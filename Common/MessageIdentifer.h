#pragma once 
//#define  C_SHARP 
#if (C_SHARP)  
public
#endif
enum eMJActType
{
	eMJAct_Mo, // 摸牌
	eMJAct_Peng,  // 碰牌
	eMJAct_MingGang,  // 明杠
	eMJAct_AnGang, // 暗杠
	eMJAct_BuGang,  // 补杠 
	eMJAct_BuGang_Pre, // 补杠第一阶段
	eMJAct_BuGang_Done, //  补杠第二阶段，执行杠牌
	eMJAct_Hu,  //  胡牌
	eMJAct_Chi, // 吃
	eMJAct_Chu, // 出牌
	eMJAct_Pass, //  过 
	eMJAct_Max,
};

#if (C_SHARP)  
public
#endif
enum eTime
{
	eTime_ExeGameStart = 5,			// 执行游戏开始 的时间
	eTime_WaitChoseExchangeCard = 5, //  等待玩家选择换牌的时间
	eTime_DoExchangeCard = 3, //   执行换牌的时间
	eTime_WaitDecideQue = 5, // 等待玩家定缺
	eTime_DoDecideQue = 2, // 定缺时间
	eTime_WaitPlayerAct = 8,  // 等待玩家操作的时间
	eTime_DoPlayerMoPai = 1 ,  //  玩家摸牌时间
	eTime_DoPlayerActChuPai = 2,  // 玩家出牌的时间
	eTime_DoPlayerAct_Gang = 2, // 玩家杠牌时间
	eTime_DoPlayerAct_Hu = 3,  // 玩家胡牌的时间
};

#if (C_SHARP)  
public
#endif
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

// room state 
#if (C_SHARP)  
public
#endif
enum eRoomState  // 玩家的状态
{
	eRoomState_None,
	eRoomState_Opening,
	eRoomState_WillDead,
	eRoomState_Dead,
	eRoomState_WillClose,
	eRoomState_Close,
	eRoomSate_WaitReady,  // 等待玩家准备
	eRoomState_StartGame, // 开始游戏

	eRoomState_WaitExchangeCards, //  等待玩家换牌
	eRoomState_DoExchangeCards , // 玩家换牌
	eRoomState_WaitDecideQue,  // 等待玩家定缺
	eRoomState_DoDecideQue,  //  玩家定缺
	eRoomState_DoFetchCard, // 玩家摸牌
	eRoomState_WaitPlayerAct,  // 等待玩家操作
	eRoomState_DoPlayerAct,  // 玩家操作
	eRoomState_WaitOtherPlayerAct,  // 等待玩家操作，有人出牌了
	eRoomState_DoOtherPlayerAct,  // 其他玩家操作了。
	eRoomState_WaitPlayerRecharge,  //  等待玩家充值
	eRoomState_GameEnd, // 游戏结束

	eRoomState_Max,
};

// player State 
#if (C_SHARP)  
public
#endif
enum eRoomPeerState  // 牌局内玩家的状态
{
	eRoomPeer_None,
	// peer state for taxas poker peer
	eRoomPeer_SitDown = 1,
	eRoomPeer_StandUp = 1 << 1,
	eRoomPeer_Ready =  (1<<12)|eRoomPeer_SitDown , // 已经准备的状态
	eRoomPeer_StayThisRound = ((1 << 2)|eRoomPeer_SitDown)| eRoomPeer_Ready ,
	eRoomPeer_WaitCaculate = ((1 << 7)|eRoomPeer_StayThisRound ),
	eRoomPeer_AllIn = ((1 << 3)|eRoomPeer_WaitCaculate) ,
	eRoomPeer_GiveUp = ((1 << 4)|eRoomPeer_StayThisRound),
	eRoomPeer_CanAct = ((1 << 5)|eRoomPeer_WaitCaculate), // 可操作状态，正在牌局中
	eRoomPeer_WaitNextGame = ((1 << 6)|eRoomPeer_SitDown ),
	eRoomPeer_AlreadyHu = ((1 << 8)|eRoomPeer_CanAct ),  //  已经胡牌的状态
	eRoomPeer_DecideLose = eRoomPeer_GiveUp ,  // 认输状态
	eRoomPeer_Max,
};

#if (C_SHARP)  
public
#endif
enum eMsgType 
{
	MSG_CLIENT = 10000,
	MSG_JSON_CONTENT,

	// login 
	MSG_PLAYER_REGISTER,     //玩家注册
	// client : {acc: "account" , pwd : "password",regType : 1 , regChannel : 0  } 
	// svr : { ret: 0  , regType : 1 , UID : 2345 }
	// ret : 0 success , 1 account already exist
	// regType : 注册类型。0 游客注册 , 1 正常注册 , 2 绑定账号 ;
	// regChannel : 0 appstore  // 注册渠道

	MSG_PLAYER_LOGIN,  // 玩家登陆 ; 
	// client : {acc: "account" , pwd : "password" }
	// svr : { ret : 0 , regType : 2 }
	// ret : 0 ,登录成功 , 1 账号错误 , 2 密码错误 , 3 状态错误（账号被封） 

	MSG_PLAYER_BIND_ACCOUNT, //  绑定账号，游客需要绑定账号 ; 
	// client : {acc: "account" , pwd : "password", UID : 2330 }
	// svr :	{ ret: 0 }
	// ret : 0 绑定成功 , 1 账号已经存在 , 2 uid 不存在, 3 未知错误  ;

	MSG_MODIFY_PASSWORD,  
	// client : { oldPwd: "oldPassword", pwd : "password", UID : 2000 }
	// svr : { ret : 0 }
	//  oldPwd：旧密码， pwd：新密码，uid：玩家的唯一ID。
	// ret :  0 成功 , 1 uid 不存在 , 2 旧密码错误 

	MSG_PLAYER_OTHER_LOGIN,  // 账号在其他设备登录，当前设备需要退出

	MSG_PLAYER_BASE_DATA, // 玩家的基础信息 ,
	// svr : { name: "nickName",sex : 0,coin : 235 , diamond: 500, }
	// name ： 名字，sex ： 性别（0 是男，1 是女）， diamond ：钻石。 coin ： 金币；

	// modify name and sigure
	MSG_PLAYER_MODIFY_NAME,
	MSG_PLAYER_MODIFY_SIGURE,
	MSG_PLAYER_MODIFY_PHOTO,
	MSG_PLAYER_UPDATE_MONEY,  // USE WHEN OTHER MAIL A GITF  ;
	MSG_PLAYER_MODIFY_SEX,
	MSG_RESET_PASSWORD,
	MSG_REQUEST_PLAYER_INFO,
	
	// mj room msg 麻将房间信息。客户端发给svr的信息，必须包含 dstRoomID 的 key 
	MSG_REQ_ENTER_ROOM,
	// client : { roomType: "blood" , configID : "12" }
	// svr : { ret : 0  }
	// ret :  0 success , 1 已经在房间里 , 2 房间要求游客不能进入 ; 3 金币不足 ; 4 ;  金币太多 ; 5 找不到指定id 的fangjian ,  6 房间类型错误 

	MSG_ROOM_INFO,  // 房间的基本信息
	// svr : { roomID ： 23 , configID : 23 , roomState :  23 , players : [ {idx : 0 , uid : 233, coin : 2345 , state : 34 }, {idx : 0 , uid : 233, coin : 2345, state : 34 },{idx : 0 , uid : 233, coin : 2345 , state : 34} , ... ] }
	// roomState  , 房间状态

	MSG_ROOM_PLAYER_ENTER, // 有其他玩家进入房间
	// svr : {idx : 0 , uid : 233, coin : 2345,state : 34 }

	MSG_PLAYER_SET_READY,   // 玩家准备
	// client : { dstRoomID : 2345 } ;

	MSG_ROOM_PLAYER_READY,  // 其他玩家准备
	// svr : { idx : 2 }

	MSG_ROOM_START_GAME,  // 开始游戏的消息
	// svr : { banker: 2 , dice : 3 , peerCards : [ { cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] },{cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] } ] }
	// banker 庄家的索引 , dice : 骰子的点数； cards ： 玩家的手牌

	MSG_ROOM_WAIT_CHOSE_EXCHANG,  //  通知玩家选择三张牌 进行交互
	// svr : null 

	MSG_PLAYER_CHOSED_EXCHANGE,   // 玩家选好要交换的牌
	// client : { dstRoomID : 2345 ,cards: [ 3, 1,2] }
	// svr : { ret : 0 }
	// ret : 0 成功 , 1 你选择的牌里面有错误, 2 选择的牌 张数不对 3 . 你没有参加牌局 , 4 你已经选择了，不要选择两次;

	MSG_ROOM_FINISH_EXCHANGE,  //  所有玩家完成选择；
	// svr : { mode : 0 , result : [ { idx = 0 , cards : [ 2, 4 ,5]}, { idx = 1 , cards : [ 2, 4 ,5]},{ idx = 2 , cards : [ 2, 4 ,5]},{ idx = 3 , cards : [ 2, 4 ,5]}  ] }
	// mode : 换牌模式， 0 顺时针换 , 1 逆时针换, 2 对家换 

	MSG_ROOM_WAIT_DECIDE_QUE, // 进入等待玩家定缺的状态
	// svr : null ;

	MSG_PLAYER_DECIDE_QUE,  // 玩家定缺
	// client : { dstRoomID : 2345 , type : 2 }
	// type: 定缺的类型，0 万 , 1 条 , 2 筒 


	MSG_ROOM_FINISH_DECIDE_QUE,  // 玩家完成 定缺
	// svr : { ret : [{type0, type 1 , type 2, type3 ] }
	// 数组对应玩家 定的缺门类型。

	MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,  // 有人出了一张牌，等待需要这张牌的玩家 操作，可以 碰，杠，胡
	// svr : { players : [ {idx : 1 , acts : [type0, type 1 , ..] },{idx : 0 , acts : [type0, type 1 , ..] }, ... ] } ;
	// 可操作的玩家是一个数组，因为同一张牌，可能有多个玩家需要。 玩家可执行的操作，也是一个数组，同一张牌，玩家可以执行多种操作，type 类型参照 eMJActType

	MSG_PLAYER_ACT, // 玩家操作
	// client : { dstRoomID : 2345 ,actType : 0 , card : 23}
	// actType : eMJActType   操作类型，参照枚举值
	// svr : { ret : 0 }
	// ret : 0 操作成功 , 1 没有轮到你操作 , 2 不能执行指定的操作，条件不满足, 3 参数错误 , 4 状态错误 ;

	MSG_ROOM_ACT,  // 房间里有玩家执行了一个操作
	// svr : { idx : 0 , actType : 234, card : 23 }
	// idx :  执行操作的那个玩家的索引。 actType : 执行操作的类型，参照枚举值eMJActType 。 card： 操作涉及到的牌

	MSG_ROOM_WAIT_RECHARGE, // 等待一些玩家充值
	// svr: { players: [0,1,3]}    
	// players : 需要充值的玩家所以 数组，可能有多个玩家。
	
	MSG_ROOM_GAME_OVER, // 游戏结束
	// svr : { players : [ {idx : 0 , coin : 2345 } ,{idx : 1 , coin : 2345 } ,{idx : 2 , coin : 2345 },{idx : 3 , coin : 2345 } ]  } 
	// players: 结束后，每个玩家最终的钱数。

	MSG_PLAYER_LEAVE_ROOM, // 玩家离开房间
	// client : {dstRoomID : 23 } ;
	// svr : { ret : 0 }
	// ret : 0 表示成功， 1 房间号错误,不在该房间里。
	

	MSG_ROOM_PLAYER_CARD_INFO,
	// svr : { playersCard: [ { anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34] },{ anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34] }, .... ] }
	// 重新进入已经在玩的房间，或者断线重连，就会收到这个消息， anPai 就是牌，没有展示出来的，mingPai 就是已经展示出来的牌（碰，杠），huPai ： 已经胡了的牌。
};
