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
    virtual void OnOK();

protected:
	void ScrollWindowSimple(int step);

protected:
	CWnd* m_pParent;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

