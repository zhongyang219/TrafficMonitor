#pragma once
#include <functional>

// CBaseDialog 对话框
//用于实现记住对话框大小
//并将窗口初始大小设置为最小大小

class CBaseDialog : public CDialog
{
    DECLARE_DYNAMIC(CBaseDialog)

public:
    CBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CBaseDialog();

    // 复制自CDialogEx，与其功能相同（新增滑动条控件和超链接控件的处理）
    void SetBackgroundColor(COLORREF color, BOOL bRepaint = TRUE);

    // 对话框数据
    //#ifdef AFX_DESIGN_TIME
    //	enum { IDD = IDD_BASEDIALOG };
    //#endif

    void SetMinSize(int cx, int cy);		//设置窗口的最小大小，如果未设置，则使用窗口的初始大小作为最小大小
    static HWND GetUniqueHandel(LPCTSTR dlg_name);          //获指定窗口唯一的句柄
    static const std::map<CString, HWND>& AllUniqueHandels();   //获取所有窗口的句柄
    static bool IsAllDialogClosed();

private:
    void LoadConfig();
    void SaveConfig() const;
    void IterateControls(CWnd* pParent, std::function<void(CWnd*)> func);

private:
    CSize m_min_size{};     //窗口的最小大小（以 96dpi 的大小保存）
    CSize m_window_size{ -1, -1 };
    CBrush m_brBkgr;
    static std::map<CString, HWND> m_unique_hwnd;        //针对每一个基类的唯一的窗口句柄

protected:
    virtual CString GetDialogName() const { return CString(); }
    void EnableDlgCtrl(UINT id, bool enable);
    void SetButtonIcon(UINT id, HICON hIcon);

    //遍历所有子控件
    void IterateControls(std::function<void(CWnd*)> func);

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual INT_PTR DoModal();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
