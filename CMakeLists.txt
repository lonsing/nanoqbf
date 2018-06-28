cmake_minimum_required(VERSION 3.10)
project(miniqbf)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -g)

include(${CMAKE_CURRENT_SOURCE_DIR}/AssureOutOfSourceBuilds.txt)
AssureOutOfSourceBuilds()

set(SOLVER_FILES main.cpp MiniQBF.cpp MiniQBF.h)
set(COMMON_FILES parseutils.h Reader.cpp Reader.h types/Assignment.cpp types/Assignment.h Logger.cpp Logger.h hashes.cpp hashes.h ipasir.h SatSolver.h types/Formula.cpp types/Formula.h types/common.h types/Quant.cpp types/Quant.h types/Clause.cpp types/Clause.h)
set(IPASIR_FILE ipasir-glucose4.cpp)

add_executable(miniqbf ${SOLVER_FILES} ${COMMON_FILES} ${IPASIR_FILE})
add_executable(test_types ${COMMON_FILES} test/test_types.cpp)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( miniqbf ${ZLIB_LIBRARIES} )
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

add_dependencies(miniqbf glucose)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glucose/src/glucose/glucose-syrup-4.1/")

add_library(libglucose STATIC IMPORTED)
set_target_properties(libglucose PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/lib/libglucose.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/glucose/src/glucose/glucose-syrup-4.1/"
        )

add_dependencies(libglucose glucose)
add_dependencies(miniqbf libglucose)

target_link_libraries(miniqbf libglucose)
