#pragma once
#include "PluginInterface.h"
class CCustomDrawItem2 : public IPluginItem
{
public:
    // 通过 IPluginItem 继承
    virtual const wchar_t* GetItemName() const override;
    virtual const wchar_t* GetItemId() const override;
    virtual const wchar_t* GetItemLableText() const override;
    virtual const wchar_t* GetItemValueText() const override;
    virtual const wchar_t* GetItemValueSampleText() const override;
    virtual bool IsCustomDraw() const override;
    virtual int GetItemWidth() const override;
    virtual bool DrawItemEx(IPluginDrawer* pDrawer, int x, int y, int w, int h, bool dark_mode) override;
    virtual int IsDoubleLineExclusive() const { return 1; }     //在任务栏中独占双行

private:
    HICON m_icon;
};

