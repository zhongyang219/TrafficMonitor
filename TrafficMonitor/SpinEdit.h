#pragma once
// 这是一个带微调按钮的Edit控件，在CEdit右侧附加一个微调按钮，请预留空间
// CSpinEdit

//微调按钮点击消息。WPARAM: CSpinButtonCtrl的指针，LPARAM: LPNMUPDOWN
#define WM_SPIN_EDIT_POS_CHANGED (WM_USER + 1010)

class CSpinEdit : public CEdit
{
    DECLARE_DYNAMIC(CSpinEdit)

public:
    CSpinEdit();
    virtual ~CSpinEdit();

    void SetRange(short lower, short upper, short step = 1);        // 设置文本框中的数值范围
    void SetValue(int value);                       //设置文本框中的数值
    int GetValue();                                 //获取文本框中的数值
    void SetMouseWheelEnable(bool enable);          //设置是否允许响应鼠标滚轮

protected:
    CSpinButtonCtrl m_spin;                         //微调按钮控件
    int m_step{ 1 };
    int m_spin_width{};
    bool m_mouse_wheel_enable{ true };

    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


