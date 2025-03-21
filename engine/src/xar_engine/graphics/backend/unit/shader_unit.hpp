#pragma once

#include <xar_engine/graphics/api/shader_reference.hpp>


namespace xar_engine::graphics::backend::unit
{
    class IShaderUnit
    {
    public:
        struct MakeShaderParameters;

    public:
        virtual ~IShaderUnit();


        virtual api::ShaderReference make_shader(const MakeShaderParameters& parameters) = 0;
    };


    struct IShaderUnit::MakeShaderParameters
    {
        std::vector<char> shader_byte_code;
    };
}