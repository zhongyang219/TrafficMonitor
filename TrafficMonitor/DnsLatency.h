#pragma once
#include <icmpapi.h>
#include <iphlpapi.h>

class CDnsLatency
{
public:
    CDnsLatency();
    ~CDnsLatency();

    // Executa o ping de forma assíncrona. 
    // Retorna a latência em ms ou -1 se falhar.
    static int GetLatency(const char* ip_address, int timeout = 1000);

private:
    static HANDLE m_icmp_handle;
};
