#pragma once
class CFilePathHelper
{
public:
	CFilePathHelper(const wstring& file_path);
	CFilePathHelper(){}
	~CFilePathHelper();

	void SetFilePath(const wstring& file_path) { m_file_path = file_path; }

	wstring GetFileExtension(bool upper = false, bool width_dot = false) const;		//获取文件的扩展名(upper:是否大写; width_dot:是否包含“.”)
	wstring GetFileName() const;							//获取文件名
	wstring GetFileNameWithoutExtension() const;			//获取文件名（不含扩展名）
	wstring GetFolderName() const;							//获取文件夹名
	wstring GetDir() const;									//获取目录
	wstring GetParentDir() const;							//获取上级目录
	wstring GetFilePath() const { return m_file_path; }		//获取完整路径
	const wstring& ReplaceFileExtension(const wchar_t* new_extension);		//替换文件的扩展名，返回文件完整路径
    wstring GetFilePathWithoutExtension() const;            //获取文件路径（不含扩展名）
protected:
	wstring m_file_path;
};

