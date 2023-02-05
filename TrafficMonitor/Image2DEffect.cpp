#include "stdafx.h"
#include "Image2DEffect.h"
#include "Common.h"

namespace D3DQuadrangle
{
    const std::array<std::uint16_t, 6> VERTEX_INDEX_LIST{
        0, 1, 2,
        2, 3, 0};

    const CShader& GetVsShader()
    {
        static auto result = MakeStaticVariableWrapper<CShader>(
            [](CShader* p_content)
            {
                p_content->SetCode(
                             CIMAGE2DEFFECT_SHADER_VS_INPUT_DECLARATION
                                 CIMAGE2DEFFECT_SHADER_VS_OUTPUT_DECLARATION
                             R"(
VsOutput VS(VsInput input){
   VsOutput result;
   result.position = input.position;
   result.texcoord = input.texcoord;
   return result;
}
)")
                    .SetEntryPoint("VS")
                    .SetName("D3DQuadrangleDefaultVS")
                    .SetTarget("vs_4_1")
                    .SetFlags1(D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS);
            });
        return result.Get();
    }

}

CImage2DEffect::CImage2DEffect(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1)
    : m_p_owner_device1{p_device1}
{
    RecreateUnmodifiableResource(m_p_owner_device1);
    RecreateVsByteCodeRelatedResources(m_p_vs_byte_code);
}

