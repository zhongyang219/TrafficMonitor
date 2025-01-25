//此文件包含全局结构体、枚举类型的定义
#pragma once
#include "stdafx.h"
#include "TaskbarItemOrderHelper.h"

struct Date
{
    int year{};
    int month{};
    int day{};

    int week() const;       //该日期是一年的第几周

    //比较两个HistoryTraffic对象的日期，如果a的时间大于b，则返回true
    static bool DateGreater(const Date& a, const Date& b);

    //判断两个HistoryTraffic对象的日期是否相等
    static bool DateEqual(const Date& a, const Date& b);

};

//储存某一天的历史流量
struct HistoryTraffic : public Date
{
    //当天使用的流量（以KB为单位）
    unsigned __int64 up_kBytes{};
    unsigned __int64 down_kBytes{};
    bool mixed{ true };     //如果不区分上传和下载流量，则为true

    unsigned __int64 kBytes() const;

};

//历史流量统计中用于指示不同范围内的流量的颜色
#define TRAFFIC_COLOR_BLUE RGB(0, 183, 238)
#define TRAFFIC_COLOR_GREEN RGB(128, 194, 105)
#define TRAFFIC_COLOR_YELLOE RGB(255, 216, 58)
#define TRAFFIC_COLOR_RED RGB(255, 95, 74)
#define TRAFFIC_COLOR_DARK_RED RGB(166, 19, 0)

//网速单位
enum class SpeedUnit
{
    AUTO,       //自动
    KBPS,       //KB/s
    MBPS        //MB/s
};


//硬件监控的项目
enum HardwareItem
{
    HI_CPU = 1 << 0,        //CPU
    HI_GPU = 1 << 1,        //显卡
    HI_HDD = 1 << 2,        //硬盘
    HI_MBD = 1 << 3         //主板
};

#define DEF_CH L'\"'        //写入和读取ini文件字符串时，在字符串前后添加的字符
#define NONE_STR L"@@@"     //用于指定一个无效字符串
struct DispStrings      //显示的文本
{
private:
    std::map<CommonDisplayItem, wstring> map_str;

public:
    //获取一个显示的文本
    wstring& Get(CommonDisplayItem item);

    const std::map<CommonDisplayItem, wstring>& GetAllItems() const;

    void operator=(const DispStrings& disp_str);     //重载赋值运算符

    //载入一个插件项目的显示文本
    void Load(const std::wstring& plugin_id, const std::wstring& disp_str);

    //是否无效
    bool IsInvalid() const;
};

//鼠标双击窗口的动作
enum class DoubleClickAction
{
    CONNECTION_INFO,    //连接详情
    HISTORY_TRAFFIC,    //历史流量统计
    SHOW_MORE_INFO,     //显示更多信息
    OPTIONS,            //选项设置
    TASK_MANAGER,       //任务管理器
    SEPCIFIC_APP,       //指定应用程序
    CHANGE_SKIN,        //更换皮肤
    NONE                //不执行任何动作
};

//语言
enum class Language
{
    FOLLOWING_SYSTEM,       //跟随系统
    ENGLISH,                //英语
    SIMPLIFIED_CHINESE,     //简体中文
    TRADITIONAL_CHINESE     //繁体中文
};

//颜色模式
enum class ColorMode
{
    Default, //默认颜色
    Light   //浅色
};

//将字号转成LOGFONT结构中的lfHeight
inline int FontSizeToLfHeight(int font_size, int dpi = 0)
{
    if (dpi == 0)
    {
        HDC hDC = ::GetDC(HWND_DESKTOP);
        dpi = GetDeviceCaps(hDC, LOGPIXELSY);
        ::ReleaseDC(HWND_DESKTOP, hDC);
    }
    int lfHeight = -MulDiv(font_size, dpi, 72);
    return lfHeight;
}

//字体
struct FontInfo
{
    CString name;   //字体名称
    int size;       //字体大小
    bool bold;          //粗体
    bool italic;        //斜体
    bool underline;     //下划线
    bool strike_out;    //删除线

