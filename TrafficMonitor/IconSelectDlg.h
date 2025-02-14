#pragma once
#include "afxwin.h"
#include "PictureStatic.h"
#include "BaseDialog.h"

// CIconSelectDlg 对话框

class CIconSelectDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CIconSelectDlg)

public:
    CIconSelectDlg(int icon_selected, CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CIconSelectDlg();

    int GetIconSelected() const;

    void SetAutoAdaptNotifyIcon(bool val);
    bool AutoAdaptNotifyIcon() const;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ICON_SELECT_DIALOG };
#endif

#define PREVIEW_WIDTH theApp.DPI(200)		//预览图的宽高
#define PREVIEW_HEIGHT theApp.DPI(40)

#define ICON_X theApp.DPI(46)		//预览图中图标的位置
#define ICON_Y theApp.DPI(12)

protected:
    CPictureStatic m_preview_pic;
    CComboBox m_icon_select_combo;
    CButton m_auto_adapt_chk;

    int m_icon_selected{};

    bool m_atuo_adapt_notify_icon;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

    void DrawPreviewIcon(CDC* pDC);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    //afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnCbnSelchangeCombo1();
protected:
    afx_msg LRESULT OnControlRepaint(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedAutoAdaptCheck();
};
