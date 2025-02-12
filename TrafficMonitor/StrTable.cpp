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

void CStrTable::Init()
{
    //先读取默认字符串资源，如果当前语言中没有对应字符串，则会使用默认的字符串资源
    CIniHelper ini_default(IDR_LANGUAGE_DEFAULT);
    ini_default.GetAllKeyValues(L"text", m_text_string_table);
    ini_default.GetAllKeyValues(L"menu", m_menu_string_table);

    //读取字符串资源
    CIniHelper ini(IDR_LANGUAGE);
    ini.GetAllKeyValues(L"text", m_text_string_table);
    ini.GetAllKeyValues(L"menu", m_menu_string_table);

    m_language_info.display_name = ini.GetString(L"general", L"DISPLAY_NAME", L"");
    m_language_info.bcp_47 = ini.GetString(L"general", L"BCP_47", L"");
    m_language_info.default_font_name = ini.GetString(L"general", L"DEFAULT_FONT", L"Microsoft Sans Serif");
    ini.GetStringList(L"general", L"TRANSLATOR", m_language_info.translator, vector<wstring>{ L"<Unknown>" });
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

const wstring& CStrTable::GetDefaultFontName() const
{
    return m_language_info.default_font_name;
}

const wstring& CStrTable::GetLanguageTag() const
{
    return m_language_info.bcp_47;
}
