
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "PolylineViewer.h"
#include "ChildView.h"
#include "ZoomDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView
const TCHAR CChildView::m_scfilters[] =
_T("Shape Files (*.shp)¦*.shp¦All Files (*.*)¦*.*¦¦");

double CChildView::m_current_zoom = 1;

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_PAINT()
	ON_COMMAND(ID_REVERTZOOM, &CChildView::OnRevertZoom)
	ON_UPDATE_COMMAND_UI(ID_REVERTZOOM, &CChildView::OnUpdateRevertZoomUI)
	ON_COMMAND(ID_ZOOMOUT, &CChildView::OnZoomOut)
	ON_COMMAND(ID_ZOOMIN, &CChildView::OnZoomIn)
	ON_COMMAND_RANGE(ID_ZOOM_1, ID_ZOOM_OTHER, &CChildView::OnSelectZoom)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_1, ID_ZOOM_OTHER, OnUpdateSelectZoomUI)
	ON_COMMAND(ID_ZOOM_OTHER, &CChildView::OnZoomOther)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OTHER, &CChildView::OnUpdateZoomOtherUI)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T("shp"), _T("*.shp"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, m_scfilters);
	if (dlg.DoModal() == IDOK) {
		if (LoadFile(dlg.GetPathName())) {
			Invalidate();
			return;
		}
		else {
			MessageBox(_T("Failed to open file"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
		}
	}
}

void CChildView::OnPaint()
{
	if (m_opened) {
		CPaintDC dc(this); // device context for painting
		CRect rect;
		GetClientRect(&rect);
		if (m_origin_init) {
			m_width = rect.Width();
			m_height = rect.Height();
			m_origin_init = false;
		}
		dc.SetMapMode(MM_ISOTROPIC);
		// two functions for handling zoom feature
		dc.SetWindowExt(((m_max_bound[0] - m_min_bound[0]) * m_precision*m_current_zoom), -((m_max_bound[1] - m_min_bound[1]) * m_precision*m_current_zoom));
		dc.SetViewportExt(m_width, m_height);
		// intial shape extents in device coordinates
		POINT shape_ext = { (m_max_bound[0] - m_min_bound[0]) *m_precision*m_current_zoom, (m_max_bound[1] - m_min_bound[1]) *m_precision*m_current_zoom };
		LPtoDP(dc, &shape_ext, 1);
		// adjust zoom_shift
		if (m_zoom_change_tracker != m_current_zoom) {
			m_zoom_change_tracker = m_current_zoom;
			m_logical_org_shift.x = round((m_prev_zoom / m_current_zoom) *(m_org_shift.x - m_prev_shift.x));
			m_logical_org_shift.y = round((m_prev_zoom / m_current_zoom) *(m_org_shift.y - m_prev_shift.y));
			// distance of center of client area to current logical origin in pixels
			int center_to_origin_x = (rect.Width() / 2 - (m_width / 2 - shape_ext.x / 2 - m_zoom_shift.x + (1 / (m_prev_zoom / m_current_zoom - 1))*m_logical_org_shift.x));
			int center_to_origin_y = (rect.Height() / 2 - (m_height / 2 - shape_ext.y / 2 - m_zoom_shift.y + (1 / (m_prev_zoom / m_current_zoom - 1))*m_logical_org_shift.y));
			// translation of logical origin to preserve client area postion of logical point currently in center of client area
			m_zoom_shift.x += round((m_prev_zoom / m_current_zoom - 1)*center_to_origin_x);
			m_zoom_shift.y += round((m_prev_zoom / m_current_zoom - 1)*center_to_origin_y);
			m_prev_shift = m_org_shift;
			m_origin = { m_width / 2 - shape_ext.x / 2 - m_zoom_shift.x, m_height / 2 - shape_ext.y / 2 - m_zoom_shift.y };

		}
		// for handling panning when no zoom change occured
		else {
			m_origin = { m_width / 2 - shape_ext.x / 2 - m_zoom_shift.x + (m_org_shift.x - m_prev_shift.x), m_height / 2 - shape_ext.y / 2 - m_zoom_shift.y + (m_org_shift.y - m_prev_shift.y) };
		}
		dc.SetViewportOrg(m_origin);
		// loop handling drawing of shape entities in m_vec_shapes
		for (const auto& x : m_vec_shapes) {
			DrawShape(dc, x);
		}

	}
}


BOOL CChildView::LoadFile(LPCTSTR pfile) {
	ShapeOpen Open(CStringA(pfile), "rb"); 
	if (Open.GetState() == FALSE) return FALSE;
	SHPGetInfo(Open.GetSHPHandle(), &m_entities,	// load info from shp file
		&m_shape_type, m_min_bound, m_max_bound);
	for (int i = 0; i < m_entities; ++i) {
		m_vec_shapes.push_back(Shape(Open.GetSHPHandle(), i));
	}
	m_opened = true;
	return TRUE;
}


void CChildView::DrawShape(CPaintDC& dc, const Shape& shape) const {
	// draw shapes by part, handle the special case when number of parts is less than 2
	if (shape.Get()->nParts > 1) {
		for (auto i = 0; i < shape.Get()->nParts - 1; ++i) {
			dc.MoveTo((shape.Get()->padfX[shape.Get()->panPartStart[i]] - m_min_bound[0])*m_precision, 
			(shape.Get()->padfY[shape.Get()->panPartStart[i]] - m_min_bound[1])*m_precision);
			for (auto j = shape.Get()->panPartStart[i] + 1; (j < shape.Get()->panPartStart[i + 1]); ++j) {
				dc.LineTo((shape.Get()->padfX[j] - m_min_bound[0])*m_precision, 
				(shape.Get()->padfY[j] - m_min_bound[1])*m_precision);
			}
		}
		//handle the special case of drawing the last part of an entity
		dc.MoveTo((shape.Get()->padfX[shape.Get()->panPartStart[shape.Get()->nParts - 1]] - m_min_bound[0])*m_precision, 
		(shape.Get()->padfY[shape.Get()->panPartStart[shape.Get()->nParts - 1]] - m_min_bound[1])*m_precision);
		for (auto i = shape.Get()->panPartStart[shape.Get()->nParts - 1] + 1; i < shape.Get()->nVertices; ++i) {
			dc.LineTo((shape.Get()->padfX[i] - m_min_bound[0])*m_precision, 
			(shape.Get()->padfY[i] - m_min_bound[1])*m_precision);
		}
	}
	//handle drawing entities with single part
	else {
		dc.MoveTo((shape.Get()->padfX[0] - m_min_bound[0])*m_precision, (shape.Get()->padfY[0] - m_min_bound[1])*m_precision);
		for (auto i = 1; i < shape.Get()->nVertices; ++i) {
			dc.LineTo((shape.Get()->padfX[i] - m_min_bound[0])*m_precision, (shape.Get()->padfY[i] - m_min_bound[1])*m_precision);
		}
	}
}


void CChildView::OnRevertZoom()
{
	swap(m_current_zoom, m_prev_zoom);
	Invalidate();
}


void CChildView::OnUpdateRevertZoomUI(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CChildView::OnZoomOut()
{
	m_prev_zoom = m_current_zoom;
	m_current_zoom *= constants::zoom_out_multiplier;
	// cut-off for out of range values
	if (m_current_zoom > 1 / constants::zoom_min) {
		m_current_zoom = 1 / constants::zoom_min;
	}
	Invalidate();
}


void CChildView::OnZoomIn()
{
	m_prev_zoom = m_current_zoom;
	m_current_zoom *= constants::zoom_in_multiplier;
	// cut-off for out of range values
	if (m_current_zoom < 1 / constants::zoom_max) {
		m_current_zoom = 1 / constants::zoom_max;
	}
	Invalidate();
}


void CChildView::OnSelectZoom(UINT ID)
{
	if (ID != ID_ZOOM_OTHER) {
		m_prev_zoom = m_current_zoom;
		m_current_zoom = constants::zoom_min*pow(2, ID - ID_ZOOM_1);
	}
	else {
		m_zoom_other_trail = true;
		ZoomDialog dlg;
		int nReturn = dlg.DoModal();
		if (nReturn == IDOK) {
			m_current_zoom = 1 / (ZoomDialog::m_zoom_percent / 100);
		}
	}
	Invalidate();
}


void CChildView::OnUpdateSelectZoomUI(CCmdUI *pCmdUI)
{
	// code handling radio mark switching
	pCmdUI->SetRadio((constants::zoom_min*pow(2, pCmdUI->m_nID - ID_ZOOM_1) ==
		m_current_zoom) && !m_zoom_other_trail);
	if (pCmdUI->m_nID == ID_ZOOM_OTHER) {
		pCmdUI->SetRadio(m_zoom_other_trail);
		m_zoom_other_trail = false;
	}
}

void CChildView::OnZoomOther()
{


}


void CChildView::OnUpdateZoomOtherUI(CCmdUI *pCmdUI)
{

}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_point_from = point;
	m_point_to = point;
	m_tracking = true;
	SetCapture();
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_tracking) {
		// area selection border 
		if (ZoomDialog::m_select_area_trace) {
			// invert color values, erase previous border
			CClientDC dc(this);
			InvertRectBorder(&dc, m_point_from, m_point_to);
			InvertRectBorder(&dc, m_point_from, point);

		}
		else {
			// code controlling vieportorg shift when panning
			m_org_shift.x = m_org_shift.x + (point.x - m_point_from.x);
			m_org_shift.y = m_org_shift.y + (point.y - m_point_from.y);
			m_point_from = point;
			Invalidate();
		}
		m_point_to = point;
	}
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_tracking) {
		m_tracking = false;
		if (GetCapture() == this)
			::ReleaseCapture();
		if (ZoomDialog::m_select_area_trace) {
			CClientDC dc(this);
			
			InvertRectBorder(&dc, m_point_from, m_point_to);
			CRect rect1;
			GetClientRect(&rect1);
			CRect rect2{ m_point_from, point };
			// select area shift
			m_org_shift.x = m_org_shift.x - (point.x + m_point_from.x) / 2 + (rect1.Width() / 2);
			m_org_shift.y = m_org_shift.y - (point.y + m_point_from.y) / 2 + (rect1.Height() / 2);
			// code setting zoom behaviour of select area feature
			// currently works only in 1:1 scale setting
			if (rect2.Height() >= rect2.Width()) {
				m_current_zoom *= (rect2.Height() + 0.0) / (rect1.Height() + 0.0);
			}
			else {
				m_current_zoom *= (rect2.Width() + 0.0) / (rect1.Width() + 0.0);
			}
			Invalidate();
		}
	}
	if (m_tracking) {
		m_tracking = false;
		// check if view window owns the capture
		if (GetCapture() == this)
			::ReleaseCapture();
		if (ZoomDialog::m_select_area_trace) {
			CClientDC dc(this);
			// erase border
			InvertRectBorder(&dc, m_point_from, m_point_to);
			CRect rect1;
			GetClientRect(&rect1);
			CRect rect2{ m_point_from, point };
			// translate center of selected area to the center of client area
			m_org_shift.x = m_org_shift.x - ((point.x + m_point_from.x) / 2 - (rect1.Width() / 2));
			m_org_shift.y = m_org_shift.y - ((point.y + m_point_from.y) / 2 - (rect1.Height() / 2));
			m_prev_zoom = m_current_zoom;
			// code setting zoom behaviour of select area feature
			if (rect2.Height() >= rect2.Width()) {
				m_current_zoom *= (abs(rect2.Height()) + 0.0) / (rect1.Height() + 0.0);
			}
			else {
				m_current_zoom *= (abs(rect2.Width()) + 0.0) / (rect1.Width() + 0.0);
			}
			Invalidate();
		}
	}
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// zoom out/in by mouse wheel
	m_prev_zoom = m_current_zoom;
	if (zDelta > 0) {
		m_current_zoom *= constants::zoom_in_multiplier;
		// out of range cut-off
		if (m_current_zoom < 1 / constants::zoom_max) {
			m_current_zoom = 1 / constants::zoom_max;
		}
	}
	else {
		m_current_zoom *= constants::zoom_out_multiplier;
		// out of range cut-off
		if (m_current_zoom > 1 / constants::zoom_min) {
			m_current_zoom = 1 / constants::zoom_min;
		}
	}
	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
