#pragma once
#include "afxwin.h"
#include"StaticEx.h"
#include "PictureStatic.h"

// CSkinDlg �Ի���

class CSkinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSkinDlg();

	vector<wstring> m_skins;		//Ƥ���ļ���·��
	int m_skin_selected;			//ѡ���Ƥ��
	//int m_skin_width;		//Ԥ��ͼ���
	//int m_skin_height_s;	//Ԥ��ͼ�߶ȣ�С��
	//int m_skin_height_l;	//Ԥ��ͼ�߶ȣ���
	CFont* m_pFont;		//Ԥ��ͼ������

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SKIN_DIALOG };
#endif

	COLORREF GetTextColor() const { return m_text_color; }
	DispStrings GetDispStrings() const { return m_disp_str; }

	static void GetSkinLayout(const wstring& cfg_path, LayoutData& layout_data);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	CComboBox m_select_box;			//ѡ���
	CPictureStatic m_skin_preview_s;		//��ʾ��Ԥ��ͼ�Ŀؼ�
	CPictureStatic m_skin_preview_l;		//��ʾСԤ��ͼ�Ŀؼ�
	HBITMAP m_image;				//ͼƬ���

	COLORREF m_text_color;			//��ǰƤ�����ı���ɫ
	//wstring m_skin_author;			//Ƥ������
	DispStrings m_disp_str;		//��ʾ���ı�

	LayoutData m_layout_data;
#define PREVIEW_START_X theApp.DPI(38)	//Ԥ��ͼ����ʼ����
#define PREVIEW_START_Y theApp.DPI(83)

	void ShowPreview();		//��ʾƤ��Ԥ��
	void SetPreviewText(CDC* pDC, bool l_preview);	//����Ԥ�����֡����l_previewΪtrue������ƴ�Ԥ��ͼ�ؼ�

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnControlRepaint(WPARAM wParam, LPARAM lParam);
};
