#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class ShaderTag;
    using ShaderReference = meta::TResourceReference<ShaderTag>;


    enum class EShaderType
    {
        VERTEX,
        FRAGMENT,
    };
}