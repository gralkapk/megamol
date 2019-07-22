/*
 * AstroSchulz.cpp
 *
 * Copyright (C) 2019 by VISUS (Universitaet Stuttgart)
 * All rights reserved.
 */

#include "stdafx.h"
#include "AstroSchulz.h"

#include <array>

#include "mmcore/param/BoolParam.h"

#include "vislib/math/mathfunctions.h"


/*
 * megamol::astro::AstroSchulz::AstroSchulz
 */
megamol::astro::AstroSchulz::AstroSchulz(void) : Module(),
        frameID(0),
        hash(0),
        paramIncrementalRange("incrementalRange", "Do not reset the value range for every frame."),
        slotAstroData("astroData", "Input slot for astronomical data"),
        slotTableData("tableData", "Output slot for the resulting sphere data") {
    using megamol::stdplugin::datatools::table::TableDataCall;

    // Publish the slots.
    this->slotAstroData.SetCompatibleCall<AstroDataCallDescription>();
    this->MakeSlotAvailable(&this->slotAstroData);

    this->slotTableData.SetCallback(megamol::stdplugin::datatools::table::TableDataCall::ClassName(),
        megamol::stdplugin::datatools::table::TableDataCall::FunctionName(0), &AstroSchulz::getData);
    this->slotTableData.SetCallback(megamol::stdplugin::datatools::table::TableDataCall::ClassName(),
        megamol::stdplugin::datatools::table::TableDataCall::FunctionName(1), &AstroSchulz::getHash);
    this->MakeSlotAvailable(&this->slotTableData);

    this->paramIncrementalRange << new core::param::BoolParam(false);
    this->MakeSlotAvailable(&this->paramIncrementalRange);

    // Define the data format of the output.
    this->columns.reserve(21);

    this->columns.emplace_back();
    this->columns.back().SetName("PositionX");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("PositionY");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("PositionZ");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("VelocityX");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("VelocityY");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("VelocityZ");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Velocity");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Temperature");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Mass");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("InternalEnergy");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("SmoothingLength");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("MolecularWeight");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Density");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("GraviationalPotential");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Entropy");
    this->columns.back().SetType(TableDataCall::ColumnType::QUANTITATIVE);
    this->columns.back().SetMinimumValue(std::numeric_limits<float>::lowest());
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());

    this->columns.emplace_back();
    this->columns.back().SetName("Baryon");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0.0f);
    this->columns.back().SetMaximumValue(1.0f);

    this->columns.emplace_back();
    this->columns.back().SetName("Star");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0.0f);
    this->columns.back().SetMaximumValue(1.0f);

    this->columns.emplace_back();
    this->columns.back().SetName("Wind");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0.0f);
    this->columns.back().SetMaximumValue(1.0f);

    this->columns.emplace_back();
    this->columns.back().SetName("StarFormingGas");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0.0f);
    this->columns.back().SetMaximumValue(1.0f);

    this->columns.emplace_back();
    this->columns.back().SetName("ActiveGalacticNucleus");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0.0f);
    this->columns.back().SetMaximumValue(1.0f);

    this->columns.emplace_back();
    this->columns.back().SetName("ID");
    this->columns.back().SetType(TableDataCall::ColumnType::CATEGORICAL);
    this->columns.back().SetMinimumValue(0);
    this->columns.back().SetMaximumValue((std::numeric_limits<float>::max)());
}


/*
 * megamol::astro::AstroSchulz::~AstroSchulz
 */
megamol::astro::AstroSchulz::~AstroSchulz(void) {
    // TODO: This is toxic!
    this->Release();
}


/*
 * megamol::astro::AstroSchulz::create
 */
bool megamol::astro::AstroSchulz::create(void) {
    return true;
}


/*
 * megamol::astro::AstroSchulz::release
 */
void megamol::astro::AstroSchulz::release(void) { }


/*
 * megamol::astro::AstroSchulz::updateRange
 */
