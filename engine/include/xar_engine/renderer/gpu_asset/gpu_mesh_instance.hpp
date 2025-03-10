#pragma once

#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>

#include <xar_engine/math/matrix.hpp>


namespace xar_engine::renderer::gpu_asset
{
    struct GpuMeshInstance
    {
        GpuMeshReference gpu_mesh;
        math::Matrix4x4f model_matrix;
    };
}