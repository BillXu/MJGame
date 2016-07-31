#pragma once 
//#define  C_SHARP 
#if (C_SHARP)  
public
#endif
enum eMJActType
{
	eMJAct_None,
	eMJAct_Mo = eMJAct_None, // ����
	eMJAct_Chi, // ��
	eMJAct_Peng,  // ����
	eMJAct_MingGang,  // ����
	eMJAct_AnGang, // ����
	eMJAct_BuGang,  // ���� 
	eMJAct_BuGang_Pre, // ���ܵ�һ�׶�
	eMJAct_BuGang_Declare = eMJAct_BuGang_Pre, // ����Ҫ���� 
	eMJAct_BuGang_Done, //  ���ܵڶ��׶Σ�ִ�и���
	eMJAct_Hu,  //  ����
	eMJAct_Chu, // ����
	eMJAct_Pass, //  �� 
	eMJAct_Max,
};

#if (C_SHARP)  
public
#endif
enum eFanxingType 
{
	eFanxing_PingHu, // ƽ��

	eFanxing_DuiDuiHu, //  �ԶԺ�

	eFanxing_QingYiSe, // ��һɫ
	eFanxing_DaiYaoJiu, //  ���۾�
	eFanxing_QiDui, //  �߶�
	eFanxing_JinGouDiao, //  �𹳵�

	eFanxing_QingDuiDuiHu, // ��ԶԺ�

	eFanxing_QingDui, //  ���߶�
	eFanxing_LongQiDui, //  ���߶�
	eFanxing_QingDaiYaoJiu, //  �� ���۾�
	eFanxing_JiangJinGouDiao, // ���𹳵�
	eFanxing_QingJinGouDiao, // ��𹳵�

	eFanxing_QingLongQiDui, //  �����߶�
	eFanxing_ShiBaLuoHan, //  ʮ���޺�
	eFanxing_Max, // û�к�
};

#if (C_SHARP)  
public
#endif
enum eRoomType
{
	eRoom_None,
	eRoom_MJ = eRoom_None,
	eRoom_MJ_Blood_End = eRoom_None, // Ѫս���� 
	eRoom_MJ_Blood_River,// Ѫ���ɺ�
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
	eTime_ExeGameStart = 5,			// ִ����Ϸ��ʼ ��ʱ��
	eTime_WaitChoseExchangeCard = 5, //  �ȴ����ѡ���Ƶ�ʱ��
	eTime_DoExchangeCard = 3, //   ִ�л��Ƶ�ʱ��
	eTime_WaitDecideQue = 10, // �ȴ���Ҷ�ȱ
	eTime_DoDecideQue = 2, // ��ȱʱ��
	eTime_WaitPlayerAct = 80000,  // �ȴ���Ҳ�����ʱ��
	eTime_WaitPlayerChoseAct = eTime_WaitPlayerAct,
	eTime_DoPlayerMoPai = 1 ,  //  �������ʱ��
	eTime_DoPlayerActChuPai = 2,  // ��ҳ��Ƶ�ʱ��
	eTime_DoPlayerAct_Gang = 2, // ��Ҹ���ʱ��
	eTime_DoPlayerAct_Hu = 3,  // ��Һ��Ƶ�ʱ��
	eTime_DoPlayerAct_Peng = 2, // �������ʱ��
	eTime_GameOver = 1, // ��Ϸ����״̬����ʱ��
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
enum eRoomState  // ��ҵ�״̬
{
	eRoomState_None,
	eRoomState_Opening,
	eRoomState_WillDead,
	eRoomState_Dead,
	eRoomState_WillClose,
	eRoomState_Close,
	eRoomSate_WaitReady,  // �ȴ����׼��
	eRoomState_StartGame, // ��ʼ��Ϸ

