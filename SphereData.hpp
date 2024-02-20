#pragma once
#include "RgbaF.hpp"


struct SphereData
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Eigen::Vector3d Position;
    float Radius;
    RgbaF Color;
};
