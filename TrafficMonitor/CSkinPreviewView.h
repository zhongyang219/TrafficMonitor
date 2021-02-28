#pragma once
#include "DrawCommon.h"
#include "SkinFile.h"


// CSkinPreviewView 视图


class CSkinPreviewView : public CScrollView
{
	DECLARE_DYNCREATE(CSkinPreviewView)

protected:
	CSkinPreviewView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSkinPreviewView();

public:
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//#ifndef _WIN32_WCE
//	virtual void Dump(CDumpContext& dc) const;
//#endif
//#endif

//成员函数
public:
	void InitialUpdate();
	void SetSize(int width, int hight);
	void SetSkinData(CSkinFile* skin_data) { m_skin_data = skin_data; }

	//成员变量
protected:
	CSize m_size;
	CPoint m_start_point;			//绘图的起始位置

    CSkinFile* m_skin_data;

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
};


