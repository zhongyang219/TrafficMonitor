/*继承于CStatic类，用作超链接：
调用SetURL函数设置超链接；
调用SetLinkIsURL函数设置点击控件后是打开超链接还是响应命令
如果调用SetLinkIsURL(false)，则点击控件后会向父窗口发送一个WM_LINK_CLICKED消息，
并通过WPARAM传递控件的指针。
否则，点击控件后打开超链接
*/
#pragma once

#define WM_LINK_CLICKED (WM_USER + 1002)

// CLinkStatic

class CLinkStatic : public CStatic
{
	DECLARE_DYNAMIC(CLinkStatic)

public:
	CLinkStatic();
	virtual ~CLinkStatic();
    void SetBackgroundColor(COLORREF background_color);

public:
	void SetURL(CString strURL);		//设置超链接
	CString GetURL() const;			//获取超链接字符串
	void SetLinkIsURL(bool enable) { m_link_is_url = enable; }
protected:
	bool m_link_is_url{ true };		//如果为true，点击后打开超链接，否则向父窗口发送一个点击消息
	bool m_bHot{ false };			//当鼠标指向超链接时，则为true
	CString m_strURL;				//超链接字符串
    COLORREF m_back_color{ GetSysColor(COLOR_BTNFACE) };
    CFont m_hover_font;
    bool IsLinkValid() const;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
};


