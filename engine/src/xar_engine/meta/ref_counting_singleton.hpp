#pragma once

#include <functional>
#include <memory>


namespace xar_engine::meta
{
    class RefCountedSingleton
    {
    public:
        template <typename T>
        static std::shared_ptr<T> get_instance();

    private:
        static std::shared_ptr<void*> get_instance(const std::function<std::shared_ptr<void*>()>& maker_function);
    };


    template <typename T>
    std::shared_ptr<T> RefCountedSingleton::get_instance()
    {
        static const auto maker = []()
        {
            return std::reinterpret_pointer_cast<void*>(std::make_shared<T>());
        };

        auto instance = get_instance(maker);
        return std::reinterpret_pointer_cast<T>(instance);
    }
}
