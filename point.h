
#ifndef _POINT_H_
#define _POINT_H_

#include <cassert>
#include <vector>
#include <iostream>
#include "stMBR.h"

//#define DIMENSION 65 
	//DIGITS

//#define DIMENSION 54 
	//COVERS

#define DIMENSION 2 

#include "stRLogicNode.h"

template<class TObject>
class stRLogicNode;


#define PointToXYZ(p) p[0], p[1], p[2]
// Error codes
enum Error {
	Enot,	// no error
	Edim,	// error: dim of point invalid for operation
	Esum	// error: sum not affine (cooefs add to 1)
};
enum booleano {FALSE_B=0, TRUE_B=1, ERROR_B= -1 };

class stPoint : public stMBR{
protected:
	Error err;           // error indicator
	stPageID  ObjectID;
	std::vector<double> myPoint;
	typedef stRLogicNode<stMBR> tLogicNode;
	tLogicNode* myNode;	

public:
	//----------------------------------------------------------
	// Lots of Constructors (add more as needed)
	stPoint(int dim = DIMENSION) : myPoint(dim) {
		for (int i=0; i < this->dim(); i++){
			myPoint[i] = 0;
		}
		myNode = NULL;
	} 
	stPoint(const stPoint& p) : myPoint(p.dim()){
		for (int i=0; i < dim(); i++){
			myPoint[i] = p.myPoint[i];
		}
		ObjectID = p.ObjectID;
		err=Enot;
		myNode = NULL;
	}
	// Destructor
	~stPoint() {};

	void insertArray(double v[], int dim) {
		this->myPoint.clear();
		for (int i = 0; i < dim ; i++)
		{
			myPoint.push_back(v[i]);
		}
	}
	void insertArray(float v[], int dim) {
		this->myPoint.clear();
		for (int i = 0; i < dim ; i++)
		{
			myPoint.push_back(v[i]);
		}
	}
	//----------------------------------------------------------
	friend std::ostream& operator<<( std::ostream&, const stPoint&);

	// Assignment "=": use the default to copy all members
	int dim() const{ return this->myPoint.size(); }      // get dimension
	
	//----------------------------------------------------------
	// Comparison (dimension must match, or not)
	bool operator == ( const stPoint&);
	bool operator != ( const stPoint&);
	void operator = ( const stPoint&);	
	//----------------------------------------------------------
	// stPoint and Vector Operations (always valid) 
	stPoint  operator-( const stPoint&);       // Vector difference
	stPoint  operator+( const stPoint&);      // +translate
	
	stPoint  operator*( const stPoint&);       // Vector difference
	stPoint  operator/( const stPoint&);      // +translate
	
	stPoint& operator+=( const stPoint&);     // inc translate
	stPoint& operator-=( const stPoint&);     // dec translate
	
	double& operator [] (int idx)  {
		assert(idx < dim()); 
		return this->myPoint[idx];
	}
	double operator [] (int idx)  const{
		return this->myPoint[idx];
	}

	// Scalar Multiplication
	friend stPoint operator*( int, const stPoint&);
	friend stPoint operator*( double, const stPoint&);
	friend stPoint operator*( const stPoint&, double);
	// Scalar Division
	friend stPoint operator/( const stPoint&, double);

	friend stPoint operator+( const stPoint&, double);
	friend stPoint operator-( const stPoint&, double);

	//----------------------------------------------------------
	// stPoint Relations
	static double distance( const stPoint&, const stPoint&);         // Distance
	//----------------------------------------------------------
	// Error Handling
	void  clerr() { err = Enot;}            // clear error
	int   geterr() { return err;}           // get error
	char* errstr();                         // return error string

//////////////////////////////////////////////////////////////////////////
	void SetOID(stPageID oid) {
		this->ObjectID = oid;
	}
	stPageID GetOID() const {
		return ObjectID;
	}
	virtual bool IsEqual(stMBR* obj) {
		return this->GetOID() == ((stPoint*)obj)->GetOID();
	}
	
	virtual stMBR* Clone() {
		return new stPoint(*this);
	}
	virtual void	    AddInterval(int idx, stDistance origin) {
		myPoint[idx] = origin;
	}
	virtual void	    AddInterval(int idx, stDistance begin, stDistance end)  {
		myPoint[idx] = begin;
	}
	virtual stSize	    GetDimension() const {
		return myPoint.size();
	}
	virtual void	    SetDimension()  {
		////////?????
	}
	
	virtual stDistance  GetArea	()  {
		return 0.0f;
	}
	
	virtual stDistance& GetBegin(int idx) {
		return myPoint[idx];
	}
	virtual stDistance& GetEnd(int idx) {
		return myPoint[idx];
	}
	virtual stDistance GetBegin(int idx) const  {
		return myPoint[idx];
	}
	virtual stDistance GetEnd(int idx) const  {
		return myPoint[idx];
	}	
	void Print(std::ostream& out) {
		out << *this << "\n";
	}
	stPoint::tLogicNode* GetNode() {
		return myNode;
	}
	void  SetNode(tLogicNode* node) {
		assert(node->GetNumberOfEntries() > 0);
		std::cout << "******GetOID: " << GetOID() << "\n";
		myNode = node;
	}
};
//-------------------------------------------------------------


#endif //POINT_H_