    //创建一个CFont对象
    void Create(CFont& font, int dpi = 0)
    {
        font.CreateFont(
            FontSizeToLfHeight(size, dpi), // nHeight
            0, // nWidth
            0, // nEscapement
            0, // nOrientation
            (bold ? FW_BOLD : FW_NORMAL), // nWeight
            italic, // bItalic
            underline, // bUnderline
            strike_out, // cStrikeOut
            DEFAULT_CHARSET, // nCharSet
            OUT_DEFAULT_PRECIS, // nOutPrecision
            CLIP_DEFAULT_PRECIS, // nClipPrecision
            DEFAULT_QUALITY, // nQuality
            DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
            name);
    }
};

//历史流量统计列表视图中显示模式
enum class HistoryTrafficViewType
{
    HV_DAY,         //日视图
    HV_WEEK,        //周视图
    HV_MONTH,          //月视图
    HV_QUARTER,     //季视图
    HV_YEAR            //年视图
};

struct StringSet
{
public:
    bool Contains(const std::wstring& str) const;
    void SetStrContained(const std::wstring& str, bool contained);
    void FromString(const std::wstring& str);
    std::wstring ToString() const;
    void FromVector(const std::vector<std::wstring>& vec);
    std::vector<std::wstring> ToVector() const;
    std::set<std::wstring>& data();
private:
    std::set<std::wstring> string_set;
};

//选项设置数据
struct MainConfigData
{
    int m_transparency{ 100 };          //窗口透明度
    bool m_show_more_info{ false };     //显示更多信息
    bool m_show_task_bar_wnd{ false };  //显示任务栏窗口
    bool m_hide_main_window;            //隐藏主窗口
    //bool m_tbar_show_cpu_memory;      //任务栏窗口显示CPU和内存利用率

    int m_position_x;   //窗口位置的x坐标
    int m_position_y;   //窗口位置的y坐标

    bool m_auto_select{ false };    //自动选择连接
    bool m_select_all{ false };     //统计所有连接的网速
    string m_connection_name;      //当前选择网络的名称

    wstring m_skin_name;            //选择的皮肤的名称

    bool skin_auto_adapt{ false };  //根据深色/浅色模式自动切换皮肤
    wstring skin_name_light_mode;   //浅色模式下的皮肤名称
    wstring skin_name_dark_mode;    //深色模式下的皮肤名称

    int m_dft_notify_icon = 0;      //默认的通知图标(用于区分win10的深色和浅色模式)
    int m_notify_icon_selected{};   //要显示的通知区图标
    bool m_notify_icon_auto_adapt{ false }; //通知区图标是否自动适应Win10深浅色模式

    //bool m_show_internet_ip{ false };     //是否在“连接详情”对话框中显示外网IP地址
    bool m_use_log_scale{ false };          //“历史流量统计”对话框中绘制表示历史流量数值的矩形时是否使用对数比例
    HistoryTrafficViewType m_view_type{};
    bool m_sunday_first{ true };            //是否将周日作为一周的第一天
    StringSet plugin_disabled;      //已禁用的插件

};

//内存显示方式
enum class MemoryDisplay
{
    USAGE_PERCENTAGE,       //已使用百分比
    MEMORY_USED,            //内存已使用
    MEMORY_AVAILABLE        //内存可用
};

//选项设置中“主窗口设置”和“任务栏窗口设置”中公共的数据（不使用此结构体创建对象）
struct PublicSettingData
{
    bool specify_each_item_color{ false };      //是否指定每个项目的颜色
    FontInfo font;          //字体
    DispStrings disp_str;   //显示的文本
    bool speed_short_mode{ false };     //网速显示简洁模式（减少小数点的位数，单位不显示“B”）
    bool separate_value_unit_with_space{ true };    //网速数值和单位用空格分隔
    bool show_tool_tip{ true };         //显示鼠标提示
    MemoryDisplay memory_display{ MemoryDisplay::USAGE_PERCENTAGE };    //内存显示方式

