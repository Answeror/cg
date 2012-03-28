get_filename_component(PARENT_PATH ${CMAKE_CURRENT_SOURCE_DIR} PATH)
set(HEADER_PATH "${PARENT_PATH}/include")

include_directories("${HEADER_PATH}/*")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE HDRS "${HEADER_PATH}/*.hpp")
file(GLOB_RECURSE SRCS *.hpp *.cpp *.cu)
set(FILES ${HDRS} ${SRCS})

group_source(RELATIVE "${PARENT_PATH}" ${FILES})

qt_support(QT_FILES QT_LIBS HEADER ${HDRS})
list(APPEND FILES ${QT_FILES})

boost_support()
include(ans.cmlex_support)
cmlex_support()

include(ans.cuda_support)
cuda_support()
cuda_add_executable(radiosity ${FILES})

include(ans.openmesh_support)
openmesh_support(radiosity)
include(ans.gl_support)
gl_support(radiosity GLEW GLUT)
include(ans.ans_support)
ans_support(radiosity)
