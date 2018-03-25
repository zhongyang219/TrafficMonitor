#pragma once
#include "afxwin.h"
#include"StaticEx.h"
#include "PictureStatic.h"

// CSkinDlg 对话框

class CSkinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSkinDlg();

	vector<wstring> m_skins;		//皮肤文件的路径
	int m_skin_selected;			//选择的皮肤
	//int m_skin_width;		//预览图宽度
	//int m_skin_height_s;	//预览图高度（小）
	//int m_skin_height_l;	//预览图高度（大）
	CFont* m_pFont;		//预览图的字体

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SKIN_DIALOG };
#endif

	COLORREF GetTextColor() const { return m_text_color; }
	DispStrings GetDispStrings() const { return m_disp_str; }

	static void GetSkinLayout(const wstring& cfg_path, LayoutData& layout_data);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CComboBox m_select_box;			//选择框
	CPictureStatic m_skin_preview_s;		//显示大预览图的控件
	CPictureStatic m_skin_preview_l;		//显示小预览图的控件
	HBITMAP m_image;				//图片句柄
	CStaticEx m_skin_course{ true };	//“皮肤制作教程”超链接
	CStaticEx m_skin_download{ true };

	COLORREF m_text_color;			//当前皮肤的文本颜色
	//wstring m_skin_author;			//皮肤作者
	DispStrings m_disp_str;		//显示的文本

	LayoutData m_layout_data;
#define PREVIEW_START_X theApp.DPI(38)	//预览图的起始坐标
#define PREVIEW_START_Y theApp.DPI(83)

	void ShowPreview();		//显示皮肤预览
	void SetPreviewText(CDC* pDC, bool l_preview);	//设置预览文字。如果l_preview为true，则绘制大预览图控件

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnControlRepaint(WPARAM wParam, LPARAM lParam);
};
