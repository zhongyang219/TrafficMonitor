#include "stdafx.h"
#include "PingHelper.h"
#include <iphlpapi.h>
#include <icmpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int CPingHelper::Ping(const std::wstring& ip)
{
    IN_ADDR addr{};
    if (InetPtonW(AF_INET, ip.c_str(), &addr) != 1)
        return -1;

    HANDLE h_icmp = IcmpCreateFile();
    if (h_icmp == INVALID_HANDLE_VALUE)
        return -1;

    char send_data[32]{ "TrafficMonitorPing" };
    //回应的缓冲区必须按ICMP_ECHO_REPLY对齐，且容量要足够容纳ICMP_ECHO_REPLY结构、回送的数据和ICMP报头
    //（缓冲区不够时IcmpSendEcho会直接返回失败，因此这里留足余量）
    union
    {
        ICMP_ECHO_REPLY reply;
        BYTE buffer[sizeof(ICMP_ECHO_REPLY) + sizeof(send_data) + 64];
    } reply_data{};

    DWORD reply_count = IcmpSendEcho(h_icmp, addr.S_un.S_addr, send_data, sizeof(send_data), nullptr,
        reply_data.buffer, sizeof(reply_data.buffer), 500);      //超时时间（毫秒）

    int delay{ -1 };
    if (reply_count > 0 && reply_data.reply.Status == IP_SUCCESS)
        delay = static_cast<int>(reply_data.reply.RoundTripTime);

    IcmpCloseHandle(h_icmp);
    return delay;
}

bool CPingHelper::IsValidIP(const std::wstring& ip)
{
    IN_ADDR addr{};
    return InetPtonW(AF_INET, ip.c_str(), &addr) == 1;
}
