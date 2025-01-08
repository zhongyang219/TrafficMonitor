#pragma once
#include <tuple>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>
#include <type_traits>
#include <initializer_list>
#include "IDrawCommon.h"
#include "Common.h"
#include "D3D10Support1.h"
#include "D2D1Support.h"
#include "DCompositionSupport.h"
#include "Dxgi1Support2.h"
#include "Image2DEffect.h"
#include "Nullable.hpp"
#include "DrawTextManager.h"

void LogWin32ApiErrorMessage(DWORD error_code) noexcept;

namespace DrawCommonHelper
{
    void LogDeviceRecreateReason(HRESULT hr);

    namespace Details
    {
        template <typename T, T First>
        constexpr bool CompareEqOr(const T input)
        {
            return input == First;
        }
        template <typename T, T First, T... Others, typename... PlaceHolder>
        constexpr auto CompareEqOr(const T input, PlaceHolder...)
            -> std::enable_if_t<sizeof...(PlaceHolder) == 0, bool>
        {
            return input == First || CompareEqOr<T, Others...>(input);
        }
    } // namespace Details

    template <HRESULT... HRESULTs>
    struct hresult_sequence
    {
        constexpr static bool CompareEqOr(const HRESULT input)
        {
            return Details::CompareEqOr<HRESULT, HRESULTs...>(input);
        }
    };
    template <class T>
    struct is_hresult_sequence : public std::false_type
    {
    };
    template <HRESULT... HRESULTs>
    struct is_hresult_sequence<hresult_sequence<HRESULTs...>> : public std::true_type
    {
    };
    template <class HResultSequence, class Func, class = std::enable_if_t<is_hresult_sequence<HResultSequence>{}>>
    bool HandleIfNeedRecreate(CHResultException& ref_ex, Func handler)
    {
        auto hr = ref_ex.GetHResult();
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET || HResultSequence::CompareEqOr(hr))
        {
            handler();
            return true;
        }
        return false;
    }
    template <class Func>
    bool HandleIfNeedRecreate(CHResultException& ref_ex, Func handler)
    {
        auto hr = ref_ex.GetHResult();
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            handler();
            return true;
        }
        return false;
    }

    constexpr std::uint32_t MAX_D2D1_RENDER_ERROR_COUNT = 77;
    template <class Func>
    bool HandleIfD2D1DeviceNeedRecreate(CHResultException& ref_ex, Func handler)
    {
        return HandleIfNeedRecreate<hresult_sequence<D2DERR_RECREATE_TARGET, D2DERR_WRONG_STATE>>(ref_ex, handler);
    }
    class DefaultD2DDrawCommonExceptionHandler
    {
    private:
        CHResultException& m_ref_ex;
        static std::uint32_t m_error_count;

    public:
        DefaultD2DDrawCommonExceptionHandler(CHResultException& ex) noexcept;
        DefaultD2DDrawCommonExceptionHandler(const DefaultD2DDrawCommonExceptionHandler&) = delete;
        DefaultD2DDrawCommonExceptionHandler& operator=(const DefaultD2DDrawCommonExceptionHandler&) = delete;
        DefaultD2DDrawCommonExceptionHandler* operator&() = delete;

        static void ResetErrorCount();
        static void HandleErrorCountIncrement();
        static void IncreaseErrorCountManually();
        /**
         * @brief 使用自定义处理函数在LogHResultException(m_ref_ex)语句后接受m_ref_ex进行处理
         *
         * @tparam Func 形如bool (CHResultException&)的可调用类型
         * @param handler 自定义处理函数，接受CHResultException&，返回true代表不增加D2D渲染的错误计数，返回false代表含义则相反。
         例如，在发现需要重新初始化设备后，请求重新初始化设备并返回true
         */
        template <class Func>
        void operator()(Func handler)
        {
            LogHResultException(m_ref_ex);
            if (handler(m_ref_ex))
            {
                return;
            }
            HandleErrorCountIncrement();
        }
        void operator()()
        {
            LogHResultException(m_ref_ex);
            HandleErrorCountIncrement();
        }
    };
    HDC Get1x1AlphaEqual1DC();
}

