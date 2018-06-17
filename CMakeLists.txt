cmake_minimum_required(VERSION 3.10)
project(miniqbf)

set(CMAKE_CXX_STANDARD 14)


include(${CMAKE_CURRENT_SOURCE_DIR}/AssureOutOfSourceBuilds.txt)
AssureOutOfSourceBuilds()

set(SOURCE_FILES main.cpp MiniQBF.cpp MiniQBF.h ParseUtils.h Reader.cpp Reader.h Types.cpp Types.h)

add_executable(miniqbf ${SOURCE_FILES})

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( miniqbf ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )