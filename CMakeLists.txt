project (arkcomm)

# settings
cmake_minimum_required (VERSION 2.6)
set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "1")
set(APPLICATION_VERSION_PATCH "4")
set(MAVLINK_VERSION "3eb931ac38b94854aa47483739cb1e4492e9120a")
set(LIBRARY_SOVERSION "0.0.0")
set(PROJECT_CONTACT_EMAIL james.goppert@gmail.com)
set(PROJECT_CONTACT_VENDOR arkTools)

# modules
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/cmake
	${CMAKE_SOURCE_DIR}/cmake/arkcmake
)

# use common setup
include(CommonSetup)
set(LIBRARY_VERSION ${APPLICATION_VERSION})

# modules
include(FindMAVLINK)

# cpack
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501) # target xp
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_SOURCE_GENERATOR "ZIP")
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\arkmath.exe")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}-${APPLICATION_VERSION}")
    set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\github.com/arktools/arkmath")
    set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\github.com/arktools/arkmath")
    set(CPACK_NSIS_CONTACT ${PROJECT_CONTACT_EMAIL})
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    #set(CPACK_STRIP_FILES "bin/ins")
    #set(CPACK_SOURCE_STRIP_FILES "")
    if(APPLE)
        set(CPACK_GENERATOR "PackageMaker")
        set(CPACK_SOURCE_GENERATOR "TGZ")
            elseif(UNIX)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_SOURCE_GENERATOR "ZIP")
    else()
        message(FATAL_ERROR "unknown operating system")
    endif()
endif()
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "arkTools math module")
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_CONTACT ${PROJECT_CONTACT_EMAIL})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${APPLICATION_VERSION_MAJOR}") 
set(CPACK_PACKAGE_VERSION_MINOR "${APPLICATION_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${APPLICATION_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}-${APPLICATION_VERSION}")
set(CPACK_SET_DESTDIR TRUE)
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES}
	/.git/;/build/;~$;.*\\\\.bin$;.*\\\\.swp$)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
#set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
include(CPack)

# enable languages
enable_language(C)
enable_language(CXX)

# find libraries with cmake modules
find_package(SIMGEAR)
find_package(PLIB)
find_package(Threads)
if (NOT BUILD_SHARED_LIBS)
    set(BOOST_USE_STATIC_LIBS ON)
endif()
if (WIN32)
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread_win32-mt system-mt)
else()
    find_package(Boost 1.40.0 REQUIRED COMPONENTS thread-mt system-mt)
endif()
find_package(MAVLINK)
if (NOT MAVLINK_FOUND)
    build_mavlink(${MAVLINK_VERSION} ${EP_BASE_DIR} ${EP_INSTALL_PREFIX} ${EP_DATADIR})
endif()

# install required dependencies from external projects
install(DIRECTORY ${EP_BASE_DIR}/${EP_INSTALL_PREFIX}/ DESTINATION ${CMAKE_INSTALL_PREFIX})

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

install(TARGETS ${PROJECT_NAME} DESTINATION lib EXPORT ${PROJECT_NAME}-targets)
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}Headers} DESTINATION include/${PROJECT_NAME})

if (BUILD_TESTING)
    if (MAVLINK_FOUND)
        add_definitions(-DBOOST_THREAD_USE_LIB)
        add_executable(mavlinkHilState test/mavlinkHilState.cpp)
        target_link_libraries(mavlinkHilState ${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
    endif()
endif ()
