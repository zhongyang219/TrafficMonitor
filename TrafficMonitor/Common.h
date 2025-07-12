#pragma once
#include "CommonData.h"
#include "CVariant.h"
#include <set>
#include <functional>

class CCommon
{
public:
    CCommon();
    ~CCommon();
    //将const char*字符串转换成宽字符字符串
    static wstring StrToUnicode(const char* str, bool utf8 = false);

    static string UnicodeToStr(const wchar_t* wstr, bool utf8 = false);

    static wstring AsciiToUnicode(const string& str);
    static string AsciiToStr(const std::wstring& wstr);

    static void StringNormalize(std::string& str);
    static void StringNormalize(std::wstring& str);

    //将一个字符串分割成若干个字符（模板类型只能为string或wstring）
    //str: 原始字符串
    //div_ch: 用于分割的字符
    //result: 接收分割后的结果
    static void StringSplit(const std::string& str, char div_ch, vector<std::string>& results, bool skip_empty = true, bool trim = true);
    static void StringSplit(const std::wstring& str, wchar_t div_ch, vector<std::wstring>& results, bool skip_empty = true, bool trim = true);

    static void StringSplit(const std::string& str, const std::string& div_str, vector<std::string>& results, bool skip_empty = true, bool trim = true);
    static void StringSplit(const std::wstring& str, const std::wstring& div_str, vector<std::wstring>& results, bool skip_empty = true, bool trim = true);

    static bool StringTransform(std::string& str, bool upper);
    static bool StringTransform(std::wstring& str, bool upper);

