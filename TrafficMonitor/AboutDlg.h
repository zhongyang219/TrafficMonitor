#pragma once
#include "DonateDlg.h"
#include "LinkStatic.h"
#include "BaseDialog.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CBaseDialog
{
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    CLinkStatic m_mail;             //“联系作者”超链接
    CLinkStatic m_acknowledgement;  //“鸣谢”超链接
    CLinkStatic m_github;           //“GitHub”超链接
    CLinkStatic m_gitee;            //“Gitee”超链接
    CLinkStatic m_donate;           //“捐助”超链接
    CLinkStatic m_license;          //“开源协议”超链接
    CToolTipCtrl m_tool_tip;            //鼠标指向时的工具提示
    CLinkStatic m_translator_static;
    CLinkStatic m_openhardwaremonitor_link;
    CLinkStatic m_tinyxml2_link;
    CLinkStatic m_musicplayer2_link;
    CLinkStatic m_simplenotepad_link;

    CBitmap m_about_pic;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    CString GetDonateList();        //从资源文件加载捐赠人员名单
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    CRect CalculatePicRect();       //计算图片的位置

    // 实现
protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //  afx_msg void OnStnClickedStaticDonate();
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
