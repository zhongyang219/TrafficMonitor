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
    // 重新应用布局管理器参数，这会使控件的基础大小/位置以当前为准
    void ReLoadLayoutResource();

private:
    UINT m_nDialogID;                                   // 成员变量用于保存资源 ID
    CSize m_min_size{};     //窗口的最小大小（以 96dpi 的大小保存）
    CSize m_window_size{ -1, -1 };
    CBrush m_brBkgr;
    CDC* m_pDC = nullptr;                               // InitializeControls期间有效，用于测量文本长度
    static std::map<CString, HWND> m_unique_hwnd;       //针对每一个基类的唯一的窗口句柄
    bool m_remember_dlg_size{ true };                   //是否记住窗口大小（当此标志为true且GetDialogName返回字符串不为空时会记住窗口大小）
    CFont m_dlg_font;
    int m_dpi;              //窗口自己的DPI

protected:
    int DPI(int pixel) const;

    // 仅在InitializeControls期间可用，测量控件文本长度
    CRect GetTextExtent(const CString& text);

    struct CtrlTextInfo
    {
        // 这里的枚举作为能够类型检查的int使用
        // Col具体值大小无意义，只表示列之间的相对顺序以及哪些控件同一列，实际上没有数量限制，需要的话可以增加
        enum Col { L4 = -4, L3 = -3, L2 = -2, L1 = -1, C0 = 0, R1 = 1, R2 = 2, R3 = 3, R4 = 4, UN_USE = 100 };
        // 控件除去文本后剩余的宽度。
        // Static静态文本推荐W0，按钮因为可能有图标推荐W32
        enum Width { W_50 = -50, W0 = 0, W16 = 16, W24 = 24, W32 = 32, W40 = 40, W60 = 60, W64 = 64, W96 = 96, W128 = 128, W256 = 256 };

        Col col_index{ UN_USE };    // 指示控件的位置 ，从左向右递增，小于0左贴靠，大于0右贴靠，等于0使用剩余空间
        UINT id{ 0 };               // 控件ID
        Width ext_width{ W0 };      // 控件宽度至少需要“文字宽度+ext_width”（内部会执行DPI转换 theApp.DPI(ext_width)）
    };
    // 仅在InitializeControls期间可用，根据文本长度重排控件，不会进行任何垂直方向调整，不会改变控件间距
    // 只会增加控件宽度故推荐在资源中设置更小的宽度，使用此方法调整到合适的状态
    // 优先保证中间的剩余宽度（或col_index为0的控件宽度）至少为center_min_width
    // 空间不足时其他控件文字会无法完全显示（此时应重新设计窗口）（此方法仅适用于文字不可能太长的情况）
    void RepositionTextBasedControls(const vector<CtrlTextInfo>& items, CtrlTextInfo::Width center_min_width = CtrlTextInfo::W16);
    
    // 由CBaseDialog::OnInitDialog在还原配置中窗口大小前调用
    // 派生类执行部分控件初始化，比如设置控件文本，重排控件RepositionTextBasedControls
    // 返回true会重新应用布局管理器参数，这会使控件动态布局管理器的基础大小/位置以当前为准
    // 与实际窗口大小相关的初始化（比如表格列宽）应在派生类的OnInitDialog进行
    virtual bool InitializeControls() { return false; };

    virtual CString GetDialogName() const { return CString(); }
    void EnableDlgCtrl(UINT id, bool enable);
    void SetButtonIcon(UINT id, HICON hIcon);
    void SetRememberDlgSize(bool enable) { m_remember_dlg_size = enable; }

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
