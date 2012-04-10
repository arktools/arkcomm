project (arkcomm C CXX)

# settings
cmake_minimum_required (VERSION 2.8)
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "8")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_CONTACT_EMAIL james.goppert@gmail.com)
set(PROJECT_CONTACT_VENDOR arkTools)
set(LIBRARY_VERSION ${PROJECT_VERSION})
set(LIBRARY_SOVERSION "0.0.0")

# third party libraries
set(BOOST_REQUIRED_VERSION 1.4.2)
set(SIMGEAR_REQUIRED_VERSION 2.7.0)
set(MAVLINK_REQUIRED_VERSION 1.0.4)

# tags
set(SIMGEAR_TAG 43ab2b6ecb31fb6ca72f3cfdb4395b352a95e365)
set(MAVLINK_TAG 112cad9a4f6dddd0e65472fdc2a0956ab39da0e6)

# options
option(WITH_BUILD_DEPS "Build dependencies." ON)
option(WITH_BUILD_STATIC "Prefer static linking." ON)
option(WITH_MAVLINK "Build with mavlink support." ON)
option(WITH_FLIGHTGEAR "Build with flightgear support." ON)
option(WITH_BUILD_SHARED "Build the shared library." OFF)
option(WITH_BUILD_TESTS "Build tests." OFF)
option(WITH_WARNINGS "Enable warnings." OFF)

set(RECURSE_OPTIONS_LIST
    WITH_BUILD_DEPS
    WITH_BUILD_STATIC
    WITH_MAVLINK
    WITH_FLIGHTGEAR
    WITH_BUILD_SHARED
    WITH_BUILD_TESTS
    WITH_WARNINGS
    CMAKE_TOOLCHAIN_FILE
    CMAKE_INSTALL_PREFIX
    EP_BASE_DIR
) 

# variables
set(ROOT_THREAD TRUE CACHE INTERNAL "Is this the top level of the recursion?")
if (MINGW)
    set(INSTALL_DATA_DIR "C:/Program Files/${PROJECT_NAME} ${PROJECT_VERSION}/share/${PROJECT_NAME}")
endif()

# modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR}/cmake/arkcmake)
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(BundleUtilities)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckLibraryExists)
#include(CheckTypeSize)
#include(CheckPrototypeExists)
#include(CheckCXXSourceCompiles)
#include(CheckCSourceCompiles)
include(ExternalProjectWithFilename)

# external projects find path
if(NOT EP_BASE_DIR)
    set(EP_BASE_DIR "${CMAKE_BINARY_DIR}/CMakeExternals")
endif()
set_property(DIRECTORY PROPERTY EP_BASE ${EP_BASE_DIR})
set(EP_INSTALL_DIR "${EP_BASE_DIR}/Install")
list(APPEND CMAKE_FIND_ROOT_PATH ${EP_INSTALL_DIR})

# spawn new cmake to build deps
if (WITH_BUILD_DEPS AND ROOT_THREAD)
    set(CMAKE_RECURSE_ARGS "-DROOT_THREAD=FALSE")
    foreach(OPT ${RECURSE_OPTIONS_LIST})
        list(APPEND CMAKE_RECURSE_ARGS "-D${OPT}=${${OPT}}")
    endforeach()
    execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} ${CMAKE_RECURSE_ARGS}
        RESULT_VARIABLE ERROR)
    if (ERROR)
        message(FATAL_ERROR "error, recursing loop returned error code: ${ERROR}")
    endif()
    message("** Making dependencies")
    execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} "-j4" "-f${CMAKE_BINARY_DIR}/Makefile")
    message("** Configuring ${PROJECT_NAME}")
endif()

# prefer static packages if building static library
message("** Finding libraries")
if (WITH_BUILD_STATIC)
    set(BOOST_USE_STATIC_LIBS ON)
    # prefer static libs
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()
endif()

# find libraries with cmake modules
set(REQUIRED_IF_ROOT_THREAD)
if (ROOT_THREAD)
    set(REQUIRED_IF_ROOT_THREAD "REQUIRED")
