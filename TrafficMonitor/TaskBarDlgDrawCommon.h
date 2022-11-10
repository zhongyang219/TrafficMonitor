#pragma once
#include <tuple>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>
#include <type_traits>
#include "IDrawCommon.h"
#include "Common.h"
#include "D3D10Support1.h"
#include "D2D1Support.h"
#include "Image2DEffect.h"
#include "Nullable.hpp"
#include "DrawTextManager.h"

void LogWin32ApiErrorMessage() noexcept;

namespace DrawCommonHelper
{
    void LogDeviceRecreateReason(HRESULT hr);

    constexpr std::uint32_t MAX_D2D1_RENDER_ERROR_COUNT = 77;
    class DefaultD2DDrawCommonExceptionHandler
    {
    private:
        CHResultException& m_ref_ex;
        std::function<void()> m_device_recreate_handler;
        static std::uint32_t m_error_count;

    public:
        DefaultD2DDrawCommonExceptionHandler(CHResultException& ex, std::function<void()> device_recreate_handler) noexcept;
        DefaultD2DDrawCommonExceptionHandler(const DefaultD2DDrawCommonExceptionHandler&) = delete;
        DefaultD2DDrawCommonExceptionHandler& operator=(const DefaultD2DDrawCommonExceptionHandler&) = delete;
        DefaultD2DDrawCommonExceptionHandler* operator&() = delete;

        static void ResetErrorCount();
        void operator()();
    };
    HDC Get1x1AlphaEqual1DC();
}

class CTaskBarDlgDrawCommonSupport
{
private:
    CD3D10Device1 m_d3d10_device1{};
    CD2D1Device m_d2d1_device{};
    Microsoft::WRL::ComPtr<ID2D1StrokeStyle> m_p_ps_dot_like_style{};

public:
    CTaskBarDlgDrawCommonSupport();
    ~CTaskBarDlgDrawCommonSupport() = default;

    auto GetD3D10Device1() noexcept
        -> CD3D10Device1&;
    auto GetD2D1Device() noexcept
        -> CD2D1Device&;
    auto GetPsDotLikeStyle() noexcept
        -> Microsoft::WRL::ComPtr<ID2D1StrokeStyle>;
    void RecreateAll();
    void RecreateD2D1Device();
    static bool CheckSupport() noexcept;
};

template <class T>
bool IsBitwiseEquality(T& lhs, T& rhs) noexcept
{
    auto p_lhs = std::addressof(lhs);
    auto p_rhs = std::addressof(rhs);
    constexpr auto size = sizeof(T);

    return ::memcmp(p_lhs, p_rhs, size) == 0;
}

// 0.0039215686 = 1 / 255
#define TRAFFICMONITOR_ONE_IN_255_NUM 0.0039215686
// 0.0039215686 + 0.0000784314
#define TRAFFICMONITOR_ONE_IN_255_NUM_CEIL 0.004
#define TRAFFICMONITOR_ONE_IN_255_NUM_FLOOR 0.0038431372
#define TRAFFICMONITOR_ONE_IN_255_CEIL TRAFFICMONITOR_STR(TRAFFICMONITOR_ONE_IN_255_NUM_CEIL)
#define TRAFFICMONITOR_ONE_IN_255_FLOOR TRAFFICMONITOR_STR(TRAFFICMONITOR_ONE_IN_255_NUM_FLOOR)

/**
 * @brief 每次循环都会在最开始时调用Resize()。
 * 此类在调用Resize前是部分初始化状态，此时调用其中的成员会发生未定义行为
 *
 */
class CTaskBarDlgDrawCommonWindowSupport
{
private:
    class D3DPart final : public CDeviceResource<CD3D10Device1>
    {
        friend CTaskBarDlgDrawCommonWindowSupport;

        using Base = CDeviceResource<CD3D10Device1>;
        using Base::DeviceType;

    private:
        DeviceType m_p_device1;
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_render_target_surface{};

        Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_gdi_initial_texture{};
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_initial_surface{};
        Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_gdi_final_texture{};
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_final_surface{};
        CImage2DEffect m_alpha_value_effect;
        bool m_is_p_device1_recreated{false};

        static auto CreateGdiCompatibleTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;
        static auto CreateDefaultTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;
        static auto CreateCpuWriteableTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;
        static auto GetPsAlphaValueReducer() noexcept
            -> Microsoft::WRL::ComPtr<ID3D10Blob>;

