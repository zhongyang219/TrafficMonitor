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

string CCommon::UnicodeToStr(const wchar_t* wstr, bool utf8)
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

wstring CCommon::AsciiToUnicode(const string& str)
{
    std::wstring result;
    result.resize(str.size());
    for (size_t i{}; i < str.size(); i++)
        result[i] = str[i];
    return result;
}

string CCommon::AsciiToStr(const std::wstring& wstr)
{
    std::string result;
    result.resize(wstr.size());
    for (size_t i{}; i < wstr.size(); i++)
        result[i] = static_cast<char>(wstr[i]);
    return result;
}

template<class T>
static void _StringNormalize(T& str)
{
    if (str.empty()) return;

    int size = static_cast<int>(str.size());  //字符串的长度
    if (size < 0) return;
    int index1 = 0;     //字符串中第1个不是空格或控制字符的位置
    int index2 = size - 1;  //字符串中最后一个不是空格或控制字符的位置
    while (index1 < size && str[index1] >= 0 && str[index1] <= 32)
        index1++;
    while (index2 >= 0 && str[index2] >= 0 && str[index2] <= 32)
        index2--;
    if (index1 > index2)    //如果index1 > index2，说明字符串全是空格或控制字符
        str.clear();
    else if (index1 == 0 && index2 == size - 1) //如果index1和index2的值分别为0和size - 1，说明字符串前后没有空格或控制字符，直接返回
        return;
    else
        str = str.substr(index1, index2 - index1 + 1);
}

void CCommon::StringNormalize(std::string& str)
{
    _StringNormalize(str);
}

void CCommon::StringNormalize(std::wstring& str)
{
    _StringNormalize(str);
}

template<class T>
static void _StringSplit(const T& str, wchar_t div_ch, vector<T>& results, bool skip_empty = true, bool trim = true)
{
    results.clear();
    size_t split_index = -1;
    size_t last_split_index = -1;
    while (true)
    {
        split_index = str.find(div_ch, split_index + 1);
        T split_str = str.substr(last_split_index + 1, split_index - last_split_index - 1);
        if (trim)
            _StringNormalize(split_str);
        if (!split_str.empty() || !skip_empty)
            results.push_back(split_str);
        if (split_index == wstring::npos)
            break;
        last_split_index = split_index;
    }
}

void CCommon::StringSplit(const std::string& str, char div_ch, vector<std::string>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_ch, results, skip_empty, trim);
}

void CCommon::StringSplit(const std::wstring& str, wchar_t div_ch, vector<std::wstring>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_ch, results, skip_empty, trim);
}

template<class T>
static void _StringSplit(const T& str, const T& div_str, vector<T>& results, bool skip_empty = true, bool trim = true)
{
    results.clear();
    size_t split_index = 0 - div_str.size();
    size_t last_split_index = 0 - div_str.size();
    while (true)
    {
        split_index = str.find(div_str, split_index + div_str.size());
        T split_str = str.substr(last_split_index + div_str.size(), split_index - last_split_index - div_str.size());
        if (trim)
            _StringNormalize(split_str);
        if (!split_str.empty() || !skip_empty)
            results.push_back(split_str);
        if (split_index == wstring::npos)
            break;
        last_split_index = split_index;
    }
}

void CCommon::StringSplit(const std::string& str, const std::string& div_str, vector<std::string>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_str, results, skip_empty, trim);
}

void CCommon::StringSplit(const std::wstring& str, const std::wstring& div_str, vector<std::wstring>& results, bool skip_empty, bool trim)
{
    _StringSplit(str, div_str, results, skip_empty, trim);
}

template<class T>
static bool _StringTransform(T& str, bool upper)
{
    if (str.empty()) return false;
    if (upper)
    {
        for (auto& ch : str)
        {
            {
                if (ch >= 'a' && ch <= 'z')
                    ch -= 32;
            }
        }
    }
    else
    {
        for (auto& ch : str)
        {
            if (ch >= 'A' && ch <= 'Z')
                ch += 32;
        }
    }
    return true;
}

bool CCommon::StringTransform(std::string& str, bool upper)
{
    return _StringTransform(str, upper);
}

bool CCommon::StringTransform(std::wstring& str, bool upper)
{
    return _StringTransform(str, upper);
}

