#pragma once
#include <string>

class CDataManager
{
private:
    CDataManager();

public:
    static CDataManager& Instance();

public:
    std::wstring m_cur_time;
    std::wstring m_cur_date;

};
