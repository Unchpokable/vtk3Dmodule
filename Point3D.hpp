#pragma once
#include "Property.hpp"

/**
 * \brief Light-Weight class represents 3d point. Used to avoid heavy \c Eigen::Vector3d or controversial \c double* arrays
 */
class Point3D
{
public:
    Point3D(Eigen::Vector3d pos) {
        _x = pos.x();
        _y = pos.y();
        _z = pos.z();
    }

    Point3D(const double* pos) {
        _x = pos[0];
        _y = pos[1];
        _z = pos[2];
    }

    Property<double> x() {
        return Property<double>(_x, [](double) { return true; });
    }

    Property<double> y() {
        return Property<double>(_y, [](double) { return true; });
    }

    Property<double> z() {
        return Property<double>(_z, [](double) { return true; });
    }

    bool operator==(const Point3D& other) const {
        return Equals(_x, other._x) && Equals(_y, other._y) && Equals(_z, other._z);
    }

    bool operator!=(const Point3D& other) const {
        return !Equals(_x, other._x) || !Equals(_y, other._y) || !Equals(_z, other._z);
    }

    Eigen::Vector3d ToEigenVector()
    {
        return Eigen::Vector3d(_x, _y, _z);
    }

    std::array<double, 3> ToArray()
    {
        return { _x, _y, _z };
    }

private:
    double _x;
    double _y;
    double _z;

    double _epsilon = 1e-8;

    bool Equals(double left, double right) const {
        return std::fabs(left - right) < _epsilon;
    }
};
