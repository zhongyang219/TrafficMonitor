#include "stdafx.h"
#include "TaskbarDefaultStyle.h"
#include "IniHelper.h"
#include "TrafficMonitor.h"
#include "WindowsSettingHelper.h"

CTaskbarDefaultStyle::CTaskbarDefaultStyle()
{
}

CTaskbarDefaultStyle::~CTaskbarDefaultStyle()
{
	SaveConfig();
}

void CTaskbarDefaultStyle::LoadConfig()
{
	CIniHelper ini{ theApp.m_config_path };
	for (int i = 0; i < TASKBAR_DEFAULT_STYLE_NUM; i++)
	{
        COLORREF default_text_color = (TASKBAR_DEFAULT_LIGHT_STYLE(i) ? RGB(0, 0, 0) : RGB(255, 255, 255));
        COLORREF default_back_color = (TASKBAR_DEFAULT_LIGHT_STYLE(i) ? RGB(210, 210, 211) : 0);
        COLORREF default_transparent_color = (TASKBAR_DEFAULT_LIGHT_STYLE(i) ? RGB(210, 210, 211) : 0);
        COLORREF default_status_bar_color = (TASKBAR_DEFAULT_LIGHT_STYLE(i) ? RGB(165, 165, 165) : 0x005A5A5A);
        wchar_t buff[64];
        swprintf_s(buff, L"default%d_", i + 1);
        wstring key_name = buff;

        bool specify_each_item_color_default = false;
        //为预设2和预设3设置默认的文本颜色
        if (i == 1)
        {
            const wchar_t* str_default_text_color = L"4574711,16777215,5107653,16777215,16632499,16777215,16494036,16777215,14601983,16777215,16760992,10221779,13425662,16777215,4574711,16777215,";
            ini.LoadTaskbarWndColors(L"taskbar_default_style", (key_name + L"text_color").c_str(), m_default_style[i].text_colors, str_default_text_color);
            specify_each_item_color_default = true;
        }
        else if (i == 2)
        {
            const wchar_t* str_default_text_color = L"31957,0,2533120,0,13857281,0,14354551,0,1145775,0,4328380,0,167504,0,31957,0,";
            ini.LoadTaskbarWndColors(L"taskbar_default_style", (key_name + L"text_color").c_str(), m_default_style[i].text_colors, str_default_text_color);
            specify_each_item_color_default = true;
        }
        else
        {
            ini.LoadTaskbarWndColors(L"taskbar_default_style", (key_name + L"text_color").c_str(), m_default_style[i].text_colors, default_text_color);
        }
        m_default_style[i].back_color = ini.GetInt(L"taskbar_default_style", (key_name + L"back_color").c_str(), default_back_color);
        m_default_style[i].transparent_color = ini.GetInt(L"taskbar_default_style", (key_name + L"transparent_color").c_str(), default_transparent_color);
        m_default_style[i].status_bar_color = ini.GetInt(L"taskbar_default_style", (key_name + L"status_bar_color").c_str(), default_status_bar_color);
        m_default_style[i].specify_each_item_color = ini.GetBool(L"taskbar_default_style", (key_name + L"specify_each_item_color").c_str(), specify_each_item_color_default);
	}
}

void CTaskbarDefaultStyle::SaveConfig() const
{

	CIniHelper ini{ theApp.m_config_path };
	for (int i = 0; i < TASKBAR_DEFAULT_STYLE_NUM; i++)
	{
		wchar_t buff[64];
		swprintf_s(buff, L"default%d_", i + 1);
		wstring key_name = buff;
        if (IsTaskBarStyleDataValid(m_default_style[i]))           //保存前检查当前颜色预设是否有效
        {
            ini.SaveTaskbarWndColors(L"taskbar_default_style", (key_name + L"text_color").c_str(), m_default_style[i].text_colors);
            ini.WriteInt(L"taskbar_default_style", (key_name + L"back_color").c_str(), m_default_style[i].back_color);
            ini.WriteInt(L"taskbar_default_style", (key_name + L"transparent_color").c_str(), m_default_style[i].transparent_color);
            ini.WriteInt(L"taskbar_default_style", (key_name + L"status_bar_color").c_str(), m_default_style[i].status_bar_color);
            ini.WriteBool(L"taskbar_default_style", (key_name + L"specify_each_item_color").c_str(), m_default_style[i].specify_each_item_color);
        }
        else
        {
            //写入日志
            CString log_str;
            log_str.Format(_T("在保存预设%d时检测到背景色和文字颜色都为黑色，该预设未被保存。"), i);
            CCommon::WriteLog(log_str, theApp.m_log_path.c_str());
            return;
        }
	}
	ini.Save();
}

void CTaskbarDefaultStyle::ApplyDefaultStyle(int index, TaskBarSettingData & data) const
{
    /*if (index == TASKBAR_DEFAULT_LIGHT_STYLE_INDEX)
    {
        ApplyDefaultLightStyle(data);
    }
    else */if (index >= 0 && index < TASKBAR_DEFAULT_STYLE_NUM)
	{
        if (!IsTaskBarStyleDataValid(m_default_style[index]))
            return;

        data.text_colors = m_default_style[index].text_colors;
		data.back_color = m_default_style[index].back_color;
		data.transparent_color = m_default_style[index].transparent_color;
		data.status_bar_color = m_default_style[index].status_bar_color;
		data.specify_each_item_color = m_default_style[index].specify_each_item_color;
		if (data.transparent_color == data.back_color)
		{
			CCommon::TransparentColorConvert(data.back_color);
			CCommon::TransparentColorConvert(data.transparent_color);
		}
	}
}

//void CTaskbarDefaultStyle::ApplyDefaultLightStyle(TaskBarSettingData& data)
//{
//	for (auto& item : data.text_colors)
//    {
//        item.second.label = RGB(0, 0, 0);
//        item.second.value = RGB(0, 0, 0);
//    }
//	data.back_color = RGB(210, 210, 211);
//	data.transparent_color = RGB(210, 210, 211);
//	data.status_bar_color = RGB(165, 165, 165);
//}

void CTaskbarDefaultStyle::ModifyDefaultStyle(int index, TaskBarSettingData & data)
{
	if (index < 0 || index >= TASKBAR_DEFAULT_STYLE_NUM)
		return;

    m_default_style[index].text_colors = data.text_colors;
	m_default_style[index].back_color = data.back_color;
	m_default_style[index].transparent_color = data.transparent_color;
	m_default_style[index].status_bar_color = data.status_bar_color;
	m_default_style[index].specify_each_item_color = data.specify_each_item_color;
}

bool CTaskbarDefaultStyle::IsTaskBarStyleDataValid(const TaskBarStyleData& data)
{
    for (const auto& item : data.text_colors)
    {
        if (item.second.label != data.back_color || item.second.value != data.back_color)
            return true;
    }
    return false;     //如果文本颜色全部等于背景颜色，则该颜色预设无效
}
