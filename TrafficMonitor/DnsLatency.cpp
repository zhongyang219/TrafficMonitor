#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "DnsLatency.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "icmp.lib")

HANDLE CDnsLatency::m_icmp_handle = INVALID_HANDLE_VALUE;

CDnsLatency::CDnsLatency()
{
}

CDnsLatency::~CDnsLatency()
{
    if (m_icmp_handle != INVALID_HANDLE_VALUE)
    {
        IcmpCloseHandle(m_icmp_handle);
        m_icmp_handle = INVALID_HANDLE_VALUE;
    }
}

int CDnsLatency::GetLatency(const char* ip_address, int timeout)
{
    if (m_icmp_handle == INVALID_HANDLE_VALUE)
    {
        m_icmp_handle = IcmpCreateFile();
    }

    if (m_icmp_handle == INVALID_HANDLE_VALUE)
        return -1;

    unsigned long ip = inet_addr(ip_address);
    if (ip == INADDR_NONE)
        return -1;

    char sendData[] = "PingData";
    LPVOID replyBuffer = NULL;
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
    replyBuffer = (VOID*)malloc(replySize);
    if (replyBuffer == NULL)
        return -1;

    int latency = -1;
    DWORD dwRetVal = IcmpSendEcho(m_icmp_handle, ip, sendData, sizeof(sendData), 
        NULL, replyBuffer, replySize, timeout);

    if (dwRetVal != 0)
    {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuffer;
        if (pEchoReply->Status == IP_SUCCESS)
        {
            latency = static_cast<int>(pEchoReply->RoundTripTime);
        }
    }

    free(replyBuffer);
    return latency;
}
