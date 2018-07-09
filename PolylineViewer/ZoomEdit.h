#pragma once

#define WM_USER_UPDATE  WM_USER + 1

// three ZoomEdit versions



class ZoomEdit1 : public CEdit
{
	DECLARE_DYNAMIC(ZoomEdit1)

public:
	static int m_zoom_num;		// zoom value numerator
	static bool m_update_trace;	// flag used to prevent message loops
	ZoomEdit1();
	virtual ~ZoomEdit1();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnUpdate();
	afx_msg LRESULT OnUserUpdate(WPARAM wParam, LPARAM lParam);
	
};

class ZoomEdit2 : public CEdit
{
	DECLARE_DYNAMIC(ZoomEdit2)

public:
	static int m_zoom_den;		// zoom value denominator
	static bool m_update_trace;	// flag used to prevent message loops
	ZoomEdit2();
	virtual ~ZoomEdit2();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChange();
	afx_msg LRESULT OnUserUpdate(WPARAM wParam, LPARAM lParam);
};

class ZoomEdit3 : public CEdit
{
	DECLARE_DYNAMIC(ZoomEdit3)

public:
	static double m_zoom_percent;	// zoom value in percent form
	static bool m_update_trace;		// flags used to prevent message loops
	static bool m_spin_trace;		
	ZoomEdit3();
	virtual ~ZoomEdit3();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChange();
	afx_msg LRESULT OnUserUpdate(WPARAM wParam, LPARAM lParam);
};

