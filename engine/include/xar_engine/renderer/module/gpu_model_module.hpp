#pragma once

#include <vector>

#include <xar_engine/asset/model.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>


namespace xar_engine::renderer::module
{
    class IGpuModelModule
    {
    public:
        virtual ~IGpuModelModule();

        virtual std::vector<gpu_asset::GpuModel> make_gpu_model(const std::vector<asset::Model>& model_list) = 0;
    };
}