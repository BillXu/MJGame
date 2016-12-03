#include "MJHuPaiInfo.h"
#include "MJCard.h"
#include <set>
#include <algorithm>
bool CMJHuPaiInfo::parseHuPaiInfo( const std::vector<uint8_t>& vHoldCard )
{
	clear(); 

	VEC_CARD vCheckCards ;
	vCheckCards.assign(vHoldCard.begin(),vHoldCard.end()) ;
	std::sort(vCheckCards.begin(),vCheckCards.end());
	if ( check7Pair(vCheckCards) )
	{
		return true ;
	}

	// find jiang pai 
	VEC_CARD vJiang ;
	for ( uint8_t nIdx = 0 ; (uint8_t)(nIdx + 1 ) < vCheckCards.size() ; )
	{
		if ( vCheckCards[nIdx] == vCheckCards[nIdx+1] )
		{
			vJiang.push_back(vCheckCards[nIdx] ) ;
			nIdx += 2 ;
		}
		else
		{
			++nIdx ;
		}
	}

	// check hu 
	for ( auto& refJiang : vJiang )
	{
		VEC_CARD vCheckHu , vCheckWithoutJiang;
		vCheckWithoutJiang.assign(vCheckCards.begin(),vCheckCards.end()) ;
		// remove jiang ;
		auto iter = std::find(vCheckWithoutJiang.begin(),vCheckWithoutJiang.end(),refJiang);
		vCheckWithoutJiang.erase(iter);
		iter = std::find(vCheckWithoutJiang.begin(),vCheckWithoutJiang.end(),refJiang);
		vCheckWithoutJiang.erase(iter);
		vCheckHu.assign(vCheckWithoutJiang.begin(),vCheckWithoutJiang.end()) ;

		// check shunzi ;
		getAnZiFromCards(vCheckHu,m_vAnKeZi);
		getShunZiFromCards(vCheckHu,m_vAllShunzi);
		if ( vCheckHu.empty() )
		{
			m_nJianPai = refJiang ;
			return true ;
		}
		

		// may be ke should represent shun , check this situation 
		if ( m_vAnKeZi.size() == 1 || m_vAnKeZi.size() == 2)
		{
			vCheckHu.assign(vCheckWithoutJiang.begin(),vCheckWithoutJiang.end()) ;
			if ( m_vAnKeZi.size() == 1 && vCheckHu.size() >= 9 )
			{
				clear();
				getShunZiFromCards(vCheckHu,m_vAllShunzi);
				if ( vCheckHu.empty() )
				{
					m_nJianPai = refJiang ;
					return true ;
				}
			}
			else if ( m_vAnKeZi.size() == 2 && vCheckHu.size() == 12 ) 
			{
				clear();
				if ( vCheckHu[0] == vCheckHu[2] )
				{
					m_vAnKeZi.push_back(vCheckHu[0]);
					auto n = 3 ;
					while (n--)
					{
						auto iter = vCheckHu.begin();
						vCheckHu.erase(iter);
					}
				}
				else if ( vCheckHu[11] == vCheckHu[9] )
				{
					m_vAnKeZi.push_back(vCheckHu[11]);
					auto n = 3 ;
					while ( n-- )
					{
						auto iter = vCheckHu.end();
						--iter;
						vCheckHu.erase(iter);
					}
				}
				
				if ( m_vAnKeZi.empty() == false )
				{
					getShunZiFromCards(vCheckHu,m_vAllShunzi);
					if ( vCheckHu.empty() )
					{
						m_nJianPai = refJiang ;
						return true ;
					}
				}
			}
			
		}
		clear();
	}

	return false ;
}

bool CMJHuPaiInfo::is7Pairs()
{
	return m_v7Pairs.size() == 7 ;
}

void CMJHuPaiInfo::clear()
{
	m_v7Pairs.clear();

	m_nJianPai = 0;
	m_vAnKeZi.clear(); 
	for ( auto& ref : m_vAllShunzi )
	{
		recycleShunziPtr(ref) ;
	}
	m_vAllShunzi.clear();
}
	
bool CMJHuPaiInfo::getAnZiFromCards( VEC_CARD& vCards, VEC_CARD& vOutAnZi )
{
	vOutAnZi.clear() ;
	for ( uint8_t nIdx = 0 ; (uint8_t)( nIdx + 2 ) < vCards.size() ; )
	{
		if ( vCards[nIdx] == vCards[nIdx + 2 ] )
		{
			vOutAnZi.push_back(vCards[nIdx]) ;
			// mark find result to 0 , for remove later 
			vCards[nIdx] = 0 ;
			vCards[nIdx+1] = 0 ;
			vCards[nIdx + 2 ] = 0 ;

			nIdx = nIdx + 3 ;
		}
		else
		{
			++nIdx ;
		}
	}

	if ( vOutAnZi.empty() )
	{
		return false ;
	}

	auto iter = std::find(vCards.begin(),vCards.end(),0 );
	while (iter != vCards.end() )
	{
		vCards.erase(iter) ;
		iter = std::find(vCards.begin(),vCards.end(),0 );
	}
	return true ;
}
	
