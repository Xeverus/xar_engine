#pragma once


namespace xar_engine::graphics
{
    class IRenderer
    {
    public:
        virtual ~IRenderer();

        virtual void update() = 0;
    };
}