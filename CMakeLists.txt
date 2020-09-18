cmake_minimum_required(VERSION 3.8)
project(CRP)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

include_directories(/usr/local/include/)
link_directories(/usr/local/lib/)

FILE(GLOB_RECURSE STRUCTURE_SOURCE_FILES
        datastructures/*.cpp
        datastructures/*.h
        )

FILE(GLOB_RECURSE ALGORITHM_SOURCE_FILES
        algorithm/*.cpp
        algorithm/*.h
        )

FILE(GLOB_RECURSE IO_SOURCE_FILES
        io/*.cpp
        io/*.h
        )

add_library(crp STATIC ${STRUCTURE_SOURCE_FILES} ${ALGORITHM_SOURCE_FILES} ${IO_SOURCE_FILES})

# pre-processing
add_executable(osm2graph precalculation/OSM2Graph.cpp)
target_link_libraries(osm2graph crp boost_iostreams)

add_executable(pre_calculation precalculation/Precalculation.cpp)
target_link_libraries(pre_calculation crp boost_iostreams)

# customization
add_executable(customization customization/Customization.cpp)
target_link_libraries(customization crp boost_iostreams)

# test
add_executable(dijkstra_test test/DijkstraTest.cpp)
target_link_libraries(dijkstra_test crp boost_iostreams)

add_executable(overlay_graph_test test/OverlayGraphTest.cpp)
target_link_libraries(overlay_graph_test crp boost_iostreams)

add_executable(query_test test/QueryTest.cpp)
target_link_libraries(query_test crp boost_iostreams)

add_executable(unpack_path test/UnpackPathTest.cpp)
target_link_libraries(unpack_path crp boost_iostreams)
