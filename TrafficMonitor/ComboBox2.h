#pragma once


// CComboBox2

class CComboBox2 : public CComboBox
{
	DECLARE_DYNAMIC(CComboBox2)

public:
	CComboBox2();
	virtual ~CComboBox2();
    void SetMouseWheelEnable(bool enable);  //设置是否允许响应鼠标滚轮

private:
    bool m_mouse_wheel_enable{ true };

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};


