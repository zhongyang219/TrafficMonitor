#include "stdafx.h"
#include "StrTable.h"
#include "Common.h"
#include "IniHelper.h"
#include "TrafficMonitor.h"

CStrTable::CStrTable()
{
}

CStrTable::~CStrTable()
{
}

static void LanguageInfoFromIni(CStrTable::LanguageInfo& language_info, const CIniHelper& ini)
{
    language_info.display_name = ini.GetString(L"general", L"DISPLAY_NAME", L"");
    language_info.bcp_47 = ini.GetString(L"general", L"BCP_47", L"");
    language_info.default_font_name = ini.GetString(L"general", L"DEFAULT_FONT", L"Microsoft Sans Serif");
    language_info.translator = ini.GetString(L"general", L"TRANSLATOR", L"<Unknown>");
    language_info.translator_url = ini.GetString(L"general", L"TRANSLATOR_URL", L"");
}

// 回调函数，用于枚举资源语言
static BOOL CALLBACK EnumResLangProc(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wIDLanguage, LONG_PTR lParam)
{
    std::vector<CStrTable::LanguageInfo>* pLanguages = reinterpret_cast<std::vector<CStrTable::LanguageInfo>*>(lParam);

    // 获取资源句柄
    HRSRC hRes = FindResourceEx(hModule, lpType, lpName, wIDLanguage);
    if (hRes != NULL)
    {
        // 加载资源
        HGLOBAL hGlobal = LoadResource(hModule, hRes);
        if (hGlobal != NULL)
        {
            // 锁定资源并获取数据指针
            LPVOID pResourceData = LockResource(hGlobal);
            if (pResourceData != NULL)
            {
                // 获取资源大小
                DWORD resSize = SizeofResource(hModule, hRes);

                // 将资源内容转换为字符串
                std::string strData((const char*)pResourceData, resSize);
                std::wstring resData = CCommon::StrToUnicode(strData.c_str(), true).c_str();

                CIniHelper ini;
                ini.FromDirectString(resData);
                CStrTable::LanguageInfo lanugage_info;
                LanguageInfoFromIni(lanugage_info, ini);
                lanugage_info.language_id = wIDLanguage;
                pLanguages->push_back(lanugage_info);
            }
        }
    }

    return TRUE; // 继续枚举
}


void CStrTable::Init()
{
    // 获取 IDR_LANGUAGE 资源的所有语言版本
    EnumResourceLanguages(NULL, _T("TEXT"), MAKEINTRESOURCE(IDR_LANGUAGE), EnumResLangProc, reinterpret_cast<LONG_PTR>(&m_language_list));
    //按bcp47排序
    std::sort(m_language_list.begin(), m_language_list.end(), [](const LanguageInfo& a, const LanguageInfo& b) {
        return a.bcp_47 < b.bcp_47;
    });

    //先读取默认字符串资源，如果当前语言中没有对应字符串，则会使用默认的字符串资源
    CIniHelper ini_default(IDR_LANGUAGE_DEFAULT);
    ini_default.GetAllKeyValues(L"text", m_text_string_table);
    ini_default.GetAllKeyValues(L"menu", m_menu_string_table);

    //读取字符串资源
    CIniHelper ini(IDR_LANGUAGE);
    ini.GetAllKeyValues(L"text", m_text_string_table);
    ini.GetAllKeyValues(L"menu", m_menu_string_table);
    LanguageInfoFromIni(m_language_info, ini);

    //从外部language文件夹获取语言文件
    vector<wstring> files;
    std::wstring language_dir;
#ifdef _DEBUG
    language_dir = L".\\language";
#else
    language_dir = theApp.m_module_dir + L"language";
#endif
    CCommon::GetFiles((language_dir + L"\\*.ini").c_str(), files);
    for (const wstring& file_name : files)
    {
        std::wstring file_path{ language_dir + file_name };
        CIniHelper ini_file(file_path);
        LanguageInfo language_info;
        LanguageInfoFromIni(language_info, ini_file);
        language_info.language_id = LocaleNameToLCID(language_info.bcp_47.c_str(), 0);  //根据语言bcp-47代码获取语言id
        WORD cur_language_id = GetThreadUILanguage();   //当前语言id
        //从外部语言文件读取到当前语言，先从外部语言文件加载
        if (language_info.language_id == cur_language_id)
        {
            m_language_info = language_info;
            ini_file.GetAllKeyValues(L"text", m_text_string_table);
            ini_file.GetAllKeyValues(L"menu", m_menu_string_table);
        }

        //如果语言不在m_language_list，添加到该列表
        auto iter = std::find(m_language_list.begin(), m_language_list.end(), language_info);
        if (iter == m_language_list.end())
            m_language_list.push_back(language_info);
    }
}

const wstring& CStrTable::LoadText(const wstring& key) const
{
    auto iter = m_text_string_table.find(key);
    if (iter != m_text_string_table.end())
        return iter->second;
    ASSERT(false);
    static std::wstring str_empty;
    return str_empty;
}

wstring CStrTable::LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const
{
    auto iter = m_text_string_table.find(key);
    if (iter == m_text_string_table.end())
    {
        ASSERT(false);
        return std::wstring();
    }
    wstring str{ iter->second };    // 复制以避免原始字符串修改
    int index{ 1 };
    for (const auto& para : paras)
    {
        wstring format_str{ L"<%" + std::to_wstring(index) + L"%>" };
        CCommon::StringReplace(str, format_str, para.ToString().GetString());
        ++index;
    }
    return str;
}

const wstring& CStrTable::LoadMenuText(const wstring& key) const
{
    auto iter = m_menu_string_table.find(key);
    if (iter != m_menu_string_table.end())
        return iter->second;
    ASSERT(false);
    static std::wstring str_empty;
    return str_empty;
}
