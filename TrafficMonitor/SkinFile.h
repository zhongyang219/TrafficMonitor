#pragma once
#include "CommonData.h"
#include "TinyXml2Helper.h"
#include "DrawCommon.h"
#include <gdiplus.h>

class CSkinFile
{
public:
    CSkinFile();
    ~CSkinFile();


    //从文件载入皮肤信息
    void Load(const wstring& file_path);

    //皮肤信息
    struct SkinInfo
    {
        std::vector<COLORREF> text_color;   //文本颜色
        bool specify_each_item_color{};     //是否指定每个项目的颜色
        wstring skin_author;                //皮肤的作者
        FontInfo font_info;                 //字体信息
        DispStrings display_text;    //每一项的显示文本

        COLORREF TextColor(size_t i) const
        {
            if (i >= 0 && i < text_color.size())
                return text_color[i];
            else if (!text_color.empty())
                return text_color.front();
            else
                return 0;
        }
    };

    //皮肤中每一项的布局信息
    struct LayoutItem
    {
        int x{};            //X位置
        int y{};            //Y位置
        int width{};        //宽度
        Alignment align{};  //对齐方式
        bool show{};        //是否显示
    };

    //皮肤布局
    struct Layout
    {
        int width{};        //宽度
        int height{};       //高度
        std::map<CommonDisplayItem, LayoutItem> layout_items; //每一项的布局信息
        LayoutItem GetItem(CommonDisplayItem display_item) const
        {
            auto iter = layout_items.find(display_item);
            if (iter != layout_items.end())
                return iter->second;
            return LayoutItem();
        }
    };

    //皮肤布局信息
    struct LayoutInfo
    {
        int text_height{};  //皮肤文本的高度
        bool no_label{};    //是否不显示标签
        Layout layout_l;    //“显示更多信息”时的布局
        Layout layout_s;    //不“显示更多信息”时的布局
    };

    //皮肤预览图信息
    struct PreviewInfo
    {
        struct Pos
        {
            int x{};
            int y{};
        };
        int width{};    //预览图的宽度
        int height{};   //预览图的高度
        Pos l_pos;      //“显示更多信息”时的窗口在预览图中的位置
        Pos s_pos;      //不“显示更多信息”时的窗口在预览图中的位置
    };

    const SkinInfo& GetSkinInfo() const { return m_skin_info; }
    const LayoutInfo& GetLayoutInfo() const { return m_layout_info; }
    const PreviewInfo& GetPreviewInfo() const { return m_preview_info; }

    const CImage& GetBackgroundL() const { return m_background_l; }
    const CImage& GetBackgroundS() const { return m_background_s; }

    bool IsPNG() const { return m_is_png; }
    void SetAlpha(int alpha);       //设置主窗口的不透明度，alpha:0~255，仅当使用png背景时有效

    //绘制预览图
    //pDC: 绘图的CDC
    //rect: 绘图区域
    void DrawPreview(CDC* pDC, CRect rect);

    //绘制主界面
    void DrawInfo(CDC* pDC, bool show_more_info, CFont& font);

    static string GetDisplayItemXmlNodeName(DisplayItem display_item);

private:
    void LoadFromXml(const wstring& file_path);     //从xml文件读取皮肤数据
    void LoadFromIni(const wstring& file_path);     //从ini文件读取皮肤数据（用于兼容旧版皮肤）

    CSkinFile::Layout LayoutFromXmlNode(tinyxml2::XMLElement* ele);

    struct DrawStr
    {
        CString label;
        CString value;
        CString GetStr() const
        {
            return label + value;
        }
    };

    static void DrawSkinText(IDrawCommon& drawer, DrawStr draw_str, CRect rect, COLORREF color, Alignment align);

    //绘制主界面中除背景图外所有显示项目
    void DrawItemsInfo(IDrawCommon& drawer, Layout& layout, CFont& font);

private:
    SkinInfo m_skin_info;
    LayoutInfo m_layout_info;
    PreviewInfo m_preview_info;
    std::map<std::string, std::string> m_plugin_map;  //插件名称与xml节点名称的映射关系。key是xml节点名称，value是插件ID

    CFont m_font;
    CImage m_background_s;
    CImage m_background_l;
    bool m_is_png{};
    Gdiplus::Image* m_background_png_s{};
    Gdiplus::Image* m_background_png_l{};
    int m_alpha{ 255 };     //不透明度，仅当背景为png时有效
};
