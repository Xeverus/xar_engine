#pragma once

#include <xar_engine/renderer/module/gpu_model_module.hpp>

#include <xar_engine/renderer/renderer_state.hpp>


namespace xar_engine::renderer::module
{
    class GpuModelModuleImpl
        : public IGpuModelModule
        , public SharedRendererState
    {
    public:
        using SharedRendererState::SharedRendererState;


        std::vector<gpu_asset::GpuModel> make_gpu_model(const std::vector<asset::Model>& model_list) override;
    };
}