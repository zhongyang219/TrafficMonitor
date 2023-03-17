#pragma once
class CWinVersionHelper
{
public:
	CWinVersionHelper();
	~CWinVersionHelper();

	bool IsWindows11OrLater() const;			//判断当前Windows版本是否为Win11或更新的版本
	bool IsWindows10FallCreatorOrLater() const;		//判断当前Windows版本是否为Win10秋季创意者更新或更新的版本
	bool IsWindows7() const;					//判断Windows版本是否为Windows7
	bool IsWindows8Or8point1() const;			//判断Windows版本是否为Windows8或Windows8.1
    bool IsWindows8Point1OrLater() const;       //判断Windows版本是否大于等于Windows8.1
    bool IsWindows8OrLater() const;
    bool IsWindows10OrLater() const;

	int GetMajorVersion() const { return m_major_version; }
	int GetMinorVersion() const { return m_minor_version; }
	int GetBuildNumber() const { return m_build_number; }
protected:
	int m_major_version{};
	int m_minor_version{};
	int m_build_number{};
};
