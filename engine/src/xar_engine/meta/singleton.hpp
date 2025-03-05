#pragma once


namespace xar_engine::meta
{
    class TSingleton
    {
    public:
        template <typename T>
        static T& get_instance();
    };


    template <typename T>
    T& TSingleton::get_instance()
    {
        static T instance;
        return instance;
    }
}
