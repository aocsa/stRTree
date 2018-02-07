

#include <cmath>


#include "stHyperCube.h"
#include "stMBR.h"
#include "point.h"


stMBR* 	stMBR::GetUnionMBR(stMBR* mbr)
{
	stMBR& v = *(new stHyperCube);
	stMBR& I1 = *this;
	stMBR& I2 = *mbr;

	for (size_t k=0; k < DIMENSION; k++) {
		v.GetBegin(k) = I1.GetBegin(k) < I2.GetBegin(k)? I1.GetBegin(k) : I2.GetBegin(k);
		v.GetEnd(k)   = I1.GetEnd(k) > I2.GetEnd(k)? I1.GetEnd(k) : I2.GetEnd(k); 
	}
	return &v;
}

stDistance  stMBR::IntersectionArea(stMBR* mbr)
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

bool stMBR::Intersection(stMBR* mbr) 
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



stDistance stMBR::GetMinDist(stMBR* p)
{
	assert(p->GetDimension() == GetDimension()); 

	stDistance ret = 0;
	for (int i = 0; i < GetDimension(); i++) {
		float q = p->GetBegin(i);
		float s = this->GetBegin(i); //Interval[i].Begin;
		float t = this->GetEnd(i);	//this->Interval[i].End;
		float r;

		if (q < s) 
			r = s;
		else if (q > t) 
			r = t;
		else
			r = q;

		ret += pow(fabs(q - r), 2);
	}
	return sqrt(ret);
}
