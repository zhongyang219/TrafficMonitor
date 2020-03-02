#include "stdafx.h"
#include "Common.h"
#include "TrafficMonitor.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}

wstring CCommon::StrToUnicode(const char* str, bool utf8)
{
	wstring result;
	int size;
	size = MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, NULL, 0);
	if (size <= 0) return wstring();
	wchar_t* str_unicode = new wchar_t[size + 1];
	MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, str_unicode, size);
	result.assign(str_unicode);
	delete[] str_unicode;
	return result;
}

string CCommon::UnicodeToStr(const wchar_t * wstr, bool utf8)
{
	string result;
	int size{ 0 };
	size = WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, NULL, 0, NULL, NULL);
	if (size <= 0) return string();
	char* str = new char[size + 1];
	WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, str, size, NULL, NULL);
	result.assign(str);
	delete[] str;
	return result;
}

void CCommon::StringNormalize(wstring & str)
{
	if (str.empty()) return;

	int size = str.size();	//�ַ����ĳ���
	if (size < 0) return;
	int index1 = 0;		//�ַ����е�1�����ǿո������ַ���λ��
	int index2 = size - 1;	//�ַ��������һ�����ǿո������ַ���λ��
	while (index1 < size && str[index1] >= 0 && str[index1] <= 32)
		index1++;
	while (index2 >= 0 && str[index2] >= 0 && str[index2] <= 32)
		index2--;
	if (index1 > index2)	//���index1 > index2��˵���ַ���ȫ�ǿո������ַ�
		str.clear();
	else if (index1 == 0 && index2 == size - 1)	//���index1��index2��ֵ�ֱ�Ϊ0��size - 1��˵���ַ���ǰ��û�пո������ַ���ֱ�ӷ���
		return;
	else
		str = str.substr(index1, index2 - index1 + 1);
}

CString CCommon::DataSizeToString(unsigned int size, const PublicSettingData& cfg)
{
	//CString str;
	CString value_str, unit_str;
	if (!cfg.unit_byte)		//���ʹ�ñ���(bit)Ϊ��λ������ֵ����8
	{
		size *= 8;
	}
	switch (cfg.speed_unit)
	{
	case SpeedUnit::AUTO:
		if (cfg.speed_short_mode)
		{
			if (size < 1024 * 10)					//10KB������KBΪ��λ������1λС��
			{
				value_str.Format(_T("%.1f"), size / 1024.0f);
				unit_str = _T("K");
			}
			else if (size < 1024 * 1000)			//1000KB������KBΪ��λ����������
			{
				value_str.Format(_T("%.0f"), size / 1024.0f);
				unit_str = _T("K");
			}
			else if (size < 1024 * 1024 * 1000)		//1000MB������MBΪ��λ������1λС��
			{
				value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
				unit_str = _T("M");
			}
			else
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
				unit_str = _T("G");
			}
		}
		else
		{
			if (size < 1024 * 10)					//10KB������KBΪ��λ������2λС��
			{
				value_str.Format(_T("%.2f"), size / 1024.0f);
				unit_str = _T("KB");
			}
			else if (size < 1024 * 1000)			//1000KB������KBΪ��λ������1λС��
			{
				value_str.Format(_T("%.1f"), size / 1024.0f);
				unit_str = _T("KB");
			}
			else if (size < 1024 * 1024 * 1000)		//1000MB������MBΪ��λ������2λС��
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
				unit_str = _T("MB");
			}
			else
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
				unit_str = _T("GB");
			}
		}
		break;
	case SpeedUnit::KBPS:
		if (cfg.speed_short_mode)
		{
			if (size < 1024 * 10)					//10KB���±���1λС��
				value_str.Format(_T("%.1f"), size / 1024.0f);
			else					//10KB���ϱ�������
				value_str.Format(_T("%.0f"), size / 1024.0f);
			if (!cfg.hide_unit)
				unit_str = _T("K");
		}
		else
		{
			if (size < 1024 * 10)					//10KB���±���2λС��
				value_str.Format(_T("%.2f"), size / 1024.0f);
			else			//10KB���ϱ���1λС��
				value_str.Format(_T("%.1f"), size / 1024.0f);
			if (!cfg.hide_unit)
				unit_str = _T("KB");
		}
		break;
	case SpeedUnit::MBPS:
		if (cfg.speed_short_mode)
		{
			value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
			if (!cfg.hide_unit)
				unit_str = _T("M");
		}
		else
		{
			value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
			if (!cfg.hide_unit)
				unit_str = _T("MB");
		}
		break;
	}
	CString str;
	if (cfg.separate_value_unit_with_space && !cfg.hide_unit)
		str = value_str + _T(' ') + unit_str;
	else
		str = value_str + unit_str;
	if (!cfg.unit_byte)	
	{
		if (cfg.speed_short_mode && !cfg.hide_unit)
			str += _T('b');		//���ʹ�ñ���(bit)Ϊ��λ����ʹ���������ټ��ģʽ��Ҳ����b����ʾ����
		else
			str.Replace(_T('B'), _T('b'));	//���ʹ�ñ���(bit)Ϊ��λ����B�滻��b
	}
	return str;
}