    bool unit_byte{ true };             //使用字节(B)而不是比特(b)为单位
    SpeedUnit speed_unit;       //网速的单位
    bool hide_unit;         //隐藏单位
    bool hide_percent;      //隐藏百分号
    DoubleClickAction double_click_action;      //鼠标双击动作
    wstring double_click_exe;   //鼠标双击动作为打开指定应用程序时，打开的程序路径
};

//#define MAIN_WND_COLOR_NUM 9      //主窗口颜色数量
//选项设置中“主窗口设置”的数据
struct MainWndSettingData : public PublicSettingData
{
    std::map<CommonDisplayItem, COLORREF> text_colors{};    //方字的颜色
    bool swap_up_down{ false };     //交换上传和下载显示的位置
    bool hide_main_wnd_when_fullscreen;     //有程序全屏运行时隐藏悬浮窗
    bool m_always_on_top{ false };      //窗口置顶
    bool m_lock_window_pos{ false };    //锁定窗口位置
    bool m_mouse_penetrate{ false };    //鼠标穿透
    bool m_alow_out_of_border{ false };     //是否允许悬浮窗超出屏幕边界

};

//#define TASKBAR_COLOR_NUM 18      //任务栏窗口颜色数量

struct TaskbarItemColor //任务栏窗口每一项的颜色
{
    COLORREF label{};   //标签颜色
    COLORREF value{};   //数值颜色

    bool operator==(const TaskbarItemColor& item) const
    {
        return label == item.label && value == item.value;
    }
};

//选项设置中“任务栏窗口设置”的数据
struct TaskBarSettingData : public PublicSettingData
{
    COLORREF  back_color{ RGB(0, 0, 0) };   //背景颜色
    COLORREF transparent_color{ RGB(0, 0, 0) };     //透明色
    COLORREF status_bar_color{ RGB(0, 0, 0) };      // CPU/内存 状态条颜色
    std::map<CommonDisplayItem, TaskbarItemColor> text_colors{};    //文字的颜色
    int dft_back_color = 0;                         //默认背景颜色
    int dft_transparent_color = 0;                  //默认透明色
    int dft_status_bar_color = 0x005A5A5A;          //默认CPU/内存 状态条颜色
    int dft_text_colors = 0x00ffffffU;              //默认文字颜色

    bool auto_adapt_light_theme{ true };            //是否自动适应浅色主题
    int dark_default_style{ 0 };                    //深色主题时使用的预设方案
    int light_default_style{ -1 };                  //浅色主题时使用的预设方案
    bool auto_set_background_color{ false };        //根据任务栏颜色自动设置背景色
    bool auto_save_taskbar_color_settings_to_preset{};    //当启用“自动适应Windows10深色/浅色主题”时，是否在颜色设置有更改时自动将当前颜色设置保存到对应的预设
    bool IsTaskbarTransparent() const;
    void SetTaskabrTransparent(bool transparent);

    CTaskbarItemOrderHelper item_order;
    unsigned int m_tbar_display_item{ TDI_UP | TDI_DOWN };      //任务栏窗口显示的项目
    StringSet plugin_display_item;                  //任务窗口显示的插件项目

    bool value_right_align{ false };    //数值是否右对齐
    int digits_number{ 4 };             //数据位数
    bool horizontal_arrange{ true };    //水平排列
    bool show_status_bar{ true };       //显示 CPU/内存的状态条
    bool tbar_wnd_on_left{ false };     //如果为true，则任务栏窗口显示在任务栏的左侧（或上方）
    bool tbar_wnd_snap{ false };     	//如果为true，则在Win11中任务栏窗口贴靠中间任务栏，否则靠近边缘
    bool cm_graph_type{ false };        //如果为false，默认原样式，柱状图显示占用率，如为true，滚动显示占用率
    bool show_graph_dashed_box{ true }; //显示占用图虚线框

    int item_space{};                   //项目间距
    int vertical_margin{};              //项目垂直间距
    int window_offset_top{};            //任务栏窗口顶部边距
    int window_offset_left{};           //任务栏窗口左侧边距
    void ValidItemSpace();
    void ValidVerticalMargin();
    void ValidWindowOffsetTop();
    void ValidWindowOffsetLeft();
    bool avoid_overlap_with_widgets{ false };   //避免与右侧小组件重叠
    int taskbar_left_space_win11{};         //Windows11下，任务栏小工具的宽度

