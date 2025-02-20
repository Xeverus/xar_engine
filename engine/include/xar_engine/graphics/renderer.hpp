#pragma once


namespace xar_engine::graphics
{
    class IRenderer
    {
    public:
        virtual ~IRenderer();

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void shutdown() = 0;
    };
}