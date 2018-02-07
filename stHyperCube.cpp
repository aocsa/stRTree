// stHyperCube.cpp: implementation of the stHyperCube class.
//
//////////////////////////////////////////////////////////////////////
#include <cmath>

#include "stHyperCube.h"


stHyperCube::stHyperCube() : stMBR(){
	this->Buffer = NULL;
	SetOID(0);
	for (int idx = 0; idx < DIMENSION; idx++) {
		Interval[idx].Begin = 0;
		Interval[idx].End = 0;
	}
}

stHyperCube::stHyperCube(const stHyperCube& obj) : stMBR() {
	SetOID(obj.ObjectID);	
	this->Buffer = NULL;
	for (int i = 0; i < DIMENSION;i++ ) {
		Interval[i] = obj.Interval[i];
	}
}

stHyperCube::~stHyperCube()
{
	if( this->Buffer) {
		delete []Buffer;
		this->Buffer = NULL;
	}
}

bool stHyperCube::operator == (const stHyperCube& obj) const {
	for (int i = 0; i < DIMENSION;i++ ) {
		if( (Interval[i].Begin != obj.Interval[i].Begin) || (Interval[i].End != obj.Interval[i].End) )
			return false;
	}
	return true;
}
void stHyperCube::AddInterval(int idx, stDistance origin) {
	assert(idx < DIMENSION);
	Interval[idx].Begin = origin;
	Interval[idx].End = origin;
}	

void stHyperCube::AddInterval(int idx, stDistance begin, stDistance end) {
	assert(idx < DIMENSION);
	assert(begin <= end);	
	Interval[idx].Begin = begin;
	Interval[idx].End = end;
}

stSize stHyperCube::GetDimension() const{
	return DIMENSION;
}

void stHyperCube::SetDimension() {
	;/////////////////////////////////////////////
}
stDistance& stHyperCube::GetBegin(int idx)  {
	assert(idx < DIMENSION);
	return Interval[idx].Begin;
}
stDistance& stHyperCube::GetEnd(int idx) {
	assert(idx < DIMENSION);
	return Interval[idx].End;
}
stDistance stHyperCube::GetBegin(int idx)  const{
	assert(idx < DIMENSION);
	return Interval[idx].Begin;
}
stDistance stHyperCube::GetEnd(int idx) const{
	assert(idx < DIMENSION);
	return Interval[idx].End;
}

