#pragma once 
//#define  C_SHARP 
#if (C_SHARP)  
public
#endif
enum eMJActType
{
	eMJAct_None,
	eMJAct_Mo = eMJAct_None, // 摸牌
	eMJAct_Chi, // 吃
	eMJAct_Peng,  // 碰牌
	eMJAct_MingGang,  // 明杠
	eMJAct_AnGang, // 暗杠
	eMJAct_BuGang,  // 补杠 
	eMJAct_BuGang_Pre, // 补杠第一阶段
	eMJAct_BuGang_Declare = eMJAct_BuGang_Pre, // 声称要补杠 
	eMJAct_BuGang_Done, //  补杠第二阶段，执行杠牌
	eMJAct_Hu,  //  胡牌
	eMJAct_Chu, // 出牌
	eMJAct_Pass, //  过 
	eMJAct_Max,
};

#if (C_SHARP)  
public
#endif
enum eFanxingType 
{
	eFanxing_PingHu, // 平胡

	eFanxing_DuiDuiHu, //  对对胡

	eFanxing_QingYiSe, // 清一色
	eFanxing_DaiYaoJiu, //  带幺九
	eFanxing_QiDui, //  七对
	eFanxing_JinGouDiao, //  金钩钓

	eFanxing_QingDuiDuiHu, // 清对对胡

	eFanxing_QingDui, //  清七对
	eFanxing_LongQiDui, //  龙七对
	eFanxing_QingDaiYaoJiu, //  清 带幺九
	eFanxing_JiangJinGouDiao, // 将金钩钓
	eFanxing_QingJinGouDiao, // 清金钩钓

	eFanxing_QingLongQiDui, //  清龙七对
	eFanxing_ShiBaLuoHan, //  十八罗汉
	eFanxing_Max, // 没有胡
};

#if (C_SHARP)  
public
#endif
enum eRoomType
{
	eRoom_None,
	eRoom_MJ = eRoom_None,
	eRoom_MJ_Blood_End = eRoom_None, // 血战到底 
	eRoom_MJ_Blood_River,// 血流成河
	eRoom_MJ_MAX,
	eRoom_NiuNiu = eRoom_MJ_Blood_River, // not used 
	eRoom_TexasPoker,   // not used 
	eRoom_Golden, // not used 
	eRoom_Max = eRoom_MJ_MAX,
};

#if (C_SHARP)  
public
#endif
enum eTime
{
	eTime_ExeGameStart = 5,			// 执行游戏开始 的时间
	eTime_WaitChoseExchangeCard = 5, //  等待玩家选择换牌的时间
	eTime_DoExchangeCard = 3, //   执行换牌的时间
	eTime_WaitDecideQue = 10, // 等待玩家定缺
	eTime_DoDecideQue = 2, // 定缺时间
	eTime_WaitPlayerAct = 80000,  // 等待玩家操作的时间
	eTime_WaitPlayerChoseAct = eTime_WaitPlayerAct,
	eTime_DoPlayerMoPai = 1 ,  //  玩家摸牌时间
	eTime_DoPlayerActChuPai = 2,  // 玩家出牌的时间
	eTime_DoPlayerAct_Gang = 2, // 玩家杠牌时间
	eTime_DoPlayerAct_Hu = 3,  // 玩家胡牌的时间
	eTime_DoPlayerAct_Peng = 2, // 玩家碰牌时间
	eTime_GameOver = 1, // 游戏结束状态持续时间
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
	eRoomState_WaitPlayerAct,  // 等待玩家操作 { idx : 0 , exeAct : eMJActType , isWaitChoseAct : 0 , actCard : 23  }
	eRoomState_DoPlayerAct,  // 玩家操作
	eRoomState_WaitOtherPlayerAct,  // 等待玩家操作，有人出牌了 { invokerIdx : 0 , card : 0 ,cardFrom : eMJActType , arrNeedIdxs : [2,0,1] } 
	eRoomState_DoOtherPlayerAct,  // 其他玩家操作了。
	eRoomState_WaitSupplyCoin , // 等待玩家补充金币
	eRoomState_WaitPlayerRecharge = eRoomState_WaitSupplyCoin,  //  等待玩家充值
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
enum eSex
{
	eSex_Unknown,  // 未知
	eSex_Male,  // 男
	eSex_Female, // 女
	eSex_Max,
};

#if (C_SHARP)  
public
#endif
enum eMailType
{
	eMail_SysOfflineEvent,// { event: concret type , arg:{ arg0: 0 , arg 1 = 3 } }  // processed in svr , will not send to client ;
	eMail_DlgNotice, // content will be send by , stMsgDlgNotice 
	eMail_ReadTimeTag,  // use tell time for public mail ;
	eMail_Sys_End,

