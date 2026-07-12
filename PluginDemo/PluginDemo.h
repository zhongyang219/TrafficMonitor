#pragma once
#include "PluginInterface.h"
#include "PluginSystemDate.h"
#include "PluginSystemTime.h"
#include "CustomDrawItem.h"
#include "CustomDrawItem2.h"
#include <map>

class CPluginDemo : public ITMPlugin
{
private:
    CPluginDemo();

public:
    static CPluginDemo& Instance();
    HICON GetIcon(UINT id);
    int DPI(int x, ITrafficMonitor::DPIType type = ITrafficMonitor::DPI_MAIN_WND);

    // 通过 ITMPlugin 继承
    virtual IPluginItem* GetItem(int index) override;
    virtual void DataRequired() override;
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override;
    virtual OptionReturn ShowOptionsDialog(void* hParent) override;
    virtual void OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data) override;
    virtual void OnInitialize(ITrafficMonitor* pApp) override;

private:
    CPluginSystemDate m_system_date;
    CPluginSystemTime m_system_time;
    CCustomDrawItem m_custom_draw_item;
    CCustomDrawItem2 m_custom_draw_item2;
    ITrafficMonitor* m_app{};

    static CPluginDemo m_instance;
    std::map<UINT, HICON> m_icon_map;
};

#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) ITMPlugin* TMPluginGetInstance();

#ifdef __cplusplus
}
#endif
