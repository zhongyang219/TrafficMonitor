#include "stdafx.h"
#include "PictureStatic.h"


CPictureStatic::CPictureStatic()
{
}


CPictureStatic::~CPictureStatic()
{
	m_memDC.DeleteDC();
}

void CPictureStatic::SetPicture(UINT pic_id)
{
	m_bitmap.LoadBitmap(pic_id);		//载入位图
	CDC* pDC = GetDC();
	//获取图像实际大小
	GetObject(m_bitmap, sizeof(BITMAP), &m_bm);
	m_memDC.CreateCompatibleDC(pDC);
	m_memDC.SelectObject(&m_bitmap);
	//获取控件大小
	GetClientRect(m_rect);

}

BEGIN_MESSAGE_MAP(CPictureStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CPictureStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	if (m_bitmap.m_hObject != NULL)
	{
		// 以下两行避免图片失真
		dc.SetStretchBltMode(HALFTONE);
		dc.SetBrushOrg(0, 0);
		//绘制将内存DC中的图像
		dc.StretchBlt(0, 0, m_rect.Width(), m_rect.Height(), &m_memDC, 0, 0, m_bm.bmWidth, m_bm.bmHeight, SRCCOPY);
	}
}
