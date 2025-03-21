#pragma once

#include <xar_engine/asset/image.hpp>

#include <xar_engine/renderer/renderer_state.hpp>

#include <xar_engine/renderer/unit/gpu_material_unit.hpp>


namespace xar_engine::renderer::unit
{
    class GpuMaterialUnitImpl
        : public IGpuMaterialUnit
          , public SharedRendererState
    {
    public:
        using SharedRendererState::SharedRendererState;

        gpu_asset::GpuMaterialReference make_gpu_material(const MakeGpuMaterialParameters& parameters) override;

    private:
        graphics::api::ImageReference init_texture(const asset::Image& image);
    };
}