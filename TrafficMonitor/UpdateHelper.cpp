#include "stdafx.h"
#include "UpdateHelper.h"
#include "SimpleXML.h"
#include "Common.h"


CUpdateHelper::CUpdateHelper()
{
}


CUpdateHelper::~CUpdateHelper()
{
}

bool CUpdateHelper::CheckForUpdate()
{
    wstring version_info;
    if (CCommon::GetURL(L"https://raw.githubusercontent.com/zhongyang219/TrafficMonitor/master/version_utf8.info", version_info, true))		//获取版本信息
    {
        m_row_data = true;
    }
    else if (CCommon::GetURL(L"https://github.com/zhongyang219/TrafficMonitor/blob/master/version_utf8.info", version_info, true))		//获取版本信息
    {
        m_row_data = false;
    }
    else
    {
        return false;
    }

    if (!m_row_data)
    {
        size_t index = version_info.find(L"﻿&lt;version&gt;");
        if (index != std::wstring::npos)
            version_info = version_info.substr(index);

        CString str_version_info = version_info.c_str();
        str_version_info.Replace(L"&lt;", L"<");
        str_version_info.Replace(L"&gt;", L">");

        version_info = str_version_info;
    }

    CSimpleXML version_xml;
    version_xml.LoadXMLContentDirect(version_info);

    m_version = version_xml.GetNode(L"version");
    m_link64 = version_xml.GetNode(L"link_x64");
    m_link = version_xml.GetNode(L"link");
    CString contents_zh_cn = version_xml.GetNode(L"contents_zh_cn", L"update_contents").c_str();
    CString contents_en = version_xml.GetNode(L"contents_en", L"update_contents").c_str();
    CString contents_zh_tw = version_xml.GetNode(L"contents_zh_tw", L"update_contents").c_str();
    contents_zh_cn.Replace(L"\\n", L"\r\n");
    contents_en.Replace(L"\\n", L"\r\n");
    contents_zh_tw.Replace(L"\\n", L"\r\n");
    m_contents_zh_cn = contents_zh_cn;
    m_contents_en = contents_en;
    m_contents_zh_tw = contents_zh_tw;

    return true;
}

const std::wstring & CUpdateHelper::GetVersion() const
{
    return m_version;
}

const std::wstring & CUpdateHelper::GetLink() const
{
    return m_link;
}

const std::wstring & CUpdateHelper::GetLink64() const
{
    return m_link64;
}

const std::wstring & CUpdateHelper::GetContentsEn() const
{
    return m_contents_en;
}

const std::wstring & CUpdateHelper::GetContentsZhCn() const
{
    return m_contents_zh_cn;
}

const std::wstring& CUpdateHelper::GetContentsZhTw() const
{
    return m_contents_zh_tw;
}

bool CUpdateHelper::IsRowData()
{
    return m_row_data;
}
