#pragma once

#include "mmstd_gl/renderer/CallRender3DGL.h"

namespace megamol::optix_owl {
class BaseDataWrapper {
public:
    virtual bool HasDataUpdate(void* dc, unsigned int fid, uint64_t hash) = 0;

    virtual unsigned int GetFrameID(void* dc) = 0;

    virtual uint64_t GetDataHash(void* dc) = 0;

    virtual bool CheckData(mmstd_gl::CallRender3DGL& cr, void* dc) = 0;

    virtual bool CheckExtent(mmstd_gl::CallRender3DGL& cr, void* dc) = 0;

    virtual bool AssertData(void* dc) = 0;
};
} // namespace megamol::optix_owl
