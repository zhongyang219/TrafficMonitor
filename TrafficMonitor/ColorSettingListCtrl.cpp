#include "stdafx.h"
#include "ColorSettingListCtrl.h"

IMPLEMENT_DYNAMIC(CColorSettingListCtrl, CListCtrl)

CColorSettingListCtrl::CColorSettingListCtrl()
{
}


CColorSettingListCtrl::~CColorSettingListCtrl()
{
}

void CColorSettingListCtrl::SetItemColor(int row, int col, COLORREF color)
{
    m_colors[row][col] = color;
}


COLORREF CColorSettingListCtrl::GetItemColor(int row, int col)
{
    return m_colors[row][col];
}

BEGIN_MESSAGE_MAP(CColorSettingListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CColorSettingListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()


void CColorSettingListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = CDRF_DODEFAULT;
	LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
	switch (lplvdr->nmcd.dwDrawStage)	//判断状态   
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:			//如果为画ITEM之前就要进行颜色的改变
		if (nmcd.dwItemSpec >= 0 && nmcd.dwItemSpec < GetItemCount())
		{
			//double range = m_item_rage_data[nmcd.dwItemSpec].data_value;
			//CDC* pDC = CDC::FromHandle(nmcd.hdc);		//获取绘图DC
			//CRect item_rect, draw_rect;
			//GetSubItemRect(nmcd.dwItemSpec,m_draw_item_range_row, LVIR_BOUNDS, item_rect);	//获取绘图单元格的矩形区域
			//CDrawCommon::SetDrawRect(pDC, item_rect);		//设置绘图区域为当前列
			//draw_rect = item_rect;
			//if (draw_rect.Height() > 2 * m_margin)
			//{
			//	draw_rect.top += m_margin;
			//	draw_rect.bottom -= m_margin;
			//}
			//int width;
			//if (m_use_log_scale)	//使用对数比例（y=ln(x+1)）
			//{
			//	range = std::log(range + 1);
			//	width = static_cast<int>(range*draw_rect.Width() / std::log(1000 + 1));
			//}
			//else		//使用线性比例（y=x）
			//{
			//	width = static_cast<int>(range*draw_rect.Width() / 1000);
			//}
			//draw_rect.right = draw_rect.left + width;
			//pDC->FillSolidRect(draw_rect, m_item_rage_data[nmcd.dwItemSpec].color);

			////当前列绘制完成后将绘图区域设置为左边的区域，防止当前列的区域被覆盖
			//CRect rect1{ item_rect };
			//rect1.left = 0;
			//rect1.right = item_rect.left;
			//CDrawCommon::SetDrawRect(pDC, rect1);

            CDC* pDC = CDC::FromHandle(nmcd.hdc);		//获取绘图DC
            CRect item_rect;
            auto& col_color_map = m_colors[nmcd.dwItemSpec];
            for (auto iter = col_color_map.begin(); iter != col_color_map.end(); ++iter)
            {
                GetSubItemRect(nmcd.dwItemSpec, iter->first, LVIR_BOUNDS, item_rect);	//获取绘图单元格的矩形区域
                //设置绘图剪辑区域
                CDrawCommon::SetDrawRect(pDC, item_rect);
                //使用双缓冲绘图
                CDrawDoubleBuffer draw_double_buffer(pDC, item_rect);
                //填充背景
                item_rect.MoveToXY(0, 0);
                draw_double_buffer.GetMemDC()->FillSolidRect(item_rect, GetSysColor(COLOR_WINDOW));
                //绘制颜色矩形
                item_rect.DeflateRect(m_margin, m_margin);
                draw_double_buffer.GetMemDC()->FillSolidRect(item_rect, iter->second);
                //绘制矩形边框
                CDrawCommon drawer;
                drawer.Create(draw_double_buffer.GetMemDC(), this);
                drawer.DrawRectOutLine(item_rect, RGB(192, 192, 192));
            }
            //当前列绘制完成后将绘图区域设置为第一列的区域，防止颜色列的区域被覆盖
            CRect rect1{};
            GetSubItemRect(nmcd.dwItemSpec, 1, LVIR_BOUNDS, rect1);	//获取第1列单元格的矩形区域
            rect1.right = rect1.left;
            rect1.left = 0;
            CDrawCommon::SetDrawRect(pDC, rect1);

		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}
