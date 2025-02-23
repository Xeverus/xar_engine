#pragma once


namespace xar_engine::meta
{
    template <typename T>
    class Singleton
    {
    public:
        static T& get_instance()
        {
            static T instance;
            return instance;
        }
    };
}