    //读取文件内容
    static bool GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary = true);

    //读取文件内容
    //file_path: 文件的路径
    //length: 文件的长度
    //binary: 是否以进制方式读取
    //返回值: 读取成功返回读取到的文件内容的const char类型的指针，在使用完毕后这个指针需要自行使用delete释放。读取失败返回nullptr
    static const char* GetFileContent(const wchar_t* file_path, size_t& length, bool binary = true);

    /*根据数据的大小转换成以KB、MB、GB为单位的字符串
    size：数据的字节数
    返回值：转换后的字符串
    */
    static CString DataSizeToString(unsigned long long size, const PublicSettingData& cfg);

    /*根据数据的大小转换成以KB、MB、GB为单位的字符串
    size：数据的字节数
    with_space：数值和单位是否使用空格分隔
    返回值：转换后的字符串
    */
    static CString DataSizeToString(unsigned long long size, bool with_space = true);

    //将温度信息转换成字符串
    static CString TemperatureToString(float temperature, const PublicSettingData& cfg);

    //将使用率转换成字符串
    static CString UsageToString(int usage, const PublicSettingData& cfg);
    static CString FreqToString(float usage, const PublicSettingData& cfg);
    //static CString KBytesToString(unsigned int kb_size);
    static CString KBytesToString(unsigned __int64 kb_size);

    //返回两个FILETIME结构的时间差
    static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

    //将一个日志信息str_text写入到file_path文件中
    static void WriteLog(const char* str_text, LPCTSTR file_path);
    static void WriteLog(const wchar_t* str_text, LPCTSTR file_path);

    /*
    函数功能：对指定文件在指定的目录下创建其快捷方式
    函数参数：
    lpszLnkFileDir  指定目录，不能为NULL。
    lpszFileName    指定文件，为NULL表示当前进程的EXE文件。
    lpszLnkFileName 快捷方式名称，为NULL表示EXE文件名。
    wHotkey         为0表示不设置快捷键
    pszDescription  备注
    iShowCmd        运行方式，默认为常规窗口
    */
    static BOOL CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);

    //获取开始菜单“所有程序”中的“启动”目录的路径
    static wstring GetStartUpPath();

    //获取path路径下的文件或文件夹，并将文件或文件夹名称保存在files容器中。
    static void GetFiles(const wchar_t* path, vector<wstring>& files);

    //获取path路径下的文件或文件夹，每次遍历时调用函数对象func
    //path: 查找的路径
    //func: 可以是一个函数对象或lambda表达式，参数是遍历到的文件或文件夹名
    static void GetFiles(const wchar_t* path, std::function<void(const wstring&)> func);

    //判断一个文件是否存在
    static bool FileExist(LPCTSTR file_name);

    //判断是否是文件夹
    static bool IsFolder(const wstring& path);

    static bool MoveAFile(LPCTSTR exist_file, LPCTSTR new_file);

    //计算两个SYSTEMTIME结构时间的差（a-b，只保留时、分、秒）
    static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

    //获取从1970年1月1日到现在的毫秒数
    static ULONGLONG GetCurrentTimeSinceEpochMilliseconds();

    //获取当前程序的目录
    static wstring GetModuleDir();

    //获取system32文件夹的路径
    static wstring GetSystemDir();

    //获取临时文件夹的路径
    static wstring GetTemplateDir();

    //获取Appdata/Local/TrafficMonitor的目录，如果不存在，则会自动创建
    static wstring GetAppDataConfigDir();

    //在指定位置绘制文本
    static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

    ////设置绘图的剪辑区域
    //static void SetDrawArea(CDC* pDC, CRect rect);

    /**
     * 判断当前正在与用户交互的前台窗口是否是全屏的
     * @param[in] hMonitor 要判断的显示器（如果为空，则指定为主显示器）
     * @return 
     */
    static bool IsForegroundFullscreen(HMONITOR hMonitor = NULL);

    //将一个字符串保存到剪贴板
    static bool CopyStringToClipboard(const wstring& str);

    static wstring GetJsonValueSimple(const wstring& json_str, const wstring& name);

    //获取URL的内容
    static bool GetURL(const wstring& url, std::string& result, const wstring& user_agent = wstring());
    static bool GetURL(const wstring& url, wstring& result, bool utf8 = false, const wstring& user_agent = wstring());

    //获取外网IP地址和IP归属地
    static void GetInternetIp(wstring& ip_address, wstring& ip_location, bool global);

    static void GetInternetIp2(wstring& ip_address, wstring& ip_location, bool ipv6);

    static void SetRect(CRect& rect, int x, int y, int width, int height);

    //从资源文件载入字符串。其中，front_str、back_str为载入字符串时需要在前面或后面添加的字符串
    static CString LoadText(const wchar_t* id, LPCTSTR back_str = nullptr);
    static CString LoadText(LPCTSTR front_str, const wchar_t* id, LPCTSTR back_str);

    //安全的格式化字符串，将format_str中形如<%序号%>的字符串替换成初始化列表paras中的元素，元素支持int/double/LPCTSTR/CString格式，序号从1开始
    static CString StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras);

    //从资源文件中载入字符串，并将资源字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    static CString LoadTextFormat(const wchar_t* id, const std::initializer_list<CVariant>& paras);

    //将int类型转换成字符串
    //n：要转换的数值
    //thousand_separation：是否要每隔3位数使用逗号分隔
    //is_unsigned：数值是否是无符号的
    static CString IntToString(__int64 n, bool thousand_separation = false, bool is_unsigned = false);

    //删除字体名称后面的Bold、Light等字符串，并根据这些字符串设置字体粗细
    static void NormalizeFont(LOGFONT& font);

    //安全的字符串复制函数
    static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);

    //字符串替换
    static bool StringReplace(wstring& str, const wstring& str_old, const wstring& str_new);

    /// <summary>
    /// 字符串相似度算法-编辑距离法
    /// </summary>
    /// <returns>返回的值为0~1，越大相似度越高</returns>
    static double StringSimilarDegree_LD(const std::string& srcString, const std::string& matchString);
    static double StringSimilarDegree_LD(const std::wstring& srcString, const std::wstring& matchString);
    
    //设置线程语言
    static void SetThreadLanguage(WORD language);

    //设置颜色模式
    static void SetColorMode(ColorMode mode);

    //经过测试发现，似乎当透明色的R和B值相等时，会出现右击任务栏窗口时无法弹出右键菜单，而是弹出系统任务栏右键菜单的问题
    //为了解决这个问题，需要将颜色值进行转换
    //此函数的作用是判断一个颜色的R和B值是否相等，如果是则将颜色的B值加1（如果B==255，则减1）
    static void TransparentColorConvert(COLORREF& transparent_color);

    static void SetDialogFont(CWnd* pDlg, CFont* pFont);

    //从资源加载自定义文本资源。id：资源的ID，code_type：文本的编码格式：0:ANSI, 1:UTF8, 2:UTF16
    static CString GetTextResource(UINT id, int code_type);

    //从资源文件读取上次编译时间
    static CString GetLastCompileTime();

    //从资源加载一个图标
    static HICON LoadIconResource(UINT id, int size);

    //获取一个菜单项的序号
    static int GetMenuItemPosition(CMenu* pMenu, UINT id);

    //从资源文件加载一个菜单，并处理文本翻译
    static void LoadMenuResource(CMenu& menu, UINT res_id);

    static bool IsColorSimilar(COLORREF color1, COLORREF color2);

    //计算二进制中1的个数
    static int CountOneBits(unsigned int value);

    //设置一个数字的某个bit位
    static void SetNumberBit(unsigned int& num, int bit, bool value);

    //获取一个数字的某个bit位
    static bool GetNumberBit(unsigned int num, int bit);

    //在不排序的情况下删除vector中的重复元素
    template<class T>
    static void RemoveVectorDuplicateItem(vector<T>& vec)
    {
        std::set<T> si;
        for (auto it = vec.begin(); it != vec.end();)
        {
            if (si.count(*it) == 0)//这里判断当前元素是否已经出现过，若没有出现过则将该元素保留，并做标记
            {
                si.insert(*it);
                ++it;
            }
            else//如果前面已经出现过，则删除当前元素
            {
                it = vec.erase(it);//这里删除it指向的元素
            }
        }
    }

    /**
     * @brief   限制一个数的范围
     * @param   value 要限制范围的值
     * @param   min_value 最小值
     * @param   max_value 最大值
     */
    template<class T>
    static void ValidatValue(T& value, const T& min_value, const T& max_value)
    {
        if (value < min_value)
            value = min_value;
        if (value > max_value)
            value = max_value;
    }

    //获取Windows主题颜色
    static COLORREF GetWindowsThemeColor();

    static CString GetErrorMessage(DWORD error_code);
};

