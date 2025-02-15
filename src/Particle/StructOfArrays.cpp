/* Copyright 2022 The AMReX Community
 *
 * Authors: Ryan Sandberg
 * License: BSD-3-Clause-LBNL
 */
#include "pyAMReX.H"

#include <AMReX_GpuAllocators.H>
#include <AMReX_StructOfArrays.H>

#include <sstream>


template <int NReal, int NInt,
          template<class> class Allocator=amrex::DefaultAllocator>
void make_StructOfArrays(py::module &m, std::string allocstr)
{
    using namespace amrex;

    using SOAType = StructOfArrays<NReal, NInt, Allocator>;

    auto const soa_name = std::string("StructOfArrays_") + std::to_string(NReal) + "_" +
                          std::to_string(NInt) + "_" + allocstr;
    py::class_<SOAType>(m, soa_name.c_str())
        .def(py::init())
        .def("define", &SOAType::define)
        .def("NumRealComps", &SOAType::NumRealComps,
             "Get the number of compile-time + runtime Real components")
        .def("NumIntComps", &SOAType::NumIntComps,
             "Get the number of compile-time + runtime Int components")

        // compile-time components
        .def("GetRealData", py::overload_cast<>(&SOAType::GetRealData),
            py::return_value_policy::reference_internal,
            "Get access to the particle Real Arrays (only compile-time components)")
        .def("GetIntData", py::overload_cast<>(&SOAType::GetIntData),
            py::return_value_policy::reference_internal,
            "Get access to the particle Int Arrays (only compile-time components)")
        // compile-time and runtime components
        .def("GetRealData", py::overload_cast<const int>(&SOAType::GetRealData),
             py::return_value_policy::reference_internal,
             py::arg("index"),
             "Get access to a particle Real component Array (compile-time and runtime component)")
        .def("GetIntData", py::overload_cast<const int>(&SOAType::GetIntData),
             py::return_value_policy::reference_internal,
             py::arg("index"),
             "Get access to a particle Real component Array (compile-time and runtime component)")

        .def("size", &SOAType::size,
             "Get the number of particles")
        .def("__len__", &SOAType::size,
             "Get the number of particles")
        .def("numParticles", &SOAType::numParticles)
        .def("numRealParticles", &SOAType::numRealParticles)
        .def("numTotalParticles", &SOAType::numTotalParticles)
        .def("setNumNeighbors", &SOAType::setNumNeighbors)
        .def("getNumNeighbors", &SOAType::getNumNeighbors)
        .def("resize", &SOAType::resize)
    ;
}

template <int NReal, int NInt>
void make_StructOfArrays(py::module &m)
{
    // see Src/Base/AMReX_GpuContainers.H
    //   !AMREX_USE_GPU: DefaultAllocator = std::allocator
    //    AMREX_USE_GPU: DefaultAllocator = amrex::ArenaAllocator

    //   work-around for https://github.com/pybind/pybind11/pull/4581
    //make_StructOfArrays<NReal, NInt, std::allocator>(m, "std");
    //make_StructOfArrays<NReal, NInt, amrex::ArenaAllocator>(m, "arena");
#ifdef AMREX_USE_GPU
    make_StructOfArrays<NReal, NInt, std::allocator>(m, "std");
    make_StructOfArrays<NReal, NInt, amrex::DefaultAllocator> (m, "default");  // amrex::ArenaAllocator
#else
    make_StructOfArrays<NReal, NInt, amrex::DefaultAllocator> (m, "default");  // std::allocator
    make_StructOfArrays<NReal, NInt, amrex::ArenaAllocator>(m, "arena");
#endif
    //   end work-around
    make_StructOfArrays<NReal, NInt, amrex::PinnedArenaAllocator>(m, "pinned");
#ifdef AMREX_USE_GPU
    make_StructOfArrays<NReal, NInt, amrex::DeviceArenaAllocator>(m, "device");
    make_StructOfArrays<NReal, NInt, amrex::ManagedArenaAllocator>(m, "managed");
    make_StructOfArrays<NReal, NInt, amrex::AsyncArenaAllocator>(m, "async");
#endif
}

void init_StructOfArrays(py::module& m) {
    make_StructOfArrays< 2, 1>(m);
    make_StructOfArrays< 4, 0>(m);  // HiPACE++ 22.08+
    make_StructOfArrays< 5, 0>(m);  // ImpactX 22.07 - 23.08
    make_StructOfArrays< 8, 2>(m);  // ImpactX 23.08+
    make_StructOfArrays<37, 1>(m);  // HiPACE++ 22.08+
}