    public:
        D3DPart(CD3D10Device1& ref_device);
        ~D3DPart() = default;

        void OnDeviceRecreate(DeviceType p_new_device) noexcept override;

        bool HandleDeviceRecreationIfNecessary();
        void ResizeRenderTarget(D2D1_SIZE_U size);
        void ResizeGdiInteropPart(D2D1_SIZE_U size);
        auto GetRenderTargetSurface()
            -> Microsoft::WRL::ComPtr<IDXGISurface>;
    };
    class D2DPart final : public CDeviceResource<CD2D1Device>
    {
        friend class D3DPart;
        friend CTaskBarDlgDrawCommonWindowSupport;

        using Base = CDeviceResource<CD2D1Device>;
        using Base::DeviceType;

    private:
        /**
         * @brief 即ID2D1RenderTarget
         *
         */
        DeviceType m_p_device;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_p_device_context{};
        Microsoft::WRL::ComPtr<ID2D1Bitmap> m_p_d2d_render_target_bitmap{};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_p_foreground_color_brush{};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_p_background_color_brush{};
        std::weak_ptr<class CD2D1BitmapCache> m_wp_bitmap_cache{};
        D2D1_COLOR_F m_foreground_color{D2D1::ColorF::Black};
        D2D1_COLOR_F m_background_color{D2D1::ColorF::Black};
        bool m_is_p_device_recreated{false};

        void Reinitialize();

    public:
        D2DPart(CD2D1Device& ref_device);
        ~D2DPart() = default;

        void OnDeviceRecreate(DeviceType p_new_device) noexcept override;

        bool HandleDeviceRecreationIfNecessary();
        void RebindRenderTargetSurface(Microsoft::WRL::ComPtr<IDXGISurface> p_surface);
        void SetForeColor(COLORREF color, BYTE alpha);
        void SetBackColor(COLORREF color, BYTE alpha);
    };

    class DWriteHelper
    {
    private:
        Microsoft::WRL::ComPtr<IDWriteTextFormat> m_p_text_format{};
        HFONT m_h_last_font{};
        LOGFONTW m_last_font{};

    public:
        DWriteHelper() = default;
        ~DWriteHelper() = default;

        void SetFont(HFONT h_font);
        auto GetFont() noexcept
            -> HFONT;
        auto GetTextFormat() noexcept
            -> Microsoft::WRL::ComPtr<IDWriteTextFormat>;
    };

    D2D1_SIZE_U m_size{0, 0};
    D3DPart m_d3d_part;
    D2DPart m_d2d_part;
    DWriteHelper m_dwrite_helper{};
    CTaskBarDlgDrawCommonSupport& m_ref_taskbardlg_draw_common_support;
    bool m_is_size_updated{true};

public:
    constexpr static float DEFAULT_DPI = 96.f;
    constexpr static auto PIXEL_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;

    explicit CTaskBarDlgDrawCommonWindowSupport(CTaskBarDlgDrawCommonSupport& ref_taskbdlg_draw_common_support);
    ~CTaskBarDlgDrawCommonWindowSupport() = default;

    void Resize(const D2D1_SIZE_U size);
    void ResizeGdiInteropPart();
    void SetFont(HFONT h_font);
    void SetBackColor(COLORREF color, BYTE alpha) noexcept;
    auto GetRawBackSolidColorBruch() noexcept
        -> ID2D1SolidColorBrush*;
    auto SetForeColor(COLORREF color, BYTE alpha) noexcept
        -> CTaskBarDlgDrawCommonWindowSupport*;
    auto GetRawForeSolidColorBruch() noexcept
        -> ID2D1SolidColorBrush*;
    auto GetRenderTarget() noexcept
        -> Microsoft::WRL::ComPtr<ID2D1DeviceContext>;
    /**
     * @brief 从内存复制图片到显存中并作为D2D和GDI互操作时使用的临时缓存，并默认格式为BGRA
     *
     * @param p_data 内存图片的地址
     * @param size 内存图片的大小
     */
    void SetGdiInteropTexture(void* p_data, D2D1_SIZE_U size);
    auto GetTextFormat() noexcept
        -> Microsoft::WRL::ComPtr<IDWriteTextFormat>;
    auto GetRawPsDotLikeStyle() noexcept
        -> ID2D1StrokeStyle*;
    auto GetGdiInitalTextureSurface() noexcept
        -> Microsoft::WRL::ComPtr<IDXGISurface1>;
    auto GetGdiFinalTextureSurface() noexcept
        -> Microsoft::WRL::ComPtr<IDXGISurface1>;
    auto DrawAlphaValueReduceEffect() noexcept
        -> CD3D10DrawCallWaiter;
    auto GetSize() const noexcept
        -> D2D1_SIZE_U;
    auto GetFont() noexcept
        -> HFONT;
    auto GetRenderTargetSurface() noexcept
        -> Microsoft::WRL::ComPtr<IDXGISurface1>;
    void RebindD2D1BitmapCache(std::weak_ptr<class CD2D1BitmapCache> up_cache);
    auto GetCachedBitmap(HBITMAP hbitmap) const
        -> Microsoft::WRL::ComPtr<ID2D1Bitmap>;
    void RequestD3D10Device1Recreate();
    void RequestD2D1DeviceRecreate();
};