stDistance stHyperCube::GetArea() {
	stDistance area = 1;
	for (int k = 0; k < DIMENSION; k++)
		area *= fabs(Interval[k].Begin - Interval[k].End);
	return area;
}	
/*
stDistance  stHyperCube::IntersectionArea(stMBR* mbr)
{
    stDistance sum = 0.0f;
	stMBR& I1 = *this;
	stMBR& I2 = *mbr;

    for (int k=0; k<DIMENSION; k++) {
        if( (I1.GetBegin(k) <= I2.GetBegin(k)) && (I2.GetEnd(k) <= I1.GetEnd(k)) )	
            sum += fabs(I1.GetEnd(k) - I1.GetBegin(k) );
        else if((I2.GetBegin(k) <= I1.GetBegin(k)) && (I1.GetEnd(k) <= I2.GetEnd(k)))
            sum += abs(I2.GetEnd(k) - I2.GetBegin(k) );
        else if( (I1.GetBegin(k) <= I2.GetBegin(k)) && (I2.GetBegin(k) <= I1.GetEnd(k)) )
            sum += abs(I1.GetEnd(k) - I2.GetBegin(k));	
        else
            sum += abs(I1.GetBegin(k) - I2.GetEnd(k));
    }
    return sum;
}

bool stHyperCube::Intersection(stMBR* mbr) 
{	
	stDistance sum = 0.0f;
	stMBR& I1 = *this;
	stMBR& I2 = *mbr;

	for (size_t k = 0; k < DIMENSION; k++)	{
		if( !(  
				( (I2.GetBegin(k) < I1.GetBegin(k)) && (I1.GetBegin(k) < I2.GetEnd(k)) ) 
				||
				( (I2.GetBegin(k) < I1.GetEnd(k)) && (I1.GetEnd(k) < I2.GetEnd(k)) )
			 )
		  )
		  return false;
	}
	return true;	
}

stMBR* 	stHyperCube::GetUnionMBR(stMBR* mbr)
{
	stHyperCube& v = *(new stHyperCube());
	stMBR& I1 = *this;
	stMBR& I2 = *mbr;

	for (size_t k=0; k < DIMENSION; k++) {
		v.GetBegin(k) = I1.GetBegin(k) < I2.GetBegin(k)? I1.GetBegin(k) : I2.GetBegin(k);
		v.GetEnd(k)   = I1.GetEnd(k) > I2.GetEnd(k)? I1.GetEnd(k) : I2.GetEnd(k); 
	}
	return &v;
}

stDistance stHyperCube::GetMinDist(const stPoint &p) {
	assert(p.dim() == GetDimension()); 
		//throw new illegal_argument_exception("Point dimension is different from HyperCube dimension.");

	stDistance ret = 0;
	for (int i = 0; i < GetDimension(); i++) {
		float q = p[i];
		float s = this->Interval[i].Begin;
		float t = this->Interval[i].End;
		float r;

		if (q < s) 
			r = s;
		else if (q > t) 
			r = t;
		else
			r = q;

		ret += pow(fabs(q - r), 2);
	}
	stHyperCube::NumberOfDC++;
	return sqrt(ret);
}

stDistance stHyperCube::GetMinDist(stHyperCube* p)
{
	assert(p->GetDimension() == GetDimension()); 

	stDistance ret = 0;
	for (int i = 0; i < GetDimension(); i++) {
		float q = p->GetBegin(i);
		float s = this->Interval[i].Begin;
		float t = this->Interval[i].End;
		float r;

		if (q < s) 
			r = s;
		else if (q > t) 
			r = t;
		else
			r = q;

		ret += pow(fabs(q - r), 2);
	}
	stHyperCube::NumberOfDC++;
	return sqrt(ret);
}
*/

bool stHyperCube::IsEqual(stMBR* obj){
	//return *this == *(obj);
	return this->GetOID() == ((stHyperCube*)obj)->GetOID();
}

stSize  stHyperCube::GetSerializedSize() const
{
	return sizeof(stInterval) * DIMENSION + sizeof(stPageID);
}

const stByte* stHyperCube::Serialize() 
{
    //if( this->Buffer == NULL) { //important if update, GetGegin(i)++;
        this->Buffer = new stByte[ this->GetSerializedSize()  ];
        stPageID* pOid = (stPageID* )(this->Buffer);
		pOid[0] = this->GetOID();

		stInterval* d = (stInterval* )(this->Buffer + sizeof(stPageID));
        for (int i = 0; i < DIMENSION; i++) {
            d[i].Begin = this->Interval[i].Begin;
			d[i].End = this->Interval[i].End;
		}
    //}
    return this->Buffer;	
}

void	stHyperCube::Unserialize(const stByte * data, stSize datasize)
{
	stPageID* pOid = (stPageID* )(data);
	SetOID(pOid[0]);

	stInterval* d = (stInterval* )(data + sizeof(stPageID));
    for (int i = 0; i < DIMENSION; i++) {
        this->Interval[i].Begin = d[i].Begin;
		this->Interval[i].End = d[i].End;
	}
	if( this->Buffer) {
		delete []Buffer;
		this->Buffer = NULL;
	}
}

void	stHyperCube::Print(std::ostream &out) 
{
	out << "OID: " << GetOID() << "\n";
	for (int i = 0; i < DIMENSION;i++) {
		out << this->GetBegin(i) << ",  " << this->GetEnd(i) << "\n";
	}
	out << "\n";
};
