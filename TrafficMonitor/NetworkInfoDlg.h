#pragma once
#include"Common.h"
#include "afxcmn.h"
#include "AdapterCommon.h"
#include "BaseDialog.h"

// CNetworkInfoDlg 对话框

class CNetworkInfoDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CNetworkInfoDlg)

public:
    CNetworkInfoDlg(vector<NetWorkConection>& adapters, MIB_IFROW* pIfRow, int connection_selected, CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CNetworkInfoDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_NETWORK_INFO_DIALOG };
#endif

    SYSTEMTIME m_start_time;		//程序启动的时间

protected:

    vector<NetWorkConection>& m_connections;
    MIB_IFROW* m_pIfRow;
    int m_connection_selected;		//当前对话框显示的连接
    int m_current_connection;		//初始选择的连接

    CListCtrl m_info_list;
    CMenu m_menu;
    CString m_selected_string;
    CFont m_font_bold;		//默认字体的粗体

    CWinThread* m_pGetIPThread;			//获取外网IP的线程
    bool m_ip_acquired{ false };        //如果已获取外网ip地址，则为true

    //void GetIPAddress();	//获取IP地址
    void ShowInfo();
    void GetProgramElapsedTime();
    MIB_IFROW& GetConnectIfTable(int connection_index);    //获取当前选择的网络连接的MIB_IFROW对象。connection_index为m_connections中的索引
    NetWorkConection GetConnection(int connection_index); //获取当前选择的网络连接的NetWorkConection对象。connection_index为m_connections中的索引

    //获取外网IP的线程函数
    static UINT GetInternetIPThreadFunc(LPVOID lpParam);

    virtual CString GetDialogName() const override;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();
public:
    afx_msg void OnCopyText();
    afx_msg void OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClose();
    afx_msg void OnBnClickedPreviousButton();
    afx_msg void OnBnClickedNextButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnNMDblclkInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
};
