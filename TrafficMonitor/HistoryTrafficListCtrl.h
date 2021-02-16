#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
class CHistoryTrafficListCtrl :
	public CListCtrl
{
DECLARE_DYNAMIC(CHistoryTrafficListCtrl)
public:
	CHistoryTrafficListCtrl();
	~CHistoryTrafficListCtrl();

	void EnableDrawItemRange(bool draw = true) { m_draw_item_range = draw; }	//设置是否要在某一列单元格中绘制表示数值大小的矩形
	void SetDrawItemRangeRow(int row) { m_draw_item_range_row = row; }			//设置要绘制表示数值大小的矩形的列，注意此列必须为最右边的列，否则，此列右边的所有列将无法显示
	void SetDrawItemRangeData(int item, double range, COLORREF color);			//设置某一行的项目数据大小（取值为1~1000）和颜色
	void SetDrawItemRangMargin(int margin) { m_margin = margin; }			//设置绘制绘制的矩形的边缘到单元格边框的矩形，此值越大则绘制的矩形越细，但是不能超过列表行距的一半
	void SetDrawItemRangInLogScale(bool log_scale);				//设置要绘制表示数值大小的矩形时是否使用对数比例，否则使用线性比例

protected:
	struct ItemData
	{
		double data_value;		//要绘制的矩形表示的数值大小，范围为0~1000
		COLORREF color;		//要绘制的矩形的颜色
	};
	bool m_draw_item_range{ false };		//是否需要在某一列单元格中绘制表示数值大小的矩形
	int m_draw_item_range_row{};		//需要绘制表示数值大小的矩形的列
	int m_margin{};
	vector<ItemData> m_item_rage_data;	//用于表示每一项需要绘制的数据大小和绘图的颜色
	bool m_use_log_scale{ false };		//如果为true则使用对数比例绘制矩形，否则使用线性比例

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

