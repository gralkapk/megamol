#pragma once

#include "geometry_calls/MultiParticleDataCall.h"
#include "mmstd_gl/renderer/CallRender3DGL.h"

#include "BaseDataWrapper.h"

namespace megamol::optix_owl {
template<typename T>
class MultiParticleDataWrapper : public BaseDataWrapper {
public:
    MultiParticleDataWrapper(T* t) : that(t) {};

    bool HasDataUpdate(void* dc, unsigned int fid, uint64_t hash) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        return dcp->FrameID() != fid || dcp->DataHash() != hash;
    }

    unsigned int GetFrameID(void* dc) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        return dcp->FrameID();
    }

    uint64_t GetDataHash(void* dc) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        return dcp->DataHash();
    }

    bool CheckData(mmstd_gl::CallRender3DGL& cr, void* dc) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        dcp->SetFrameID(cr.Time());
        if (!(*dcp) (1))
            return false;
        if (!(*dcp) (0))
            return false;

        return true;
    }

    bool CheckExtent(mmstd_gl::CallRender3DGL& cr, void* dc) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        dcp->SetFrameID(cr.Time());
        if (!(*dcp) (1))
            return false;

        cr.AccessBoundingBoxes() = dcp->GetBoundingBoxes();
        cr.SetTimeFramesCount(dcp->FrameCount());

        return true;
    };

    bool AssertData(void* dc) override {
        auto dcp = reinterpret_cast<geocalls::MultiParticleDataCall*>(dc);
        return that->assertData(*dcp);
    }

private:
    T* that;
};
} // namespace megamol::optix_owl
