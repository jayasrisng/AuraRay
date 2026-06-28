#pragma once

#include <random>

#include "core/Vec3.h"

namespace auraray::core {

double randomDouble(std::mt19937& random);
double randomDouble(double minimum, double maximum, std::mt19937& random);
Vec3 randomInUnitSphere(std::mt19937& random);
Vec3 randomUnitVector(std::mt19937& random);

}  // namespace auraray::core