	eRoomState_WaitExchangeCards, //  �ȴ���һ���
	eRoomState_DoExchangeCards , // ��һ���
	eRoomState_WaitDecideQue,  // �ȴ���Ҷ�ȱ
	eRoomState_DoDecideQue,  //  ��Ҷ�ȱ
	eRoomState_DoFetchCard, // �������
	eRoomState_WaitPlayerAct,  // �ȴ���Ҳ��� { idx : 0 , exeAct : eMJActType , isWaitChoseAct : 0 , actCard : 23  }
	eRoomState_DoPlayerAct,  // ��Ҳ���
	eRoomState_WaitOtherPlayerAct,  // �ȴ���Ҳ��������˳����� { invokerIdx : 0 , card : 0 ,cardFrom : eMJActType , arrNeedIdxs : [2,0,1] } 
	eRoomState_DoOtherPlayerAct,  // ������Ҳ����ˡ�
	eRoomState_WaitSupplyCoin , // �ȴ���Ҳ�����
	eRoomState_WaitPlayerRecharge = eRoomState_WaitSupplyCoin,  //  �ȴ���ҳ�ֵ
	eRoomState_GameEnd, // ��Ϸ����
	eRoomState_Max,
};

// player State 
#if (C_SHARP)  
public
#endif
enum eRoomPeerState  // �ƾ�����ҵ�״̬
{
	eRoomPeer_None,
	// peer state for taxas poker peer
	eRoomPeer_SitDown = 1,
	eRoomPeer_StandUp = 1 << 1,
	eRoomPeer_Ready =  (1<<12)|eRoomPeer_SitDown , // �Ѿ�׼����״̬
	eRoomPeer_StayThisRound = ((1 << 2)|eRoomPeer_SitDown)| eRoomPeer_Ready ,
	eRoomPeer_WaitCaculate = ((1 << 7)|eRoomPeer_StayThisRound ),
	eRoomPeer_AllIn = ((1 << 3)|eRoomPeer_WaitCaculate) ,
	eRoomPeer_GiveUp = ((1 << 4)|eRoomPeer_StayThisRound),
	eRoomPeer_CanAct = ((1 << 5)|eRoomPeer_WaitCaculate), // �ɲ���״̬�������ƾ���
	eRoomPeer_WaitNextGame = ((1 << 6)|eRoomPeer_SitDown ),
	eRoomPeer_AlreadyHu = ((1 << 8)|eRoomPeer_CanAct ),  //  �Ѿ����Ƶ�״̬
	eRoomPeer_DecideLose = eRoomPeer_GiveUp ,  // ����״̬
	eRoomPeer_Max,
};

#if (C_SHARP)  
public
#endif
enum eSex
{
	eSex_Unknown,  // δ֪
	eSex_Male,  // ��
	eSex_Female, // Ů
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

	// ����Ϊ�ͻ�����Ҫʶ����ʼ�����
	eMail_RealMail_Begin, // ��ʼ��ʶ ;
	eMail_PlainText,  // ����Ϊ��ͨ�ַ�����ע�⣺ ����json���� ;
	eMail_InvitePrize, // { targetUID : 2345 , addCoin : 300 } // you invite player to join game ,and give prize to you 
	eMail_WinMatch, // { gameType:234,roomName:234,rankIdx:2,addCoin:345,cup : 2 , diamomd : 34 }
	eMail_VIP_INVITE, // { inviteUID : 2345 , roomID : 2345 } vip �������룬 inviteUID �����ߵ�UID�� roomID �����id ������ͨ��������뷿��
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
	MSG_PLAYER_REGISTER,     //���ע��
	// client : {acc: "account" , pwd : "password",regType : 1 , regChannel : 0  } 
	// svr : { ret: 0  , acc: "account" , pwd : "password",regType : 1 , UID : 2345 }
	// ret : 0 success , 1 account already exist
	// regType : ע�����͡�0 �ο�ע�� , 1 ����ע�� , 2 ���˺� ;
	// regChannel : 0 appstore  // ע������

