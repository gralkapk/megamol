/*
 * ArrowDataCall.h
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "mmcore/factories/CallAutoDescription.h"
#include "mmstd/data/AbstractGetDataCall.h"

namespace arrow {
class Table;
} // namespace arrow

namespace megamol::mmarrow {
class ArrowDataCall : public core::AbstractGetDataCall {
public:
    struct DataStruct {
        std::shared_ptr<arrow::Table> table_;
        std::uint64_t hash_;
        std::uint64_t frame_id_;
        DataStruct() : table_{nullptr}, hash_{0}, frame_id_{0} {}
    };

    static const char* ClassName() {
        return "ArrowDataCall";
    }

    static const char* Description() {
        return "Arrow Data Table";
    }

    static unsigned int FunctionCount() {
        return 2;
    }

    static const char* FunctionName(unsigned int idx) {
        switch (idx) {
        case 0:
            return "GetData";
        case 1:
            return "GetFrameCount";
        }
        return nullptr;
    }

    ArrowDataCall();
    virtual ~ArrowDataCall();

    std::shared_ptr<DataStruct> Data() const {
        return data_;
    }

    void Data(std::shared_ptr<DataStruct> data) {
        data_ = data;
    }

    std::uint64_t FrameCount() const {
        return frame_count_;
    }

    void FrameCount(std::uint64_t frame_count) {
        frame_count_ = frame_count;
    }

    std::uint64_t FrameID() const {
        return frame_id_;
    }

    void FrameID(std::uint64_t frame_id) {
        frame_id_ = frame_id;
    }

private:
    std::shared_ptr<DataStruct> data_ = nullptr;
    std::uint64_t frame_count_ = 0;
    std::uint64_t frame_id_ = 0;
};

typedef core::factories::CallAutoDescription<ArrowDataCall> ArrowDataCallDescription;
} // namespace megamol::mmarrow
