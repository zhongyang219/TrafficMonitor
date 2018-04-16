// CColorStatic
//派生自CStatic，用于显示颜色的Static控件
#pragma once
#define WM_STATIC_CLICKED (WM_USER + 1001)

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	void SetFillColor(COLORREF fill_color);		//设置要填充单一的背景色
	void SetColorNum(int color_num);			//设置颜色的数量
	void SetFillColor(int index, COLORREF fill_color);
	void SetLinkCursor(bool link_cursor = true);		//设置指向控件时光标变成超链接形状

protected:
	//COLORREF m_fill_color{ RGB(255, 255,255) };
	vector<COLORREF> m_colors;
	int m_color_num;
	bool m_hover{ false };		//当鼠标指向控件时为true
	bool m_link_cursor{ false };	//是否启用超链接形状的光标

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


