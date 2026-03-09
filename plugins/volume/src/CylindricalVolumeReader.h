/*
 * CylindricalVolumeReader.h
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "mmcore/param/ParamSlot.h"
#include "mmstd/data/AnimDataModule.h"

namespace megamol::volume {
class CylindricalVolumeReader : public core::view::AnimDataModule {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName() {
        return "CylindricalVolumeReader";
    }
    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description() {
        return "Cylindrical volume reader";
    }
    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable() {
        return true;
    }
    /** Ctor. */
    CylindricalVolumeReader();
    /** Dtor. */
    virtual ~CylindricalVolumeReader();

protected:
    /**
     * Implementation of 'Create'.
     *
     * @return 'true' on success, 'false' otherwise.
     */
    bool create();
    /**
     * Implementation of 'Release'.
     */
    void release();

private:
    // Inherited via AnimDataModule
    Frame* constructFrame() const override;
    void loadFrame(Frame* frame, unsigned int idx) override;

    core::param::ParamSlot filename_slot_;
};
} // namespace megamol::volume
