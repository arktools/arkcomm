project (arkcomm C CXX)

# settings
cmake_minimum_required (VERSION 2.6)
set(APPLICATION_NAME "${PROJECT_NAME}")
set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "1")
set(APPLICATION_VERSION_PATCH "4")
set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")
set(LIBRARY_VERSION ${APPLICATION_VERSION})
set(MAVLINK_VERSION "3eb931ac38b94854aa47483739cb1e4492e9120a")
set(LIBRARY_SOVERSION "0.0.0")
set(PROJECT_CONTACT_EMAIL james.goppert@gmail.com)
set(PROJECT_CONTACT_VENDOR arkTools)

# modules
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/cmake
	${CMAKE_SOURCE_DIR}/cmake/arkcmake
)
include(ExternalProject)
include(DefineCMakeDefaults)
include(DefinePlatformDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(FindMAVLINK)

# settings
set(BUILDING_DEPS OFF CACHE INTERNAL "Whether are not we are building the dependencies.")
option(WITH_BUILD_STATIC "Prefer static linking." ON)
option(WITH_BUILD_DEPS_ALL "Build all external dependencies, even if found." OFF)
option(WITH_BUILD_DEPS "Build external dependencies." ON)

# built variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
if(NOT EP_BASE_DIR)
    set(EP_BASE_DIR "${CMAKE_BINARY_DIR}/CMakeExternals")
endif()
set(CMAKE_FIND_ROOT_PATH "${EP_BASE_DIR}")

# static lib options
if (WITH_BUILD_STATIC)
    set(BOOST_USE_STATIC_LIBS ON)
    # prefer static libs
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()
endif()

# add make dist target
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# build external dependencies automatically if make found
if ( NOT ("${CMAKE_MAKE_PROGRAM}" STREQUAL "") AND (NOT BUILDING_DEPS) AND (WITH_BUILD_DEPS) )
    execute_process(COMMAND ${CMAKE_COMMAND} ".." "-DBUILDING_DEPS=y")
    execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} "-f${CMAKE_BINARY_DIR}/Makefile")
endif()

# find packages
find_package(SIMGEAR)
find_package(PLIB)
find_package(Threads)
if (WIN32)
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread_win32-mt system-mt)
else()
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread-mt system-mt)
endif()
find_package(MAVLINK)

# build external libraries if required
if (BUILDING_DEPS)
    set_property(DIRECTORY PROPERTY EP_BASE ${EP_BASE_DIR})
    if (WITH_BUILD_DEPS_ALL OR (NOT MAVLINK_FOUND))
        set(CMAKE_ARGS_MAVLINK
            -DEP_BASE_DIR=${EP_BASE_DIR}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        )
        build_mavlink(${MAVLINK_VERSION} ${EP_BASE_DIR} "${CMAKE_ARGS_MAVLINK}")
    endif()
    # the return here terminates the cmake once all dependencies are built
    return()
endif()

# pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc)
install(FILES
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION lib/pkgconfig
    )

# project wide flags
include_directories( ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
include_directories(${Boost_INCLUDE_DIRS} ${MAVLINK_INCLUDE_DIRS})

# library
if (WIN32)
    set(${PROJECT_NAME}Libs ${Boost_LIBRARIES} ws2_32)
else()
    set(${PROJECT_NAME}Libs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

set(${PROJECT_NAME}Sources
        src/AsyncSerial.cpp)

set(${PROJECT_NAME}Headers
        src/AsyncSerial.hpp)

if (MAVLINK_FOUND)
    list(APPEND ${PROJECT_NAME}Sources
        src/asio_mavlink_bridge.cpp
        )
    list(APPEND ${PROJECT_NAME}Headers
        src/asio_mavlink_bridge.h
        )
endif()

if (SIMGEAR_FOUND AND PLIB_FOUND)
    list(APPEND ${PROJECT_NAME}Sources
        src/FlightGearComm.cpp
        src/flightGearUtils.cpp
        )
    list(APPEND ${PROJECT_NAME}Headers
        src/FlightGearComm.hpp
        src/flightGearUtils.hpp
        src/net_ctrls.hxx
        src/net_fdm.hxx
        )
    list(APPEND ${PROJECT_NAME}Libs
        ${PLIB_LIBRARY} ${SIMGEAR_LIBRARIES} 
        )
endif()

add_library(${PROJECT_NAME} ${${PROJECT_NAME}Sources})
add_dependencies(${PROJECT_NAME} mavlink)
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}Libs})
set_target_properties(${PROJECT_NAME} PROPERTIES 
    VERSION ${LIBRARY_VERSION} 
    SOVERSION ${LIBRARY_SOVERSION})

# test programs
if (BUILD_TESTING)
    if (MAVLINK_FOUND)
        add_definitions(-DBOOST_THREAD_USE_LIB)
        add_executable(mavlinkHilState test/mavlinkHilState.cpp)
        target_link_libraries(mavlinkHilState ${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
    endif()
endif()

# install libraries
install(TARGETS ${PROJECT_NAME} DESTINATION lib EXPORT ${PROJECT_NAME}-targets)
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}Headers} DESTINATION include/${PROJECT_NAME})

# install external projects, if built
if (EXISTS ${EP_BASE_DIR}/${EP_INSTALL_PREFIX}/)
    install(DIRECTORY ${EP_BASE_DIR}/${EP_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