CString CCommon::DataSizeToString(unsigned long long size)
{
	CString str;
	if (size < 1024 * 10)					//10KB������KBΪ��λ������2λС��
		str.Format(_T("%.2f KB"), size / 1024.0);
	else if (size < 1024 * 1024)			//1MB������KBΪ��λ������1λС��
		str.Format(_T("%.1f KB"), size / 1024.0);
	else if (size < 1024 * 1024 * 1024)		//1GB������MBΪ��λ������2λС��
		str.Format(_T("%.2f MB"), size / 1024.0 / 1024.0);
	else
		str.Format(_T("%.2f GB"), size / 1024.0 / 1024.0 / 1024.0);
	return str;
}

CString CCommon::KBytesToString(unsigned int kb_size)
{
	CString k_bytes_str;
	if (kb_size < 1024)
		k_bytes_str.Format(_T("%d KB"), kb_size);
	else if (kb_size < 1024 * 1024)
		k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
	else
		k_bytes_str.Format(_T("%.2f GB"), kb_size / 1024.0 / 1024.0);
	return k_bytes_str;
}

CString CCommon::KBytesToStringL(__int64 kb_size)
{
	CString k_bytes_str;
	if (kb_size < 1024)
		k_bytes_str.Format(_T("%d KB"), kb_size);
	else if (kb_size < 1024 * 1024)
		k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
	else
		k_bytes_str.Format(_T("%.2f GB"), kb_size / (1024.0 * 1024.0));
	return k_bytes_str;
}

__int64 CCommon::CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
	return b - a;
}

void CCommon::WriteLog(const char* str_text, LPCTSTR file_path)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	char buff[32];
	sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	ofstream file{ file_path, std::ios::app };	//��׷�ӵķ�ʽ����־�ļ�
	file << buff;
	file << str_text << std::endl;
}

void CCommon::WriteLog(const wchar_t * str_text, LPCTSTR file_path)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	char buff[32];
	sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	ofstream file{ file_path, std::ios::app };	//��׷�ӵķ�ʽ����־�ļ�
	file << buff;
	file << UnicodeToStr(str_text).c_str() << std::endl;
}

