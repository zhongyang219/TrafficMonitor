#pragma once
class CDrawCommonEx
{
public:
	CDrawCommonEx(CDC* pDC);
	~CDrawCommonEx();

	void DrawWindowText(CRect rect, LPCTSTR str, COLORREF color);
	void SetFont(CFont* pFont);

private:
	HTHEME hThm;
	CFont* m_pFont{};
	CDC* m_pDC{};
};

