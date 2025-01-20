#pragma once

class CFileDialogEx {
public:
    CFileDialogEx(BOOL bOpenFileDialog, LPCWSTR lpszDefExt = NULL, LPCWSTR lpszFilter = NULL);
    ~CFileDialogEx();

    // 显示对话框
    int DoModal(HWND hWndOwner = NULL);

    // 获取选定的文件路径和文件名
    std::wstring GetPathName() const;
    std::wstring GetFileName() const;

private:
    void ParseFilter(LPCWSTR lpszFilter);

private:
    IFileDialog* pFileDialog;
    std::wstring m_filePath;
    BOOL m_bOpenFileDialog;
    std::vector<COMDLG_FILTERSPEC> m_filterSpecs;  // 存储过滤器描述和模式
    std::vector<std::wstring> m_filterDescriptions;  // 存储过滤器描述
    std::vector<std::wstring> m_filterSpecsStrings;  // 存储过滤器模式
};