bool CCommon::GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary /*= true*/)
{
    std::ifstream file{ file_path, (binary ? std::ios::binary : std::ios::in) };
    if (file.fail())
        return false;
    //获取文件长度
    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);

    char* buff = new char[length];
    file.read(buff, length);
    file.close();

    contents_buff.assign(buff, length);
    delete[] buff;

    return true;
}

const char* CCommon::GetFileContent(const wchar_t* file_path, size_t& length, bool binary /*= true*/)
{
    std::ifstream file{ file_path, (binary ? std::ios::binary : std::ios::in) };
    length = 0;
    if (file.fail())
        return nullptr;
    //获取文件长度
    file.seekg(0, file.end);
    length = file.tellg();
    file.seekg(0, file.beg);

    char* buff = new char[length];
    file.read(buff, length);
    file.close();

    return buff;
}

CString CCommon::DataSizeToString(unsigned long long size, const PublicSettingData& cfg)
{
    //CString str;
    CString value_str, unit_str;
    if (!cfg.unit_byte)     //如果使用比特(bit)为单位，则数值乘以8
    {
        size *= 8;
    }
    switch (cfg.speed_unit)
    {
    case SpeedUnit::AUTO:
        if (cfg.speed_short_mode)
        {
            if (size < 1024 * 10)                   //10KB以下以KB为单位，保留1位小数
            {
                value_str.Format(_T("%.1f"), size / 1024.0f);
                unit_str = _T("K");
            }
            else if (size < 1024 * 1000)            //1000KB以下以KB为单位，保留整数
            {
                value_str.Format(_T("%.0f"), size / 1024.0f);
                unit_str = _T("K");
            }
            else if (size < 1024 * 1024 * 1000)     //1000MB以下以MB为单位，保留1位小数
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
            if (size < 1024 * 10)                   //10KB以下以KB为单位，保留2位小数
            {
                value_str.Format(_T("%.2f"), size / 1024.0f);
                unit_str = _T("KB");
            }
            else if (size < 1024 * 1000)            //1000KB以下以KB为单位，保留1位小数
            {
                value_str.Format(_T("%.1f"), size / 1024.0f);
                unit_str = _T("KB");
            }
            else if (size < 1024 * 1024 * 1000)     //1000MB以下以MB为单位，保留2位小数
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
            if (size < 1024 * 10)                   //10KB以下保留1位小数
                value_str.Format(_T("%.1f"), size / 1024.0f);
            else                    //10KB以上保留整数
                value_str.Format(_T("%.0f"), size / 1024.0f);
            if (!cfg.hide_unit)
                unit_str = _T("K");
        }
        else
        {
            if (size < 1024 * 10)                   //10KB以下保留2位小数
                value_str.Format(_T("%.2f"), size / 1024.0f);
            else            //10KB以上保留1位小数
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
            str += _T('b');     //如果使用比特(bit)为单位，即使设置了网速简洁模式，也将“b”显示出来
        else
            str.Replace(_T('B'), _T('b'));  //如果使用比特(bit)为单位，将B替换成b
    }
    return str;
}

CString CCommon::DataSizeToString(unsigned long long size, bool with_space)
{
    CString str;
    if (size < 1024 * 10)                   //10KB以下以KB为单位，保留2位小数
        str.Format(_T("%.2f KB"), size / 1024.0);
    else if (size < 1024 * 1024)            //1MB以下以KB为单位，保留1位小数
        str.Format(_T("%.1f KB"), size / 1024.0);
    else if (size < 1024 * 1024 * 1024)     //1GB以下以MB为单位，保留2位小数
        str.Format(_T("%.2f MB"), size / 1024.0 / 1024.0);
    else if (size < 1024ll * 1024 * 1024 * 1024)
        str.Format(_T("%.2f GB"), size / 1024.0 / 1024.0 / 1024.0);
    else
        str.Format(_T("%.2f TB"), size / 1024.0 / 1024.0 / 1024.0 / 1024.0);
    if (!with_space)
        str.Remove(_T(' '));
    return str;
}

CString CCommon::TemperatureToString(float temperature, const PublicSettingData& cfg)
{
    CString str_val;
    if (temperature <= 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%d"), static_cast<int>(temperature));
    if (cfg.separate_value_unit_with_space)
        str_val += _T(' ');
    str_val += _T("°C");
    return str_val;
}

CString CCommon::UsageToString(int usage, const PublicSettingData& cfg)
{
    CString str_val;
    if (usage < 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%d"), usage);
    if (!cfg.hide_percent)
    {
        if (cfg.separate_value_unit_with_space)
            str_val += _T(' ');
        str_val += _T('%');
    }
    return str_val;
}

CString CCommon::FreqToString(float freq, const PublicSettingData& cfg)
{
    CString str_val;
    if (freq < 0)
        str_val = _T("--");
    else
        str_val.Format(_T("%.2f"), freq);
    if (cfg.separate_value_unit_with_space)
        str_val += _T(' ');
    str_val += _T("GHz");
    return str_val;
}
//CString CCommon::KBytesToString(unsigned int kb_size)
//{
//  CString k_bytes_str;
//  if (kb_size < 1024)
//      k_bytes_str.Format(_T("%d KB"), kb_size);
//  else if (kb_size < 1024 * 1024)
//      k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
//  else if (kb_size < 1024 * 1024 * 1024)
//      k_bytes_str.Format(_T("%.2f GB"), kb_size / 1024.0 / 1024.0);
//  else
//      k_bytes_str.Format(_T("%.2f TB"), kb_size / 1024.0 / 1024.0 / 1024.0);
//  return k_bytes_str;
//}

CString CCommon::KBytesToString(unsigned __int64 kb_size)
{
    CString k_bytes_str;
    if (kb_size < 1024)
        k_bytes_str.Format(_T("%d KB"), kb_size);
    else if (kb_size < 1024 * 1024)
        k_bytes_str.Format(_T("%.2f MB"), kb_size / 1024.0);
    else if (kb_size < 1024 * 1024 * 1024)
        k_bytes_str.Format(_T("%.2f GB"), kb_size / 1024.0 / 1024.0);
    else
        k_bytes_str.Format(_T("%.2f TB"), kb_size / 1024.0 / 1024.0 / 1024.0);
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
    ofstream file{ file_path, std::ios::app };  //以追加的方式打开日志文件
    file << buff;
    file << str_text << std::endl;
}

void CCommon::WriteLog(const wchar_t* str_text, LPCTSTR file_path)
{
    SYSTEMTIME cur_time;
    GetLocalTime(&cur_time);
    char buff[32];
    sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
        cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
    ofstream file{ file_path, std::ios::app };  //以追加的方式打开日志文件
    file << buff;
    file << UnicodeToStr(str_text).c_str() << std::endl;
}

BOOL CCommon::CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName, LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
{
    if (lpszLnkFileDir == NULL)
        return FALSE;

    HRESULT hr;
    IShellLink* pLink;  //IShellLink对象指针
    IPersistFile* ppf; //IPersisFil对象指针

                         //创建IShellLink对象
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
    if (FAILED(hr))
        return FALSE;

    //从IShellLink对象中获取IPersistFile接口
    hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if (FAILED(hr))
    {
        pLink->Release();
        return FALSE;
    }

    TCHAR file_path[MAX_PATH];
    GetModuleFileName(NULL, file_path, MAX_PATH);

    //目标
    if (lpszFileName == NULL)
        pLink->SetPath(file_path);
    else
        pLink->SetPath(lpszFileName);

    //工作目录
    if (lpszWorkDir != NULL)
    {
        pLink->SetWorkingDirectory(lpszWorkDir);
    }
    else
    {
        //设置工作目录为快捷方式目标所在位置
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

    //快捷键
    if (wHotkey != 0)
        pLink->SetHotkey(wHotkey);

    //备注
    if (lpszDescription != NULL)
        pLink->SetDescription(lpszDescription);

    //显示方式
    pLink->SetShowCmd(iShowCmd);


    //快捷方式的路径 + 名称
    wchar_t szBuffer[MAX_PATH];
    if (lpszLnkFileName != NULL) //指定了快捷方式的名称
        swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, lpszLnkFileName);
    else
    {
        //没有指定名称，就从取指定文件的文件名作为快捷方式名称。
        const wchar_t* pstr;
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
        //注意后缀名要从.exe改为.lnk
        swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, pstr);
        int nLen = wcslen(szBuffer);
        szBuffer[nLen - 3] = L'l';
        szBuffer[nLen - 2] = L'n';
        szBuffer[nLen - 1] = L'k';
    }
    //保存快捷方式到指定目录下
    //WCHAR  wsz[MAX_PATH];  //定义Unicode字符串
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
    //文件句柄
    intptr_t hFile = 0;
    //文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
    _wfinddata_t fileinfo;
    wstring file_name;
    if ((hFile = _wfindfirst(path, &fileinfo)) != -1)
    {
        do
        {
            file_name.assign(fileinfo.name);
            if (file_name != L"." && file_name != L"..")
                //files.push_back(wstring(path) + L"\\" + file_name);  //将文件名保存(忽略"."和"..")
                files.push_back(L"\\" + file_name);  //将文件名保存(忽略"."和"..")
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

void CCommon::GetFiles(const wchar_t* path, std::function<void(const wstring&)> func)
{
    //文件句柄
    intptr_t hFile = 0;
    _wfinddata_t fileinfo;
    wstring file_name;
    if ((hFile = _wfindfirst(path, &fileinfo)) != -1)
    {
        do
        {
            file_name.assign(fileinfo.name);
            if (file_name != L"." && file_name != L"..")
                func(file_name);
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

bool CCommon::FileExist(LPCTSTR file_name)
{
    return (PathFileExists(file_name) != 0);
}

bool CCommon::IsFolder(const wstring& path)
{
    DWORD dwAttrib = GetFileAttributes(path.c_str());
    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool CCommon::MoveAFile(LPCTSTR exist_file, LPCTSTR new_file)
{
    if (!FileExist(exist_file))
        return false;
    //if (FileExist(new_file))      //如果目标文件已经存在，则先删除它
    //  DeleteFile(new_file);
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

ULONGLONG CCommon::GetCurrentTimeSinceEpochMilliseconds()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);  // 获取当前系统时间

    // 将FILETIME转换为ULARGE_INTEGER以便计算
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;

    // 从1601年1月1日到1970年1月1日的100纳秒间隔数
    const ULONGLONG EPOCH_OFFSET = 116444736000000000ULL;

    // 转换为从1970年1月1日开始的毫秒数
    ULONGLONG millisecondsSince1970 = (uli.QuadPart - EPOCH_OFFSET) / 10000;
    return millisecondsSince1970;
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
    GetTempPath(MAX_PATH, buff);        //获取临时文件夹的路径
    result = buff;
    if (result.back() != L'\\' && result.back() != L'/')        //确保路径后面有斜杠
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
    wstring app_data_path{ pszAppDataPath };        //获取到C:/User/用户名/AppData/Roaming路径
    CreateDirectory(app_data_path.c_str(), NULL);       //如果Roaming不存在，则创建它
    app_data_path += L"\\TrafficMonitor\\";
    CreateDirectory(app_data_path.c_str(), NULL);       //如果C:/User/用户名/AppData/Roaming/TrafficMonitor不存在，则创建它

    return app_data_path;
}

void CCommon::DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color)
{
    pDC->SetTextColor(color);
    //m_pDC->SetBkMode(TRANSPARENT);
    //用背景色填充矩形区域
    pDC->FillSolidRect(rect, back_color);
    pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

}

//void CCommon::SetDrawArea(CDC * pDC, CRect rect)
//{
//  CRgn rgn;
//  rgn.CreateRectRgnIndirect(rect);
//  pDC->SelectClipRgn(&rgn);
//}


bool CCommon::IsForegroundFullscreen(HMONITOR hMonitor)
{
    if (hMonitor == NULL)
        hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    bool bFullscreen{ false };      //用于指示前台窗口是否是全屏
    HWND hWnd{};
    RECT rcApp{};

    // 获取显示器信息
    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    RECT monitorRect = monitorInfo.rcMonitor;

    hWnd = GetForegroundWindow();   //获取当前正在与用户交互的前台窗口句柄
    TCHAR buff[256];
    GetClassName(hWnd, buff, 256);      //获取前台窗口的类名
    CString class_name{ buff };
    if (hWnd != GetDesktopWindow() && class_name != _T("WorkerW") && hWnd != GetShellWindow())//如果前台窗口不是桌面窗口，也不是控制台窗口
    {
        GetWindowRect(hWnd, &rcApp);    //获取前台窗口的坐标
        if (rcApp.left <= monitorRect.left && //如果前台窗口的坐标完全覆盖住桌面窗口，就表示前台窗口是全屏的
            rcApp.top <= monitorRect.top &&
            rcApp.right >= monitorRect.right &&
            rcApp.bottom >= monitorRect.bottom)
        {
            bFullscreen = true;
        }
    }//如果前台窗口是桌面窗口，或者是控制台窗口，就直接返回不是全屏
    return bFullscreen;
}

bool CCommon::CopyStringToClipboard(const wstring& str)
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


wstring CCommon::GetJsonValueSimple(const wstring& json_str, const wstring& name)
{
    wstring str_name{ L"\"" };
    str_name += name;
    str_name += L'\"';
    size_t index = json_str.find(str_name);
    if (index == wstring::npos)
        return wstring();
    index = json_str.find(L':', index + 1);
    if (index == wstring::npos)
        return wstring();
    index = json_str.find_first_not_of(L"\" ", index + 1);
    size_t index_end = json_str.find_first_of(L"\",]}\r\n", index);
    wstring result = json_str.substr(index, index_end - index);
    return result;
}

bool CCommon::GetURL(const wstring& url, std::string& result, const wstring& user_agent)
{
    bool succeed{ false };
    CInternetSession* pSession{};
    CHttpFile* pfile{};
    try
    {
        pSession = new CInternetSession(user_agent.c_str());
        pfile = (CHttpFile*)pSession->OpenURL(url.c_str());
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
            result = std::string((const char*)content.GetString());
            succeed = true;
        }
        pfile->Close();
        delete pfile;
        pSession->Close();
    }
    catch (CInternetException* e)
    {
        //写入错误日志
        if (theApp.m_debug_log)
        {
            CString info = CCommon::LoadTextFormat(IDS_GET_URL_ERROR_LOG_INFO, { url, static_cast<size_t>(e->m_dwError) });
            CCommon::WriteLog(info, theApp.m_log_path.c_str());
        }
        if (pfile != nullptr)
        {
            pfile->Close();
            delete pfile;
        }
        if (pSession != nullptr)
            pSession->Close();
        succeed = false;
        e->Delete();        //没有这句会造成内存泄露
        SAFE_DELETE(pSession);
    }
    SAFE_DELETE(pSession);
    return succeed;
}

bool CCommon::GetURL(const wstring& url, wstring& result, bool utf8, const wstring& user_agent)
{
    std::string str_result;
    bool succeed = GetURL(url, str_result, user_agent);
    if (succeed)
    {
        result = CCommon::StrToUnicode(str_result.c_str(), utf8);
    }
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
            ip_address = web_page.substr(index + 6, index1 - index - 6);    //获取IP地址
        if (ip_address.size() > 15 || ip_address.size() < 7)        //IP地址最长15个字符，最短7个字符
            ip_address.clear();

        //获取IP地址归属地
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

void CCommon::GetInternetIp2(wstring& ip_address, wstring& ip_location, bool ipv6)
{
    wstring raw_string;
    wstring user_agent{ L"TrafficMonitor/" };
    user_agent += VERSION;
    if (GetURL((ipv6 ? L"https://v6.yinghualuo.cn/bejson" : L"https://v4.yinghualuo.cn/bejson"), raw_string, true, user_agent))
    {
        //解析获取的json字符串
        ip_address = GetJsonValueSimple(raw_string, L"ip");
        ip_location = GetJsonValueSimple(raw_string, L"location");

    }
    else
    {
        ip_address.clear();
        ip_location.clear();
    }
}


void CCommon::SetRect(CRect& rect, int x, int y, int width, int height)
{
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
}

CString CCommon::LoadText(const wchar_t* id, LPCTSTR back_str)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
    if (back_str != nullptr)
        str += back_str;
    return str;
}

CString CCommon::LoadText(LPCTSTR front_str, const wchar_t* id, LPCTSTR back_str)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
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

CString CCommon::LoadTextFormat(const wchar_t* id, const std::initializer_list<CVariant>& paras)
{
    CString str = theApp.m_str_table.LoadText(id).c_str();
    return StringFormat(str.GetString(), paras);
}

CString CCommon::IntToString(__int64 n, bool thousand_separation, bool is_unsigned)
{
    wstring str = std::to_wstring(is_unsigned ? static_cast<unsigned __int64>(n) : n);
    int count{};
    if (thousand_separation)
    {
        int length{ static_cast<int>(str.size()) };
        for (int i{ length - 1 }; i > 0; i--)
        {
            count++;
            if (count % 3 == 0)
                str.insert(i, L",");
        }
    }
    return str.c_str();
}

void CCommon::NormalizeFont(LOGFONT& font)
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

void CCommon::WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size)
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
    //确保目标字符串末尾有一个\0
    int copy_cnt = i;
    if (copy_cnt < dest_size)
        str_dest[copy_cnt] = L'\0';
    else
        str_dest[dest_size - 1] = L'\0';
}

bool CCommon::StringReplace(wstring& str, const wstring& str_old, const wstring& str_new)
{
    if (str.empty())
        return false;
    bool replaced{ false };
    size_t pos = 0;
    while ((pos = str.find(str_old, pos)) != std::wstring::npos)
    {
        str.replace(pos, str_old.length(), str_new);
        replaced = true;
        pos += str_new.length();    // 前进到替换后的字符串末尾
    }
    return replaced;
}

template<class T>
static double _StringSimilarDegree_LD(const T& srcString, const T& matchString)
{
    int n = static_cast<int>(srcString.size());
    int m = static_cast<int>(matchString.size());
    //int[, ] d = new int[n + 1, m + 1]; // matrix
    vector<vector<int>> d(n + 1, vector<int>(m + 1));
    int cost; // cost
    // Step 1（如果其中一个字符串长度为0，则相似度为1）？
    //if (n == 0) return (double)m / max(srcString.size(), matchString.size());
    //if (m == 0) return (double)n / max(srcString.size(), matchString.size());
    if (n == 0 || m == 0) return 0.0;   //如果其中一个字符串长度为0，则相似度为0
    // Step 2
    for (int i = 0; i <= n; i++)
    {
        d[i][0] = i;
    }
    for (int j = 0; j <= m; j++)
    {
        d[0][j] = j;
    }
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


double CCommon::StringSimilarDegree_LD(const std::string& srcString, const std::string& matchString)
{
    return _StringSimilarDegree_LD(srcString, matchString);
}

double CCommon::StringSimilarDegree_LD(const std::wstring& srcString, const std::wstring& matchString)
{
    return _StringSimilarDegree_LD(srcString, matchString);
}

void CCommon::SetThreadLanguage(WORD language)
{
    if (language != 0)
        SetThreadUILanguage(language);
}

void CCommon::SetColorMode(ColorMode mode)
{
    switch (mode)
    {
    case ColorMode::Default:
        //Win8/8.1下背景色和透明色不使用纯黑色
        if (theApp.m_win_version.IsWindows8Or8point1())
        {
            CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = RGB(0, 0, 1);
            CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = RGB(0, 0, 1);
        }
        else
        {
            CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0;
            CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0;
        }
        CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x005A5A5A;
        CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00ffffffU;
        CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 0;
        break;
    case ColorMode::Light:
        CTrafficMonitorApp::self->m_taskbar_data.dft_back_color = 0x00D3D2D2;
        CTrafficMonitorApp::self->m_taskbar_data.dft_transparent_color = 0x00D3D2D2;
        CTrafficMonitorApp::self->m_taskbar_data.dft_status_bar_color = 0x00A5A5A5;
        CTrafficMonitorApp::self->m_taskbar_data.dft_text_colors = 0x00000000U;
        CTrafficMonitorApp::self->m_cfg_data.m_dft_notify_icon = 4;
        break;
    default:
        break;
    }
}

void CCommon::TransparentColorConvert(COLORREF& transparent_color)
{
    if (transparent_color == 0)
        return;
    BYTE r = GetRValue(transparent_color);
    BYTE g = GetGValue(transparent_color);
    BYTE b = GetBValue(transparent_color);
    if (r == b)
    {
        if (b >= 255)
            b--;
        else
            b++;
        transparent_color = RGB(r, g, b);
    }
}

void CCommon::SetDialogFont(CWnd* pDlg, CFont* pFont)
{
    if (pDlg->GetSafeHwnd() != NULL)
    {
        CWnd* pWndChild;
        pWndChild = pDlg->GetWindow(GW_CHILD);
        while (pWndChild)
        {
            pWndChild->SetFont(pFont);
            pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
        }
    }
}

CString CCommon::GetTextResource(UINT id, int code_type)
{
    CString res_str;
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(id), _T("TEXT"));
    if (hRes != NULL)
    {
        DWORD resSize = SizeofResource(NULL, hRes);  // 获取资源的大小
        HGLOBAL hglobal = LoadResource(NULL, hRes);
        if (hglobal != NULL)
        {
            LPVOID pResourceData = LockResource(hglobal);  // 获取资源数据的指针
            if (code_type == 2)
            {
                // 资源是宽字符字符串
                res_str = CString((const wchar_t*)pResourceData, resSize / sizeof(wchar_t));
            }
            else
            {
                // 资源是窄字符字符串
                std::string strData((const char*)pResourceData, resSize);
                res_str = CCommon::StrToUnicode(strData.c_str(), (code_type != 0)).c_str();
            }
        }
    }
    return res_str;
}

CString CCommon::GetLastCompileTime()
{
    CString str_compile_time = GetTextResource(IDR_COMPILE_TIME, 0);
    str_compile_time.Replace(_T("\r\n"), _T(""));
    str_compile_time.Delete(str_compile_time.GetLength() - 1, 1);
    return str_compile_time;
}

HICON CCommon::LoadIconResource(UINT id, int size)
{
    return (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, size, size, 0);
}

int CCommon::GetMenuItemPosition(CMenu* pMenu, UINT id)
{
    int pos = -1;
    int item_count = pMenu->GetMenuItemCount();
    for (int i = 0; i < item_count; i++)
    {
        if (pMenu->GetMenuItemID(i) == id)
        {
            pos = i;
            break;
        }
    }
    return pos;
}

// 递归遍历菜单项并处理多语言翻译
static void TranslateMenuItems(CMenu& menu)
{
    // 遍历菜单项
    for (int i = 0; i < menu.GetMenuItemCount(); ++i)
    {
        UINT menuItemID = menu.GetMenuItemID(i);
        CString menuText;
        menu.GetMenuString(i, menuText, MF_BYPOSITION);

        // 检查菜单项文本是否以TXT_开头
        if (menuText.Left(4) == _T("TXT_"))
        {
            // 获取翻译后的文本
            std::wstring key(menuText);
            const std::wstring& translatedText = theApp.m_str_table.LoadMenuText(key);

            // 更新菜单项文本
            menu.ModifyMenu(i, MF_BYPOSITION | MF_STRING, menuItemID, translatedText.c_str());
        }

        if (menuItemID == -1)
        {
            // 这是一个弹出菜单（子菜单），递归处理
            CMenu* pSubMenu = menu.GetSubMenu(i);
            if (pSubMenu)
            {
                TranslateMenuItems(*pSubMenu); // 递归调用
            }
        }
    }
}

void CCommon::LoadMenuResource(CMenu& menu, UINT res_id)
{
    // 加载菜单资源
    menu.LoadMenu(res_id);

    // 处理菜单项翻译
    TranslateMenuItems(menu);
}

bool CCommon::IsColorSimilar(COLORREF color1, COLORREF color2)
{
    const int DIFF{ 24 };
    return (std::abs(GetRValue(color1) - GetRValue(color2)) < DIFF
        && std::abs(GetGValue(color1) - GetGValue(color2)) < DIFF
        && std::abs(GetBValue(color1) - GetBValue(color2)) < DIFF);
}

int CCommon::CountOneBits(unsigned int value)
{
    int count = 0;
    while (value != 0)
    {
        if (value % 2 == 1)
        {
            count++;
        }
        value = value >> 1;
    }
    return count;
}

void CCommon::SetNumberBit(unsigned int& num, int bit, bool value)
{
    if (value)
    {
        num |= (1 << bit);
    }
    else
    {
        num &= ~(1 << bit);
    }
}

bool CCommon::GetNumberBit(unsigned int num, int bit)
{
    return (num & (1 << bit)) != 0;
}

COLORREF CCommon::GetWindowsThemeColor()
{
    DWORD crColorization;
    BOOL fOpaqueBlend;
    COLORREF theme_color{};
    HRESULT result = DwmGetColorizationColor(&crColorization, &fOpaqueBlend);
    if (result == S_OK)
    {
        BYTE r, g, b;
        r = (crColorization >> 16) % 256;
        g = (crColorization >> 8) % 256;
        b = crColorization % 256;
        theme_color = RGB(r, g, b);
    }
    return theme_color;
}

CString CCommon::GetErrorMessage(DWORD error_code)
{
    CString error_msg;
    if (error_code != 0)
    {
        LPVOID lpMsgBuf = nullptr;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL);

        if (lpMsgBuf != nullptr)
            error_msg = (LPCTSTR)lpMsgBuf;

        LocalFree(lpMsgBuf);
    }
    return error_msg;
}
