//���ļ�����ȫ�ֽṹ�塢ö�����͵Ķ���
#pragma once
#include "stdafx.h"

//����ĳһ�����ʷ����
struct HistoryTraffic
{
	int year{};
	int month{};
	int day{};
	//unsigned int kBytes;	//����ʹ�õ���������KBΪ��λ��
	unsigned int up_kBytes{};
	unsigned int down_kBytes{};
	bool mixed{ true };		//����������ϴ���������������Ϊtrue

	unsigned int kBytes() const
	{
		return up_kBytes + down_kBytes;
	}

	//�Ƚ�����HistoryTraffic��������ڣ����a��ʱ�����b���򷵻�true
	static bool DateGreater(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		if (a.year != b.year)
			return a.year > b.year;
		else if (a.month != b.month)
			return a.month > b.month;
		else if (a.day != b.day)
			return a.day > b.day;
		else
			return false;
	}

	//�ж�����HistoryTraffic����������Ƿ����
	static bool DateEqual(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		return a.year == b.year && a.month == b.month && a.day == b.day;
	}
};

//��ʷ����ͳ��������ָʾ��ͬ��Χ�ڵ���������ɫ
#define TRAFFIC_COLOR_BLUE RGB(0, 183, 238)
#define TRAFFIC_COLOR_GREEN RGB(128, 194, 105)
#define TRAFFIC_COLOR_YELLOE RGB(255, 216, 58)
#define TRAFFIC_COLOR_RED RGB(255, 95, 74)

//���ٵ�λ
enum class SpeedUnit
{
	AUTO,		//�Զ�
	KBPS,		//KB/s
	MBPS		//MB/s
};

#define DEF_CH L'\"'		//д��Ͷ�ȡini�ļ��ַ���ʱ�����ַ���ǰ����ӵ��ַ�
#define NONE_STR L"@@@"		//����ָ��һ����Ч�ַ���
struct DispStrings		//��ʾ���ı�
{
	wstring up;
	wstring down;
	wstring cpu;
	wstring memory;
	void operator=(const DispStrings& disp_str)		//���ظ�ֵ�����
	{
		//�����ֵ���ַ����Ƕ������Ч�ַ������򲻸�ֵ
		if (disp_str.up != NONE_STR)
			up = disp_str.up;
		if (disp_str.down != NONE_STR)
			down = disp_str.down;
		if (disp_str.cpu != NONE_STR)
			cpu = disp_str.cpu;
		if (disp_str.memory != NONE_STR)
			memory = disp_str.memory;
	}
	bool IsInvalid() const
	{
		return (up == NONE_STR && down == NONE_STR && cpu == NONE_STR && memory == NONE_STR);
	}
};

//���˫�����ڵĶ���
enum class DoubleClickAction
{
	CONNECTION_INFO,	//��������
	HISTORY_TRAFFIC,	//��ʷ����ͳ��
	SHOW_MORE_INFO,		//��ʾ������Ϣ
	OPTIONS,			//ѡ������
	TASK_MANAGER,		//���������
	CHANGE_SKIN,		//����Ƥ��
	NONE				//��ִ���κζ���
};

//����
enum class Language
{
	FOLLOWING_SYSTEM,		//����ϵͳ
	ENGLISH,				//Ӣ��
	SIMPLIFIED_CHINESE,		//��������
	TRADITIONAL_CHINESE		//��������
};

//��ɫģʽ
enum class ColorMode
{
	Default, //Ĭ����ɫ
	Light	//ǳɫ
};

//����
struct FontInfo
{
	CString name;	//��������
	int size;		//�����С
	bool bold;			//����
	bool italic;		//б��
	bool underline;		//�»���
	bool strike_out;	//ɾ����
};

//���ֺ�ת��LOGFONT�ṹ�е�lfHeight
#define FONTSIZE_TO_LFHEIGHT(font_size) (-MulDiv(font_size, GetDeviceCaps(::GetDC(HWND_DESKTOP), LOGPIXELSY), 72))

//ѡ����������
struct MainConfigData
{
	bool m_always_on_top{ false };		//�����ö�
	int m_transparency{ 100 };			//����͸����
	bool m_lock_window_pos{ false };	//��������λ��
	bool m_show_more_info{ false };		//��ʾ������Ϣ
	bool m_mouse_penetrate{ false };	//��괩͸
	bool m_show_task_bar_wnd{ false };	//��ʾ����������
	bool m_hide_main_window;			//����������
	bool m_show_notify_icon{ true };	//��ʾ֪ͨ����ͼ��
	bool m_tbar_show_cpu_memory;		//������������ʾCPU���ڴ�������

