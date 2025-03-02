#pragma once
#include "CommonData.h"
#include "SkinFile.h"

class CSkinManager
{
public:
    void Init();
    static CSkinManager& Instance();
    std::wstring GetSkinName(int index) const;
    int FindSkinIndex(const std::wstring& skin_name) const;
    bool GetSkinSettingDataByIndex(int index, SkinSettingData& skin_data);
    const vector<wstring>& GetSkinNames() const;
    int Size() const;
    void AddSkinSettingData(const std::wstring& skin_name, const SkinSettingData& data);

    void Save();

    //从一个皮肤文件获取SkinSettingData对象
    static void SkinSettingDataFronSkin(SkinSettingData& skin_setting_data, const CSkinFile& skin_file);

    //去掉皮肤名称前面的斜杠
    static void SkinNameNormalize(std::wstring& skin_name);

private:
    CSkinManager()
    {}

private:
    vector<wstring> m_skins;    //储存皮肤文件的路径
    std::map<std::wstring, SkinSettingData> m_skin_setting_data_map;    //保存每个皮肤的字体、显示文本设置

    static CSkinManager m_instance;
    bool m_init{ false };
};

