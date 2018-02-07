

#include <cmath>

#include "point.h"
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif


//==================================================================
//------------------------------------------------------------------
// Assign (set) dimension
//------------------------------------------------------------------



bool stPoint::operator==( const stPoint& Q)
{
	if (dim() != Q.dim()) return FALSE_B;
	for (int i = 0; i < dim() ; i++)
	{
		if ( this->myPoint[i] != Q.myPoint[i] ) {
			return false;
		}
	}
		
	return true;
}


bool stPoint::operator!=( const stPoint& Q)
{
	return !(operator == (Q));
}


void stPoint::operator=(const stPoint& p)
{
	err = p.err;           // error indicator
	
	for (int i = 0; i < dim() ; i++)
	{
		this->myPoint[i] = p.myPoint[i];
	}
}
//------------------------------------------------------------------
// Point Vector Operations
//------------------------------------------------------------------


stPoint stPoint::operator+( const stPoint& v)        // +ve translation
{
	stPoint P(v.dim());
	assert(this->dim() == v.dim());
	for (int i = 0; i < dim() ; i++)
	{
		P[i] = this->myPoint[i] + v.myPoint[i];
	}
	return P;
}


stPoint stPoint::operator-( const stPoint& v)        // -ve translation
{
	stPoint P(v.dim());
	assert(this->dim() == v.dim());

	for (int i = 0; i < dim() ; i++) {
		P[i] = this->myPoint[i] - v.myPoint[i];
	}
	return P;
}


stPoint stPoint::operator*( const stPoint& v)        // +ve translation
{
	stPoint P(v.dim());
	assert(this->dim() == v.dim());
	
	for (int i = 0; i < dim() ; i++)
	{
		P[i] = this->myPoint[i] * v.myPoint[i];
	}

	return P;
}


stPoint stPoint::operator/( const stPoint& v)        // -ve translation
{
	stPoint P(v.dim());
	assert(this->dim() == v.dim());
	for (int i = 0; i < dim() ; i++)
	{
		assert(v.myPoint[i] != 0);
		P[i] = this->myPoint[i] / v.myPoint[i];
	}

	return P;
}


stPoint& stPoint::operator+=( const stPoint& v)        // +ve translation
{
	assert(this->dim() == v.dim());
	for (int i = 0; i < dim() ; i++) {
		this->myPoint[i] += v.myPoint[i];
	}
	return *this;
}


stPoint& stPoint::operator-=( const stPoint& v)        // -ve translation
{
	assert(this->dim() == v.dim());
	for (int i = 0; i < dim() ; i++)
	{
		this->myPoint[i] -= v.myPoint[i];
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
// Point Scalar Operations (convenient but often illegal)
//        are not valid for points in general,
//        unless they are 'affine' as coeffs of 
//        a sum in which all the coeffs add to 1,
//        such as: the sum (a*P + b*Q) with (a+b == 1).
//        The programmer must enforce this (if they want to).
//------------------------------------------------------------------

std::ostream& operator<<( std::ostream& out,const stPoint& p) {
	out << "OID: " << p.GetOID() << "\n";
	for (int i = 0; i < p.dim(); i++) {
		out << p[i] << ", ";
	}
	return out;
}

stPoint operator*( int c, const stPoint& Q) {
	stPoint P(Q.dim());
	
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = c * Q[i];
	}
	return P;
}

stPoint operator*( double c, const stPoint& Q) {
	stPoint P(Q.dim());
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = c * Q[i];
	}
	return P;
}

stPoint operator*( const stPoint& Q, double c) {
	stPoint P(Q.dim());
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = c * Q[i];
	}
	return P;
}

stPoint operator/( const stPoint& Q, double c) {
	stPoint P(Q.dim());
	assert(c != 0);
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = Q[i]/c;
	}
	return P;
}

//------------------------------------------------------------------
// Point Addition (also convenient but often illegal)
//    is not valid unless part of an affine sum.
//    The programmer must enforce this (if they want to).
//------------------------------------------------------------------

stPoint operator+( const stPoint& Q, double c) {
	stPoint P(Q.dim());
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = Q[i] + c;
	}
	return P;
}

stPoint operator-( const stPoint& Q, double c) {
	stPoint P(Q.dim());
	for (int i = 0; i < Q.dim() ; i++)
	{
		P[i] = Q[i] - c;
	}
	return P;
}


//------------------------------------------------------------------
// Sidedness of a Point wrt a directed line P1->P2
//        - makes sense in 2D only
//------------------------------------------------------------------

//------------------------------------------------------------------
// Error Routines
//------------------------------------------------------------------

char* stPoint::errstr() {            // return error string
	switch (err) {
	case Enot:
		return "no error";
	case Edim:
		return "error: invalid dimension for operation";
	case Esum:
		return "error: Point sum is not affine";
	default:
		return "error: unknown err value";
	}
}

double stPoint::distance( const stPoint&p0, const stPoint&p1)         // Distance
{
	double sum = 0.0f;
	//sum += (p0.x - p1.x)*(p0.x - p1.x);
	//sum += (p0.y - p1.y)*(p0.y - p1.y);
	//sum += (p0.z - p1.z)*(p0.z - p1.z);
	assert(p0.dim() == p1.dim());
	for (int i = 0; i < p0.dim(); i++) {
		sum += (p0[i] - p1[i])*(p0[i] - p1[i]);
	}
	return sqrt(sum);
}

