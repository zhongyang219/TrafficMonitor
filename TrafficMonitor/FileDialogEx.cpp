#include "stdafx.h"
#include "FileDialogEx.h"
#include <sstream>

CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCWSTR lpszDefExt, LPCWSTR lpszFilter)
    : pFileDialog(nullptr), m_bOpenFileDialog(bOpenFileDialog)
{
    // 创建 IFileDialog 实例
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

    if (pFileDialog)
    {
        // 设置对话框选项
        DWORD dwOptions;
        pFileDialog->GetOptions(&dwOptions);
        pFileDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);  // 仅文件系统项

        if (bOpenFileDialog)
        {
            pFileDialog->SetOptions(dwOptions | FOS_FILEMUSTEXIST); // 打开文件时文件必须存在
        }

        // 设置默认扩展名
        if (lpszDefExt)
        {
            pFileDialog->SetDefaultExtension(lpszDefExt);
        }

        // 解析并设置过滤器
        if (lpszFilter)
        {
            ParseFilter(lpszFilter);
            for (size_t i = 0; i < m_filterDescriptions.size() && i < m_filterSpecsStrings.size(); i++)
            {
                COMDLG_FILTERSPEC filterSpec = { m_filterDescriptions[i].c_str(), m_filterSpecsStrings[i].c_str()};
                m_filterSpecs.push_back(filterSpec);
            }
            pFileDialog->SetFileTypes(m_filterSpecs.size(), m_filterSpecs.data());
        }
    }
}

CFileDialogEx::~CFileDialogEx()
{
    if (pFileDialog)
    {
        pFileDialog->Release();
    }
}

int CFileDialogEx::DoModal(HWND hWndOwner)
{
    if (!pFileDialog) return -1;

    // 显示对话框
    HRESULT hr = pFileDialog->Show(hWndOwner);
    if (FAILED(hr))
        return -1;

    // 获取选定的文件路径
    IShellItem* pItem = nullptr;
    hr = pFileDialog->GetResult(&pItem);
    if (SUCCEEDED(hr))
    {
        PWSTR pszFilePath = nullptr;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (SUCCEEDED(hr))
        {
            m_filePath = pszFilePath;
            CoTaskMemFree(pszFilePath);
        }
        pItem->Release();
    }

    return (SUCCEEDED(hr)) ? 1 : 0;
}

std::wstring CFileDialogEx::GetPathName() const
{
    return m_filePath;
}

std::wstring CFileDialogEx::GetFileName() const
{
    // 获取文件名
    size_t pos = m_filePath.find_last_of(L"\\");
    if (pos != std::wstring::npos) {
        return m_filePath.substr(pos + 1);
    }
    return m_filePath;
}

void CFileDialogEx::ParseFilter(LPCWSTR lpszFilter)
{
    m_filterSpecs.clear();
    m_filterDescriptions.clear();
    m_filterSpecsStrings.clear();

    // 使用 | 分割过滤器字符串
    std::wstringstream filterStream(lpszFilter);
    std::wstring description, spec;

    while (std::getline(filterStream, description, L'|'))
    {
        if (std::getline(filterStream, spec, L'|'))
        {
            // 将描述和模式存储在 m_filterDescriptions 和 m_filterSpecsStrings 中
            m_filterDescriptions.push_back(description);
            m_filterSpecsStrings.push_back(spec);
        }
    }
}
