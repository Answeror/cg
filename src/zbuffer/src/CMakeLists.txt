get_filename_component(PARENT_PATH ${CMAKE_CURRENT_SOURCE_DIR} PATH)
set(HEADER_PATH "${PARENT_PATH}/include")

include_directories("${HEADER_PATH}/core")
include_directories("${HEADER_PATH}/widgets")

file(GLOB_RECURSE HDRS "${HEADER_PATH}/*.hpp")
#file(GLOB_RECURSE SRCS *.cpp *.cu)
file(GLOB_RECURSE SRCS *.cpp)
set(FILES ${HDRS} ${SRCS})

group_source(RELATIVE "${PARENT_PATH}" ${FILES})

qt_support(QT_FILES QT_LIBS HEADER ${HDRS})
list(APPEND FILES ${QT_FILES})

boost_support()
cmlex_support()

set(TARGET zbuffer)
add_executable(${TARGET} ${FILES})
#cuda_add_executable(${TARGET} ${FILES})
target_link_libraries(${TARGET} ${QT_LIBS})

# for unit test {{{
add_definitions(-DCG_UNIT_TEST)
add_library(lib_${TARGET} STATIC ${FILES})
# }}}

ans_support(${TARGET})
ans_support(lib_${TARGET})
