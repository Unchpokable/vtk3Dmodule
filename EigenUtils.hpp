#pragma once

#include "pch.h"
#include "result.hpp"

namespace traits
{
    template<typename vec>
    constexpr bool IsEigenVector() {
        return std::is_same_v<vec, Eigen::Vector3d>;
    }
}

inline double* EigenVector3dToCArray(const Eigen::Vector3d& vec)
{
    const auto out = new double[3];

    for(int i = 0; i < 3; ++i) {
        out[i] = vec[i];
    }
    return out;
}

inline Result<Eigen::Vector3d> QStringToEigenVector3d(const QString& str)
{
    const auto list = str.split(' ');

    if(list.size() != 3)
        return { Status::BadArgument, "String should contain 3 float values" };

    bool okX, okY, okZ;
    double x = list.at(0).toDouble(&okX);
    double y = list.at(1).toDouble(&okY);
    double z = list.at(2).toDouble(&okZ);

    if(!okX || !okY || !okZ)
        return { Status::BadArgument, "One of the given numbers can not be parsed as a float" };
    return { { x, y, z } };
}

