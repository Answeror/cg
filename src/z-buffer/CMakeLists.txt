project(z-buffer)

file(GLOB_RECURSE HDRS *.hpp)
file(GLOB_RECURSE SRCS *.cpp)
set(FILES ${HDRS} ${SRCS})

qt_support(QT_FILES QT_LIBS)
list(APPEND FILES ${QT_FILES})

boost_header_only_support()
cmlex_support()

set(TARGET z-buffer)
add_executable(${TARGET} ${FILES})

ans_support(${TARGET})
