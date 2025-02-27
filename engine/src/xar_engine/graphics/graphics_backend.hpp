#pragma once

#include <vector>


namespace xar_engine::graphics
{
    class IGraphicsBackend
    {
    public:
        virtual ~IGraphicsBackend();


        virtual void make_shader_object(const std::vector<char>& shader_byte_code) = 0;
    };
}