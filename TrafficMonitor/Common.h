#pragma once
#include "CommonData.h"
#include "CVariant.h"

class CCommon
{
public:
	CCommon();
	~CCommon();
	//��const char*�ַ���ת���ɿ��ַ��ַ���
	static wstring StrToUnicode(const char* str, bool utf8 = false);

	static string UnicodeToStr(const wchar_t* wstr, bool utf8 = false);

	static void StringNormalize(wstring& str);

	/*�������ݵĴ�Сת������KB��MB��GBΪ��λ���ַ���
	size�����ݵ��ֽ���
	����ֵ��ת������ַ���
	*/
	static CString DataSizeToString(unsigned int size, const PublicSettingData& cfg);
	static CString DataSizeToString(unsigned long long size);

	static CString KBytesToString(unsigned int kb_size);
	static CString KBytesToStringL(__int64 kb_size);

	//��������FILETIME�ṹ��ʱ���
	static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

	//��һ����־��Ϣstr_textд�뵽file_path�ļ���
	static void WriteLog(const char* str_text, LPCTSTR file_path);
	static void WriteLog(const wchar_t* str_text, LPCTSTR file_path);

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

	static bool MoveAFile(LPCTSTR exist_file, LPCTSTR new_file);

	//��������SYSTEMTIME�ṹʱ��Ĳa-b��ֻ����ʱ���֡��룩
	static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

	//��ȡ��ǰ�����Ŀ¼
	static wstring GetModuleDir();

	//��ȡsystem32�ļ��е�·��
	static wstring GetSystemDir();

	//��ȡ��ʱ�ļ��е�·��
	static wstring GetTemplateDir();

	//��ȡAppdata/Local/TrafficMonitor��Ŀ¼����������ڣ�����Զ�����
	static wstring GetAppDataConfigDir();

	//��ָ��λ�û����ı�
	static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

	////���û�ͼ�ļ�������
	//static void SetDrawArea(CDC* pDC, CRect rect);

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

	//��ȡURL������
	static bool GetURL(const wstring& url, wstring& result, bool utf8 = false);

	//��ȡ����IP��ַ��IP������
	static void GetInternetIp(wstring& ip_address, wstring& ip_location, bool global);

	static void SetRect(CRect& rect, int x, int y, int width, int height);

	//����Դ�ļ������ַ��������У�front_str��back_strΪ�����ַ���ʱ��Ҫ��ǰ��������ӵ��ַ���
	static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
	static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);

	//��ȫ�ĸ�ʽ���ַ�������format_str������<%���%>���ַ����滻�ɳ�ʼ���б�paras�е�Ԫ�أ�Ԫ��֧��int/double/LPCTSTR/CString��ʽ����Ŵ�1��ʼ
	static CString StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras);

	//����Դ�ļ��������ַ�����������Դ�ַ���������<%���%>���ַ����滻�ɿɱ�����б��еĲ���
	static CString LoadTextFormat(UINT id, const std::initializer_list<CVariant>& paras);

	//��int����ת�����ַ���
	//n��Ҫת������ֵ
	//thousand_separation���Ƿ�Ҫÿ��3λ��ʹ�ö��ŷָ�
	//is_unsigned����ֵ�Ƿ����޷��ŵ�
	static CString IntToString(int n, bool thousand_separation = false, bool is_unsigned = false);

	//ɾ���������ƺ����Bold��Light���ַ�������������Щ�ַ������������ϸ
	static void NormalizeFont(LOGFONT& font);

	//��ȫ���ַ������ƺ���
	static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);

	/// <summary>
	/// �ַ������ƶ��㷨-�༭���뷨
	/// </summary>
	/// <returns>���ص�ֵΪ0~1��Խ�����ƶ�Խ��</returns>
	static double StringSimilarDegree_LD(const string& srcString, const string& matchString);

	//�����߳�����
	static void SetThreadLanguage(Language language);

	//������ɫģʽ
	static void SetColorMode(ColorMode mode);

	static void SetDialogFont(CWnd* pDlg, CFont* pFont);
};

