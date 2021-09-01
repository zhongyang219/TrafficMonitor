#pragma once
#include "PluginInterface.h"
#include "PluginSystemDate.h"
#include "PluginSystemTime.h"
#include "CustomDrawItem.h"

class CPluginDemo : public ITMPlugin
{
public:
    CPluginDemo();

    // 通过 ITMPlugin 继承
    virtual IPluginItem* GetItem(int index) override;
    virtual void DataRequired() override;
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override;
    virtual OptionReturn ShowOptionsDialog(HWND hParent) override;

private:
    CPluginSystemDate m_system_date;
    CPluginSystemTime m_system_time;
    CCustomDrawItem m_custom_draw_item;
};

#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) ITMPlugin* TMPluginCreateInstance();

#ifdef __cplusplus
}
#endif
