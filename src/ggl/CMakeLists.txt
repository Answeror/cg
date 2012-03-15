file(GLOB SRCS *.cpp)
file(GLOB HDRS *.hpp)
set(FILES ${SRCS} ${HDRS})

# boost header only support
boost_header_only_support()
cmlex_support()

add_executable(ggl ${FILES})
