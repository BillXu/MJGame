#include "makeCardConfig.h"
bool CMakeCardConfig::OnPaser(CReaderRow& refReaderRow)
{
	uint8_t nIsRobot = refReaderRow["isRobot"]->IntValue();
	if (nIsRobot)
	{
		nRobotCardMakeRate = refReaderRow["makeCardRate"]->IntValue();
		vCardRateRobot[0] = refReaderRow["card7Rate"]->IntValue();
		vCardRateRobot[1] = refReaderRow["card8Rate"]->IntValue();
		vCardRateRobot[2] = refReaderRow["card9Rate"]->IntValue();
	}
	else
	{
		nMakeCardRate = refReaderRow["makeCardRate"]->IntValue();
		vCardRate[0] = refReaderRow["card7Rate"]->IntValue();
		vCardRate[1] = refReaderRow["card8Rate"]->IntValue();
		vCardRate[2] = refReaderRow["card9Rate"]->IntValue();
	}

	return true;
}

uint8_t CMakeCardConfig::getMakeCardRate(bool isRobot )
{
	if (isRobot)
	{
		return nRobotCardMakeRate;
	}
	return nMakeCardRate;
}

uint8_t CMakeCardConfig::get7CardRate( bool isRobot )
{
	uint8_t nIdx = 0;
	if (isRobot)
	{
		return vCardRateRobot[nIdx];
	}

	return vCardRate[nIdx];
}

uint8_t CMakeCardConfig::get8CardRate(bool isRobot)
{
	uint8_t nIdx = 1;
	if (isRobot)
	{
		return vCardRateRobot[nIdx];
	}

	return vCardRate[nIdx];
}

uint8_t CMakeCardConfig::get9CardRate(bool isRobot)
{
	uint8_t nIdx = 2;
	if (isRobot)
	{
		return vCardRateRobot[nIdx];
	}

	return vCardRate[nIdx];
}