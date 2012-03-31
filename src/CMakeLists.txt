#add_subdirectory(ggl)

if(CG_BUILD_ZBUFFER)
    add_subdirectory(zbuffer)
endif()

if(CG_BUILD_RADIOSITY)
    add_subdirectory(radiosity)
endif()
