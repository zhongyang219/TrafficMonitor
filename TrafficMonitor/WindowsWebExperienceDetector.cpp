#include "stdafx.h"
#include "WindowsWebExperienceDetector.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Storage.h>
#include "TrafficMonitor.h"

bool WindowsWebExperienceDetector::IsDetected() noexcept
{
    if (theApp.m_win_version.IsWindows11OrLater())
    {
        auto manager = winrt::Windows::Management::Deployment::PackageManager{};
        auto packages = manager.FindPackagesForUser(
            {},
            winrt::hstring{L"MicrosoftWindows.Client.WebExperience"},
            winrt::hstring{
                L"CN=Microsoft Windows, O=Microsoft Corporation, L=Redmond, S=Washington, C=US"});
        auto result = packages.First().HasCurrent();
        return result;
    }
    return false;
}
