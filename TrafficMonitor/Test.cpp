#include "stdafx.h"
#include "Test.h"
#include "Common.h"
#include "SkinFile.h"
#include "TrafficMonitor.h"
#include "IniHelper.h"
#include "PluginUpdateHelper.h"
#include "MessageDlg.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void TestHttpQequest()
{
    wstring result;
    bool rtn = CCommon::GetURL(L"https://v4.yinghualuo.cn/bejson", result, true, L"TrafficMonitor_V1.78");
    int a = 0;
}

static void TestGetLicense()
{
    CString license_str;
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_LICENSE), _T("TEXT"));
    if (hRes != NULL)
    {
        HGLOBAL hglobal = LoadResource(NULL, hRes);
        if (hglobal != NULL)
        {
            license_str = CCommon::StrToUnicode((const char*)hglobal, true).c_str();
            int a = 0;
        }
    }
}

static void TestSkin()
{
    CSkinFile skin;
    skin.Load(L"0默认皮肤");
    int a = 0;
}

static void TestCrash()
{
    CString* pStr = nullptr;
    int a = pStr->GetLength();
    printf("%d", a);

}

static void TestPlugin()
{
    if (!theApp.m_plugins.GetPlugins().empty())
    {
        theApp.m_plugins.GetPlugins()[0].plugin->ShowOptionsDialog(theApp.m_pMainWnd->m_hWnd);
    }
}

static void TestDate()
{
    Date d;
    d.year = 2021;
    d.month = 1;
    d.day = 4;
    int week = d.week();
    int a = 0;
}

static void TestIni()
{
    CIniHelper ini(L"D:\\Temp\\config.ini");
    ini.RemoveSection(L"skin_360悬浮窗dark");
    ini.Save();
    int a = 0;
}

static void TestPluginVersion()
{
    ASSERT(PluginVersion(L"1.0.0") == PluginVersion(L"1.00"));
    ASSERT(PluginVersion(L"1.2") < PluginVersion(L"1.20"));
    ASSERT(PluginVersion(L"0.8.0") < PluginVersion(L"1.00"));
    ASSERT(PluginVersion(L"1.0.3") < PluginVersion(L"1.03"));

    //CPluginUpdateHelper helper;
    //helper.CheckForUpdate();
    //int a = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTest::CTest()
{
}

CTest::~CTest()
{
}

void CTest::Test()
{
    //TestHttpQequest();
    //TestGetLicense();
    //TestSkin();
    //TestCrash();
    //TestDate();
    //TestIni();
    TestPluginVersion();
}

void CTest::TestCommand()
{
    //TestPlugin();

    //测试消息对话框
    CMessageDlg dlg;
    dlg.SetWindowTitle(_T("System Info"));
    dlg.SetInfoText(_T("System Information for TrafficMonitor."));
    dlg.SetMessageText(theApp.GetSystemInfoString().GetString());
    dlg.SetStandarnMessageIcon(CMessageDlg::SI_INFORMATION);
    dlg.DoModal();
}
