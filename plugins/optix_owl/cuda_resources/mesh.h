#pragma once

#include <owl/common/math/vec.h>
#include <owl/common/math/box.h>

namespace megamol {
namespace optix_owl {
namespace device {
using namespace owl::common;
struct MeshGeomData {
    owl::vec3i* index;
    owl::vec3f* vertex;
};
} // namespace device
} // namespace optix_owl
} // namespace megamol
