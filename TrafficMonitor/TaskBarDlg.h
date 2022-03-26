#pragma once
#include "Common.h"
#include "afxwin.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "CommonData.h"
#include "TaskbarItemOrderHelper.h"
#include <list>

// CTaskBarDlg 对话框
#define TASKBAR_WND_HEIGHT theApp.DPI(32)				//任务栏窗口的高度
#define WM_TASKBAR_MENU_POPED_UP (WM_USER + 1004)		//定义任务栏窗口右键菜单弹出时发出的消息
//#define TASKBAR_GRAPH_MAX_LEN 600						//历史数据存储最大长度
#define TASKBAR_GRAPH_STEP 5							//几秒钟画一条线

class CTaskBarDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CTaskBarDlg)

public:
    CTaskBarDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CTaskBarDlg();

    CToolTipCtrl m_tool_tips;

    void ShowInfo(CDC* pDC); 	//将信息绘制到控件上
    void TryDrawStatusBar(CDrawCommon& drawer, const CRect& rect_bar, int usage_percent); //绘制CPU/内存状态条

    void TryDrawGraph(CDrawCommon& drawer, const CRect& value_rect, DisplayItem item_type);		// 绘制CPU/内存动态图

    bool AdjustWindowPos();	//设置窗口在任务栏中的位置
    void ApplyWindowTransparentColor();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TASK_BAR_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    HWND m_hTaskbar;	//任务栏窗口句柄
    HWND m_hBar;		//任务栏窗口二级容器的句柄
    HWND m_hMin;		//最小化窗口的句柄
    HWND m_hNotify;     //任务栏通知区域的句柄

    CRect m_rcNotify;   //任务栏通知区域的矩形区域
    CRect m_rcBar;		//初始状态时任务栏窗口的矩形区域
    CRect m_rcMin;		//最小化窗口的矩形区域
    CRect m_rcMinOri;   //初始状态时最小化窗口的矩形区域
    CRect m_rect;		//当前窗口的矩形区域
    int m_window_width{};
    int m_window_height{};

    //任务栏各个部分的宽度
    struct ItemWidth
    {
        int label_width{};      //标签部分宽度
        int value_width{};      //数值部分宽度

        int TotalWidth() const  //总宽度
        {
            return label_width + value_width;
        }

        int MaxWidth() const	//最大宽度
        {
            return max(label_width, value_width);
        }
    };

    struct ItemWidthInfo : public CommonDisplayItem
    {
        ItemWidth item_width;

        ItemWidthInfo()
        {}

        ItemWidthInfo(const CommonDisplayItem& item)
            : CommonDisplayItem(item)
        {}
    };

    std::vector<ItemWidthInfo> m_item_widths;   //任务栏窗口每个部分的宽度
    std::map<CommonDisplayItem, CRect> m_item_rects;    //任务栏窗口每个部分的矩形区域
    CommonDisplayItem m_clicked_item;           //鼠标点击的任务栏项目

    int m_min_bar_width;	//最小化窗口缩小宽度后的宽度
    int m_min_bar_height;	//最小化窗口缩小高度后的高度（用于任务栏在屏幕左侧或右侧时）

    std::map<DisplayItem, std::list<int>> m_map_history_data;  //保存各项数据历史数据的链表，链表保存按照时间顺序，越靠近头部数据越新
    std::map<DisplayItem, int> m_history_data_count;            //统计添加到历史数据链表的次数

    int m_left_space{};			//最小化窗口和二级窗口窗口左侧的边距
    int m_top_space{};			//最小化窗口和二级窗口窗口顶部的边距（用于任务栏在屏幕左侧或右侧时）

    bool m_connot_insert_to_task_bar{ false };	//如果窗口无法嵌入任务栏，则为true
    bool m_taskbar_on_top_or_bottom{ true };		//如果任务栏在屏幕顶部或底部，则为ture
    int m_error_code{};
    bool m_menu_popuped{ false };               //指示当前是否有菜单处于弹出状态

    CFont m_font;			//字体

    CDC* m_pDC{};		//窗口的DC，用来计算窗口的宽度

    void CheckTaskbarOnTopOrBottom();		//检查任务栏是否在屏幕的顶部或底部，并将结果保存在m_taskbar_on_top_or_bottom中
    CString GetMouseTipsInfo();		//获取鼠标提示

    void AddHisToList(DisplayItem item_type, int current_usage_percent);		//将当前利用率数值添加进链表

    int CalculateNetspeedPercent(unsigned __int64 net_speed);     //计算网速占网速占用图的最大值的百分比

    //判断一个点在哪个显示项目的区域内，并保存到m_clicked_item。如果返回false，则该点不在任何一个项目的区域内，否则返回true
    bool CheckClickedItem(CPoint point);

    //绘制任务栏窗口中的一个显示项目
    //  drawer: 绘图类的对象
    //  type: 项目的类型
    //  rect: 绘制矩形区域
    //  label_width: 标签区域的宽度
    //  vertical: 如果为true，则标签和数值上下显示
    void DrawDisplayItem(CDrawCommon& drawer, DisplayItem type, CRect rect, int label_width, bool vertical = false);

    //绘制任务栏窗口中的一个插件项目
   //  drawer: 绘图类的对象
   //  item: 插件显示项目的指针
   //  rect: 绘制矩形区域
   //  label_width: 标签区域的宽度
   //  vertical: 如果为true，则标签和数值上下显示
    void DrawPluginItem(CDrawCommon& drawer, IPluginItem* item, CRect rect, int label_width, bool vertical = false);

    void MoveWindow(CRect rect);

public:
    void SetTextFont();
    void ApplySettings();
    void CalculateWindowSize();		//计算窗口每部分的大小，及各个部分的宽度。窗口大小保存到m_window_width和m_window_height中，各部分宽度保存到m_item_widths中

    void SetToolTipsTopMost();			//设置鼠标提示置顶
    void UpdateToolTips();

    bool GetCannotInsertToTaskBar() const { return m_connot_insert_to_task_bar; }
    int GetErrorCode() const { return m_error_code; }
    bool IsTasksbarOnTopOrBottom() { return m_taskbar_on_top_or_bottom; }

    static bool IsItemShow(DisplayItem item);
    static bool IsShowCpuMemory();
    static bool IsShowNetSpeed();

    CommonDisplayItem GetClickedItem() const { return m_clicked_item; }

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    //afx_msg void OnSetBackgroundColor();
    //afx_msg void OnSetTextColor();
    afx_msg void OnInitMenu(CMenu* pMenu);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //afx_msg void OnSetFont();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnClose();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
    afx_msg LRESULT OnExitmenuloop(WPARAM wParam, LPARAM lParam);
};
