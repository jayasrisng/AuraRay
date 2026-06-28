#include "geometry/HittableList.h"

#include <utility>

namespace auraray::geometry {

void HittableList::add(std::shared_ptr<Hittable> object) {
    objects_.push_back(std::move(object));
}

std::size_t HittableList::size() const { return objects_.size(); }

bool HittableList::hit(const core::Ray& ray, double minimumDistance,
                       double maximumDistance, HitRecord& record) const {
    HitRecord temporaryRecord;
    bool hitAnything = false;
    double closestDistance = maximumDistance;

    for (const auto& object : objects_) {
        if (object->hit(ray, minimumDistance, closestDistance,
                        temporaryRecord)) {
            hitAnything = true;
            closestDistance = temporaryRecord.distance;
            record = temporaryRecord;
        }
    }

    return hitAnything;
}

}  // namespace auraray::geometry
