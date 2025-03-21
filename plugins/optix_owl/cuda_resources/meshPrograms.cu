#include <cuda_runtime.h>
#include <optix_device.h>

#include <owl/common/math/box.h>
#include <owl/common/math/vec.h>
#include <owl/owl_device.h>

#include "mesh.h"
#include "intersect.h"
#include "perraydata.h"

namespace megamol {
namespace optix_owl {
namespace device {
using namespace owl::common;

OPTIX_CLOSEST_HIT_PROGRAM(mesh_ch)() {
    const int primID = optixGetPrimitiveIndex();
    PerRayData& prd = owl::getPRD<PerRayData>();
    const auto& self = owl::getProgramData<MeshGeomData>();
    prd.primID = primID;
    prd.t = optixGetRayTmax();
    owl::Ray ray(optixGetWorldRayOrigin(), optixGetWorldRayDirection(), optixGetRayTmin(), optixGetRayTmax());
    const vec3i index = self.index[primID];
    const vec3f& A = self.vertex[index.x];
    const vec3f& B = self.vertex[index.y];
    const vec3f& C = self.vertex[index.z];
    prd.Ng = normalize(cross(B - A, C - A));
}

} // namespace device
} // namespace optix_owl
} // namespace megamol
