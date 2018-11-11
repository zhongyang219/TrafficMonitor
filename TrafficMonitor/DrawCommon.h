//封装的绘图类
#pragma once
#include "CommonData.h"
class CDrawCommon
{
public:

	//拉伸模式
	enum class StretchMode
	{
		STRETCH,		//拉伸，会改变比例
		FILL,			//填充，不改变比例，会裁剪长边
		FIT			//适应，不会改变比例，不裁剪
	};

	CDrawCommon();
	~CDrawCommon();

	void Create(CDC* pDC, CWnd* pMainWnd);
	void SetFont(CFont* pfont);		//设置绘制文本的字体
	void SetDC(CDC* pDC);		//设置绘图的DC
	CDC* GetDC() { return m_pDC; }
	void SetBackColor(COLORREF back_color) { m_back_color = back_color; }

	void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false);	//在指定的矩形区域内绘制文本

	void SetDrawRect(CRect rect);		//设置绘图剪辑区域
	static void SetDrawRect(CDC* pDC, CRect rect);

	//绘制一个位图
	//（注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
	//需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
	void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
	void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
	void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);

	//将图片拉伸到指定尺寸(https://blog.csdn.net/sichuanpb/article/details/22986877)
	static void BitmapStretch(CImage *pImage, CImage *ResultImage, CSize size);

	void FillRect(CRect rect, COLORREF color);		//用纯色填充矩形
	void FillRectWithBackColor(CRect rect);			//使用背景色填充矩形
	void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false);	//绘制矩形边框。如果dot_line为true，则为虚线

	//从图像创建区域，如果像素点的亮度小于threshold（取值为0~255，0为黑色，255为白色），则该像素点在区域外
	//https://blog.csdn.net/tajon1226/article/details/6589180
	static void GetRegionFromImage(CRgn& rgn, CBitmap &cBitmap, int threshold);

private:
	CDC* m_pDC{};		//用于绘图的CDC类的指针
	CWnd* m_pMainWnd{};	//绘图窗口的句柄
	CFont* m_pfont{};
	COLORREF m_back_color{};

	static int GetColorBritness(COLORREF color);
};

