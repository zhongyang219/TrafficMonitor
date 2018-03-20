#pragma once
#include "CommonData.h"

class CCommon
{
public:
	CCommon();
	~CCommon();
	//��const char*�ַ���ת���ɿ��ַ��ַ���
	static wstring StrToUnicode(const char* str, bool utf8 = false);

	static string UnicodeToStr(const wchar_t* wstr);

	//��һ��int��������д��ini�ļ�������ɹ��򷵻�true
	static bool WritePrivateProfileIntW(const wchar_t * AppName, const wchar_t * KeyName, int value, const wchar_t * Path);
	//��һ��wstring��������д��ini�ļ�������ɹ��򷵻�true
	static bool WriteIniStringW(const wchar_t* AppName, const wchar_t* KeyName, wstring str, const wchar_t* path);
	//��int�ļ���ȡһ��wstring��������
	static wstring GetIniStringW(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str, const wchar_t* path);

	/*�������ݵĴ�Сת������KB��MB��GBΪ��λ���ַ���
	size�����ݵĴ�С����λΪ�ֽ�
	short_mode���Ƿ�ʹ�þ���ģʽ����СС����λ������λ����ʾ��B����
	speed_unit�����ݵĵ�λ���������Զ���KB��MB
	hide_unit���Ƿ����ص�λ
	����ֵ��ת������ַ���
	*/
	static CString DataSizeToString(unsigned int size, bool short_mode = false, SpeedUnit unit = SpeedUnit::AUTO, bool hide_unit = false);

	static CString KBytesToString(unsigned int kb_size);

	//��������FILETIME�ṹ��ʱ���
	static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

	//��һ����־��Ϣstr_textд�뵽file_path�ļ���
	static void WriteLog(const char* str_text, LPCTSTR file_path);

	/*
	�������ܣ���ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ
	����������
	lpszLnkFileDir  ָ��Ŀ¼������ΪNULL��
	lpszFileName    ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ���
	lpszLnkFileName ��ݷ�ʽ���ƣ�ΪNULL��ʾEXE�ļ�����
	wHotkey         Ϊ0��ʾ�����ÿ�ݼ�
	pszDescription  ��ע
	iShowCmd        ���з�ʽ��Ĭ��Ϊ���洰��
	*/
	static BOOL CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);

	//��ȡ��ʼ�˵������г����еġ�������Ŀ¼��·��
	static wstring GetStartUpPath();

	//��ȡpath·���µ��ļ����ļ��У������ļ����ļ������Ʊ�����files�����С�
	static void GetFiles(const wchar_t* path, vector<wstring>& files);

	//�ж�һ���ļ��Ƿ����
	static bool FileExist(LPCTSTR file_name);

	//��������SYSTEMTIME�ṹʱ��Ĳa-b��ֻ����ʱ���֡��룩
	static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

	static wstring GetExePath();

	//��ָ��λ�û����ı�
	static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

	////Ϊһ��Static�ؼ����ָ������ɫ
	//static void FillStaticColor(CStatic& static_ctr, COLORREF color);

	//���û�ͼ�ļ�������
	static void SetDrawArea(CDC* pDC, CRect rect);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* �������ƣ�IsForegroundFullscreen
	* ����˵�����жϵ�ǰ�������û�������ǰ̨�����Ƿ���ȫ���ġ�
	* ����˵������
	* ����˵����true���ǡ�
	false����
	* �̰߳�ȫ����
	* ����������IsForegroundFullscreen ()����ʾ�жϵ�ǰ�������û�������ǰ̨�����Ƿ���ȫ���ġ�
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	static bool IsForegroundFullscreen();

	//��һ���ַ������浽������
	static bool CopyStringToClipboard(const wstring& str);

	////�ж������Ƿ�տ������ڸտ�����time�����ڷ���true�����򷵻�false���������˿�������ʱ���˺������ܻ��޷��õ���ȷ�Ľ����
	//static bool WhenStart(int time, bool write_log = false);

	////��ʾ�����ʾ
	//static CString GetMouseTipsInfo(__int64 today_traffic, int cpu_usage, int memory_usage, int used_memory, int total_memory, bool show_cpu_memory);

	//��ȡWindows�汾
	static void GetWindowsVersion(int& major_version, int& minor_version, int& build_number);

	//�жϵ�ǰWindows�汾�Ƿ�ΪWin10�＾�����߸��»���µİ汾
	static bool IsWindows10FallCreatorOrLater();

	//��ȡURL������
	static bool GetURL(const wstring& url, wstring& result);
};

