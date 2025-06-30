#include "stdafx.h"
#include "DisplayItem.h"
#include "Common.h"
#include "TrafficMonitor.h"

CommonDisplayItem::CommonDisplayItem(DisplayItem item)
{
    is_plugin = false;
    item_type = item;
    plugin_item = nullptr;
}

CommonDisplayItem::CommonDisplayItem(IPluginItem* item)
{
    is_plugin = true;
    plugin_item = item;
}

bool CommonDisplayItem::operator<(const CommonDisplayItem& item) const
{
    if (is_plugin && !item.is_plugin)
        return false;
    else if (!is_plugin && item.is_plugin)
        return true;
    else if (!is_plugin)
        return item_type < item.item_type;
    else
        return theApp.m_plugins.GetItemIndex(plugin_item) < theApp.m_plugins.GetItemIndex(item.plugin_item);
}

bool CommonDisplayItem::operator==(const CommonDisplayItem& item) const
{
    if (is_plugin != item.is_plugin)
        return false;
    else if (!is_plugin)
        return item_type == item.item_type;
    else
        return plugin_item == item.plugin_item;
}

bool CommonDisplayItem::IsPlugin() const
{
    return is_plugin;
}

DisplayItem CommonDisplayItem::ItemType() const
{
    return item_type;
}

IPluginItem* CommonDisplayItem::PluginItem() const
{
    return plugin_item;
}

CString CommonDisplayItem::GetItemName() const
{
    if (is_plugin)
    {
        if (plugin_item != nullptr)
            return plugin_item->GetItemName();
        else
            return CString();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return CCommon::LoadText(IDS_UPLOAD);
        case TDI_DOWN: return CCommon::LoadText(IDS_DOWNLOAD);
        case TDI_TOTAL_SPEED: return CCommon::LoadText(IDS_TOTAL_NET_SPEED);
        case TDI_CPU: return CCommon::LoadText(IDS_CPU_USAGE);
        case TDI_MEMORY: return CCommon::LoadText(IDS_MEMORY_USAGE);
        case TDI_GPU_USAGE: return CCommon::LoadText(IDS_GPU_USAGE);
        case TDI_CPU_TEMP: return CCommon::LoadText(IDS_CPU_TEMPERATURE);
        case TDI_GPU_TEMP: return CCommon::LoadText(IDS_GPU_TEMPERATURE);
        case TDI_HDD_TEMP: return CCommon::LoadText(IDS_HDD_TEMPERATURE);
        case TDI_MAIN_BOARD_TEMP: return CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
        case TDI_HDD_USAGE: return CCommon::LoadText(IDS_HDD_USAGE);
        case TDI_CPU_FREQ: return CCommon::LoadText(IDS_CPU_FREQ);
        case TDI_TODAY_TRAFFIC: return CCommon::LoadText(IDS_TRAFFIC_USED);
        default:
            ASSERT(false);
            break;
        }
    }
    return CString();
}

std::wstring CommonDisplayItem::DefaultString(bool is_main_window) const
{
    std::wstring default_text;
    if (is_plugin)
    {
        default_text = plugin_item->GetItemLableText();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_UPLOAD_DISP, _T(": "));
            else
                default_text = _T("↑: ");
            break;
        case TDI_DOWN:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_DOWNLOAD_DISP, _T(": "));
            else
                default_text = _T("↓: ");
            break;
        case TDI_TOTAL_SPEED:
            default_text = _T("↑↓: ");
            break;
        case TDI_TODAY_TRAFFIC:
            default_text = CCommon::LoadText(IDS_TRAFFIC_USED, _T(": "));
            break;
        case TDI_CPU:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_TEMP:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_FREQ:
            default_text = CCommon::LoadText(IDS_CPU_FREQ, _T(": "));
            break;
        case TDI_MEMORY:
            default_text = CCommon::LoadText(IDS_MEMORY_DISP, _T(": "));
            break;
        case TDI_GPU_USAGE:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_GPU_TEMP:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_HDD_TEMP:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        case TDI_MAIN_BOARD_TEMP:
            default_text = CCommon::LoadText(IDS_MAINBOARD_DISP, _T(": "));
            break;
        case TDI_HDD_USAGE:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        default:
            ASSERT(false);
            break;
        }
    }

    return default_text;
}

const wchar_t* CommonDisplayItem::GetItemIniKeyName() const
{
    if (is_plugin)
    {
        return plugin_item->GetItemId();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return L"up_string";
        case TDI_DOWN: return L"down_string";
        case TDI_CPU: return L"cpu_string";
        case TDI_MEMORY: return L"memory_string";
        case TDI_GPU_USAGE: return L"gpu_string";
        case TDI_CPU_TEMP: return L"cpu_temp_string";
        case TDI_GPU_TEMP: return L"gpu_temp_string";
        case TDI_HDD_TEMP: return L"hdd_temp_string";
        case TDI_MAIN_BOARD_TEMP: return L"main_board_temp_string";
        case TDI_HDD_USAGE: return L"hdd_string";
        case TDI_TOTAL_SPEED: return L"total_speed_string";
        case TDI_CPU_FREQ: return L"cpu_freq_string";
        case TDI_TODAY_TRAFFIC: return L"today_traffic_string";
        }
        ASSERT(FALSE);
        return L"";
    }
}

