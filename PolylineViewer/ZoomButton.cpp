// ZoomButton.cpp : implementation file
//

#include "stdafx.h"
#include "PolylineViewer.h"
#include "ZoomButton.h"


// ZoomButton

bool ZoomButton::m_select_area_trace = false;	// flag for checking select area button state (toggle button)

IMPLEMENT_DYNAMIC(ZoomButton, CButton)

ZoomButton::ZoomButton()
{
}

ZoomButton::~ZoomButton()
{
}


BEGIN_MESSAGE_MAP(ZoomButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &ZoomButton::OnBnClicked)
END_MESSAGE_MAP()



// ZoomButton message handlers

// toggle behaviour 

void ZoomButton::OnBnClicked()
{
	m_select_area_trace = !m_select_area_trace;
}