    bool show_netspeed_figure{ false };     //是否显示网速占用图
    int netspeed_figure_max_value;          //网速占用图的最大值
    int netspeed_figure_max_value_unit{};   //网速占用图最大值的单位（0: KB, 1: MB）
    unsigned __int64 GetNetspeedFigureMaxValueInBytes() const;  //获取网速占用图的最大值（以字节为单位）

    bool disable_d2d{ false };//是否禁用d2d绘图
    DWORD update_layered_window_error_code{0}; // 使用D2D1渲染时，UpdateLayeredWindowIndirect失败的错误代码，会在关闭任务栏窗口时被重置为0
    bool enable_colorful_emoji{ true };       //是否显示彩色emoji

    bool is_windows_web_experience_detected{ false }; //是否检测到Windows Web Experience小组件安装信息
};

//选项设置中“常规设置”的数据
struct GeneralSettingData
{
    bool check_update_when_start{ true };
    int update_source{};                    //更新源。0: GitHub; 1: Gitee
    bool auto_run{ false };
    bool allow_skin_cover_font{ true };
    bool allow_skin_cover_text{ true };
    bool show_notify_icon{ true };    //显示通知区域图标
//通知消息
    bool traffic_tip_enable{ false };       //是否启用流量超出时提示
    int traffic_tip_value;                  //要提示的流量临界值
    int traffic_tip_unit{};                 //要提示的流量值的单位（0: MB, 1: GB）

    struct NotifyTipSettings        //超过某个值时弹出提示的设置
    {
        bool enable;                //是否启用提示
        int tip_value;              //要弹出提示的临界值
    };
    NotifyTipSettings memory_usage_tip;     //用内存使用率超出提示
    NotifyTipSettings cpu_temp_tip;         //CPU温度超出提示
    NotifyTipSettings gpu_temp_tip;         //显卡温度超出提示
    NotifyTipSettings hdd_temp_tip;         //硬盘温度超出提示
    NotifyTipSettings mainboard_temp_tip;   //主板温度超出提示


    //语言
    Language language;

    bool show_all_interface{ true };

    //CPU利用率获取方式
    enum CpuUsageAcquireMethod
    {
        CA_CPU_TIME,    //使用时间
        CA_PDH,         //性能计数器
        CA_HARDWARE_MONITOR     //来自硬件监控
    };
    CpuUsageAcquireMethod cpu_usage_acquire_method{};  //获取CPU利用率的方式

    bool portable_mode{ false };        //便携模式，如果为true，则程序所有数据都保存到exe所在目录下，否则保存到Appdata\Romaing目录下
    int monitor_time_span{ 1000 };    //监控的时间间隔

    std::wstring hard_disk_name;        //要监控的硬盘名称
    std::wstring cpu_core_name;         //要监控的CPU核心的名称

    unsigned int hardware_monitor_item{};   //要监控哪些硬件
    bool IsHardwareEnable(HardwareItem item_type) const
    {
        return hardware_monitor_item & item_type;
    }
    void SetHardwareEnable(HardwareItem item_type, bool enable)
    {
        if (enable)
            hardware_monitor_item |= item_type;
        else
            hardware_monitor_item &= ~item_type;
    }

    StringSet connections_hide;     //用于保存哪些网络要从“选择网络连接”子菜单项中隐藏
};

//定义监控时间间隔有效的最大值和最小值
#define MONITOR_TIME_SPAN_MIN 200
#define MONITOR_TIME_SPAN_MAX 30000

enum class Alignment
{
    LEFT,       //左对齐
    RIGHT,      //右对齐
    CENTER,     //居中
    SIDE        //两端对齐
};

//通过构造函数传递一个bool变量的引用，在构造时将其置为true，析构时置为false
class CFlagLocker
{
public:
    CFlagLocker(bool& flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~CFlagLocker()
    {
        m_flag = false;
    }

private:
    bool& m_flag;
};
