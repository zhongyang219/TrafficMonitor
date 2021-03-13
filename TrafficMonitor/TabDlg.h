//这是用于Tab标签中的子对话框类
#pragma once
class CTabDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTabDlg)
public:
	CTabDlg(UINT nIDTemplate, CWnd *pParent = NULL);
	~CTabDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
    CWnd* GetParentWindow();
	void SetScrollbarInfo(int nPage, int nMax);
    void ResetScroll();
    virtual void OnTabEntered() {}      //当标签切换到当前窗口时被调用
    virtual void OnOK();
	void SetScrollEnable(bool enable) { m_scroll_enable = enable; }

protected:
	void ScrollWindowSimple(int step);

    void EnableDlgCtrl(UINT id, bool enable);
    void ShowDlgCtrl(UINT id, bool show);

protected:
	CWnd* m_pParent;
	bool m_scroll_enable{ false };
    int m_last_pos{};

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

