#pragma once
class CTest
{
public:
    CTest();
    ~CTest();

    static void Test();

private:
    static void TestHttpQequest();
    static void TestGetLicense();
    static void TestSkin();
    static void TestCrash();
};