class CTaskBarDlgDrawCommonSupport
{
private:
    CD3D10Device1 m_d3d10_device1{};
    CD2D1Device m_d2d1_device{};
    CDCompositionDevice m_dcomposition_device{};
    Microsoft::WRL::ComPtr<ID2D1StrokeStyle> m_p_ps_dot_like_style{};

    bool IsAllDevicesRecreatedByThisFunction();
    void InternalRecreateD3D10Device1();

public:
    CTaskBarDlgDrawCommonSupport();
    ~CTaskBarDlgDrawCommonSupport() = default;

    auto GetD3D10Device1() noexcept
        -> CD3D10Device1&;
    auto GetD2D1Device() noexcept
        -> CD2D1Device&;
    auto GetDCompositionDevice() noexcept
        -> CDCompositionDevice&;
    auto GetPsDotLikeStyle() noexcept
        -> Microsoft::WRL::ComPtr<ID2D1StrokeStyle>;
    void RecreateAll();
    void RecreateD3D10Device1(const HRESULT recreate_reason = S_OK);
    void RecreateD2D1Device(const HRESULT recreate_reason = S_OK);
    void RecreateDCompositionDevice(const HRESULT recreate_reason = S_OK);
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
    class CD3DGdiInteropHelper final : public CDeviceResource<CD3D10Device1>
    {
        friend CTaskBarDlgDrawCommonWindowSupport;

        using Base = CDeviceResource<CD3D10Device1>;
        using Base::DeviceType;

    private:
        DeviceType m_p_device1{};
        Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_gdi_initial_texture{};
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_initial_surface{};
        Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_gdi_final_texture{};
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_final_surface{};
        CImage2DEffect m_alpha_value_effect;
        bool m_is_p_device1_recreated{false};

        static auto CreateDefaultTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;
        static auto CreateCpuWriteableTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;
        static auto GetPsAlphaValueReducer() noexcept
            -> Microsoft::WRL::ComPtr<ID3D10Blob>;

    public:
        CD3DGdiInteropHelper(CD3D10Device1& ref_device);
        ~CD3DGdiInteropHelper() = default;

        void OnDeviceRecreate(DeviceType p_new_device) noexcept override;

        bool HandleDeviceRecreationIfNecessary();
        void Resize(D2D1_SIZE_U size);
        auto GetRenderTargetSurface()
            -> Microsoft::WRL::ComPtr<IDXGISurface>;
    };
    class CDWriteHelper
    {
    private:
        Microsoft::WRL::ComPtr<IDWriteTextFormat> m_p_text_format{};
        HFONT m_h_last_font{};
        LOGFONTW m_last_font{};

    public:
        CDWriteHelper() = default;
        ~CDWriteHelper() = default;

        void SetFont(HFONT h_font);
        auto GetFont() noexcept
            -> HFONT;
        auto GetTextFormat() noexcept
            -> Microsoft::WRL::ComPtr<IDWriteTextFormat>;
    };

    D2D1_SIZE_U m_size{0, 0};
    CD3DGdiInteropHelper m_d3d_gdi_interop_helper;
    CDWriteHelper m_dwrite_helper{};
    CTaskBarDlgDrawCommonSupport& m_ref_taskbardlg_draw_common_support;
    DWORD m_update_layered_window_error_code{0};
    bool m_is_size_updated{true};

public:
    constexpr static float DEFAULT_DPI = 96.f;
    constexpr static auto PIXEL_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;

    explicit CTaskBarDlgDrawCommonWindowSupport(CTaskBarDlgDrawCommonSupport& ref_taskbdlg_draw_common_support);
    ~CTaskBarDlgDrawCommonWindowSupport() = default;

    void Resize(const D2D1_SIZE_U size);
    void ResizeGdiInteropPart();
    void SetFont(HFONT h_font);
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
    void RequestD3D10Device1Recreate();
};

