// ZoomDialog.cpp : implementation file
//
#include "stdafx.h"
#include "PolyLineViewer.h"
#include "ChildView.h"
#include "ZoomDialog.h"
#include "afxdialogex.h"


// ZoomDialog dialog

bool ZoomDialog::m_select_area_trace = false;
int ZoomDialog::m_zoom_num = 0;
int ZoomDialog::m_zoom_den = 0;
double ZoomDialog::m_zoom_percent = 0;


IMPLEMENT_DYNAMIC(ZoomDialog, CDialogEx)


ZoomDialog::ZoomDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZOOMDIALOG, pParent)
{

}

ZoomDialog::~ZoomDialog()
{
}


BEGIN_MESSAGE_MAP(ZoomDialog, CDialogEx)
	ON_CONTROL(EN_UPDATE, IDC_EDIT1, OnEnUpdateEdit1)
	ON_CONTROL(EN_UPDATE, IDC_EDIT2, OnEnUpdateEdit2)
	ON_CONTROL(EN_UPDATE, IDC_EDIT3, OnEnUpdateEdit3)
	ON_CONTROL(EN_KILLFOCUS, IDC_EDIT1, OnKillFocusEdit1)
	ON_CONTROL(EN_KILLFOCUS, IDC_EDIT2, OnKillFocusEdit2)
	ON_CONTROL(EN_KILLFOCUS, IDC_EDIT3, OnKillFocusEdit3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaPosSpin3)
	ON_CONTROL(BN_CLICKED, IDC_BUTTON, OnClickedButton)
END_MESSAGE_MAP()


// ZoomDialog message handlers


BOOL ZoomDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// set initial values in edit controls, set spin control properties
	vector<int> v(2);
	v = GetRationalRepresentation(CChildView::m_current_zoom, constants::init_bound,
		constants::zoom_max, constants::interval);
	CSpinButtonCtrl spin_button;
	spin_button.Attach(GetDlgItem(IDC_SPIN1)->m_hWnd);
	spin_button.SetPos(v[1]);
	spin_button.SetRange(constants::num_min, constants::num_max);
	spin_button.Detach();
	spin_button.Attach(GetDlgItem(IDC_SPIN2)->m_hWnd);
	spin_button.SetPos(v[0]);
	spin_button.SetRange(constants::den_min, constants::den_max);
	spin_button.Detach();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


// zoom dialog control communication


void ZoomDialog::OnEnUpdateEdit1() {
	// get int value from edit control string
	CString buffer;
	GetDlgItemText(IDC_EDIT1, buffer);
	CStringA buffer1(buffer);
	m_zoom_num = atof(buffer1);
	// cut-off for out of range values
	if (m_zoom_num < constants::num_min) {
		m_zoom_num = constants::num_min;
	}
	else if (m_zoom_num > constants::num_max) {
		m_zoom_num = constants::num_max;
	}
	// code handling updates of control text in accordance with other edit controls
	if (!m_update_trace) {
		m_zoom_percent = 100.0 * ZoomDialog::m_zoom_num / ZoomDialog::m_zoom_den;
		m_update_trace = true;
		buffer.Format(_T("%.*f"), 2, m_zoom_percent);
		SetDlgItemText(IDC_EDIT3, buffer);
	}
}


void ZoomDialog::OnEnUpdateEdit2() {
	// get int value from edit control string
	CString buffer;
	GetDlgItemText(IDC_EDIT2, buffer);
	CStringA buffer1(buffer);
	m_zoom_den = atof(buffer1);
	// cut-off for out of range values
	if (m_zoom_den < constants::den_min) {
		m_zoom_den = constants::den_min;
	}
	else if (m_zoom_den > constants::den_max) {
		m_zoom_den = constants::den_max;
	}
	// code handling updates of control text in accordance with other edit controls
	if (!m_update_trace) {
		m_update_trace = true;
		m_zoom_percent = 100.0 * ZoomDialog::m_zoom_num / (ZoomDialog::m_zoom_den);
		buffer.Format(_T("%.*f"), 2, m_zoom_percent);
		SetDlgItemText(IDC_EDIT3, buffer);
	}
	m_update_trace = false;
}


void ZoomDialog::OnEnUpdateEdit3() {
	// get double value from edit control string
	CString buffer;
	GetDlgItemText(IDC_EDIT3, buffer);
	CStringA buffer1(buffer);
	m_zoom_percent = atof(buffer1);
	// cut-off for out of range values
	if (m_zoom_percent < 100 * constants::zoom_min) {
		m_zoom_percent = 100 * constants::zoom_min;
	}
	else if (m_zoom_percent > 100 * constants::zoom_max) {
		m_zoom_percent = 100 * constants::zoom_max;
	}
	// code handling updates of control text in accordance with other edit controls
	if (!m_update_trace) {
		m_update_trace = true;
		vector<int> v;
		double temp = ZoomDialog::m_zoom_percent / 100;
		v = GetRationalRepresentation(temp, constants::init_bound,
			constants::zoom_max, constants::interval);
		m_zoom_num = v[0];
		m_zoom_den = v[1];
		buffer.Format(_T("%i"), m_zoom_num);
		SetDlgItemText(IDC_EDIT1, buffer);
		buffer.Format(_T("%i"), m_zoom_den);
		SetDlgItemText(IDC_EDIT2, buffer);
	}
	m_update_trace = false;

}


// edit control kill focus handlers


void ZoomDialog::OnKillFocusEdit1() {
	CString buffer;
	buffer.Format(_T("%i"), m_zoom_num);
	SetDlgItemText(IDC_EDIT1, buffer);
}


void ZoomDialog::OnKillFocusEdit2() {
	CString buffer;
	buffer.Format(_T("%i"), m_zoom_den);
	SetDlgItemText(IDC_EDIT2, buffer);
}


void ZoomDialog::OnKillFocusEdit3() {
	CString buffer;
	buffer.Format(_T("%.*f"), 2, m_zoom_percent);
	SetDlgItemText(IDC_EDIT3, buffer);
}


// spin control (IDC_SPIN3) scroll handler


void ZoomDialog::OnDeltaPosSpin3(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (pNMUpDown->iDelta > 0) {
		m_zoom_percent -= constants::spin_delta;
	}
	else {
		m_zoom_percent += constants::spin_delta;
	}
	CString buffer;
	buffer.Format(_T("%.*f"), 2, m_zoom_percent);
	SetDlgItemText(IDC_EDIT3, buffer);
	*pResult = 0;
}


// button control toggle behaviour (pan to select area)


void ZoomDialog::OnClickedButton() {
	m_select_area_trace = !m_select_area_trace;
}