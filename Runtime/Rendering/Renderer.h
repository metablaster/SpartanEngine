/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ========================
#include <unordered_map>
#include <array>
#include <atomic>
#include "Renderer_ConstantBuffers.h"
#include "Material.h"
#include "../Core/ISubsystem.h"
#include "../Math/Rectangle.h"
#include "../RHI/RHI_Definition.h"
#include "../RHI/RHI_Viewport.h"
#include "../RHI/RHI_Vertex.h"
//===================================

namespace Spartan
{
    // Forward declarations
    class Entity;
    class Camera;
    class Light;
    class ResourceCache;
    class Font;
    class Variant;
    class Grid;
    class Transform_Gizmo;
    class Profiler;

    namespace Math
    {
        class BoundingBox;
        class Frustum;
    }

    enum Renderer_Option : uint64_t
    {
        Render_Debug_Aabb               = 1 << 0,
        Render_Debug_PickingRay         = 1 << 1,
        Render_Debug_Grid               = 1 << 2,
        Render_Debug_Transform          = 1 << 3,
        Render_Debug_SelectionOutline   = 1 << 4,
        Render_Debug_Lights             = 1 << 5,
        Render_Debug_PerformanceMetrics = 1 << 6,
        Render_Debug_Physics            = 1 << 7,
        Render_Debug_Wireframe          = 1 << 8,
        Render_Bloom                    = 1 << 9,    
        Render_VolumetricLighting       = 1 << 10,
        Render_AntiAliasing_Taa         = 1 << 11,
        Render_AntiAliasing_Fxaa        = 1 << 12,
        Render_Hbao                     = 1 << 13,
        Render_Ssgi                     = 1 << 14,
        Render_ScreenSpaceShadows       = 1 << 15,
        Render_ScreenSpaceReflections   = 1 << 16,
        Render_MotionBlur               = 1 << 17,
        Render_DepthOfField             = 1 << 18,
        Render_FilmGrain                = 1 << 19,
        Render_Sharpening_LumaSharpen   = 1 << 20,
        Render_ChromaticAberration      = 1 << 21,
        Render_Dithering                = 1 << 22,
        Render_ReverseZ                 = 1 << 23,
        Render_DepthPrepass             = 1 << 24
    };

    enum Renderer_Option_Value
    {
        Option_Value_Anisotropy,
        Option_Value_ShadowResolution,
        Option_Value_Tonemapping,
        Option_Value_Gamma,
        Option_Value_Bloom_Intensity,
        Option_Value_Sharpen_Strength,
        Option_Value_Fog
    };

    enum Renderer_ToneMapping_Type
    {
        Renderer_ToneMapping_Off,
        Renderer_ToneMapping_ACES,
        Renderer_ToneMapping_Reinhard,
        Renderer_ToneMapping_Uncharted2
    };

    enum Renderer_Object_Type
    {
        Renderer_Object_Opaque,
        Renderer_Object_Transparent,
        Renderer_Object_Light,
        Renderer_Object_Camera
    };

    enum class RendererShader
    {
        Gbuffer_V,
        Gbuffer_P,
        Depth_V,
        Depth_P,
        Quad_V,
        Texture_P,
        Copy_C,
        Fxaa_C,
        Fxaa_Luminance_C,
        FilmGrain_C,
        Taa_C,
        MotionBlur_C,
        Dof_DownsampleCoc_C,
        Dof_Bokeh_C,
        Dof_Tent_C,
        Dof_UpscaleBlend_C,
        Sharpening_C,
        ChromaticAberration_C,    
        BloomDownsampleLuminance_C,
        BloomDownsample_C,
        BloomUpsampleBlendFrame_C,
        BloomUpsampleBlendMip_C,
        ToneMapping_C,
        GammaCorrection_C,
        Dithering_C,
        DebugNormal_C,
        DebugVelocity_C,
        DebugChannelR_C,
        DebugChannelA_C,
        DebugChannelRgbGammaCorrect_C,
        BrdfSpecularLut_C,
        Light_C,
        Composition_P,
        Composition_Transparent_P,
        Color_V,
        Color_P,
        Font_V,
        Font_P,
        Hbao_C,
        Ssgi_C,
        Ssr_C,
        Entity_V,
        Entity_Transform_P,
        BlurBox_P,
        BlurGaussian_P,
        BlurGaussianBilateral_P,
        Entity_Outline_P
    };

