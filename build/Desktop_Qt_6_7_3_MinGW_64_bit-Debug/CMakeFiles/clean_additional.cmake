# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ProyectDeimos_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ProyectDeimos_autogen.dir\\ParseCache.txt"
  "ProyectDeimos_autogen"
  )
endif()
