/*
 * ArrowDataSource.h
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "mmcore/CalleeSlot.h"
#include "mmcore/param/ParamSlot.h"
#include "mmstd/data/AnimDataModule.h"

#include "mmarrow/ArrowDataCall.h"

namespace megamol::mmarrow {
class ArrowDataSource : public core::view::AnimDataModule {
public:
    static const char* ClassName() {
        return "ArrowDataSource";
    }
    static const char* Description() {
        return "";
    }
    static bool IsAvailable() {
        return true;
    }

    ArrowDataSource();

    virtual ~ArrowDataSource();

protected:
    bool create() override;

    void release() override;

private:
    enum Mode { MODE_PERFORMANCE, MODE_MEMORY };

    // Inherited via AnimDataModule
    Frame* constructFrame() const override;
    void loadFrame(Frame* frame, unsigned int idx) override;

    /**
     * Callback receiving the update of the file name parameter.
     *
     * @param slot The updated ParamSlot.
     *
     * @return Always 'true' to reset the dirty flag.
     */
    bool filenameChanged(core::param::ParamSlot& slot);

    bool getDataCallback(core::Call& caller);

    //bool assertData();

    core::CalleeSlot get_data_slot_;

    core::param::ParamSlot filename_slot_;
    core::param::ParamSlot mode_slot_;
    core::param::ParamSlot remote_slot_;

    std::shared_ptr<ArrowDataCall::DataStruct> data_;
};
} // namespace megamol::mmarrow
