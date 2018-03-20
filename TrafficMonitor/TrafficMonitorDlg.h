
// TrafficMonitorDlg.h : ͷ�ļ�
//

#pragma once
#pragma comment (lib, "iphlpapi.lib")
#include"NetworkInfoDlg.h"
#include "afxwin.h"
#include "StaticEx.h"
#include "Common.h"
#include "TaskBarDlg.h"
#include "SkinDlg.h"
#include "HistoryTrafficDlg.h"
#include "DonateDlg.h"
#include "OptionsDlg.h"
#include "PictureStatic.h"
#include "IconSelectDlg.h"

// CTrafficMonitorDlg �Ի���
class CTrafficMonitorDlg : public CDialogEx
{
// ����
public:
	CTrafficMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTrafficMonitorDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAFFICMONITOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CMenu m_menu;		//�Ҽ��˵�����
	CMenu* m_select_connection_menu;	//��ѡ���������˵���
	NOTIFYICONDATA m_ntIcon;	//֪ͨ����ͼ��
	CTaskBarDlg* m_tBarDlg;		//���������ڵ�ָ��

	//Ƕ�׽ṹ�壬����һ���������ӵ���������������Ϣ
	struct NetWorkConection
	{
		int index;				//��������
		string description;		//��������
		unsigned int in_bytes;	//��ʼʱ�ѽ����ֽ���
		unsigned int out_bytes;	//��ʼʱ�ѷ����ֽ���
		NetWorkConection(int idx, string desc, unsigned int in_bytes, unsigned out_bytes)
			: index{ idx }, description{ desc }, in_bytes{ in_bytes }, out_bytes{out_bytes}		//���캯��
		{}
	};

	vector<NetWorkConection> m_connections;	//�����ȡ����Ҫ��ʾ����ѡ���������˵����е�������������
	MIB_IFTABLE* m_pIfTable;
	DWORD m_dwSize{};	//m_pIfTable�Ĵ�С
	int m_connection_selected{ 0 };	//Ҫ��ʾ���������ӵ����
	bool m_auto_select{ false };	//�Զ�ѡ������
	unsigned int m_in_bytes;		//��ǰ�ѽ��յ��ֽ���
	unsigned int m_out_bytes;	//��ǰ�ѷ��͵��ֽ���
	unsigned int m_last_in_bytes{};	//�ϴ��ѽ��յ��ֽ���
	unsigned int m_last_out_bytes{};	//�ϴ��ѷ��͵��ֽ���
	string m_connection_name;	//��ǰѡ�����������

	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;

	bool m_always_on_top{ false };		//�����ö�
	int m_transparency{ 100 };			//����͸����
	bool m_lock_window_pos{ false };	//��������λ��
	bool m_show_more_info{ false };		//��ʾ������Ϣ
	bool m_mouse_penetrate{ false };	//��괩͸
	bool m_show_task_bar_wnd{ false };	//��ʾ����������
	//bool m_swap_up_down{ false };		//�����ϴ���������ʾ��λ��
	//bool m_auto_run;				//�Ƿ񿪻��Զ�����
	//wstring m_start_up_path;		//��ʼ�˵��еġ��������ļ��е�·��

	bool m_first_start{ true };		//��ʼʱΪtrue���ڶ�ʱ����һ����������Ϊflase

	int m_position_x;	//����λ�õ�x����
	int m_position_y;	//����λ�õ�y����

	//int m_screen_width;		//��Ļ���
	//int m_screen_height;	//��Ļ�߶�
	CRect m_screen_rect;		//��Ļ�ķ�Χ����������������

	//int m_window_height;	//���ڸ߶�
	//int m_window_height_s;	//����ʾCPU���ڴ�������ʱ�Ĵ��ڸ߶�
	LayoutData m_layout_data;

	HBITMAP m_back_img;		//����ͼƬ

	CStaticEx m_disp_up;	//��ʾ�ϴ����ı��ؼ�
	CStaticEx m_disp_down;	//��ʾ���ص��ı��ؼ�
	CStaticEx m_disp_cpu;	//��ʾCPU�����ʵ��ı��ؼ�
	CStaticEx m_disp_memory;	//��ʾ�ڴ������ʵ��ı��ؼ�

	//COLORREF m_text_color{};		//������ɫ
	CFont m_font;			//����
	//CString m_font_name;	//��������
	//int m_font_size;		//�����С

