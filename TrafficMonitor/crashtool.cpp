#include "StdAfx.h"
#include "crashtool.h"

#include <strsafe.h>
#include <DbgHelp.h>
#include <tchar.h>
#include "MessageDlg.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include <sstream>

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

    //根据地址获取模块路径
    std::wstring GetModulePath(DWORD64 address)
    {
        // 获取模块信息
        HMODULE hModule = NULL;
        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)address, &hModule)) 
        {
            TCHAR moduleName[MAX_PATH];
            if (GetModuleFileName(hModule, moduleName, MAX_PATH))
            {
                return moduleName;
            }
        }
        return L"Unknown Module";
    }

    //获取崩溃堆栈信息
    std::wstring GetStackTrace(EXCEPTION_POINTERS* pExceptionInfo)
    {
        std::wstringstream stream;

        // 初始化符号处理
        if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
        {
            stream << L"Failed to initialize symbol handler.\r\n";
            return stream.str();
        }

        // RAII 确保 SymCleanup 被调用
        struct SymCleanupHelper {
            ~SymCleanupHelper() { SymCleanup(GetCurrentProcess()); }
        } cleanupHelper;

        STACKFRAME64 stackFrame = {};
        CONTEXT context = *pExceptionInfo->ContextRecord;

        // 初始化堆栈帧
#if defined _M_IX86
        DWORD machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context.Eip;    // x86 使用 EIP
        stackFrame.AddrFrame.Offset = context.Ebp; // x86 使用 EBP
        stackFrame.AddrStack.Offset = context.Esp; // x86 使用 ESP
//#elif defined _M_ARM64EC
//        DWORD machineType = IMAGE_FILE_MACHINE_ARM64;
//        stackFrame.AddrPC.Offset = context.Pc;     // ARM64 使用 PC
//        stackFrame.AddrFrame.Offset = context.Fp;  // ARM64 使用 FP
//        stackFrame.AddrStack.Offset = context.Sp;  // ARM64 使用 SP
#else
        DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context.Rip;    // x64 使用 RIP
        stackFrame.AddrFrame.Offset = context.Rbp; // x64 使用 RBP
        stackFrame.AddrStack.Offset = context.Rsp; // x64 使用 RSP
#endif
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;

        // 遍历堆栈帧
        while (StackWalk64(machineType, GetCurrentProcess(), GetCurrentThread(), &stackFrame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
            if (stackFrame.AddrPC.Offset == 0) break;

            // 获取符号信息
            BYTE symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;

            stream << L"--------------------------------------\r\n";

            DWORD64 displacement = 0;
            if (SymFromAddr(GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, pSymbol)) {
                stream << L"Function: " << CCommon::AsciiToUnicode(pSymbol->Name) << L" (Displacement: " << displacement << L")\r\n";
            }
            else {
                stream << L"Unknown Function at address: " << (void*)stackFrame.AddrPC.Offset << L"\r\n";
            }

            std::wstring modulePath = GetModulePath(stackFrame.AddrPC.Offset);
            if (!modulePath.empty())
                stream << L"Module Path: " << modulePath << L"\r\n";

            // 获取源代码行信息
            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(GetCurrentProcess(), stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
                stream << L"File: " << CCommon::AsciiToUnicode(line.FileName) << L" (Line: " << line.LineNumber << L")\r\n";
            }
        }
        return stream.str();
    }

	void ShowCrashInfo(EXCEPTION_POINTERS* pEP)
	{
		CString info = CCommon::LoadTextFormat(IDS_CRASH_INFO, { m_dumpFile });
		info += _T("\r\n");
        //在崩溃信息中调用堆栈
        std::wstring stack_trace = GetStackTrace(pEP);
        if (!stack_trace.empty())
        {
            info += _T("Stack trace:\r\n");
            info += stack_trace.c_str();
            info += _T("\r\n");
        }
		info += theApp.GetSystemInfoString();
        //写入日志
        CString crash_log = info;
        crash_log.Replace(_T("\r\n"), _T("\n"));
        crash_log.Replace(_T("\n\n"), _T("\n"));
        CCommon::WriteLog(crash_log.GetString(), theApp.m_log_path.c_str());
        //显示崩溃对话框
        CMessageDlg dlg;
        dlg.SetWindowTitle(APP_NAME);
        dlg.SetInfoText(CCommon::LoadText(IDS_ERROR_MESSAGE));
        dlg.SetMessageText(info);
        dlg.SetStandarnMessageIcon(CMessageDlg::SI_ERROR);
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
		cr.ShowCrashInfo(pEP);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    void StartCrashReport()
    {
        ::SetUnhandledExceptionFilter(__UnhandledExceptionFilter);
    }
}
