#pragma once
#include <string>

//用于获取网络延迟（ICMP ping）
class CPingHelper
{
public:
    //ping指定的IP地址，返回延迟（单位：毫秒）。超时或失败时返回-1
    static int Ping(const std::wstring& ip, int timeout_ms = 500);

    //判断一个字符串是否为有效的IPv4地址（点分十进制，四段，每段0~255）
    static bool IsValidIP(const std::wstring& ip);
};