namespace DrawCommonHelper
{
    void DefaultD3D10Exception1Handler(CD3D10Exception1& ex, std::function<void()> device_recreate_handler);
}
namespace TaskBarDlgUser32DrawTextHook
{
    class EnableAllReplaceFunctionGuard;
    namespace Details
    {
        auto GetArgb32BitmapInfo(CRect rect) noexcept
            -> ::BITMAPINFO;
        auto GetArgb32BitmapInfo(LONG width, LONG height) noexcept
            -> ::BITMAPINFO;

        struct DrawTextReplacedFunctionState
        {
            HDC m_on_draw_text_call_matched_hdc{NULL};
        };

        template <class DrawTextFunctionNamespace>
        class CDrawTextReplacedFunction
        {
            friend auto EnableAllReplaceFunction(Details::DrawTextReplacedFunctionState& ref_state) noexcept
                -> ::TaskBarDlgUser32DrawTextHook::EnableAllReplaceFunctionGuard;

        private:
            using ReplacedFunction = typename DrawTextFunctionNamespace::ReplacedFunction;

            ReplacedFunction m_p_replaced_function;

        public:
            mutable DrawTextReplacedFunctionState m_state{};

            CDrawTextReplacedFunction(ReplacedFunction replaced_function)
                : m_p_replaced_function{replaced_function}
            {
            }

            void Enable(const DrawTextReplacedFunctionState& state) const
            {
                m_state = state;

                auto draw_text_function_manager_state =
                    typename DrawTextFunctionNamespace::State{true, m_p_replaced_function};
                DrawTextFunctionNamespace::SetState(draw_text_function_manager_state);
            }
            void Disable() const
            {
                DrawTextFunctionNamespace::SetEnable(false);
            }
            void Reset() const
            {
                Destroy(&m_state);
                EmplaceAt(&m_state);
            }
        };

        template <std::size_t N, std::size_t... Indexs>
        constexpr auto AddNToTheSequence(std::index_sequence<Indexs...>)
            -> std::index_sequence<(Indexs + N)...>
        {
            return {};
        }

#define TRAFFICMONITOR_MAKE_TAG_TYPE(tag_name) \
    struct tag_name                            \
    {                                          \
    };

        TRAFFICMONITOR_MAKE_TAG_TYPE(WhenTupleAtFirst)
        TRAFFICMONITOR_MAKE_TAG_TYPE(WhenTupleAtLast)
        TRAFFICMONITOR_MAKE_TAG_TYPE(WhenTupleAtMiddle)

        template <class NthTuple, class Func, class Tuple,
                  std::size_t... IndexN, std::size_t... IndexAfterN>
        constexpr auto ReplaceNthConsecutiveArgumentAndApplyImpl(
            NthTuple&& nth_tuple, Func&& func, Tuple&& tuple, WhenTupleAtFirst,
            std::index_sequence<IndexN...>, std::index_sequence<IndexAfterN...>)
        {
            return func(std::get<IndexN>(nth_tuple)..., std::get<IndexAfterN>(tuple)...);
        }

        template <class NthTuple, class Func, class Tuple,
                  std::size_t... IndexBeforeN, std::size_t... IndexN>
        constexpr auto ReplaceNthConsecutiveArgumentAndApplyImpl(
            NthTuple&& nth_tuple, Func&& func, Tuple&& tuple, WhenTupleAtLast,
            std::index_sequence<IndexBeforeN...>, std::index_sequence<IndexN...>)
        {
            return func(std::get<IndexBeforeN>(tuple)..., std::get<IndexN>(nth_tuple)...);
        }

