#pragma once


namespace xar_engine::meta
{
    class Singleton
    {
    public:
        template <typename T>
        static T& get_instance();
    };


    template <typename T>
    T& Singleton::get_instance()
    {
        static T instance;
        return instance;
    }
}