endif()
if (WIN32)
    find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED COMPONENTS thread_win32-mt system-mt)
else()
    find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED COMPONENTS thread-mt system-mt)
endif()
if (WITH_FLIGHTGEAR)
    find_package(SimGear ${SIMGEAR_REQUIRED_VERSION} ${REQUIRED_IF_ROOT_THREAD} COMPONENTS io math timing debug)
endif()
find_package(Threads REQUIRED)
find_package(MAVLink ${MAVLINK_REQUIRED_VERSION} ${REQUIRED_IF_ROOT_THREAD})

# build dependencies
if (WITH_BUILD_DEPS AND (NOT ROOT_THREAD) )
    message("** Configuring dependencies")

    # add external projects

    set(CMAKE_EP_DEFAULT_ARGS
        -DEP_BASE_DIR=${EP_BASE_DIR}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        )

    if (WITH_MAVLINK AND (NOT MAVLINK_FOUND))
        ExternalProjectWithFilename_Add(mavlink
            URL http://nodeload.github.com/mavlink/mavlink/tarball/${MAVLINK_TAG}
            FILENAME mavlink-${MAVLINK_TAG}.tar.gz
            CMAKE_ARGS ${CMAKE_EP_DEFAULT_ARGS}
            INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} DESTDIR=${EP_INSTALL_DIR} install
            INSTALL_DIR ${EP_INSTALL_DIR}
           )
    endif()

    if (WITH_FLIGHTGEAR AND (NOT SIMGEAR_FOUND))
        ExternalProjectWithFilename_Add(simgear
            URL http://gitorious.org/fg/jgopperts-simgear/archive-tarball/${SIMGEAR_TAG}
            FILENAME jgoppert-jgopperts-simgear-${SIMGEAR_TAG}.tar.gz
            CMAKE_ARGS ${CMAKE_EP_DEFAULT_ARGS} "-DCMAKE_FIND_ROOT=/opt/local" "-DSIMGEAR_HEADLESS=ON" "-DENABLE_TESTS=OFF"
            INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} DESTDIR=${EP_INSTALL_DIR} install
            INSTALL_DIR ${E_INSTALL_DIR}
           )
    endif()

    return()

endif()

# configure
#check_include_files(string.h HAVE_STRING_H)
#check_symbol_exists(LC_MESSAGES "locale.h" HAVE_LC_MESSAGES) 
#check_library_exists(arkosg  attachNode "" HAVE_ARKOSG)

# config files
configure_file(config.h.in config.h)
install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/${PROJECT_NAME} COMPONENT Dev)

# build settings
if (MINGW)
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_definitions(-DBOOST_THREAD_USE_LIB)
elseif(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "i386;ppc;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
endif()

if (WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pedantic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wmissing-prototypes -Wdeclaration-after-statement")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wformat-security")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wformat-security")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute")
endif()

# project wide flags
include_directories(src ${CMAKE_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS} ${MAVLINK_INCLUDE_DIRS})

