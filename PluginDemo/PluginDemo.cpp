#include "pch.h"
#include "PluginDemo.h"
#include "DataManager.h"
#include "OptionsDlg.h"

CPluginDemo::CPluginDemo()
{
}

IPluginItem* CPluginDemo::GetItem(int index)
{
    switch (index)
    {
    case 0:
        return &m_system_date;
    case 1:
        return &m_system_time;
    default:
        break;
    }
    return nullptr;
}

void CPluginDemo::DataRequired()
{
    //获取时间和日期
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);
    wchar_t buff[128];
    swprintf_s(buff, L"%d/%.2d/%.2d", system_time.wYear, system_time.wMonth, system_time.wDay);
    CDataManager::Instance().m_cur_date = buff;

    if (CDataManager::Instance().m_setting_data.show_second)
        swprintf_s(buff, L"%.2d:%.2d:%.2d", system_time.wHour, system_time.wMinute, system_time.wSecond);
    else
        swprintf_s(buff, L"%.2d:%.2d", system_time.wHour, system_time.wMinute);
    CDataManager::Instance().m_cur_time = buff;
}

const wchar_t* CPluginDemo::GetInfo(PluginInfoIndex index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    static CString str;
    switch (index)
    {
    case TMI_NAME:
        str.LoadString(IDS_PLUGIN_NAME);
        return str.GetString();
    case TMI_DESCRIPTION:
        str.LoadString(IDS_PLUGIN_DESCRIPTION);
        return str.GetString();
    case TMI_AUTHOR:
        return L"zhongyang219";
    case TMI_COPYRIGHT:
        return L"Copyright (C) by Zhong Yang 2021";
    default:
        break;
    }
    return L"";
}

ITMPlugin::OptionReturn CPluginDemo::ShowOptionsDialog(HWND hParent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    COptionsDlg dlg(CWnd::FromHandle(hParent));
    dlg.m_data = CDataManager::Instance().m_setting_data;
    if (dlg.DoModal() == IDOK)
    {
        CDataManager::Instance().m_setting_data = dlg.m_data;
        return ITMPlugin::OR_OPTION_CHANGED;
    }
    return ITMPlugin::OR_OPTION_UNCHANGED;
}


ITMPlugin* TMPluginCreateInstance()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return new CPluginDemo();
}
