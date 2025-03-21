#pragma once

#include <owl/common/math/vec.h>

namespace megamol {
namespace optix_owl {
namespace device {
struct PerRayData {
    int primID;
    float t;
    owl::vec3f Ng;
};
} // namespace device
} // namespace optix_owl
} // namespace megamol
