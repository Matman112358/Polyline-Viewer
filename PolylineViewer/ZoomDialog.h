#pragma once

#include "utilities.h"
// ZoomDialog dialog


class ZoomDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ZoomDialog)
public:
	ZoomDialog(CWnd* pParent = nullptr);   // standard constructor
	static bool m_select_area_trace;
	virtual ~ZoomDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZOOMDIALOG };
#endif
	bool m_update_trace = false;	// flag used to avoid message loops
	static int m_zoom_num;			// zoom ratio numerator
	static  int m_zoom_den;			// zoom ratio denominator
	static double m_zoom_percent;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateEdit1();
	afx_msg void OnEnUpdateEdit2();
	afx_msg void OnEnUpdateEdit3();
	afx_msg void OnKillFocusEdit1();
	afx_msg void OnKillFocusEdit2();
	afx_msg void OnKillFocusEdit3();
	afx_msg void OnDeltaPosSpin3(NMHDR*, LRESULT*);
	afx_msg void OnClickedButton();
};
