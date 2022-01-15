#pragma once
#include "afxwin.h"
#include "PictureStatic.h"
#include "BaseDialog.h"

// CDonateDlg 对话框

class CDonateDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CDonateDlg)

public:
    CDonateDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CDonateDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DONATE_DIALOG };
#endif

protected:
    //CPictureStatic m_donate_pic;
    CRect m_pic1_rect;
    CRect m_pic2_rect;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
};
