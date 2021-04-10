#pragma once
#include "Common.h"

//保存一个网络连接信息
struct NetWorkConection
{
	int index{};			//该连接在MIB_IF_TABLE2中的索引
	wstring description;		//网络描述（获取自GetAdapterInfo）
	wstring description_2;	//网络描述（获取自GetIfTable）
	unsigned __int64 in_bytes;	//初始时已接收字节数
	unsigned __int64 out_bytes;	//初始时已发送字节数
	wstring ip_address{ L"-.-.-.-" };	//IP地址
	wstring subnet_mask{ L"-.-.-.-" };	//子网掩码
	wstring default_gateway{ L"-.-.-.-" };	//默认网关
};

class CAdapterCommon
{
public:
	CAdapterCommon();
	~CAdapterCommon();

	//获取网络连接列表，填充网络描述、IP地址、子网掩码、默认网关信息
	static void GetAdapterInfo(vector<NetWorkConection>& adapters);

	//刷新网络连接列表中的IP地址、子网掩码、默认网关信息
	static void RefreshIpAddress(vector<NetWorkConection>& adapters);

	//获取网络列表中每个网络连接的MIB_IF_TABLE2中的索引、初始时已接收/发送字节数的信息
	static void GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IF_TABLE2* pIfTable);

	//直接将MIB_IF_TABLE2中的所有连接添加到adapters容器中
	static void GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IF_TABLE2* pIfTable);
private:
	//根据一个网络连接描述判断是否在IfTable列表里，返回索引，找不到则返回-1
	static int FindConnectionInIfTable(wstring connection, MIB_IF_TABLE2* pIfTable);

	//根据一个网络连接描述判断是否在IfTable接列表里，返回索引，找不到则返回-1。只需要部分匹配
	static int FindConnectionInIfTableFuzzy(wstring connection, MIB_IF_TABLE2* pIfTable);
};

