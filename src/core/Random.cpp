#include "core/Random.h"

namespace auraray::core {
namespace {

Vec3 randomVector(double minimum, double maximum, std::mt19937& random) {
    return Vec3(randomDouble(minimum, maximum, random),
                randomDouble(minimum, maximum, random),
                randomDouble(minimum, maximum, random));
}

}  // namespace

double randomDouble(std::mt19937& random) {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(random);
}

double randomDouble(double minimum, double maximum, std::mt19937& random) {
    return minimum + (maximum - minimum) * randomDouble(random);
}

Vec3 randomInUnitSphere(std::mt19937& random) {
    while (true) {
        const Vec3 point = randomVector(-1, 1, random);
        if (point.lengthSquared() < 1) {
            return point;
        }
    }
}

Vec3 randomUnitVector(std::mt19937& random) {
    return unitVector(randomInUnitSphere(random));
}

}  // namespace auraray::core
