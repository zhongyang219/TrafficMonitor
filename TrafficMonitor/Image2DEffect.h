#pragma once
#include <cstddef>
#include <array>
#include "D3D10Support1.h"

namespace D3DQuadrangle
{
    extern const std::array<std::uint16_t, 6> VERTEX_INDEX_LIST;

    const CShader& GetVsShader();

    /**
     * @brief 由三角形012和三角形132组成的四边形，顶点序号见VERTEX_INDEX_LIST
     *
     * @tparam Vertex 顶点类型
     */
    template <class Vertex>
    struct QuadrangleVertexs
    {
        /**
         * @brief 由三角形012和三角形123组成的四边形，下面是顶点排列顺序： \n
         * 1·--------------·2 \n
         *  |              |  \n
         *  |              |  \n
         * 0·--------------·3
         */
        std::array<Vertex, 4> vertexs{};

        Vertex& GetLeftTopVertex() noexcept
        {
            return vertexs[1];
        }
        Vertex& GetRightTopVertex() noexcept
        {
            return vertexs[2];
        }
        Vertex& GetLeftBottomVertex() noexcept
        {
            return vertexs[0];
        }
        Vertex& GetRightBottomVertex() noexcept
        {
            return vertexs[3];
        }
        Vertex* GetData() noexcept
        {
            return vertexs.data();
        }
        static constexpr std::size_t GetSize() noexcept
        {
            return sizeof(vertexs);
        }
        static constexpr std::size_t GetStride() noexcept
        {
            return sizeof(Vertex);
        }
    };
}

struct Image2DVertex
{
    float position[4];
    float texture0[2];
};

/**
 * @brief 默认vertex shader的输入输出定义 \n
 struct VsInput{                          \n
   float4 position : POSITION;            \n
   float2 texcoord : TEXCOORD;            \n
};                                        \n
                                          \n
struct VsOutput{                          \n
   float4 position : SV_POSITION;         \n
   float2 texcoord : TEXCOORD;            \n
};
 *
 */
class CImage2DEffect
{
private:
    Microsoft::WRL::ComPtr<ID3D10Device1> m_p_owner_device1{};
    Microsoft::WRL::ComPtr<ID3DBlob> m_p_vs_byte_code{D3DQuadrangle::GetVsShader().Compile()};
    Microsoft::WRL::ComPtr<ID3DBlob> m_p_ps_byte_code{};
    Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_input{};
    Microsoft::WRL::ComPtr<ID3D10Texture2D> m_p_output{};
    //内部使用
    Microsoft::WRL::ComPtr<ID3D10InputLayout> m_p_input_layout{};
    Microsoft::WRL::ComPtr<ID3D10Buffer> m_p_vertex_buffer{};
    Microsoft::WRL::ComPtr<ID3D10Buffer> m_p_index_buffer{};
    Microsoft::WRL::ComPtr<ID3D10VertexShader> m_p_vs{};
    Microsoft::WRL::ComPtr<ID3D10RasterizerState> m_p_rasterizer_state{};
    Microsoft::WRL::ComPtr<ID3D10BlendState1> m_p_blend_state1{};
    Microsoft::WRL::ComPtr<ID3D10DepthStencilState> m_p_depth_stencil_state{};
    UINT m_output_width{};
    UINT m_output_height{};
    Microsoft::WRL::ComPtr<ID3D10ShaderResourceView> m_p_ps_tex0_resource_view{};
    Microsoft::WRL::ComPtr<ID3D10SamplerState> m_p_ps_tex0_sampler_state{};
    Microsoft::WRL::ComPtr<ID3D10PixelShader> m_p_ps{};
    Microsoft::WRL::ComPtr<ID3D10RenderTargetView> m_p_render_target_view{};

protected:
    void RecreateUnmodifiableResource(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1);
    void RecreateInputTextureRelatedResources(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_input);
    void RecreateOutputTextureRelatedResources(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_output);
    void RecreateVsByteCodeRelatedResources(Microsoft::WRL::ComPtr<ID3DBlob> p_vs_byte_code);
    void RecreatePsByteCodeRelatedResources(Microsoft::WRL::ComPtr<ID3DBlob> p_ps_byte_code);

public:
    constexpr static auto PIXEL_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM;
    constexpr static UINT SLOT = 0;
    using QuadrangleVertexs = D3DQuadrangle::QuadrangleVertexs<Image2DVertex>;

    CImage2DEffect(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1);
    ~CImage2DEffect() = default;

    auto RebindDevice1(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1)
        -> CImage2DEffect&;

    auto SetOutputSize(UINT width, UINT height) noexcept
        -> CImage2DEffect&;
    UINT GetOutputWidth() const noexcept;
    UINT GetOutputHeight() const noexcept;

    auto SetInputTexture(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_input)
        -> CImage2DEffect&;
    auto GetInputTexture() noexcept
        -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;

    auto SetOutputTexture(Microsoft::WRL::ComPtr<ID3D10Texture2D> p_output)
        -> CImage2DEffect&;
    auto GetOutputTexture() noexcept
        -> Microsoft::WRL::ComPtr<ID3D10Texture2D>;

    auto SetVsByteCode(Microsoft::WRL::ComPtr<ID3DBlob> p_vs_byte_code)
        -> CImage2DEffect&;
    auto GetVsByteCode() noexcept
        -> Microsoft::WRL::ComPtr<ID3DBlob>;

    auto SetPsByteCode(Microsoft::WRL::ComPtr<ID3DBlob> p_ps_byte_code)
        -> CImage2DEffect&;
    auto GetPsByteCode() noexcept
        -> Microsoft::WRL::ComPtr<ID3DBlob>;

    auto ApplyPipelineConfig() const
        -> const CImage2DEffect&;
    [[nodiscard]] auto Draw() const
        -> CD3D10DrawCallWaiter;
    void DrawOnly() const noexcept;
    [[nodiscard]] auto Clear(const std::array<FLOAT, 4>& color) const
        -> CD3D10DrawCallWaiter;
    auto ClearOnly(const std::array<FLOAT, 4>& color) const noexcept
        -> const CImage2DEffect&;
};
#define CIMAGE2DEFFECT_SHADER_VS_INPUT_DECLARATION \
    "struct VsInput"                               \
    "{"                                            \
    "   float4 position : POSITION;"               \
    "   float2 texcoord : TEXCOORD;"               \
    "};"

#define CIMAGE2DEFFECT_SHADER_VS_OUTPUT_DECLARATION \
    "struct VsOutput"                               \
    "{"                                             \
    "   float4 position : SV_POSITION;"             \
    "   float2 texcoord : TEXCOORD;"                \
    "};"
