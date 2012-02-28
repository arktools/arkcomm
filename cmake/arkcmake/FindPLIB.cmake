# - Try to find  PLIB
# Once done, this will define
#
#  PLIB_FOUND - system has scicoslab 
#  PLIB_INCLUDE_DIRS - the scicoslab include directories
#  PLIB_LIBRARIES - libraries to link to

include(LibFindMacros)
include(MacroCommonPaths)

MacroCommonPaths(PLIB)

# Include dir
find_path(PLIB_INCLUDE_DIR
	NAMES plib/net.h
	PATHS ${COMMON_INCLUDE_PATHS_PLIB}
)

# the library itself
find_library(PLIB_FNT_LIBRARY
	NAMES plibfnt
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_JS_LIBRARY
	NAMES plibjs
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_NET_LIBRARY
	NAMES plibnet
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_PSL_LIBRARY
	NAMES plibpsl
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_PU_LIBRARY
	NAMES plibpu
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_PUAUX_LIBRARY
	NAMES plibpuaux
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_PW_LIBRARY
	NAMES plibpw
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_SG_LIBRARY
	NAMES plibsg
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_SL_LIBRARY
	NAMES plibsl
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_SM_LIBRARY
	NAMES plibsm
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_SSG_LIBRARY
	NAMES plibssg
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_SSGAUX_LIBRARY
	NAMES plibssgaux
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)
find_library(PLIB_UL_LIBRARY
	NAMES plibul
	PATHS ${COMMON_LIBRARY_PATHS_PLIB}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(PLIB_PROCESS_INCLUDES PLIB_INCLUDE_DIR)
set(PLIB_PROCESS_LIBS
    PLIB_FNT_LIBRARY
    PLIB_JS_LIBRARY
    PLIB_NET_LIBRARY
    PLIB_PSL_LIBRARY
    PLIB_PU_LIBRARY
    PLIB_PUAUX_LIBRARY
    PLIB_SG_LIBRARY
    PLIB_SL_LIBRARY
    PLIB_SM_LIBRARY
    PLIB_SSG_LIBRARY
    PLIB_SSGAUX_LIBRARY
    PLIB_UL_LIBRARY
    )
libfind_process(PLIB)
