#pragma once

#include <xar_engine/renderer/unit/gpu_model_unit.hpp>

#include <xar_engine/renderer/renderer_state.hpp>


namespace xar_engine::renderer::unit
{
    class GpuModelUnitImpl
        : public IGpuModelUnit
        , public SharedRendererState
    {
    public:
        using SharedRendererState::SharedRendererState;

        std::vector<gpu_asset::GpuModel> make_gpu_model(const MakeGpuModelParameters& parameters) override;
    };
}