        template <class NthTuple, class Func, class Tuple,
                  std::size_t... IndexBeforeN, std::size_t... IndexN, std::size_t... IndexAfterN>
        constexpr auto ReplaceNthConsecutiveArgumentAndApplyImpl(
            NthTuple&& nth_tuple, Func&& func, Tuple&& tuple, WhenTupleAtMiddle,
            std::index_sequence<IndexBeforeN...>, std::index_sequence<IndexN...>, std::index_sequence<IndexAfterN...>)
        {
            return func(std::get<IndexBeforeN>(tuple)..., std::get<IndexN>(nth_tuple)..., std::get<IndexAfterN>(tuple)...);
        }

        template <std::int64_t N, std::int64_t delta_size, class Checker = void>
        struct ReplaceNthConsecutiveArgumentAndApplyDispatcher;
        template <std::int64_t delta_size>
        struct ReplaceNthConsecutiveArgumentAndApplyDispatcher<0, delta_size, std::enable_if_t<true>>
        {
            template <class NthTuple, class Func, class Tuple>
            constexpr static auto Dispatch(NthTuple&& nth_tuple, Func&& func, Tuple&& tuple)
            {
                constexpr std::int64_t nth_tuple_size = std::tuple_size<NthTuple>::value;
                constexpr std::int64_t function_args_size = std::tuple_size<Tuple>::value;

                return ReplaceNthConsecutiveArgumentAndApplyImpl(
                    std::forward<NthTuple>(nth_tuple), std::forward<Func>(func), std::forward<Tuple>(tuple), WhenTupleAtFirst{},
                    std::make_index_sequence<nth_tuple_size>{}, AddNToTheSequence<nth_tuple_size>(std::make_index_sequence<function_args_size - nth_tuple_size>{}));
            }
        };
        template <std::int64_t N, std::int64_t delta_size>
        struct ReplaceNthConsecutiveArgumentAndApplyDispatcher<N, delta_size, std::enable_if_t<(N != 0 && delta_size > 0)>>
        {
            template <class NthTuple, class Func, class Tuple>
            constexpr static auto Dispatch(NthTuple&& nth_tuple, Func&& func, Tuple&& tuple)
            {
                constexpr std::int64_t nth_tuple_size = std::tuple_size<NthTuple>::value;

                return ReplaceNthConsecutiveArgumentAndApplyImpl(
                    std::forward<NthTuple>(nth_tuple), std::forward<Func>(func), std::forward<Tuple>(tuple), WhenTupleAtMiddle{},
                    std::make_index_sequence<N>{}, std::make_index_sequence<nth_tuple_size>{}, AddNToTheSequence<N + nth_tuple_size>(std::make_index_sequence<delta_size>{}));
            }
        };
        template <std::int64_t N, std::int64_t delta_size>
        struct ReplaceNthConsecutiveArgumentAndApplyDispatcher<N, delta_size, std::enable_if_t<N != 0 && !(delta_size > 0)>>
        {
            template <class NthTuple, class Func, class Tuple>
            constexpr static auto Dispatch(NthTuple&& nth_tuple, Func&& func, Tuple&& tuple)
            {
                constexpr std::int64_t nth_tuple_size = std::tuple_size<NthTuple>::value;

                return ReplaceNthConsecutiveArgumentAndApplyImpl(
                    std::forward<NthTuple>(nth_tuple), std::forward<Func>(func), std::forward<Tuple>(tuple), WhenTupleAtLast{},
                    std::make_index_sequence<N>{}, std::make_index_sequence<nth_tuple_size>{});
            }
        };

        template <std::int64_t N, class NthTuple, class Func, class... Args>
        constexpr auto ReplaceNthConsecutiveArgumentAndApply(NthTuple&& nth_tuple, Func&& func, Args&&... args)
        {
            constexpr std::int64_t size = sizeof...(Args);
            constexpr std::int64_t nth_tuple_size = std::tuple_size<NthTuple>::value;
            static_assert(size >= N + nth_tuple_size, "N can NOT larger than size of parameters pack.");
            constexpr std::int64_t delta_size = size - nth_tuple_size - N;

            return ReplaceNthConsecutiveArgumentAndApplyDispatcher<N, delta_size>::template Dispatch(
                std::forward<NthTuple>(nth_tuple),
                std::forward<Func>(func),
                std::forward_as_tuple(args...));
        }

        extern const CDrawTextReplacedFunction<User32DrawTextManager::A> on_draw_text_a_call;

