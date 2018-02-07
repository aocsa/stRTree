/* 
 * File:   stMBR.h
 * Author: aocsa
 *
 * Created on January 17, 2007, 6:37 PM
 */

#ifndef _stMBR_H
#define	_stMBR_H

#include <vector>
#include <ostream>

#include <arboretum/stObject.h>
#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>


//#include "stRLogicNode.h"
//template<class TObject>
//class stRLogicNode;

class stMBR {
protected:
	stMBR() {}
	
public:
	virtual stMBR*		Clone() = 0;
	virtual void	    AddInterval(int idx, stDistance origin) = 0;
	virtual void	    AddInterval(int idx, stDistance begin, stDistance end) = 0;
	virtual stSize	    GetDimension() const = 0;
	virtual void	    SetDimension() = 0;
	virtual stDistance& GetBegin(int idx) = 0;
	virtual stDistance& GetEnd(int idx) = 0;
	virtual stDistance GetBegin(int idx) const = 0;
	virtual stDistance GetEnd(int idx) const = 0;
	virtual stDistance  GetArea	() = 0;

	stMBR*		GetUnionMBR(stMBR* mbr);
	stDistance  GetMinDist(stMBR* p) ;
	stDistance  IntersectionArea(stMBR* mbr) ;
	bool	    Intersection(stMBR* mbr) ;

	virtual bool IsEqual(stMBR* obj) = 0;
	virtual void Print(std::ostream& out) = 0;

	/*virtual stRLogicNode<stMBR>* GetNode()  {
		return NULL;
	}
	virtual void  SetNode(stRLogicNode<stMBR>* node) {

	}*/

};

#endif	/* _stMBR_H */