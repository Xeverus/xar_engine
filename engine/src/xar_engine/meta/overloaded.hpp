#pragma once

namespace xar_engine::meta
{
    template <typename... T>
    struct Overloaded : public T...
    {
        using T::operator()...;
    };

    template <typename... T>
    Overloaded(T...) -> Overloaded<T...>;
}