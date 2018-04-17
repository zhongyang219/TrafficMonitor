/*
CStatic类的派生类，用作带颜色的文本控件：
调用SetTextColor设置文本颜色；
在需要的时候调用SetWindowTextEx设置控件文本
*/
#pragma once
#include "afxwin.h"
#include "CommonData.h"
class CStaticEx :
	public CStatic
{
public:
	CStaticEx();
	~CStaticEx();

	//将Static控件用作绘制有颜色的文本时
public:
	void SetWindowTextEx(LPCTSTR lpszString, Alignment align = Alignment::LEFT);	//为控件设置有颜色的文本（需要配合SetTextColor使用）
	void SetTextColor(COLORREF textColor);		//设置控件文本颜色
	void SetBackColor(COLORREF back_color);		//设置控件背景颜色
	CString GetString() const;			//获取控件文本

protected:
	bool m_color_text{ false };
	COLORREF m_text_color;	//控件文字颜色
	COLORREF m_back_color;
	CString m_text;			//控件上的文本
	Alignment m_align{};		//文本的对齐方式
	bool m_draw_background_color{ false };	//是否需要为控件填充背景颜色

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
};

