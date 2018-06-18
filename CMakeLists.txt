cmake_minimum_required(VERSION 3.10)
project(miniqbf)

set(CMAKE_CXX_STANDARD 14)


include(${CMAKE_CURRENT_SOURCE_DIR}/AssureOutOfSourceBuilds.txt)
AssureOutOfSourceBuilds()

set(SOLVER_FILES main.cpp MiniQBF.cpp MiniQBF.h)
set(COMMON_FILES parseutils.h Reader.cpp Reader.h types.cpp types.h Logger.cpp Logger.h hashes.cpp hashes.h)

add_executable(miniqbf ${SOLVER_FILES} ${COMMON_FILES})
add_executable(test_types ${COMMON_FILES} test/test_types.cpp)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( miniqbf ${ZLIB_LIBRARIES} )
    target_link_libraries( test_types ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )