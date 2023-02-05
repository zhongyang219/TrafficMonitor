#include "stdafx.h"
#include "DCompositionSupport.h"
#include "Common.h"
#include "DllFunctions.h"

void CDCompositionDevice::Recreate(Microsoft::WRL::ComPtr<IDXGIDevice> p_dxgi_device)
{
    ThrowIfFailed<CDCompositionException>(
        CDllFunctions::DCompositionCreateDevice(
            p_dxgi_device.Get(),
            IID_PPV_ARGS(&m_p_device)),
        TRAFFICMONITOR_ERROR_STR("Recreate DComposition device failed."));

    NotifyAllResourceWhenDeviceRecreate(m_resource_tracker, m_p_device);
}

auto CDCompositionDevice::GetStorage()
    -> std::shared_ptr<Storage>
{
    return m_resource_tracker.GetSharedResourceTrackerStorage();
}

bool CDCompositionSupport::CheckSupport()
{
    static auto result =
        FunctionChecker::CheckFunctionExist(_T("dcomp.dll"), "DCompositionCreateDevice");
    return result;
}

auto CDCompositionDevice::Get()
    -> Microsoft::WRL::ComPtr<IDCompositionDevice>
{
    return m_p_device;
}
