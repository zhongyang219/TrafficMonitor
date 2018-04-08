//这是用于Tab标签中的子对话框类
#pragma once
class CTabDlg : public CDialogEx
{
public:
	CTabDlg(UINT nIDTemplate, CWnd *pParent = NULL);
	~CTabDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
protected:
	CWnd* m_pParent;
};