	MSG_PLAYER_LOGIN,  // ��ҵ�½ ; 
	// client : {acc: "account" , pwd : "password" }
	// svr : { ret : 0 , regType : 2 }
	// ret : 0 ,��¼�ɹ� , 1 �˺Ŵ��� , 2 ������� , 3 ״̬�����˺ű��⣩ 

	MSG_PLAYER_BIND_ACCOUNT, //  ���˺ţ��ο���Ҫ���˺� ; 
	// client : {acc: "account" , pwd : "password", UID : 2330 }
	// svr :	{ ret: 0 }
	// ret : 0 �󶨳ɹ� , 1 �˺��Ѿ����� , 2 uid ������, 3 δ֪����  ;

	MSG_MODIFY_PASSWORD,  
	// client : { oldPwd: "oldPassword", pwd : "password", UID : 2000 }
	// svr : { ret : 0 }
	//  oldPwd�������룬 pwd�������룬uid����ҵ�ΨһID��
	// ret :  0 �ɹ� , 1 uid ������ , 2 ��������� 

	MSG_PLAYER_OTHER_LOGIN,  // �˺��������豸��¼����ǰ�豸��Ҫ�˳�

	MSG_PLAYER_BASE_DATA, // ��ҵĻ�����Ϣ ,
	// svr : { name: "nickName",sex : eSex,coin : 235 , diamond: 500,uid : 2345, sessionID : 2345, vipRoomCard : 23, clothe : [235,235,234] }
	// name �� ���֣�sex �� ����ö��eSex�� diamond ����ʯ�� coin �� ��ң� clothe : ��Ҵ������ϵ��·�������Ʒ

	// modify name and sigure
	MSG_PLAYER_MODIFY_NAME, // ����޸��ǳ�
	// client : { newName : helloWorld }
	// svr : { ret : 0 , newName : helloWorld}
	// newName : ��������õ�����
	// ret : 0 �ɹ���1 ���ֳ���̫��

	MSG_PLAYER_MODIFY_SIGURE,
	MSG_PLAYER_MODIFY_PHOTO,
	MSG_PLAYER_UPDATE_MONEY,  // USE WHEN OTHER MAIL A GITF  ;
	
	MSG_PLAYER_MODIFY_SEX, // �޸���ҵ��Ա�
	// client : { newSex : eSex }
	// svr : { newSex : eSex }
	// newSex ,�����õ��Ա� ���ο�eSex ö��ֵ

	MSG_RESET_PASSWORD,
	MSG_REQUEST_PLAYER_INFO,
	
	MSG_DO_RECONNECT, // ������������Ϣ
	// client : { nSessionID : 233 }
	// svr : { ret : 0 }
	// ret : 0 ��ʾ�ɹ���1 ��ʾʧ�ܣ�seesion id ���ǻỰID ��¼�ɹ��Ժ�����������

	// mj room msg �齫������Ϣ���ͻ��˷���svr����Ϣ��������� dstRoomID �� key 
	MSG_REQ_ENTER_ROOM = 10115,
	// client : { type : 0 �� targetID : 23 }
	// svr : { ret : 0  }
	// ret :  0 success , 1 �Ѿ��ڷ����� , 2 ����Ҫ���οͲ��ܽ��� ; 3 ��Ҳ��� ; 4 ;  ���̫�� ; 5 �Ҳ���ָ��id ��fangjian ,  6 �������ʹ��� 8 �����Ѿ����ˡ�
	// type = 0 , �������ƥ�䷿�䣬targetID ��ֵ��Ӧ����configID��ֵ�� type = 1 �� ��ʱ���ʾ����ָ����ĳ�����䣬targetID ��ʱ��ʾ���� RoomID ��

	MSG_ROOM_INFO,  // ����Ļ�����Ϣ
	// svr : { roomID �� 23 , configID : 23 , roomState :  23 , players : [ {idx : 0 , uid : 233, coin : 2345 , state : 34 }, {idx : 0 , uid : 233, coin : 2345, state : 34 },{idx : 0 , uid : 233, coin : 2345 , state : 34} , ... ] }
	// roomState  , ����״̬

