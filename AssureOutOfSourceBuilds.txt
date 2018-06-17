
function(AssureOutOfSourceBuilds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if(${srcdir} STREQUAL ${bindir})
    message("######################################################")
    message("You are attempting to build in your Source Directory.")
    message("You must run cmake from a build directory.")
    message("######################################################")
    file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/CMakeCache.txt" "${CMAKE_SOURCE_DIR}/CMakeFiles")
    message(FATAL_ERROR "Attempted to run an in-source build.")
  endif()

endfunction()