/**
 * @brief 编译期获取数组长度
 *
 * @tparam T 数组元素类型
 * @tparam N 编译期推断的数组长度
 * @return constexpr std::size_t 编译期推断的数组长度
 */
template <typename T, std::size_t N>
constexpr std::size_t GetArrayLength(const T (&)[N]) noexcept
{
    return N;
}

/**
 * @brief 析构StaticVariableWrapper包装对象前默认执行的函数，实际上无操作
 *
 * @tparam T
 */
template <class T>
class CDefaultStaticVariableWrapperDtor
{
public:
    void operator()(T*){};
};
/**
 * @brief 设计上用于静态变量包装类，用于自定义变量默认初始化后行为和析构前行为
 *
 * @tparam T 要被包装的类型
 * @tparam DTOR 自定义执行析构函数前的行为
 */
template <class T, class DTOR = CDefaultStaticVariableWrapperDtor<T>>
class CStaticVariableWrapper : private DTOR
{
private:
    T m_content;

public:
    /**
     * @brief 构造一个StaticVariableWrapper
     *
     * @tparam CTOR 自定义变量默认初始化后的函数类型
     * @param ctor 自定义变量默认初始化后的行为，传入变量的指针作为参数
     * @param dtor 自定义变量执行析构函数前的行为，传入变量的指针作为参数
     */
    template <class CTOR>
    CStaticVariableWrapper(CTOR ctor, DTOR dtor = {})
        : DTOR{dtor}
    {
        ctor(std::addressof(m_content));
    }
    ~CStaticVariableWrapper()
    {
        (*static_cast<DTOR*>(this))(std::addressof(m_content));
    }
    T& Get() noexcept
    {
        return m_content;
    }
    const T& Get() const noexcept
    {
        return m_content;
    }
};
/**
 * @brief 生成静态变量包装类的函数
 *
 * @tparam T 要被包装的类型
 * @tparam CTOR 自定义变量默认初始化后的函数类型
 * @tparam DTOR 自定义变量执行析构函数前的函数类型
 * @param ctor 自定义变量默认初始化后的行为，传入变量的指针作为参数
 * @param dtor 自定义变量执行析构函数前的行为，传入变量的指针作为参数
 * @return CStaticVariableWrapper<T, DTOR> 包装后的变量，已经初始化
 */
template <class T, class CTOR, class DTOR = CDefaultStaticVariableWrapperDtor<T>>
auto MakeStaticVariableWrapper(CTOR ctor, DTOR dtor = {})
    -> CStaticVariableWrapper<T, DTOR>
{
    return {ctor, dtor};
}

/**
 * @brief 调用指针指向的对象的对应类型的析构函数
 *
 * @tparam T 传入的移除了指针后的类型
 * @param p_memory 指向要执行析构函数的对象的指针
 */
template <class T>
void Destroy(T* p_memory)
{
    p_memory->~T();
}

template <class T, class... Args>
void EmplaceAt(T* p_memory, Args&&... args)
{
    ::new (p_memory) T(std::forward<Args>(args)...);
}
