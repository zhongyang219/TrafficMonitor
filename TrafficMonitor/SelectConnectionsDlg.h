#pragma once
#include "CommonData.h"
#include "BaseDialog.h"

// CSelectConnectionsDlg 对话框

class CSelectConnectionsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSelectConnectionsDlg)

public:
	CSelectConnectionsDlg(const StringSet& connections_hide, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSelectConnectionsDlg();

    const StringSet& GetData() const { return m_connections_hide; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_CONNECTIONS_DIALOG };
#endif

private:
    CCheckListBox m_list_ctrl;
    std::vector<std::wstring> m_all_connection_names;
    StringSet m_connections_hide;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
