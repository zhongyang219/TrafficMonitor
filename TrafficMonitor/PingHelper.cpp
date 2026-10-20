#include "stdafx.h"
#include "PingHelper.h"
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//把点分十进制的IPv4地址转换成IN_ADDR，无效时返回false
static bool ParseIPv4(const std::wstring& ip, IN_ADDR& addr)
{
    int part_index{};       //当前是第几段
    int part_value{};       //当前段的值
    int digit_count{};      //当前段的数字个数
    for (size_t i{}; i <= ip.size(); i++)
    {
        if (i == ip.size() || ip[i] == L'.')
        {
            //每段必须有1~3个数字，且值不能大于255
            if (digit_count == 0 || digit_count > 3 || part_value > 255 || part_index > 3)
                return false;
            reinterpret_cast<BYTE*>(&addr)[part_index] = static_cast<BYTE>(part_value);
            part_index++;
            part_value = 0;
            digit_count = 0;
        }
        else if (ip[i] >= L'0' && ip[i] <= L'9')
        {
            part_value = part_value * 10 + (ip[i] - L'0');
            digit_count++;
        }
        else
        {
            return false;       //含有非数字字符
        }
    }
    return part_index == 4;     //必须正好是4段
}

int CPingHelper::Ping(const std::wstring& ip, int timeout_ms)
{
    IN_ADDR addr{};
    if (!ParseIPv4(ip, addr))
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
        reply_data.buffer, sizeof(reply_data.buffer), static_cast<DWORD>(timeout_ms));

    int delay{ -1 };
    if (reply_count > 0 && reply_data.reply.Status == IP_SUCCESS)
        delay = static_cast<int>(reply_data.reply.RoundTripTime);

    IcmpCloseHandle(h_icmp);
    return delay;
}

bool CPingHelper::IsValidIP(const std::wstring& ip)
{
    IN_ADDR addr{};
    return ParseIPv4(ip, addr);
}
