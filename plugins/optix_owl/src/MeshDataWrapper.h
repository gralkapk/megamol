#pragma once

#include "mesh/MeshCalls.h"
#include "mmstd_gl/renderer/CallRender3DGL.h"

#include "BaseDataWrapper.h"

namespace megamol::optix_owl {
template<typename T>
class MeshDataWrapper : public BaseDataWrapper {
public:
    MeshDataWrapper(T* t) : that(t) {};

    bool HasDataUpdate(void* dc, unsigned int fid, uint64_t hash) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        return dcp->getMetaData().m_frame_ID != fid || dcp->hasUpdate();
    }

    unsigned int GetFrameID(void* dc) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        return dcp->getMetaData().m_frame_ID;
    }

    uint64_t GetDataHash(void* dc) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        return dcp->version();
    }

    bool CheckData(mmstd_gl::CallRender3DGL& cr, void* dc) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        auto meta = dcp->getMetaData();
        meta.m_frame_ID = cr.Time();
        dcp->setMetaData(meta);
        if (!(*dcp)(1))
            return false;
        if (!(*dcp)(0))
            return false;

        return true;
    }

    bool CheckExtent(mmstd_gl::CallRender3DGL& cr, void* dc) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        auto meta = dcp->getMetaData();
        meta.m_frame_ID = cr.Time();
        dcp->setMetaData(meta);
        if (!(*dcp)(1))
            return false;

        cr.AccessBoundingBoxes() = dcp->getMetaData().m_bboxs;
        cr.SetTimeFramesCount(dcp->getMetaData().m_frame_cnt);

        return true;
    };

    bool AssertData(void* dc) override {
        auto dcp = reinterpret_cast<mesh::CallMesh*>(dc);
        return that->assertData(*dcp);
    }

private:
    T* that;
};
} // namespace megamol::optix_owl
