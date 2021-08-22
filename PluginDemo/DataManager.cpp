#include "pch.h"
#include "DataManager.h"

CDataManager::CDataManager()
{
}

CDataManager& CDataManager::Instance()
{
    static CDataManager instance;
    return instance;
}