	MSG_ROOM_PLAYER_ENTER, // ��������ҽ��뷿��
	// svr : {idx : 0 , uid : 233, coin : 2345,state : 34 }

	MSG_PLAYER_SET_READY,   // ���׼��
	// client : { dstRoomID : 2345 } ;

	MSG_ROOM_PLAYER_READY,  // �������׼��
	// svr : { idx : 2 }

	MSG_ROOM_START_GAME,  // ��ʼ��Ϸ����Ϣ
	// svr : { banker: 2 , dice : 3 , peerCards : [ { cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] },{cards : [1,3,4,5,64,23,64] },{ cards : [1,3,4,5,64,23,64] } ] }
	// banker ׯ�ҵ����� , dice : ���ӵĵ����� cards �� ��ҵ�����

	MSG_ROOM_WAIT_CHOSE_EXCHANG,  //  ֪ͨ���ѡ�������� ���н���
	// svr : null 

	MSG_PLAYER_CHOSED_EXCHANGE,   // ���ѡ��Ҫ��������
	// client : { dstRoomID : 2345 ,cards: [ 3, 1,2] }
	// svr : { ret : 0 }
	// ret : 0 �ɹ� , 1 ��ѡ����������д���, 2 ѡ����� �������� 3 . ��û�вμ��ƾ� , 4 ���Ѿ�ѡ���ˣ���Ҫѡ������;

	MSG_ROOM_FINISH_EXCHANGE,  //  ����������ѡ��
	// svr : { mode : 0 , result : [ { idx = 0 , cards : [ 2, 4 ,5]}, { idx = 1 , cards : [ 2, 4 ,5]},{ idx = 2 , cards : [ 2, 4 ,5]},{ idx = 3 , cards : [ 2, 4 ,5]}  ] }
	// mode : ����ģʽ�� 0 ˳ʱ�뻻 , 1 ��ʱ�뻻, 2 �Լһ� 

	MSG_ROOM_WAIT_DECIDE_QUE, // ����ȴ���Ҷ�ȱ��״̬
	// svr : null ;

	MSG_PLAYER_DECIDE_QUE,  // ��Ҷ�ȱ
	// client : { dstRoomID : 2345 , type : 2 }
	// type: ��ȱ�����ͣ�1,�� 2, Ͳ 3, ��


	MSG_ROOM_FINISH_DECIDE_QUE,  // ������ ��ȱ
	// svr : { ret : [ {type : 0, idx : 2 }, {type : 0, idx : 1 } , {type : 0, idx : 2 }, {type : 0, idx : 3 }] }
	// �����Ӧ��� ����ȱ�����͡�

	MSG_PLAYER_WAIT_ACT_ABOUT_OTHER_CARD,  // ���˳���һ���ƣ��ȴ���Ҫ�����Ƶ���� ���������� �����ܣ���
	// svr : { cardNum : 32 , acts : [type0, type 1 , ..] }  ;
	// �����Ϣ����㲥��ֻ�ᷢ����Ҫ�����Ƶ���ң�cardNum ����Ҫ���ƣ�type ���Ͳ��� eMJActType

	MSG_PLAYER_WAIT_ACT_AFTER_RECEIVED_CARD,  // �Լ����һ���ƣ��ܻ�������Ȼ����Խ��еĲ��� �ܣ���
	// svr : { acts : [ {act :eMJActType , cardNum : 23 } , {act :eMJActType , cardNum : 56 }, ... ]  }  ;
	// �����Ϣ����㲥��ֻ�ᷢ����ǰ��������ң�acts�� �ɲ��������飬 ��Ϊ���һ���ƣ��Ժ���Խ��еĲ����ܶࡣcardNum �������Ӧ���ƣ�type ���Ͳ��� eMJActType