    enum class RendererRt : uint64_t
    {
        Gbuffer_Albedo                  = 1 << 0,
        Gbuffer_Normal                  = 1 << 1,
        Gbuffer_Material                = 1 << 2,
        Gbuffer_Velocity                = 1 << 3,
        Gbuffer_Depth                   = 1 << 4,
        Brdf_Prefiltered_Environment    = 1 << 5,
        Brdf_Specular_Lut               = 1 << 6,
        Light_Diffuse                   = 1 << 7,
        Light_Diffuse_Transparent       = 1 << 8,
        Light_Specular                  = 1 << 9,
        Light_Specular_Transparent      = 1 << 10,
        Light_Volumetric                = 1 << 11,
        Frame_Hdr                       = 1 << 12,
        Frame_Ldr                       = 1 << 13,
        Frame_Hdr_2                     = 1 << 14,
        Frame_Ldr_2                     = 1 << 15,
        Dof_Half                        = 1 << 16,
        Dof_Half_2                      = 1 << 17,
        Bloom                           = 1 << 18,
        Hbao                            = 1 << 19,
        Hbao_Blurred                    = 1 << 20,
        Ssgi                            = 1 << 21,
        Ssr                             = 1 << 22,
        Accumulation_Taa                = 1 << 23,
        Accumulation_Ssgi               = 1 << 24,
    };

    class SPARTAN_CLASS Renderer : public ISubsystem
    {
    public:
        // Constants
        const uint32_t m_resolution_shadow_min  = 128;
        const float m_gizmo_size_max            = 2.0f;
        const float m_gizmo_size_min            = 0.1f;
        const float m_thread_group_count        = 8.0f;
        const float m_depth_bias                = 0.003f; // bias that's applied directly into the depth buffer
        const float m_depth_bias_clamp          = 0.0f;
        const float m_depth_bias_slope_scaled   = 2.0f;
        #define DEBUG_COLOR                     Math::Vector4(0.41f, 0.86f, 1.0f, 1.0f)

        Renderer(Context* context);
        ~Renderer();

        //= ISubsystem ======================
        bool Initialize() override;
        void Tick(float delta_time) override;
        //===================================

        void DrawLine(const Math::Vector3& from, const Math::Vector3& to, const Math::Vector4& color_from = DEBUG_COLOR, const Math::Vector4& color_to = DEBUG_COLOR, bool depth = true);
        void DrawRectangle(const Math::Rectangle& rectangle, const Math::Vector4& color = DEBUG_COLOR, bool depth = true);
        void DrawBox(const Math::BoundingBox& box, const Math::Vector4& color = DEBUG_COLOR, bool depth = true);

        // Viewport
        const RHI_Viewport& GetViewport()           const { return m_viewport; }
        const Math::Vector2& GetViewportOffset()    const { return m_viewport_editor_offset; }
        void SetViewport(float width, float height, float offset_x = 0, float offset_y = 0);

        // Resolution
        const Math::Vector2& GetResolution() const { return m_resolution; }
        void SetResolution(uint32_t width, uint32_t height);

        // Editor
        float m_gizmo_transform_size    = 0.015f;
        float m_gizmo_transform_speed   = 12.0f;
        std::weak_ptr<Entity> SnapTransformGizmoTo(const std::shared_ptr<Entity>& entity) const;

        // Debug/Visualise a render target
        void SetRenderTargetDebug(const uint64_t render_target_debug)   { m_render_target_debug = render_target_debug; }
        auto GetRenderTargetDebug() const                               { return m_render_target_debug; }

