#pragma once

#include <vector>

#include <xar_engine/asset/model.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>


namespace xar_engine::renderer::module
{
    class IGpuModelModule
    {
    public:
        struct MakeGpuModelParameters;

    public:
        virtual ~IGpuModelModule();

        virtual std::vector<gpu_asset::GpuModel> make_gpu_model(const MakeGpuModelParameters& parameters) = 0;
    };


    struct IGpuModelModule::MakeGpuModelParameters
    {
        std::vector<asset::Model> model_list;
    };
}