#pragma once

#include "mmcore/Call.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"
#include "mmcore/moldyn/MultiParticleDataCall.h"
#include "mmcore/param/ParamSlot.h"

//// CGAL
//#include "CGAL/Exact_predicates_exact_constructions_kernel.h"
//#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
//
//#include "CGAL/Delaunay_triangulation_3.h"
//#include "CGAL/Fixed_alpha_shape_3.h"
//#include "CGAL/Fixed_alpha_shape_cell_base_3.h"
//#include "CGAL/Fixed_alpha_shape_vertex_base_3.h"
//
//#include "CGAL/Min_sphere_d.h"
//#include "CGAL/Min_sphere_of_points_d_traits_d.h"
//
//#include "CGAL/Triangulation_vertex_base_with_info_3.h"
//// END CGAL

#include "SurfaceHelper.h"

#include "mesh/MeshCalls.h"

namespace megamol::thermodyn {
class ParticleSurface : public core::Module {
public:
    /** Return module class name */
    static const char* ClassName(void) {
        return "ParticleSurface";
    }

    /** Return module class description */
    static const char* Description(void) {
        return "Determines the surface of MD data";
    }

    /** Module is always available */
    static bool IsAvailable(void) {
        return true;
    }

    ParticleSurface();

    virtual ~ParticleSurface();

    //using Gt = CGAL::Exact_predicates_inexact_constructions_kernel;
    //// using Gt = CGAL::Exact_predicates_exact_constructions_kernel;

    //using Vbt = CGAL::Triangulation_vertex_base_with_info_3<std::size_t, Gt>;
    //using Vb = CGAL::Fixed_alpha_shape_vertex_base_3<Gt, Vbt>;
    //using Fb = CGAL::Fixed_alpha_shape_cell_base_3<Gt>;
    //using Tds = CGAL::Triangulation_data_structure_3<Vb, Fb>;
    //using Triangulation_3 = CGAL::Delaunay_triangulation_3<Gt, Tds>;
    //using Alpha_shape_3 = CGAL::Fixed_alpha_shape_3<Triangulation_3>;

    //using Delaunay = CGAL::Delaunay_triangulation_3<Gt>;
    //using DFacet = Delaunay::Facet;

    //// using Point_3 = Gt::Point_3;
    //using Point_3 = Alpha_shape_3::Point;

    //// using Triangle = Alpha_shape_3::Triangle;
    //using Triangle = Triangulation_3::Triangle;
    //using Facet = Alpha_shape_3::Facet;
    //using Vertex = Alpha_shape_3::Vertex_handle;

protected:
    bool create() override;

    void release() override;

private:
    /*using Traits = CGAL::Min_sphere_of_points_d_traits_d<Gt, Point_3::FT, 3>;
    using Min_sphere = CGAL::Min_sphere_d<Traits>;*/

    enum class surface_type : std::uint8_t { alpha_shape, gtim };

    bool is_dirty() {
        return _alpha_slot.IsDirty() || _type_slot.IsDirty() || _vert_type_slot.IsDirty();
    }

    void reset_dirty() {
        _alpha_slot.ResetDirty();
        _type_slot.ResetDirty();
        _vert_type_slot.ResetDirty();
    }

    bool assert_data(core::moldyn::MultiParticleDataCall& call);

    bool get_data_cb(core::Call& c);

    bool get_extent_cb(core::Call& c);

    bool get_part_data_cb(core::Call& c);

    bool get_part_extent_cb(core::Call& c);

    core::CalleeSlot _out_mesh_slot;

    core::CalleeSlot _out_part_slot;

    core::CallerSlot _in_data_slot;

    core::CallerSlot _tf_slot;

    core::CallerSlot _flags_read_slot;

    core::param::ParamSlot _alpha_slot;

    core::param::ParamSlot _type_slot;

    core::param::ParamSlot _vert_type_slot;

    std::vector<std::vector<float>> _vertices;

    std::vector<std::vector<float>> _normals;

    std::vector<std::vector<float>> _colors;

    std::vector<std::vector<float>> _unsel_colors;

    std::vector<std::vector<std::uint32_t>> _indices;

    std::shared_ptr<mesh::MeshDataAccessCollection> _mesh_access_collection;

    unsigned int _frame_id = std::numeric_limits<unsigned int>::max();

    std::size_t _in_data_hash = std::numeric_limits<std::size_t>::max();

    std::size_t _out_data_hash = 0;

    std::vector<std::shared_ptr<Alpha_shape_3>> _alpha_shapes;

    std::vector<std::list<Facet>> _facets;

    std::vector<std::list<Vertex>> _as_vertices;

    std::vector<std::vector<float>> _part_data;

    int _fcr_version = -1;

    std::vector<std::tuple<size_t, size_t>> _sel;
};
} // namespace megamol::thermodyn