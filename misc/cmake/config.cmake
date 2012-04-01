#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/misc/3rd/cmake)
find_package(ACMake REQUIRED HINTS ${CMAKE_SOURCE_DIR}/misc/3rd/acmake)
include(ans.common)
