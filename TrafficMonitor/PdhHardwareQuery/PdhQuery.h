#pragma once
#include <Pdh.h>
#include <PdhMsg.h>

class CPdhQuery
{
public:
    CPdhQuery(LPCTSTR _fullCounterPath);
    virtual ~CPdhQuery();

protected:
    bool Initialize();
    bool QueryValue(double& value);
    struct ValueItem
    {
        std::wstring name;
        double value{};
    };
    bool QueryValues(std::vector<ValueItem>& values);

protected:
    HQUERY query = nullptr;
    HCOUNTER counter = nullptr;
    bool isInitialized = false;
    CString fullCounterPath;
};