	int m_position_x;	//����λ�õ�x����
	int m_position_y;	//����λ�õ�y����

	bool m_auto_select{ false };	//�Զ�ѡ������
	bool m_select_all{ false };		//ͳ���������ӵ�����
	string m_connection_name;		//��ǰѡ�����������

	wstring m_skin_name;			//ѡ���Ƥ��������
	int m_dft_notify_icon = 0;		//Ĭ�ϵ�֪ͨͼ��(��������win10����ɫ��ǳɫģʽ)
	int m_notify_icon_selected{};	//Ҫ��ʾ��֪ͨ��ͼ��
	bool m_alow_out_of_border{ false };		//�Ƿ�����������������Ļ�߽�

	bool m_show_internet_ip{ false };		//�Ƿ��ڡ��������顱�Ի�������ʾ����IP��ַ
	bool m_use_log_scale{ false };			//����ʷ����ͳ�ơ��Ի����л��Ʊ�ʾ��ʷ������ֵ�ľ���ʱ�Ƿ�ʹ�ö�������
};

//ѡ�������С����������á��͡��������������á��й��������ݣ���ʹ�ô˽ṹ�崴������
struct PublicSettingData
{
	bool specify_each_item_color{ false };		//�Ƿ�ָ��ÿ����Ŀ����ɫ
	FontInfo font;			//����
	DispStrings disp_str;	//��ʾ���ı�
	bool swap_up_down{ false };		//�����ϴ���������ʾ��λ��
	bool speed_short_mode{ false };		//������ʾ���ģʽ������С�����λ������λ����ʾ��B����
	bool separate_value_unit_with_space{ true };	//������ֵ�͵�λ�ÿո�ָ�
    bool show_tool_tip{ true };         //��ʾ�����ʾ
	bool unit_byte{ true };				//ʹ���ֽ�(B)�����Ǳ���(b)Ϊ��λ
	SpeedUnit speed_unit;		//���ٵĵ�λ
	bool hide_unit;			//���ص�λ
	bool hide_percent;		//���ذٷֺ�
	DoubleClickAction double_click_action;		//���˫������
	wstring double_click_exe;	//���˫������Ϊ4ʱ���򿪵ĳ���·����Ĭ�����������
};

#define MAIN_WND_COLOR_NUM 4		//��������ɫ����
//ѡ�������С����������á�������
struct MainWndSettingData : public PublicSettingData
{
	COLORREF text_colors[MAIN_WND_COLOR_NUM]{};		//������ɫ���ֱ�Ϊ���ϴ����������ء�����CPU�������ڴ桱����ɫ��
	bool hide_main_wnd_when_fullscreen;		//�г���ȫ������ʱ����������
};

#define TASKBAR_COLOR_NUM 8		//������������ɫ����
//ѡ�������С��������������á�������
struct TaskBarSettingData : public PublicSettingData
{
	COLORREF  back_color{ RGB(0, 0, 0) };	//������ɫ
	COLORREF transparent_color{ RGB(0, 0, 0) };		//͸��ɫ
	COLORREF status_bar_color{ RGB(0, 0, 0) };		// CPU/�ڴ� ״̬����ɫ
	COLORREF text_colors[TASKBAR_COLOR_NUM]{};		//������ɫ������Ϊ���ϴ����������ء�����CPU�������ڴ桱�ı�ǩ��������ɫ��
	int dft_back_color = 0;							//Ĭ�ϱ�����ɫ
	int dft_transparent_color = 0;					//Ĭ��͸��ɫ
	int dft_status_bar_color = 0x005A5A5A;			//Ĭ��CPU/�ڴ� ״̬����ɫ
	int dft_text_colors = 0x00ffffffU;				//Ĭ��������ɫ������Ϊ���ϴ����������ء�����CPU�������ڴ桱�ı�ǩ��������ɫ��
	bool value_right_align{ false };	//��ֵ�Ƿ��Ҷ���
	int digits_number{ 4 };				//����λ��
	bool horizontal_arrange{ true };	//ˮƽ����
	bool show_status_bar{ true };		//��ʾ CPU/�ڴ��״̬��
	bool tbar_wnd_on_left{ false };		//���Ϊtrue����������������ʾ������������ࣨ���Ϸ���
};