BOOL CCommon::CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName, LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
{
	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink����ָ��
	IPersistFile   *ppf; //IPersisFil����ָ��

						 //����IShellLink����
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//��IShellLink�����л�ȡIPersistFile�ӿ�
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	TCHAR file_path[MAX_PATH];
	GetModuleFileName(NULL, file_path, MAX_PATH);

	//Ŀ��
	if (lpszFileName == NULL)
		pLink->SetPath(file_path);
	else
		pLink->SetPath(lpszFileName);

	//����Ŀ¼
	if (lpszWorkDir != NULL)
	{
		pLink->SetWorkingDirectory(lpszWorkDir);
	}
	else
	{
		//���ù���Ŀ¼Ϊ��ݷ�ʽĿ������λ��
		TCHAR workDirBuf[MAX_PATH]{};
		if (lpszFileName == NULL)
			//wcscpy_s(workDirBuf, file_path);
			WStringCopy(workDirBuf, 260, file_path, 260);
		else
			//wcscpy_s(workDirBuf, lpszFileName);
			WStringCopy(workDirBuf, 260, lpszFileName);
		LPTSTR pstr = wcsrchr(workDirBuf, _T('\\'));
		*pstr = _T('\0');
		pLink->SetWorkingDirectory(workDirBuf);
	}

	//��ݼ�
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//��ע
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//��ʾ��ʽ
	pLink->SetShowCmd(iShowCmd);


	//��ݷ�ʽ��·�� + ����
	wchar_t szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //ָ���˿�ݷ�ʽ������
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else
	{
		//û��ָ�����ƣ��ʹ�ȡָ���ļ����ļ�����Ϊ��ݷ�ʽ���ơ�
		const wchar_t *pstr;
		if (lpszFileName != NULL)
			pstr = wcsrchr(lpszFileName, L'\\');
		else
			pstr = wcsrchr(file_path, L'\\');

		if (pstr == NULL)
		{
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//ע���׺��Ҫ��.exe��Ϊ.lnk
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, pstr);
		int nLen = wcslen(szBuffer);
		szBuffer[nLen - 3] = L'l';
		szBuffer[nLen - 2] = L'n';
		szBuffer[nLen - 1] = L'k';
	}
	//�����ݷ�ʽ��ָ��Ŀ¼��
	//WCHAR  wsz[MAX_PATH];  //����Unicode�ַ���
	//MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(szBuffer, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

wstring CCommon::GetStartUpPath()
{
	LPITEMIDLIST ppidl;
	TCHAR pszStartUpPath[MAX_PATH]{};
	if (SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &ppidl) == S_OK)
	{
		SHGetPathFromIDList(ppidl, pszStartUpPath);
		CoTaskMemFree(ppidl);
	}
	return wstring(pszStartUpPath);
}

