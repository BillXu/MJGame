#pragma once
#include "NativeTypes.h"
#include <vector>
#include <memory>
class CMJHuPaiInfo
{
public:
	struct stShunZi
	{
		uint8_t vCards[3];
		void clear(){ memset(vCards,0,sizeof(vCards));}
	};

	typedef std::shared_ptr<stShunZi> shunzi_ptr ;
	typedef std::vector<uint8_t> VEC_CARD ;
	typedef std::vector<shunzi_ptr> VEC_SHUN;
public:
	CMJHuPaiInfo(){ m_vReuseShunZiPtr.clear() ; clear() ;}
	~CMJHuPaiInfo(){ clear();}
	bool parseHuPaiInfo( const std::vector<uint8_t>& vHoldCard );
	bool is7Pairs();
	bool isHu(){ return is7Pairs() || m_nJianPai != 0 ; }
protected:
	void clear();
	bool getAnZiFromCards( VEC_CARD& vCards, VEC_CARD& vOutAnZi );
	bool getShunZiFromCards( VEC_CARD& vCards, VEC_SHUN& vOutAnZi );
	bool getFengShunZiFromCards( VEC_CARD& vCards, VEC_SHUN& vOutAnZi );
	bool check7Pair( VEC_CARD vCards );

	shunzi_ptr getReuseShunziPtr();
	void recycleShunziPtr(shunzi_ptr ptr );
public:
	uint8_t m_nJianPai;
	VEC_SHUN m_vAllShunzi ;
	VEC_CARD m_vAnKeZi ; // an ke zi ;

	VEC_CARD m_v7Pairs ;  
protected:
	VEC_SHUN m_vReuseShunZiPtr ;
};