void megamol::astro::AstroSchulz::updateRange(std::pair<float, float>& range,
        const float value) {
    if (value < range.first) {
        range.first = value;
    }
    if (value > range.second) {
        range.second = value;
    }
}


/*
 * megamol::astro::AstroSchulz::convert
 */
void megamol::astro::AstroSchulz::convert(float* dst, const std::size_t col,
        const vec3ArrayPtr& src) {
    assert(dst != nullptr);
    assert(src != nullptr);

    std::array<std::pair<float, float>, 3> range = {
        AstroSchulz::initialiseRange(),
        AstroSchulz::initialiseRange(),
        AstroSchulz::initialiseRange()
    };

    for (auto s : *src) {
        for (std::size_t i = 0; i < s.length(); ++i) {
            dst[i] = s[i];

            AstroSchulz::updateRange(range[i], dst[i]);
            assert(range[i].first <= range[i].second);
            assert(dst[i] >= range[i].first);
            assert(dst[i] <= range[i].second);
        }

        dst += this->columns.size();
    }

    for (std::size_t i = 0; i < 3; ++i) {
        this->setRange(col + i, range[i]);
    }
}


/*
 * megamol::astro::AstroSchulz::convert
 */
void megamol::astro::AstroSchulz::convert(float* dst, const std::size_t col,
        const floatArrayPtr& src) {
    assert(dst != nullptr);
    assert(src != nullptr);
    auto range = AstroSchulz::initialiseRange();
    assert(range.first > range.second);

    for (auto s : *src) {
        *dst = s;

        AstroSchulz::updateRange(range, *dst);
        assert(range.first <= range.second);
        assert(s >= range.first);
        assert(s <= range.second);

        dst += this->columns.size();
    }

    this->setRange(col, range);
}


/*
 * megamol::astro::AstroSchulz::convert
 */
void megamol::astro::AstroSchulz::convert(float* dst, const std::size_t col,
        const boolArrayPtr& src) {
    assert(dst != nullptr);
    assert(src != nullptr);
    auto range = AstroSchulz::initialiseRange();

    for (auto s : *src) {
        *dst = s ? 1.0f : 0.0f;
        assert((*dst == 0.0f) || (*dst == 1.0f));

        AstroSchulz::updateRange(range, *dst);
        assert(range.first <= range.second);
        assert(*dst >= range.first);
        assert(*dst <= range.second);

        dst += this->columns.size();
    }

    // this->columns[col].SetMinimumValue(range.first);
    // this->columns[col].SetMaximumValue(range.second);
}


/*
 * megamol::astro::AstroSchulz::convert
 */
void megamol::astro::AstroSchulz::convert(float* dst, const std::size_t col,
        const idArrayPtr& src) {
    assert(dst != nullptr);
    assert(src != nullptr);
    auto range = AstroSchulz::initialiseRange();

    for (auto s : *src) {
        *dst = static_cast<float>(s);

        AstroSchulz::updateRange(range, *dst);
        assert(range.first <= range.second);
        assert(*dst >= range.first);
        assert(*dst <= range.second);

        dst += this->columns.size();
    }

    this->setRange(col, range);
}


/*
 * megamol::astro::AstroSchulz::getData
 */
