#pragma once

#include <xar_engine/graphics/api/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class ShaderTag;
    using ShaderReference = TResourceReference<ShaderTag>;


    enum class EShaderType
    {
        VERTEX,
        FRAGMENT,
    };
}