#pragma once

#include <xar_engine/renderer/renderer.hpp>

#include <xar_engine/asset/image.hpp>

#include <xar_engine/renderer/unit/gpu_model_unit.hpp>

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
            std::unique_ptr<unit::IGpuMaterialUnit> gpu_material_unit,
            std::unique_ptr<unit::IGpuModelUnit> gpu_model_unit);

        ~RendererImpl() override;


        unit::IGpuModelUnit& gpu_model_unit() override;
        unit::IGpuMaterialUnit& gpu_material_unit() override;

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
        std::unique_ptr<unit::IGpuMaterialUnit> _gpu_material_unit;
        std::unique_ptr<unit::IGpuModelUnit> _gpu_model_unit;
    };
}