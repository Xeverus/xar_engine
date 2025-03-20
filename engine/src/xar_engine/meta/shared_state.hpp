#pragma once

#include <memory>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::meta
{
    template <typename StateT>
    class TSharedState
    {
    public:
        using StateType = StateT;

    public:
        explicit TSharedState(std::shared_ptr<StateT> state);
        virtual ~TSharedState();

    protected:
        [[nodiscard]]
        const StateT& get_state() const;

        [[nodiscard]]
        StateT& get_state();

    private:
        std::shared_ptr<StateT> _state;
    };


    template <typename StateT>
    TSharedState<StateT>::TSharedState(std::shared_ptr<StateT> state)
        : _state(std::move(state))
    {
        XAR_THROW_IF(
            _state == nullptr,
            error::XarException,
            "Shared state cannot be null");
    }

    template <typename StateT>
    TSharedState<StateT>::~TSharedState() = default;

    template <typename StateT>
    const StateT& TSharedState<StateT>::get_state() const
    {
        return *_state;
    }

    template <typename StateT>
    StateT& TSharedState<StateT>::get_state()
    {
        return *_state;
    }
}