	MSG_PLAYER_ACT, // ��Ҳ���
	// client : { dstRoomID : 2345 ,actType : 0 , card : 23}
	// actType : eMJActType   �������ͣ�����ö��ֵ
	// svr : { ret : 0 }
	// ret : 0 �����ɹ� , 1 û���ֵ������ , 2 ����ִ��ָ���Ĳ���������������, 3 �������� , 4 ״̬���� ;

	MSG_ROOM_ACT,  // �����������ִ����һ������
	// svr : { idx : 0 , actType : 234, card : 23, gangCard : 12 }
	// idx :  ִ�в������Ǹ���ҵ������� actType : ִ�в��������ͣ�����ö��ֵeMJActType �� card�� �����漰������  gangCard: ���ƺ� ��õ���;

	MSG_ROOM_WAIT_RECHARGE, // �ȴ�һЩ��ҳ�ֵ
	// svr: { players: [0,1,3]}    
	// players : ��Ҫ��ֵ��������� ���飬�����ж����ҡ�
	
	MSG_ROOM_GAME_OVER, // ��Ϸ����
	// svr : { players : [ {idx : 0 , coin : 2345 ,huType : eFanxingType } ,{idx : 1 , coin : 2345 ,huType : eFanxingType } ,{idx : 2 , coin : 2345,huType : eFanxingType },{idx : 3 , coin : 2345,huType : eFanxingType } ]  } 
	// eFanxingType ����ö��ֵ
	// players: ������ÿ��������յ�Ǯ����

	MSG_PLAYER_LEAVE_ROOM, // ����뿪����
	// client : {dstRoomID : 23 } ;
	// svr : { ret : 0 }
	// ret : 0 ��ʾ�ɹ��� 1 ����Ŵ���,���ڸ÷����

	MSG_ROOM_PLAYER_LEAVE, // ������뿪����;
	// svr : { idx : 2 }
	
	MSG_PLAYER_REQ_ROOM_INFO,
	// client : {dstRoomID : 23 } ;
	// ����Ϣ���󷿼���ϸ��Ϣ�������ָ�������ֳ���һ�����ڶ��������ɹ���

	MSG_ROOM_PLAYER_CARD_INFO,
	// svr : { bankerIdx : 2, leftCardCnt : 32 ,playersCard: [ { idx : 2,queType: 2, anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34], chuPai: [2,34,4] },{ anPai : [2,3,4,34], mingPai : [ 23,67,32] , huPai : [1,34] }, .... ] }
	// leftCardCnt : ʣ���Ƶ����������½����Ѿ�����ķ��䣬���߶����������ͻ��յ������Ϣ�� anPai �����ƣ�û��չʾ�����ģ�mingPai �����Ѿ�չʾ�������ƣ������ܣ���huPai �� �Ѿ����˵��ơ� queType : 1,�� 2, Ͳ 3, ��

	MSG_ROOM_REQ_TOTAL_INFO,
	// client : {dstRoomID : 23 } ;
	// ���������ɹ�,�Ժ����󷿼���Ϣ���ָ������ֳ���

	MSG_TELL_ROBOT_TYPE = 10338, // ������������ͻ����ǻ����˵����
	MSG_TELL_ROBOT_IDLE, // ֪ͨ����������ǰ�����˿��У�Ҳ����û���ڷ����ڡ�
	MSG_SVR_INFORM_ROBOT_LEAVE, // ������֪ͨ�������˳���ǰ���䡣
	MSG_SVR_INFOR_ROBOT_ENTER, // ������֪ͨ�����˽��뷿�䣻
	// svr : { dstRoomID : 0 }
	// dstRoomID , ������֪ͨ��������Ҫ����ķ���ID��

