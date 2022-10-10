﻿#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include "HResultException.h"

class CD2D1Exception final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CDWriteException final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CD2D1Support
{
public:
    static bool CheckSupport();
    static ID2D1Factory* GetFactory();
};

class CD2D1DCSupport
{
public:
    CD2D1DCSupport();
    ~CD2D1DCSupport();
    static bool CheckSupport();
    auto GetRenderTarget()
        -> Microsoft::WRL::ComPtr<ID2D1DCRenderTarget>;
    auto GetWeakRenderTarget()
        -> ID2D1DCRenderTarget*;
    auto GetWeakSolidColorBrush()
        -> ID2D1SolidColorBrush*;
    auto GetWeakPsDotStyle()
        -> ID2D1StrokeStyle*;
    auto GetWeakDWriteGdiInterop()
        -> IDWriteGdiInterop*;
    auto GetWeakSoildBackColorBrush()
        -> ID2D1SolidColorBrush*;

private:
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> m_p_render_target{NULL};
    ID2D1SolidColorBrush* m_p_soild_color_brush{NULL};
    ID2D1SolidColorBrush* m_p_soild_back_color_brush{NULL};
    ID2D1StrokeStyle* m_p_ps_dot_style{NULL};
    IDWriteGdiInterop* m_p_dwrite_gdi_interop{NULL};
};

class DWriteSupport
{
public:
    static bool CheckSupport();
    static IDWriteFactory* GetFactory();
};