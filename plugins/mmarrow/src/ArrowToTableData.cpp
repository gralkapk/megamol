/*
 * ArrowToTableData.cpp
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#include "ArrowToTableData.h"

#include "datatools/table/TableDataCall.h"
#include "mmarrow/ArrowDataCall.h"

#include <arrow/api.h>
#include <arrow/compute/cast.h>

#include "ArrowUtility.h"

namespace megamol::mmarrow {
ArrowToTableData::ArrowToTableData()
        : arrow_data_in_{"arrowDataIn", ""}
        , table_data_out_{"tableDataOut", ""}
        , in_data_hash_{0}
        , out_data_hash_{0} {
    arrow_data_in_.SetCompatibleCall<ArrowDataCallDescription>();
    MakeSlotAvailable(&arrow_data_in_);

    table_data_out_.SetCallback(
        datatools::table::TableDataCall::ClassName(), "GetData", &ArrowToTableData::getDataCallback);
    table_data_out_.SetCallback(
        datatools::table::TableDataCall::ClassName(), "GetHash", &ArrowToTableData::getHashCallback);
    MakeSlotAvailable(&table_data_out_);
}

ArrowToTableData::~ArrowToTableData() {
    this->Release();
}

bool ArrowToTableData::create() {
    return true;
}

void ArrowToTableData::release() {}

bool ArrowToTableData::getDataCallback(core::Call& caller) {
    try {
        auto arrow_call = arrow_data_in_.CallAs<ArrowDataCall>();
        if (!arrow_call) {
            return false;
        }

        auto table_call = dynamic_cast<datatools::table::TableDataCall*>(&caller);
        if (!table_call) {
            return false;
        }

        if (!(*arrow_call)(0)) {
            return false;
        }

        auto data = arrow_call->Data();
        if (in_data_hash_ != data->hash_) {
            // process data
            if (!assertData(data)) {
                return false;
            }
            in_data_hash_ = data->hash_;
            ++out_data_hash_;
        }
        table_call->Set(columns_.size(), values_.size() / columns_.size(), columns_.data(), values_.data());
        table_call->SetDataHash(out_data_hash_);
        table_call->SetFrameID(0);
        table_call->SetFrameCount(1);
    } catch (...) {
        return false;
    }

    return true;
}

bool ArrowToTableData::getHashCallback(core::Call& caller) {
    try {
        auto table_call = dynamic_cast<datatools::table::TableDataCall*>(&caller);
        if (!table_call) {
            return false;
        }
        table_call->SetDataHash(out_data_hash_);
        table_call->SetFrameID(0);
        table_call->SetFrameCount(1);
    } catch (...) {
        return false;
    }
    return true;
}

bool isArithmeticType(std::shared_ptr<arrow::DataType> const& type) {
    return arrow::is_integer(type->id()) || arrow::is_floating(type->id());
}

std::pair<float, float> find_non_nan_minmax(std::vector<float> const& vec) {
    if (vec.empty()) {
        throw std::runtime_error("Cannot find min/max of an empty vector!");
    }
    auto is_not_nan = [](float v) { return !std::isnan(v); };
    auto first_valid = std::find_if(vec.begin(), vec.end(), is_not_nan);

    if (first_valid == vec.end()) {
        // All values are NaN
        float nan = std::numeric_limits<float>::quiet_NaN();
        return {nan, nan};
    }

    float min_val = *first_valid;
    float max_val = *first_valid;
    for (auto it = first_valid + 1; it != vec.end(); ++it) {
        if (!std::isnan(*it)) {
            if (*it < min_val)
                min_val = *it;
            if (*it > max_val)
                max_val = *it;
        }
    }
    return {min_val, max_val};
}

bool ArrowToTableData::assertData(std::shared_ptr<ArrowDataCall::DataStruct> const& data) {
    auto const& table = data->table_;

    auto const num_rows = table->num_rows();
    auto const num_columns = table->num_columns();

    auto const col_names = table->ColumnNames();

    columns_.clear();
    columns_.reserve(num_columns);

    for (int c = 0; c < num_columns; ++c) {
        auto const col_name = col_names[c];
        auto const col = table->column(c);
        if (isArithmeticType(col->type())) {
            datatools::table::TableDataCall::ColumnInfo ci;
            ci.SetName(col_name);
            columns_.push_back(ci);
        }
    }

    values_.clear();
    values_.reserve(num_rows * columns_.size());

    std::vector<std::vector<float>> column_storage;
    column_storage.reserve(columns_.size());

    for (auto const& column_info : columns_) {
        auto const col = table->GetColumnByName(column_info.Name());
        auto const num_chunks = col->num_chunks();
        std::vector<float> column_values;
        column_values.reserve(num_rows);
        for (int c = 0; c < num_chunks; ++c) {
            auto const array = col->chunk(c);
            convertArrowArrayToVector(array, std::back_inserter(column_values));
        }
        column_storage.push_back(std::move(column_values));
    }

    // interleave column values into row-major order
    for (int64_t r = 0; r < num_rows; ++r) {
        for (size_t c = 0; c < columns_.size(); ++c) {
            values_.push_back(column_storage[c][r]);
        }
    }

    for (size_t c = 0; c < columns_.size(); ++c) {
        /*auto min_max_el = std::minmax_element(column_storage[c].begin(), column_storage[c].end(), [](float a, float b) {
            if (std::isnan(a)) {
                return false;
            }
            if (std::isnan(b)) {
                return true;
            }
            return a < b;
        });*/
        auto [min_val, max_val] = find_non_nan_minmax(column_storage[c]);
        columns_[c].SetMinimumValue(min_val);
        columns_[c].SetMaximumValue(max_val);
        columns_[c].SetType(datatools::table::TableDataCall::ColumnType::QUANTITATIVE);
    }

    return true;
}
} // namespace megamol::mmarrow
