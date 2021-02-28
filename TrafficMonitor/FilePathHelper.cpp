#include "stdafx.h"
#include "FilePathHelper.h"
#include "Common.h"


CFilePathHelper::CFilePathHelper(const wstring & file_path)
	: m_file_path{ file_path }
{
}

CFilePathHelper::~CFilePathHelper()
{
}

wstring CFilePathHelper::GetFileExtension(bool upper, bool width_dot) const
{
	size_t index;
	index = m_file_path.rfind('.');
	if (index == wstring::npos || index == m_file_path.size() - 1)
		return wstring();
	wstring file_extension{ m_file_path.substr(width_dot ? index : index + 1) };
	CCommon::StringTransform(file_extension, upper);
	return file_extension;
}

wstring CFilePathHelper::GetFileName() const
{
	size_t index;
	index = m_file_path.rfind('\\');
	if (index == wstring::npos)
		index = m_file_path.rfind('/');
	return m_file_path.substr(index + 1);
}

wstring CFilePathHelper::GetFileNameWithoutExtension() const
{
	size_t index, index1;
	index = m_file_path.rfind('.');
	index1 = m_file_path.rfind('\\');
	if (index1 == wstring::npos)
		index1 = m_file_path.rfind('/');
	return m_file_path.substr(index1 + 1, (index - index1 - 1));
}

wstring CFilePathHelper::GetFolderName() const
{
	int index, index1;
	index = m_file_path.find_last_of(L"\\/");
	if (index == wstring::npos || index == 0)
		return wstring();

	index1 = m_file_path.find_last_of(L"\\/", index - 1);
	if (index1 == wstring::npos || index1 == 0)
		return wstring();

	return m_file_path.substr(index1 + 1, (index - index1 - 1));
}

wstring CFilePathHelper::GetDir() const
{
	if (!m_file_path.empty() && (m_file_path.back() == L'\\' || m_file_path.back() == L'/'))
		return m_file_path;
	size_t index;
	index = m_file_path.rfind('\\');
	if (index == wstring::npos)
		index = m_file_path.rfind('/');
	return m_file_path.substr(0, index + 1);
}

wstring CFilePathHelper::GetParentDir() const
{
	wstring dir{ GetDir() };
	size_t index;
	if (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
		dir.pop_back();
	index = dir.rfind('\\');
	if (index == wstring::npos)
		index = dir.rfind('/');
	return m_file_path.substr(0, index + 1);
}

const wstring& CFilePathHelper::ReplaceFileExtension(const wchar_t * new_extension)
{
	size_t index, index1;
	index = m_file_path.rfind('.');
	index1 = m_file_path.rfind('\\');
	if (index == wstring::npos || (index1 != wstring::npos && index < index1))		//如果没有找到“.”，或者“.”在反斜杠的左边，则在末尾添加一个“.”
	{
		m_file_path.push_back(L'.');
	}
	else if (index != m_file_path.size() - 1)	//如果“.”不在最后的位置，则删除“.”后面的字符串
	{
		m_file_path.erase(index + 1);
	}
    if (new_extension == nullptr || *new_extension == L'\0')
    {
        if (!m_file_path.empty() && m_file_path.back() == L'.')
            m_file_path.pop_back();
    }
    else
    {
        m_file_path.append(new_extension);		//在末尾添加扩展名
    }
	return m_file_path;
}

wstring CFilePathHelper::GetFilePathWithoutExtension() const
{
    size_t index;
    index = m_file_path.rfind('.');
    return m_file_path.substr(0, index);
}
