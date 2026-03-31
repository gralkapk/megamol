/*
 * ArrowDataSource.cpp
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#include "ArrowDataSource.h"

#include <arrow/api.h>
#include <parquet/arrow/reader.h>

#include "mmcore/param/BoolParam.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/FilePathParam.h"

#include "mmarrow/ArrowDataCall.h"

namespace megamol::mmarrow {
ArrowDataSource::ArrowDataSource()
        : get_data_slot_("getData", "")
        , filename_slot_{"filename", ""}
        , mode_slot_{"mode", ""}
        , remote_slot_{"remote", ""} {
    get_data_slot_.SetCallback(ArrowDataCall::ClassName(), "GetData", &ArrowDataSource::getDataCallback);
    MakeSlotAvailable(&get_data_slot_);

    filename_slot_ << new core::param::FilePathParam("");
    filename_slot_.SetUpdateCallback(&ArrowDataSource::filenameChanged);
    this->MakeSlotAvailable(&filename_slot_);

    auto ep = new core::param::EnumParam(Mode::MODE_PERFORMANCE);
    ep->SetTypePair(Mode::MODE_PERFORMANCE, "Performance");
    ep->SetTypePair(Mode::MODE_MEMORY, "Memory");
    mode_slot_ << ep;
    this->MakeSlotAvailable(&mode_slot_);

    remote_slot_ << new core::param::BoolParam(false);
    this->MakeSlotAvailable(&remote_slot_);

    data_ = std::make_shared<ArrowDataCall::DataStruct>(nullptr, 0);
}

ArrowDataSource::~ArrowDataSource() {
    this->Release();
}

bool ArrowDataSource::create() {
    return true;
}

void ArrowDataSource::release() {}

core::view::AnimDataModule::Frame* ArrowDataSource::constructFrame() const {
    return nullptr;
}

void ArrowDataSource::loadFrame(Frame* frame, unsigned int idx) {}

bool ArrowDataSource::filenameChanged(core::param::ParamSlot& slot) {
    auto filename = filename_slot_.Param<core::param::FilePathParam>()->Value();

    arrow::MemoryPool* pool = arrow::default_memory_pool();

    auto reader_properties = parquet::ReaderProperties(pool);
    reader_properties.disable_buffered_stream();

    auto arrow_reader_properties = parquet::ArrowReaderProperties(true);
    if (remote_slot_.Param<core::param::BoolParam>()->Value()) {
        arrow_reader_properties.set_pre_buffer(true);
    }

    parquet::arrow::FileReaderBuilder builder;
    auto status = builder.OpenFile(filename.generic_string(), false, reader_properties);
    if (!status.ok()) {
        return false;
    }
    builder.memory_pool(pool);
    builder.properties(arrow_reader_properties);

    std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
    status = builder.Build(&arrow_reader);
    if (!status.ok()) {
        return false;
    }

    arrow_reader->ReadTable(&data_->table_);
    ++data_->hash_;

    return true;
}

bool ArrowDataSource::getDataCallback(core::Call& caller) {
    auto data_call = dynamic_cast<ArrowDataCall*>(&caller);
    if (!data_call) {
        return false;
    }

    data_call->Data(data_);

    return true;
}

//bool isArithmeticType(std::shared_ptr<arrow::DataType> const& type) {
//    return arrow::is_integer(type->id()) || arrow::is_floating(type->id());
//}

//bool ArrowDataSource::assertData() {
//    if (!table_) {
//        return false;
//    }
//
//    auto const num_rows = table_->num_rows();
//    auto const num_columns = table_->num_columns();
//
//    auto const col_names = table_->ColumnNames();
//
//    columns_.clear();
//    columns_.reserve(num_columns);
//
//    for (int c = 0; c < num_columns; ++c) {
//        auto const col_name = col_names[c];
//        auto const col = table_->column(c);
//        if (isArithmeticType(col->type())) {
//            datatools::table::TableDataCall::ColumnInfo ci;
//            ci.SetName(col_name);
//            columns_.push_back(ci);
//        }
//    }
//
//
//
//    return true;
//}

} // namespace megamol::mmarrow
