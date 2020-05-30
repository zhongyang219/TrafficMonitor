#pragma once
#include "afxwin.h"
#include"StaticEx.h"
#include "PictureStatic.h"
#include "CSkinPreviewView.h"
#include "LinkStatic.h"
#include "BaseDialog.h"

// CSkinDlg 对话框

class CSkinDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSkinDlg();

	vector<wstring> m_skins;		//皮肤文件的路径
	int m_skin_selected;			//选择的皮肤
	CFont* m_pFont;		//预览图的字体

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SKIN_DIALOG };
#endif

	//COLORREF GetTextColor() const { return m_skin_data.text_color; }
	//DispStrings GetDispStrings() const { return m_skin_data.disp_str; }
	const SkinData& GetSkinData() const { return m_skin_data; }
	static void LoadSkinLayout(const wstring& cfg_path, LayoutData& layout_data);

protected:
	virtual CString GetDialogName() const override;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	//控件变量
	CComboBox m_select_box;			//选择框
	CLinkStatic m_skin_course;	//“皮肤制作教程”超链接
	CLinkStatic m_skin_download;	//“更多皮肤下载”超链接
	CSkinPreviewView* m_view;	//预览区视图类
	CStatic m_preview_static;
	CStaticEx m_notify_static;	//显示提示信息的static控件

	SkinData m_skin_data;		//皮肤数据
	CImage m_background_s;		//小预览图背景图
	CImage m_background_l;		//大预览图背景图

	void ShowPreview();		//显示皮肤预览
	void LoadBackImage(const wstring& path, bool small_image);		//载入背景图，path为皮肤所在路径；small_image指定载入小背景图还是大背景图
	CRect CalculateViewRect();		//根据窗口大小计算预览视图的大小

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
