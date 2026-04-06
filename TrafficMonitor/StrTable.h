#pragma once
#include "CVariant.h"
#include <map>
#include "CommonData.h"
class CIniHelper;

class CStrTable
{
public:
    CStrTable();
    ~CStrTable();

    void Init();

    // 载入翻译字符串
    const wstring& LoadText(const wstring& key) const;
    // 载入翻译字符串，并将字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    wstring LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const;

    const wstring& LoadMenuText(const wstring& key) const;

    const wstring& LoadText(const wstring& key, const wstring& section);

    // 获取默认字体名称
    const LanguageInfo& GetLanguageInfo() const { return m_language_info; }

    // 获取所有支持语言
    const std::vector<LanguageInfo>& GetLanguageList() const { return m_language_list; }

    // 当前语言是否为简体中文（选择默认更新源以及更新信息语言）
    bool IsSimplifiedChinese() const { return  m_language_info.bcp_47 == L"zh-CN"; };

protected:
    void ReadStringtableFronIni(const CIniHelper& ini);
    const std::map<std::wstring, std::wstring>& GetTextStringTable() const;
    const std::map<std::wstring, std::wstring>& GetMenuStringTable() const;

    static const wstring& LoadText(const wstring& key, const std::map<std::wstring, std::wstring>& string_table);
    static wstring LoadTextFormat(const wstring& key, const std::map<std::wstring, std::wstring>& string_table, const std::initializer_list<CVariant>& paras);


private:
    std::map<std::wstring, std::map<std::wstring, std::wstring>> m_string_table;    //保存ini文件每个section的键值对
    LanguageInfo m_language_info;                   //当前语言
    std::vector<LanguageInfo> m_language_list;      //所有支持的语言
};

