/*
#define FUNCTION_TYPE 3
CStatic类的派生类，可实现以下FUNCTION_TYPE种功能
1、用作带颜色的文本控件：
调用SetTextColor设置文本颜色；
在需要的时候调用SetWindowTextEx设置控件文本

2、用作超链接：
构造时将构造函数参数设为true；
调用SetURL函数设置超链接；
调用SetLinkEnable函数设置点击控件后是打开超链接还是响应命令
如果调用SetLinkEnable(false)，则点击控件后会向父窗口发送一个WM_LINK_CLICKED消息，
并通过WPARAM传递控件的指针

3、用作颜色填充：
使用SetFillColor()函数设置要填充的颜色

以上FUNCTION_TYPE种功能不能同时使用
*/
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

	//将Static控件用作绘制有颜色的文本时
public:
	void SetWindowTextEx(LPCTSTR lpszString);	//为控件设置有颜色的文本（需要配合SetTextColor使用）
	void SetTextColor(COLORREF textColor);		//设置控件文本颜色
	CString GetString() const;			//获取控件文本

protected:
	bool m_color_text{ false };
	COLORREF m_TextColor;	//控件文字颜色
	CString m_text;			//控件上的文本

	//将Static控件用作超链接时
public:
	void SetURL(CString strURL);		//设置超链接
	CString GetURL() const;			//获取超链接字符串
	void SetLinkEnable(bool enable) { m_linkEnable = enable; }
protected:
	bool m_isHyperLink{ false };	//如果要将控件作为超链接，则为true
	bool m_linkEnable{ true };		//如果为true，点击后打开超链接，否则向父窗口发送一个点击消息
	bool m_bHot{ false };			//当鼠标指向超链接时，则为true
	CString m_strURL;				//超链接字符串

	//将Static控件用作颜色填充时
public:
	void SetFillColor(COLORREF fill_color);		//设置要填充的背景色
protected:
	bool m_fill_color_enable{ false };		//是否为控件填充颜色
	COLORREF m_fill_color{};

protected:
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