	// 以下为客户端需要识别的邮件类型
	eMail_RealMail_Begin, // 开始标识 ;
	eMail_PlainText,  // 内容为普通字符串，注意： 不是json对象 ;
	eMail_InvitePrize, // { targetUID : 2345 , addCoin : 300 } // you invite player to join game ,and give prize to you 
	eMail_WinMatch, // { gameType:234,roomName:234,rankIdx:2,addCoin:345,cup : 2 , diamomd : 34 }
	eMail_VIP_INVITE, // { inviteUID : 2345 , roomID : 2345 } vip 房间邀请， inviteUID 邀请者的UID， roomID 房间的id ，可以通过这个进入房间
	eMail_Max,
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
	// svr : { ret: 0  , acc: "account" , pwd : "password",regType : 1 , UID : 2345 }
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
	// svr : { name: "nickName",sex : eSex,coin : 235 , diamond: 500,uid : 2345, sessionID : 2345, vipRoomCard : 23, clothe : [235,235,234] }
	// name ： 名字，sex ： 参照枚举eSex， diamond ：钻石。 coin ： 金币； clothe : 玩家穿在身上的衣服或者饰品

	// modify name and sigure
	MSG_PLAYER_MODIFY_NAME, // 玩家修改昵称
	// client : { newName : helloWorld }
	// svr : { ret : 0 , newName : helloWorld}
	// newName : 玩家新设置的姓名
	// ret : 0 成功，1 名字长度太长

	MSG_PLAYER_MODIFY_SIGURE,
	MSG_PLAYER_MODIFY_PHOTO,
	MSG_PLAYER_UPDATE_MONEY,  // USE WHEN OTHER MAIL A GITF  ;
	
	MSG_PLAYER_MODIFY_SEX, // 修改玩家的性别
	// client : { newSex : eSex }
	// svr : { newSex : eSex }
	// newSex ,新设置的性别 ，参考eSex 枚举值

	MSG_RESET_PASSWORD,
	MSG_REQUEST_PLAYER_INFO,
	
	MSG_DO_RECONNECT, // 断线重连的消息
	// client : { nSessionID : 233 }
	// svr : { ret : 0 }
	// ret : 0 表示成功，1 表示失败；seesion id 就是会话ID 登录成功以后会服务器返回

	// mj room msg 麻将房间信息。客户端发给svr的信息，必须包含 dstRoomID 的 key 
	MSG_REQ_ENTER_ROOM = 10115,
	// client : { type : 0 ， targetID : 23 }
	// svr : { ret : 0  }
	// ret :  0 success , 1 已经在房间里 , 2 房间要求游客不能进入 ; 3 金币不足 ; 4 ;  金币太多 ; 5 找不到指定id 的fangjian ,  6 房间类型错误 8 房间已经满了。
	// type = 0 , 就是随机匹配房间，targetID 的值对应的是configID的值， type = 1 ， 的时候表示进入指定的某个房间，targetID 此时表示的是 RoomID 。

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
	// type: 定缺的类型，1,万 2, 筒 3, 条


