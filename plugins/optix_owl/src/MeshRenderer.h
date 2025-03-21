#pragma once

#if 1

#include <vector>

#include "mmcore/param/ParamSlot.h"

#include "mesh/MeshCalls.h"
#include "mmstd_gl/renderer/Renderer3DModuleGL.h"

#include <owl/common/math/vec.h>
#include <owl/owl.h>

#include "framestate.h"
#include "particle.h"

#include "BaseRenderer.h"

namespace megamol::optix_owl {
class MeshRenderer : public BaseRenderer<mesh::CallMesh> {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName(void) {
        return "MeshRenderer";
    }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description(void) {
        return "";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable(void) {
        return true;
    }

    MeshRenderer();

    virtual ~MeshRenderer();

protected:
    bool create() override;

    void release() override;

private:
    bool assertData(mesh::CallMesh const& call) override;

    bool data_param_is_dirty() override;
    void data_param_reset_dirty() override;

    OWLModule pkd_module_;
    OWLGeomType tri_type_;

    std::vector<OWLBuffer> vertex_buffers_;
    std::vector<OWLBuffer> index_buffers_;
    std::vector<OWLGeom> geoms_;
};
} // namespace megamol::optix_owl

#endif