        // Depth
        auto GetClearDepth()                { return GetOption(Render_ReverseZ) ? m_viewport.depth_min : m_viewport.depth_max; }
        auto GetComparisonFunction() const  { return GetOption(Render_ReverseZ) ? RHI_Comparison_GreaterEqual : RHI_Comparison_LessEqual; }

        // Environment
        const std::shared_ptr<RHI_Texture>& GetEnvironmentTexture();
        void SetEnvironmentTexture(const std::shared_ptr<RHI_Texture>& texture);

        // Options
        uint64_t GetOptions()                           const { return m_options; }
        void SetOptions(const uint64_t options)               { m_options = options; }
        bool GetOption(const Renderer_Option option)    const { return m_options & option; }
        void SetOption(Renderer_Option option, bool enable);
        
        // Options values
        template<typename T>
        T GetOptionValue(const Renderer_Option_Value option) { return static_cast<T>(m_option_values[option]); }
        void SetOptionValue(Renderer_Option_Value option, float value);

        // Swapchain
        RHI_SwapChain* GetSwapChain() const { return m_swap_chain.get(); }
        bool Present();
        bool Flush();

        // Default textures
        RHI_Texture* GetDefaultTextureWhite()       const { return m_default_tex_white.get(); }
        RHI_Texture* GetDefaultTextureBlack()       const { return m_default_tex_black.get(); }
        RHI_Texture* GetDefaultTextureTransparent() const { return m_default_tex_transparent.get(); }

        // Global shader resources
        void SetGlobalShaderObjectTransform(RHI_CommandList* cmd_list, const Math::Matrix& transform);
        void SetGlobalSamplersAndConstantBuffers(RHI_CommandList* cmd_list) const;

        // Misc
        const std::shared_ptr<RHI_Device>& GetRhiDevice()   const { return m_rhi_device; } 
        RHI_PipelineCache* GetPipelineCache()               const { return m_pipeline_cache.get(); }
        RHI_DescriptorCache* GetDescriptorCache()           const { return m_descriptor_cache.get(); }
        RHI_Texture* GetFrameTexture()                      const { return m_render_targets.at(RendererRt::Frame_Ldr).get(); }
        auto GetFrameNum()                                  const { return m_frame_num; }
        const auto& GetCamera()                             const { return m_camera; }
        auto IsInitialized()                                const { return m_initialized; }
        auto& GetShaders()                                  const { return m_shaders; }
        bool IsRendering()                                  const { return m_is_rendering; }
        uint32_t GetMaxResolution() const;

    private:
        // Resource creation
        void CreateConstantBuffers();
        void CreateDepthStencilStates();
        void CreateRasterizerStates();
        void CreateBlendStates();
        void CreateFonts();
        void CreateTextures();
        void CreateShaders();
        void CreateSamplers();
        void CreateRenderTextures();

