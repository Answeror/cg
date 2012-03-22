get_filename_component(PARENT_PATH ${CMAKE_CURRENT_SOURCE_DIR} PATH)
set(HEADER_PATH "${PARENT_PATH}/include")

include_directories("${HEADER_PATH}/core")

file(GLOB_RECURSE HDRS "${HEADER_PATH}/*.hpp")
file(GLOB_RECURSE SRCS *.cpp *.cu)
set(FILES ${HDRS} ${SRCS})

group_source(RELATIVE "${PARENT_PATH}" ${FILES})

qt_support(QT_FILES QT_LIBS HEADER ${HDRS})
list(APPEND FILES ${QT_FILES})

boost_header_only_support()
cmlex_support()

find_package(CUDA REQUIRED)
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=sm_10;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_10,code=compute_10;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=sm_11;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_11,code=compute_11;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=sm_12;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_12,code=compute_12;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=sm_13;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_13,code=compute_13;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=sm_20;${CUDA_NVCC_FLAGS}")
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_20,code=compute_20;${CUDA_NVCC_FLAGS}")

set(TARGET zbuffer)
#add_executable(${TARGET} ${FILES})
cuda_add_executable(${TARGET} ${FILES})
target_link_libraries(${TARGET} ${QT_LIBS})

# for unit test {{{
add_definitions(-DCG_UNIT_TEST)
add_library(lib_${TARGET} STATIC ${FILES})
# }}}

ans_support(${TARGET})
ans_support(lib_${TARGET})
