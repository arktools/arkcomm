# - Try to find  OSGEARTH
# Once done, this will define
#
#  OSGEARTH_FOUND - system has scicoslab 
#  OSGEARTH_INCLUDE_DIRS - the scicoslab include directories
#  OSGEARTH_LIBRARIES - libraries to link to

include(LibFindMacros)
include(MacroCommonPaths)

MacroCommonPaths(OSGEARTH)

# Include dir
find_path(OSGEARTH_INCLUDE_DIR
    NAMES osgearth/header.h
    PATHS ${COMMON_INCLUDE_PATHS_OSGEARTH}
)

# data dir
find_path(OSGEARTH_DATA_DIR_SEARCH
    NAMES osgearth/data/file.dat
    PATHS ${COMMON_DATA_PATHS_OSGEARTH}
)
set(OSGEARTH_DATA_DIR ${OSGEARTH_DATA_DIR_SEARCH}/jsbsim)

# Finally the library itself
find_library(OSGEARTH_LIBRARY
    NAMES osgearth
    PATHS ${COMMON_LIBRARY_PATHS_OSGEARTH}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(OSGEARTH_PROCESS_INCLUDES OSGEARTH_INCLUDE_DIR)
set(OSGEARTH_PROCESS_LIBS OSGEARTH_LIBRARY OSGEARTH_LIBRARIES)
set(OSGEARTH_INCLUDE_DIR ${OSGEARTH_INCLUDE_DIR})
set(OSGEARTH_INCLUDES ${OSGEARTH_INCLUDES})

libfind_process(OSGEARTH)

macro(build_osgearth TAG EP_BASE_DIR)
    if(NOT OSGEARTH_FOUND)
        set(CMAKE_ARGS 
            -DEP_BASE_DIR=${EP_BASE_DIR}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            )
        if (WITH_BUILD_STATIC)
            list(APPEND CMAKE_ARGS -DDYNAMIC_OSGEARTH=n)
        endif()
        ExternalProject_Add(osgearth
            GIT_REPOSITORY "git://github.com/gwaldron/osgearth.git"
            GIT_TAG ${TAG}
            PATCH_COMMAND sed -i s/FIND_PACKAGE\(OSG\)/FIND_PACKAGE\(OpenSceneGraph\)/g CMakeLists.txt
            INSTALL_DIR ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}
            CMAKE_ARGS ${CMAKE_ARGS}
            INSTALL_COMMAND make DESTDIR=${EP_BASE_DIR} install
            )
        set(OSGEARTH_INCLUDE_DIRS  ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/include)
        set(OSGEARTH_DATA_DIR ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/share/osgearth)
        if (WITH_BUILD_STATIC)
            set(OSGEARTH_LIBRARIES ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/lib/libosgearth.a)
        else()
            if (WIN32)
                set(OSGEARTH_LIBRARIES ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/lib/libosgearth.dll)
            elseif(UNIX)
                set(OSGEARTH_LIBRARIES ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/lib/libosgearth.so)
            endif()
        endif()
        set(OSGEARTH_FOUND TRUE)
    endif()
endmacro()
