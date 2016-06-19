#pragma once
#include "MessageIdentifer.h"
enum eMJGameType
{
	eMJ_None,
	eMJ_BloodRiver = eMJ_None,
	eMJ_BloodTheEnd,
	eMJ_COMMON,
	eMJ_Max,
};

enum  eMJCardType
{
	eCT_None,
	eCT_Wan,
	eCT_Tong,
	eCT_Tiao,
	eCT_Feng,
	eCT_ZFB,
	eCT_Max,
};

enum  ePosType
{
	ePos_Self,
	ePos_Last,
	ePos_Next ,
	ePos_Oppsite,
	ePos_Any,
	ePos_Other,
	ePos_Already,
	ePos_Max,
};

