#include "StdAfx.h"
#include "crashtool.h"

#include <strsafe.h>
#include <DbgHelp.h>
#include <tchar.h>
#include "MessageDlg.h"
#include "Common.h"
#include "TrafficMonitor.h"

#pragma comment(lib, "Dbghelp.lib")

class CCrashReport
{
public:
    CCrashReport()
    {
        GetAppPath();
    }
    ~CCrashReport() {}
public:
    // 生成MiniDump文件
    void CreateMiniDump(EXCEPTION_POINTERS* pEP)
    {
        SYSTEMTIME stLocalTime;
        ::GetLocalTime(&stLocalTime);
        TCHAR szDumpFile[MAX_PATH] = {0};
        ::StringCchPrintf(szDumpFile, _countof(szDumpFile), TEXT("%s%04d%02d%02d%02d%02d%02d_%s.dmp"), m_szDumpFilePath,
                          stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, m_szModuleFileName);

        HANDLE hDumpFile;
        hDumpFile = ::CreateFile(szDumpFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
        if (INVALID_HANDLE_VALUE == hDumpFile)
        {
            return;
        }

		m_dumpFile = szDumpFile;

        MINIDUMP_EXCEPTION_INFORMATION ExpParam;
        ExpParam.ThreadId = ::GetCurrentThreadId();
        ExpParam.ExceptionPointers = pEP;
        ExpParam.ClientPointers = TRUE;

        // 生成minidump文件
        BOOL bResult = ::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &ExpParam, NULL, NULL);
        ::CloseHandle(hDumpFile);
    }

	void ShowCrashInfo()
	{
		CMessageDlg dlg;
		dlg.SetWindowTitle(CCommon::LoadText(IDS_ERROR1));
		dlg.SetInfoText(CCommon::LoadText(IDS_ERROR_MESSAGE));

		CString info = CCommon::LoadTextFormat(IDS_CRASH_INFO, { m_dumpFile });
		info += _T("\r\n");
		info += theApp.GetSystemInfoString();

		dlg.SetMessageText(info);
		dlg.DoModal();
	}

private:
    void GetAppPath()
    {
        ZeroMemory(m_szModuleFileName, MAX_PATH);
        ::GetModuleFileName(NULL, m_szDumpFilePath, _countof(m_szDumpFilePath));
        for (int nIndex = (int)_tcslen(m_szDumpFilePath); nIndex >= 0; --nIndex)
        {
            if (m_szDumpFilePath[nIndex] == TEXT('\\'))
            {
                ::memmove(m_szModuleFileName, m_szDumpFilePath + nIndex + 1, (int)_tcslen(m_szDumpFilePath));
                m_szDumpFilePath[nIndex + 1] = 0;
                break;
            }
        }
        ZeroMemory(m_szDumpFilePath, MAX_PATH);
        if (!::GetTempPath(MAX_PATH, m_szDumpFilePath))
        {
            m_szDumpFilePath[0] = _T('C');
            m_szDumpFilePath[1] = _T(':');
            m_szDumpFilePath[2] = _T('\\');
            m_szDumpFilePath[3] = _T('\0');
        }
    }
private:
    wchar_t m_szDumpFilePath[MAX_PATH];
    wchar_t m_szModuleFileName[MAX_PATH];

	CString m_dumpFile;
};

namespace CRASHREPORT
{
    static LONG WINAPI __UnhandledExceptionFilter(PEXCEPTION_POINTERS pEP)
    {
        ::SetErrorMode(0); //使用默认的
        CCrashReport cr;
        cr.CreateMiniDump(pEP);
		cr.ShowCrashInfo();
        return EXCEPTION_CONTINUE_SEARCH;
    }

    void StartCrashReport()
    {
        ::SetUnhandledExceptionFilter(__UnhandledExceptionFilter);
    }
}
