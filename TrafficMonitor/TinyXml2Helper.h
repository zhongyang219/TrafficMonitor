#pragma once
#include "tinyxml2/tinyxml2.h"
#include <string>
#include <functional>

class CTinyXml2Helper
{
public:

    //从文件读取XML内容
    static bool LoadXmlFile(tinyxml2::XMLDocument& doc, const wchar_t* file_path);

    //遍历一个XML节点
    //fun: 一个函数对象，遍历到一个节点时被调用
    static void IterateChildNode(tinyxml2::XMLElement* ele, std::function<void(tinyxml2::XMLElement*)> fun);

    //获取一个节点的属性（返回值不会为空指针，如果找不到则返回空字符串）
    static const char* ElementAttribute(tinyxml2::XMLElement* ele, const char* attr);

    //获取一个节点的名称（返回值不会为空指针，如果找不到则返回空字符串）
    static const char* ElementName(tinyxml2::XMLElement* ele);

    //获取一个节点的文本（返回值不会为空指针，如果找不到则返回空字符串）
    static const char* ElementText(tinyxml2::XMLElement* ele);

    static bool StringToBool(const char* str);
};
