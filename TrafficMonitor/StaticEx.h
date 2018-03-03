//CStatic类的派生类，用于实现设置Static控件文字颜色，
//以及解决设置了窗口背景图片时控件文字重叠的问题。
#pragma once
#include "afxwin.h"
#define WM_LINK_CLICKED (WM_USER + 1002)
class CStaticEx :
	public CStatic
{
public:
	CStaticEx();
	CStaticEx(bool is_hyperlink);		//构造函数，设置文件控件文本是否为超链接
	~CStaticEx();

	void SetWindowTextEx(LPCTSTR lpszString);	//为控件设置有颜色的文本（需要配合SetTextColor使用）
	void SetTextColor(COLORREF textColor);		//设置控件文本颜色
	CString GetString() const;			//获取控件文本
	void SetURL(CString strURL);		//设置超链接
	CString GetURL() const;			//获取超链接字符串

	void SetLinkEnable(bool enable) { m_linkEnable = enable; }

	void SetFillColor(COLORREF fill_color);		//设置要填充的背景色

protected:
	COLORREF m_TextColor;	//控件文字颜色
	CString m_text;			//控件上的文本

	bool m_isHyperLink{ false };	//如果要将控件作为超链接，则为true
	bool m_linkEnable{ true };		//如果为true，点击后打开超链接，否则向父窗口发送一个点击消息
	bool m_bHot{ false };			//当鼠标指向超链接时，则为true
	CString m_strURL;				//超链接字符串

	bool m_fill_color_enable{ false };		//是否为控件填充颜色
	COLORREF m_fill_color{};

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void PreSubclassWindow();
};

