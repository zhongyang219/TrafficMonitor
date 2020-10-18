#pragma once

namespace CRASHREPORT
{
    /**@brief
        开启dmup模式
        开启后，当程序崩溃时，会生成一个dump文件，该文件在C:\Users\<用户名>\AppData\Local\Temp目录
        文件名格式：崩溃时间_程序名称.exe.dmp
        如下
        20150116174802_CrashShare.exe.dmp
    */
    void StartCrashReport();
}