	// vip ������Ϣ
	MSG_CREATE_VIP_ROOM,  // ����vip���� 
	// client : { circle : 2 , baseBet : 1, initCoin : 2345 , roomType : eRoomType, seatCnt : 4  }
	// svr : { ret : 0 , roomID : 2345 }
	// circle ��ʾ���������Ȧ����baseBet ������ע ��initCoin ÿ���˵ĳ�ʼ��ң� roomType �������ͣ� 0 ��Ѫ����1 ��Ѫս�� ret �� 0 ��ʾ�ɹ���1 ��ʾ���������� 2 ����ʾ���ܴ������෿��, seatCnt : ��λ������

	MSG_VIP_ROOM_INFO_EXT, // VIP ����Ķ�����Ϣ��
	// svr : { leftCircle : 2 , baseBet : 1 , creatorUID : 2345 , initCoin : 2345 }
	// letCircle : ʣ���Ȧ���� baseBet ������ע ��creatorUID �����ߵ�ID , initCoin ÿ���˵ĳ�ʼ���

	MSG_APPLY_DISMISS_VIP_ROOM, // �����ɢvip ����
	// client : { dstRoomID : 234 } 

	MSG_ROOM_APPLY_DISMISS_VIP_ROOM , //���������������ɢvip ����
	// svr : { applyerIdx : 2 }
	// applyerIdx : �����ߵ�idx 

	MSG_REPLY_DISSMISS_VIP_ROOM_APPLY,  // �������ɢ������
	// client { dstRoomID : 23 , reply : 0 }
	// reply �� 0 ��ʾͬ�⣬ 1 ��ʾ�ܾ���
	
	MSG_VIP_ROOM_GAME_OVER,  // vip �������
	// svr : { ret : 0 , initCoin : 235 , bills : [ { uid : 2345 , curCoin : 234 }, ....]  }
	// ret , 0 ���������� 1 ���䱻��ɢ�� initCoin ����ĳ�ʼ��ң�bills����һ������ ���ž���ÿ����ҵ������curCoin ��ʾ�������ʣ����, uid ��ҵ�Ψһid 

	// shop module
	MSG_SHOP_BUY_ITEM = 10548,  // �����̵������Ʒ
	// client : { shopItemID : 2345 }
	// svr : { ret : 0 , shopItemID : 2345 }
	// shopItemID �� ��Ʒ������ID��
	// ret : 0 ����ɹ��� 1 ��Ʒ�����ڣ�2 ���Ҳ���, 3 ����ID �����ڣ�

	MSG_REQ_MY_BAG,  // ���󱳰�����
	// client : null 
	// svr : { items : [ {itemID : 234 , cnt : 23 , buyTime : 23345, leftTime : 2352 } , {itemID : 2 , cnt : 23 , buyTime : 23345, leftTime : 2352},  ...... ] }
	// items : ������Ʒ����
	//  itemID ���ߵ�ID , cnt : ���� �� buyTime : �����ʱ��, leftTime ʣ��ʱ�� ��λ����

	MSG_START_ROLL_PLATE,// תת��
	// client : null 
	// svr : { ret : 0 , plateID : 234 , isFree : 0  }
	// ret : 0 �ɹ� , 1 ���Ҳ��� , 2 ϵͳ����
	// plateID :  ���е� ����ID�� ��������ID �������Ʒ
	// isFree : ����ת�� �Ƿ���ѣ� 0 �Ƿ�1 �� �ǡ�

	MSG_PLAYER_DO_EXCHANGE, // ��Ҷһ�
	// client : { configID : 235 , info :{ phone : 12345 , addr : "shanghai changNing chian" } }
	// ret : { ret : 0 , configID : 235 }
	// ret : // 0 �һ��ɹ� , 1 ָ��������id ������ , 2 ��ʯ���Ҳ��㣬���ܶһ� ; 3 �������� ; 
	// configID : �һ����������ID��info �� �����д�Ļ�����Ϣ���ͻ��˿���������չ��ԭ���������ݿ⡣

