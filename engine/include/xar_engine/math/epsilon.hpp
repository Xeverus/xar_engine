#pragma once


namespace xar_engine::math::epsilon
{
    float get_epsilon(float);
    double get_epsilon(double);

    bool equal(
        float left,
        float right);
    bool not_equal(
        float left,
        float right);
    bool less(
        float left,
        float right);
    bool less_or_equal(
        float left,
        float right);
    bool greater(
        float left,
        float right);
    bool greater_or_equal(
        float left,
        float right);

    bool equal(
        double left,
        double right);
    bool not_equal(
        double left,
        double right);
    bool less(
        double left,
        double right);
    bool less_or_equal(
        double left,
        double right);
    bool greater(
        double left,
        double right);
    bool greater_or_equal(
        double left,
        double right);
}