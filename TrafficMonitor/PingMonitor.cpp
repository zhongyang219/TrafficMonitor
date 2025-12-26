#include "stdafx.h"
#include "PingMonitor.h"
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

CPingMonitor::CPingMonitor()
    : m_ping_ms(0)
    , m_ping_failed(true)
{
}

CPingMonitor::~CPingMonitor()
{
}

void CPingMonitor::SetPingServer(const CString& server)
{
    CSingleLock lock(&m_critical, TRUE);
    m_ping_server = server;
}

int CPingMonitor::GetPingResult()
{
    CSingleLock lock(&m_critical, TRUE);
    return m_ping_ms;
}

bool CPingMonitor::IsPingFailed() const
{
    return m_ping_failed;
}

ULONG CPingMonitor::ResolveHost(const CString& host)
{
#pragma warning(push)
#pragma warning(disable:4996)
    // Use CStringA for ANSI conversion (native MFC class)
    CStringA host_str(host);
    
    // Try to parse as IPv4 address directly
    ULONG ip_addr = inet_addr(host_str);
    if (ip_addr != INADDR_NONE)
    {
#pragma warning(pop)
        return ip_addr;
    }

    // If not an IP, try DNS resolution
    struct addrinfo hints = { 0 };
    struct addrinfo* result = nullptr;
    
    hints.ai_family = AF_INET;  // IPv4 only
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host_str, nullptr, &hints, &result) == 0)
    {
        if (result != nullptr && result->ai_family == AF_INET)
        {
            struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)result->ai_addr;
            ip_addr = sockaddr_ipv4->sin_addr.S_un.S_addr;
            freeaddrinfo(result);
#pragma warning(pop)
            return ip_addr;
        }
        if (result != nullptr)
            freeaddrinfo(result);
    }
#pragma warning(pop)
    return INADDR_NONE;
}

void CPingMonitor::DoPing()
{
    CSingleLock lock(&m_critical, TRUE);
    
    if (m_ping_server.IsEmpty())
    {
        m_ping_failed = true;
        return;
    }

    // 解析主机地址
    ULONG ip_addr = ResolveHost(m_ping_server);
    if (ip_addr == INADDR_NONE)
    {
        m_ping_failed = true;
        return;
    }

    // 创建ICMP句柄
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
    {
        m_ping_failed = true;
        return;
    }

    // 准备ping数据
    char SendData[] = "TrafficMonitor Ping";
    DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    LPVOID ReplyBuffer = (VOID*)malloc(ReplySize);
    
    if (ReplyBuffer == nullptr)
    {
        IcmpCloseHandle(hIcmpFile);
        m_ping_failed = true;
        return;
    }

    // 发送ping请求（超时1000ms）
    DWORD dwRetVal = IcmpSendEcho(hIcmpFile, ip_addr, SendData, sizeof(SendData),
        nullptr, ReplyBuffer, ReplySize, 1000);

    if (dwRetVal != 0)
    {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        
        if (pEchoReply->Status == IP_SUCCESS)
        {
            m_ping_ms = pEchoReply->RoundTripTime;
            m_ping_failed = false;
        }
        else
        {
            m_ping_failed = true;
        }
    }
    else
    {
        m_ping_failed = true;
    }

    // 清理资源
    free(ReplyBuffer);
    IcmpCloseHandle(hIcmpFile);
}
