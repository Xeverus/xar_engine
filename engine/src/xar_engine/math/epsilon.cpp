#include <xar_engine/math/epsilon.hpp>

#include <cmath>
#include <limits>


namespace xar_engine::math::epsilon
{
    namespace
    {
        template <typename T>
        T get_epsilon()
        {
            return std::numeric_limits<T>::epsilon();
        }

        template <typename T>
        bool equal_impl(
            const T left,
            const T right)
        {
            return std::fabs(left - right) <= get_epsilon<T>();
        }

        template <typename T>
        bool not_equal_impl(
            const T left,
            const T right)
        {
            return !equal_impl(
                left,
                right);
        }

        template <typename T>
        bool less_impl(
            const T left,
            const T right)
        {
            return not_equal_impl(
                left,
                right) && left < right;
        }

        template <typename T>
        bool less_or_equal_impl(
            const T left,
            const T right)
        {
            return equal_impl(
                left,
                right) || left < right;
        }

        template <typename T>
        bool greater_impl(
            const T left,
            const T right)
        {
            return not_equal_impl(
                left,
                right) && left > right;
        }

        template <typename T>
        bool greater_or_equal_impl(
            const T left,
            const T right)
        {
            return equal_impl(
                left,
                right) || left > right;
        }
    }

    float get_epsilon(float)
    {
        return get_epsilon<float>();
    }

    bool equal(
        const float left,
        const float right)
    {
        return equal_impl(
            left,
            right);
    }

    bool not_equal(
        const float left,
        const float right)
    {
        return not_equal_impl(
            left,
            right);
    }

    bool less(
        const float left,
        const float right)
    {
        return less_impl(
            left,
            right);
    }

    bool less_or_equal(
        const float left,
        const float right)
    {
        return less_or_equal_impl(
            left,
            right);
    }

    bool greater(
        const float left,
        const float right)
    {
        return greater_impl(
            left,
            right);
    }

    bool greater_or_equal(
        const float left,
        const float right)
    {
        return greater_or_equal_impl(
            left,
            right);
    }


    double get_epsilon(double)
    {
        return get_epsilon<double>();
    }

    bool equal(
        const double left,
        const double right)
    {
        return equal_impl(
            left,
            right);
    }

    bool not_equal(
        const double left,
        const double right)
    {
        return not_equal_impl(
            left,
            right);
    }

    bool less(
        const double left,
        const double right)
    {
        return less_impl(
            left,
            right);
    }

    bool less_or_equal(
        const double left,
        const double right)
    {
        return less_or_equal_impl(
            left,
            right);
    }

    bool greater(
        const double left,
        const double right)
    {
        return greater_impl(
            left,
            right);
    }

    bool greater_or_equal(
        const double left,
        const double right)
    {
        return greater_or_equal_impl(
            left,
            right);
    }
}