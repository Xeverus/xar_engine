#pragma once

#include <xar_engine/algorithm/interval.hpp>

#include <set>


namespace xar_engine::algorithm
{
    template <typename T>
    class TIntervalCollection
    {
    public:
        using IntervalType = TInterval<T>;

    public:
        void add(IntervalType interval);
        bool contains(const T& value);

        std::vector<IntervalType> to_vector() const;

    private:
        struct IntervalComparator;

    private:
        using IntervalSet = std::set<IntervalType, IntervalComparator>;
        using IntervalRange = std::pair<typename IntervalSet::const_iterator, typename IntervalSet::const_iterator>;

    private:
        IntervalRange find_overlapping_interval_range(const IntervalType& interval) const;

    private:
        IntervalSet _interval_set;
    };


    template <typename T>
    struct TIntervalCollection<T>::IntervalComparator
    {
        bool operator()(const IntervalType& left, const IntervalType& right) const;
    };

    template <typename T>
    bool TIntervalCollection<T>::IntervalComparator::operator()(const IntervalType& left, const IntervalType& right) const
    {
        return left.begin < right.begin;
    }


    template <typename T>
    void TIntervalCollection<T>::add(IntervalType interval)
    {
        const auto range = find_overlapping_interval_range(interval);

        if (range.first != range.second)
        {
            interval = merge_intervals_t(interval, *range.first);
            interval = merge_intervals_t(interval, *std::prev(range.second));

            _interval_set.erase(
                range.first,
                range.second);
        }

        _interval_set.insert(interval);
    }

    template <typename T>
    bool TIntervalCollection<T>::contains(const T& value)
    {
        auto iter = _interval_set.upper_bound({value, {}});
        if (iter == _interval_set.begin())
        {
            return false;
        }

        --iter;
        return interval_contains_t(*iter, value);
    }

    template <typename T>
    std::vector<typename TIntervalCollection<T>::IntervalType> TIntervalCollection<T>::to_vector() const
    {
        std::vector<typename TIntervalCollection<T>::IntervalType> result;
        result.reserve(_interval_set.size());

        for (const auto& interval : _interval_set)
        {
            result.push_back(interval);
        }

        return result;
    }

    template <typename T>
    TIntervalCollection<T>::IntervalRange TIntervalCollection<T>::find_overlapping_interval_range(const IntervalType& interval) const
    {
        const auto range_end = _interval_set.upper_bound({interval.end, {}});

        auto range_begin = range_end;
        for (auto iter = _interval_set.begin(); iter != _interval_set.end(); ++iter)
        {
            if (interval_overlap_t(interval, *iter))
            {
                range_begin = iter;
                break;
            }
        }

        return {range_begin, range_end};
    }
}