bool CMJHuPaiInfo::getShunZiFromCards( VEC_CARD& vCards, VEC_SHUN& vOutShunZi )
{
	//getFengShunZiFromCards(vCards,vOutShunZi);

	for ( uint8_t nIdx = 0 ; (uint8_t)(nIdx + 2) < vCards.size() ; ++nIdx )
	{
		uint8_t nV0 = vCards[nIdx] ;
		if ( nV0 == 0 )
		{
			continue;
		}

		if ( CMJCard::parseCardType(nV0) == eCT_Feng || CMJCard::parseCardType(nV0) == eCT_Jian )
		{
			continue;
		}

		uint8_t nV1 = nV0 + 1 ;
		uint8_t nV2 = nV1 + 1 ;
		auto iterV1 = std::find(vCards.begin(),vCards.end(),nV1) ;
		if ( iterV1 == vCards.end() )
		{
			continue;
		}

		auto iterV2 = std::find(vCards.begin(),vCards.end(),nV2) ;
		if ( iterV2 == vCards.end() )
		{
			continue;
		}

		// find the ShunZi ;
		auto shunPtr = getReuseShunziPtr();
		shunPtr->vCards[0] = nV0 ;
		shunPtr->vCards[1] = nV1 ;
		shunPtr->vCards[2] = nV2 ;
		vOutShunZi.push_back(shunPtr) ;

		// mark 0 for remove later 
		vCards[nIdx] = 0 ;
		*iterV1 = 0 ;
		*iterV2 = 0 ;
	}

	if ( vOutShunZi.empty() )
	{
		return false ;
	}

	auto iter = std::find(vCards.begin(),vCards.end(),0 );
	while (iter != vCards.end() )
	{
		vCards.erase(iter) ;
		iter = std::find(vCards.begin(),vCards.end(),0 );
	}

	return vOutShunZi.empty() == false ;
}

bool CMJHuPaiInfo::getFengShunZiFromCards( VEC_CARD& vCards, VEC_SHUN& vOutShunZi )
{
	std::set<uint8_t> vF ;
	do 
	{
		vF.clear();
		for ( auto& ref : vCards )
		{
			if ( CMJCard::parseCardType(ref) != eCT_Feng )
			{
				continue;
			}
			vF.insert(ref);
			if ( vF.size() == 3 )
			{
				break;
			}
		}

		if ( vF.size() != 3 )
		{
			break;
		}

		// find shun zi 
		auto ptr = getReuseShunziPtr();
		uint8_t nIdx = 0 ;
		for ( auto& vVal : vF )
		{
			ptr->vCards[nIdx++] = vVal ;
			// remove the value from all cards ;
			auto iter = std::find(vCards.begin(),vCards.end(),vVal);
			vCards.erase(iter);
		}
		vOutShunZi.push_back(ptr) ;
	} while ( vF.size() == 3 );

	return vOutShunZi.empty() == false ;
}

bool CMJHuPaiInfo::check7Pair( VEC_CARD vCards )
{
	if ( vCards.size() != 14 )
	{
		return false;
	}

	for ( uint8_t nIdx = 0 ; (uint8_t)(nIdx + 1) < vCards.size(); nIdx += 2 )
	{
		if ( vCards[nIdx] != vCards[nIdx+1] )
		{
			m_v7Pairs.clear();
			return false ;
		}
		m_v7Pairs.push_back(vCards[nIdx]) ;
	}
	return true ;
}

CMJHuPaiInfo::shunzi_ptr CMJHuPaiInfo::getReuseShunziPtr()
{
	auto iter = m_vReuseShunZiPtr.begin() ;
	if ( iter != m_vReuseShunZiPtr.end() )
	{
		auto ptr = *iter ;
		m_vReuseShunZiPtr.erase(iter) ;
		ptr->clear();
		return ptr ;
	}

	auto ptr = shunzi_ptr(new stShunZi());
	ptr->clear();
	return ptr ;
}

void CMJHuPaiInfo::recycleShunziPtr(shunzi_ptr ptr )
{
	m_vReuseShunZiPtr.push_back(ptr) ;
}