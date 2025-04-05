#pragma once


namespace xar_engine::algorithm
{
    template <typename T>
    struct TInterval
    {
        T begin;
        T end;
    };


    template <typename T>
    bool interval_contains_t(
        const TInterval<T>& interval,
        const T& value)
    {
        return interval.begin <= value && value <= interval.end;
    }

    template <typename T>
    bool interval_overlap_t(
        const TInterval<T>& left,
        const TInterval<T>& right)
    {
        return interval_contains_t(
            left,
            right.begin) || interval_contains_t(
            right,
            left.begin);
    }

    template <typename T>
    TInterval<T> merge_intervals_t(
        const TInterval<T>& left,
        const TInterval<T>& right)
    {
        return {
            std::min(
                left.begin,
                right.begin),
            std::max(
                left.end,
                right.end)
        };
    }
}