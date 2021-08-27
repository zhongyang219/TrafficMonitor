#include "pch.h"
#include "DataManager.h"

CDataManager::CDataManager()
{
    //获取模块的路径
    HMODULE hModule = reinterpret_cast<HMODULE>(&__ImageBase);
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);
    m_module_path = path;
    //从配置文件读取配置
    LoadConfig();
}

CDataManager::~CDataManager()
{
    SaveConfig();
}

CDataManager& CDataManager::Instance()
{
    static CDataManager instance;
    return instance;
}

void CDataManager::LoadConfig()
{
    std::wstring config_path = m_module_path + L".ini";
    m_setting_data.show_second = GetPrivateProfileInt(_T("config"), _T("show_second"), 0, config_path.c_str());
}

void CDataManager::SaveConfig() const
{
    std::wstring config_path = m_module_path + L".ini";
    wchar_t buff[16];
    swprintf_s(buff, L"%d", m_setting_data.show_second);
    WritePrivateProfileString(_T("config"), _T("show_second"), buff, config_path.c_str());
}