bool megamol::astro::AstroSchulz::getData(core::Call& call) {
    using megamol::stdplugin::datatools::table::TableDataCall;

    auto ast = this->slotAstroData.CallAs<AstroDataCall>();
    auto tab = static_cast<TableDataCall*>(&call);
    auto retval = false;

    if (ast == nullptr) {
        return false;
    }
    if (tab == nullptr) {
        return false;
    }

    if ((*ast)(AstroDataCall::CallForGetExtent)) {
        tab->SetFrameCount(ast->FrameCount());
    }

    if (this->paramIncrementalRange.IsDirty()) {
        auto p = this->paramIncrementalRange.Param<core::param::BoolParam>();

        if (p->Value()) {
            this->ranges.resize(this->columns.size());
            std::generate(this->ranges.begin(), this->ranges.end(),
                AstroSchulz::initialiseRange);
        } else {
            this->ranges.resize(0);
        }

        this->paramIncrementalRange.ResetDirty();
    }

    ast->SetFrameID(tab->GetFrameID(), false);

    if ((*ast)(AstroDataCall::CallForGetData)) {
        if ((this->hash != ast->DataHash()) || (this->frameID == frameID)) {
            this->hash = ast->DataHash();
            this->frameID = frameID;
        }

        auto cnt = ast->GetParticleCount();
        this->values.resize(cnt * this->columns.size());

        auto col = 0;
        auto dst = this->values.data();

        this->convert(dst, col, ast->GetPositions());
        col += 3;
        dst += 3;

        this->convert(dst, col, ast->GetVelocities());
        col += 3;
        dst += 3;

        this->norm(dst, col, ast->GetVelocities());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetTemperature());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetMass());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetInternalEnergy());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetSmoothingLength());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetMolecularWeights());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetDensity());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetGravitationalPotential());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetEntropy());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetIsBaryonFlags());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetIsStarFlags());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetIsWindFlags());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetIsStarFormingGasFlags());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetIsAGNFlags());
        ++col;
        ++dst;

        this->convert(dst, col, ast->GetParticleIDs());
        ++col;
        ++dst;
        assert(col == this->columns.size());

        tab->SetDataHash(this->hash);
        tab->Set(this->columns.size(), cnt, this->columns.data(), this->values.data());

        retval = true;
    } /* end if ((*ast)(AstroDataCall::CallForGetData)) */

    tab->SetUnlocker(nullptr);
    return retval;
}


/*
 * megamol::astro::AstroSchulz::getHash
 */
bool megamol::astro::AstroSchulz::getHash(core::Call& call) {
    using megamol::stdplugin::datatools::table::TableDataCall;

    auto ast = this->slotAstroData.CallAs<AstroDataCall>();
    auto tab = dynamic_cast<TableDataCall*>(&call);
    if (tab == nullptr) {
        return false;
    }
    if (ast == nullptr) {
        return false;
    }

    //???? this->assertData();
    if (!(*ast)(AstroDataCall::CallForGetExtent)) {
        return false;
    }

    auto fc = ast->FrameCount();
    tab->SetFrameCount(ast->FrameCount());

    tab->SetDataHash(this->hash);
    tab->SetUnlocker(nullptr);

    return true;
}


/*
 * megamol::astro::AstroSchulz::norm
 */
void megamol::astro::AstroSchulz::norm(float* dst, const std::size_t col,
        const vec3ArrayPtr& src) {
    auto range = AstroSchulz::initialiseRange();

    for (auto s : *src) {
        *dst = glm::length(s);

        AstroSchulz::updateRange(range, *dst);
        assert(range.first <= range.second);
        assert(*dst >= range.first);
        assert(*dst <= range.second);

        dst += this->columns.size();
    }

    this->setRange(col, range);
}


/*
 * megamol::astro::AstroSchulz::setRange
 */
void megamol::astro::AstroSchulz::setRange(const std::size_t col,
        const std::pair<float, float>& src) {
    using megamol::stdplugin::datatools::table::TableDataCall;
    assert(col < this->columns.size());
    assert(src.first <= src.second);

    if (this->ranges.empty()) {
        this->columns[col].SetMinimumValue(src.first);
        this->columns[col].SetMaximumValue(src.second);

    } else if (this->columns[col].Type()
            == TableDataCall::ColumnType::QUANTITATIVE) {
        // Update the range over all frames and use this value.
        AstroSchulz::updateRange(this->ranges[col], src.first);
        AstroSchulz::updateRange(this->ranges[col], src.second);
        this->columns[col].SetMinimumValue(this->ranges[col].first);
        this->columns[col].SetMaximumValue(this->ranges[col].second);
    }
}
