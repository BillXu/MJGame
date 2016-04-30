#pragma once 
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
	

};
