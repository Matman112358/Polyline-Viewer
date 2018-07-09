
// utilities.h : interface of utility classes, functions etc.
//



#pragma once
#include "stdafx.h"
#include <vector>
#include "shapefil.h"


using namespace std;


namespace constants {
	//rational representation of zoom value ranges
	constexpr int num_min = 1;
	constexpr int num_max = 256;
	constexpr int den_min = 1;
	constexpr int den_max = 16;
	//zoom ranges
	constexpr double zoom_min = 0.0625;
	constexpr double zoom_max = 256;
	// init bound for searching algorithm
	constexpr double init_bound = 1;
	// interval for searching algorithm
	constexpr double interval = 1.0 / 16;
}


//RAII wrappers


//SHP object wrapper

class Shape {
	SHPObject* m_pshape;
public:
	SHPObject * Get() const { return m_pshape; };
	Shape(Shape&&);
	Shape & operator=(Shape&&);
	Shape(SHPHandle, int);
	~Shape();
};

// shape file i/o handler wrapper

class ShapeOpen {
	SHPHandle m_handle_shape;
	BOOL m_opened = FALSE;	// flag for cheking if file opening was succesful
public:
	BOOL GetState() { return m_opened; };
	const SHPHandle& GetSHPHandle() const { return m_handle_shape; };
	ShapeOpen(const char *, const char *);
	~ShapeOpen();
};


//Function for converting doubles to integer ordered pairs


void InvertRectBorder(CDC*, POINT, POINT);

// binary search for finding approx rational representation of a double

void Search(double, double, double& , double);


vector<int> GetRationalRepresentation(double&, double, double, double);