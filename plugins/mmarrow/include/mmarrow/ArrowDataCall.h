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
    };

    static const char* ClassName() {
        return "ArrowDataCall";
    }

    static const char* Description() {
        return "Arrow Data Table";
    }

    static unsigned int FunctionCount() {
        return 1;
    }

    static const char* FunctionName(unsigned int idx) {
        switch (idx) {
        case 0:
            return "GetData";
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

private:
    std::shared_ptr<DataStruct> data_;
};

typedef core::factories::CallAutoDescription<ArrowDataCall> ArrowDataCallDescription;
} // namespace megamol::mmarrow
