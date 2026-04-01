/*
 * ArrowDataSource.cpp
 *
 * Copyright (C) 2026 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */
#include "ArrowDataSource.h"

#include <filesystem>
#include <regex>

#include <arrow/api.h>
#include <parquet/arrow/reader.h>

#include "mmcore/param/BoolParam.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/FilePathParam.h"
#include "mmcore/param/StringParam.h"

#include "mmarrow/ArrowDataCall.h"

namespace megamol::mmarrow {
ArrowDataSource::ArrowDataSource()
        : get_data_slot_("getData", "")
        , filename_slot_{"filename", ""}
        , pattern_slot_{"pattern", ""}
        , mode_slot_{"mode", ""}
        , remote_slot_{"remote", ""}
        , data_hash_{0} {
    get_data_slot_.SetCallback(ArrowDataCall::ClassName(), "GetData", &ArrowDataSource::getDataCallback);
    get_data_slot_.SetCallback(ArrowDataCall::ClassName(), "GetFrameCount", &ArrowDataSource::getFrameCountCallback);
    MakeSlotAvailable(&get_data_slot_);

    filename_slot_ << new core::param::FilePathParam("", core::param::FilePathParam::FilePathFlags_::Flag_Directory |
                                                             core::param::FilePathParam::FilePathFlags_::Flag_File);
    filename_slot_.SetUpdateCallback(&ArrowDataSource::filenameChanged);
    this->MakeSlotAvailable(&filename_slot_);

    pattern_slot_ << new core::param::StringParam("*.parquet");
    this->MakeSlotAvailable(&pattern_slot_);

    /*auto ep = new core::param::EnumParam(Mode::MODE_PERFORMANCE);
    ep->SetTypePair(Mode::MODE_PERFORMANCE, "Performance");
    ep->SetTypePair(Mode::MODE_MEMORY, "Memory");
    mode_slot_ << ep;
    this->MakeSlotAvailable(&mode_slot_);*/

    remote_slot_ << new core::param::BoolParam(false);
    this->MakeSlotAvailable(&remote_slot_);

    data_ = std::make_shared<ArrowDataCall::DataStruct>();
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

std::vector<std::filesystem::path> enumerate_files_regex(std::filesystem::path const& dir, std::string const& pattern) {
    std::vector<std::filesystem::path> result;
    std::regex re(pattern);
    for (auto const& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), re)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

bool ArrowDataSource::filenameChanged(core::param::ParamSlot& slot) {
    auto filename = filename_slot_.Param<core::param::FilePathParam>()->Value();

    if (!std::filesystem::exists(filename))
        return false;

    files_.clear();
    ++data_hash_;

    if (std::filesystem::is_directory(filename)) {
        auto pattern = pattern_slot_.Param<core::param::StringParam>()->Value();

        files_ = enumerate_files_regex(filename, pattern);
    } else {
        files_.push_back(filename);

        /*arrow::MemoryPool* pool = arrow::default_memory_pool();

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

        arrow_reader->ReadTable(&data_->table_);*/
    }

    return true;
}

bool ArrowDataSource::getDataCallback(core::Call& caller) {
    auto data_call = dynamic_cast<ArrowDataCall*>(&caller);
    if (!data_call) {
        return false;
    }

    auto out_data = data_call->Data();
    if (out_data && !(out_data->hash_ != data_hash_ || out_data->frame_id_ != data_call->FrameID())) {
        // No need to reload
        return true;
    }

    auto requested_frame = data_call->FrameID();
    if (requested_frame >= files_.size()) {
        return false;
    }
    if (!loadFile(files_[requested_frame])) {
        return false;
    }
    data_->frame_id_ = requested_frame;
    data_call->Data(data_);

    return true;
}

bool ArrowDataSource::getFrameCountCallback(core::Call& caller) {
    auto data_call = dynamic_cast<ArrowDataCall*>(&caller);
    if (!data_call) {
        return false;
    }

    data_call->FrameCount(files_.size());

    return true;
}

bool ArrowDataSource::loadFile(std::filesystem::path const& path) {
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    auto reader_properties = parquet::ReaderProperties(pool);
    reader_properties.disable_buffered_stream();

    auto arrow_reader_properties = parquet::ArrowReaderProperties(true);
    if (remote_slot_.Param<core::param::BoolParam>()->Value()) {
        arrow_reader_properties.set_pre_buffer(true);
    }

    parquet::arrow::FileReaderBuilder builder;
    auto status = builder.OpenFile(path.generic_string(), false, reader_properties);
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
    data_->hash_ = data_hash_;

    return true;
}

} // namespace megamol::mmarrow
