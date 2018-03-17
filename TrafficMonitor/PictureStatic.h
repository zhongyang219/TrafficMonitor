/*一个用于显示图片的静态控件
在需要的时候调用SetPicture()函数设置静态图片。
控件在重绘时会向父窗口发送WM_CONTROL_REPAINT消息，
并通过wParam传递当前控件的CWnd指针，通过lParam传递CDC的指针
*/
#pragma once
#define WM_CONTROL_REPAINT (WM_USER + 1003)		//控件重绘消息

class CPictureStatic : public CStatic
{
public:
	CPictureStatic();
	~CPictureStatic();

	void SetPicture(UINT pic_id);
	void SetPicture(HBITMAP hBitmap);

protected:
	CDC m_memDC;
	CBitmap m_bitmap;
	CRect m_rect;
	BITMAP m_bm;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

