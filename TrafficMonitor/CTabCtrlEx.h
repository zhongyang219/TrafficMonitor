#pragma once


// CTabCtrlEx

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)

public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();

	void AddWindow(CWnd* pWnd, LPCTSTR lable_text);		//向当前tab控件添加一个子窗口
	void SetCurTab(int index);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	vector<CWnd*> m_tab_list;		//保存tab控件每个子窗口的指针
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();

	CRect m_tab_rect;
};