	int m_restart_cnt{ -1 };	//���³�ʼ������
	unsigned int m_timer_cnt{};		//��ʱ����������
	int m_zero_speed_cnt{};	//�����ⲻ�����٣��ñ����ͻ��Լ�
	int m_insert_to_taskbar_cnt{};	//����ͳ�Ƴ���Ƕ���������Ĵ���
	int m_cannot_intsert_to_task_bar_warning{ true };	//ָʾ�Ƿ�����޷�Ƕ��������ʱ������ʾ��

	static unsigned int m_WM_TASKBARCREATED;	//������������Ϣ

	vector<wstring> m_skins;	//����Ƥ���ļ���·��
	int m_skin_selected{};		//ѡ���Ƥ�����
	wstring m_skin_name;		//ѡ���Ƥ��������

	int m_notify_icon_selected{};	//Ҫ��ʾ��֪ͨ��ͼ��

	SYSTEMTIME m_start_time;	//��������ʱ��ʱ��

	deque<HistoryTraffic> m_history_traffics;	//������ʷ����

	CToolTipCtrl m_tool_tips;

	bool m_connection_change_flag{ false };		//���ִ�й�IniConnection()��������flag����Ϊtrue
	bool m_cannot_save_config_warning{ true };	//ָʾ�Ƿ�����޷���������ʱ������ʾ��
	bool m_is_foreground_fullscreen{ false };	//ָʾǰ̨�����Ƿ�����ȫ����ʾ
	bool m_menu_popuped{ false };				//ָʾ��ǰ�Ƿ��в˵����ڵ���״̬


	void ShowInfo();		//���ϴ������ٶ���Ϣ��ʾ��������
	CString GetMouseTipsInfo();		//��ȡ�����ʾ��Ϣ
	void SetTransparency();			//����m_transparency��ֵ���ô���͸����
	void SetTransparency(int transparency);
	void SetAlwaysOnTop();			//����m_always_on_top��ֵ���ô����ö�
	void SetMousePenetrate();		//����m_mouse_penetrate��ֵ�����Ƿ���괩͸
	void CheckWindowPos();			//���Դ��ڵ�λ�ã��細�ڵ�λ������Ļ�⣬���ƶ�����ʹ��ȫ��������Ļ�ڣ���������λ��
	void GetScreenSize();			//��ȡ��Ļ�Ĵ�С

	void LoadConfig();	//��������
	void SaveConfig();	//��������

	void AutoSelect();		//�Զ�ѡ������
	void IniConnection();	//��ʼ������

	void CloseTaskBarWnd();	//�ر�����������
	void OpenTaskBarWnd();	//������������

	void AddNotifyIcon();		//���֪ͨ��ͼ��
	void DeleteNotifyIcon();	

	void SaveHistoryTraffic();
	void LoadHistoryTraffic();

	void _OnOptions(int tab);	//�򿪡�ѡ��Ի���Ĵ�������Ϊ��ʱ�л��ı�ǩ

	void SetItemPosition();		//������ʾ��4����Ŀ��λ��
	void GetSkinLayout();		//�ӵ�ǰƤ����ȡ��������

public:
	void ApplySettings();

protected:
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNetworkInfo();
	afx_msg void OnAlwaysOnTop();
	afx_msg void OnUpdateAlwaysOnTop(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTransparency100();
	afx_msg void OnTransparency80();
	afx_msg void OnTransparency60();
	afx_msg void OnTransparency40();
	afx_msg void OnClose();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenu(CMenu* pMenu);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLockWindowPos();
	afx_msg void OnUpdateLockWindowPos(CCmdUI *pCmdUI);
	afx_msg void OnMove(int x, int y);
protected:
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnShowNotifyIcon();
	afx_msg void OnUpdateShowNotifyIcon(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnShowCpuMemory();
	afx_msg void OnUpdateShowCpuMemory(CCmdUI *pCmdUI);
	afx_msg void OnMousePenetrate();
	afx_msg void OnUpdateMousePenetrate(CCmdUI *pCmdUI);
	//afx_msg void OnTextColor();
	afx_msg void OnShowTaskBarWnd();
	afx_msg void OnUpdateShowTaskBarWnd(CCmdUI *pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnShowCpuMemory2();
//	afx_msg void OnAutoRunWhenStart();
	afx_msg void OnHideMainWnd();
	afx_msg void OnUpdateHideMainWnd(CCmdUI *pCmdUI);
	afx_msg void OnChangeSkin();
//	afx_msg void OnUpdateAutoRunWhenStart(CCmdUI *pCmdUI);
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
};
