#pragma once 
//#define  C_SHARP 
#if (C_SHARP)  
public
#endif
enum eMJActType
{
	eMJAct_Mo, // ����
	eMJAct_Peng,  // ����
	eMJAct_MingGang,  // ����
	eMJAct_AnGang, // ����
	eMJAct_BuGang,  // ���� 
	eMJAct_BuGang_Pre, // ���ܵ�һ�׶�
	eMJAct_BuGang_Done, //  ���ܵڶ��׶Σ�ִ�и���
	eMJAct_Hu,  //  ����
	eMJAct_Chi, // ��
	eMJAct_Chu, // ����
	eMJAct_Pass, //  �� 
	eMJAct_Max,
};

#if (C_SHARP)  
public
#endif
enum eRoomType
{
	eRoom_None,
	eRoom_MJ = eRoom_None,
	eRoom_MJ_Blood_River = eRoom_MJ, // Ѫ���ɺ�
	eRoom_NiuNiu = eRoom_MJ,
	eRoom_MJ_Blood_End, // Ѫս����
	eRoom_TexasPoker,
	eRoom_Golden,
	eRoom_Max ,
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
	eTime_DoPlayerMoPai = 1 ,  //  �������ʱ��
	eTime_DoPlayerActChuPai = 2,  // ��ҳ��Ƶ�ʱ��
	eTime_DoPlayerAct_Gang = 2, // ��Ҹ���ʱ��
	eTime_DoPlayerAct_Hu = 3,  // ��Һ��Ƶ�ʱ��
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
	eRoomState_WaitPlayerAct,  // �ȴ���Ҳ���
	eRoomState_DoPlayerAct,  // ��Ҳ���
	eRoomState_WaitOtherPlayerAct,  // �ȴ���Ҳ��������˳�����
	eRoomState_DoOtherPlayerAct,  // ������Ҳ����ˡ�
	eRoomState_WaitPlayerRecharge,  //  �ȴ���ҳ�ֵ
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
	// svr : { name: "nickName",sex : 0,coin : 235 , diamond: 500,uid : 2345, sessionID : 2345, vipRoomCard : 23 }
	// name �� ���֣�sex �� �Ա�0 ���У�1 ��Ů���� diamond ����ʯ�� coin �� ��ң�

	// modify name and sigure
	MSG_PLAYER_MODIFY_NAME,
	MSG_PLAYER_MODIFY_SIGURE,
	MSG_PLAYER_MODIFY_PHOTO,
	MSG_PLAYER_UPDATE_MONEY,  // USE WHEN OTHER MAIL A GITF  ;
	MSG_PLAYER_MODIFY_SEX,
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
	// svr : { players : [ {idx : 0 , coin : 2345 } ,{idx : 1 , coin : 2345 } ,{idx : 2 , coin : 2345 },{idx : 3 , coin : 2345 } ]  } 
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
	// client : { circle : 2 , baseBet : 1, initCoin : 2345 , roomType : 0, seatCnt : 4  }
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
};
