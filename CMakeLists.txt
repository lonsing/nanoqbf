cmake_minimum_required(VERSION 3.10)
project(nanoqbf)

set(CMAKE_CXX_STANDARD 11)
add_compile_options(-Wall -Wextra -g)

include(${CMAKE_CURRENT_SOURCE_DIR}/AssureOutOfSourceBuilds.txt)
AssureOutOfSourceBuilds()

set(SOLVER_FILES main.cpp NanoQBF.cpp NanoQBF.h)
set(TYPE_FILES types/Assignment.cpp types/Assignment.h types/Formula.cpp types/Formula.h types/common.h types/Quant.cpp types/Quant.h types/Clause.cpp types/Clause.h)
set(COMMON_FILES parseutils.h Reader.cpp Reader.h Logger.cpp Logger.h hashes.cpp hashes.h Options.cpp Options.h auxutils.h)
set(SAT_FILES ipasir-glucose4.cpp ipasir.h SatSolver.h)

add_executable(nanoqbf ${SOLVER_FILES} ${TYPE_FILES} ${COMMON_FILES} ${SAT_FILES})
add_executable(test_types Logger.cpp Logger.h ${TYPE_FILES} test/test_types.cpp)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( nanoqbf ${ZLIB_LIBRARIES} )
    target_link_libraries( test_types ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

include(ExternalProject)

ExternalProject_Add(glucose
        URL "http://www.labri.fr/perso/lsimon/downloads/softwares/glucose-syrup-4.1.tgz"
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glucose
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/glucose/src/glucose/glucose-syrup-4.1/simp/ &&
        make libr &&
        mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/lib/ &&
        mv lib_release.a ${CMAKE_CURRENT_BINARY_DIR}/lib/libglucose.a &&
        cd ${CMAKE_CURRENT_BINARY_DIR}/
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        INSTALL_COMMAND ""
        )

add_dependencies(nanoqbf glucose)

ExternalProject_Add(cxxopts
        GIT_REPOSITORY "https://github.com/jarro2783/cxxopts"
        GIT_TAG "0fe1dc892bcbef48b3911510209c7033f31ce05e"
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cxxopts
        BUILD_COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/include/ &&
        cp ${CMAKE_CURRENT_BINARY_DIR}/cxxopts/src/cxxopts/include/cxxopts.hpp ${CMAKE_CURRENT_BINARY_DIR}/include/cxxopts.h
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        INSTALL_COMMAND ""
        )

add_dependencies(nanoqbf cxxopts)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)


file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glucose/src/glucose/glucose-syrup-4.1/")
add_library(libglucose STATIC IMPORTED)
set_target_properties(libglucose PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/lib/libglucose.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/glucose/src/glucose/glucose-syrup-4.1/"
        )

add_dependencies(libglucose glucose)
add_dependencies(nanoqbf libglucose)

target_link_libraries(nanoqbf libglucose)
