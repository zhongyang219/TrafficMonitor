#pragma once
class CUpdateHelper
{
public:
    CUpdateHelper();
    ~CUpdateHelper();

    enum class UpdateSource
    {
        GitHubSource,
        GiteeSource
    };

    void SetUpdateSource(UpdateSource update_source);

    bool CheckForUpdate();

    const std::wstring& GetVersion() const;
    const std::wstring& GetLink() const;
    const std::wstring& GetLink64() const;
    const std::wstring& GetLinkArm64ec() const;
    const std::wstring& GetContentsEn() const;
    const std::wstring& GetContentsZhCn() const;
    const std::wstring& GetContentsZhTw() const;
    bool IsRowData();

private:
    void ParseUpdateInfo(wstring version_info);

private:
    std::wstring m_version;
    std::wstring m_link;
    std::wstring m_link64;
    std::wstring m_link_arm64ec;
    std::wstring m_contents_en;
    std::wstring m_contents_zh_cn;
    std::wstring m_contents_zh_tw;
    bool m_row_data{ true };
    UpdateSource m_update_source{ UpdateSource::GitHubSource };
};