/**
 * @brief 此类具有和CTaskBarDlgDrawCommonWindowSupport相似的逻辑，
 必须先SetWorkingDevice，再进行Resize才可以视为完全初始化的状态。
 在没有Resize前，都视为未初始化的状态，此时不应该调用其它成员函数。
 *
 */
class CD2D1DeviceContextWindowSupport
{
public:
    class CD2D1DeviceContextHelper : public CDeviceResource<CD2D1Device>
    {
        using Base = CDeviceResource<CD2D1Device>;
        using Base::DeviceType;

    private:
        DeviceType m_p_device{};
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_p_device_context{};
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_p_d2d_render_target_bitmap{};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_p_foreground_color_brush{};
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_p_background_color_brush{};
        std::weak_ptr<class CD2D1BitmapCache> m_wp_bitmap_cache{};
        D2D1_COLOR_F m_foreground_color{D2D1::ColorF::Black};
        D2D1_COLOR_F m_background_color{D2D1::ColorF::Black};
        bool m_need_recreate{false};

        void Reinitialize();

    public:
        CD2D1DeviceContextHelper(CD2D1Device& ref_device);
        bool HandleDeviceRecreationIfNecessary();
        void SetBackColor(COLORREF color, BYTE alpha) noexcept;
        auto GetRawBackSolidColorBruch() noexcept
            -> ID2D1SolidColorBrush*;
        void SetForeColor(COLORREF color, BYTE alpha) noexcept;
        auto GetRawForeSolidColorBruch() noexcept
            -> ID2D1SolidColorBrush*;
        void OnDeviceRecreate(DeviceType p_new_device) noexcept override;
        void RebindD2D1BitmapCache(std::weak_ptr<class CD2D1BitmapCache> wp_cache);
        void RebindRenderTargetSurface(Microsoft::WRL::ComPtr<IDXGISurface> p_surface);
        void Reset();
        auto GetCachedBitmap(HBITMAP hbitmap) const
            -> Microsoft::WRL::ComPtr<ID2D1Bitmap>;
        auto GetDeviceContext() const
            -> Microsoft::WRL::ComPtr<ID2D1DeviceContext>;
    };
    class CDCompositionHelper : public CDeviceResource<CDCompositionDevice>,
                                public CDeviceResource<CD3D10Device1>
    {
        using CDCompositionResourceBase = CDeviceResource<CDCompositionDevice>;
        using CDCompositionDeviceType = typename CDCompositionResourceBase::DeviceType;
        using CD3D10Device1ResourceBase = CDeviceResource<CD3D10Device1>;
        using CD3D10Device1Type = typename CD3D10Device1ResourceBase::DeviceType;

    private:
        // DComposition Part Begin
        CDCompositionDeviceType m_p_composition_device{};
        Microsoft::WRL::ComPtr<IDCompositionTarget> m_p_target{};
        Microsoft::WRL::ComPtr<IDCompositionVisual> m_p_visual{};
        // D3D10Device1 Part Begin
        CD3D10Device1Type m_p_d3d10_device1{};
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_p_swap_chain1{};
        HWND m_hwnd{};
        D2D1_SIZE_U m_size{};
        bool m_need_recreate_dcomposition_device{false};
        bool m_need_recreate_d3d10_device1{false};

        void InitializeSwapChain(const D2D1_SIZE_U size);
        void RebindSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain1> p_swap_chain1);
        void ReinitializeDCompositionPart();
        void ReinitializeD3D10Device1Part();

