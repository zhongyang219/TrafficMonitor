#pragma once
#include <string>

struct SettingData
{
    bool show_second{};
    bool show_label_text{};
};

class CDataManager
{
private:
    CDataManager();
    ~CDataManager();

public:
    static CDataManager& Instance();

    void LoadConfig();
    void SaveConfig() const;

public:
    std::wstring m_cur_time;
    std::wstring m_cur_date;
    SettingData m_setting_data;

private:
    std::wstring m_module_path;
};
