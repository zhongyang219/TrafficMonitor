#pragma once
#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

// Ping监控类
class CPingMonitor
{
public:
    CPingMonitor();
    ~CPingMonitor();

    // 设置ping的目标服务器
    void SetPingServer(const CString& server);

    // 获取最后一次ping的结果（毫秒）
    int GetPingResult();

    // 执行一次ping操作
    void DoPing();

    // 检查最后一次ping是否失败
    bool IsPingFailed() const;

private:
    CString m_ping_server;     // 要ping的服务器地址
    int m_ping_ms;                  // 最后一次ping的结果（毫秒）
    bool m_ping_failed;             // 最后一次ping是否失败
    CCriticalSection m_critical;    // 用于线程同步

    // 解析域名或IP地址
    ULONG ResolveHost(const CString& host);
};
