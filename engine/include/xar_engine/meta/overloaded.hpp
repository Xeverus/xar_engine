#pragma once


namespace xar_engine::meta
{
    template <typename... T>
    struct TOverloaded
        : public T ...
    {
        using T::operator()...;
    };

    template <typename... T>
    TOverloaded(T...) -> TOverloaded<T...>;
}