    public:
        CDCompositionHelper(CDCompositionDevice& ref_dcomposition_device, CD3D10Device1& ref_d3d10_device1, HWND hwnd);
        void OnDeviceRecreate(CDCompositionDeviceType p_new_composition_device) noexcept override;
        void OnDeviceRecreate(CD3D10Device1Type p_new_d3d10_device1) noexcept override;
        bool HandleDeviceRecreationIfNecessary();
        /**
         * @brief 执行此函数前，必须调用CD2D1DeviceContextHelper::Reset()释放持有交换链的ID2D1Bitmap1和ID2D1DeviceContext
         *
         * @param size 新的交换链大小
         * @return Microsoft::WRL::ComPtr<IDXGISurface> 使用Resize后交换链创建的IDXGISurface
         */
        auto Resize(const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<IDXGISurface>;
        void Present();
        auto GetD3D10Device1()
            -> Microsoft::WRL::ComPtr<ID3D10Device1>;
        bool IsInvalid() const;
    };
    class CD3DHelper : public CDeviceResource<CD3D10Device1>
    {
        using Base = CDeviceResource<CD3D10Device1>;
        using Base::DeviceType;

    private:
        Microsoft::WRL::ComPtr<ID3D10Device1> m_p_device1{};
        Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_render_target_texture{};
        Microsoft::WRL::ComPtr<IDXGISurface1> m_p_render_target_surface{};
        bool m_need_recreate{false};

        static auto CreateGdiCompatibleTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;

    public:
        CD3DHelper(CD3D10Device1& ref_device);
        auto Resize(const D2D1_SIZE_U size)
            -> Microsoft::WRL::ComPtr<IDXGISurface>;
        bool HandleDeviceRecreationIfNecessary();
        void OnDeviceRecreate(DeviceType p_new_device) noexcept override;
        auto GetRenderTargetSurface()
            -> Microsoft::WRL::ComPtr<IDXGISurface1>;
        auto GetD3D10Device1()
            -> Microsoft::WRL::ComPtr<ID3D10Device1>;
    };
    using VariantStorage = AlignedUnionStorage<
        CNullable<CDCompositionHelper>,
        CNullable<CD3DHelper>>;

    CTaskBarDlgDrawCommonSupport& m_ref_task_bar_dlg_draw_common_support;
    D2D1_SIZE_U m_size{0, 0};
    CD2D1DeviceContextHelper m_d2d1_device_context_helper;
    DrawCommonHelper::RenderType m_render_type{DrawCommonHelper::RenderType::D2D1};
    bool m_is_initialized{false};
    VariantStorage m_impl_storage;

    void Dispatch(std::initializer_list<std::tuple<DrawCommonHelper::RenderType, std::function<void()>>> initializer_list)
    {
        DrawCommonHelper::RenderType render_type;
        std::function<void()> function;
        for (auto&& item : initializer_list)
        {
            std::tie(render_type, function) = item;
            if (render_type == m_render_type)
            {
                return function();
            }
        }
    }