CString CommonDisplayItem::GetItemValueText(bool is_main_window) const
{
    if (is_plugin)
    {
        return plugin_item->GetItemValueText();
    }
    else
    {
        const PublicSettingData* cfg_data{};
        if (is_main_window)
            cfg_data = &theApp.m_main_wnd_data;
        else
            cfg_data = &theApp.m_taskbar_data;
        CString str_value;
        switch (item_type)
        {
        //上传、下载、总网速
        case TDI_UP:
        case TDI_DOWN:
        case TDI_TOTAL_SPEED:
        {
            CString str_in_speed = CCommon::DataSizeToString(theApp.m_in_speed, *cfg_data);
            CString str_out_speed = CCommon::DataSizeToString(theApp.m_out_speed, *cfg_data);
            CString str_total_speed = CCommon::DataSizeToString(theApp.m_in_speed + theApp.m_out_speed, *cfg_data);
            if (!cfg_data->hide_unit || cfg_data->speed_unit == SpeedUnit::AUTO)
            {
                str_in_speed += _T("/s");
                str_out_speed += _T("/s");
                str_total_speed += _T("/s");
            }
            //交换上传和下载位置
            if (is_main_window && theApp.m_main_wnd_data.swap_up_down)
                std::swap(str_in_speed, str_out_speed);
            if (item_type == TDI_UP)
                str_value = str_out_speed;
            else if (item_type == TDI_DOWN)
                str_value = str_in_speed;
            else
                str_value = str_total_speed;
        }
            break;
        //CPU利用率
        case TDI_CPU:
            str_value = CCommon::UsageToString(theApp.m_cpu_usage, *cfg_data);
            break;
        //内存利用率
        case TDI_MEMORY:
            if (cfg_data->memory_display == MemoryDisplay::MEMORY_USED)
                str_value = CCommon::DataSizeToString(static_cast<unsigned long long>(theApp.m_used_memory) * 1024, cfg_data->separate_value_unit_with_space);
            else if (cfg_data->memory_display == MemoryDisplay::MEMORY_AVAILABLE)
                str_value = CCommon::DataSizeToString((static_cast<unsigned long long>(theApp.m_total_memory) - static_cast<unsigned long long>(theApp.m_used_memory)) * 1024, cfg_data->separate_value_unit_with_space);
            else
                str_value = CCommon::UsageToString(theApp.m_memory_usage, *cfg_data);
            break;
        //显卡利用率
        case TDI_GPU_USAGE:
            str_value = CCommon::UsageToString(theApp.m_gpu_usage, *cfg_data);
            break;
        //硬盘利用率
        case TDI_HDD_USAGE:
            str_value = CCommon::UsageToString(theApp.m_hdd_usage, *cfg_data);
            break;
        //CPU温度
        case TDI_CPU_TEMP:
            str_value = CCommon::TemperatureToString(theApp.m_cpu_temperature, *cfg_data);
            break;
        //显卡温度
        case TDI_GPU_TEMP:
            str_value = CCommon::TemperatureToString(theApp.m_gpu_temperature, *cfg_data);
            break;
        //硬盘温度
        case TDI_HDD_TEMP:
            str_value = CCommon::TemperatureToString(theApp.m_hdd_temperature, *cfg_data);
            break;
        //主板温度
        case TDI_MAIN_BOARD_TEMP:
            str_value = CCommon::TemperatureToString(theApp.m_main_board_temperature, *cfg_data);
            break;
        //CPU频率
        case TDI_CPU_FREQ:
            str_value = CCommon::FreqToString(theApp.m_cpu_freq, *cfg_data);
            break;
        //总流量
        case TDI_TODAY_TRAFFIC:
            str_value = CCommon::KBytesToString((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u);
            break;
        default:
            break;
        }
        return str_value;
    }
}

CString CommonDisplayItem::GetItemValueSampleText(bool is_main_window) const
{
    if (is_plugin)
    {
        return plugin_item->GetItemValueSampleText();
    }
    //主窗口（用于绘制预览图）
    else if (is_main_window)
    {
        CString sample_str;
        switch (item_type)
        {
        case TDI_UP:
            sample_str = _T("88.8 KB/s");
            break;
        case TDI_DOWN:
            sample_str = _T("88.9 KB/s");
            break;
        case TDI_TOTAL_SPEED:
            sample_str = _T("90 KB/s");
            break;
        case TDI_TODAY_TRAFFIC:
            sample_str = _T("100 MB");
            break;
        case TDI_CPU:
            sample_str = _T("50 %");
            break;
        case TDI_MEMORY:
            sample_str = _T("51 %");
            break;
        case TDI_CPU_TEMP: case TDI_GPU_TEMP: case TDI_HDD_TEMP: case TDI_MAIN_BOARD_TEMP:
            sample_str = _T("40 °C");
            break;
        case TDI_CPU_FREQ:
            sample_str = _T("1.0 GHz");
            break;
        default:
            sample_str = _T("99");
            break;
        }
        return sample_str;
    }
    //任务栏窗口（用于计算任务栏窗口宽度）
    else
    {
        CString sample_str;
        switch (item_type)
        {
        //网速
        case TDI_UP:
        case TDI_DOWN:
        case TDI_TOTAL_SPEED:
        {
            wstring digits(theApp.m_taskbar_data.digits_number, L'8');      //根据数据位数生成指定个数的“8”
            bool hide_unit{ theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO };
            if (theApp.m_taskbar_data.speed_short_mode)
            {
                if (hide_unit)
                    sample_str.Format(_T("%s."), digits.c_str());
                else
                    sample_str.Format(_T("%s.M/s"), digits.c_str());
            }
            else
            {
                if (hide_unit)
                    sample_str.Format(_T("%s.8"), digits.c_str());
                else
                    sample_str.Format(_T("%s.8MB/s"), digits.c_str());
            }
            if (!hide_unit && theApp.m_taskbar_data.separate_value_unit_with_space)
                sample_str += _T(' ');
            if (theApp.m_taskbar_data.speed_short_mode && !theApp.m_taskbar_data.unit_byte && !theApp.m_taskbar_data.hide_unit)
                sample_str += _T('b');
        }
            break;
        //占用率百分比
        case TDI_CPU:
        case TDI_MEMORY:
        case TDI_GPU_USAGE:
        case TDI_HDD_USAGE:
        {
            //获取当前数值
            int value = 0;
            if (item_type == TDI_CPU)
                value = theApp.m_cpu_usage;
            else if (item_type == TDI_MEMORY)
                value = theApp.m_memory_usage;
            else if (item_type == TDI_GPU_USAGE)
                value = theApp.m_gpu_usage;
            else if (item_type == TDI_HDD_USAGE)
                value = theApp.m_hdd_usage;

            //当数值达到100时，使用字符串“100”作为宽度，防止显示不全
            if (value >= 100)
                sample_str = _T("100");
            else
                sample_str = _T("99");
            if (!theApp.m_taskbar_data.hide_percent)
            {
                if (theApp.m_taskbar_data.separate_value_unit_with_space)
                    sample_str += _T(" %");
                else
                    sample_str += _T("%");
            }
            //内存显示不为已使用百分比时
            if (item_type == TDI_MEMORY)
            {
                if (theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_USED || theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_AVAILABLE)
                {
                    //宽度为总内存的宽度
                    sample_str = CCommon::DataSizeToString(static_cast<unsigned long long>(theApp.m_total_memory) * 1024, theApp.m_taskbar_data.separate_value_unit_with_space);
                }
            }
        }
            break;
        //温度
        case TDI_CPU_TEMP:
        case TDI_GPU_TEMP:
        case TDI_HDD_TEMP:
        case TDI_MAIN_BOARD_TEMP:
        {
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                sample_str = _T("99 °C");
            else
                sample_str = _T("99°C");
        }
            break;
        //CPU频率
        case TDI_CPU_FREQ:
        {
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                sample_str = _T("1.00 GHz");
            else
                sample_str = _T("1.00GHz");
        }
            break;
        //流量
        case TDI_TODAY_TRAFFIC:
        {
            if (theApp.m_taskbar_data.separate_value_unit_with_space)
                sample_str = _T("999.99 MB");
            else
                sample_str = _T("999.99MB");
        }
            break;
        }
        return sample_str;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
DisplayItemSet::DisplayItemSet(std::initializer_list<DisplayItem> items)
    : data(items)
{
}

void DisplayItemSet::Add(DisplayItem item)
{
    data.insert(item);
}

void DisplayItemSet::Remove(DisplayItem item)
{
    data.erase(item);
}

bool DisplayItemSet::Contains(DisplayItem item) const
{
    auto iter = data.find(item);
    return iter != data.end();
}

int DisplayItemSet::ToInt() const
{
    int value = 0;
    //将set中的枚举值转换成int的每个bit位
    for (const auto& item : data)
    {
        if (item <= 31)     //int左移不超过31位
            value |= (1 << item);
    }
    return value;
}

void DisplayItemSet::FromInt(int value)
{
    data.clear();
    //将int的每个bit位转换成set中的枚举值
    for (const auto& item : AllDisplayItems)
    {
        if (item <= 31 && (value & (1 << item)))
            data.insert(item);
    }
}

bool DisplayItemSet::IsEmpty() const
{
    return data.empty();
}
