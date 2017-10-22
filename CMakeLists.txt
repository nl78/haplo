CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message (FATAL_ERROR
    "In-source builds are not allowed. Please create a directory and run cmake "
    "from there, passing the path to this source directory as the last argument:"
    "
    "
    "cmake path_to_arts"
    "
    "
    "IMPORTANT: This process created the file 'CMakeCache.txt' and the directory "
    "'CMakeFiles'.  Please delete them."
    "
    "
    "If you get this error message although you are in a separate build directory, "
    "please check that there is no file 'CMakeCache.txt' and no directory "
    "'CMakeFiles' in the arts source directory (not the directory you're "
    "currently in).  Please delete them."
    )
ENDIF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

PROJECT (haplo)

SET (HAPLO_MAJOR 0)
SET (HAPLO_MINOR 9)
EXECUTE_PROCESS(COMMAND svnversion
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE HAPLO_SCM
	OUTPUT_STRIP_TRAILING_WHITESPACE)

######################
# CHECK FOR PROGRAMS #
######################

INCLUDE (FindPerl)


#################################
# CHECK AVAILABILITY OF HEADERS #
#################################

ADD_DEFINITIONS(-DHAPLO)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

INCLUDE (CheckIncludeFile)
INCLUDE (CheckIncludeFile)

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(direct.h HAVE_DIRECT_H)
CHECK_INCLUDE_FILE(io.h HAVE_IO_H)
CHECK_INCLUDE_FILE(dl.h HAVE_DL_H)
CHECK_INCLUDE_FILE(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILE(Windows.h HAVE_WINDOWS_H)
CHECK_INCLUDE_FILE(libexc.h HAVE_LIBEXC_H)
CHECK_INCLUDE_FILE(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE(sys/resource.h HAVE_SYS_RESOURCE_H)


#######################
# CHECK FOR FUNCTIONS #
#######################

INCLUDE (CheckFunctionExists)

CHECK_FUNCTION_EXISTS (log HAVE_LOG)



#######################
# CHECK FOR LIBRARIES #
#######################

FIND_PACKAGE (ZLIB)
FIND_PACKAGE (THREADS)


####################################
# CHECK FOR MODULES IMPLEMENTATION #
####################################

SET (HAPLO_MODULE_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
SET (HAPLO_MODULE_PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}")
SET (HAPLO_MODULE_IMPL HAPLO_MODULE_IMPL_NONE) 
SET (HAPLO_MODULE_LIB "")
           
IF (UNIX)
        CHECK_FUNCTION_EXISTS(dlerror HAVE_DLERROR)
   	CHECK_FUNCTION_EXISTS(dlopen HAVE_DLOPEN)
   	IF (HAVE_DLOPEN)
      	        SET (HAPLO_MODULE_IMPL HAPLO_MODULE_IMPL_DL)            
   	ELSE () 
   	     	FIND_LIBRARY(LIBDL dl) 
  	     	IF (NOT LIBDL STREQUAL "LIBDL-NOTFOUND")
	                SET (HAPLO_MODULE_LIB "dl")
	   		SET (HAPLO_MODULE_IMPL HAPLO_MODULE_IMPL_DL)            
	 	ENDIF ()
   	ENDIF ()

	FIND_LIBRARY(LIBDLD dld) 
	IF (NOT LIBDLD STREQUAL "LIBDLD-NOTFOUND")
	        SET (HAPLO_MODULE_LIB "dld")
     		SET (HAPLO_MODULE_IMPL HAPLO_MODULE_IMPL_DLD)            
 	ENDIF ()

ELSE (UNIX)
	     SET (HAPLO_MODULE_IMPL HAPLO_MODULE_IMPL_WIN32)
ENDIF (UNIX)


if (CMAKE_GENERATOR STREQUAL "Xcode")
  set (COMPILER "Xcode")
  set (CMAKE_OSX_SYSROOT "")
  set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build CACHE PATH
       "Single output directory for building all libraries.")
  set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build CACHE PATH
       "Single output directory for building all executables.")
  mark_as_advanced (LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
endif ()

# OPTIONS #
OPTION (DEBUG_MEMORY
       "Blabla" ON
)



####################
# CONFIGURE PACKAGE #
####################

INCLUDE_DIRECTORIES ("${PROJECT_BINARY_DIR}/kernel/include") 
INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/kernel/include") 


GET_PROPERTY (
	HAPLO_OPTIONS
	DIRECTORY "."
	PROPERTY COMPILE_DEFINITIONS
)

IF (DEBUG_MEMORY)
   ADD_DEFINITIONS(-DHAPLO_DEBUG_MEMORY)
ENDIF ()


ADD_SUBDIRECTORY (kernel)
ADD_SUBDIRECTORY (modules)

CONFIGURE_FILE (
	"${PROJECT_SOURCE_DIR}/kernel/include/haplo/config.h.in"
	"${PROJECT_BINARY_DIR}/kernel/include/haplo/config.h"
)







######################
# BUILDING AN INSTALLER #
######################

INCLUDE (InstallRequiredSystemLibraries)


SET(CPACK_PACKAGE_FILE_NAME "haplo-${HAPLO_MAJOR}.${HAPLO_MINOR}") 
#SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/doc/img/haplo.png")
SET(CPACK_BUNDLE_NAME "haplo")
SET(CPACK_BUNDLE_ICON 
		       "${CMAKE_CURRENT_SOURCE_DIR}/doc/img/haplo.png" )
SET(CPACK_BUNDLE_PLIST "${PROJECT_SOURCE_DIR}/doc/haplo.plist")
SET(CPACK_BUNDLE_STARTUP_COMMAND  "${PROJECT_SOURCE_DIR}/doc/haplo.sh")

SET (CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.txt")
SET (CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/doc/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_LICENSE"${PROJECT_SOURCE_DIR}/doc/LICENSE.txt") 

SET (CPACK_PACKAGE_VERSION_MAJOR "${HAPLO_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR "${HAPLO_MINOR}")

INCLUDE (CPack)