	MSG_ROOM_FINISH_DECIDE_QUE,  // 玩家完成 定缺
	// svr : { ret : [ {type : 0, idx : 2 }, {type : 0, idx : 1 } , {type : 0, idx : 2 }, {type : 0, idx : 3 }] }
	// 数组对应玩家 定的缺门类型。

	MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,  // 有人出了一张牌，等待需要这张牌的玩家 操作，可以 碰，杠，胡
	// svr : { cardNum : 32 , acts : [type0, type 1 , ..] }  ;
	// 这个消息不会广播，只会发给需要这张牌的玩家，cardNum 待需要的牌，type 类型参照 eMJActType

	MSG_PLAYER_WAIT_ACT_AFTER_RECEIVED_CARD,  // 自己获得一张牌，杠或者摸，然后可以进行的操作 杠，胡
	// svr : { acts : [ {act :eMJActType , cardNum : 23 } , {act :eMJActType , cardNum : 56 }, ... ]  }  ;
	// 这个消息不会广播，只会发给当前操作的玩家，acts： 可操作的数组， 因为获得一张牌，以后可以进行的操作很多。cardNum 操作相对应的牌，type 类型参照 eMJActType

	MSG_PLAYER_ACT, // 玩家操作
	// client : { dstRoomID : 2345 ,actType : 0 , card : 23}
	// actType : eMJActType   操作类型，参照枚举值
	// svr : { ret : 0 }
	// ret : 0 操作成功 , 1 没有轮到你操作 , 2 不能执行指定的操作，条件不满足, 3 参数错误 , 4 状态错误 ;

	MSG_ROOM_ACT,  // 房间里有玩家执行了一个操作
	// svr : { idx : 0 , actType : 234, card : 23, gangCard : 12 }
	// idx :  执行操作的那个玩家的索引。 actType : 执行操作的类型，参照枚举值eMJActType 。 card： 操作涉及到的牌  gangCard: 杠牌后 获得的牌;

	MSG_ROOM_WAIT_RECHARGE, // 等待一些玩家充值
	// svr: { players: [0,1,3]}    
	// players : 需要充值的玩家所以 数组，可能有多个玩家。
	
	MSG_ROOM_GAME_OVER, // 游戏结束
	// svr : { players : [ {idx : 0 , coin : 2345 ,huType : eFanxingType } ,{idx : 1 , coin : 2345 ,huType : eFanxingType } ,{idx : 2 , coin : 2345,huType : eFanxingType },{idx : 3 , coin : 2345,huType : eFanxingType } ]  } 
	// eFanxingType 参照枚举值
	// players: 结束后，每个玩家最终的钱数。

	MSG_PLAYER_LEAVE_ROOM, // 玩家离开房间
	// client : {dstRoomID : 23 } ;
	// svr : { ret : 0 }
	// ret : 0 表示成功， 1 房间号错误,不在该房间里。

	MSG_ROOM_PLAYER_LEAVE, // 有玩家离开房间;
	// svr : { idx : 2 }
	
	MSG_PLAYER_REQ_ROOM_INFO,
	// client : {dstRoomID : 23 } ;
	// 此消息请求房间详细信息，用来恢复房间的现场，一般用在断线重连成功。

	MSG_ROOM_PLAYER_CARD_INFO,
	// svr : { bankerIdx : 2, leftCardCnt : 32 ,playersCard: [ { idx : 2,queType: 2, anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34], chuPai: [2,34,4] },{ anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34] }, .... ] }
	// leftCardCnt : 剩余牌的数量，重新进入已经在玩的房间，或者断线重连，就会收到这个消息， anPai 就是牌，没有展示出来的，mingPai 就是已经展示出来的牌（碰，杠），huPai ： 已经胡了的牌。 queType : 1,万 2, 筒 3, 条

