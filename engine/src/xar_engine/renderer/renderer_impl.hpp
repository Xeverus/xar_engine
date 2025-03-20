#pragma once

#include <xar_engine/renderer/renderer.hpp>

#include <xar_engine/asset/image.hpp>

#include <xar_engine/renderer/module/gpu_model_module.hpp>

#include <xar_engine/renderer/renderer_state.hpp>


namespace xar_engine::renderer
{
    class RendererImpl
        : public IRenderer
        , public SharedRendererState
    {
    public:
        RendererImpl(
            std::shared_ptr<RendererState> state,
            std::unique_ptr<module::IGpuModelModule> gpu_model_module);

        ~RendererImpl() override;


        module::IGpuModelModule& gpu_model_module() override;


        gpu_asset::GpuMaterialReference make_gpu_material(const asset::Material& material) override;

        void add_gpu_mesh_instance_to_render(
            const gpu_asset::GpuMeshInstance& gpu_mesh_instance,
            const gpu_asset::GpuMaterialReference& gpu_material) override;
        void clear_gpu_mesh_instance_to_render() override;

        void update() override;

    private:
        void init_color_msaa();
        void init_depth();

        graphics::api::ImageReference init_texture(const asset::Image& image);

        void updateUniformBuffer(uint32_t currentImage);

    private:
        std::unique_ptr<module::IGpuModelModule> _gpu_model_module;
    };
}