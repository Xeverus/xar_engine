#pragma once

#include <xar_engine/asset/material.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_material.hpp>


namespace xar_engine::renderer::module
{
    class IGpuMaterialModule
    {
    public:
        struct MakeGpuMaterialParameters;

    public:
        virtual ~IGpuMaterialModule();

        virtual gpu_asset::GpuMaterialReference make_gpu_material(const MakeGpuMaterialParameters& parameters) = 0;
    };


    struct IGpuMaterialModule::MakeGpuMaterialParameters
    {
        asset::Material material;
    };
}