#include "stdafx.h"
#include "AdapterCommon.h"


CAdapterCommon::CAdapterCommon()
{
}


CAdapterCommon::~CAdapterCommon()
{
}

void CAdapterCommon::GetAdapterInfo(vector<NetWorkConection>& adapters)
{
	adapters.clear();
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();		//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);		//得到结构体大小,用于GetAdaptersInfo参数
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		delete pIpAdapterInfo;	//释放原来的内存空间
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];	//重新申请内存空间用来存储所有网卡信息
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
	}

	PIP_ADAPTER_INFO pIpAdapterInfoHead = pIpAdapterInfo;	//保存pIpAdapterInfo链表中第一个元素的地址
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			NetWorkConection connection;
			connection.description = pIpAdapterInfo->Description;
			connection.ip_address = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpAddress.String);
			connection.subnet_mask = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpMask.String);
			connection.default_gateway = CCommon::StrToUnicode(pIpAdapterInfo->GatewayList.IpAddress.String);

			adapters.push_back(connection);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//释放内存空间
	if (pIpAdapterInfoHead)
	{
		delete pIpAdapterInfoHead;
	}
	if (adapters.empty())
	{
		NetWorkConection connection{};
		connection.description = CCommon::UnicodeToStr(CCommon::LoadText(L"<", IDS_NO_CONNECTION, L">"));
		adapters.push_back(connection);
	}
}

void CAdapterCommon::GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable)
{
	//依次在IfTable里查找每个连接
	for (size_t i{}; i < adapters.size(); i++)
	{
		if (adapters[i].description.empty())
			continue;
		int index;
		index = FindConnectionInIfTable(adapters[i].description, pIfTable);
		if (index == -1)		//如果使用精确匹配的方式没有找到，则采用模糊匹配的方式再查找一次
			index = FindConnectionInIfTableFuzzy(adapters[i].description, pIfTable);
		//if (index != -1)
		//{
		adapters[i].index = index;
		adapters[i].in_bytes = pIfTable->table[index].dwInOctets;
		adapters[i].out_bytes = pIfTable->table[index].dwOutOctets;
		adapters[i].description_2 = (const char*)pIfTable->table[index].bDescr;
		//}
	}
}

void CAdapterCommon::GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE * pIfTable)
{
	vector<NetWorkConection> adapters_tmp;
	GetAdapterInfo(adapters_tmp);		//获取IP地址
	adapters.clear();
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		NetWorkConection connection;
		connection.description = connection.description_2 = (const char*)pIfTable->table[i].bDescr;
		connection.index = i;
		connection.in_bytes = pIfTable->table[i].dwInOctets;
		connection.out_bytes = pIfTable->table[i].dwOutOctets;
		for (size_t j{}; j < adapters_tmp.size(); j++)
		{
			if (connection.description.find(adapters_tmp[j].description) != string::npos)
			{
				connection.ip_address = adapters_tmp[j].ip_address;
				connection.subnet_mask = adapters_tmp[j].subnet_mask;
				connection.default_gateway = adapters_tmp[j].default_gateway;
				break;
			}
		}
		adapters.push_back(connection);
	}
}

int CAdapterCommon::FindConnectionInIfTable(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		if (descr == connection)
			return i;
	}
	return -1;
}

int CAdapterCommon::FindConnectionInIfTableFuzzy(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		size_t index;
		//在较长的字符串里查找较短的字符串
		if (descr.size() >= connection.size())
			index = descr.find(connection);
		else
			index = connection.find(descr);
		if (index != wstring::npos)
			return i;
	}
	//如果还是没有找到，则使用字符串匹配算法查找
	double max_degree{};
	int best_index{};
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		double degree = CCommon::StringSimilarDegree_LD(descr, connection);
		if (degree > max_degree)
		{
			max_degree = degree;
			best_index = i;
		}
	}
	return best_index;
}
