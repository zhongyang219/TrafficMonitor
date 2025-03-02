#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "SpinEdit.h"
#include "TabDlg.h"
#include "MainWndColorDlg.h"
#include "ComboBox2.h"

// CMainWndSettingsDlg 对话框

class CMainWndSettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CMainWndSettingsDlg)

public:
    CMainWndSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CMainWndSettingsDlg();

    //选项设置数据
    MainWndSettingData m_data;

    bool m_text_disable{ false };   //如果为true，则不允许设置“显示文本”，并不允许交换上传和下载的位置

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MAIN_WND_SETTINGS_DIALOG };
#endif


protected:
    //控件变量
    CColorStatic m_color_static;
    CToolTipCtrl m_toolTip;
    CComboBox2 m_unit_combo;
    CButton m_hide_unit_chk;
    CSpinEdit m_font_size_edit;
    CComboBox2 m_double_click_combo;
    CComboBox2 m_memory_display_combo;

protected:
    void DrawStaticColor();
    void IniUnitCombo();
    void EnableControl();

    virtual void SetControlMouseWheelEnable(bool enable) override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    //afx_msg void OnEnChangeUploadEdit();
    //afx_msg void OnEnChangeDownloadEdit();
    //afx_msg void OnEnChangeCpuEdit();
    //afx_msg void OnEnChangeMemoryEdit();
    //afx_msg void OnBnClickedSetDefaultButton();
    afx_msg void OnBnClickedSetFontButton();
    afx_msg void OnBnClickedSwitchUpDownCheck();
    afx_msg void OnBnClickedFullscreenHideCheck();
    afx_msg void OnBnClickedSpeedShortModeCheck2();
    afx_msg void OnCbnSelchangeUnitCombo();
    afx_msg void OnBnClickedHideUnitCheck();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();
    afx_msg void OnBnClickedHidePercentageCheck();
protected:
    afx_msg LRESULT OnStaticClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedSpecifyEachItemColorCheck();
    afx_msg void OnCbnSelchangeDoubleClickCombo();
    afx_msg void OnBnClickedSeparateValueUnitCheck();
    afx_msg void OnBnClickedUnitByteRadio();
    afx_msg void OnBnClickedUnitBitRadio();
    afx_msg void OnBnClickedShowToolTipChk();
    afx_msg void OnBnClickedBrowseButton();
    afx_msg void OnBnClickedDisplayTextSettingButton();
    afx_msg void OnCbnSelchangeMemoryDisplayCombo();
    afx_msg void OnBnClickedAlwaysOnTopCheck();
    afx_msg void OnBnClickedMousePenetrateCheck();
    afx_msg void OnBnClickedLockWindowPosCheck();
    afx_msg void OnBnClickedAlowOutOfBorderCheck();
    afx_msg void OnBnClickedResotreSkinDefaultButton();
    afx_msg void OnEnChangeFontSizeEdit();
};
