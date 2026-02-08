#pragma once
#include "PluginInterface.h"
#include "PluginSystemDate.h"
#include "PluginSystemTime.h"
#include "CustomDrawItem.h"

class CPluginDemo : public ITMPlugin
{
private:
    CPluginDemo();

public:
    static CPluginDemo& Instance();

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
    ITrafficMonitor* m_app{};

    static CPluginDemo m_instance;
};

#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) ITMPlugin* TMPluginGetInstance();

#ifdef __cplusplus
}
#endif
