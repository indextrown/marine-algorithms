#include "marine_c.h"

#include "marine/geodesy.hpp"
#include "marine/units.hpp"

namespace {

namespace geo = marine::geodesy;

geo::Coordinate to_cpp(ma_coordinate c) noexcept { return {c.latitude, c.longitude}; }
geo::Ellipsoid to_cpp(ma_ellipsoid e) noexcept { return {e.semi_major_axis, e.flattening}; }

ma_coordinate to_c(geo::Coordinate c) noexcept { return {c.latitude, c.longitude}; }
ma_inverse to_c(const geo::InverseSolution& r) noexcept {
    return {r.distance, r.initial_bearing, r.final_bearing};
}
ma_direct to_c(const geo::DirectSolution& r) noexcept { return {to_c(r.destination), r.final_bearing}; }

ma_status to_c(geo::GeodesyError error) noexcept {
    switch (error) {
    case geo::GeodesyError::not_converged: return MA_NOT_CONVERGED;
    }
    return MA_NOT_CONVERGED;
}

} // namespace

extern "C" {

ma_ellipsoid ma_wgs84(void) { return {geo::kWGS84.semi_major_axis, geo::kWGS84.flattening}; }

double ma_meters_to_nautical_miles(double meters) { return marine::units::meters_to_nautical_miles(meters); }

double ma_nautical_miles_to_meters(double nautical_miles) {
    return marine::units::nautical_miles_to_meters(nautical_miles);
}

double ma_knots_to_meters_per_second(double knots) { return marine::units::knots_to_meters_per_second(knots); }

double ma_meters_per_second_to_knots(double meters_per_second) {
    return marine::units::meters_per_second_to_knots(meters_per_second);
}

double ma_haversine_distance(ma_coordinate from, ma_coordinate to) {
    return geo::haversine_distance(to_cpp(from), to_cpp(to));
}

double ma_initial_bearing(ma_coordinate from, ma_coordinate to) {
    return geo::initial_bearing(to_cpp(from), to_cpp(to));
}

ma_status ma_vincenty_inverse(ma_coordinate from, ma_coordinate to, ma_ellipsoid ellipsoid,
                              ma_inverse* out) {
    if (!out) return MA_INVALID_ARGUMENT;
    const auto r = geo::vincenty_inverse(to_cpp(from), to_cpp(to), to_cpp(ellipsoid));
    if (!r) return to_c(r.error());
    *out = to_c(*r);
    return MA_OK;
}

ma_status ma_vincenty_direct(ma_coordinate from, double bearing, double distance,
                             ma_ellipsoid ellipsoid, ma_direct* out) {
    if (!out) return MA_INVALID_ARGUMENT;
    const auto r = geo::vincenty_direct(to_cpp(from), bearing, distance, to_cpp(ellipsoid));
    if (!r) return to_c(r.error());
    *out = to_c(*r);
    return MA_OK;
}

} // extern "C"