	MSG_PLAYER_DO_GET_CHARITY, // ��ȡ�ȼý�
	// client : null 
	// svr : { ret : 0 , finalCoin : 23455 , recievedCoin : 234 , leftTimes : 23 }
	// ret : 0 �ɹ��� 1 ��ʾ�ﵽ�������ƣ�finalCoin ��ȡ������ս������ ��recievedCoin ��ȡ���Ľ�ң� leftTimes ʣ�����ȡ����;

	MSG_REQUEST_PLAYER_BRIF_INFO, // ������ҵĻ�����Ϣ��
	// client : { targetUID : 23456 }
	// svr : { ret : 0 , nickName : "lucy" , photoID : 2345 , uid : 2345 , ip : "192.168.1.56" ,sex : eSex, diamond : 2345, phone : 18023043245 }
	// ret : 0 �ɹ���1 �����ڸ����
	// nickName : �ǳ� �� photoID : ͷ��ID�� uid �� ���Id , ip�� ��ҵ�ip��ַ��ֻ�����ߵ���Ҵ��ڣ�����Ϊ�ա� sex �� ����Ա� diamond : ��ҵ���ʯ�� phone �� ��ҵ��ֻ���

	MSG_TELL_SELF_IP, // ֪ͨ����Լ���IP ��ַ
	// svr : { ip : "192.235.133.23" }

	MSG_REQ_UPDATE_COIN, // ��ȡ������µĽ�Ǯ
	// client : null ;
	// svr : { coin : 2345 , diamond : 2345 };

	// friend module 
	MSG_REQ_ADD_FRIEND, // ������Ӻ���
	// client : { targetUID : 2345 , notice : "i want to make friend with you" }
	// svr : { ret : 0 , targetUID ��2345 }
	// taregetUID : ����Ӻ��ѣ��Է���ID�� notice ���������Ӵ���һ�仰
	// ret : 0 �����ɹ� ; 1 �Ҳ���Ŀ����� , 2 �Է������Ѿ����ˣ� 3 �Լ��ĺ����Ѿ�����, 4 �Է��Ѿ����Լ��ĺ���
	
	MSG_REQ_ADD_FRIEND_RESULT, // ������Ӻ��ѵĽ��
	// svr : { targetUID ��2345 �� name : "bigGirl", isAgree : 0 }
	// targetUID :  �ظ��ߵ�UID �� name �ظ��ߵ�name �� isAgree �� 1 ��ͬ�⣬ 0 �ǲ�ͬ��


	MSG_RECIEVED_NEW_FRIEND_REQ,  //  �յ��Ӻ��ѵ�����
	// svr : { reqUID : 2345 , reqName : "name" , notice : "i want to make friend with you " }
	// reqUID �����ߵ�UID�� reqName �����ߵ����֣�notice �������Ӵ���һ�仰
	MSG_REPLY_NEW_FRIEND_REQ, // �ظ��Ӻ��ѵ�����
	// client : { targetUID : 2345 , isAgree : 0 }
	// target �� �ظ�Ŀ���UID ��isAgree �Ƿ�ͬ�� 1 ��ͬ�⣬0 �ǲ�ͬ�⣻
	MSG_DELETE_FRIEND , // ɾ������
	// client { targetUID : 2345 } ;
	// svr : { ret : 0 , targetUID : 2345 } ;
	// ret : 0 �ɹ��� 1 �Է�������ĺ��ѡ�  targetUID : Ҫɾ���ĺ���UID ��

	MSG_REQ_FRIEND_LIST, // ��������б�
	// client : { startIdx : 0 , cnt : 2345 }
	// svr : { totalCnt : 1 , friendUIDs : [2345 ,235 ,2346 ,2345 ] }
	// startIdx :  ��������б�Ŀ�ʼ���У� cnt �� ����������ٸ����ѡ� 
	// totalCnt : �ܺ�������

