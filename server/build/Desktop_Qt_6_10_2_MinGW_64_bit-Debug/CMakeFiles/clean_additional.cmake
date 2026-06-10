# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\StudentTopicSystemServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\StudentTopicSystemServer_autogen.dir\\ParseCache.txt"
  "StudentTopicSystemServer_autogen"
  )
endif()
