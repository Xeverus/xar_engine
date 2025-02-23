#include <xar_engine/meta/ref_counting_singleton.hpp>

#include <thread>


namespace xar_engine::meta
{
    std::shared_ptr<void*> RefCountedSingleton::get_instance(const std::function<std::shared_ptr<void*>()>& maker_function)
    {
        static std::weak_ptr<void*> _instance;
        static std::mutex _mutex;

        std::lock_guard<std::mutex> guard{_mutex};
        auto instance = _instance.lock();
        if (instance == nullptr)
        {
            instance = std::shared_ptr<void*>(maker_function());
            _instance = instance;
        }

        return instance;
    }
}
