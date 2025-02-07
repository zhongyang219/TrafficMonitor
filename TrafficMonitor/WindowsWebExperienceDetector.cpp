#include "stdafx.h"
#include "WindowsWebExperienceDetector.h"
#ifndef DISABLE_WINDOWS_WEB_EXPERIENCE_DETECTOR
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Storage.h>
#endif
#include "TrafficMonitor.h"

bool WindowsWebExperienceDetector::IsDetected() noexcept
{
#ifndef DISABLE_WINDOWS_WEB_EXPERIENCE_DETECTOR
    if (theApp.m_win_version.IsWindows11OrLater())
    {
        auto manager = winrt::Windows::Management::Deployment::PackageManager{};
        auto packages = manager.FindPackagesForUser(
            {},
            winrt::hstring{ L"MicrosoftWindows.Client.WebExperience" },
            winrt::hstring{
                L"CN=Microsoft Windows, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" });
        auto result = packages.First().HasCurrent();
        return result;
    }
#endif
    return false;
}