	MSG_ROOM_REQ_TOTAL_INFO,
	// client : {dstRoomID : 23 } ;
	// 断线重连成功,以后请求房间信息，恢复房间现场。

	MSG_TELL_ROBOT_TYPE = 10338, // 向服务器表明客户端是机器人的身份
	MSG_TELL_ROBOT_IDLE, // 通知服务器，当前机器人空闲，也就是没有在房间内。
	MSG_SVR_INFORM_ROBOT_LEAVE, // 服务器通知机器人退出当前房间。
	MSG_SVR_INFOR_ROBOT_ENTER, // 服务器通知机器人进入房间；
	// svr : { dstRoomID : 0 }
	// dstRoomID , 服务器通知机器人需要进入的房间ID。

	// vip 房间消息
	MSG_CREATE_VIP_ROOM,  // 创建vip房间 
	// client : { circle : 2 , baseBet : 1, initCoin : 2345 , roomType : eRoomType, seatCnt : 4  }
	// svr : { ret : 0 , roomID : 2345 }
	// circle 表示创建房间的圈数，baseBet 基础底注 ，initCoin 每个人的初始金币， roomType 房间类型， 0 是血流，1 是血战。 ret ： 0 表示成功，1 表示房卡不够， 2 ，表示不能创建更多房间, seatCnt : 座位个数。

	MSG_VIP_ROOM_INFO_EXT, // VIP 房间的额外信息；
	// svr : { leftCircle : 2 , baseBet : 1 , creatorUID : 2345 , initCoin : 2345 }
	// letCircle : 剩余的圈数， baseBet 基础底注 ，creatorUID 创建者的ID , initCoin 每个人的初始金币

	MSG_APPLY_DISMISS_VIP_ROOM, // 申请解散vip 房间
	// client : { dstRoomID : 234 } 

	MSG_ROOM_APPLY_DISMISS_VIP_ROOM , //房间里有人申请解散vip 房间
	// svr : { applyerIdx : 2 }
	// applyerIdx : 申请者的idx 

	MSG_REPLY_DISSMISS_VIP_ROOM_APPLY,  // 答复申请解散的请求
	// client { dstRoomID : 23 , reply : 0 }
	// reply ： 0 表示同意， 1 表示拒绝。
	
	MSG_VIP_ROOM_GAME_OVER,  // vip 房间结束
	// svr : { ret : 0 , initCoin : 235 , bills : [ { uid : 2345 , curCoin : 234 }, ....]  }
	// ret , 0 正常结束， 1 房间被解散。 initCoin 房间的初始金币，bills，是一个数组 放着具体每个玩家的情况，curCoin 表示玩家最终剩余金额, uid 玩家的唯一id 

	// shop module
	MSG_SHOP_BUY_ITEM = 10548,  // 购买商店里的商品
	// client : { shopItemID : 2345 }
	// svr : { ret : 0 , shopItemID : 2345 }
	// shopItemID ， 商品的配置ID。
	// ret : 0 购买成功， 1 商品部存在，2 货币不足, 3 道具ID 不存在；

	MSG_REQ_MY_BAG,  // 请求背包内容
	// client : null 
	// svr : { items : [ {itemID : 234 , cnt : 23 , buyTime : 23345, leftTime : 2352 } , {itemID : 2 , cnt : 23 , buyTime : 23345, leftTime : 2352},  ...... ] }
	// items : 所以物品数组
	//  itemID 道具的ID , cnt : 数量 ， buyTime : 购买的时间, leftTime 剩余时间 单位是秒

	MSG_START_ROLL_PLATE,// 转转盘
	// client : null 
	// svr : { ret : 0 , plateID : 234 , isFree : 0  }
	// ret : 0 成功 , 1 货币不足 , 2 系统错误
	// plateID :  命中的 配置ID， 根据配置ID 给玩家物品
	// isFree : 本次转盘 是否免费， 0 是否，1 是 是。

