#pragma once
class CTest
{
public:
    CTest();
    ~CTest();

    static void Test();
    static void TestCommand();

private:
    static void TestHttpQequest();
    static void TestGetLicense();
    static void TestSkin();
    static void TestCrash();
    static void TestPlugin();
    static void TestDate();
    static void TestIni();
};