	// mail module 
	MSG_REQUEST_NEW_MAIL_LIST, // �������ʼ��б�
	// client : null ;
	// svr : { mails : [ { type : eMailType ,recvTime : 2345, content : ���� }, { type : 1 ,recvTime : 2345, content :���� }, ... ] } ;
	// mails : �ʼ����飻 ���������ʼ���json ���󣬰���type ���͡� content �ʼ����ݣ� ע���ʼ�����̫content �п�����json�����п��ܲ��ǣ�����Ҫ����type��������content �����Ϣ
	// ��ο� eMailType ��Ӧö����Ľ��� ��recvTime : �ʼ����յ���ʱ��

	MSG_RECIEVED_NEW_MAIL,  // ���ʼ�֪ͨ���������ʼ������ʱ�򣬻��յ������Ϣ��
	// svr : { newMailCnt : 12 }  
	// newMailCnt : �յ����ʼ�������

	MSG_PLAYER_WEAR_CLOTHE, // ��Ҵ����·���
	// client : { itemID : 23456 }
	// svr : { ret : 0 ,itemID : 23456 }
	// itemID ���������·��� ��ƷID ��
	// ret : 0 �ɹ��� 1 ���û������·���2 ���·��Ѿ�����, 3 �����ڸ���Ʒ

	MSG_REQ_GAME_DATA, // ������Ϸ��ͳ�����ݣ��Ǳ�����
	// client : { gameType : eRoomType  }
	// svr : { ret : 0 , gameType : eRoomType , roundPlayed : 2345 ,maxFanShu : 345 , maxFanXing : eFanxingType  }
	// ret : 0 �ɹ����أ�1 ��������
	// gameType �������ͣ�ȡֵ�ο�eRoomType��roundPlayed�� �ܹ���ľ����� maxFanShu ������� maxFanXing ��������� �� ȡֵ�ο�eFanxingType��
	
	MSG_REQ_VIP_ROOM_BILL_INFO,  // ����vip ������˵�, ����Ϣ������Ϸ��������
	// client : { billID : 2345 }
	// svr : { ret : 0 , billID : 234, billTime : 23453, roomID : 235, roomType : eRoomType , creatorUID : 345 , circle�� 8 ��initCoin : 2345 , detail : [ { uid : 2345 , curCoin : 234 }, ....]  } 
	// ret : 0 �ɹ���1 �˵�id�����ڣ�billID, �˵�ID�� billTime �� �˵�������ʱ��, roomID : ����ID �� roomType ��������eRoomType�� creatorUID �����ߵ�ID��circle �����Ȧ����initCoin �� ��ʼ��ң�detail : ÿ���˵���Ӯ���� json����
	// uid : ��ҵ�uid��curCoin ����ʱʣ��Ǯ��

	MSG_REQ_VIP_ROOM_BILL_IDS, // �������10�� vip���� �˵���ID �� ����Ϣ���͸� ��������� data server��
	// client : null
	// svr : { billIDs : [234,234,2345 ] }
	// billIDs �˵�id�����飬ͨ��id ����ͨ�� MSG_REQ_VIP_ROOM_BILL_INFO �����Ϣ����ȡ�˵�����

	MSG_ROOM_PLAYER_COIN_UPDATE, // ��������ҵĽ�Ҹ��£������ֵ�ˣ���ȼý𰡡����ȵ�֮��ġ�
	// svr : { idx : 23 , coin : 2345 }
	// coin Ϊ������յĽ������

	MSG_ROOM_INFORM_SUPPLY_COIN, // ֪ͨ��Ҳ����ң�
	// svr: { players : [2,3,1] } 
	// players �� ��Ҫ�����ҵ��������������

	MSG_PLAYER_DECIDE_LOSE, // ������䣬����������, �����Ϣ������Ϸ������
	// client : { dstRoomID : 356 } ,
	
	MSG_ROOM_PLAYER_SUPPLY_COIN_RESULT, // ֪ͨ��Ҳ����ҵĽ�������������е���Ҷ������յ�������Ϣ
	// svr : { playerIdx : 2 , result : 0 }
	// playerIdx �������������
	// result : 0 �����ҳɹ���1 ���������ң�����;
};
