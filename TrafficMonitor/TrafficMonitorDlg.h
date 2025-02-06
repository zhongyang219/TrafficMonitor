
// TrafficMonitorDlg.h : 头文件
//

#pragma once
#pragma comment (lib, "iphlpapi.lib")

#include "NetworkInfoDlg.h"
#include "afxwin.h"
#include "StaticEx.h"
#include "Common.h"
#include "TaskBarDlg.h"
#include "SkinDlg.h"
#include "HistoryTrafficDlg.h"
#include "OptionsDlg.h"
#include "PictureStatic.h"
#include "IconSelectDlg.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "LinkStatic.h"
#include "AdapterCommon.h"
#include "AboutDlg.h"
#include "CPUUsage.h"
#include "HistoryTrafficFile.h"

// CTrafficMonitorDlg 对话框
class CTrafficMonitorDlg : public CDialog
{
    // 构造
public:
    CTrafficMonitorDlg(CWnd* pParent = NULL);   // 标准构造函数
    ~CTrafficMonitorDlg();
    CTaskBarDlg* GetTaskbarWindow() const;

    static CTrafficMonitorDlg* Instance();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TRAFFICMONITOR_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;
    NOTIFYICONDATA m_ntIcon;    //通知区域图标
    CTaskBarDlg* m_tBarDlg{};     //任务栏窗口的指针

    vector<NetWorkConection> m_connections; //保存获取到的要显示到“选择网卡”菜单项中的所有网络连接
    MIB_IFTABLE* m_pIfTable;
    DWORD m_dwSize{};	//m_pIfTable的大小
    int m_connection_selected{ 0 }; //要显示流量的连接的序号
    unsigned __int64 m_in_bytes{};        //当前已接收的字节数
    unsigned __int64 m_out_bytes{};   //当前已发送的字节数
    unsigned __int64 m_last_in_bytes{}; //上次已接收的字节数
    unsigned __int64 m_last_out_bytes{};    //上次已发送的字节数

    CCPUUsage m_cpu_usage_helper;
    CCpuFreq m_cpu_freq_helper;

    bool m_first_start{ true };     //初始时为true，在定时器第一次启动后置为flase

    // https://www.jianshu.com/p/9d4b68cdbd99
    struct Monitors
    {
        std::vector<MONITORINFO> monitorinfos;

        static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
        {
            MONITORINFO iMonitor;
            iMonitor.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMon, &iMonitor);

            Monitors* pThis = reinterpret_cast<Monitors*>(pData);
            pThis->monitorinfos.push_back(iMonitor);
            return TRUE;
        }

        Monitors()
        {
            EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
        }
    };

    //CRect m_screen_rect;        //屏幕的范围（不包含任务栏）
    vector<CRect> m_screen_rects;       //所有屏幕的范围（不包含任务栏）
    vector<CRect> m_last_screen_rects;       //上一次所有屏幕的范围（不包含任务栏）
    CSize m_screen_size;        //屏幕的大小（包含任务栏）
    CSkinFile m_skin;
    CommonDisplayItem m_clicked_item;           //鼠标点击的项目

    CFont m_font;           //字体

    int m_restart_cnt{ -1 };    //重新初始化次数
    unsigned int m_timer_cnt{};     //定时器触发次数（自程序启动以来的秒数）
    unsigned int m_taskbar_timer_cnt{0}; //适用于TaskBarDlg的定时器触发次数（自程序启动以来的秒数）
    unsigned int m_monitor_time_cnt{};
    int m_zero_speed_cnt{}; //如果检测不到网速，该变量就会自加
    int m_insert_to_taskbar_cnt{};  //用来统计尝试嵌入任务栏的次数
    int m_cannot_insert_to_task_bar_warning{ true };   //指示是否会在无法嵌入任务栏时弹出提示框

    static unsigned int m_WM_TASKBARCREATED;    //任务栏重启消息

    vector<wstring> m_skins;    //储存皮肤文件的路径
    int m_skin_selected{};      //选择的皮肤序号

    SYSTEMTIME m_start_time;    //程序启动时的时间
    CHistoryTrafficFile m_history_traffic{ theApp.m_history_traffic_path }; //储存历史流量

    CToolTipCtrl m_tool_tips;

    bool m_connection_change_flag{ false };     //如果执行过IniConnection()函数，该flag会置为true
    bool m_is_foreground_fullscreen{ false };   //指示前台窗口是否正在全局显示
    bool m_menu_popuped{ false };               //指示当前是否有菜单处于弹出状态

    HDC m_desktop_dc;

    string m_connection_name_preferd{ theApp.m_cfg_data.m_connection_name };          //保存用户手动选择的网络连接名称

    void DoMonitorAcquisition();    //获取一次监控信息
    static UINT MonitorThreadCallback(LPVOID dwUser);   //获取监控信息的线程函数
    bool m_monitor_data_required{ false };          //线程中需要获取监控数据标志，当需要获取监控数据时置为true，获取到一次监控数据时置为false
    bool m_is_thread_exit{ false }; //线程退出标志
    CEvent m_threadExitEvent;       //用于通知主线程工作线程已退出
