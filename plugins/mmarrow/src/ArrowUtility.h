#pragma once

#include <iterator>
#include <vector>

#include <arrow/api.h>

namespace megamol::mmarrow {
template<typename T>
struct getArrowArrayDataType {
    static_assert(false, "Unsupported target type");
};

template<>
struct getArrowArrayDataType<float> {
    using type = arrow::FloatArray;
};
template<>
struct getArrowArrayDataType<double> {
    using type = arrow::DoubleArray;
};
template<>
struct getArrowArrayDataType<int32_t> {
    using type = arrow::Int32Array;
};
template<>
struct getArrowArrayDataType<int64_t> {
    using type = arrow::Int64Array;
};
template<>
struct getArrowArrayDataType<uint32_t> {
    using type = arrow::UInt32Array;
};
template<>
struct getArrowArrayDataType<uint64_t> {
    using type = arrow::UInt64Array;
};

template<typename Ttarget>
std::shared_ptr<arrow::DataType> getArrowDataType() {
    if constexpr (std::is_same_v<Ttarget, float>) {
        return arrow::float32();
    } else if constexpr (std::is_same_v<Ttarget, double>) {
        return arrow::float64();
    } else if constexpr (std::is_same_v<Ttarget, int32_t>) {
        return arrow::int32();
    } else if constexpr (std::is_same_v<Ttarget, int64_t>) {
        return arrow::int64();
    } else if constexpr (std::is_same_v<Ttarget, uint32_t>) {
        return arrow::uint32();
    } else if constexpr (std::is_same_v<Ttarget, uint64_t>) {
        return arrow::uint64();
    } else {
        static_assert(always_false<Ttarget>::value, "Unsupported target type");
    }
}

template<typename Tarray, typename Tvector>
inline void copyArrayToVector(
    std::shared_ptr<Tarray> const& array, std::back_insert_iterator<std::vector<Tvector>>& out_data) {
    std::copy(array->raw_values(), array->raw_values() + array->length(), out_data);
}

template<typename Tarray, typename Tvector, typename Predicate>
inline void transformArrayToVector(
    std::shared_ptr<Tarray> const& array, std::back_insert_iterator<std::vector<Tvector>>& out_data, Predicate pred) {
    std::transform(array->begin(), array->end(), out_data, pred);
}

template<typename Tarray, typename Tvector>
inline void castAndCopyArrayToVector(
    std::shared_ptr<arrow::Array> const& array, std::back_insert_iterator<std::vector<Tvector>>& out_data) {
    auto const typed_array = std::static_pointer_cast<getArrowArrayDataType<Tarray>::type>(array);
    if constexpr (std::is_same_v<Tarray, Tvector>) {
        copyArrayToVector(typed_array, out_data);
        return;
    }
    transformArrayToVector(typed_array, out_data, [](auto const& value) {
        if (value.has_value()) {
            return static_cast<Tvector>(value.value());
        } else {
            return std::numeric_limits<Tvector>::quiet_NaN();
        }
    });
}

template<typename Ttarget>
inline void convertArrowArrayToVector(
    std::shared_ptr<arrow::Array> const& array, std::back_insert_iterator<std::vector<Ttarget>> out_data) {
    auto const type_id = array->type_id();

    switch (type_id) {
    case arrow::Type::FLOAT: {
        castAndCopyArrayToVector<float>(array, out_data);
        break;
    }
    case arrow::Type::DOUBLE: {
        castAndCopyArrayToVector<double>(array, out_data);
        break;
    }
    case arrow::Type::INT32: {
        castAndCopyArrayToVector<int32_t>(array, out_data);
        break;
    }
    case arrow::Type::INT64: {
        castAndCopyArrayToVector<int64_t>(array, out_data);
        break;
    }
    case arrow::Type::UINT32: {
        castAndCopyArrayToVector<uint32_t>(array, out_data);
        break;
    }
    case arrow::Type::UINT64: {
        castAndCopyArrayToVector<uint64_t>(array, out_data);
        break;
    }
    default:
        throw std::runtime_error("Unsupported data type");
    }
}
} // namespace megamol::mmarrow

#include "ArrowUtility.inl"
