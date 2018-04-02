//封装的绘图类
#pragma once
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

	void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, bool center);	//在指定的矩形区域内绘制文本

	void SetDrawRect(CRect rect);		//设置绘图剪辑区域

	//绘制一个位图
	//（注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
	//需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
	void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
	void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
	void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);

	//将图片拉伸到指定尺寸(https://blog.csdn.net/sichuanpb/article/details/22986877)
	static void BitmapStretch(CImage *pImage, CImage *ResultImage, CSize size);

	void FillRect(CRect rect, COLORREF color);		//用纯色填充矩形

private:
	CDC* m_pDC{};		//用于绘图的CDC类的指针
	CWnd* m_pMainWnd{};	//绘图窗口的句柄
	CFont* m_pfont{};
};