public:
    void ExitMonitorThread();       //停止监控线程

protected:
    CString GetMouseTipsInfo();     //获取鼠标提示信息
    void SetTransparency();         //根据m_transparency的值设置窗口透明度
    void SetTransparency(int transparency);
public:
    void SetAlwaysOnTop();          //根据m_always_on_top的值设置窗口置顶
protected:
    void SetMousePenetrate();       //根据m_mouse_penetrate的值设置是否鼠标穿透
    POINT CalculateWindowMoveOffset(CRect rect, bool screen_changed);  //计算当窗口处于屏幕区域外时，移动到屏幕区域需要移动的位置
    void CheckWindowPos(bool screen_changed = false);          //测试窗口的位置，如窗口的位置在屏幕外，则移动窗口使其全部都在屏幕内，并返回新位置
    void GetScreenSize();           //获取屏幕的大小

    void AutoSelect();
    //void UpdateConnections();
    //自动选择连接
    void IniConnection();   //初始化连接

    MIB_IFROW GetConnectIfTable(int connection_index);    //获取当前选择的网络连接的MIB_IFROW对象。connection_index为m_connections中的索引
    NetWorkConection GetConnection(int connection_index); //获取当前选择的网络连接的NetWorkConection对象。connection_index为m_connections中的索引

    void IniConnectionMenu(CMenu* pMenu);   //初始化“选择网络连接”菜单
    void IniTaskBarConnectionMenu();        //初始化任务栏窗口的“选择网络连接”菜单
    void SetConnectionMenuState(CMenu* pMenu);      //设置“选择网络连接”菜单中选中的项目

    void CloseTaskBarWnd(); //关闭任务栏窗口
    void OpenTaskBarWnd();  //打开任务栏窗口

    void AddNotifyIcon();       //添加通知区图标
    void DeleteNotifyIcon();
    void ShowNotifyTip(const wchar_t* title, const wchar_t* message);       //显示通知区提示
    void UpdateNotifyIconTip();     //更新通知区图标的鼠标提示

    void SaveHistoryTraffic();
    void LoadHistoryTraffic();
    void BackupHistoryTrafficFile();

    void _OnOptions(int tab);   //打开“选项”对话框的处理，参数为打开时切换的标签

    void ApplySettings(COptionsDlg& optionsDlg);

    void SetItemPosition();     //设置显示的4个项目的位置
    void LoadSkinLayout();      //从当前皮肤获取布局数据

    void LoadBackGroundImage();
    void SetTextFont();

    bool IsTaskbarWndValid() const;

    void TaskbarShowHideItem(DisplayItem type);

    //判断一个点在哪个显示项目的区域内，并保存到m_clicked_item
    void CheckClickedItem(CPoint point);

    int FindSkinIndex(const wstring& skin_name);

    //应用一个皮肤
    void ApplySkin(int skin_index);

public:
    //void ApplySettings();
    bool IsTemperatureNeeded() const;       //判断是否需要显示温度信息

protected:
    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    //  afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    //  afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    //  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnNetworkInfo();
    afx_msg void OnAlwaysOnTop();
    afx_msg void OnTransparency100();
    afx_msg void OnTransparency80();
    afx_msg void OnTransparency60();
    afx_msg void OnTransparency40();
    afx_msg void OnClose();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnInitMenu(CMenu* pMenu);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnLockWindowPos();
    afx_msg void OnMove(int x, int y);
protected:
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnShowNotifyIcon();
    afx_msg void OnDestroy();
    afx_msg void OnShowCpuMemory();
    afx_msg void OnMousePenetrate();
    //afx_msg void OnTextColor();
    afx_msg void OnShowTaskBarWnd();
    afx_msg void OnAppAbout();
    afx_msg void OnShowCpuMemory2();
    afx_msg void OnShowMainWnd();
    afx_msg void OnChangeSkin();
    afx_msg LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);
    //afx_msg void OnSetFont();
    afx_msg void OnTrafficHistory();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnOptions();
    afx_msg void OnOptions2();
protected:
    afx_msg LRESULT OnExitmenuloop(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnChangeNotifyIcon();
    afx_msg void OnAlowOutOfBorder();
    afx_msg void OnCheckUpdate();
protected:
    afx_msg LRESULT OnTaskbarMenuPopedUp(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnShowNetSpeed();
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnPaint();
protected:
    afx_msg LRESULT OnDpichanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskbarWndClosed(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMonitorInfoUpdated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDisplaychange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnReopenTaksbarWnd(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnExitSizeMove();
    afx_msg void OnPluginManage();
    afx_msg void OnOpenTaskManager();
protected:
    afx_msg LRESULT OnSettingsApplied(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDisplaySettings();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRefreshConnectionList();
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPluginOptions();
    afx_msg void OnPluginDetail();
    afx_msg void OnPluginOptionsTaksbar();
    afx_msg void OnPluginDetailTaksbar();
};
