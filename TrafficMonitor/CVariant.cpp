#include "stdafx.h"
#include "CVariant.h"


CVariant::CVariant(int value)
{
	m_value_int = value;
	m_type = eType::INT;
}

CVariant::CVariant(size_t value)
{
	m_value_int = static_cast<int>(value);
	m_type = eType::UINT;
}

CVariant::CVariant(double value)
{
	m_value_double = value;
	m_type = eType::DOUBLE;
}

CVariant::CVariant(LPCTSTR value)
{
	m_value_string = value;
	m_type = eType::STRING;
}

CVariant::CVariant(const CString& value)
{
	m_value_string = value;
	m_type = eType::STRING;
}

CVariant::CVariant(const wstring & value)
{
	m_value_string = value.c_str();
	m_type = eType::STRING;
}

CVariant::~CVariant()
{
}

CString CVariant::ToString() const
{
	CString str;
	switch (m_type)
	{
	case CVariant::eType::INT:
		str.Format(_T("%d"), m_value_int);
		break;
	case eType::UINT:
		str.Format(_T("%u"), static_cast<unsigned int>(m_value_int));
		break;
	case CVariant::eType::DOUBLE:
		str.Format(_T("%g"), m_value_double);
		break;
	case CVariant::eType::STRING:
		str = m_value_string;
		break;
	default:
		break;
	}
	return str;
}
