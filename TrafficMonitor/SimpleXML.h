//一个简易的XML解析的类
#pragma once
#include "Common.h"

class CSimpleXML
{
public:
	CSimpleXML(const wstring& xml_path);
	CSimpleXML();
	~CSimpleXML();

	void LoadXMLContentDirect(const wstring& xml_content) { m_xml_content = xml_content; }

	wstring GetNode(const wchar_t* node, const wchar_t* parent) const;
	wstring GetNode(const wchar_t* node) const;

	static wstring _GetNode(const wchar_t* node, const wstring& content);

protected:
	wstring m_xml_content;

};

