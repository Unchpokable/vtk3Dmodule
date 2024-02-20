#pragma once

#include "pch.h"

#include "RgbaF.hpp"

/**
 * \brief Represents LineData structure with native C++ types for containing Begin and End positions
 */
struct CLineData
{
    const double* Begin;
    const double* End;
    RgbaF Color;
};

/**
 * \brief Represents Line data with Eigen's types for containing Begin and End positions
 */
struct LineData
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Eigen::Vector3d Begin;
    Eigen::Vector3d End;

    RgbaF Color;
};