void CImage2DEffect::RecreateUnmodifiableResource(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1)
{
    constexpr static std::array<D3D10_INPUT_ELEMENT_DESC, 2> input_elements_desc{
        {{"POSITION",
          0,
          DXGI_FORMAT_R32G32B32_FLOAT,
          SLOT,
          offsetof(Image2DVertex, position),
          D3D10_INPUT_PER_VERTEX_DATA,
          0},
         {"TEXCOORD",
          0,
          DXGI_FORMAT_R32G32_FLOAT,
          SLOT,
          offsetof(Image2DVertex, texture0),
          D3D10_INPUT_PER_VERTEX_DATA,
          0}}};
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateInputLayout(
            input_elements_desc.data(),
            static_cast<UINT>(input_elements_desc.size()),
            m_p_vs_byte_code->GetBufferPointer(),
            m_p_vs_byte_code->GetBufferSize(),
            &m_p_input_layout),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect call CreateInputLayout failed."));

    QuadrangleVertexs vertexes;
    float* vertex_position;
    float* vertex_texture;

    // 左上角
    vertex_position = vertexes.GetLeftTopVertex().position;
    vertex_position[0] = -1.f; // x
    vertex_position[1] = 1.f;  // y
    vertex_position[2] = .0f;  // z
    vertex_texture = vertexes.GetLeftTopVertex().texture0;
    vertex_texture[0] = 0.f; // u
    vertex_texture[1] = 0.f; // v

    // 右上角
    vertex_position = vertexes.GetRightTopVertex().position;
    vertex_position[0] = 1.f;
    vertex_position[1] = 1.f;
    vertex_position[2] = .0f;
    vertex_texture = vertexes.GetRightTopVertex().texture0;
    vertex_texture[0] = 1.f;
    vertex_texture[1] = 0.f;

    // 右下角
    vertex_position = vertexes.GetRightBottomVertex().position;
    vertex_position[0] = 1.f;
    vertex_position[1] = -1.f;
    vertex_position[2] = .0f;
    vertex_texture = vertexes.GetRightBottomVertex().texture0;
    vertex_texture[0] = 1.f;
    vertex_texture[1] = 1.f;

    // 左下角
    vertex_position = vertexes.GetLeftBottomVertex().position;
    vertex_position[0] = -1.f;
    vertex_position[1] = -1.f;
    vertex_position[2] = .0f;
    vertex_texture = vertexes.GetLeftBottomVertex().texture0;
    vertex_texture[0] = 0;
    vertex_texture[1] = 1;

    D3D10_BUFFER_DESC vertex_buffer_desc{};
    vertex_buffer_desc.ByteWidth = static_cast<UINT>(vertexes.GetSize());
    vertex_buffer_desc.Usage = D3D10_USAGE_IMMUTABLE;
    vertex_buffer_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    D3D10_SUBRESOURCE_DATA vertex_data{};
    vertex_data.pSysMem = vertexes.GetData();
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateBuffer(
            &vertex_buffer_desc,
            &vertex_data,
            &m_p_vertex_buffer),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create vertex buffer failed."));

    D3D10_BUFFER_DESC index_buffer_desc{};
    index_buffer_desc.Usage = D3D10_USAGE_IMMUTABLE;
    index_buffer_desc.ByteWidth = sizeof(D3DQuadrangle::VERTEX_INDEX_LIST);
    index_buffer_desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    D3D10_SUBRESOURCE_DATA index_data{};
    index_data.pSysMem = &D3DQuadrangle::VERTEX_INDEX_LIST;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateBuffer(
            &index_buffer_desc,
            &index_data,
            &m_p_index_buffer),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create index buffer failed."));

    D3D10_RASTERIZER_DESC rasterizer_desc{};
    rasterizer_desc.FillMode = D3D10_FILL_SOLID;
    rasterizer_desc.CullMode = D3D10_CULL_NONE;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    rasterizer_desc.DepthClipEnable = FALSE;
    rasterizer_desc.ScissorEnable = FALSE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateRasterizerState(
            &rasterizer_desc,
            &m_p_rasterizer_state),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create rasterizer state failed."));

    D3D10_SAMPLER_DESC tex0_sampler = {};
    tex0_sampler.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
    tex0_sampler.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
    tex0_sampler.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
    tex0_sampler.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
    tex0_sampler.MipLODBias = 0.0f;
    tex0_sampler.MaxAnisotropy = 1;
    tex0_sampler.ComparisonFunc = D3D10_COMPARISON_NEVER;
    tex0_sampler.MinLOD = -FLT_MAX;
    tex0_sampler.MaxLOD = FLT_MAX;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateSamplerState(
            &tex0_sampler,
            &m_p_ps_tex0_sampler_state),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create default sampler state failed."));

    D3D10_BLEND_DESC1 blend_desc1{};
    blend_desc1.AlphaToCoverageEnable = FALSE;
    blend_desc1.IndependentBlendEnable = FALSE;
    auto& render_target_blend_desc0 = blend_desc1.RenderTarget[0];
    render_target_blend_desc0.BlendEnable = TRUE;
    render_target_blend_desc0.SrcBlend = D3D10_BLEND_SRC_ALPHA;
    render_target_blend_desc0.DestBlend = D3D10_BLEND_INV_DEST_ALPHA;
    render_target_blend_desc0.BlendOp = D3D10_BLEND_OP_ADD;
    render_target_blend_desc0.SrcBlendAlpha = D3D10_BLEND_ONE;
    render_target_blend_desc0.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
    render_target_blend_desc0.BlendOpAlpha = D3D10_BLEND_OP_ADD;
    render_target_blend_desc0.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateBlendState1(
            &blend_desc1,
            &m_p_blend_state1),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create D3D10.1 blend state for alpha blend failed."));

    D3D10_DEPTH_STENCIL_DESC depth_stencil_desc{};
    depth_stencil_desc.DepthEnable = FALSE;
    depth_stencil_desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D10_COMPARISON_LESS;
    depth_stencil_desc.StencilEnable = FALSE;
    depth_stencil_desc.StencilReadMask = 0xFF;
    depth_stencil_desc.StencilWriteMask = 0xFF;
    depth_stencil_desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    depth_stencil_desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
    depth_stencil_desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateDepthStencilState(
            &depth_stencil_desc,
            &m_p_depth_stencil_state),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect call CreateDepthStencilState failed."));
}

