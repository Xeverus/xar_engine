#pragma once

#include <cstdint>
#include <functional>
#include <memory>


namespace xar_engine::meta
{
    template <typename Tag>
    class TResourceReference
    {
    public:
        using Id = std::uint32_t;
        using Deleter = std::function<void()>;

    public:
        TResourceReference();
        TResourceReference(
            Id id,
            Deleter deleter);


        [[nodiscard]]
        Id get_id() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };


    template <typename Tag>
    struct TResourceReference<Tag>::State
    {
    public:
        State(Id id,
              Deleter deleter);

        ~State();

    public:
        Id id;
        Deleter deleter;
    };

    template <typename Tag>
    TResourceReference<Tag>::State::State(
        const Id id,
        Deleter deleter)
        : id(id)
        , deleter(std::move(deleter))
    {
    }

    template <typename Tag>
    TResourceReference<Tag>::State::~State()
    {
        if (deleter)
        {
            deleter();
        }
    }


    template <typename Tag>
    TResourceReference<Tag>::TResourceReference()
        : _state(nullptr)
    {
    }

    template <typename Tag>
    TResourceReference<Tag>::TResourceReference(
        const Id id,
        Deleter deleter)
        : _state(std::make_shared<State>(id, std::move(deleter)))
    {
    }

    template <typename Tag>
    TResourceReference<Tag>::Id TResourceReference<Tag>::get_id() const
    {
        return _state->id;
    }
}