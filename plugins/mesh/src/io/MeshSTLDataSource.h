#pragma once

#include "mmstd_datatools/io/AbstractSTLDataSource.h"

#include "mesh/MeshCalls.h"

namespace megamol::mesh::io {
class MeshSTLDataSource : public stdplugin::datatools::io::AbstractSTLDataSource<CallMesh> {
public:
    /// <summary>
    /// Answer the name of this module
    /// </summary>
    /// <returns>Module name</returns>
    static const char* ClassName() {
        return "MeshSTLDataSource";
    }

    /// <summary>
    /// Answer a human-readable description
    /// </summary>
    /// <returns>Description</returns>
    static const char* Description() {
        return "Reader for triangle data in STL files";
    }

    /// <summary>
    /// Answer the availability of this module
    /// </summary>
    /// <returns>Availability</returns>
    static bool IsAvailable() {
        return true;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    MeshSTLDataSource();

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~MeshSTLDataSource();

protected:
    /// <summary>
    /// Callback function for requesting information
    /// </summary>
    /// <param name="caller">Call for this request</param>
    /// <returns>True on success; false otherwise</returns>
    virtual bool get_extent_callback(core::Call& caller) override;

    /// <summary>
    /// Callback function for requesting data
    /// </summary>
    /// <param name="caller">Call for this request</param>
    /// <returns>True on success; false otherwise</returns>
    virtual bool get_mesh_data_callback(core::Call& caller) override;

private:
    std::shared_ptr<MeshDataAccessCollection> data_;

    std::vector<float> colors_;
};
} // namespace megamol::mesh::io