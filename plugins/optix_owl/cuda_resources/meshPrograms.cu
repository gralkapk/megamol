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
    //prd.pos = self.particleBuffer[primID].pos;
}

} // namespace device
} // namespace optix_owl
} // namespace megamol