void CCommon::GetFiles(const wchar_t* path, vector<wstring>& files)
{
	//�ļ���� 
	intptr_t hFile = 0;
	//�ļ���Ϣ����Unicode����ʹ��_wfinddata_t�����ֽ��ַ���ʹ��_finddata_t��
	_wfinddata_t fileinfo;
	wstring file_name;
	if ((hFile = _wfindfirst(wstring(path).append(L"\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			file_name.assign(fileinfo.name);
			if (file_name != L"." && file_name != L"..")
				//files.push_back(wstring(path) + L"\\" + file_name);  //���ļ�������(����"."��"..")
				files.push_back(L"\\" + file_name);  //���ļ�������(����"."��"..")
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

bool CCommon::FileExist(LPCTSTR file_name)
{
	_wfinddata_t fileinfo;
	return (_wfindfirst(file_name, &fileinfo) != -1);
}

bool CCommon::MoveAFile(LPCTSTR exist_file, LPCTSTR new_file)
{
	if(!FileExist(exist_file))
		return false;
	//if (FileExist(new_file))		//���Ŀ���ļ��Ѿ����ڣ�����ɾ����
	//	DeleteFile(new_file);
	return (MoveFile(exist_file, new_file) != 0);
}

SYSTEMTIME CCommon::CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b)
{
	SYSTEMTIME result{};
	short hour = a.wHour - b.wHour;
	short minute = a.wMinute - b.wMinute;
	short second = a.wSecond - b.wSecond;

	if (second < 0)
	{
		second += 60;
		minute--;
	}

	if (minute < 0)
	{
		minute += 60;
		hour--;
	}

	if (hour < 0)
	{
		hour += 24;
	}
	result.wHour = hour;
	result.wMinute = minute;
	result.wSecond = second;
	return result;
}

wstring CCommon::GetModuleDir()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	size_t index;
	wstring current_path{ path };
	index = current_path.find_last_of(L'\\');
	current_path = current_path.substr(0, index + 1);
	return current_path;
}

wstring CCommon::GetSystemDir()
{
	wchar_t buff[MAX_PATH];
	GetSystemDirectory(buff, MAX_PATH);
	return wstring(buff);
}

wstring CCommon::GetTemplateDir()
{
	wstring result;
	wchar_t buff[MAX_PATH];
	GetTempPath(MAX_PATH, buff);		//��ȡ��ʱ�ļ��е�·��
	result = buff;
	if (result.back() != L'\\' && result.back() != L'/')		//ȷ��·��������б��
		result.push_back(L'\\');
	return result;
}

wstring CCommon::GetAppDataConfigDir()
{
	LPITEMIDLIST ppidl;
	TCHAR pszAppDataPath[MAX_PATH];
	if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &ppidl) == S_OK)
	{
		SHGetPathFromIDList(ppidl, pszAppDataPath);
		CoTaskMemFree(ppidl);
	}
	wstring app_data_path{ pszAppDataPath };		//��ȡ��C:/User/�û���/AppData/Roaming·��
	CreateDirectory(app_data_path.c_str(), NULL);		//���Roaming�����ڣ��򴴽���
	app_data_path += L"\\TrafficMonitor\\";
	CreateDirectory(app_data_path.c_str(), NULL);		//���C:/User/�û���/AppData/Roaming/TrafficMonitor�����ڣ��򴴽���

	return app_data_path;
}

void CCommon::DrawWindowText(CDC * pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color)
{
	pDC->SetTextColor(color);
	//m_pDC->SetBkMode(TRANSPARENT);
	//�ñ���ɫ����������
	pDC->FillSolidRect(rect, back_color);
	pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

}

//void CCommon::SetDrawArea(CDC * pDC, CRect rect)
//{
//	CRgn rgn;
//	rgn.CreateRectRgnIndirect(rect);
//	pDC->SelectClipRgn(&rgn);
//}


bool CCommon::IsForegroundFullscreen()
{
	bool bFullscreen{ false };		//����ָʾǰ̨�����Ƿ���ȫ��
	HWND hWnd;
	RECT rcApp;
	RECT rcDesk;
	hWnd = GetForegroundWindow();	//��ȡ��ǰ�������û�������ǰ̨���ھ��
	TCHAR buff[256];
	GetClassName(hWnd, buff, 256);		//��ȡǰ̨���ڵ�����
	CString class_name{ buff };
	if (hWnd != GetDesktopWindow() && class_name!=_T("WorkerW") && hWnd != GetShellWindow())//���ǰ̨���ڲ������洰�ڣ�Ҳ���ǿ���̨����
	{
		GetWindowRect(hWnd, &rcApp);	//��ȡǰ̨���ڵ�����
		GetWindowRect(GetDesktopWindow(), &rcDesk);	//�������洰�ھ������ȡ������Ļ������
		if (rcApp.left <= rcDesk.left && //���ǰ̨���ڵ�������ȫ����ס���洰�ڣ��ͱ�ʾǰ̨������ȫ����
			rcApp.top <= rcDesk.top &&
			rcApp.right >= rcDesk.right &&
			rcApp.bottom >= rcDesk.bottom)
		{
			bFullscreen = true;
		}
	}//���ǰ̨���������洰�ڣ������ǿ���̨���ڣ���ֱ�ӷ��ز���ȫ��
	return bFullscreen;
}

bool CCommon::CopyStringToClipboard(const wstring & str)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		EmptyClipboard();
		size_t size = (str.size() + 1) * 2;
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
		memcpy_s(GlobalLock(clipbuffer), size, str.c_str(), size);
		GlobalUnlock(clipbuffer);
		if (SetClipboardData(CF_UNICODETEXT, clipbuffer) == NULL)
			return false;
		CloseClipboard();
		return true;
	}
	else return false;
}


