# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\StudentTopicSystemClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\StudentTopicSystemClient_autogen.dir\\ParseCache.txt"
  "StudentTopicSystemClient_autogen"
  )
endif()
