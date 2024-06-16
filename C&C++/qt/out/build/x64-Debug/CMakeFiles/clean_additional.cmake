# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\mygame_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\mygame_autogen.dir\\ParseCache.txt"
  "mygame_autogen"
  )
endif()