        extern const CDrawTextReplacedFunction<User32DrawTextManager::W> on_draw_text_w_call;

        extern const CDrawTextReplacedFunction<User32DrawTextManager::ExA> on_draw_text_exa_call;

        extern const CDrawTextReplacedFunction<User32DrawTextManager::ExW> on_draw_text_exw_call;
    }

    auto EnableAllReplaceFunction(Details::DrawTextReplacedFunctionState& ref_state) noexcept
        -> class EnableAllReplaceFunctionGuard;

    class EnableAllReplaceFunctionGuard
    {
        friend auto EnableAllReplaceFunction(Details::DrawTextReplacedFunctionState&) noexcept
            -> EnableAllReplaceFunctionGuard;

    private:
        EnableAllReplaceFunctionGuard() = default;

    public:
        ~EnableAllReplaceFunctionGuard();

        EnableAllReplaceFunctionGuard(const EnableAllReplaceFunctionGuard&) = delete;
        EnableAllReplaceFunctionGuard& operator=(const EnableAllReplaceFunctionGuard&) = delete;
        EnableAllReplaceFunctionGuard(EnableAllReplaceFunctionGuard&&) = default;
        EnableAllReplaceFunctionGuard& operator=(EnableAllReplaceFunctionGuard&&) = default;
    };
};

#define TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(sp_data) \
    std::lock_guard<std::mutex> cache_and_expire_interval_lock_guard(sp_data->m_mutex)

#define TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_GC_INTERVAL(sp_data) \
    std::lock_guard<std::mutex> gc_lock_guard(sp_data->m_gc_mutex)

class CD2D1BitmapCache
{
public:
    using CacheInitializer =
        std::function<Microsoft::WRL::ComPtr<ID2D1Bitmap>(Microsoft::WRL::ComPtr<ID2D1RenderTarget>, HBITMAP)>;

private:
    struct Cache
    {
        Microsoft::WRL::ComPtr<ID2D1Bitmap> m_cache{};
        std::chrono::steady_clock::time_point m_init_timestamp{std::chrono::steady_clock::now()};
        CacheInitializer m_cache_initializer{CD2D1BitmapCache::CreateD2D1BitmapFromHBitmap};
        void Update(Microsoft::WRL::ComPtr<ID2D1RenderTarget> p_render_target, HBITMAP hbitmap);
    };
    struct HeapData
    {
        std::mutex m_mutex{};
        std::unordered_map<HBITMAP, Cache> m_cache_map;
        std::chrono::seconds m_cache_expire_interval{std::chrono::seconds(10)};
        /**
         * @brief 检查缓存是否过期 ！！此函数不加锁！！
         *
         * @param cache 输入的缓存
         * @param now 输入的时间，默认为std::chrono::steady_clock::now()
         * @return true 缓存已过期
         * @return false 缓存未过期
         */
        bool IsCacheExpire(const Cache& cache, std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now());

        std::mutex m_gc_mutex{};
        std::chrono::seconds m_gc_interval{std::chrono::seconds(60)};
    };

    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_p_render_target;
    std::shared_ptr<HeapData> m_sp_data{std::make_shared<HeapData>()};
    std::thread m_gc_thread{
        [wp_data = std::weak_ptr<HeapData>(m_sp_data)]()
        {
            do
            {
                auto sp_data = wp_data.lock();
                if (!sp_data)
                {
                    break;
                }

                CD2D1BitmapCache::GCImpl(sp_data);

                decltype(sp_data->m_gc_interval) gc_interval;
                {
                    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_GC_INTERVAL(sp_data);
                    gc_interval = sp_data->m_gc_interval;
                }
                std::this_thread::sleep_for(gc_interval);
            } while (true);
        }};

public:
    CD2D1BitmapCache(Microsoft::WRL::ComPtr<ID2D1RenderTarget> p_render_target);
    ~CD2D1BitmapCache();
    // 尚不支持复制或移动
    CD2D1BitmapCache(const CD2D1BitmapCache&) = delete;
    CD2D1BitmapCache& operator=(const CD2D1BitmapCache&) = delete;