void CImage2DEffect::RecreateInputTextureRelatedResources(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_input)
{
    D3D10_SHADER_RESOURCE_VIEW_DESC tex0_shader_resource_view_desc = {};
    tex0_shader_resource_view_desc.Format = PIXEL_FORMAT;
    tex0_shader_resource_view_desc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
    tex0_shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
    tex0_shader_resource_view_desc.Texture2D.MipLevels = 1;
    ThrowIfFailed<CD3D10Exception1>(
        m_p_owner_device1->CreateShaderResourceView(
            p_input.Get(),
            &tex0_shader_resource_view_desc,
            &m_p_ps_tex0_resource_view),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create shader resource view1 failed."));
}

void CImage2DEffect::RecreateOutputTextureRelatedResources(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_output)
{
    ThrowIfFailed<CD3D10Exception1>(
        m_p_owner_device1->CreateRenderTargetView(
            p_output.Get(),
            NULL,
            &m_p_render_target_view),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create D3D10.1 render target view failed."));
}

void CImage2DEffect::RecreateVsByteCodeRelatedResources(Microsoft::WRL::ComPtr<ID3DBlob> p_vs_byte_code)
{
    ThrowIfFailed<CD3D10Exception1>(
        m_p_owner_device1->CreateVertexShader(
            p_vs_byte_code->GetBufferPointer(),
            p_vs_byte_code->GetBufferSize(),
            &m_p_vs),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create vertex shader failed."));
}

void CImage2DEffect::RecreatePsByteCodeRelatedResources(Microsoft::WRL::ComPtr<ID3DBlob> p_ps_byte_code)
{
    ThrowIfFailed<CD3D10Exception1>(
        m_p_owner_device1->CreatePixelShader(
            p_ps_byte_code->GetBufferPointer(),
            p_ps_byte_code->GetBufferSize(),
            &m_p_ps),
        TRAFFICMONITOR_ERROR_STR("CImage2DEffect create pixel shader failed."));
}

auto CImage2DEffect::ApplyPipelineConfig() const
    -> const CImage2DEffect&
{
    m_p_owner_device1->ClearState();

    m_p_owner_device1->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_p_owner_device1->IASetInputLayout(m_p_input_layout.Get());
    auto raw_m_p_vertex_buffer = m_p_vertex_buffer.Get();
    constexpr static std::array<UINT, 1> strides{static_cast<UINT>(QuadrangleVertexs::GetStride())};
    constexpr static std::array<UINT, 1> offsets{0};
    m_p_owner_device1->IASetVertexBuffers(
        SLOT,
        1,
        &raw_m_p_vertex_buffer,
        strides.data(),
        offsets.data());
    m_p_owner_device1->IASetIndexBuffer(
        m_p_index_buffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0);

    m_p_owner_device1->VSSetShader(m_p_vs.Get());

    D3D10_VIEWPORT viewport{};
    viewport.Width = m_output_width;
    viewport.Height = m_output_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_p_owner_device1->RSSetViewports(1, &viewport);
    m_p_owner_device1->RSSetState(m_p_rasterizer_state.Get());

    ID3D10SamplerState* raw_m_p_ps_tex0_sampler_state = m_p_ps_tex0_sampler_state.Get();
    m_p_owner_device1->PSSetSamplers(
        SLOT,
        1,
        &raw_m_p_ps_tex0_sampler_state);
    auto raw_m_p_ps_tex0_resource_view = m_p_ps_tex0_resource_view.Get();
    m_p_owner_device1->PSSetShaderResources(
        SLOT,
        1,
        &raw_m_p_ps_tex0_resource_view);
    m_p_owner_device1->PSSetShader(m_p_ps.Get());

    m_p_owner_device1->OMSetBlendState(m_p_blend_state1.Get(), NULL, 0xffffffff);
    m_p_owner_device1->OMSetDepthStencilState(m_p_depth_stencil_state.Get(), 0);
    auto p_render_target_view = m_p_render_target_view.Get();
    m_p_owner_device1->OMSetRenderTargets(1, &p_render_target_view, NULL);

    return *this;
}

auto CImage2DEffect::RebindDevice1(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1)
    -> CImage2DEffect&
{
    RecreateUnmodifiableResource(p_device1);
    m_p_owner_device1 = p_device1;
    m_p_input.Reset();
    m_p_output.Reset();
    if (m_p_vs_byte_code)
    {
        RecreateVsByteCodeRelatedResources(m_p_vs_byte_code);
    }
    if (m_p_ps_byte_code)
    {
        RecreatePsByteCodeRelatedResources(m_p_ps_byte_code);
    }
    return *this;
}

auto CImage2DEffect::SetOutputSize(UINT width, UINT height) noexcept
    -> CImage2DEffect&
{
    m_output_width = width > 0 ? width : 1;
    m_output_height = height > 0 ? height : 1;
    return *this;
}

UINT CImage2DEffect::GetOutputWidth() const noexcept
{
    return m_output_width;
}

UINT CImage2DEffect::GetOutputHeight() const noexcept
{
    return m_output_height;
}

auto CImage2DEffect::SetInputTexture(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_input)
    -> CImage2DEffect&
{
    if(m_p_input == p_input)
    {
        return *this;
    }

    RecreateInputTextureRelatedResources(p_input);
    m_p_input = p_input;
    return *this;
}

auto CImage2DEffect::GetInputTexture() noexcept
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    return m_p_input;
}

auto CImage2DEffect::SetOutputTexture(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_output)
    -> CImage2DEffect&
{
    if (m_p_output == p_output)
    {
        return *this;
    }

    RecreateOutputTextureRelatedResources(p_output);
    m_p_output = p_output;
    return *this;
}

auto CImage2DEffect::GetOutputTexture() noexcept
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    return m_p_output;
}

auto CImage2DEffect::SetVsByteCode(Microsoft::WRL::ComPtr<ID3DBlob> p_vs_byte_code)
    -> CImage2DEffect&
{
    RecreateVsByteCodeRelatedResources(p_vs_byte_code);
    m_p_vs_byte_code = p_vs_byte_code;
    return *this;
}

auto CImage2DEffect::GetVsByteCode() noexcept
    -> Microsoft::WRL::ComPtr<ID3DBlob>
{
    return m_p_vs_byte_code;
}

auto CImage2DEffect::SetPsByteCode(Microsoft::WRL::ComPtr<ID3DBlob> p_ps_byte_code)
    -> CImage2DEffect&
{
    RecreatePsByteCodeRelatedResources(p_ps_byte_code);
    m_p_ps_byte_code = p_ps_byte_code;
    return *this;
}

auto CImage2DEffect::GetPsByteCode() noexcept
    -> Microsoft::WRL::ComPtr<ID3DBlob>
{
    return m_p_ps_byte_code;
}

auto CImage2DEffect::Draw() const
    -> CD3D10DrawCallWaiter
{
    DrawOnly();
    return CD3D10DrawCallWaiter{m_p_owner_device1};
}

void CImage2DEffect::DrawOnly() const noexcept
{
    constexpr auto index_count =
        static_cast<UINT>(D3DQuadrangle::VERTEX_INDEX_LIST.size());
    m_p_owner_device1->DrawIndexed(index_count, 0, 0);
}

auto CImage2DEffect::Clear(const std::array<FLOAT, 4>& color) const
    -> CD3D10DrawCallWaiter
{
    ClearOnly(color);
    return CD3D10DrawCallWaiter{m_p_owner_device1};
}

auto CImage2DEffect::ClearOnly(const std::array<FLOAT, 4>& color) const noexcept
    -> const CImage2DEffect&
{
    m_p_owner_device1->ClearRenderTargetView(m_p_render_target_view.Get(), color.data());
    return *this;
}
