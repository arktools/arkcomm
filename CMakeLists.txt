project (arkcomm C CXX)

# settings
cmake_minimum_required (VERSION 2.8)
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "4")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_CONTACT_EMAIL james.goppert@gmail.com)
set(PROJECT_CONTACT_VENDOR arkTools)
set(LIBRARY_VERSION ${PROJECT_VERSION})
set(LIBRARY_SOVERSION "0.0.0")

# third party
set(MAVLINK_VERSION "3eb931ac38b94854aa47483739cb1e4492e9120a")

# options
option(WITH_BUILD_DEPS "Build dependencies." ON)
option(WITH_BUILD_STATIC "Prefer static linking." ON)
option(WITH_MAVLINK "Build with mavlink support." ON)
option(WITH_FLIGHTGEAR "Build with flightgear support." ON)

# variables
set(ROOT_THREAD TRUE CACHE INTERNAL "Is this the top level of the recursion?")
set(INSTALLED_TARGETS "")

# modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR}/cmake/arkcmake)
include(BundleUtilities)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckLibraryExists)
#include(CheckTypeSize)
#include(CheckPrototypeExists)
#include(CheckCXXSourceCompiles)
#include(CheckCSourceCompiles)
include(ExternalProject)
include(FindMAVLINK)

# spawn new cmake to build deps
if (WITH_BUILD_DEPS AND ROOT_THREAD)
    execute_process(COMMAND ${CMAKE_COMMAND} "${CMAKE_SOURCE_DIR}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DWITH_BUILD_DEPS=${WITH_BUILD_STATIC}"
        "-DWITH_BUILD_STATIC=${WITH_BUILD_STATIC}"
        "-DWITH_MAVLINK=${WITH_MAVLINK}"
        "-DWITH_FLIGHTGEAR=${WITH_FLIGHTGEAR}"
        "-DROOT_THREAD=FALSE"
        RESULT_VARIABLE ERROR)
    if (ERROR)
        message(FATAL_ERROR "error, recursing loop returned error code: ${ERROR}")
    endif()
    message("** Making dependencies")
    execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} "-j4" "-f${CMAKE_BINARY_DIR}/Makefile")
    message("** Configuring ${PROJECT_NAME}")
endif()

# external projects find path
if(NOT EP_BASE_DIR)
    set(EP_BASE_DIR "${CMAKE_BINARY_DIR}/CMakeExternals")
endif()
list(APPEND CMAKE_FIND_ROOT_PATH ${EP_BASE_DIR})

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
if (WIN32)
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread_win32-mt system-mt)
else()
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread-mt system-mt)
endif()
if (WITH_FLIGHTGEAR)
    find_package(SIMGEAR)
    find_package(PLIB)
endif()
find_package(Threads)
find_package(MAVLINK)

# build dependencies
if (WITH_BUILD_DEPS AND (NOT ROOT_THREAD) )
    message("** Configuring dependencies")

    # add external projects

    set(CMAKE_DEFAULT_ARGS
        -DEP_BASE_DIR=${EP_BASE_DIR}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        )

    if (WITH_MAVLINK AND (NOT MAVLINK_FOUND))
        build_mavlink(${MAVLINK_VERSION} ${EP_BASE_DIR} "${CMAKE_DEFAULT_ARGS}")
    endif()

    # terminate non root cmake thread
    return()
endif()

# configure
check_include_files(expat_config.h HAVE_EXPAT_CONFIG_H)
check_include_files(sys_types.h HAVE_SYS_TYPES_H)
check_include_files(sys_stat.h HAVE_SYS_STAT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_function_exists(memmove HAVE_MEMMOVE)
check_function_exists(bcopy HAVE_BCOPY)
#check_symbol_exists(LC_MESSAGES "locale.h" HAVE_LC_MESSAGES) 
#check_library_exists(arkosg  attachNode "" HAVE_ARKOSG)

# config files
configure_file(config.h.in config.h)


# project wide flags
include_directories( ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
include_directories(${Boost_INCLUDE_DIRS} ${MAVLINK_INCLUDE_DIRS})

# library
if (WIN32)
    set(arkcommLibs ${Boost_LIBRARIES} ws2_32)
else()
    set(arkcommLibs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

set(arkcommSources
        src/AsyncSerial.cpp)

set(arkcommHeaders
        src/AsyncSerial.hpp)

if (MAVLINK_FOUND)
    list(APPEND arkcommSources
        src/asio_mavlink_bridge.cpp
        )
    list(APPEND arkcommHeaders
        src/asio_mavlink_bridge.h
        )
endif()

if (SIMGEAR_FOUND AND PLIB_FOUND)
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
        ${PLIB_LIBRARY} ${SIMGEAR_LIBRARIES} 
        )
endif()

add_library(arkcommShared SHARED ${arkcommSources})
add_dependencies(arkcommShared mavlink)
set_target_properties(arkcommShared PROPERTIES 
    OUTPUT_NAME arkcomm
    VERSION ${LIBRARY_VERSION} 
    SOVERSION ${LIBRARY_SOVERSION}
    FRAMEWORK ON
    PUBLIC_HEADERS ${ARKCOMM_PUBLIC_HDRS}
    PRIVATE_HEADERS ${ARKCOMM_PRIVATE_HDRS}
    )
target_link_libraries(arkcommShared ${arkcommLibs})
list(APPEND INSTALLED_TARGETS arkcommShared)

add_library(arkcommStatic STATIC ${arkcommSources})
add_dependencies(arkcommStatic mavlink)
set_target_properties(arkcommStatic PROPERTIES 
    OUTPUT_NAME arkcomm
    VERSION ${LIBRARY_VERSION} 
    )
target_link_libraries(arkcommStatic ${arkcommLibs})
list(APPEND INSTALLED_TARGETS arkcommStatic)

# test programs
if (WITH_TESTS)
    if (MAVLINK_FOUND)
        add_definitions(-DBOOST_THREAD_USE_LIB)
        add_executable(mavlinkHilState test/mavlinkHilState.cpp)
        target_link_libraries(mavlinkHilState arkcomm ${arkcomm_LIBS})
    endif()
endif()

# install libraries
install(FILES ${arkcommHeaders} DESTINATION include/arkcomm COMPONENT Dev)

# install
install(TARGETS  ${INSTALLED_TARGETS}
    RUNTIME DESTINATION "bin" COMPONENT Runtime
    LIBRARY DESTINATION "lib" COMPONENT Runtime
    ARCHIVE DESTINATION "lib" COMPONENT Dev
    BUNDLE DESTINATION "/Applications" COMPONENT Runtime
    FRAMEWORK DESTINATION "/Library/Frameworks" COMPONENT Runtime
    # where to put files on non-mac computers, mac overrides and uses default
    PRIVATE_HEADER DESTINATION "include/jsbsim" COMPONENT Runtime
    PUBLIC_HEADER DESTINATION "include/jsbsim" COMPONENT Runtime
    RESOURCE DESTINATION "share/jsbsim" COMPONENT Runtime
    )

# install external projects, if built
if (EXISTS ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/)
    install(DIRECTORY ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime)
endif()

# packaging

# set NSIS image
if (WIN32)
    set(CPACK_PACKAGE_ICON "\\\\/cmake/jsbsim-nsis.bmp")
endif(WIN32)

# add file extensions and set resource files
configure_file("COPYING" "COPYING.txt" COPYONLY)
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
