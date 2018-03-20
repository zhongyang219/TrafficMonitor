//���ļ�����ȫ�ֽṹ�塢ö�����͵Ķ���
#pragma once
#include "stdafx.h"

//����ĳһ�����ʷ����
struct HistoryTraffic
{
	int year;
	int month;
	int day;
	unsigned int kBytes;	//����ʹ�õ���������KBΪ��λ��

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

//���ٵ�λ
enum class SpeedUnit
{
	AUTO,		//�Զ�
	KBPS,		//KB/s
	MBPS		//MB/s
};

#define NONE_CH L'$'		//д��Ͷ�ȡini�ļ��ַ���ʱ�����ַ���ǰ����ӵ��ַ�
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
};

//ѡ����������
struct MainWndSettingData
{
	//������
	COLORREF text_color{};		//������ɫ
	CString font_name;	//��������
	int font_size;		//�����С
	DispStrings disp_str;	//��ʾ���ı�
	bool speed_short_mode{ false };		//������ʾ���ģʽ������С�����λ������λ����ʾ��B����
	bool swap_up_down{ false };		//�����ϴ���������ʾ��λ��
	bool hide_main_wnd_when_fullscreen;		//�г���ȫ������ʱ����������
	SpeedUnit m_speed_unit;		//���ٵĵ�λ
	bool m_hide_unit;			//���ص�λ
};

struct TaskBarSettingData
{
	//����������
	COLORREF  back_color{ RGB(0, 0, 0) };	//������ɫ
	COLORREF  text_color{ RGB(255, 255, 255) };	//������ɫ
	CString  font_name;	//��������
	int  font_size;		//�����С
	DispStrings disp_str;	//��ʾ���ı�
	bool swap_up_down{ false };		//�����ϴ���������ʾ��λ��
	bool speed_short_mode{ false };		//������ʾ���ģʽ������С�����λ������λ����ʾ��B����
	bool tbar_wnd_on_left{ false };		//���Ϊtrue����������������ʾ������������ࣨ���Ϸ���
	SpeedUnit m_speed_unit;		//���ٵĵ�λ
	bool m_hide_unit;			//���ص�λ
};

struct GeneralSettingData
{
	//��������
	bool check_update_when_start{ true };
	bool auto_run{ false };
};

//�����ڲ�������
struct LayoutData
{
	int text_height;	//�ı�����ĸ߶�

	//�󴰿�ģʽ����ѡ����ʾ������Ϣ��ʱ�Ĳ��֣�
	int width_l;	//���ڵĿ��
	int height_l;	//���ڵĸ߶�
	int up_x_l;		//���ϴ�����x����
	int up_y_l;		//���ϴ�����y����
	int up_width_l;		//���ϴ����Ŀ��
	bool show_up_l;			//�Ƿ���ʾ���ϴ���
	int down_x_l;			//�����ء���x����
	int down_y_l;			//�����ء���y����
	int down_width_l;		//�����ء��Ŀ��
	bool show_down_l;		//�Ƿ���ʾ�����ء�
	int cpu_x_l;			//��CPU����x����
	int cpu_y_l;			//��CPU����y����
	int cpu_width_l;		//��CPU���Ŀ��
	bool show_cpu_l;		//�Ƿ���ʾ��CPU��
	int memory_x_l;			//���ڴ桱��x����
	int memory_y_l;			//���ڴ桱��y����
	int memory_width_l;		//���ڴ桱�Ŀ��
	bool show_memory_l;		//�Ƿ���ʾ���ڴ桱
	int preview_x_l;		//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��x����
	int preview_y_l;		//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��y����

	//С����ģʽ������ѡ����ʾ������Ϣ��ʱ�Ĳ��֣�
	int width_s;	//���ڵĿ��
	int height_s;	//���ڵĸ߶�
	int up_x_s;		//���ϴ�����x����
	int up_y_s;		//���ϴ�����y����
	int up_width_s;		//���ϴ����Ŀ��
	bool show_up_s;			//�Ƿ���ʾ���ϴ���
	int down_x_s;			//�����ء���x����
	int down_y_s;			//�����ء���y����
	int down_width_s;		//�����ء��Ŀ��
	bool show_down_s;		//�Ƿ���ʾ�����ء�
	int cpu_x_s;			//��CPU����x����
	int cpu_y_s;			//��CPU����y����
	int cpu_width_s;		//��CPU���Ŀ��
	bool show_cpu_s;		//�Ƿ���ʾ��CPU��
	int memory_x_s;			//���ڴ桱��x����
	int memory_y_s;			//���ڴ桱��y����
	int memory_width_s;		//���ڴ桱�Ŀ��
	bool show_memory_s;		//�Ƿ���ʾ���ڴ桱
	int preview_x_s;	//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��x����
	int preview_y_s;	//�ڡ�ѡ��Ƥ����������Ԥ��ͼ��y����
};