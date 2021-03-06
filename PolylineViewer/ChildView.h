
// ChildView.h : interface of the CChildView class
//


#pragma once
#include <vector>
#include <string>
#include <math.h>
#include "utilities.h"
#include "shapefil.h"


using namespace std;


// CChildView window


namespace constants {
	constexpr double zoom_in_multiplier = 0.5;
	constexpr double zoom_out_multiplier = 2;
}


class CChildView : public CWnd
{
	// Construction
public:
	CChildView();

	// Attributes
public:

	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	static const int m_precision = 1000;
	static double m_current_zoom;
	virtual ~CChildView();


	// Generated message map functions
protected:
	BOOL LoadFile(LPCTSTR pszFile);
	void DrawShape(CPaintDC&, const Shape&) const;
	static const TCHAR m_scfilters[];
	CString m_path_name;				// path name for open file dialog
	vector<Shape> m_vec_shapes;			// vector for holding shapes by entity
	double m_min_bound[4] = {};			// shape bounds
	double m_max_bound[4] = { 1,1,1,1 };
	int m_entities;						// number of entities in a shape file
	int m_shape_type;					// shape type of a shape file
	int m_width;						// initial client area width
	int m_height;						// initial client area height
	bool m_opened = false;				// flag used in OnPaint handler to skip unnecessary function calls etc.
	bool m_zoom_other_trail = false;	//	flag used for checking if Zoom>Other dialog option was chosen
	bool m_tracking = false;			// flag used for tracking mouse movement after lmb was clicked
	bool m_origin_init = true;			// flag used for setting intial client area viewport origin
	POINT m_origin;						// POINT structure used to set viewport origin
	POINT m_org_shift = { 0,0 };		// vieport origin shift for handling mouse panning
	POINT m_logical_org_shift = { 0, 0 }; // value in logical coordinates (in current zoom setting) of pan translation in previous zoom setting
	POINT m_prev_shift = { 0, 0 };		// value of pan translation before zoom setting have been changed
	POINT m_zoom_shift = { 0, 0 };		// origin transtaltion due to zoom change (preserves center of client area)
	double m_zoom_change_tracker;		// reference value for checking if zoom change occured
	POINT m_point_from;					// POINT structure holding lmb click location
	POINT m_point_to;					// POINT structure holding current cursor position
	double m_prev_zoom = 1;				// previous zoom value for handling zoom dialog 'previous zoom' option
	afx_msg void OnFileOpen();			// message handler for File>Open option
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	// handlers for zoom dialog options
	afx_msg void OnRevertZoom();		
	afx_msg void OnUpdateRevertZoomUI(CCmdUI *pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnZoomIn();
	afx_msg void OnSelectZoom(UINT);
	afx_msg void OnUpdateSelectZoomUI(CCmdUI *pCmdUI);
	afx_msg void OnZoomOther();
	afx_msg void OnUpdateZoomOtherUI(CCmdUI *pCmdUI);
	//handlers for mouse input
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};







