// stHyperCube.h: interface for the stHyperCube class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STHYPERCUBE_H__868DAFC6_34E5_4CC9_A461_032F45D9DE9C__INCLUDED_)
#define AFX_STHYPERCUBE_H__868DAFC6_34E5_4CC9_A461_032F45D9DE9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cassert>
#include <cmath>

#include "point.h"
#include "stMBR.h"


class stHyperCube : public stMBR{
public:
	struct stInterval {
		stDistance Begin;
		stDistance End;
	};

public:
	stHyperCube();
	stHyperCube(const stHyperCube& obj) ;
	~stHyperCube();

	bool operator == (const stHyperCube& obj) const ;
	virtual void	   AddInterval(int idx, stDistance origin) ;
	virtual void	   AddInterval(int idx, stDistance begin, stDistance end) ;
	virtual stSize	   GetDimension() const;
	virtual void	   SetDimension() ;
	virtual stDistance& GetBegin(int idx) ;
	virtual stDistance& GetEnd(int idx) ;
	virtual stDistance GetBegin(int idx) const;
	virtual stDistance GetEnd(int idx) const;
	virtual stDistance GetArea() ;
	
	virtual stMBR* Clone() {
		return new stHyperCube(*this);
	}	
	virtual bool	IsEqual(stMBR* obj);
    virtual stSize  GetSerializedSize() const;
    virtual const stByte* Serialize();
    virtual void	Unserialize(const stByte * data, stSize datasize);
    virtual void	Print(std::ostream &out);
	
	void SetOID(stPageID oid) {
		this->ObjectID = oid;
	}
	stPageID GetOID() const {
		return ObjectID;
	}
	

public:
	static int NumberOfDC;

private:
	stInterval  Interval[DIMENSION];
	stByte*		Buffer;
	stPageID	ObjectID;
}; 


#endif // !defined(AFX_STHYPERCUBE_H__868DAFC6_34E5_4CC9_A461_032F45D9DE9C__INCLUDED_)