#pragma once

#include <xar_engine/asset/material.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_material.hpp>


namespace xar_engine::renderer::unit
{
    class IGpuMaterialUnit
    {
    public:
        struct MakeGpuMaterialParameters;

    public:
        virtual ~IGpuMaterialUnit();

        virtual gpu_asset::GpuMaterialReference make_gpu_material(const MakeGpuMaterialParameters& parameters) = 0;
    };


    struct IGpuMaterialUnit::MakeGpuMaterialParameters
    {
        asset::Material material;
    };
}