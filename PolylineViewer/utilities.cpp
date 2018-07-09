
// utilities.cpp : implementation of utility classes, functions etc.
//


#include "stdafx.h"
#include <vector>
#include "shapefil.h"
#include "utilities.h"
#include "mpir.h"


//RAII constructor, destructor pairs


Shape::Shape(SHPHandle hSHP, int nShape) {
	m_pshape = SHPReadObject(hSHP, nShape);
}


Shape::~Shape() {
	SHPDestroyObject(m_pshape);
}


ShapeOpen::ShapeOpen(const char *  pszShapeFile, const char * pszAccess) {
	if (m_handle_shape = SHPOpen(CStringA(pszShapeFile), "rb")) {
		m_opened = TRUE;
	}
}


ShapeOpen::~ShapeOpen() {
	SHPClose(m_handle_shape);
}


// Shape move constructor and assigment


Shape::Shape(Shape&& Shape) : m_pshape{ Shape.m_pshape } {
	Shape.m_pshape = nullptr;
}


Shape & Shape::operator=(Shape&& Shape) {
	m_pshape = Shape.m_pshape;
	Shape.m_pshape = nullptr;
	return *this;


}

// select area border draw handler

void InvertRectBorder(CDC* pdc, POINT point_from, POINT point_to) {
	int prev_mode = pdc->SetROP2(R2_NOT);
	pdc->MoveTo(point_from);
	pdc->LineTo(point_from.x, point_to.y);
	pdc->LineTo(point_to);
	pdc->LineTo(point_to.x,point_from.y);
	pdc->LineTo(point_from);
	pdc->SetROP2(prev_mode);
}

//Double to rational conversion (approx.)

//normalize input double to fit rational representation num, denom ranges

void Normalize(double &d, double max_bound, double interval) {
	mpq_t op = {};
	mpq_set_d(op, interval);
	mpq_canonicalize(op);
	long denom = mpz_get_si(mpq_denref(op));
	if (d > max_bound/denom)
		d = round(d);
}

// binary search
// divides zoom range so it consists of intervals of specified length
// then it finds which interval the input double fits into
// rational representation of this double equals the value of
//closest border of this interval 
void Search(double left_search_bound, double right_search_bound, double& d, double interval) {
	double compare_point = (left_search_bound + right_search_bound) / 2;
	if (d < compare_point) {
		right_search_bound = compare_point;
	}
	else if (d > compare_point) {
		left_search_bound = compare_point;
	}
	if (right_search_bound - left_search_bound > interval && d != compare_point) {
		Search(left_search_bound, right_search_bound, d, interval);
	}
	else if (right_search_bound - left_search_bound == interval && d != compare_point) {
		if (right_search_bound - d <= d - left_search_bound) {
			d = right_search_bound;
		}
		else {
			d = left_search_bound;
		}
	}
}

// uses mpir to get rational representation of doubles in mpq_set_d range
// (denom is a power of 2)

vector<int> GetRationalRepresentation(double& d, double init_bound, double max_bound, double interval) {
	Normalize(d, max_bound, interval);
	vector<int> v;
	//handle non zero case
	if (d) {
		// rational number handler
		mpq_t op = {};
		// double to rational conversion
		mpq_set_d(op, d);
		// get canonical form of op
		mpq_canonicalize(op);
		//get denom of op
		long denom = mpz_get_si(mpq_denref(op));
		// handle values that are out of range of mpq_set_d function
		if (!denom) {
			double left_search_bound = 0;
			double right_search_bound = 0;
			//find the range to search in
			for (int i = init_bound; i <= max_bound; i *= 2) {
				if (d < i) {
					right_search_bound = i;
					Search(left_search_bound, right_search_bound, d, interval);
					break;
				}
			}
		}
		//  handle doubles in range of mpq_set_d
		mpq_set_d(op, d);
		mpq_canonicalize(op);
		long numer = mpz_get_si(mpq_numref(op));
		denom = mpz_get_si(mpq_denref(op));
		mpq_clear(op);
		v.push_back(numer);
		v.push_back(denom);
		return v;
	}
	// handle 0 case
	v.push_back(0);
	v.push_back(1);
	return v;
}