bool CCommon::GetURL(const wstring & url, wstring & result, bool utf8)
{
	bool succeed{ false };
    CInternetSession* pSession{};
    CHttpFile* pfile{};
	try
	{
        pSession = new CInternetSession();
		pfile = (CHttpFile *)pSession->OpenURL(url.c_str());
		DWORD dwStatusCode;
		pfile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode == HTTP_STATUS_OK)
		{
			CString content;
			CString data;
			while (pfile->ReadString(data))
			{
				content += data;
			}
			result = StrToUnicode((const char*)content.GetString(), utf8);
			succeed = true;
		}
		pfile->Close();
		delete pfile;
        pSession->Close();
	}
	catch (CInternetException* e)
	{
		if (pfile != nullptr)
		{
			pfile->Close();
			delete pfile;
		}
        if (pSession != nullptr)
            pSession->Close();
		succeed = false;
		e->Delete();		//û����������ڴ�й¶
        SAFE_DELETE(pSession);
	}
    SAFE_DELETE(pSession);
    return succeed;
}

void CCommon::GetInternetIp(wstring& ip_address, wstring& ip_location, bool global)
{
	wstring web_page;
	if (GetURL(L"https://ip.cn/", web_page, true))
	{
#ifdef _DEBUG
		ofstream file{ L".\\IP_web_page.log" };
		file << UnicodeToStr(web_page.c_str()) << std::endl;
#endif // _DEBUG
		size_t index, index1;
		index = web_page.find(L"<code>");
		index1 = web_page.find(L"</code>", index + 6);
		if (index == wstring::npos || index1 == wstring::npos)
			ip_address.clear();
		else
			ip_address = web_page.substr(index + 6, index1 - index - 6);	//��ȡIP��ַ
		if (ip_address.size() > 15 || ip_address.size() < 7)		//IP��ַ�15���ַ������7���ַ�
			ip_address.clear();

		//��ȡIP��ַ������
		if (!global)
		{
			index = web_page.find(L"<code>", index1 + 7);
			index1 = web_page.find(L"</code>", index + 6);
			if (index == wstring::npos || index1 == wstring::npos)
				ip_location.clear();
			else
				ip_location = web_page.substr(index + 6, index1 - index - 6);
		}
		else
		{
			index = web_page.find(L"GeoIP", index1 + 7);
			index1 = web_page.find(L"</p>", index + 6);
			if (index == wstring::npos || index1 == wstring::npos)
				ip_location.clear();
			else
				ip_location = web_page.substr(index + 7, index1 - index - 7);
		}
	}
	else
	{
		ip_address.clear();
	}
}

void CCommon::SetRect(CRect & rect, int x, int y, int width, int height)
{
	rect.left = x;
	rect.top = y;
	rect.right = x + width;
	rect.bottom = y + height;
}

CString CCommon::LoadText(UINT id, LPCTSTR back_str)
{
	CString str;
	str.LoadString(id);
	if (back_str != nullptr)
		str += back_str;
	return str;
}

CString CCommon::LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str)
{
	CString str;
	str.LoadString(id);
	if (back_str != nullptr)
		str += back_str;
	if (front_str != nullptr)
		str = front_str + str;
	return str;
}

CString CCommon::StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras)
{
	CString str_rtn = format_str;
	int index = 1;
	for (const auto& para : paras)
	{
		CString para_str = para.ToString();
		CString format_para;
		format_para.Format(_T("<%%%d%%>"), index);
		str_rtn.Replace(format_para, para_str);

		index++;
	}
	return str_rtn;
}

CString CCommon::LoadTextFormat(UINT id, const std::initializer_list<CVariant>& paras)
{
	CString str;
	str.LoadString(id);
	return StringFormat(str.GetString(), paras);
}

CString CCommon::IntToString(int n, bool thousand_separation, bool is_unsigned)
{
	CString str;
	if(is_unsigned)
		str.Format(_T("%u"), static_cast<unsigned int>(n));
	else
		str.Format(_T("%d"), n);
	int length{ str.GetLength() };
	int count{};
	if (thousand_separation)
	{
		for (int i{ length - 1 }; i > 0; i--)
		{
			count++;
			if (count % 3 == 0)
				str.Insert(i, _T(","));
		}
	}
	return str;
}

