file(GLOB FILES *.cpp)

add_executable(test_zbuffer ${FILES})
target_link_libraries(test_zbuffer lib_zbuffer ${QT_LIBS})

boost_support(test_zbuffer)

add_test(test_zbuffer ${CMAKE_BINARY_DIR}/test_zbuffer)
