#include "stdafx.h"
#include "Test.h"
#include "Common.h"
#include "SkinFile.h"
#include "TrafficMonitor.h"
#include "IniHelper.h"

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
}

void CTest::TestCommand()
{
    //TestPlugin();
}

void CTest::TestHttpQequest()
{
    wstring result;
    bool rtn = CCommon::GetURL(L"https://v4.yinghualuo.cn/bejson", result, true, L"TrafficMonitor_V1.78");
    int a = 0;
}

void CTest::TestGetLicense()
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

void CTest::TestSkin()
{
    CSkinFile skin;
    skin.Load(L"0默认皮肤");
    int a = 0;
}

void CTest::TestCrash()
{
    CString* pStr = nullptr;
    int a = pStr->GetLength();
    printf("%d", a);

}

void CTest::TestPlugin()
{
    if (!theApp.m_plugins.GetPlugins().empty())
    {
        theApp.m_plugins.GetPlugins()[0].plugin->ShowOptionsDialog(theApp.m_pMainWnd->m_hWnd);
    }
}

void CTest::TestDate()
{
    Date d;
    d.year = 2021;
    d.month = 1;
    d.day = 4;
    int week = d.week();
    int a = 0;
}

void CTest::TestIni()
{
    CIniHelper ini(L"D:\\Temp\\config.ini");
    ini.RemoveSection(L"skin_360悬浮窗dark");
    ini.Save();
    int a = 0;
}