    void RebindRenderTarget(Microsoft::WRL::ComPtr<ID2D1RenderTarget> p_render_target);
    void AddHBitmap(HBITMAP hbitmap, CacheInitializer initializer);
    void AddHBitmap(HBITMAP hbitmap);
    void RemoveHBitmap(HBITMAP hbitmap);
    void RecreateAllHBitmaps();
    static auto CreateD2D1BitmapFromHBitmap(Microsoft::WRL::ComPtr<ID2D1RenderTarget> p_render_target, HBITMAP hbitmap)
        -> Microsoft::WRL::ComPtr<ID2D1Bitmap>;
    void GC();
    void SetGCInterval(const std::chrono::seconds interval);
    void SetExpireInterval(const std::chrono::seconds interval);
    auto GetCachedBitmap(HBITMAP hbitmap)
        -> Microsoft::WRL::ComPtr<ID2D1Bitmap>;

private:
    /**
     * @brief 检查BITMAP是否已经缓存 ！！此函数不加锁！！
     *
     * @param hbitmap 输入的HBITMAP
     * @return true BITMAP已经被缓存
     * @return false BITMAP尚未缓存
     */
    bool IsHBitmapExist(HBITMAP hbitmap) const;
    static void GCImpl(std::shared_ptr<HeapData> sp_data);
};

class CTaskBarDlgDrawCommon final : public IDrawCommon
{
private:
    constexpr static int DEFAULT_GDI_OP_TEXTURE_ALPHA = 2;

    CTaskBarDlgDrawCommonWindowSupport* m_p_window_support{nullptr};
    class CGdiInteropObject
    {
    public:
        CDC m_gdi_interop_cdc{};
        HBITMAP m_gdi_interop_hbitmap{};
        void* m_p_gdi_interop_hbitmap_data{};
        HGDIOBJ m_gdi_interop_old_hbitmap{};

        CGdiInteropObject(D2D1_SIZE_U size);
        ~CGdiInteropObject();
    };
    DefaultCLazyConstructableWithInitializer<CGdiInteropObject, D2D1_SIZE_U> m_gdi_interop_object{
        [this]()
        { return std::make_tuple(m_p_window_support->GetSize()); }};
    COLORREF m_text_color{};
    bool m_is_clipped{false};

    void ResetClippedStateIfSet();

public:
    CTaskBarDlgDrawCommon() = default;
    ~CTaskBarDlgDrawCommon();

    void SetBackColor(COLORREF back_color, BYTE apha = 255) override;
    void SetFont(CFont* pfont) override;
    // 在指定的矩形区域内绘制文本
    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) override;
    // 设置绘图剪辑区域
    void SetDrawRect(CRect rect) override;
    // 用纯色填充矩形
    void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) override;
    // 绘制矩形边框。如果dot_line为true，则为虚线
    void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255) override;
    // 使用当前画笔画线
    void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255) override;
    void SetTextColor(const COLORREF color, BYTE alpha = 255) override;
    // 绘制一个位图
    // （注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
    // 需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
    void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) override;

    void Create(CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support, const D2D1_SIZE_U size);
    template <class GdiOp>
    void ExecuteGdiOperation(CRect rect, GdiOp gdi_op)
    {
        auto& ref_gdi_interop_object = m_gdi_interop_object.Get();
        auto old_hfont = ref_gdi_interop_object.m_gdi_interop_cdc.SelectObject(
            m_p_window_support->GetFont());
        ref_gdi_interop_object.m_gdi_interop_cdc.SetTextColor(m_text_color);
        TaskBarDlgUser32DrawTextHook::Details::DrawTextReplacedFunctionState state{ref_gdi_interop_object.m_gdi_interop_cdc};

        {
            auto enable_all_replaced_function_guard =
                TaskBarDlgUser32DrawTextHook::EnableAllReplaceFunction(state);
            gdi_op(ref_gdi_interop_object.m_gdi_interop_cdc);
        }

        ref_gdi_interop_object.m_gdi_interop_cdc.SelectObject(old_hfont);
    }

    static auto Convert(CPoint point) noexcept
        -> D2D1_POINT_2F;
    static auto Convert(CRect rect) noexcept
        -> D2D1_RECT_F;
};

class CTaskBarDlgDrawBuffer final : public IDrawBuffer
{
private:
    Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_surface{};
    CSize m_size;
    HWND m_target_hwnd;
    CTaskBarDlgDrawCommonWindowSupport& m_ref_window_support;

public:
    CTaskBarDlgDrawBuffer(CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support, CSize size, HWND hwnd);
    ~CTaskBarDlgDrawBuffer();

    static auto GetDefaultBlendFunctionPointer() noexcept
        -> const ::PBLENDFUNCTION;
};