	MSG_PLAYER_DO_EXCHANGE, // 玩家兑换
	// client : { configID : 235 , info :{ phone : 12345 , addr : "shanghai changNing chian" } }
	// ret : { ret : 0 , configID : 235 }
	// ret : // 0 兑换成功 , 1 指定的配置id 不存在 , 2 钻石货币不足，不能兑换 ; 3 其他错误 ; 
	// configID : 兑换表里的配置ID，info ： 玩家填写的基本信息，客户端可以自行扩展，原样存入数据库。

	MSG_PLAYER_DO_GET_CHARITY, // 领取救济金
	// client : null 
	// svr : { ret : 0 , finalCoin : 23455 , recievedCoin : 234 , leftTimes : 23 }
	// ret : 0 成功， 1 表示达到次数限制，finalCoin 领取后的最终金币数额 ，recievedCoin 领取到的金币， leftTimes 剩余可领取次数;

	MSG_REQUEST_PLAYER_BRIF_INFO, // 请求玩家的基本信息；
	// client : { targetUID : 23456 }
	// svr : { ret : 0 , nickName : "lucy" , photoID : 2345 , uid : 2345 , ip : "192.168.1.56" ,sex : eSex, diamond : 2345, phone : 18023043245 }
	// ret : 0 成功，1 不存在该玩家
	// nickName : 昵称 ， photoID : 头像ID， uid ： 玩家Id , ip： 玩家的ip地址，只有在线的玩家存在，否则为空。 sex ： 玩家性别， diamond : 玩家的钻石， phone ： 玩家的手机号

	MSG_TELL_SELF_IP, // 通知玩家自己的IP 地址
	// svr : { ip : "192.235.133.23" }

	MSG_REQ_UPDATE_COIN, // 获取玩家最新的金钱
	// client : null ;
	// svr : { coin : 2345 , diamond : 2345 };

	// friend module 
	MSG_REQ_ADD_FRIEND, // 请求添加好友
	// client : { targetUID : 2345 , notice : "i want to make friend with you" }
	// svr : { ret : 0 , targetUID ：2345 }
	// taregetUID : 请求加好友，对方的ID， notice 是请求是捎带的一句话
	// ret : 0 发出成功 ; 1 找不到目标玩家 , 2 对方好友已经满了， 3 自己的好友已经满了, 4 对方已经是自己的好友
	
	MSG_REQ_ADD_FRIEND_RESULT, // 请求添加好友的结果
	// svr : { targetUID ：2345 ， name : "bigGirl", isAgree : 0 }
	// targetUID :  回复者的UID ， name 回复者的name 。 isAgree ： 1 是同意， 0 是不同意


	MSG_RECIEVED_NEW_FRIEND_REQ,  //  收到加好友的请求
	// svr : { reqUID : 2345 , reqName : "name" , notice : "i want to make friend with you " }
	// reqUID 请求者的UID， reqName 请求者的名字，notice 请求者捎带的一句话
	MSG_REPLY_NEW_FRIEND_REQ, // 回复加好友的请求
	// client : { targetUID : 2345 , isAgree : 0 }
	// target ： 回复目标的UID ，isAgree 是否同意 1 是同意，0 是不同意；
	MSG_DELETE_FRIEND , // 删除好友
	// client { targetUID : 2345 } ;
	// svr : { ret : 0 , targetUID : 2345 } ;
	// ret : 0 成功， 1 对方不是你的好友。  targetUID : 要删除的好友UID ；

	MSG_REQ_FRIEND_LIST, // 请求好友列表；
	// client : { startIdx : 0 , cnt : 2345 }
	// svr : { totalCnt : 1 , friendUIDs : [2345 ,235 ,2346 ,2345 ] }
	// startIdx :  请求好友列表的开始所有， cnt ： 本次请求多少个好友。 
	// totalCnt : 总好友数量

