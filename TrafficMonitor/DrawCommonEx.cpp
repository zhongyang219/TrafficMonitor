#include "stdafx.h"
#include "DrawCommonEx.h"


CDrawCommonEx::CDrawCommonEx(CDC* pDC)
{
	m_pDC = pDC;
	hThm = OpenThemeData(::GetDesktopWindow(), L"Window");
}


CDrawCommonEx::~CDrawCommonEx()
{
	CloseThemeData(hThm);
}

void CDrawCommonEx::DrawWindowText(CRect rect, LPCTSTR str, COLORREF color)
{
	m_pDC->SelectObject(m_pFont);
	m_pDC->SetTextColor(color);

	DTTOPTS dttopts{};
	dttopts.dwSize = sizeof(DTTOPTS);

	dttopts.dwFlags = DTT_GLOWSIZE | DTT_COMPOSITED; //设置选项 
	dttopts.iGlowSize = 0; //发光的范围大小 
	HRESULT hr = DrawThemeTextEx(hThm, m_pDC->GetSafeHdc(), TEXT_LABEL, 0, str, -1, DT_LEFT | DT_SINGLELINE, rect, &dttopts);
}

void CDrawCommonEx::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}
