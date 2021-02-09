//#include "owl/common/math/random.h"
//#include "owl/owl_device.h"

#include "camera.h"
#include "raygen.h"

#include "hpg/optix/utils_device.h"
#include "hpg/optix/random.h"

namespace megamol {
namespace hpg {
    namespace optix {
        namespace device {
            //using Random = owl::common::LCG<>;

            inline __device__ glm::vec4 traceRay(
                const RayGenData& self, Ray& ray/*, Random& rnd*/, PerRayData& prd, glm::vec4& bg) {
                glm::vec4 attenuation = glm::vec4(1.f);
                glm::vec4 ambientLight(.8f, .8f, .8f, 1.f);

                prd.primID = -1;

                // printf("TRACE: ray %f %f %f %f %f %f\n", ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x,
                // ray.direction.y, ray.direction.z);
                
                //owl::traceRay(/*accel to trace against*/ self.world,
                //    /*the ray to trace*/ ray,
                //    /*prd*/ prd, OPTIX_RAY_FLAG_DISABLE_ANYHIT);

                unsigned int p0 = 0;
                unsigned int p1 = 0;
                packPointer(&prd, p0, p1);

                optixTrace(self.world, (const float3&) ray.origin, (const float3&) ray.direction, ray.tmin, ray.tmax,
                    0, (OptixVisibilityMask) -1,
                    /*rayFlags     */ OPTIX_RAY_FLAG_DISABLE_ANYHIT,
                    /*SBToffset    */ 0,
                    /*SBTstride    */ 1,
                    /*missSBTIndex */ 0, p0, p1);

                if (prd.primID == -1) {
                    // miss...
                    // return attenuation * ambientLight;
                    return bg;
                }

                //const glm::vec4 albedo(0.f, 1.f, 0.f, 1.f);
                const glm::vec4 albedo = prd.albedo;
                auto const factor =
                    (.2f + .6f * fabsf(glm::dot(prd.N, ray.direction)));
                return albedo * glm::vec4(factor, factor, factor, 1.f);
            }

            MM_OPTIX_RAYGEN_KERNEL(raygen_program)() {
                //printf("RAYGEN1\n");
                const RayGenData& self = getProgramData<RayGenData>();
                auto const index = optixGetLaunchIndex();
                glm::ivec2 pixelID = glm::ivec2(index.x, index.y);
                //const owl::vec2i pixelID = owl::getLaunchIndex();
                //const owl::vec2i launchDim = owl::getLaunchDims();

                unsigned int seed = tea<4>(pixelID.y * 42 + pixelID.x, 42);

                if (pixelID.x >= self.fbSize.x)
                    return;
                if (pixelID.y >= self.fbSize.y)
                    return;
                const int pixelIdx = pixelID.x + self.fbSize.x * pixelID.y;

                const FrameState* fs = &self.frameStateBuffer[0];
                glm::vec4 col(0.f);
                glm::vec4 bg = fs->background;

                //printf("RAYGEN FS %f\n", fs->near);

                PerRayData prd;

                //Random rnd(pixelIdx, 0);

                float u = -fs->rw + (fs->rw + fs->rw) * float(pixelID.x + rnd(seed)) / self.fbSize.x;
                float v = -(fs->th + (-fs->th - fs->th) * float(pixelID.y + rnd(seed)) / self.fbSize.y);
                /*float u = -fs->rw + (fs->rw + fs->rw) * float(pixelID.x) / self.fbSize.x;
                float v = -(fs->th + (-fs->th - fs->th) * float(pixelID.y) / self.fbSize.y);*/
                auto ray = generateRay(*fs, u, v);
                col = traceRay(self, ray/*, rnd*/, prd, bg);
                //printf("RAYGEN2\n");

                // for (int s = 0; s < fs->samplesPerPixel; ++s) {
                //     float u = -fs->rw + (fs->rw+fs->rw)*float(pixelID.x + rnd())/self.fbSize.x;
                //     float v = -(fs->th + (-fs->th-fs->th)*float(pixelID.y + rnd())/self.fbSize.y);
                //     //float u = float(pixelID.x + rnd());
                //     //float v = float(pixelID.y + rnd());
                //     owl::Ray ray = generateRay(*fs, u, v);
                //     col += owl::vec4f(traceRay(self, ray, rnd, prd), 1);
                // }
                // col = col / float(fs->samplesPerPixel);

                // uint32_t rgba = owl::make_rgba(col);
                // self.colorBufferPtr[pixelIdx] = rgba;

                self.colorBufferPtr[pixelIdx] = col;
            }
        } // namespace device
    }     // namespace optix
} // namespace hpg
} // namespace megamol