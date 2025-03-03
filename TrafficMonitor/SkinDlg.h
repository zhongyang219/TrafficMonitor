#pragma once
#include "afxwin.h"
#include"StaticEx.h"
#include "PictureStatic.h"
#include "CSkinPreviewView.h"
#include "LinkStatic.h"
#include "BaseDialog.h"

// CSkinDlg 对话框

class CSkinDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSkinDlg)

public:
    CSkinDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CSkinDlg();

    int m_skin_selected;			//选择的皮肤

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SKIN_DIALOG };
#endif

    const CSkinFile& GetSkinData() const { return m_skin_data; }

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
    //控件变量
    CListBox m_skin_list_box;   //选择皮肤列表
    CLinkStatic m_skin_course;	//“皮肤制作教程”超链接
    CLinkStatic m_skin_download;	//“更多皮肤下载”超链接
    CLinkStatic m_open_skin_dir_lnk;
    CSkinPreviewView* m_view;	//预览区视图类
    CStatic m_preview_static;
    CStaticEx m_notify_static;	//显示提示信息的static控件

    CSkinFile m_skin_data;		//皮肤数据

    void ShowPreview();		//显示皮肤预览
    CRect CalculateViewRect();		//根据窗口大小计算预览视图的大小

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLbnSelchangeList1();
    afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedSkinAutoAdaptButton();
    virtual void OnOK();
    afx_msg void OnBnClickedSkinAutoAdaptCheck();
};
