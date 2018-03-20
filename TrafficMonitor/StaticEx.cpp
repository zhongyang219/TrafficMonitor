#include "stdafx.h"
#include "StaticEx.h"


CStaticEx::CStaticEx()
{
}

CStaticEx::CStaticEx(bool is_hyperlink)
{
	m_isHyperLink = is_hyperlink;
}


CStaticEx::~CStaticEx()
{
}

void CStaticEx::SetWindowTextEx(LPCTSTR lpszString)
{
	m_text = lpszString;
	m_color_text = true;
	Invalidate();
}

void CStaticEx::SetTextColor(COLORREF textColor)
{
	m_TextColor = textColor;
	SetWindowTextEx(m_text);
}

CString CStaticEx::GetString() const
{
	return m_text;
}

void CStaticEx::SetURL(CString strURL)
{
	m_strURL = strURL;
}

CString CStaticEx::GetURL() const
{
	return m_strURL;
}

void CStaticEx::SetFillColor(COLORREF fill_color)
{
	m_fill_color = fill_color;
	m_fill_color_enable = true;
	Invalidate();
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (message == WM_SETTEXT)
	{
		CRect rect;
		CDC* pDC = GetDC();
		GetClientRect(rect);
		DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);
		ReleaseDC(pDC);
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


void CStaticEx::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_isHyperLink)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		_TrackMouseEvent(&tme);
	}
	else
	{
		CStatic::OnMouseMove(nFlags, point);
	}
}


void CStaticEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_isHyperLink)
	{
		if (m_linkEnable)
		{
			if (m_strURL.IsEmpty())
				m_strURL = m_text;
			ShellExecute(NULL, _T("open"), m_strURL, NULL, NULL, SW_SHOW);	//�򿪳�����
		}
		else
		{
			CWnd* pParent{ GetParent() };
			if(pParent!=nullptr)
				pParent->SendMessage(WM_LINK_CLICKED, (WPARAM)this);
		}
	}
	else
	{
		CStatic::OnLButtonUp(nFlags, point);
	}
}


void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	//�ǳ�����ʱ�Ļ�ͼ����
	if (m_isHyperLink)
	{
		CFont* pFont = GetFont();
		CFont m_Font;
		if (pFont != NULL)
		{
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			lf.lfUnderline = m_bHot;
			if (m_Font.CreateFontIndirect(&lf))
				dc.SelectObject(m_Font);
		}
		dc.SetTextColor(RGB(0, 0, 255));
		dc.SetBkMode(TRANSPARENT);
		///׼������
		CRect rect;
		CBrush BGBrush, *pOldBrush;
		this->GetClientRect(&rect);
		//������
		COLORREF backColor = GetSysColor(COLOR_BTNFACE);
		BGBrush.CreateSolidBrush(backColor);
		pOldBrush = dc.SelectObject(&BGBrush);
		dc.FillRect(&rect, &BGBrush);
		dc.SelectObject(pOldBrush);
		BGBrush.DeleteObject();
		///�������
		if (m_text.GetLength()>0)
		{
			dc.DrawText(m_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		m_Font.DeleteObject();
	}

	//��Ҫ��䱳��ɫʱ�Ļ�ͼ����
	else if (m_fill_color_enable)
	{
		CRect rect;
		GetClientRect(rect);
		dc.FillSolidRect(rect, m_fill_color);
	}

	else if (m_color_text)
	{
		dc.SetTextColor(m_TextColor);
		dc.SetBkMode(TRANSPARENT);
		dc.SelectObject(this->GetFont());
		CRect rect;
		this->GetClientRect(&rect);
		DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//�ػ�ؼ������Խ�������ص�������
		dc.DrawText(m_text, rect, DT_VCENTER | DT_SINGLELINE);
	}
}


void CStaticEx::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_isHyperLink && !m_bHot)
	{
		m_bHot = true;
		Invalidate();
	}
	else
	{
		CStatic::OnMouseHover(nFlags, point);
	}
}


void CStaticEx::OnMouseLeave()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_isHyperLink)
	{
		m_bHot = false;
		Invalidate();
	}
	else
	{
		CStatic::OnMouseLeave();
	}
}


BOOL CStaticEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_isHyperLink && m_bHot)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CStaticEx::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	if (m_isHyperLink)
	{
		DWORD dwStyle = GetStyle();
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	}
	GetWindowText(m_text);

	CStatic::PreSubclassWindow();
}
