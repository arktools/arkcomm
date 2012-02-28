# - Try to find  SIMGEAR
# Once done, this will define
#
#  SIMGEAR_FOUND - system has scicoslab 
#  SIMGEAR_INCLUDE_DIRS - the scicoslab include directories
#  SIMGEAR_LIBRARIES - libraries to link to

include(LibFindMacros)
include(MacroCommonPaths)

MacroCommonPaths(SIMGEAR)

# Include dir
find_path(SIMGEAR_INCLUDE_DIR
	NAMES simgear/version.h
	PATHS  ${COMMON_INCLUDE_PATHS_SIMGEAR}
)

# Finally the library itself
find_library(SIMGEAR_LIBRARY
	NAMES sgio
	PATHS  ${COMMON_LIBRARY_PATHS_SIMGEAR}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SIMGEAR_PROCESS_INCLUDES SIMGEAR_INCLUDE_DIR)
set(SIMGEAR_PROCESS_LIBS SIMGEAR_LIBRARY)
libfind_process(SIMGEAR)

macro(build_simgear TAG EP_BASE_DIR CMAKE_ARGS)
    ExternalProject_Add(simgear
        GIT_REPOSITORY "git clone git://gitorious.org/fg/simgear.git"
        GIT_TAG ${TAG}
        UPDATE_COMMAND ""
        INSTALL_DIR ${EP_BASE_DIR}/${CMAKE_INSTALL_PREFIX}
        CMAKE_ARGS ${CMAKE_ARGS}
        INSTALL_COMMAND make DESTDIR=${EP_BASE_DIR} install
    )
endmacro()
