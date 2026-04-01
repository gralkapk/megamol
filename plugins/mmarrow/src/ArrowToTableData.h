/*
 * ArrowToTableData.h
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "mmcore/Call.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"
#include "mmcore/param/ParamSlot.h"

#include "datatools/table/TableDataCall.h"
#include "mmarrow/ArrowDataCall.h"

namespace megamol::mmarrow {
class ArrowToTableData : public core::Module {
public:
    static const char* ClassName() {
        return "ArrowToTableData";
    }
    static const char* Description() {
        return "";
    }
    static bool IsAvailable() {
        return true;
    }

    ArrowToTableData();
    virtual ~ArrowToTableData();

protected:
    // Inherited via Module
    bool create() override;
    void release() override;

private:
    bool getDataCallback(core::Call& caller);
    bool getHashCallback(core::Call& caller);

    bool assertData(std::shared_ptr<ArrowDataCall::DataStruct> const& data);

    core::CallerSlot arrow_data_in_;
    core::CalleeSlot table_data_out_;

    std::uint64_t in_data_hash_;
    std::uint64_t out_data_hash_;

    std::uint64_t frame_id_;

    std::vector<datatools::table::TableDataCall::ColumnInfo> columns_;
    std::vector<float> values_;
};
} // namespace megamol::mmarrow
