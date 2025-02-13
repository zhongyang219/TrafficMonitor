#include "stdafx.h"
#include "StrTable.h"
#include "Common.h"
#include "IniHelper.h"

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

    //先读取默认字符串资源，如果当前语言中没有对应字符串，则会使用默认的字符串资源
    CIniHelper ini_default(IDR_LANGUAGE_DEFAULT);
    ini_default.GetAllKeyValues(L"text", m_text_string_table);
    ini_default.GetAllKeyValues(L"menu", m_menu_string_table);

    //读取字符串资源
    CIniHelper ini(IDR_LANGUAGE);
    ini.GetAllKeyValues(L"text", m_text_string_table);
    ini.GetAllKeyValues(L"menu", m_menu_string_table);

    LanguageInfoFromIni(m_language_info, ini);
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
