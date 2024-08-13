#ifndef POINTRANGE_H
#define POINTRANGE_H


#include <cstdlib>


struct PointRange
{
    double m_zero;
    double m_error;

    PointRange(double zero = 0.0, double error = 0.0);

    static constexpr double roundFactor = 0.1;
    static constexpr double errorEpsilon = roundFactor / 2.;

    bool isValidPoint(double value) const
    {
        return ((std::abs(value - this->m_zero) - this->m_error) < errorEpsilon);
    }
};

#endif // POINTRANGE_H
