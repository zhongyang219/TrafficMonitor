#include "stdafx.h"
#include "ListCtrlEx.h"

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
}


CListCtrlEx::~CListCtrlEx()
{
}
void CListCtrlEx::SetDrawItemRangeData(int item, double range, COLORREF color)
{
	if (item < 0) return;
	if (item >= m_item_rage_data.size())
		m_item_rage_data.resize(item + 1);
	m_item_rage_data[item].data_value = range;
	m_item_rage_data[item].color = color;
}

void CListCtrlEx::SetDrawItemRangInLogScale(bool log_scale)
{
	m_use_log_scale = log_scale;
	Invalidate();
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
END_MESSAGE_MAP()


void CListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_draw_item_range)
	{
		*pResult = CDRF_DODEFAULT;
		LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
		NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
		switch (lplvdr->nmcd.dwDrawStage)	//�ж�״̬   
		{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:			//���Ϊ��ITEM֮ǰ��Ҫ������ɫ�ĸı�
			if (nmcd.dwItemSpec >= 0 && nmcd.dwItemSpec < m_item_rage_data.size())
			{
				double range = m_item_rage_data[nmcd.dwItemSpec].data_value;
				CDC* pDC = CDC::FromHandle(nmcd.hdc);		//��ȡ��ͼDC
				CRect item_rect, draw_rect;
				GetSubItemRect(nmcd.dwItemSpec,m_draw_item_range_row, LVIR_BOUNDS, item_rect);	//��ȡ��ͼ��Ԫ��ľ�������
				CCommon::SetDrawArea(pDC, item_rect);		//���û�ͼ����Ϊ��ǰ��
				draw_rect = item_rect;
				if (draw_rect.Height() > 2 * m_margin)
				{
					draw_rect.top += m_margin;
					draw_rect.bottom -= m_margin;
				}
				int width;
				if (m_use_log_scale)	//ʹ�ö���������y=ln(x+1)��
				{
					range = std::log(range + 1);
					width = static_cast<int>(range*draw_rect.Width() / std::log(1000 + 1));
				}
				else		//ʹ�����Ա�����y=x��
				{
					width = static_cast<int>(range*draw_rect.Width() / 1000);
				}
				draw_rect.right = draw_rect.left + width;
				pDC->FillSolidRect(draw_rect, m_item_rage_data[nmcd.dwItemSpec].color);

				//��ǰ�л�����ɺ󽫻�ͼ��������Ϊ��ߵ����򣬷�ֹ��ǰ�е����򱻸���
				CRect rect1{ item_rect };
				rect1.left = 0;
				rect1.right = item_rect.left;
				CCommon::SetDrawArea(pDC, rect1);
			}
			*pResult = CDRF_DODEFAULT;
			break;
		}
	}
}
