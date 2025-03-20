#pragma once

#include <xar_engine/graphics/api/shader_reference.hpp>


namespace xar_engine::graphics::backend::component
{
    class IShaderComponent
    {
    public:
        struct MakeShaderParameters;

    public:
        virtual ~IShaderComponent();


        virtual api::ShaderReference make_shader(const MakeShaderParameters& parameters) = 0;
    };


    struct IShaderComponent::MakeShaderParameters
    {
        std::vector<char> shader_byte_code;
    };
}