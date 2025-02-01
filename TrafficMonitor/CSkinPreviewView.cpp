// CSkinPreviewView.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "CSkinPreviewView.h"


// CSkinPreviewView

IMPLEMENT_DYNCREATE(CSkinPreviewView, CScrollView)

CSkinPreviewView::CSkinPreviewView()
{

}

CSkinPreviewView::~CSkinPreviewView()
{
}


BEGIN_MESSAGE_MAP(CSkinPreviewView, CScrollView)
END_MESSAGE_MAP()


// CSkinPreviewView 绘图

void CSkinPreviewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	m_size.cx = 0;
	m_size.cy = 0;
	SetScrollSizes(MM_TEXT, m_size);


}

void CSkinPreviewView::OnDraw(CDC* pDC)
{
    CDrawCommon drawer;
    drawer.Create(pDC, nullptr);
    CRect view_rect;
    GetClientRect(view_rect);
    view_rect.right = (std::max)(view_rect.Width(), static_cast<int>(m_size.cx));
    view_rect.bottom = (std::max)(view_rect.Height(), static_cast<int>(m_size.cy));

    //如果皮肤是png格式，绘制10x10的棋盘背景
    if (m_skin_data->IsPNG())
    {
        int grid_size = theApp.DPI(10);

        // 检查画布大小
        int rows = view_rect.Height() / grid_size + 1;  // 行数
        int cols = view_rect.Width() / grid_size + 1;  // 列数

        // 遍历每个网格
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                // 计算当前网格的矩形区域
                CRect rect(col * grid_size, row * grid_size,
                    (col + 1) * grid_size, (row + 1) * grid_size);

                // 判断当前网格颜色（交替填充）
                COLORREF color = ((row + col) % 2 == 0) ? RGB(204, 204, 204) : RGB(254, 254, 254);

                // 填充矩形
                drawer.FillRect(rect, color);
            }
        }
    }
    //绘制纯色背景
    else
    {
        drawer.FillRect(view_rect, GetSysColor(COLOR_WINDOW));
    }

    //绘制预览图
    CRect draw_rect(CPoint(0, 0), m_size);
    m_skin_data->DrawPreview(pDC, draw_rect);
}


// CSkinPreviewView 诊断

//#ifdef _DEBUG
//void CSkinPreviewView::AssertValid() const
//{
//	CScrollView::AssertValid();
//}

//#ifndef _WIN32_WCE
//void CSkinPreviewView::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}
//#endif
//#endif //_DEBUG

void CSkinPreviewView::InitialUpdate()
{
	OnInitialUpdate();
}
void CSkinPreviewView::SetSize(int width, int hight)
{
	m_size = CSize(width, hight);
	SetScrollSizes(MM_TEXT, m_size);
}

// CSkinPreviewView 消息处理程序