    template <class R>
    auto Dispatch(std::initializer_list<std::tuple<DrawCommonHelper::RenderType, std::function<R()>>> initializer_list)
        -> CNullable<R>
    {
        DrawCommonHelper::RenderType render_type;
        std::function<R()> function;
        CNullable<R> result{};
        for (auto&& item : initializer_list)
        {
            std::tie(render_type, function) = item;
            if (render_type == m_render_type)
            {
                result.Construct(function());
                return result;
            }
        }
        return result;
    }
    template <class T>
    auto GetStorageAs()
        -> std::add_lvalue_reference_t<T>
    {
        return reinterpret_cast<std::add_lvalue_reference_t<T>>(m_impl_storage);
    }
    auto GetNullableDCompositionHelper()
        -> CNullable<CDCompositionHelper>&;
    auto GetNullableD3DHelper()
        -> CNullable<CD3DHelper>&;
    void DestroyStorage();

public:
    CD2D1DeviceContextWindowSupport(CTaskBarDlgDrawCommonSupport& ref_task_bar_dlg_draw_common_support);
    ~CD2D1DeviceContextWindowSupport();
    auto SetWorkingDevice(CDCompositionDevice& ref_dcomposition_device, CD3D10Device1& ref_d3d10_device1, HWND hwnd)
        -> CDCompositionHelper&;
    auto SetWorkingDevice(CD3D10Device1& ref_device1)
        -> CD3DHelper&;
    auto Resize(const D2D1_SIZE_U size)
        -> Microsoft::WRL::ComPtr<ID2D1DeviceContext>;
    /**
     * @brief 当m_render_type = DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION时才会执行
     *
     */
    void PresentWhenUseDComposition();
    /**
     * @brief 当m_render_type = DrawCommonHelper::RenderType::D2D1时才返回非空值
     *
     * @return Microsoft::WRL::ComPtr<IDXGISurface1>
     */
    auto GetRenderTargetSurfaceWhenUseDefault()
        -> Microsoft::WRL::ComPtr<IDXGISurface1>;
    void RebindD2D1BitmapCache(std::weak_ptr<class CD2D1BitmapCache> wp_cache);
    void SetBackColor(COLORREF color, BYTE alpha) noexcept;
    auto GetRawBackSolidColorBruch() noexcept
        -> ID2D1SolidColorBrush*;
    auto SetForeColor(COLORREF color, BYTE alpha) noexcept
        -> CD2D1DeviceContextWindowSupport*;
    auto GetRawForeSolidColorBruch() noexcept
        -> ID2D1SolidColorBrush*;
    auto GetCachedBitmap(HBITMAP hbitmap)
        -> Microsoft::WRL::ComPtr<ID2D1Bitmap>;
    void RequestD3D10Device1Recreate();
    void RequestD2D1DeviceRecreate(const HRESULT hr);
    void RequestDCompositionDeviceRecreate(const HRESULT hr);
};

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

        template <std::size_t N, std::size_t... Indexes>
        constexpr auto AddNToTheSequence(std::index_sequence<Indexes...>)
            -> std::index_sequence<(Indexes + N)...>
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
    CD2D1DeviceContextWindowSupport* m_p_d2d1_device_context_support{nullptr};
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_p_device_context{};
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

    void SetBackColor(COLORREF back_color, BYTE alpha = 255) override;
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

    void Create(
        CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support,
        CD2D1DeviceContextWindowSupport& ref_d2d1_device_window_support,
        const D2D1_SIZE_U size);
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

    auto GetD3D10Device1RecreateRequester()
        -> std::function<bool(CHResultException& ex)>;
    auto GetD2D1DeviceRecreateRequester()
        -> std::function<bool(CHResultException& ex)>;
    auto GetDCompositionDeviceRecreateRequester()
        -> std::function<bool(CHResultException& ex)>;

    static auto Convert(CPoint point) noexcept
        -> D2D1_POINT_2F;
    static auto Convert(CRect rect) noexcept
        -> D2D1_RECT_F;
};

/**
 * @brief 仅在RenderType = DrawCommonHelper::RenderType::D2D1时可用，
 此时输入的IDXGISurface1是兼容GDI的
 *
 */
class CTaskBarDlgDrawBuffer final : public IDrawBuffer
{
private:
    Microsoft::WRL::ComPtr<IDXGISurface1> m_p_gdi_surface{};
    CSize m_size;
    HWND m_target_hwnd;
    CTaskBarDlgDrawCommonWindowSupport& m_ref_window_support;

public:
    CTaskBarDlgDrawBuffer(
        CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support,
        CD2D1DeviceContextWindowSupport& ref_d2d1_device_context_support,
        CSize size,
        HWND hwnd);
    ~CTaskBarDlgDrawBuffer() override;

    static auto GetDefaultBlendFunctionPointer() noexcept
        -> const ::PBLENDFUNCTION;
};

class CTaskBarDlgDrawBufferUseDComposition : public IDrawBuffer
{
private:
    CD2D1DeviceContextWindowSupport* m_p_d2d1_device_context_support{};

public:
    CTaskBarDlgDrawBufferUseDComposition(CD2D1DeviceContextWindowSupport& ref_d2d1_device_context_support);
    ~CTaskBarDlgDrawBufferUseDComposition() override;
};