void CCommon::NormalizeFont(LOGFONT & font)
{
	wstring name;
	wstring style;
	name = font.lfFaceName;
	if (name.empty())
		return;
	if (name.back() == L' ')
		name.pop_back();
	size_t index = name.rfind(L' ');
	if (index == wstring::npos)
		return;
	style = name.substr(index + 1);
	bool style_acquired = false;
	if (style == L"Light")
	{
		font.lfWeight = FW_LIGHT;
		style_acquired = true;
	}
	else if (style == L"Semilight")
	{
		font.lfWeight = 350;
		style_acquired = true;
	}
	else if (style == L"Semibold")
	{
		font.lfWeight = FW_SEMIBOLD;
		style_acquired = true;
	}
	else if (style == L"Bold")
	{
		font.lfWeight = FW_BOLD;
		style_acquired = true;
	}
	else if (style == L"Black")
	{
		font.lfWeight = FW_BLACK;
		style_acquired = true;
	}

	if (style_acquired)
	{
		name = name.substr(0, index);
	}
	//wcsncpy_s(font.lfFaceName, name.c_str(), 32);
	WStringCopy(font.lfFaceName, 32, name.c_str(), name.size());
}

void CCommon::WStringCopy(wchar_t * str_dest, int dest_size, const wchar_t * str_source, int source_size)
{
	if (dest_size <= 0)
		return;
	if (source_size <= 0 || str_source == nullptr)
	{
		str_dest[0] = L'\0';
		return;
	}
	int i;
	for (i = 0; i < dest_size && i < source_size && str_source[i] != L'\0'; i++)
		str_dest[i] = str_source[i];
	//ȷ��Ŀ���ַ���ĩβ��һ��\0
	int copy_cnt = i;
	if (copy_cnt < dest_size)
		str_dest[copy_cnt] = L'\0';
	else
		str_dest[dest_size - 1] = L'\0';
}

double CCommon::StringSimilarDegree_LD(const string & srcString, const string & matchString)
{
	int n = srcString.size();
	int m = matchString.size();
	//int[, ] d = new int[n + 1, m + 1]; // matrix
	vector<vector<int>> d(n + 1, vector<int>(m + 1));
	int cost; // cost
			  // Step 1���������һ���ַ�������Ϊ0�������ƶ�Ϊ1����
			  //if (n == 0) return (double)m / max(srcString.size(), matchString.size());
			  //if (m == 0) return (double)n / max(srcString.size(), matchString.size());
	if (n == 0 || m == 0) return 0.0;	//�������һ���ַ�������Ϊ0�������ƶ�Ϊ0
										// Step 2
	for (int i = 0; i <= n; d[i][0] = i++);
	for (int j = 0; j <= m; d[0][j] = j++);
	// Step 3
	for (int i = 1; i <= n; i++)
	{
		//Step 4
		for (int j = 1; j <= m; j++)
		{
			// Step 5
			cost = (matchString.substr(j - 1, 1) == srcString.substr(i - 1, 1) ? 0 : 1);
			// Step 6
			d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + cost);
		}
	}

	// Step 7
	double ds = 1 - (double)d[n][m] / max(srcString.size(), matchString.size());

	return ds;
}

void CCommon::SetThreadLanguage(Language language)
{
	switch (language)
	{
	case Language::ENGLISH: SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)); break;
	case Language::SIMPLIFIED_CHINESE: SetThreadUILanguage(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)); break;
	case Language::TRADITIONAL_CHINESE: SetThreadUILanguage(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)); break;
	default: break;
	}
}

void CCommon::SetColorMode(ColorMode mode)
{
	switch (mode)
	{
	case ColorMode::Default:
		CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0;
		CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0;
		CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x005A5A5A;
		CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00ffffffU;
		CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 0;
		break;
	case ColorMode::Light:
		CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0x00D2D2D2;
		CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0x00D2D2D2;
		CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x00A5A5A5;
		CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00000000U;
		CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 4;
		break;
	default:
		break;
	}
}

void CCommon::SetDialogFont(CWnd * pDlg, CFont * pFont)
{
	if (pDlg->GetSafeHwnd() != NULL)
	{
		CWnd *pWndChild;
		pWndChild = pDlg->GetWindow(GW_CHILD);
		while (pWndChild)
		{
			pWndChild->SetFont(pFont);
			pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
		}
	}
}
