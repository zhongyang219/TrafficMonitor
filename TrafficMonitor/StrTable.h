#pragma once
#include "CVariant.h"
#include <map>
class CStrTable
{
public:
    CStrTable();
    ~CStrTable();

    struct LanguageInfo
    {
        wstring display_name;           // 在语言设置下拉菜单显示的字符串
        wstring bcp_47;                 // BCP-47代码
        wstring default_font_name;      // 默认字体
        wstring translator;
        wstring translator_url;
        WORD language_id{};

        bool operator==(const LanguageInfo& another) const
        {
            return bcp_47 == another.bcp_47;
        }
    };

    void Init();

    // 载入翻译字符串
    const wstring& LoadText(const wstring& key) const;
    // 载入翻译字符串，并将字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    wstring LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const;

    const wstring& LoadMenuText(const wstring& key) const;

    // 获取默认字体名称
    const LanguageInfo& GetLanguageInfo() const { return m_language_info; }

    // 获取所有支持语言
    const std::vector<LanguageInfo>& GetLanguageList() const { return m_language_list; }

    // 当前语言是否为简体中文（选择默认更新源以及更新信息语言）
    bool IsSimplifiedChinese() const { return  m_language_info.bcp_47 == L"zh-CN"; };

private:
    std::map<std::wstring, std::wstring> m_text_string_table;
    std::map<std::wstring, std::wstring> m_menu_string_table;
    LanguageInfo m_language_info;                   //当前语言
    std::vector<LanguageInfo> m_language_list;      //所有支持的语言
};

