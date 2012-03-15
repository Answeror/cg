project(z-buffer)

include_directories(include)

file(GLOB_RECURSE HDRS include/*.hpp)
file(GLOB_RECURSE SRCS src/*.cpp)
set(FILES ${HDRS} ${SRCS})

qt_support(QT_FILES QT_LIBS)
list(APPEND FILES ${QT_FILES})

boost_header_only_support()
cmlex_support()

set(TARGET zbuffer)
add_executable(${TARGET} ${FILES})
add_library(lib_${TARGET} STATIC ${FILES})

ans_support(${TARGET})
ans_support(lib_${TARGET})

add_subdirectory(test)
