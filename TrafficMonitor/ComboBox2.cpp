// ComboBox2.cpp: 实现文件
//

#include "stdafx.h"
#include "ComboBox2.h"


// CComboBox2

IMPLEMENT_DYNAMIC(CComboBox2, CComboBox)

CComboBox2::CComboBox2()
{

}

CComboBox2::~CComboBox2()
{
}


void CComboBox2::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}

BEGIN_MESSAGE_MAP(CComboBox2, CComboBox)
END_MESSAGE_MAP()



// CComboBox2 消息处理程序




BOOL CComboBox2::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    //如果m_mouse_wheel_enable为false，则不响应鼠标滚轮消息
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //将鼠标滚轮消息发送给父窗口
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CComboBox::PreTranslateMessage(pMsg);
}
