#pragma once
#include "IConfigFile.h"
#include "CommonDefine.h"
#include <string>
#include <map>
#include "Singleton.h"
class CMakeCardConfig
	:public IConfigFile
	, public CSingleton<CMakeCardConfig>
{
public:
	CMakeCardConfig(){ nMakeCardRate = 0; nRobotCardMakeRate = 0;  memset(vCardRate, 0, sizeof(vCardRate)); memset(vCardRateRobot, 0, sizeof(vCardRateRobot)); }
	bool OnPaser(CReaderRow& refReaderRow)override;
	uint8_t getMakeCardRate( bool isRobot );
	uint8_t get7CardRate( bool bIsRobot );
	uint8_t get8CardRate( bool bIsRobot );
	uint8_t get9CardRate( bool bIsRobot );
protected:
	uint8_t nMakeCardRate;
	uint8_t nRobotCardMakeRate;
	uint8_t vCardRate[3]; // 7 , 8 , 9 
	uint8_t vCardRateRobot[3]; // 7 , 8 , 9 
};