#ifndef POINTRANGE_H
#define POINTRANGE_H


struct PointRange
{
    double m_zero;
    double m_error;

    PointRange(double zero = 0.0, double error = 0.0);
};

#endif // POINTRANGE_H