	// mail module 
	MSG_REQUEST_NEW_MAIL_LIST, // 请求新邮件列表
	// client : null ;
	// svr : { mails : [ { type : eMailType ,recvTime : 2345, content : 【】 }, { type : 1 ,recvTime : 2345, content :【】 }, ... ] } ;
	// mails : 邮件数组； 数组内是邮件的json 对象，包含type 类型。 content 邮件内容； 注意邮件内容太content 有可能是json对象，有可能不是，具体要根据type来决定，content 里的信息
	// 请参考 eMailType 相应枚举里的解释 ；recvTime : 邮件接收到的时间

	MSG_RECIEVED_NEW_MAIL,  // 新邮件通知，当有新邮件到达的时候，会收到这个消息；
	// svr : { newMailCnt : 12 }  
	// newMailCnt : 收到新邮件的条数

	MSG_PLAYER_WEAR_CLOTHE, // 玩家穿上衣服；
	// client : { itemID : 23456 }
	// svr : { ret : 0 ,itemID : 23456 }
	// itemID 就是所穿衣服的 物品ID ；
	// ret : 0 成功， 1 玩家没有这个衣服，2 该衣服已经过期, 3 不存在改物品

	MSG_REQ_GAME_DATA, // 请求游戏的统计数据，非比赛场
	// client : { gameType : eRoomType  }
	// svr : { ret : 0 , gameType : eRoomType , roundPlayed : 2345 ,maxFanShu : 345 , maxFanXing : eFanxingType  }
	// ret : 0 成功返回，1 参数错误
	// gameType 场次类型，取值参考eRoomType，roundPlayed， 总共玩的局数， maxFanShu 最大番数， maxFanXing 最大番数牌型 ， 取值参考eFanxingType；
	
	MSG_REQ_VIP_ROOM_BILL_INFO,  // 请求vip 房间的账单, 此消息发往游戏服务器；
	// client : { billID : 2345 }
	// svr : { ret : 0 , billID : 234, billTime : 23453, roomID : 235, roomType : eRoomType , creatorUID : 345 , circle： 8 ，initCoin : 2345 , detail : [ { uid : 2345 , curCoin : 234 }, ....]  } 
	// ret : 0 成功，1 账单id不存在，billID, 账单ID， billTime ： 账单产生的时间, roomID : 房间ID ， roomType 房间类型eRoomType， creatorUID 创建者的ID，circle 房间的圈数，initCoin ： 初始金币，detail : 每个人的输赢详情 json数组
	// uid : 玩家的uid，curCoin 结束时剩余钱；

	MSG_REQ_VIP_ROOM_BILL_IDS, // 请求最近10条 vip房间 账单的ID ， 次消息发送给 舒服服务器 data server；
	// client : null
	// svr : { billIDs : [234,234,2345 ] }
	// billIDs 账单id的数组，通过id 可以通过 MSG_REQ_VIP_ROOM_BILL_INFO 这个消息，获取账单详情

	MSG_ROOM_PLAYER_COIN_UPDATE, // 房间内玩家的金币更新，比如充值了，领救济金啊。。等等之类的。
	// svr : { idx : 23 , coin : 2345 }
	// coin 为玩家最终的金币数量

	MSG_ROOM_INFORM_SUPPLY_COIN, // 通知玩家补充金币，
	// svr: { players : [2,3,1] } 
	// players ： 需要补充金币的玩家索引，数组

	MSG_PLAYER_DECIDE_LOSE, // 玩家认输，放弃补充金币, 这个消息发给游戏服务器
	// client : { dstRoomID : 356 } ,
	
	MSG_ROOM_PLAYER_SUPPLY_COIN_RESULT, // 通知玩家补充金币的结果，房间内所有的玩家都可以收到这条消息
	// svr : { playerIdx : 2 , result : 0 }
	// playerIdx 操作的玩家索引
	// result : 0 补充金币成功，1 放弃补充金币，认输;
};
