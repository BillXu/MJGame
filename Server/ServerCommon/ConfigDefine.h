#pragma once
//#define GAME_panda
//#define GAME_paiYouQuan
#define GAME_365 
//#define GAME_IN_REVIEW 

#if defined( GAME_panda)

#define Gotype_DevID "73d1ec14-84c9-46de-b6d7-fd9abbb4eb6d"
#define Gotype_ProdID "73d1ec14-84c9-46de-b6d7-fd9abbb4eb6d"
#define Wechat_notifyUrl ""
#define Wechat_MchID ""
#define Wechat_appID ""
#define Wechat_MchKey ""
#define Game_DB_Name "pandaniuniu"
#define Log_DB_Name "pandaniuniulog"

#elif defined(GAME_paiYouQuan)

#define Gotype_DevID "e87f31bb-e86c-4d87-a3f3-57b3da76b3d6"
#define Gotype_ProdID "abffee4b-deea-4e96-ac8d-b9d58f246c3f" 
#define Wechat_notifyUrl "http://abc.paiyouquan.com:5007/vxpay.php"
#define Wechat_MchID "1308480601"
#define Wechat_appID "wx66f2837c43330a7b"
#define Wechat_MchKey "E97ED2537D229C0E967042D2E7F1C936"
#define Game_DB_Name "taxpokerdb"
#define Log_DB_Name "taxpokerlogdb"

#elif defined(GAME_365)

#define Gotype_DevID "8276e6d3-5e96-44e2-9243-97a40f019bb3"
#define Gotype_ProdID "8276e6d3-5e96-44e2-9243-97a40f019bb3"
//#define Wechat_notifyUrl "http://abc.paiyouquan.com:5016/vxpay.php"  // online editon 
#define Wechat_notifyUrl "http://118.178.121.165:6016/vxpay.php"  // 7 z online used
//#define Wechat_MchID "1385365702"
//#define Wechat_appID "wxae3a38cb9960bc84"
//#define Wechat_MchKey "NUN5DKS5MJW4UBVJIL1G2XUQ66LU2ENU"
#define Wechat_MchID "1406075202"
#define Wechat_appID "wxf358d4da159c9416"
#define Wechat_MchKey "2ormEv9YGuDoiadRwqPvWwUl2k4VVCH7"
#define Game_DB_Name "niuniu365"
#define Log_DB_Name "niuniulog"

#else

#define Gotype_DevID ""
#define Gotype_ProdID ""
#define Wechat_notifyUrl "http://abc.paiyouquan.com:5006/vxpay.php"
#define Wechat_MchID ""
#define Wechat_appID ""
#define Wechat_MchKey ""
#define Game_DB_Name ""
#define Log_DB_Name ""

#endif 