        // Passes
        void Pass_Main(RHI_CommandList* cmd_list);
        void Pass_LightDepth(RHI_CommandList* cmd_list, const Renderer_Object_Type object_type);
        void Pass_DepthPrePass(RHI_CommandList* cmd_list);
        void Pass_GBuffer(RHI_CommandList* cmd_list, const bool is_transparent_pass = false);
        void Pass_Ssgi(RHI_CommandList* cmd_list);
        void Pass_Hbao(RHI_CommandList* cmd_list);
        void Pass_Ssr(RHI_CommandList* cmd_list);
        void Pass_Light(RHI_CommandList* cmd_list, const bool is_transparent_pass = false);
        void Pass_Composition(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_out, const bool is_transparent_pass = false);
        void Pass_PostProcess(RHI_CommandList* cmd_list);
        void Pass_TemporalAntialiasing(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        bool Pass_DebugBuffer(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_ToneMapping(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_GammaCorrection(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_FXAA(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in,    std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_FilmGrain(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_Sharpening(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_ChromaticAberration(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_MotionBlur(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_DepthOfField(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_Dithering(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_Bloom(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_BlurBox(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out, const float sigma, const float pixel_stride, const bool use_stencil);
        void Pass_BlurGaussian(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out, const float sigma, const float pixel_stride = 1.0f);
        void Pass_BlurBilateralGaussian(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_in, std::shared_ptr<RHI_Texture>& tex_out, const float sigma, const float pixel_stride = 1.0f, const bool use_stencil = false);
        void Pass_Lines(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_Outline(RHI_CommandList* cmd_list, std::shared_ptr<RHI_Texture>& tex_out);
        void Pass_Icons(RHI_CommandList* cmd_list, RHI_Texture* tex_out);
        void Pass_TransformHandle(RHI_CommandList* cmd_list, RHI_Texture* tex_out);
        void Pass_Text(RHI_CommandList* cmd_list, RHI_Texture* tex_out);
        void Pass_BrdfSpecularLut(RHI_CommandList* cmd_list);
        void Pass_Copy(RHI_CommandList* cmd_list, RHI_Texture* tex_in, RHI_Texture* tex_out);

        // Constant buffers
        bool UpdateFrameBuffer(RHI_CommandList* cmd_list);
        bool UpdateMaterialBuffer(RHI_CommandList* cmd_list);
        bool UpdateUberBuffer(RHI_CommandList* cmd_list);
        bool UpdateObjectBuffer(RHI_CommandList* cmd_list);
        bool UpdateLightBuffer(RHI_CommandList* cmd_list, const Light* light);

        // Misc
        void RenderablesAcquire(const Variant& renderables);
        void RenderablesSort(std::vector<Entity*>* renderables);
        void ClearEntities();

        // Render textures
        std::unordered_map<RendererRt, std::shared_ptr<RHI_Texture>> m_render_targets;
        std::vector<std::shared_ptr<RHI_Texture>> m_render_tex_bloom;

        // Standard textures
        std::shared_ptr<RHI_Texture> m_default_tex_white;
        std::shared_ptr<RHI_Texture> m_default_tex_black;
        std::shared_ptr<RHI_Texture> m_default_tex_transparent;
        std::shared_ptr<RHI_Texture> m_gizmo_tex_light_directional;
        std::shared_ptr<RHI_Texture> m_gizmo_tex_light_point;
        std::shared_ptr<RHI_Texture> m_gizmo_tex_light_spot;

        // Shaders
        std::unordered_map<RendererShader, std::shared_ptr<RHI_Shader>> m_shaders;

        // Depth-stencil states
        std::shared_ptr<RHI_DepthStencilState> m_depth_stencil_off_off;
        std::shared_ptr<RHI_DepthStencilState> m_depth_stencil_off_on_r;
        std::shared_ptr<RHI_DepthStencilState> m_depth_stencil_on_off_w;
        std::shared_ptr<RHI_DepthStencilState> m_depth_stencil_on_off_r;
        std::shared_ptr<RHI_DepthStencilState> m_depth_stencil_on_on_w;

        // Blend states 
        std::shared_ptr<RHI_BlendState> m_blend_disabled;
        std::shared_ptr<RHI_BlendState> m_blend_alpha;
        std::shared_ptr<RHI_BlendState> m_blend_additive;

        // Rasterizer states
        std::shared_ptr<RHI_RasterizerState> m_rasterizer_cull_back_solid;
        std::shared_ptr<RHI_RasterizerState> m_rasterizer_cull_back_wireframe;
        std::shared_ptr<RHI_RasterizerState> m_rasterizer_light_point_spot;
        std::shared_ptr<RHI_RasterizerState> m_rasterizer_light_directional;

        // Samplers
        std::shared_ptr<RHI_Sampler> m_sampler_compare_depth;
        std::shared_ptr<RHI_Sampler> m_sampler_point_clamp;
        std::shared_ptr<RHI_Sampler> m_sampler_bilinear_clamp;
        std::shared_ptr<RHI_Sampler> m_sampler_bilinear_wrap;
        std::shared_ptr<RHI_Sampler> m_sampler_trilinear_clamp;
        std::shared_ptr<RHI_Sampler> m_sampler_anisotropic_wrap;

        // Line rendering
        std::shared_ptr<RHI_VertexBuffer> m_vertex_buffer_lines;
        std::vector<RHI_Vertex_PosCol> m_lines_list_depth_enabled;
        std::vector<RHI_Vertex_PosCol> m_lines_list_depth_disabled;

        // Gizmos
        std::unique_ptr<Transform_Gizmo> m_gizmo_transform;
        std::unique_ptr<Grid> m_gizmo_grid;
        Math::Rectangle m_gizmo_light_rect;

        // Resolution & Viewport
        Math::Vector2 m_resolution              = Math::Vector2::Zero;
        RHI_Viewport m_viewport                 = RHI_Viewport(0, 0, 1920, 1080);
        Math::Vector2 m_viewport_editor_offset  = Math::Vector2::Zero;

        // Options
        uint64_t m_options = 0;
        std::unordered_map<Renderer_Option_Value, float> m_option_values;

        // Misc
        Math::Rectangle m_viewport_quad;
        std::unique_ptr<Font> m_font;
        Math::Vector2 m_taa_jitter          = Math::Vector2::Zero;
        Math::Vector2 m_taa_jitter_previous = Math::Vector2::Zero;
        uint64_t m_render_target_debug      = 0;
        bool m_initialized                  = false;
        float m_near_plane                  = 0.0f;
        float m_far_plane                   = 0.0f;
        uint64_t m_frame_num                = 0;
        bool m_is_odd_frame                 = false;
        std::atomic<bool> m_is_rendering    = false;
        bool m_brdf_specular_lut_rendered   = false;
        bool m_update_ortho_proj            = true;

        // RHI Core
        std::shared_ptr<RHI_Device> m_rhi_device;
        std::shared_ptr<RHI_PipelineCache> m_pipeline_cache;
        std::shared_ptr<RHI_DescriptorCache> m_descriptor_cache;

        // Swapchain
        static const uint8_t m_swap_chain_buffer_count = 3;
        std::shared_ptr<RHI_SwapChain> m_swap_chain;

        //= CONSTANT BUFFERS =====================================
        BufferFrame m_buffer_frame_cpu;
        BufferFrame m_buffer_frame_cpu_previous;
        std::shared_ptr<RHI_ConstantBuffer> m_buffer_frame_gpu;
        uint32_t m_buffer_frame_offset_index = 0;

        BufferMaterial m_buffer_material_cpu;
        BufferMaterial m_buffer_material_cpu_previous;
        std::shared_ptr<RHI_ConstantBuffer> m_buffer_material_gpu;
        uint32_t m_buffer_material_offset_index = 0;

        BufferUber m_buffer_uber_cpu;
        BufferUber m_buffer_uber_cpu_previous;
        std::shared_ptr<RHI_ConstantBuffer> m_buffer_uber_gpu;
        uint32_t m_buffer_uber_offset_index = 0;

        BufferObject m_buffer_object_cpu;
        BufferObject m_buffer_object_cpu_previous;
        std::shared_ptr<RHI_ConstantBuffer> m_buffer_object_gpu;
        uint32_t m_buffer_object_offset_index = 0;

        BufferLight m_buffer_light_cpu;
        BufferLight m_buffer_light_cpu_previous;
        std::shared_ptr<RHI_ConstantBuffer> m_buffer_light_gpu;
        uint32_t m_buffer_light_offset_index = 0;
        //========================================================

        // Entities and material references
        std::unordered_map<Renderer_Object_Type, std::vector<Entity*>> m_entities;
        std::array<Material*, m_max_material_instances> m_material_instances;    
        std::shared_ptr<Camera> m_camera;

        // Dependencies
        Profiler* m_profiler            = nullptr;
        ResourceCache* m_resource_cache = nullptr;
    };
}
