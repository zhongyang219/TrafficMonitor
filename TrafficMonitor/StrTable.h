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
        vector<wstring> translator;
    };

    void Init();

    // 载入翻译字符串
    const wstring& LoadText(const wstring& key) const;
    // 载入翻译字符串，并将字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    wstring LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const;

private:
    std::map<std::wstring, std::wstring> m_text_string_table;
    std::map<std::wstring, std::wstring> m_menu_string_table;
    LanguageInfo m_language_info;
};

