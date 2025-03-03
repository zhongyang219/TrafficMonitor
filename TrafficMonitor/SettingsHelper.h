#pragma once
#include "IniHelper.h"
#include "CommonData.h"

class CSettingsHelper : public CIniHelper
{
public:
    CSettingsHelper();
    CSettingsHelper(const std::wstring& file_path);

    void SaveFontData(const wchar_t* AppName, const FontInfo& font);
    void LoadFontData(const wchar_t* AppName, FontInfo& font, const FontInfo& default_font) const;

    /**
     * @brief  读取主窗口颜色设置
     * @param AppName[in] ini文件的段
     * @param KeyName[in] ini文件的key
     * @param text_colors[out] 保存读取到的颜色
     * @param default_color[in] 默认颜色
     */
    void LoadMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::set<CommonDisplayItem>& all_items, std::map<CommonDisplayItem, COLORREF>& text_colors, COLORREF default_color);
    
    /**
     * @brief  保存主窗口颜色设置
     * @param AppName ini文件的段
     * @param KeyName ini文件的key
     * @param text_colors 要保存的颜色
     */
    void SaveMainWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, COLORREF>& text_colors);

    void LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, const wchar_t* default_str);
    void LoadTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, COLORREF default_color);
    void SaveTaskbarWndColors(const wchar_t* AppName, const wchar_t* KeyName, const std::map<CommonDisplayItem, TaskbarItemColor>& text_colors);

    void LoadDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window) const;
    void SaveDisplayStr(const wchar_t* AppName, const DispStrings& disp_str);

    void LoadPluginDisplayStr(const wchar_t* AppName, DispStrings& disp_str, bool is_main_window);
    void SavePluginDisplayStr(const wchar_t* AppName, const DispStrings& disp_str);

};