# librarys
set(arkcommLibs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if (MINGW)
    list(APPEND arkcommLibs ws2_32)
endif()

set(arkcommSources
        src/AsyncSerial.cpp)

set(arkcommHeaders
        src/AsyncSerial.hpp)

if (WITH_FLIGHTGEAR)
    list(APPEND arkcommSources
        src/FlightGearComm.cpp
        src/flightGearUtils.cpp
        )
    list(APPEND arkcommHeaders
        src/FlightGearComm.hpp
        src/flightGearUtils.hpp
        src/net_ctrls.hxx
        src/net_fdm.hxx
        )
    list(APPEND arkcommLibs
        ${SIMGEAR_LIBRARIES} 
        )
endif()

# static library
add_library(arkcommStatic STATIC ${arkcommSources})
add_dependencies(arkcommStatic mavlink simgear)
set_target_properties(arkcommStatic PROPERTIES 
    OUTPUT_NAME arkcomm
    VERSION ${LIBRARY_VERSION} 
    )
target_link_libraries(arkcommStatic ${arkcommLibs})
install(TARGETS arkcommStatic
    ARCHIVE DESTINATION "lib" COMPONENT Dev
    )

# shared library
if (BUILD_SHARED)
    add_library(arkcommShared SHARED ${arkcommSources})
    add_dependencies(arkcommShared mavlink simgear)
    set_target_properties(arkcommShared PROPERTIES 
    OUTPUT_NAME arkcomm
    VERSION ${LIBRARY_VERSION} 
    SOVERSION ${LIBRARY_SOVERSION}
    FRAMEWORK ON
    PUBLIC_HEADERS ${ARKCOMM_PUBLIC_HDRS}
    PRIVATE_HEADERS ${ARKCOMM_PRIVATE_HDRS}
    )
    target_link_libraries(arkcommShared arkcommStatic ${arkcommLibs})
    install(TARGETS arkcommShared
        LIBRARY DESTINATION "lib" COMPONENT Runtime
        FRAMEWORK DESTINATION "/Library/Frameworks" COMPONENT Runtime
        # where to put files on non-mac computers, mac overrides and uses default
        PRIVATE_HEADER DESTINATION "include/jsbsim" COMPONENT Runtime
        PUBLIC_HEADER DESTINATION "include/jsbsim" COMPONENT Runtime
        RESOURCE DESTINATION "share/jsbsim" COMPONENT Runtime
        )
endif()

# flightgear multiplayer library
if (WITH_FLIGHTGEAR)
    #include_directories(thirdParty thirdParty/flightgear/MultiPlayer thirdParty/flightgear src)
    #add_library(fgMultiPlayer STATIC 
        #thirdParty/flightgear/MultiPlayer/mpmessages.hxx
        #thirdParty/flightgear/MultiPlayer/tiny_xdr.cxx
        #thirdParty/flightgear/MultiPlayer/tiny_xdr.hxx
        #src/multiplaymgr.hxx
        #src/multiplaymgr.cxx
        #src/multiplay.hxx
        #src/multiplay.cxx
        #)
    #add_dependencies(fgMultiPlayer simgear)
    #target_link_libraries(fgMultiPlayer ${SIMGEAR_LIBRARIES})
endif()

# test programs
if (WITH_BUILD_TESTS)
    if (MAVLINK_FOUND)
        add_executable(mavlinkHilState test/mavlinkHilState.cpp)
        target_link_libraries(mavlinkHilState arkcommStatic ${arkcommLIBS})
    endif()
    if (WITH_FLIGHTGEAR)
        add_executable(fgMultiPlayerTest test/fgMultiPlayer.cpp)
        target_link_libraries(fgMultiPlayerTest fgMultiPlayer ${SIMGEAR_LIBRARIES})
    endif()
endif()

# install headers
install(FILES ${arkcommHeaders} DESTINATION include/arkcomm COMPONENT Dev)

# packaging
if (WIN32)
    # set NSIS image
    set(CPACK_PACKAGE_ICON "\\\\/cmake/jsbsim-nsis.bmp")
endif(WIN32)
configure_file("COPYING" "COPYING.txt" COPYONLY) # add file extensions and set resource files
configure_file("README" "README.txt" COPYONLY)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_BINARY_DIR}/README.txt")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CPACK_RESOURCE_FILE_README}")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/cmake/WELCOME.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "arktools communication library")
set(CPACK_PACKAGE_VENDOR ${PROJECT_CONTACT_VENDOR})
set(CPACK_PACKAGE_CONTACT "${PROJECT_CONTACT_EMAIL}")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_SET_DESTDIR TRUE)
set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
include(CPack)

# pkgconfig
configure_file(pc.cmake arkcomm.pc)
install(FILES
    ${PROJECT_BINARY_DIR}/arkcomm.pc
    DESTINATION lib/pkgconfig COMPONENT Dev
    )
# vim:sw=4:ts=4:expandtab
