#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
class CColorSettingListCtrl :
	public CListCtrl
{
DECLARE_DYNAMIC(CColorSettingListCtrl)
public:
	CColorSettingListCtrl();
	~CColorSettingListCtrl();

    void SetItemColor(int row, int col, COLORREF color);
    COLORREF GetItemColor(int row, int col);
	void SetDrawItemRangMargin(int margin) { m_margin = margin; }			//设置绘制绘制的矩形的边缘到单元格边框的矩形，此值越大则绘制的矩形越细，但是不能超过列表行距的一半

protected:
	int m_margin{};
	std::map<int, std::map<int, COLORREF>> m_colors;	//用于保存每一行每一列的颜色

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

