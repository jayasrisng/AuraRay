#pragma once

#include <memory>
#include <vector>

#include "geometry/Hittable.h"

namespace auraray::geometry {

class HittableList final : public Hittable {
  public:
    void add(std::shared_ptr<Hittable> object);
    std::size_t size() const;

    bool hit(const core::Ray& ray, double minimumDistance,
             double maximumDistance, HitRecord& record) const override;

  private:
    std::vector<std::shared_ptr<Hittable>> objects_;
};

}  // namespace auraray::geometry