//ѡ�������С��������á�������
struct GeneralSettingData
{
	bool check_update_when_start{ true };
	bool auto_run{ false };
	bool allow_skin_cover_font{ true };
	bool allow_skin_cover_text{ true };
	//֪ͨ��Ϣ
	bool traffic_tip_enable{ false };		//�Ƿ�������������ʱ��ʾ
	int traffic_tip_value;					//Ҫ��ʾ�������ٽ�ֵ
	int traffic_tip_unit{};					//Ҫ��ʾ������ֵ�ĵ�λ��0: MB, 1: GB��
	bool memory_usage_tip_enable{ false };	//�Ƿ������ڴ�ʹ���ʳ�����ʾ
	int memory_tip_value;			//Ҫ��ʾ���ڴ�ʹ���ʵ��ٽ�ֵ
	//����
	Language language;

	bool show_all_interface{ true };

	bool portable_mode{ false };		//��Яģʽ�����Ϊtrue��������������ݶ����浽exe����Ŀ¼�£����򱣴浽Appdata\RomaingĿ¼��
};

enum class Alignment
{
	LEFT,
	RIGHT,
	CENTER
};

//�����ڲ�������
struct LayoutData
{
	int text_height;	//�ı�����ĸ߶�
	bool no_text;		//����ʾ��ֵǰ�ġ��ϴ����������ء����ı�
	int preview_width;		//Ԥ�����������
	int preview_height;		//Ԥ���������߶�

	//�󴰿�ģʽ����ѡ����ʾ������Ϣ��ʱ�Ĳ��֣�
	int width_l;	//���ڵĿ��
	int height_l;	//���ڵĸ߶�
	int up_x_l;		//���ϴ�����x����
	int up_y_l;		//���ϴ�����y����
	int up_width_l;		//���ϴ����Ŀ��
	Alignment up_align_l;	//���ϴ����Ķ��뷽ʽ
	bool show_up_l;			//�Ƿ���ʾ���ϴ���
	int down_x_l;			//�����ء���x����
	int down_y_l;			//�����ء���y����
	int down_width_l;		//�����ء��Ŀ��
	Alignment down_align_l;	//�����ء��Ķ��뷽ʽ
	bool show_down_l;		//�Ƿ���ʾ�����ء�
	int cpu_x_l;			//��CPU����x����
	int cpu_y_l;			//��CPU����y����
	int cpu_width_l;		//��CPU���Ŀ��
	Alignment cpu_align_l;	//��CPU���Ķ��뷽ʽ
	bool show_cpu_l;		//�Ƿ���ʾ��CPU��
	int memory_x_l;			//���ڴ桱��x����
	int memory_y_l;			//���ڴ桱��y����
	int memory_width_l;		//���ڴ桱�Ŀ��
	Alignment memory_align_l;	//���ڴ桱�Ķ��뷽ʽ
	bool show_memory_l;		//�Ƿ���ʾ���ڴ桱
	int preview_x_l;		//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��x����
	int preview_y_l;		//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��y����

	//С����ģʽ������ѡ����ʾ������Ϣ��ʱ�Ĳ��֣�
	int width_s;	//���ڵĿ��
	int height_s;	//���ڵĸ߶�
	int up_x_s;		//���ϴ�����x����
	int up_y_s;		//���ϴ�����y����
	int up_width_s;		//���ϴ����Ŀ��
	Alignment up_align_s;	//���ϴ����Ķ��뷽ʽ
	bool show_up_s;			//�Ƿ���ʾ���ϴ���
	int down_x_s;			//�����ء���x����
	int down_y_s;			//�����ء���y����
	int down_width_s;		//�����ء��Ŀ��
	Alignment down_align_s;	//�����ء��Ķ��뷽ʽ
	bool show_down_s;		//�Ƿ���ʾ�����ء�
	int cpu_x_s;			//��CPU����x����
	int cpu_y_s;			//��CPU����y����
	int cpu_width_s;		//��CPU���Ŀ��
	Alignment cpu_align_s;	//��CPU���Ķ��뷽ʽ
	bool show_cpu_s;		//�Ƿ���ʾ��CPU��
	int memory_x_s;			//���ڴ桱��x����
	int memory_y_s;			//���ڴ桱��y����
	int memory_width_s;		//���ڴ桱�Ŀ��
	Alignment memory_align_s;	//���ڴ桱�Ķ��뷽ʽ
	bool show_memory_s;		//�Ƿ���ʾ���ڴ桱
	int preview_x_s;	//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��x����
	int preview_y_s;	//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��y����
};


struct SkinData
{
	LayoutData layout;
	COLORREF text_colors[MAIN_WND_COLOR_NUM]{};		//������ɫ���ֱ�Ϊ���ϴ����������ء�����CPU�������ڴ桱����ɫ��
	bool specify_each_item_color;		//�Ƿ�ָ��ÿ����Ŀ����ɫ
	//wstring skin_author;
	DispStrings disp_str;
	//wstring font_name;
	//int font_size
	FontInfo font;			//����
};
