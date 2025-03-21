#include "MeshRenderer.h"

#if 1

#include <fstream>

#include "mmcore/param/BoolParam.h"
#include "mmcore/param/FilePathParam.h"
#include "mmcore/param/FloatParam.h"
#include "mmcore/param/IntParam.h"

#include <owl/common/math/box.h>
#include <owl/common/math/vec.h>

#include "PKDCreate.h"

#include "mesh.h"
#include "framestate.h"
#include "raygen.h"

#include <glad/gl.h>

#include <cuda_runtime.h>

#include "MeshDataWrapper.h"

namespace megamol::optix_owl {
extern "C" const unsigned char meshPrograms_ptx[];

MeshRenderer::MeshRenderer() {
    setBDW(std::make_unique<MeshDataWrapper<MeshRenderer>>(this));
}

MeshRenderer::~MeshRenderer() {
    this->Release();
}

bool MeshRenderer::create() {
    auto const ret = BaseRenderer::create();

    pkd_module_ = owlModuleCreate(ctx_, reinterpret_cast<const char*>(meshPrograms_ptx));

    OWLVarDecl allPKDVars[] = {
        {"index", OWL_BUFPTR, OWL_OFFSETOF(device::MeshGeomData, index)},
        {"vertex", OWL_BUFPTR, OWL_OFFSETOF(device::MeshGeomData, vertex)},
        {/* sentinel to mark end of list */}
    };

    tri_type_ = owlGeomTypeCreate(ctx_, OWL_TRIANGLES, sizeof(device::MeshGeomData), allPKDVars, -1);
    //owlGeomTypeSetBoundsProg(allPKDType, pkd_module_, "bvh_bounds");
    //owlGeomTypeSetIntersectProg(allPKDType, 0, pkd_module_, "bvh_intersect");
    owlGeomTypeSetClosestHit(tri_type_, 0, pkd_module_, "mesh_ch");

    return ret;
}

void MeshRenderer::release() {
    owlModuleRelease(pkd_module_);
    BaseRenderer::release();
}

bool MeshRenderer::assertData(mesh::CallMesh const& call) {
    auto& mc = const_cast<mesh::CallMesh&>(call);
    auto& meta_data = mc.getMetaData();

    auto mesh_data = mc.getData();

    auto& meshes = mesh_data->accessMeshes();

    for (auto& vb : vertex_buffers_) {
        owlBufferDestroy(vb);
    }
    vertex_buffers_.clear();
    index_buffers_.clear();
    for (auto& ib : index_buffers_) {
        owlBufferDestroy(ib);
    }
    for (auto& g : geoms_) {
        owlGeomRelease(g);
    }
    geoms_.clear();

    for (auto& mesh : meshes) {
        if (mesh.second.primitive_type == mesh::MeshDataAccessCollection::PrimitiveType::TRIANGLES) {
            auto idx_bs = mesh::MeshDataAccessCollection::getByteSize(mesh.second.indices.type);
            if (idx_bs == sizeof(int)) {
                index_buffers_.push_back(owlDeviceBufferCreate(
                    ctx_, OWL_INT3, mesh.second.indices.byte_size / (3 * sizeof(int)), mesh.second.indices.data));
                auto const index_count = mesh.second.indices.byte_size / (3 * sizeof(int));
                auto vertex_count = 0ull;
                for (auto& attr : mesh.second.attributes) {
                    if (attr.semantic == mesh::MeshDataAccessCollection::AttributeSemanticType::POSITION) {
                        vertex_buffers_.push_back(
                            owlDeviceBufferCreate(ctx_, OWL_FLOAT3, attr.byte_size / (3 * sizeof(float)), attr.data));
                        vertex_count = attr.byte_size / (3 * sizeof(float));
                    }
                }
                auto geom = owlGeomCreate(ctx_, tri_type_);
                owlTrianglesSetVertices(geom, vertex_buffers_.back(), vertex_count, 3 * sizeof(float), 0);
                owlTrianglesSetIndices(geom, index_buffers_.back(), index_count, 3 * sizeof(int), 0);
                owlGeomSetBuffer(geom, "vertex", vertex_buffers_.back());
                owlGeomSetBuffer(geom, "index", index_buffers_.back());
                geoms_.push_back(geom);
                break;
            }
        }
    }

    auto triangles_group = owlTrianglesGeomGroupCreate(ctx_, geoms_.size(), geoms_.data());
    owlGroupBuildAccel(triangles_group);
    if (world_)
        owlGroupRelease(world_);
    world_ = owlInstanceGroupCreate(ctx_, 1, &triangles_group);
    owlGroupBuildAccel(world_);

    //auto const pl_count = call.GetParticleListCount();

    //std::vector<device::Particle> particles_;
    //owl::common::box3f total_bounds;
    //auto const global_radius = radius_slot_.Param<core::param::FloatParam>()->Value();

    //for (unsigned int pl_idx = 0; pl_idx < pl_count; ++pl_idx) {
    //    auto const& particles = call.AccessParticles(pl_idx);

    //    auto const p_count = particles.GetCount();
    //    if (p_count == 0)
    //        continue;
    //    /*if (particles.GetVertexDataType() == geocalls::SimpleSphericalParticles::VERTDATA_FLOAT_XYZR)
    //        continue;*/

    //    particles_.reserve(particles_.size() + p_count);

    //    std::vector<device::Particle> data(p_count);

    //    auto x_acc = particles.GetParticleStore().GetXAcc();
    //    auto y_acc = particles.GetParticleStore().GetYAcc();
    //    auto z_acc = particles.GetParticleStore().GetZAcc();

    //    owl::common::box3f bounds;

    //    for (std::size_t i = 0; i < p_count; ++i) {
    //        data[i].pos = owl::common::vec3f(x_acc->Get_f(i), y_acc->Get_f(i), z_acc->Get_f(i));
    //        bounds.extend(
    //            owl::common::box3f().including(data[i].pos - global_radius).including(data[i].pos + global_radius));
    //    }

    //    particles_.insert(particles_.end(), data.begin(), data.end());
    //    total_bounds.extend(bounds);
    //}

    //if (particleBuffer_)
    //    owlBufferDestroy(particleBuffer_);
    //particleBuffer_ =
    //    owlDeviceBufferCreate(ctx_, OWL_USER_TYPE(device::Particle), particles_.size(), particles_.data());

    //core::utility::log::Log::DefaultLog.WriteInfo("[MeshRenderer] Rendering %d particles", particles_.size());

    //owlGeomSetPrimCount(geom_, particles_.size());

    //owlGeomSetBuffer(geom_, "particleBuffer", particleBuffer_);
    //owlGeomSet1f(geom_, "particleRadius", global_radius);

    //owlBuildPrograms(ctx_);

    //OWLGroup ug = owlUserGeomGroupCreate(ctx_, 1, &geom_);
    //owlGroupBuildAccel(ug);

    //world_ = owlInstanceGroupCreate(ctx_, 1, &ug);

    //owlGroupBuildAccel(world_);

    if (dump_debug_info_slot_.Param<core::param::BoolParam>()->Value()) {
        size_t memFinal = 0;
        size_t memPeak = 0;
        owlGroupGetAccelSize(triangles_group, &memFinal, &memPeak);

        //size_t comp_data_size = particles_.size() * sizeof(device::Particle);

        auto const output_path = debug_output_path_slot_.Param<core::param::FilePathParam>()->Value();
        auto of = std::ofstream(output_path / "size.csv");
        of << "BVHFinalSize[B],BVHPeakSize[B]\n";
        of << memFinal << "," << memPeak << "\n";
        of.close();
    }

    return true;
}

bool MeshRenderer::data_param_is_dirty() {
    return false;
}

void MeshRenderer::data_param_reset_dirty() {}
} // namespace megamol::optix_owl

#endif
